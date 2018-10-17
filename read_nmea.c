
#include "read_nmea.h"

status_t read_nmea(char (*statement)[], int *checksum) { //recibe la dirección de una cadena, en la cual carga la sentencia

	int c, i, cksm1, cksm2;
	*checksum = 0;
	const char gga_test[] = GGA_STR;

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

		*pos_ptr += (NMEA_TYPE_OF_STATEMENT_CHARS + 1);

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

