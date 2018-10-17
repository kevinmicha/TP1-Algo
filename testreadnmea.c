/*
ejecutar junto a archivo ejemplo.nmea

$ cat ejemplo.nmea | ./testreadnmea

tambien crear un ejemplo.nmea con sentencias erroneas en varios sentidos
para poner a prueba las funciones

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define CHAR_INIT_NMEA '$'
#define CHAR_END_NMEA '*'
#define MAX_STR_NMEA 100
#define STR_HEX "0123456789ABCDEF"
#define GGA_STR "GGA"
#define GGA_CHARS 3
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

typedef enum {ST_DATA_ERR, ST_OK, ST_HELP, ST_EOF, ST_INVALID_NUMBER_ERROR, ST_NUMERICAL_ERROR} status_t;
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


status_t read_nmea(char (*statement)[], int *);
int hexstring_2_integer(int d1, int d2);
status_t latitude(char **pos_ptr, double *lat);
status_t longitude(char **pos_ptr, double *lon);
status_t time_of_fix(char **pos_ptr, trackpt_t *trackpt, struct tm meta_time);
status_t quality_of_fix(char **pos_ptr, fix_quality_t *qual);
status_t num_of_satellites(char **pos_ptr, size_t *n_sat);
status_t hdop(char **pos_ptr, double *hdop);
status_t elevation(char **pos_ptr, double *elevation);
status_t undulation_of_geoid(char **pos_ptr, double *undulation);

/*##################  command.h #########################*/

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

//##### gpx.h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define XML_HEADER "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" // las comillas adentro podrían ser un problema?     // para imprimir comillas se pone \"   
#define GPX_VERSION "<gpx version=\"1.1\" creator=\"95.11 TP1 - Tracker\" xmlns=\"http://www.topografix.com/GPX/1/1\">"
#define OPEN_METADATA "<metadata>"
#define CLOSE_METADATA "</metadata>"
#define OPEN_NAME "<name>"
#define CLOSE_NAME "</name>"
#define OPEN_TIME "<time>"
#define CLOSE_TIME "</time>"
#define OPEN_TIME_CHAR 'T'
#define CLOSE_TIME_CHAR 'Z'
#define TRACKPT_LAT "<trkpt lat="
#define TRACKPT_LON "lon="
#define OPEN_ELEVATION "<ele>"
#define CLOSE_ELEVATION "</ele>"
#define CLOSE_TRACKPT "</trkpt>"
#define OPEN_TRKSEG "<trkseg>" 
#define OPEN_TRK "<trk>" 
#define OPEN_GPX "<gpx>" 
#define CLOSE_TRKSEG "</trkseg>" 
#define CLOSE_TRK "</trk>" 
#define CLOSE_GPX "</gpx>" 
#define INVERTED_COMMAS '"'



void print_header(void);
void print_opening_tags(void);
void print_closing_tags(void);
void display_metadata(metadata_t *metadata);
void display_track_point(trackpt_t *trackpt);

//##################        MAIN         ####################

int main(int argc, const char *argv[]) {

	int checksum = 0;
	metadata_t metadata;
	status_t arg_validation, read_val;
	trackpt_t trackpt;
	char statement[MAX_STR_NMEA], *pos_ptr;

	if((arg_validation = validate_argv(argc, argv)) == ST_DATA_ERR) {
		puts("Error --> help");//print_help();
		return EXIT_FAILURE;
	}
	if(arg_validation == ST_HELP){
		puts("Help");//print_help();
		return EXIT_SUCCESS;
	}

	if(read_time_argv(argc, argv, &(metadata.meta_time)) != ST_OK) {
		puts("Error --> help");//print_help();
		return EXIT_FAILURE;
	}

	if(read_name_argv(argc, argv, &metadata) != ST_OK) {
		puts("Error --> help");//print_help();
		return EXIT_FAILURE;
	}

	printf("%s\n%d/%d/%d\n%d:%d:%d\n\n", metadata.name, metadata.meta_time.tm_mday, metadata.meta_time.tm_mon + 1, metadata.meta_time.tm_year + MIN_YEAR_STRUCT_TM, metadata.meta_time.tm_hour, metadata.meta_time.tm_min, metadata.meta_time.tm_sec);
		
	print_header();
	display_metadata(&metadata);
	print_opening_tags();

	do {
		read_val = read_nmea(&statement, &checksum);
		if(read_val == ST_OK){
			pos_ptr = statement;
			if(time_of_fix(&pos_ptr, &trackpt, metadata.meta_time) != ST_OK);
			else if(latitude(&pos_ptr, &trackpt.latitude) != ST_OK);
			else if(longitude(&pos_ptr, &trackpt.longitude) != ST_OK);
			else if(quality_of_fix(&pos_ptr, &trackpt.quality) != ST_OK);
			else if(num_of_satellites(&pos_ptr, &trackpt.n_sat) != ST_OK);
			else if(hdop(&pos_ptr, &trackpt.hdop) != ST_OK);
			else if(elevation(&pos_ptr, &trackpt.elevation) != ST_OK);
			else if(undulation_of_geoid(&pos_ptr, &trackpt.undulation_of_geoid) == ST_OK){
				display_track_point(&trackpt);
			}
		}
	} while(read_val != ST_EOF);

	print_closing_tags();

	return EXIT_SUCCESS;
}
//#################       FUNCIONES command.c  ####################

