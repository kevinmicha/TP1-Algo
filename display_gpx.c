#include "read_nmea.h"
#include "command_arg.h"

void print_header(void) {
	
	printf("%s\n", XML_HEADER); // segun el TP va doble \n
	printf("%s\n", GPX_VERSION);

}

void print_opening_tags(void) {

	printf("\t%s\n", OPEN_TRK);	
	printf("\t\t%s\n", OPEN_TRKSEG);
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

void display_track_point(trackpt_t *trackpt) { // Exactamente lo mismo que antes, trackpt_t es un tipo de variable, la variable se llama trackpt sola (quizá fueron malos nombres y confundieron). Hace falta pasarle la direccion de la estructura. (en realidad solo alcanza pasarle el valor, pero si queremos usar notacion flechita necesita un puntero)

	printf("\t\t\t%s%c%f%c ", TRACKPT_LAT, INVERTED_COMMAS, trackpt->latitude, INVERTED_COMMAS);
	printf("%s%c%f%c>\n", TRACKPT_LON, INVERTED_COMMAS, trackpt->longitude, INVERTED_COMMAS);
	printf("\t\t\t\t%s%f%s\n", OPEN_ELEVATION, trackpt->elevation, CLOSE_ELEVATION);
	printf("\t\t\t\t%s%d-%d-%d ", OPEN_TIME, (trackpt->trackpt_time).tm_year + MIN_YEAR_STRUCT_TM, (trackpt->trackpt_time).tm_mon + 1, (trackpt->trackpt_time).tm_mday);
	printf("%c%d:%d:%d", OPEN_TIME_CHAR, (trackpt->trackpt_time).tm_hour, (trackpt->trackpt_time).tm_min, (trackpt->trackpt_time).tm_sec);
	printf("%c%s\n", CLOSE_TIME_CHAR, CLOSE_TIME);
	printf("\t\t\t%s\n", CLOSE_TRACKPT);

}


void print_closing_tags(void) {
	
	printf("\t\t%s\n", CLOSE_TRKSEG);
	printf("\t%s\n", CLOSE_TRK);
	printf("%s\n", CLOSE_GPX);

}