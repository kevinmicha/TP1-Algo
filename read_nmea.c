#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "read_nmea.h"

/*[1] Lee las sentencias NMEA y las carga en la cadena statement. 
	  Devuelve: ST_OK si se realiza correctamente, ST_EOF si se cierra el flujo de datos, ST_DATA_ERR si la sentecia debe ser ignorada.*/
status_t read_nmea(char (*statement)[]) {

	int c, i, cksm1, cksm2;
	int checksum = 0;
	const char gga_test[] = GGA_STR;

	/*[2] Lee caracteres del buffer hasta llegar a '$'.*/
	while((c = getchar()) != CHAR_INIT_NMEA && c != EOF);
	/*[3] Carga caracteres en la cadena statement hasta leer '*', '\0', '\n', EOF, '$' o haber leído 100 caracteres (MAX_STR_NMEA). Realiza el checksum.*/
	for(i = 0; (c = getchar()) != CHAR_END_NMEA && c != '\n' && c != EOF && c != '\0' && c != CHAR_INIT_NMEA && i < MAX_STR_NMEA; i++) {
		(*statement)[i] = c;
		checksum ^= c;
	}
	/*[4] Coloca un '\0' al final de la cadena leída.*/
	(*statement)[i] = '\0';

	/*[5] Si el ciclo for no se corta por la lectura de '*' la sentencia debe ser ignorada.*/
	if(c != CHAR_END_NMEA) {
		/*[6] Si el ciclo for corta por EOF devuelve ST_EOF para avisar que se cerro el flujo de datos.*/
		if(c == EOF)
			return ST_EOF;
		/*[7] Si el ciclo for corta por '$' regresa tal caracter al buffer para no descartar la siguiente sentencia. La sentencia actual debe ser ignorada.*/
		if(c == CHAR_INIT_NMEA) {
			ungetc(CHAR_INIT_NMEA, stdin);
			return ST_DATA_ERR;
		}

		return ST_DATA_ERR;
	}

	/*[8] Comprueba que la sentencia es del tipo GGA, si no lo es, la sentencia debe ser ignorada.*/
	for(i = 0; i < GGA_CHARS; i++)
		if((*statement)[i + 2] != gga_test[i])
			return ST_DATA_ERR;

	/*[9] Lee los dos caracteres del checksum, los convierte en decimal y lo compara con el checksum calculado.*/
	if((cksm1 = getchar()) == EOF || cksm1 == '\0' || (cksm2 = getchar()) == EOF || cksm2 == '\0')
		return ST_DATA_ERR;
	if(hexstring_2_integer((char)cksm1, (char)cksm2) != checksum)
		return ST_DATA_ERR;

	return ST_OK;
}

/*[10] Convierte un numero hexadecimal de dos digitos a entero.
	   Devuelve: -1 en caso de no poder realizar la conversión.*/
int hexstring_2_integer(int d1, int d2) {

	size_t hex1, hex2;
	char hexa_dic[] = {STR_HEX}, *ptr;

	/*[11] Busca el primer caracter en el diccionario de digitos hexadecimales (STR_HEX), si no lo encuentra devuelve -1.*/
	if((ptr = strrchr(hexa_dic, d1)) == NULL)
		return HEXSTRING_NULL_FIND_INT;
	/*[12] Si el caracter se encuentra en el diccionario, se calcula su posición respecto del primer caracter del diccionario.*/
	hex1 = (size_t)ptr - (size_t)hexa_dic;
	
	if((ptr = strrchr(hexa_dic, d2)) == NULL)
		return HEXSTRING_NULL_FIND_INT;
	hex2 = (size_t)ptr - (size_t)hexa_dic;
	
	/*[13] Si la conversion resulta invalida devuelve -1. De otro modo, realiza la conversion.*/
	if(hex1 >= HEX_DIGITS || hex2 >= HEX_DIGITS)
		return HEXSTRING_NULL_FIND_INT;
	return (int)hex1 * HEX_DIGITS + hex2;

}

/*[14] Carga los datos de tiempo en la estructura trackpt. Lee de la sentencia NMEA (apuntada por pos_ptr) horas, minutos y segundos. El año, mes y dia los recibe de la estructura metadata.
	   Devuelve: ST_OK si los datos se cargan correctamente y ST_NUMERICAL_ERROR si los datos son invalidos.*/
status_t time_of_fix(char **pos_ptr, trackpt_t *trackpt, struct tm meta_time) {
	
	char aux[STR_NMEA_DATA_DIGITS], *end_ptr;
	int hours, minutes, seconds, miliseconds;
	size_t i;

	/*[15] Avanza el puntero pos_ptr hasta el primer caracter del horario.*/
	*pos_ptr += (NMEA_TYPE_OF_STATEMENT_CHARS + 1);

	/*[16] Lee la cantidad de caracteres correspondientes a la horas, los carga en una cadena y la convierte a un numero.*/
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
	seconds = strtof(aux, &end_ptr);
	if(*end_ptr != '\0')
		return ST_NUMERICAL_ERROR;

	/*[17] El puntero pos_ptr queda en el punto que separa los segundos de los milisegundos. Avanza un caracter.*/
	(*pos_ptr)++;
	
	for(i = 0; i < MILISECONDS_DIGITS; i++)
		aux[i] = *(*pos_ptr)++;
	aux[i] = '\0';
	miliseconds = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0')
		return ST_NUMERICAL_ERROR;

	/*[18] El puntero pos_ptr queda en la coma que separa el horario de la latitud. Avanza un caracter para que la proxima funcion pueda leer correctamente.*/
	(*pos_ptr)++;

	(trackpt->trackpt_time).tm_hour = hours;
	(trackpt->trackpt_time).tm_min = minutes;
	(trackpt->trackpt_time).tm_sec = seconds;
	trackpt->trackpt_time_tm_milisec = miliseconds;
	(trackpt->trackpt_time).tm_year = meta_time.tm_year; 
	(trackpt->trackpt_time).tm_mon = meta_time.tm_mon;
	(trackpt->trackpt_time).tm_mday = meta_time.tm_mday;

	return ST_OK;
}

