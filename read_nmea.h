#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define CHAR_INIT_NMEA '$'
#define CHAR_END_NMEA '*'
#define MAX_STR_NMEA 70
#define STR_HEX "0123456789ABCDEF"
#define GGA_STR "GGA"
#define GGA_CHARS 3
#define NMEA_TYPE_OF_STATEMENT_CHARS 5
#define HEXSTRING_NULL_FIND_INT -1
#define HEX_DIGITS 16

#define STR_NMEA_DATA_DIGITS 10

/* macros de tiempo de fix */
#define HOURS_DIGITS 2
#define MINUTES_DIGITS 2
#define SECONDS_DIGITS 2
#define MILISECONDS_DIGITS 3

/* macros de latitud y longitud */
#define NMEA_LATITUDE_DEGREES 2
#define NMEA_LATITUDE_MINUTES 5
#define NMEA_LONGITUDE_DEGREES 3
#define NMEA_LONGITUDE_MINUTES 5
#define CONVERSION_FACTOR_MINUTES 60
#define SOUTH_CHAR 'S'
#define WEST_CHAR 'W'

/* macros de calidad de fix */

#define MAX_QUALITY 8

/* macros de cantidad de satelites */
#define MAX_SAT 12
#define MIN_SAT 0
#define NMEA_DATA_SEPARATION_CHAR ','

/* el resto */

#define HDOP_DIGITS 2
#define ELEVATION_DIGITS 2
#define UNDULATION_OF_GEOID_DIGITS 2

typedef enum {ST_DAT_ERR, ST_OK, ST_HELP, ST_EOF, ST_INVALID_NUMBER_ERROR, ST_NUMERICAL_ERROR} status_t;
typedef enum {INVALID_FIX, GPS_FIX, DGPS_FIX, PPS_FIX, RTK_FIX, FLOAT_RTK_FIX, ESTIMATED_FIX, MANUAL_FIX, SIMULATION_FIX} fix_quality_t;

/* La diferencia entre ST_NUMERICAL_ERROR y ST_INVALID_NUMBER_ERROR es que en el primer caso, por ej, se da un float en vez de un int (tipo invalido) 
En el segundo caso tiene que ver con que no respeta un rango pedido (ej: numero de satelites de 0 a 12) */

typedef struct trackpt {
	struct tm trackpt_time;
	int trackpt_time_tm_milisec; // miliseconds agregados!
	double latitude;
	double longitude;
	fix_quality_t quality;
	size_t n_sat;
	double hdop;
	double elevation;
	double undulation_of_geoid;
} trackpt_t;


status_t read_nmea(char (*statement)[]);
int hexstring_2_integer(int d1, int d2);
status_t latitude(char **pos_ptr, double *lat);
status_t longitude(char **pos_ptr, double *lon);
status_t time_of_fix(char **pos_ptr, trackpt_t *trackpt, struct tm meta_time);
status_t quality_of_fix(char **pos_ptr, fix_quality_t *qual);
status_t num_of_satellites(char **pos_ptr, size_t *n_sat);
status_t hdop(char **pos_ptr, double *hdop);
status_t elevation(char **pos_ptr, double *elevation);
status_t undulation_of_geoid(char **pos_ptr, double *undulation);