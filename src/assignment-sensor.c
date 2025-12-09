// src/assignment-sensor.c
#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static volatile sig_atomic_t g_stop = 0;

static void handle_sigterm(int sig) {
    (void)sig;
    g_stop = 1;
}

static void usage(const char *prog) {
    fprintf(stderr,
            "Usage: %s [--interval <seconds>] [--logfile <path>] [--device <path>]\n"
            "Defaults:\n"
            "  interval: 5 seconds\n"
            "  logfile: /tmp/assignment_sensor.log (fallback: /var/tmp/assignment_sensor.log)\n"
            "  device:  /dev/urandom\n",
            prog);
}

static int parse_int(const char *s, int *out) {
    char *end = NULL;
    long val = strtol(s, &end, 10);
    if (end == s || *end != '\0' || val <= 0 || val > 86400) {
        return -1;
    }
    *out = (int)val;
    return 0;
}

/**
 * Open an explicitly provided log path. No automatic fallback here:
 * if this fails, the program exits with an error so the user notices.
 */
static int open_logfile_explicit(const char *path, FILE **out_file, char **used_path) {
    FILE *f = fopen(path, "a");
    if (!f) {
        return -1;
    }
    *out_file = f;
    *used_path = strdup(path);
    return 0;
}

/**
 * Open the default log file with built-in fallback:
 *   1) /tmp/assignment_sensor.log
 *   2) /var/tmp/assignment_sensor.log
 */
static int open_default_logfile(FILE **out_file, char **used_path) {
    const char *primary = "/tmp/assignment_sensor.log";
    const char *fallback = "/var/tmp/assignment_sensor.log";

    FILE *f = fopen(primary, "a");
    if (f) {
        *out_file = f;
        *used_path = strdup(primary);
        return 0;
    }
    f = fopen(fallback, "a");
    if (f) {
        *out_file = f;
        *used_path = strdup(fallback);
        return 0;
    }
    return -1;
}

static int open_device(const char *path) {
    int fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd < 0) {
        return -1;
    }
    return fd;
}

/**
 * Format current time as ISO-8601 in UTC with milliseconds, e.g.:
 *   2025-09-19T15:03:21.123Z
 */
static void format_iso8601(char *buf, size_t buflen) {
    struct timespec ts;
    struct tm tm_utc;
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
        // Fallback: just zero
        ts.tv_sec = 0;
        ts.tv_nsec = 0;
    }
    gmtime_r(&ts.tv_sec, &tm_utc);

    int millis = (int)(ts.tv_nsec / 1000000);
    snprintf(buf, buflen,
             "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ",
             tm_utc.tm_year + 1900,
             tm_utc.tm_mon + 1,
             tm_utc.tm_mday,
             tm_utc.tm_hour,
             tm_utc.tm_min,
             tm_utc.tm_sec,
             millis);
}

int main(int argc, char *argv[]) {
    int interval_sec = 5;
    const char *log_path_arg = NULL;
    const char *device_path = "/dev/urandom";

    // Simple manual arg parsing
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--interval") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "--interval requires a value\n");
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            if (parse_int(argv[i + 1], &interval_sec) != 0) {
                fprintf(stderr, "Invalid interval: %s\n", argv[i + 1]);
                return EXIT_FAILURE;
            }
            i++;
        } else if (strcmp(argv[i], "--logfile") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "--logfile requires a value\n");
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            log_path_arg = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "--device") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "--device requires a value\n");
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            device_path = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            usage(argv[0]);
            return EXIT_SUCCESS;
        } else {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    // Setup signal handler for SIGTERM (and SIGINT for manual tests)
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigterm;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGTERM, &sa, NULL) != 0) {
        perror("sigaction(SIGTERM)");
        return EXIT_FAILURE;
    }
    if (sigaction(SIGINT, &sa, NULL) != 0) {
        perror("sigaction(SIGINT)");
        return EXIT_FAILURE;
    }

    FILE *logf = NULL;
    char *used_log_path = NULL;
    int rc;
    if (log_path_arg) {
        rc = open_logfile_explicit(log_path_arg, &logf, &used_log_path);
        if (rc != 0) {
            fprintf(stderr, "Failed to open log file '%s': %s\n",
                    log_path_arg, strerror(errno));
            return EXIT_FAILURE;
        }
    } else {
        rc = open_default_logfile(&logf, &used_log_path);
        if (rc != 0) {
            fprintf(stderr, "Failed to open default log file in /tmp or /var/tmp: %s\n",
                    strerror(errno));
            return EXIT_FAILURE;
        }
    }

    // line-buffered logging to avoid partial lines
    setvbuf(logf, NULL, _IOLBF, 0);

    int dev_fd = open_device(device_path);
    if (dev_fd < 0) {
        fprintf(stderr, "Failed to open device '%s': %s\n",
                device_path, strerror(errno));
        fclose(logf);
        free(used_log_path);
        return EXIT_FAILURE;
    }

    fprintf(logf, "# assignment-sensor started, logging to %s, interval=%d s, device=%s\n",
            used_log_path, interval_sec, device_path);
    fflush(logf);

    while (!g_stop) {
        uint32_t value = 0;
        ssize_t n = read(dev_fd, &value, sizeof(value));
        if (n < 0) {
            fprintf(stderr, "Error reading from device '%s': %s\n",
                    device_path, strerror(errno));
            break;
        } else if (n != (ssize_t)sizeof(value)) {
            fprintf(stderr, "Short read from device '%s'. Expected %zu, got %zd\n",
                    device_path, sizeof(value), n);
            break;
        }

        char tsbuf[40];
        format_iso8601(tsbuf, sizeof(tsbuf));
        // Example line: 2025-09-19T15:03:21.123Z | 0xDEADBEEF
        fprintf(logf, "%s | 0x%08X\n", tsbuf, value);
        fflush(logf);

        // sleep in 1-second chunks so SIGTERM is handled promptly
        int remaining = interval_sec;
        while (remaining > 0 && !g_stop) {
            int s = sleep(remaining);
            if (s == 0) {
                break; // slept full interval
            }
            remaining = s;
        }
    }

    fprintf(logf, "# assignment-sensor stopping gracefully\n");
    fflush(logf);

    close(dev_fd);
    fclose(logf);
    free(used_log_path);

    return EXIT_SUCCESS;
}