status_t read_time_argv(int argc, const char *argv[], struct tm *meta_time) {

	char *endptr;
	int year, month, day, i;
	/*[1] La variable time_fix se utiliza para normalizar la estructura tm. Ej: 30/02/2014 ----> 02/03/2014.*/ 
	time_t actual_time = time(NULL), time_fix;
	/*[2] Se carga el tiempo de la computadora en la estructura tm.*/
	*meta_time = *gmtime(&actual_time);

	/*[3] Si se encuentran argumentos 'time' (-f, -Y, --day, etc.) se modifica la estructura tm.*/
	for(i = 1; i < argc; i++)
		if(! strcmp(STR_ARG_DATE_1, argv[i]) || ! strcmp(STR_ARG_DATE_2, argv[i])) {
			i++;
			/*[4] Mediante la func. date2ymd se carga la fecha en la estructura (ver 7).*/
			if(date_2_ymd(argv[i], &(meta_time -> tm_year), &(meta_time -> tm_mon), &(meta_time -> tm_mday)) != ST_OK)
				return ST_DATA_ERR;
			/*[5] Se realizan correcciones del año y mes por el formato que usa struct tm.*/
			(*meta_time).tm_year -= MIN_YEAR_STRUCT_TM;
			(*meta_time).tm_mon --;
			break;
		}

	for(i = 1; i < argc; i++)
		if(! strcmp(STR_ARG_YEAR_1, argv[i]) || ! strcmp(STR_ARG_YEAR_2, argv[i])) {
			i++;
			if(! (year = strtoul(argv[i], &endptr, BASE))|| (*endptr && *endptr != CHAR_NEW_LINE))
				return ST_DATA_ERR;
			if(year < MIN_YEAR || year > MAX_YEAR)
				return ST_DATA_ERR;
			meta_time -> tm_year = year - MIN_YEAR_STRUCT_TM;
			break;
		}

	for(i = 1; i < argc; i++)
		if(! strcmp(STR_ARG_MONTH_1, argv[i]) || ! strcmp(STR_ARG_MONTH_2, argv[i])) {
			i++;
			if(! (month = strtoul(argv[i], &endptr, BASE))|| (*endptr && *endptr != CHAR_NEW_LINE))
				return ST_DATA_ERR;
			if(! month || month > MAX_MONTH)
				return ST_DATA_ERR;
			meta_time -> tm_mon = --month;
			break;
		}

	for(i = 1; i < argc; i++)
		if(! strcmp(STR_ARG_DAY_1, argv[i]) || ! strcmp(STR_ARG_DAY_2, argv[i])) {
			i++;
			if(! (day = strtoul(argv[i], &endptr, BASE))|| (*endptr && *endptr != CHAR_NEW_LINE))
				return ST_DATA_ERR;
			if(! day || day > MAX_DAY)
				return ST_DATA_ERR;
			meta_time -> tm_mday = day;
			break;
		}

	/*[6] Normaliza la estructura de ser necesario (ver 1).*/
	if((time_fix = mktime(meta_time)) == -1)
		return ST_DATA_ERR;
	return ST_OK;
}


