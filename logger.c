#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <errno.h>
#include "logger.h"

#define LOGFILE "example.gateway.log"
#define QUIT_MSG "__LOGGER_QUIT__\n"

static int pipefd[2] = {-1, -1};
static pid_t logger_pid = -1;

/* --------------------------------------------------------
 * Child process: the actual logger
 * -------------------------------------------------------- */
static void logger_loop()
{
    close(pipefd[1]);  // child only reads
    FILE *in = fdopen(pipefd[0], "r");
    if (!in) exit(1);

    FILE *logf = fopen(LOGFILE, "a");
    if (!logf) {
        perror("logger: fopen log file");
        exit(1);
    }


    unsigned long seq = 1;
    char buf[256];

    while (fgets(buf, sizeof(buf), in)) {

        if (strcmp(buf, QUIT_MSG) == 0)
            break;

        // strip newline
        size_t n = strlen(buf);
        if (n > 0 && buf[n-1] == '\n')
            buf[n-1] = '\0';

        // timestamp
        time_t t = time(NULL);
        struct tm tm;
        localtime_r(&t, &tm);
        char ts[64];
        strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", &tm);

        fprintf(logf, "%lu - %s - %s\n", seq++, ts, buf);
        fflush(logf);
    }

    fclose(logf);
    fclose(in);
    exit(0);
}

/* --------------------------------------------------------
 * API FUNCTIONS EXPECTED BY logger.h
 * -------------------------------------------------------- */

int create_log_process()
{
    printf("DEBUG: create_log_process called\n");
    fflush(stdout);

    if (pipe(pipefd) < 0) {
        perror("pipe");
        return -1;
    }

    logger_pid = fork();
    if (logger_pid < 0) {
        perror("fork");
        return -1;
    }

    if (logger_pid == 0) {
        // child
        logger_loop();
        exit(0); // never reached
    }

    // parent
    close(pipefd[0]);  // parent only writes
    return 0;
}

int write_to_log_process(char *msg)
{
    if (pipefd[1] < 0) return -1;

    dprintf(pipefd[1], "%s\n", msg);
    return 0;
}

int end_log_process()
{
    if (pipefd[1] >= 0) {
        // tell logger to quit
        dprintf(pipefd[1], QUIT_MSG);
        close(pipefd[1]);
        pipefd[1] = -1;
    }

    if (logger_pid > 0) {
        waitpid(logger_pid, NULL, 0);
        logger_pid = -1;
    }

    return 0;
}
