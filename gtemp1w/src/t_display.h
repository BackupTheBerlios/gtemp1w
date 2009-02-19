#ifndef __T_DISPLAY_H__
#define __T_DISPLAY_H__

#include <glib.h>
#include <time.h>
#include <gtk/gtk.h>
#include <libconfig.h>

#include "t_sensor.h"
#include "t_plot.h"

typedef enum {
		GRAPHICAL,
		ALPHANUMERIC
} T_DisplayType ;

// draw plane with coordinate system and plots
/*
Wyswietlacz (T_Display) jest prostokatem zawierajacym siatke dzialek wraz
z opisami oraz zbior charakterystyk dla roznych czujnikow. W przyszlosi 
nalezy dodac jeszcze legende.
*/
typedef struct {
		
		T_DisplayType	type ;
		
		GtkWidget		*canvas ;		// obszar, na ktorym wyswietlane sa wykresy, legenda, opisy osi
		
		//
		//									Uklad wspolrzednych
		//
		int				time_range ;	// zakres wartosci na osi czasu, chyba
		// w sekundach, ale ograniczyc do kilku
		// wybranych wartosci, np. godzina, doba,
		// tydzien, miesiac, rok itp....
		// Odpowiednik "czasu trwania pomiaru"
		// z Lampomittari :)))
		
		time_t		last_time ;		// Najpozniejszy punkt na wykresie, czyli 
		// z reguly czas ostatniego pomiaru. 
		// Zachowanie podobne jak w Lamp...., czyli
		// wprowadzanie wartosci od prawej strony
		// i usuwanie najstarszych.
		
		int				time_step ;		// Dzialka czasu, w sekundach, ale tylko
		// wybrane wartosci. Siatka powinna byc 
		// rysowana na calkowitych wielokrotnosciach
		// czasu od pelnych godzin, dni itd...
		
		
		int				temp_max ;		// Maksymalna mozliwa do wyswietlenia 
		// wartosc temperatury
		int				temp_min ;		// Minimalna mozliwa do wyswietlenia 
		// wartosc temperatury
		int				temp_step ;		// Tutaj prosto - 1,2,5,10 stopni i juz :)
		// Moze dodac jeszcze 0 jako brak siatki ?
		
		gboolean	show_legend ;	// czy rysowac legende ?
		
		
		int 			x_pixel_offset ;	// ramka na etykiety osi o rozmiarze podanym 
		int				y_pixel_offset ;	// w pikselach
		char			x_labels_vert ;		// opisy osi x pionowo/poziomo
		int				legend_height ;		// obszar od dolu zarezerwowany na legende
		
		
		int				font_size ; // tylko dla alfanumerycznych
		
		//
		//									W Y K R E S Y 
		//
		
		GList				*plots ;			// elementami listy sa T_Plot
		
		
} T_Display ;

/*
Docelowo ustawienie parametrow na podstawie pliku konfiguracyjnego.
*/
int t_display_inicjuj (T_Display *w) ;

T_Display* t_display_new (void) ;
void t_display_destroy (T_Display *w) ;

/*
Dopisanie pomiaru do wszystkich charakterystyk zwiazanych z przekazanym czujnikiem
*/
int t_display_dodaj_wartosc (T_Display *wyswietlacz, const char *sensor_name, T_Sample *wartosc) ;

// ta jest chyba najwazniejsza...
void t_display_recalculate_picture(T_Display *w) ;

void t_display_set_time_scale(T_Display *w, unsigned int seconds) ;
void t_display_set_time_range(T_Display *w, unsigned int seconds) ;
void t_display_set_temp_scale(T_Display *w, int scale) ;

int t_display_add_plot(T_Display *w, T_Plot *p) ;
int t_display_remove_plot_with_index(T_Display *w, int index) ;

enum {
		INDEX,
		NAME,
		COLOR,
		N_AVERAGE,
		STYLE,
		N_POINTS,
		ALLOW_DISPLAY,
		N_COLUMNS
} ;

// wylacznie dla okna zarzadzajacego wykresami
GtkListStore *t_display_new_store_with_plots(T_Display *w) ;


int t_display_save_configuration (config_setting_t *setting, T_Display *d) ;
T_Display *t_display_new_from_configuration(config_setting_t *disp) ;

#endif