/*[7] Convierte formato YYYYMMDD a año mes y dia enteros.*/
status_t date_2_ymd(const char *date_str, int *year, int *month, int *day) {
	
	char *endptr;
	int date, aux;

	if(! (date = strtoul(date_str, &endptr, BASE)) || (*endptr && *endptr != CHAR_NEW_LINE)) //valída lectura
		return ST_DATA_ERR;

	/*[8] Toma los ultimos 2 digitos.*/
	aux = date % TEN_POW_DIGITS_DAY;
	/*[9] Valida el dia.*/
	if(aux > MAX_DAY || ! aux)
		return ST_DATA_ERR;
	else {
		*day = aux;
		/*[10] Descarta el dia ya leido: YYYYMMDD --> YYYYMM.*/
		date = (date - aux) / TEN_POW_DIGITS_DAY;
	}
	
	aux = date % TEN_POW_DIGITS_MONTH;
	if(aux > MAX_MONTH || ! aux)
		return ST_DATA_ERR;
	else {
		*month = aux;
		date = (date - aux) / TEN_POW_DIGITS_MONTH;
	}

	if(date > MAX_YEAR || date < MIN_YEAR)
		return ST_DATA_ERR;
	else
		*year = date;

	return ST_OK;
}

/*[11] Busca el maximo entero en un arreglo.*/
int max_arr(int v[], int n) {
	int i, max = 0;
	if(n > 0)
		for(max = v[0], i = 1; i < n; i++)
			if(v[i] > max)
				max = v[i];
	return max;
}

/*[12] Valida los argumentos en linea de comando.*/
status_t validate_argv(int argc, const char *argv[]) {
	
	size_t i, j;
	/*[13] Utiliza dos arreglos para contar las veces que aparecen los argumentos.*/
	int arg_call[ARGV_DICCTIONARY_SIZE] = {0}, arg_call_type[ARG_TYPE] = {0}, help_flag = 0;

	/*[14] Para cada argumento: busca el tipo y valida la correcta implementacion.*/
	for(i = 1; i < argc; i++) {
		for(j = 0; j < ARGV_DICCTIONARY_SIZE; j++) {
			if(! strcmp(argv[i], argv_dicctionary[j])) {
				/*[15] Los argumentos del tipo -h y --help no requieren validación.*/
				if(j == ARG_HELP_1_POS || j == ARG_HELP_2_POS) {
					help_flag = 1;
				/*[16] Los argumentos del tipo -n y --name requieren una cadena alfanumérica en el siguiente argumento.*/
				} else if(j == ARG_NAME_1_POS || j == ARG_NAME_2_POS){
					if(++i == argc)
						return ST_DATA_ERR;
					if(! isalnum(*argv[i]))
						return ST_DATA_ERR;
				/*[17] Los argumentos restantes requieren una cadena de numeros en el siguiente argumento.*/
				} else {
					if(++i == argc)
						return ST_DATA_ERR;
					if(! isdigit(*argv[i]))
						return ST_DATA_ERR;
				}
				arg_call[j]++;
				arg_call_type[j / ARG_STR_PER_TYPE]++;
				break;
			}
		}
		/*[18] Si el argumento no es compatible con ningun tipo en el diccionario devuelve error.*/
		if(j == ARGV_DICCTIONARY_SIZE)
			return ST_DATA_ERR;
	}
	/*[19] Si alguno de los argumentos (o tipo de argumentos) se repite, devuelve error.*/
	if(max_arr(arg_call, ARGV_DICCTIONARY_SIZE) > 1 || max_arr(arg_call_type, ARG_TYPE) > 1)
		return ST_DATA_ERR;
	/*[20] Si los argumentos son válidos y uno de ellos es del tipo 'ayuda', devuelve ayuda.*/
	if(help_flag)
		return ST_HELP;
	return ST_OK;
}


status_t read_name_argv(int argc, const char *argv[], metadata_t *chosen_name) {
	
	int i, j;

	for(i = 1; i < argc; i++) {
		if(! strcmp(STR_ARG_NAME_1, argv[i]) || ! strcmp(STR_ARG_NAME_2, argv[i])) {
			i++;
			/*[21] Verifica que solo son caracteres alfanumericos o '_' para espacios.*/
			for(j = 0; j < strlen(argv[i]); j++)
				if(! isalnum(*(argv[i] + j)) && *(argv[i] + j) != CHAR_NAME_SPACE)
					return ST_DATA_ERR;
			strcpy(chosen_name -> name, argv[i]);
			return ST_OK;
		}
	}
	/*[22] Si no se define el nombre a traves de la linea de commando, se asigna el default.*/
	strcpy(chosen_name -> name, DEFAULT_NAME);
	return ST_OK;
}



//#################       FUNCIONES read.c     ####################


