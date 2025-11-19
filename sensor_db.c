#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "config.h"
#include "sensor_db.h"
#include "logger.h"

static int logger_started = 0;

FILE * open_db(char * filename, bool append)
{
    if (!logger_started) {
        create_log_process();
        logger_started = 1;
    }

    FILE *f = fopen(filename, append ? "a" : "w");

    if (!f) {
        write_to_log_process("open_db: ERROR opening database file");
        perror("open_db");
        return NULL;
    }

    char msg[256];
    snprintf(msg, sizeof(msg), "open_db: opened %s (%s)", filename, append ? "append" : "write");
    write_to_log_process(msg);

    return f;
}

int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts)
{
    if (!f) {
        write_to_log_process("insert_sensor: ERROR file is NULL");
        return -1;
    }

    int n = fprintf(f, "%d, %.6f, %ld\n", id, value, ts);
    fflush(f);

    if (n < 0) {
        write_to_log_process("insert_sensor: ERROR writing to file");
        return -1;
    }

    write_to_log_process("insert_sensor: data insertion succeeded");
    return 0;
}

int close_db(FILE * f)
{
    if (!f) {
        write_to_log_process("close_db: ERROR file is NULL");
        return -1;
    }

    write_to_log_process("close_db: closing file");

    int result = fclose(f);

    // stop logger when DB closes
    if (logger_started) {
        end_log_process();
        logger_started = 0;
    }

    return result;
}
