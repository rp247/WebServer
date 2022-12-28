#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <fcntl.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <signal.h>

#include "bind.h"

enum Returns {SUCC = 0, ERR, GET, HEAD, PUT, NOT_IMP, BUF = 2048};
enum Rand {ID = 10, ARB = 80};
int logg;

// struct for easily passing info
struct Info {
    char *method;
    char *uri;
    char request_id[ID];
    size_t content_length;
    char *ret_code;
    int client;
    int log_fd;
};
typedef struct Info Info;

// process head
void proc_head(Info *infStruct) {
    int fd = open(infStruct->uri, O_RDONLY);
    // all good, send 200 OK for HEAD
    if (fd != -1) {
        struct stat s; stat(infStruct->uri, &s);
        int size = s.st_size;
        char tBuf[ARB]; memset(tBuf, 0, ARB);
        int ret = sprintf(tBuf, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\nOK\n", size);
        send(infStruct->client, tBuf, ret+1, 0);
        close(fd);
        infStruct->ret_code = "200";
    } 
    else {
        char *resp = "HTTP/1.1 404 Not Found\r\nContent-Length: 10\r\n\r\nNot Found\n";
        send(infStruct->client, resp, strlen(resp), 0);
        infStruct->ret_code = "404";
    }
    // write log
    char tBuf[ARB];
    int ret = sprintf(tBuf, "%s,/%s,%s,%s\n", infStruct->method, infStruct->uri, infStruct->ret_code, infStruct->request_id);
    write(infStruct->log_fd, tBuf, ret);
    return;
}

// process get
void proc_get(Info *infStruct) {
    int fd = open(infStruct->uri, O_RDONLY);
    // all good, send 200 OK for GET
    if (fd != -1) {
        struct stat s; stat(infStruct->uri, &s);
        int size = s.st_size;
        char tBuf[ARB]; memset(tBuf, 0, ARB);
        int ret = sprintf(tBuf, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n", size);
        send(infStruct->client, tBuf, ret, 0);

        // write the file to the socket
        uint8_t read_buf[BUF];
        size_t tot_read, write_ret, tot_written;

        while ((tot_read = read(fd, read_buf, BUF)) > 0) {
            tot_written = 0;
            // write all the read bytes
            while (tot_written != tot_read) {
                write_ret = write(infStruct->client, read_buf+tot_written, tot_read);
                tot_written += write_ret;
            };
        }
        
        close(fd);
        infStruct->ret_code = "200";
    } 
    
    else {
        char *resp = "HTTP/1.1 404 Not Found\r\nContent-Length: 10\r\n\r\nNot Found\n";
        send(infStruct->client, resp, strlen(resp), 0);
        infStruct->ret_code = "404";
    }

    // write log
    char tBuf[ARB];
    int ret = sprintf(tBuf, "%s,/%s,%s,%s\n", infStruct->method, infStruct->uri, infStruct->ret_code, infStruct->request_id);
    write(infStruct->log_fd, tBuf, ret);
    return;
}

// process put
void proc_put(Info *infStruct, char *body, int remaining) {
    char *resp;
    int fd = open(infStruct->uri, O_RDWR | O_TRUNC);
    if (fd == -1) {
        fd = open(infStruct->uri, O_CREAT | O_RDWR | O_TRUNC);
        resp = "HTTP/1.1 201 Created\r\nContent-Length: 8\r\n\r\nCreated\n";
        infStruct->ret_code = "201";
    }
    else {
        resp = "HTTP/1.1 200 OK\r\nContent-Length: 3\r\n\r\nOK\n";
        infStruct->ret_code = "200";
    }

    if (infStruct->content_length == 0) write(fd, "", 1);
    else {
        // write the remaining body in buffer, would hang if content-length > msg_remaining
        int tot_written = 0;
        int write_ret;
        if(infStruct->content_length < remaining) remaining = infStruct->content_length;
        while (tot_written != remaining) {
            write_ret = write(fd, body+tot_written, remaining);
            tot_written += write_ret;
        };

        // more message left, get and write to file
        char rec_buf[BUF]; memset(rec_buf, '\0', BUF);
        int got, wrote;
        int rmd = infStruct->content_length - tot_written;
        while (tot_written < rmd) {
            got = recv(infStruct->client, rec_buf, BUF, 0);               // get remaining bytes
            wrote = 0;
            while (wrote != got) {
                write_ret = write(fd, rec_buf+wrote, got);
                wrote += write_ret;
            }
            tot_written += wrote;
        }
    }

    send(infStruct->client, resp, strlen(resp), 0);

    // write log
    char tBuf[ARB];
    int ret = sprintf(tBuf, "%s,/%s,%s,%s\n", infStruct->method, infStruct->uri, infStruct->ret_code, infStruct->request_id);
    write(infStruct->log_fd, tBuf, ret);
    return;
}

// parses method in the req line. returns (no method (ERR), GET, PUT, HEAD, not implemented)
int int_method(char *method) {
    if (strcmp(method, "GET") == 0) return GET;
    else if (strcmp(method, "HEAD") == 0) return HEAD;
    else return PUT;
}

// parses header fields (to get req id and content length)
void parse_hfield(char *headers, Info *infStruct) {
    char *saveptr;
    char *keyv = strtok_r(headers, "\r\n", &saveptr);
    while (keyv != NULL) {
        int i = 0;
        while (keyv[i] != ':') i++;
        char key[i+1]; memset(key, '\0', i+1); strncpy(key, keyv, i);
        if (strcmp(key, "Content-Length") == 0) 
            infStruct->content_length = (size_t) strtol(keyv+i+2, NULL, 10);
        if (strcmp(key, "Request-Id") == 0) 
            strncpy(infStruct->request_id, keyv+i+2, 10);
        // if (strcmp(keyv, "Expect: 100-continue") == 0) 
        //     send(infStruct->client, "HTTP/1.1 100 Continue\r\nContent-Length: 24\r\n\r\n", 46, 0);
        keyv = strtok_r(NULL, "\r\n", &saveptr);
    }
    return;
}

// parses headers
void parse_headers(char *headers, Info *infStruct, int remaining) {
    strcpy(infStruct->request_id, "0");
    infStruct->content_length = 0;
    int i = 0;
    if (!(headers[0] == '\r' && headers[1] == '\n')) {
        while (!(headers[i] == '\r' && headers[i+1] == '\n' && 
                headers[i+2] == '\r' && headers[i+3] == '\n')) i++;
        char wholeHead[i+2];
        strncpy(wholeHead, headers, i+2);
        parse_hfield(wholeHead, infStruct);
    }

    switch (int_method(infStruct->method)) {
        case HEAD:
            proc_head(infStruct);
            break;
        case GET:
            proc_get(infStruct);
            break;
        default:
            proc_put(infStruct, headers+i+4, remaining-i-4);
    }

    return;
}

void new_connection(int client, int log_fd) {
    Info infStruct;
    unsigned char rec_buf[BUF];
    memset(rec_buf, '\0', BUF);

    // get the request line + header field(s) + partial message body (if prev two < 2048)
    int received = recv(client, rec_buf, BUF, 0);

    char *saveptr, *reqsave;
    char *req_line = strtok_r(rec_buf, "\r\n", &saveptr);   // request line
    int remaining = received-strlen(req_line)-2;            // -2 for \r\n
    char *method = strtok_r(req_line, " ", &reqsave);       // method
    char *uri = strtok_r(NULL, " ", &reqsave);              // URI, check if valid or assume valid?
    infStruct.method = method; infStruct.uri = uri+1;       // strcpy for multi??
    infStruct.log_fd = log_fd; infStruct.client = client;

    parse_headers(saveptr+1, &infStruct, remaining); 
    return;
}

void signal_handler(int num) {
    fsync(logg); close(logg);
    exit(EXIT_FAILURE);
}

int main (int argc, char **argv) {

    char *usage = "Usage: ./httpserver [-t threads] [-l logfile] <port>\n";

    int c;
    char *optlist = "t:l:", *end = 0, *log_file = 0;
    uint16_t threads = 4, port;

    // parse args
    while ((c = getopt(argc, argv, optlist)) != -1) {
        switch (c) {
            case 't':
                threads = (uint16_t) strtoull(optarg, &end, 10);
                if ((end && *end != 0) || threads == 0) {
                    warnx("Invalid thread number: %s", optarg);
                    return ERR;
                }
                break;
            case 'l':
                log_file = optarg;
                break;
            default:
                fprintf(stderr, "%s", usage);
                return ERR;
        }
    }

    // port number missing
    if (optind == argc) {
        warnx("Missing port number");
        fprintf(stderr, "%s", usage);
        return ERR;
    }

    // port number prse
    port = (uint16_t) strtoull(argv[optind], &end, 10);
    if (end && *end != 0) {
        warnx("Invalid port number: %s", argv[optind]);
        fprintf(stderr, "%s", usage);
        return ERR;
    }

    // stderr or log file
    int log_fd = (log_file) ? open(log_file, O_RDWR | O_APPEND | O_CREAT) : STDERR_FILENO; // close on syscall SIGTERM
    logg = log_fd;

    // bind & get socket number
    int socket = create_listen_socket(port);
    if (socket < 0) {
        if (socket == -1) warnx("./httpserver: Bind: Invalid socket number.");
        else warnx("Socket bind error: %s.", strerror(errno));
        return ERR;
    }

    // signal handler
    signal(SIGTERM, signal_handler);

    // accept the connection, process requests
    int client;
    
    while (1) {
        // accept request
        client = accept(socket, NULL, NULL);
        if (client == -1) {
            warnx("Socket accept error: %s.", strerror(errno));
            continue;
        }
        new_connection(client, log_fd);
        close(client);  
    }

    return SUCC;
}