//falta validar que lea solo del tipo GGA
status_t read_nmea(char (*statement)[], int *checksum) { //recibe la dirección de una cadena, en la cual carga la sentencia

	int c, i, cksm1, cksm2;
	*checksum = 0;
	const char gga_test[] = "GGA";

	while((c = getchar()) != CHAR_INIT_NMEA && c != EOF);
	for(i = 0; (c = getchar()) != CHAR_END_NMEA && c != '\n' && c != EOF && c != '\0' && c != CHAR_INIT_NMEA && i < MAX_STR_NMEA; i++) {
		(*statement)[i] = c;
		(*checksum) ^= c;
	}
	//cuando termina la información util del statement coloca '\0'
	(*statement)[i] = '\0';


	if(c != CHAR_END_NMEA) {
		if(c == EOF)
			return ST_EOF;
		if(c == CHAR_INIT_NMEA) { // si sale del for porque c = $, pudo haber tomado tal simbolo de la siguiente sentencia, se devuelve y sale.
			ungetc(CHAR_INIT_NMEA, stdin); // se puede? es feo pero bueno no queda otra.
			return ST_DATA_ERR;
		}

		return ST_DATA_ERR;
	}

	for(i = 0; i < GGA_CHARS; i++)
		if((*statement)[i + 2] != gga_test[i])
			return ST_DATA_ERR;

	//llega al '*' lee los dos caracteres del checksum y si son "validos" los pasa a int con hexstring_2_integer
	if((cksm1 = getchar()) == EOF || cksm1 == '\0' || (cksm2 = getchar()) == EOF || cksm2 == '\0')
		return ST_DATA_ERR;
	if(hexstring_2_integer((char)cksm1, (char)cksm2) != *checksum)
		return ST_DATA_ERR;

	return ST_OK;
}



int hexstring_2_integer(int d1, int d2) {

	size_t hex1, hex2;
	char hexa_dic[] = {STR_HEX}, *ptr;

	//busca el primer caracter en el dicc, si no lo encuentra --> error
	if((ptr = strrchr(hexa_dic, d1)) == NULL)
		return HEXSTRING_NULL_FIND_INT;
	//calcula la posicion del caracter respecto al comienzo de la cadena
	hex1 = (size_t)ptr - (size_t)hexa_dic; //consultar
	
	if((ptr = strrchr(hexa_dic, d2)) == NULL)
		return HEXSTRING_NULL_FIND_INT;
	hex2 = (size_t)ptr - (size_t)hexa_dic; //consultar
	
	if(hex1 >= HEX_DIGITS || hex2 >= HEX_DIGITS) // HC?? PARA MI SI, CAMBIAR (Kevin) // Asi esta bien? E.
		return HEXSTRING_NULL_FIND_INT;
	return (int)hex1 * HEX_DIGITS + hex2; // HC ?? PARA MI SI, CAMBIAR (Kevin) // Asi esta bien? E.

} // si está mal devuelve -1

status_t time_of_fix(char **pos_ptr, trackpt_t *trackpt, struct tm meta_time) { // espera la direcc. de un puntero apuntado a statement
	
	char aux[STR_NMEA_DATA_DIGITS], *end_ptr;
	int hours, minutes, seconds, miliseconds;
	size_t i;

	*pos_ptr += 6;

	for(i = 0; i < HOURS_DIGITS; i++) 
		aux[i] = *(*pos_ptr)++;
	aux[i] = '\0';
	hours = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0')
		return ST_NUMERICAL_ERROR;	

	for(i = 0; i < MINUTES_DIGITS; i++)
		aux[i] = *(*pos_ptr)++;
	aux[i] = '\0';
	minutes = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0')
		return ST_NUMERICAL_ERROR;	

	for(i = 0; i < SECONDS_DIGITS; i++)
		aux[i] = *(*pos_ptr)++;
	aux[i] = '\0';
	(*pos_ptr)++; //saltea el punto de los segundos
	seconds = strtof(aux, &end_ptr);
	if(*end_ptr != '\0')
		return ST_NUMERICAL_ERROR;	
	
	for(i = 0; i < MILISECONDS_DIGITS; i++)
		aux[i] = *(*pos_ptr)++;
	aux[i] = '\0';
	miliseconds = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0')
		return ST_NUMERICAL_ERROR;

	(*pos_ptr)++; // deja el puntero listo para que lo use la proxima función.

	(trackpt->trackpt_time).tm_hour = hours;
	(trackpt->trackpt_time).tm_min = minutes;
	(trackpt->trackpt_time).tm_sec = seconds;
	trackpt->trackpt_time_tm_milisec = miliseconds;
	(trackpt->trackpt_time).tm_year = meta_time.tm_year; 
	(trackpt->trackpt_time).tm_mon = meta_time.tm_mon;
	(trackpt->trackpt_time).tm_mday = meta_time.tm_mday;

	return ST_OK;
}

