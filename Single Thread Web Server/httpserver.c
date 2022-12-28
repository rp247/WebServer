#include "bind.h"

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <regex.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#define ERR -1
#define SUCC 0
#define BUF 2048        // small bufs troubling
#define GET 2
#define HEAD 3
#define PUT 4
#define BAD_REQ 400
#define NOT_IMP 501

// usage message
void usage(char *arg) {
    fprintf(stderr, "usage: %s <port>\n", arg);
    return;
}

// opening file for processing, ERR if dir or forbidenn, or DNE, fd otherwise
int open_file(int client, char *uri, int method) {
    int fd;
    if (method == PUT) {
        fd = open(uri, O_RDWR | O_TRUNC);
    }
    else {
        fd = open(uri, O_RDONLY);
    }

    // check if dir
    struct stat dir;
    int sret = stat(uri, &dir);
    if (sret == -1) errno = 2;     // file DNE
    else if (!S_ISREG(dir.st_mode) && fd != -1) {
        if (fd != -1) close(fd);
        fd = -1;
        errno = EACCES;
    }

    // method PUT, file not a dir
    if (method == PUT && errno != EACCES) {
        // file DNE, create it
        if (fd == -1) {
            fd = open(uri, O_CREAT | O_RDWR | O_TRUNC);
            // forbidden
            if (fd == -1) errno = EACCES;
            else {
                char *resp = "HTTP/1.1 201 Created\r\nContent-Length: 8\r\n\r\nCreated\n";
                send(client, resp, strlen(resp), 0);
                return fd;
            }
        }

        // file exists
        else {
            char *resp = "HTTP/1.1 200 OK\r\nContent-Length: 3\r\n\r\nOK\n";
            send(client, resp, strlen(resp), 0);
            return fd;
        }
        
    }

    // could not open file
    if (fd == -1) {
        if (errno == EACCES) {
            char *resp = "HTTP/1.1 403 Forbidden\r\nContent-Length: 10\r\n\r\nForbidden\n";
            send(client, resp, strlen(resp), 0);
        }
        else if (errno == 2) {
            char *resp = "HTTP/1.1 404 Not Found\r\nContent-Length: 10\r\n\r\nNot Found\n";
            send(client, resp, strlen(resp), 0);
        }
        return ERR;
    }

    return fd;
}

