#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MAX_STR 666
#define MAX_ARGV 10
#define BASE 10
#define CHAR_NEW_LINE '\n'

#define TEN_POW_DIGITS_DAY 100
#define TEN_POW_DIGITS_MONTH 100
#define MAX_DAY 31
#define MAX_MONTH 12
#define MAX_YEAR 2666
#define MIN_YEAR 1900
#define MIN_YEAR_STRUCT_TM 1900

#define STR_ARG_HELP_1 "-h"
#define STR_ARG_HELP_2 "--help"
#define ARG_HELP_1_POS 0
#define ARG_HELP_2_POS 1
#define STR_ARG_NAME_1 "-n"
#define STR_ARG_NAME_2 "--name"
#define ARG_NAME_1_POS 2
#define ARG_NAME_2_POS 3
#define STR_ARG_DATE_1 "-f" 
#define STR_ARG_DATE_2 "--format"
#define STR_ARG_YEAR_1 "-Y"
#define STR_ARG_YEAR_2 "--year"
#define STR_ARG_MONTH_1 "-m"
#define STR_ARG_MONTH_2 "--month"
#define STR_ARG_DAY_1 "-d"
#define STR_ARG_DAY_2 "--day"
#define ARGV_DICCTIONARY_SIZE 12
#define ARG_TYPE 6
#define ARG_STR_PER_TYPE 2

#define DEFAULT_NAME "Default"
#define CHAR_NAME_SPACE '_'

typedef enum {ST_DATA_ERR, ST_OK, ST_HELP, ST_EOF, ST_INVALID_NUMBER_ERROR, ST_NUMERICAL_ERROR} status_t;

typedef struct metadata {
	char name[MAX_STR];
	struct tm meta_time;
} metadata_t;

const char argv_dicctionary[][MAX_ARGV] = {STR_ARG_HELP_1, STR_ARG_HELP_2, STR_ARG_NAME_1, STR_ARG_NAME_2, STR_ARG_DATE_1, STR_ARG_DATE_2, STR_ARG_YEAR_1, STR_ARG_YEAR_2, STR_ARG_MONTH_1, STR_ARG_MONTH_2, STR_ARG_DAY_1, STR_ARG_DAY_2};

int max_arr(int v[], int n);
status_t validate_argv(int argc, const char *argv[]);
status_t read_time_argv(int argc, const char *argv[], struct tm *meta_time); //funcion que carga la estructura time (utiliza la sig. funcion)
status_t date_2_ymd(const char *date_str, int *year, int *month, int *day); //funcion que convierte formatos de fecha YYYYMMDD
status_t read_name_argv(int argc, const char *argv[], metadata_t *chosen_name);