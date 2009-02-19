/*
Wydaj sie, ze sa trzy konkurujace ze soba watki:

1. petla glowna, ktora podobno blokuje wszystkie callbacki

2. watek z tego pliku, ktory pobiera liste czujnikow do odczytania, czyta 
i nastepnie uaktualnia wszystkie czujniki i wyswietlacze

3. sekundowy callback glib, ktory wymazuje dane z sensorow (wyswietlaczy nie dotyka)
i zapisuje je na dysk.

*/
#include <unistd.h>
#include <stdio.h>
#include <gdk/gdk.h>
#include <glib.h>
#include <string.h>

#include "sensor_read_thread.h" 
#include "sensors.h"
#include "general_settings.h"


//#define SENSOR_PATH_PREFIX "/var/1-Wire/"


//------------------------------------------------------------------------------
// UWAGA - funkcja bedzie wywolywana takze z innych watkow, ale posrednio (przez
//	is_sensor_present.
// moze parametr przez wartosc ? - chyba wystarczy w funkcji wywolujacej
GString *get_sensor_path(const T_Sensor *sensor)
{
		GString *path = g_string_new(path_1_wire_directory->str) ;
		
		if (sensor->cached == FALSE)
				g_string_append_printf (path, "uncached/") ;
		
		g_string_append_printf(path, "%s/temperature%d", 
													 sensor->sensor_id->str,
													 sensor->resolution) ;
		
		return path ;
} ;
//------------------------------------------------------------------------------
// UWAGA - funkcja bedzie wywolywana takze z innych watkow
// 
gboolean is_sensor_present(const T_Sensor sensor)
{
		FILE *f_sens ;
		GString *path = get_sensor_path(&sensor) ;
		gboolean result = FALSE ;
		
		f_sens = fopen (path->str,"r") ;
		if (f_sens != NULL) {
				fclose(f_sens) ;
				result = TRUE ;
		} ;
		g_string_free(path, TRUE) ;
		
		return result ;
} ;
//------------------------------------------------------------------------------
int read_sensor(T_Sensor *sensor, T_Sample *result)
{
		GString *path = get_sensor_path(sensor) ;
		
		FILE *f_sens ;
		char stmp[20] ;
		
		f_sens = fopen (path->str,"r") ;
		if (f_sens == NULL)
				return FALSE ;
		
		fscanf (f_sens, "%s", &(stmp[0])) ;
		result->czas = time(NULL) ; 
		//fscanf (f_sens, "%f", &(pomiar.wartosc)) ;
		fclose(f_sens) ;		
		
		unsigned j, l = strlen(stmp) ;
		for (j=0;j<l;j++) {			// FIXME: ugly hack !!!!
						if (stmp[j] == '.')
						stmp[j] = ',' ;
		} ;
		sscanf(stmp, "%f", &(result->wartosc)) ;
		
		//printf ("%d %f OK\n", (int)result->czas, result->wartosc) ;
		
		g_string_free(path, TRUE) ;
		return TRUE ;
} ;
//------------------------------------------------------------------------------
GList *get_sensor_to_read_list_tsafe(void)
{
		GList *l ;
		
		gdk_threads_enter ();
		l = get_sensor_to_read_list() ;
		gdk_threads_leave ();
		
		return l ;
} ;
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
gpointer *read_thread_func (gpointer data)
{
		GList *l, *tmp ;
		T_Sensor *s ;
		T_Sample sample ;
		
		while (TRUE) {
				
				sleep(1) ;
				//printf ("thread\n") ;
				
				//l = get_sensor_to_read_list_tsafe() ;
				
				// czytaj, az nie bedzie zadnych czujnikow w kolejce
						while ((l = get_sensor_to_read_list_tsafe()) != NULL) { 				
								
								tmp = l ;
								while (tmp != NULL) {
										s = (T_Sensor*) tmp->data ;
										
										//
												//	FIXME: czasami pojawiaja sie dwa identyczne odczyty w momencie
												//	wylaczenia owfs - nie rozumiem dlaczego, zostawiam na pozniej
												//	wydaje sie, ze jakos szczegolnie mocno to nie szkodzi...
												//
												if (read_sensor(s, &sample) ) {
														gdk_threads_enter ();
														add_sample_to_sensor(sample, s->name->str) ;
														gdk_threads_leave ();				
												} ;
										
										tmp = g_list_next(tmp) ;
								} ;
								
								l = destroy_sensor_list(l) ;
						} ;
				
		} ; // end main loop
				
				return NULL ;
} ;
//------------------------------------------------------------------------------
