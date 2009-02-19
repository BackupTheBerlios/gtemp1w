#ifndef _T_SENSOR_H
#define _T_SENSOR_H

#include <glib.h>
#include <time.h>
#include <libconfig.h>

G_BEGIN_DECLS

/*
Pojedynczy pomiar sklada sie z odczytanej wartosci i czasu wykonania.
FIXME: Zastanowic sie, czy nie zwiekszyc precyzji do double.
*/
typedef struct {
		time_t		czas ;
		float			wartosc ;		// temperatura to 12 bitow - mozna reprezentowac bez utraty dokladnosci
} T_Sample ;

typedef enum { 
		BIT_9 = 9, 
		BIT_10 = 10, 
		BIT_11 = 11, 
		BIT_12 = 12
} T_Resolution ;

/*
Struktura przechowujaca wartosci odczytane z jednego czujnika. Wartosci te sa
przechowywane do momentu zapisu na dysk.
*/
typedef struct {
		
		GString *sensor_id ;	// identyfikator czujnika - 15 znakow + '\0'
		// np. 28.C35777010000
		GString *name ;				// nazwa uzywana w programie
		
		T_Resolution resolution ;		// rozdzielczosc pomiaru (9-12 bit dla DS1820)
		
		time_t read_interval ;			// przerwa pomiedzy kolejnymi pomiarami, -1 brak pomiarow
		
		gboolean	cached ;				// odczyt buforowany przez owfs (TRUE) lub bezposredni (FALSE)
		
		
		time_t last_read_time ;
		GArray	*sample_buffer ;
		
} T_Sensor ;



T_Sensor *t_sensor_new(const char *name, const char *id, T_Resolution res, 
											 time_t read_interval, gboolean cached) ;
void t_sensor_destroy (T_Sensor *s) ;

int t_sensor_add_sample(T_Sensor *sensor, T_Sample sample) ;

//
//	Dopisuje do tablicy "samples" punkty z bufora sensora "sensor"
//
void t_sensor_add_samples_to_array (const T_Sensor* sensor, GArray *samples, 
																		time_t t_begin, time_t t_end) ;

int t_sensor_save_configuration (config_setting_t *setting, T_Sensor *sensor) ;
gboolean t_sensor_need_read(T_Sensor *s) ;

// UWAGA ! Zwrocona tablica MUSI byc zwolniona po uzyciu
GArray *t_sensor_remove_samples(T_Sensor *sensor) ;


G_END_DECLS

#endif /* _T_SENSOR_H */

