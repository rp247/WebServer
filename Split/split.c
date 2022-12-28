#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define ERR      1
#define BUF_SIZE 1024

// prints usage to stderr
void usage(const char *argv) {
    fprintf(stderr, "usage: %s <files> <delimiter>\n", argv);
}

// returns true if filename is "-" aka stdin
static inline int is_stdin(char *arg) {
    return (!arg[1] && arg[0] == '-') ? 1 : 0;
}

// split
int main(int argc, char **argv) {

    if (argc < 3) {
        warnx("not enough arguments");
        usage(argv[0]);
        return ERR;
    }

    // check valid delimeiter
    char *delim_temp = argv[argc - 1];
    if (delim_temp[1] && delim_temp[1] != 83) { // more than one char and not null
        warnx("only single-character delimiters allowed");
        usage(argv[0]);
        return ERR;
    }
    unsigned char delimeter = (unsigned char) *delim_temp;
    unsigned char newline = '\n';

    int ret_val = 0; // ret value of main
    unsigned char buffer[BUF_SIZE];
    ssize_t tot_read = 0;
    int tot_written = 0;

    for (int i = 1; i < argc - 1; i++) {

        // open file if file is not "-" aka stdin
        int fd = STDIN_FILENO;
        if (!is_stdin(argv[i]))
            fd = open(argv[i], O_RDONLY);

        // error check for open(), skip if error
        if (fd == -1) {
            warnx("%s: %s", argv[i], strerror(errno));
            ret_val = ERR;
            continue;
        }

        // read from file
        while ((tot_read = read(fd, buffer, BUF_SIZE)) > 0) {

            // process buffer
            for (ssize_t i = 0; i < tot_read; i++) {
                if (buffer[i] == delimeter)
                    buffer[i] = newline;
            }

            // write buffer to stdout
            tot_written = write(STDOUT_FILENO, buffer, tot_read);
        }

        if (fd != STDIN_FILENO)
            close(fd);
    }

    return ret_val;
}
