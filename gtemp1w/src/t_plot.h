#ifndef __T_PLOT_H__
#define __T_PLOT_H__

#include <gdk/gdk.h>
#include <libconfig.h>
#include <gtk/gtk.h>



#include "t_sensor.h"

typedef enum {
		LINES,
		POINTS,
		DIAMONDS
} T_PlotStyle ;

typedef struct {
		
		GString *name ;
		
		GdkColor	color ;
		
		gboolean visible ;
		
		unsigned n_average ; // liczba probek uśrednianych, 0 oznacza brak uśredniania
		
		T_PlotStyle style ;	// sposob wyswietlania wykresu
		
		
		GArray	*points ;			// elementy typu T_Sample
		
} T_Plot ;

T_Plot *t_plot_new(const char *name, GdkColor color, gboolean visible,
									 unsigned n_average, T_PlotStyle style) ;
void t_plot_destroy(T_Plot *p) ;

// dodanie jednego pomiaru do wykresu
void t_plot_add_point(T_Plot *plot, T_Sample point, time_t t_begin) ;
// wczytanie danych dla zadanego zakresu ze wszystkich mozliwych zrodel
gboolean t_plot_load_data (T_Plot *plot, time_t t_begin, time_t t_end) ;

void t_plot_remove_tail (T_Plot *plot, time_t plot_begin);

// minimalne wartosci dla jednej krzywej
time_t t_plot_min_time(T_Plot *p) ;
float t_plot_min_val(T_Plot *p) ;

void t_plot_set_color(T_Plot *p, GdkColor color) ;
void t_plot_set_visible(T_Plot *p, gboolean visible) ;
void t_plot_set_n_average(T_Plot *p, unsigned n_average) ;
void t_plot_set_style(T_Plot *p, T_PlotStyle style) ;

PangoLayout *t_plot_get_name_text(T_Plot *plot, GtkLayout *layout) ;
void t_plot_draw_name_text(T_Plot *plot, GtkLayout *layout, int x, int y) ;

void t_plot_draw_last_value_text(T_Plot *plot, GtkLayout *layout, int x, int y) ;

int t_plot_save_configuration (config_setting_t *setting, T_Plot *p) ;
T_Plot *t_plot_new_from_configuration(config_setting_t *p_conf) ;

#endif