status_t latitude(char **pos_ptr, double *lat) { //recibe el puntero de time_of_fix

	char aux[STR_NMEA_DATA_DIGITS], *end_ptr;
	size_t degrees, i;
	int south_flag = 1;
	float minutes;

	for(i = 0; i < NMEA_LATITUDE_DEGREES; i++)
		aux[i] = *((*pos_ptr)++);
	aux[i] = '\0';

	degrees = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0')
		return ST_NUMERICAL_ERROR;

	for(i = 0; i < NMEA_LATITUDE_MINUTES + 1; i++) // sumo uno por el punto 
		aux[i] = *((*pos_ptr)++);
	aux[i] = '\0';

	minutes = strtof(aux, &end_ptr);
	if(*end_ptr != '\0')
		return ST_NUMERICAL_ERROR;
	
	if(*(++(*pos_ptr)) == SOUTH_CHAR)
		south_flag = -1;

	(*pos_ptr)++;
	
	*lat = south_flag * (degrees + minutes / CONVERSION_FACTOR_MINUTES);

	(*pos_ptr)++; //deja el puntero preparado para la proxima funcion

	return ST_OK;
}


status_t longitude(char **pos_ptr, double *lon) {

	char aux[STR_NMEA_DATA_DIGITS], *end_ptr;
	size_t degrees, i;
	int west_flag = 1;
	float minutes;

	for(i = 0; i < NMEA_LONGITUDE_DEGREES; i++)
		aux[i] = *((*pos_ptr)++);
	aux[i] = '\0';

	degrees = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0')
		return ST_NUMERICAL_ERROR;

	for(i = 0; i < NMEA_LONGITUDE_MINUTES + 1; i++) // sumo uno por el punto 
		aux[i] = *((*pos_ptr)++);
	aux[i] = '\0';

	minutes = strtof(aux, &end_ptr);
	if(*end_ptr != '\0')
		return ST_NUMERICAL_ERROR;
	
	if(*(++(*pos_ptr)) == WEST_CHAR)
		west_flag = -1;

	(*pos_ptr)++;
	
	*lon = west_flag * (degrees + minutes / CONVERSION_FACTOR_MINUTES);

	(*pos_ptr)++;

	return ST_OK;
}


status_t quality_of_fix(char **pos_ptr, fix_quality_t *qual) {

	int option = (int) **pos_ptr - (int) '0'; //feo.
	if(! isdigit(*(*pos_ptr)++))
		return ST_INVALID_NUMBER_ERROR;
	if(option > MAX_QUALITY)
		return ST_INVALID_NUMBER_ERROR;
		
	*qual = option;

	(*pos_ptr)++; // Deja el puntero en el siguiente dato a leer

	return ST_OK;		
}


status_t num_of_satellites(char **pos_ptr, size_t *n_sat) {

	char aux[STR_NMEA_DATA_DIGITS], *end_ptr;
	size_t i;

	for(i = 0; **pos_ptr != NMEA_DATA_SEPARATION_CHAR; i++)
		aux[i] = *((*pos_ptr)++);
	aux[i] = '\0';

	*n_sat = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0')
		return ST_INVALID_NUMBER_ERROR;
	if(*n_sat < MIN_SAT || *n_sat > MAX_SAT)
		return ST_INVALID_NUMBER_ERROR;

	(*pos_ptr)++; // Deja el puntero en el siguiente dato a leer

	return ST_OK;
}

status_t hdop(char **pos_ptr, double *hdop) {
	
	size_t i;
	char aux[STR_NMEA_DATA_DIGITS], *end_ptr;

	for(i = 0; i < HDOP_DIGITS + 1; i++) 
		aux[i] = *((*pos_ptr)++);
	aux[i] = '\0';

	*hdop = strtof(aux, &end_ptr);
	if(*end_ptr != '\0')
		return ST_NUMERICAL_ERROR;

	(*pos_ptr)++; // Deja el puntero en el siguiente dato a leer

	return ST_OK;
}