/*[19] Lee la latitud de la sentencia NMEA y la guarda en una variable double (trackpt.latitude). Recibe el puntero pos_ptr apuntado al primer caracter de la latitud.
	   Devuelve: ST_OK si la lectura se realiza correctamente y ST_NUMERICAL_ERROR si los datos son invalidos.*/
status_t latitude(char **pos_ptr, double *lat) { 

	char aux[STR_NMEA_DATA_DIGITS], *end_ptr;
	size_t degrees, i;
	int south_flag = 1;
	float minutes;

	/*[20] Lee los grados y minutos, los convierte a numeros y realiza el pasaje a grados.*/
	for(i = 0; i < NMEA_LATITUDE_DEGREES; i++)
		aux[i] = *((*pos_ptr)++);
	aux[i] = '\0';

	degrees = strtoul(aux, &end_ptr, 10);
	if(*end_ptr != '\0')
		return ST_NUMERICAL_ERROR;

	for(i = 0; i < NMEA_LATITUDE_MINUTES + 1; i++)
		aux[i] = *((*pos_ptr)++);
	aux[i] = '\0';

	minutes = strtof(aux, &end_ptr);
	if(*end_ptr != '\0')
		return ST_NUMERICAL_ERROR;
	
	/*[21] Lee el caracter que indica Norte o Sur. En caso de que se lea el caracter asociado a Sur multiplica por -1 los grados.*/
	if(*(++(*pos_ptr)) == SOUTH_CHAR)
		south_flag = -1;
	
	*lat = south_flag * (degrees + minutes / CONVERSION_FACTOR_MINUTES);

	/*[22] Deja el puntero pos_ptr preparado para la proxima funcion (esto se realiza repetidas veces en este conjunto de funciones).*/
	(*pos_ptr)++;
	(*pos_ptr)++;

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

	for(i = 0; i < NMEA_LONGITUDE_MINUTES + 1; i++)
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


/*[23] Lee la calidad del fix y la carga en la estructura.
	   Devuelve: ST_OK si la lectura se realiza correctamente y ST_NUMERICAL_ERROR si los datos son invalidos.*/
status_t quality_of_fix(char **pos_ptr, fix_quality_t *qual) {
	/*[24] Carga el numerodo de la calidad del fix en una variable. Comprueba que sea un numero valido y lo carga en la estructura.*/
	int option = (int) **pos_ptr - ZERO_ASCII_VALUE;

	if(! isdigit(*(*pos_ptr)++))
		return ST_INVALID_NUMBER_ERROR;
	if(option > MAX_QUALITY)
		return ST_INVALID_NUMBER_ERROR;
		
	*qual = option;

	(*pos_ptr)++;

	return ST_OK;		
}

/*[25] Lee la cantidad de satelites y la carga en la estructura.
	   Devuelve: ST_OK si la lectura se realiza correctamente y ST_INVALID_NUMBER_ERROR si los datos son invalidos.*/
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

	(*pos_ptr)++;

	return ST_OK;
}

/*[26] Lee hdop y la carga en la estructura.
	   Devuelve: ST_OK si la lectura se realiza correctamente y ST_NUMERICAL_ERROR si los datos son invalidos.*/
status_t hdop(char **pos_ptr, double *hdop) {
	
	size_t i;
	char aux[STR_NMEA_DATA_DIGITS], *end_ptr;

	for(i = 0; i < HDOP_DIGITS + 1; i++) 
		aux[i] = *((*pos_ptr)++);
	aux[i] = '\0';

	*hdop = strtof(aux, &end_ptr);
	if(*end_ptr != '\0')
		return ST_NUMERICAL_ERROR;

	(*pos_ptr)++;

	return ST_OK;
}

/*[27] Lee la elevacion y la carga en la estructura.
	   Devuelve: ST_OK si la lectura se realiza correctamente y ST_NUMERICAL_ERROR si los datos son invalidos.*/
status_t elevation(char **pos_ptr, double *elevation) {
	
	size_t i;
	char aux[STR_NMEA_DATA_DIGITS], *end_ptr;

	for(i = 0; i < ELEVATION_DIGITS + 1; i++) 
		aux[i] = *((*pos_ptr)++);
	aux[i] = '\0';

	*elevation = strtof(aux, &end_ptr);
	if(*end_ptr != '\0')
		return ST_NUMERICAL_ERROR;

	/*[28] Ignora las unidades de la elevacion.*/
	*pos_ptr++;
	*pos_ptr++;
	*pos_ptr++;

	return ST_OK;
}

/*[28] Lee la separacion del geoide y la carga en la estructura.
	   Devuelve: ST_OK si la lectura se realiza correctamente y ST_NUMERICAL_ERROR si los datos son invalidos.*/
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

