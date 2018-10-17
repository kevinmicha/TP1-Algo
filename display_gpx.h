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