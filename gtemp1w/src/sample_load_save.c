#include <stdio.h>
#include <time.h>
#include <error.h>
#include <glib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "sample_load_save.h"
#include "sensors.h"
#include "general_settings.h"


//#define DATA_STOTE_PATH "/home/tad/tmp/data/"

//#define WRITE_SAMPLES_INTERVAL 10

time_t last_write_time = 0 ;

//------------------------------------------------------------------------------
gboolean check_and_store_samples (gpointer data)
{
		time_t now = time(NULL) ;
		
		if (now >= (last_write_time + write_samples_interval)) 
		{
				gdk_threads_enter() ; // to jest callback glib, patrz store_samples_one_day()
				save_all_sensors_data() ;
				gdk_threads_leave() ;
				
				last_write_time = time(NULL) ;
		} ;
		
		return TRUE ;
} ;
//------------------------------------------------------------------------------
gboolean is_dir(const char *path)
{
		struct stat file_status ;
		
		if ( stat(path, &file_status) < 0) 
				return FALSE ;
		
		return S_ISDIR(file_status.st_mode) ;
} ;
//------------------------------------------------------------------------------
gboolean check_and_create_dir(const char *path)
{
		//printf ("testowanie %s\n", path) ;
		if (is_dir(path))
				return TRUE ;
		
		if (mkdir(path, S_IRWXU) < 0) {
				printf ("Nie można utworzyć katalogu %s\n", path) ;
				return FALSE ;
		} ;
		
		return TRUE ;
} ;
//------------------------------------------------------------------------------
gboolean path_append_year(GString *path, const struct tm *s_date)
{
		char stmp[16] ;
		
		strftime(stmp, sizeof(stmp), "%Y", s_date) ;
		g_string_append_printf(path, "%s/", stmp) ;	
		
		return TRUE ;
} ;
gboolean path_append_month(GString *path, const struct tm *s_date)
{
		char stmp[16] ;
		
		strftime(stmp, sizeof(stmp), "%m", s_date) ;
		g_string_append_printf(path, "%s/", stmp) ;	
		
		return TRUE ;
} ;
gboolean path_append_day(GString *path, const struct tm *s_date)
{
		char stmp[16] ;
		
		strftime(stmp, sizeof(stmp), "%d", s_date) ;
		g_string_append_printf(path, "%s/", stmp) ;	
		
		return TRUE ;
} ;
//------------------------------------------------------------------------------
FILE *open_data_file_write(time_t s_date, const char* sensor_name)
{
		GString *path = g_string_new(path_data_directory->str) ;
		struct tm *d = localtime(&s_date) ;
		FILE *file = NULL ;
		
		if ( is_dir(path->str) == FALSE) {
				printf ("open_data_file_write: %s nie jest katalogiem\n", path->str) ;
				goto f_exit ;
		} ;
		
		if (path_append_year(path, d) == FALSE ||
				check_and_create_dir(path->str) == FALSE )
				goto f_exit ;
		//printf (path->str) ;
		
		if (path_append_month(path, d) == FALSE ||
				check_and_create_dir(path->str) == FALSE )
				goto f_exit ;
		//printf (path->str) ;
		
		if (path_append_day(path, d) == FALSE ||
				check_and_create_dir(path->str) == FALSE )
				goto f_exit ;
		//printf (path->str) ;
		
		
		g_string_append_printf(path, "%s.txt", sensor_name) ;
		
		file = fopen(path->str, "a") ;
		if (file == NULL) {
				printf ("open_data_file_write: nie mozna otworzyc pliku %s\n", path->str) ;
				perror("open_data_file_write") ;
		} ;
		//printf ("01\n") ;
		
		f_exit:
				g_string_free(path, TRUE) ;
		return file ;
} ;
//------------------------------------------------------------------------------
time_t get_midnight(time_t s_time)
{		
		struct tm *lt ;
		
		lt = localtime(&s_time) ;
		lt->tm_sec = 0 ;
		lt->tm_min = 0 ;
		lt->tm_hour = 0 ;
		
		return mktime(lt) ;
} ;
//------------------------------------------------------------------------------
//
//	Funkcja zapisuje do pliku probki z jednego dnia i USUWA je z tablicy.
//
//	Pytanie : czy ta funkcja moze zostac przerwana przez inny watek ???
//  Bo dostalem raz mase komunikatow
//	(linlamp:5382): GLib-CRITICAL **: g_array_remove_range: assertion `index_ + length <= array->len' failed
//
gboolean store_samples_one_day(GArray *samples, const char *sensor_name)
{
		if (samples->len == 0)
				return TRUE ;
		
		// od poczatku tablicy, nie od konca
		// moze troche kosztowne podczas zwalniania fragmentu tablicy, ale prosto napisac :)
		time_t midnight = get_midnight(g_array_index(samples, T_Sample, 0).czas) ;	
		
		FILE *file = open_data_file_write( midnight, sensor_name) ;
		if (file == NULL)
				return FALSE ;
		
		time_t s_time ;
		float s_val ;
		struct tm *s_t ;	
		char tmp[32] ;
		
		int i = 0 ;
		s_time = g_array_index(samples, T_Sample, i).czas ;
		
		//FIXME: zrobic forem :)
		while (get_midnight(s_time) == midnight) { // czy ten sam dzien ?
				
				s_val = g_array_index(samples, T_Sample, i).wartosc ;
				s_t = localtime(&s_time) ;
				strftime(tmp, sizeof(tmp), "%T", s_t) ;
				fprintf(file ,"%s %f\n", tmp, s_val) ;
				
				i++ ;
				if (i >= samples->len) // to sie chyba nie moze zdarzyc ? no nie wiadomo...
						break ;		// chyba sie jednak zdarzylo :)) - patrz logi z 2009-02-13 linia 7015
				
				s_time = g_array_index(samples, T_Sample, i).czas ;
		} ;
		
		// w tym miejscu co najmniej jedna probka musiala byc zapisana
		g_array_remove_range(samples,0,i) ;
		
		fclose(file) ;
		
		return TRUE ;
		
} ;
//------------------------------------------------------------------------------
gboolean store_samples(GArray *samples, const char *sensor_name)
{
		while (samples->len > 0) {
				if (store_samples_one_day(samples, sensor_name) == FALSE)
						return FALSE ;		// ale tablice i tak trzeba zwolnic
		} ;
		
		return TRUE ;
} ;
//------------------------------------------------------------------------------
FILE *open_data_file_read(time_t s_date, const char* sensor_name)
{
		GString *path = g_string_new(path_data_directory->str) ;
		struct tm *d = localtime(&s_date) ;
		FILE *file = NULL ;
		
		if ( is_dir(path->str) == FALSE) {
				printf ("open_data_file_read: %s nie jest katalogiem\n", path->str) ;
				goto f_exit ;
		} ;
		
		if (path_append_year(path, d) == FALSE ||
				path_append_month(path, d) == FALSE ||
				path_append_day(path, d) == FALSE ) 
		{
				goto f_exit ;
		} ;
		g_string_append_printf(path, "%s.txt", sensor_name) ;
		
		//printf ("ladowanie danych z %s\n", path->str) ;
		
		file = fopen(path->str, "r") ;
		if (file == NULL) {
				//printf ("open_data_file_read: nie mozna otworzyc pliku %s\n", path->str) ;
				//perror("open_data_file_read") ;
		} ;
		//printf ("01\n") ;
		
		f_exit:
				g_string_free(path, TRUE) ;
		return file ;
} ;
//------------------------------------------------------------------------------
gboolean read_sample_from_file (FILE *f, T_Sample *s)
{
		//struct tm time0 = {0,0,0,0,0,0,0,0,-1} ; // nie wiem, co ze zmiana czasu...
		int hour, min, secs ;
		
		if ( fscanf(f, "%d%*c%d%*c%d%f", &hour, &min, &secs, &(s->wartosc)) != 4) {
				//printf ("read_sample_from_file format error\n") ;
				return FALSE ;
		} ;
		
		s->czas = secs + 60*min + 60*60*hour ;
		
		return TRUE ;
} ;
//------------------------------------------------------------------------------
gboolean append_samples_one_day (GArray *samples, const char *s_name, 
																 time_t t_begin, time_t t_end, 
																 time_t t_day) // definiuje dzien
{
																		 FILE *file = open_data_file_read(t_day, s_name) ;
																		 if (file == NULL) {
																				 return TRUE ; // nie ma danych, nic strasznego
																		 } ;
																		 
																		 T_Sample ts ;
																		 while (read_sample_from_file(file, &ts)) {
																				 
																				 ts.czas += get_midnight(t_day) ;
																				 
																				 if ( ts.czas > t_end )
																						 break ;	// zakladam, ze dalsze dane sa jeszcze pozniejsze
																				 
																				 if ( ts.czas > t_begin) {	// dana OK
																						 g_array_append_val(samples,ts) ;
																				 } ;
																		 } ;
																		 
																		 
																		 fclose (file) ;		
																		 return TRUE ;
																 } ;
//------------------------------------------------------------------------------
//
//		Ta funkcja jest wywowylana chyba wylacznie w odpowiedzi na reakcje uzytkownika,
//		tj. chyba jest zabezpieczona ...
//		Na razie uzywana wylacznie podczas dodawania wykresu do wyswietlacza
//
GArray *load_samples (const char *sensor_name, time_t t_begin, time_t t_end) 
{
		GArray *samples = g_array_new(FALSE, TRUE, sizeof(T_Sample)) ;
		time_t t_day = t_begin ;
		
		while (get_midnight(t_day) <= get_midnight(t_end)) {
				append_samples_one_day(samples, sensor_name, t_begin, t_end, t_day) ;
				t_day += 60*60*24 ; // nie wiem tylko, co ze zmiana czasu, ale chyba bedzie OK
		} ;
		
		append_sensor_samples_to_array(samples, sensor_name, t_begin, t_end) ;
		
		return samples ;
} ;
//------------------------------------------------------------------------------
										
