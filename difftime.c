#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

const char * help_msg = "Help \n\
version: %s \n\
usage: %s \
[-a|--atime] \
[-c|--ctime] \
[-m|--mtime] \
[-A|--absolute] \
[-D|--days] \
[-H|--hours] \
[-M|--minutes] \
[-P|--print] \
[-h|--help] \
[-v|--version] \
\n\
\n\
difftime is a simple utility to calculate and print out the delta time from \n\
two files or a file and now. It's usefull to make a gate in a script, based on time.   \n\
\n\
Options: \n\
    -a, --atime file         compare last access time\n\
    -c, --ctime file         compare change time\n\
    -m, --mtime file         compare modification time\n\
    -A, --absolute           output without sign\n\
    -D, --days               output in days\n\
    -H, --hours              output in hours\n\
    -M, --minutes            output in minutes\n\
    -P, --print              add newline character after the number\n\
    -h, --help               display this help message \n\
    -v, --version            display the software version \n\
\n\
Example: \n\
    difftime -P /var/spool/newer /var/spool/older\n\
";

static struct option const long_options[] = {
    {"atime",          required_argument, NULL, 'a'},
    {"ctime",          required_argument, NULL, 'c'},
    {"mtime",          required_argument, NULL, 'm'},
    {"days",           no_argument, NULL, 'D'},
    {"hours",          no_argument, NULL, 'H'},
    {"minutes",        no_argument, NULL, 'M'},
    {"absolute",       no_argument, NULL, 'A'},
    {"print",          no_argument, NULL, 'P'},
    {"help",           no_argument, NULL, 'h'},
    {"version",        no_argument, NULL, 'v'},
    {NULL, 0, NULL, 0}
};

enum TimeField {
    ATIME,
    CTIME,
    MTIME
};

enum OutputUnit {
    SECONDS,
    MINUTES,
    HOURS,
    DAYS
};

struct {
    bool print;
    bool absolute;
    enum TimeField from_time;
    enum TimeField to_time;
    enum OutputUnit output_unit;
} config;

void config_init(void);

#include "config.h"

void print_help(char *name);
void print_version(char *name);

void die(const char *prefix, const char *msg);

/*
 * time difference
 * abs: if true, returns always a positive value
 */
time_t timediff(time_t a, time_t b, bool abs);

/* Get the time from st according to tf */
time_t extract_time(struct stat *st, enum TimeField tf);

/* convenient function */
void config_time(char *arg, char **f, char **t, enum TimeField tm);


int main(int argc, char *argv[])
{
    time_t tfrom, tto, tdiff; 
    char *ffrom, *fto; /* filenames */
    struct stat st;
    int c;
    char fmt[10];

    config_init();

    /* manage options */
    ffrom = NULL;
    fto = NULL;
    while ((c = getopt_long(argc, argv, "a:c:m:ADHMPhv", long_options, NULL)) > 0){
        switch(c){
        case 'a': /* atime */
            config_time(optarg, &ffrom, &fto, ATIME);
            break;
        case 'c': /* ctime */
            config_time(optarg, &ffrom, &fto, CTIME);
            break;
        case 'm': /* mtime */
            config_time(optarg, &ffrom, &fto, MTIME);
            break;
        case 'h': /* help */
            print_help(argv[0]);
            exit(EXIT_SUCCESS); 
            break;
        case 'v': /* version */
            print_version(argv[0]);
            break;
        case 'A': /* absolute */
            config.absolute = true;
            break;
        case 'D': /* days */
            config.output_unit = DAYS;
            break;
        case 'H': /* hours */
            config.output_unit = HOURS;
            break;
        case 'M': /* minutes */
            config.output_unit = MINUTES;
            break;
        case 'P': /* print */
            config.print = true;
            break;
        default:
            fprintf(stderr, "%s --help for usage\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    /* Extract the times */
    errno = 0;
   
    if (ffrom == NULL){
        if (optind >= argc)
            die("First file","filename required. See -h");
        ffrom = argv[optind++];
    }

    if (stat(ffrom, &st) < 0)
        die(ffrom, strerror(errno));

    tfrom = extract_time(&st, config.from_time);

    /* second argument can be set with option (like -c), wihout option
     * or not set (use time instead)
     */
    if (fto == NULL && optind < argc){ /* set without option */
        fto = argv[optind++];          /* get from argv and continue */
    }

    if (fto != NULL){ /* with filename */
        if (stat(fto, &st) < 0)
            die(fto,strerror(errno));

        tto = extract_time(&st, config.to_time);
    } else {   /* not provided, use the current time */
        if (time(&tto) < 0)
            die("Get current time",strerror(errno));
    }

    /* OUTPUT */

    if (config.print){
        strcpy(fmt, "%ld\n");
    } else {
        strcpy(fmt, "%ld");
    }

    tdiff = timediff(tfrom, tto, config.absolute);
    switch (config.output_unit){
    case SECONDS:
        /* nothing to do */
        break;
    case MINUTES:
        tdiff /= 60; /* seconds in 1 minute */
        break;
    case HOURS:
        tdiff /= 3600; /* seconds in 1 hour */
        break;
    case DAYS:
        tdiff /= 86400; /* seconds in 1 day */
        break;
    }
    printf(fmt, tdiff);

    return 0;
}

void print_help(char *name)
{
    fprintf(stderr, help_msg, VERSION, name);
}

void print_version(char *name)
{
    fprintf(stdout, "%s: %s\n", name, VERSION);
}

void die(const char *prefix, const char *msg)
{
    if (prefix){
        fprintf(stderr, "%s: ", prefix);
    } else {
        fprintf(stderr, "FATAL: ");
    }

    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

time_t timediff(time_t a, time_t b, bool abs)
{
    time_t d;

    d = difftime(a, b);
    if (abs)
        d = labs(d);

    return d;
}

time_t extract_time(struct stat *st, enum TimeField tf)
{
    time_t tm;

    switch (tf){
    case ATIME:
        tm = st->st_atime;
        break;
    case CTIME:
        tm = st->st_ctime;
        break;
    case MTIME:
        tm = st->st_mtime;
        break;
    }

    return tm;
}

void config_time(char *arg, char **f, char **t, enum TimeField tm)
{
    if (*f == NULL){
        config.from_time = tm;
        *f = arg;
    } else {
        config.to_time = tm;
        *t = arg;
    }
}