// process head request
void proc_get(int client, char *uri) {
    int fd = open_file(client, uri, GET);
    if (fd == ERR) return;

    // all good, send 200 OK for HEAD
    struct stat s;
    stat(uri, &s);
    int size = s.st_size;
    char tBuf[80];
    memset(tBuf, 0, 80);
    int ret = sprintf(tBuf, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n", size);
    send(client, tBuf, ret, 0);

    // write the file to the socket
    uint8_t read_buf[BUF];
    size_t tot_read, write_ret, tot_written;

    while ((tot_read = read(fd, read_buf, BUF)) > 0) {
        tot_written = 0;
        // write all the read bytes
        while (tot_written != tot_read) {
            write_ret = write(client, read_buf+tot_written, tot_read);
            tot_written += write_ret;
        };
    }

    close(fd);
    return;
}

// process head request
void proc_head(int client, char *uri) {

    int fd = open_file(client, uri, HEAD);
    if (fd == ERR) return;

    // all good, send 200 OK for HEAD
    struct stat s;
    stat(uri, &s);
    int size = s.st_size;
    char tBuf[80];
    int ret = sprintf(tBuf, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\nOK\n", size);
    send(client, tBuf, ret+1, 0);
    close(fd);
    return;
}

// process put request
void proc_put(int client, char *uri, char *body, int content_length, int msg_remain) {
    int fd = open_file(client, uri, PUT);
    if (fd == ERR) return;

    if (content_length == 0) {
        write(fd, "", 1);
        return;
    }

    // write the remaining body in buffer, would hang if content-length > msg_remaining
    // (no signal handling for server so cannot flush out bytes and close connection)
    int tot_written = 0;
    int write_ret;
    if(content_length < msg_remain) msg_remain = content_length;
    while (tot_written != msg_remain) {
        write_ret = write(fd, body+tot_written, msg_remain);
        tot_written += write_ret;
    };

    // more message left, get and write to file
    char rec_buf[BUF];
    memset(rec_buf, '\0', BUF);             // zero out data
    int got, wrote;
    int remaining = content_length - tot_written;
    while (tot_written < remaining) {
        got = recv(client, rec_buf, BUF, 0);               // get remaining bytes
        wrote = 0;
        while (wrote != got) {
            write_ret = write(fd, rec_buf+wrote, got);
            wrote += write_ret;
        }
        tot_written += wrote;
    }

    return;
}

// checks if a string is in ascii and has no space (for valid key)
int is_ascii(char *str, int size) {
    char temp;
    for (int i = 0; i < size; i++) {
        temp = str[i];
        if (temp == ' ' || isascii(temp) == 0) return ERR;
    }
    return SUCC;
}

// returns ERR if invalid header field(s), else returns content-length (if method is PUT)
int parse_hfield (char *header, int method) {
    int con_length = 0;
    int con_prez = ERR;

    // check request line
    char *saveptr, *colon;
    char *line = strtok_r(header, "\r\n", &saveptr);
    while (line != NULL) {
        char *length = NULL;
        // no colon
        colon = strchr(line, ':');
        if (colon == NULL || *(colon+1)!=' ') return ERR;

        // check key
        int key_size = colon - line;
        char key[key_size];
        strncpy(key, line, key_size);   
        key[key_size] = '\0';

        // check ascii and no space
        if (is_ascii(key, key_size) == ERR) return ERR;

        // check for content length (valid value)
        if (strcmp(key, "Content-Length") == 0) {
            con_length = (int) strtol(colon+2, &length, 10);
            if (length[0] != '\0') {
                return ERR;
            }
            con_prez = SUCC;
        }

        // next line
        line = strtok_r(NULL, "\r\n", &saveptr);
    }

    return (method == PUT && con_prez == ERR) ? ERR : con_length;
}

// parses the header field(s)
void parse_header(int client, int method, char *uri, char *remaining, int body_size) {

    // no header fields
    if (remaining[0] == '\r' && remaining[1] == '\n') {
        // all valid PUT requests must have a header field and a message body
        if (method == PUT) {
            char *resp = "HTTP/1.1 400 Bad Request\r\nContent-Length: 12\r\n\r\nBad Request\n";
            send(client, resp, strlen(resp), 0);
        }
        else {
            if (method == GET) proc_get(client, uri);
            else proc_head(client, uri);
        }
        return;
    }

    // remember returns the first occurence of \r\n\r\n
    char *body = strstr(remaining, "\r\n\r\n");

    // invalid, header not terminated by \r\n\r\n
    if (body == NULL) {
        char *resp = "HTTP/1.1 400 Bad Request\r\nContent-Length: 12\r\n\r\nBad Request\n";
        send(client, resp, strlen(resp), 0);
        return;
    }

    int head_size = body-remaining;
    
    body_size -= head_size+6;   //5 for \r\n\r\n\0 and one for \0

    char header[head_size];                     // place to save header
    memset(header, '\0', head_size);            // zero out data 
    strncpy(header, remaining, head_size);   
    header[head_size] = '\0';

    int hfield_ret = parse_hfield(header, method);
    if (hfield_ret == ERR || hfield_ret < 0) {
        char *resp = "HTTP/1.1 400 Bad Request\r\nContent-Length: 12\r\n\r\nBad Request\n";
        send(client, resp, strlen(resp), 0);
        return;
    }

    switch (method) {
        case GET:
            proc_get(client, uri);
            break;
        case HEAD:
            proc_head(client, uri);
            break;
        default:
            proc_put(client, uri, body+4, hfield_ret, body_size);
            break;
    }

    return;
}

// parses and checks if valid uri
int parse_uri(char *uri) {
    regex_t re;
    int val = regcomp(&re, "^[/][a-zA-Z0-9._]*$", 0);
    val = regexec(&re, uri, 0, NULL, 0);
    regfree(&re);
    return (val == 0) ? SUCC : ERR;
}

// parses method in the req line. returns (no method (ERR), GET, PUT, HEAD, not implemented)
int parse_method(char *method) {
    // what method
    if (method == NULL) return ERR;
    else if (strcmp(method, "GET") == 0) return GET;
    else if (strcmp(method, "HEAD") == 0) return HEAD;
    else if (strcmp(method, "PUT") == 0) return PUT;
    else return NOT_IMP;
}

// parse request sent by client
void parse_reqline(int client) {
    char rec_buf[BUF];                      // buffer for receive requests
    memset(rec_buf, '\0', BUF);             // zero out old data 

    // get the request line + header field(s) + partial message body (if prev two < 2048)
    int received = recv(client, rec_buf, BUF, 0);

    // check request line
    char *saveptr;
    char *line = strtok_r(rec_buf, "\r\n", &saveptr);

    // request line not found
    if (line == NULL) {
        char *resp = "HTTP/1.1 400 Bad Request\r\nContent-Length: 12\r\n\r\nBad Request\n";
        send(client, resp, strlen(resp), 0);
        return;
    }

    int body_size = received - strlen(line);

    // check if valid request line: Method URI Version
    char *reqsave;
    char *method = strtok_r(line, " ", &reqsave);
    int method_ret = parse_method(method);
    char *resp;
    
    switch (method_ret) {
        case ERR:
            resp = "HTTP/1.1 400 Bad Request\r\nContent-Length: 12\r\n\r\nBad Request\n";
            send(client, resp, strlen(resp), 0);
            return;
        case NOT_IMP:
            resp = "HTTP/1.1 501 Not Implemented\r\nContent-Length: 16\r\n\r\nNot Implemented\n";
            send(client, resp, strlen(resp), 0);
            return;
        case GET:
            break;
        case HEAD:
            break;
        case PUT:
            break;
        default:
            resp = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 22\r\n\r\nInternal Server Error\n";
            send(client, resp, strlen(resp), 0);
            return;
    }

    // check if valid uri and check version
    char *uri = strtok_r(NULL, " ", &reqsave);
    int uri_ret = parse_uri(uri);
    if (uri_ret == ERR || (strcmp(reqsave, "HTTP/1.1") != 0)) {
        resp = "HTTP/1.1 400 Bad Request\r\nContent-Length: 12\r\n\r\nBad Request\n";
        send(client, resp, strlen(resp), 0);
        return;
    }
   
    // now parse the header field(s)
    parse_header(client, method_ret, uri+1, saveptr+1, body_size);
    return;
}

// got connection
void new_connection(int client) {
    parse_reqline(client);
    return;
}

// server main
int main(int argc, char **argv) {

    if (argc < 2) {
        warnx("wrong arguments: %s port_num", argv[0]);
        usage(argv[0]);
        return ERR;
    }

    // get port number
    char *end = 0;
    uint16_t port = (uint16_t) strtoull(argv[1], &end, 10);
    if (end && *end != 0) {
        warnx("invalid port number: %s", argv[1]);
        return ERR;
    }

    // bind & get socket number
    int socket = create_listen_socket(port);
    if (socket < 0) {
        if (socket == -1) warnx("%s: Bind: Invalid socket number.", argv[0]);
        else warnx("Socket bind error: %s.", strerror(errno));
        return ERR;
    }

    // accept the connection, process requests
    int client;               // client socket id
    
    while (1) {
        // accept request
        client = accept(socket, NULL, NULL);
        if (client == -1) {
            warnx("Socket accept error: %s.", strerror(errno));
            continue;
        }
        new_connection(client);
        close(client);  
    }

    return SUCC;    // try to reach me
}