status_t elevation(char **pos_ptr, double *elevation) {
	
	size_t i;
	char aux[STR_NMEA_DATA_DIGITS], *end_ptr;

	for(i = 0; i < ELEVATION_DIGITS + 1; i++) 
		aux[i] = *((*pos_ptr)++);
	aux[i] = '\0';

	*elevation = strtof(aux, &end_ptr);
	if(*end_ptr != '\0')
		return ST_NUMERICAL_ERROR;

	*pos_ptr += 3; // ######HARDCODE, SALTEA LAS UNIDADES.

	return ST_OK;
}

status_t undulation_of_geoid(char **pos_ptr, double *undulation) {
	
	size_t i;
	char aux[STR_NMEA_DATA_DIGITS], *aux_ptr;

	for(i = 0; i < UNDULATION_OF_GEOID_DIGITS + 1; i++) 
		aux[i] = *((*pos_ptr)++);
	aux[i] = '\0';

	*undulation = strtof(aux, &aux_ptr);
	if(*aux_ptr != '\0' && *aux_ptr != '\n')
		return ST_NUMERICAL_ERROR;

	return ST_OK;
}


//########### gpx.c
void print_header(void) {
	
	printf("%s\n", XML_HEADER); // segun el TP va doble \n
	printf("%s\n", GPX_VERSION);

}


void print_closing_tags(void) {
	
	printf("\t\t%s\n", CLOSE_TRKSEG);
	printf("\t%s\n", CLOSE_TRK);
	printf("%s\n", CLOSE_GPX);

}

/* Está bien esta notación de flechitas? */

void display_metadata(metadata_t *metadata) { // deberia recibir la direccion de una variable tipo metadata_t (que es lo mismo que struct metadata)	→ display_metadata(metadata_t *metadata)
	
	printf("\t%s\n", OPEN_METADATA);
	printf("\t\t%s%s%s\n", OPEN_NAME, metadata->name, CLOSE_NAME); // metadata_t es un tipo, deberia ser metadata solo, (el nombre de la variable que es del tipo metadata_t)
	printf("\t\t%s%d-%d-%d ", OPEN_TIME, (metadata->meta_time).tm_year + MIN_YEAR_STRUCT_TM, (metadata->meta_time).tm_mon + 1, (metadata->meta_time).tm_mday); // cambiar metadata_t por metadata. no se como se manejan flechas multiples, calculo que es lo mismo
	printf("%c%d:%d:%d", OPEN_TIME_CHAR, (metadata->meta_time).tm_hour, (metadata->meta_time).tm_min, (metadata->meta_time).tm_sec);
	printf("%c%s\n", CLOSE_TIME_CHAR, CLOSE_TIME);
	printf("\t%s\n", CLOSE_METADATA);

}

void print_opening_tags(void) {

	printf("\t%s\n", OPEN_TRK);	
	printf("\t\t%s\n", OPEN_TRKSEG);
}

void display_track_point(trackpt_t *trackpt) { // Exactamente lo mismo que antes, trackpt_t es un tipo de variable, la variable se llama trackpt sola (quizá fueron malos nombres y confundieron). Hace falta pasarle la direccion de la estructura. (en realidad solo alcanza pasarle el valor, pero si queremos usar notacion flechita necesita un puntero)

	printf("\t\t\t%s%c%f%c ", TRACKPT_LAT, INVERTED_COMMAS, trackpt->latitude, INVERTED_COMMAS);
	printf("%s%c%f%c>\n", TRACKPT_LON, INVERTED_COMMAS, trackpt->longitude, INVERTED_COMMAS);
	printf("\t\t\t\t%s%f%s\n", OPEN_ELEVATION, trackpt->elevation, CLOSE_ELEVATION);
	printf("\t\t\t\t%s%d-%d-%d ", OPEN_TIME, (trackpt->trackpt_time).tm_year + MIN_YEAR_STRUCT_TM, (trackpt->trackpt_time).tm_mon + 1, (trackpt->trackpt_time).tm_mday);
	printf("%c%d:%d:%d", OPEN_TIME_CHAR, (trackpt->trackpt_time).tm_hour, (trackpt->trackpt_time).tm_min, (trackpt->trackpt_time).tm_sec);
	printf("%c%s\n", CLOSE_TIME_CHAR, CLOSE_TIME);
	printf("\t\t\t%s\n", CLOSE_TRACKPT);

}