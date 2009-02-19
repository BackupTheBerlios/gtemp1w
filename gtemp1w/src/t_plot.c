
#include "t_plot.h"
#include "sample_load_save.h"
#include <limits.h>
#include <math.h>


#define MAX_POINTS_EXCESS 0			// liczba punktow nadmiarowych, ktora jeszcze nie
// nie powoduja usuniecia ich z tablicy


//------------------------------------------------------------------------------
void t_plot_set_visible(T_Plot *p, gboolean visible) 
{
		p->visible = visible ;
} ;
//------------------------------------------------------------------------------
void t_plot_set_n_average(T_Plot *p, unsigned n_average) 
{
		p->n_average = n_average ;
} ;
//------------------------------------------------------------------------------
void t_plot_set_style(T_Plot *p, T_PlotStyle style) 
{
		p->style = style ;
} ;
//------------------------------------------------------------------------------
void t_plot_set_color(T_Plot *p, GdkColor color)
{
		p->color.red		= color.red ;
		p->color.green	= color.green ;
		p->color.blue		= color.blue ;
} ;
//------------------------------------------------------------------------------
T_Plot *t_plot_new(const char *name, GdkColor color, gboolean visible,
									 unsigned n_average, T_PlotStyle style)
{
		T_Plot *r = g_malloc0(sizeof(T_Plot)) ;
		
		r->name = g_string_new (name) ;
		r->points		= g_array_new  (FALSE, TRUE, sizeof(T_Sample)) ;
		t_plot_set_color(r, color) ;
		r->visible = visible ;
		r->n_average = n_average ;
		r->style = style ;
		
		return r ;
} ;
//------------------------------------------------------------------------------
void t_plot_destroy(T_Plot *p) 
{
		g_array_free  (p->points, TRUE) ;
		g_string_free (p->name, TRUE) ;
		g_free(p) ;
} ;
//------------------------------------------------------------------------------
//
//	W tej funkcji sa ukryte dwa zalozenia:
//		1. najstarsze punkty sa na poczatku tablicy
//		2. punkty sa posortowane wzgledem czasu
//
void t_plot_remove_tail (T_Plot *plot, time_t plot_begin)
{
		T_Sample *point ;
		unsigned i, n_excess = 0 ;
		
		for (i=0; i < plot->points->len ; i++) {
				
				point = &g_array_index(plot->points, T_Sample, i) ;
				if (point->czas < plot_begin) {
						n_excess ++ ;
				} else {
						break ;
				} ;
		} ;
		
		if (n_excess > 0 && n_excess >= MAX_POINTS_EXCESS) {
				//printf ("t_plot_remove_tail: usuwam %d punktow\n", n_excess) ;
				g_array_remove_range(plot->points, 0, n_excess) ;
		} ;
} ;
//------------------------------------------------------------------------------
void t_plot_add_point(T_Plot *plot, T_Sample point, time_t t_begin) 
{
		t_plot_remove_tail(plot, t_begin) ;
		g_array_append_val(plot->points,point) ;
} ;
//------------------------------------------------------------------------------
gboolean t_plot_load_data (T_Plot *plot, time_t t_begin, time_t t_end) 
{
		//printf ("t_plot_load_data\n") ;
		
		g_array_free  (plot->points, TRUE) ;
		plot->points = load_samples(plot->name->str, t_begin, t_end) ;
		
		return TRUE ;
} ;
//------------------------------------------------------------------------------
time_t t_plot_min_time(T_Plot *p) 
{
		int	i, npoints = p->points->len ;
		time_t min_time = INT_MAX ;
		
		for (i=0;i<npoints;i++) {		// jesli tablica pusta, to sie nie wykona :)
				if (min_time > g_array_index(p->points, T_Sample, i).czas)
						min_time = g_array_index(p->points, T_Sample, i).czas ;
		} ;
		
		return min_time ;
}
//------------------------------------------------------------------------------
float t_plot_min_val(T_Plot *p) 
{
		int	i, npoints = p->points->len ;
		float min_val = HUGE ;
		
		for (i=0;i<npoints;i++) {		// jesli tablica pusta, to sie nie wykona :)
				if (min_val > g_array_index(p->points, T_Sample, i).wartosc)
						min_val = g_array_index(p->points, T_Sample, i).wartosc ;
		} ;
		
		return min_val ;
} ;
//-----------------------------------------------------------------------------
PangoLayout *t_plot_get_name_text(T_Plot *plot, GtkLayout *layout) 
{
		
		GString *string = g_string_new(plot->name->str) ;
		PangoLayout *etykieta = gtk_widget_create_pango_layout ((GtkWidget*)layout,
																														string->str);
		g_string_free(string, TRUE) ;
		
		
		return etykieta ;
} ;
//-----------------------------------------------------------------------------
void t_plot_draw_name_text(T_Plot *plot, GtkLayout *layout, int x, int y)
{
		GdkGC *gc = gdk_gc_new (layout->bin_window);
		gdk_gc_set_rgb_fg_color(gc,&(plot->color)) ;
		
		PangoLayout *etykieta = t_plot_get_name_text(plot, layout) ;
		
		gdk_draw_layout (layout->bin_window, 
										 gc,
										 x, y, etykieta) ;		
		
		g_object_unref(gc) ;	
		g_object_unref(etykieta) ;
} ;
//-----------------------------------------------------------------------------
void t_plot_draw_last_value_text(T_Plot *plot, GtkLayout *layout, int x, int y)
{
		GdkGC *gc = gdk_gc_new (layout->bin_window);
		gdk_gc_set_rgb_fg_color(gc,&(plot->color)) ;
		
		GString *string = g_string_new("") ;
		if (plot->points->len > 0)
				g_string_append_printf(string, "%7.4f",
															 (double)(g_array_index(plot->points, T_Sample, 
																											plot->points->len-1).wartosc)
															 ) ;
		else
				g_string_append_printf(string, "--------") ;
		
		
		PangoLayout *etykieta = gtk_widget_create_pango_layout ((GtkWidget*)layout,
																														string->str);
		
		gdk_draw_layout (layout->bin_window, 
										 gc,
										 x, y, etykieta) ;		
		
		
		g_object_unref(etykieta) ;
		g_string_free(string, TRUE) ;
		g_object_unref(gc) ;
} ;
//-----------------------------------------------------------------------------
//------------------------------------------------------------------------------
int t_plot_save_configuration (config_setting_t *setting, T_Plot *p)
{
		config_setting_t *s, *sp ;
		
		s = config_setting_add(setting, NULL, CONFIG_TYPE_GROUP) ;
		
		sp = config_setting_add(s, "name", CONFIG_TYPE_STRING) ;
		config_setting_set_string(sp, p->name->str);
		sp = config_setting_add(s, "color", CONFIG_TYPE_STRING) ;
		config_setting_set_string(sp, gdk_color_to_string(&(p->color)));
		sp = config_setting_add(s, "visible", CONFIG_TYPE_BOOL) ;
		config_setting_set_bool(sp, p->visible);
		sp = config_setting_add(s, "n_average", CONFIG_TYPE_INT) ;
		config_setting_set_int(sp, p->n_average);
		sp = config_setting_add(s, "style", CONFIG_TYPE_INT) ;
		config_setting_set_int(sp, p->style);
		
		return 0 ;
} ;
//------------------------------------------------------------------------------
T_Plot *t_plot_new_from_configuration(config_setting_t *p_conf)
{
		config_setting_t *s ;
		GdkColor col ;
		gboolean visible = TRUE ;
		unsigned n_average = 0 ;
		T_PlotStyle style = LINES ;
		
		s = config_setting_get_member(p_conf, "visible") ;
		if (s != NULL) visible = config_setting_get_bool(s) ;
		s = config_setting_get_member(p_conf, "color") ;
		if (s != NULL) gdk_color_parse(config_setting_get_string(s), &col) ;
		s = config_setting_get_member(p_conf, "n_average") ;
		if (s != NULL) n_average = config_setting_get_int(s) ;
		s = config_setting_get_member(p_conf, "style") ;
		if (s != NULL) style = config_setting_get_int(s) ;
		
		s = config_setting_get_member(p_conf, "name") ;
		
		if (s != NULL)
				return t_plot_new(config_setting_get_string(s), col, visible, n_average, style) ;
		
		return t_plot_new("default_sensor_name", col, visible, n_average, style) ;
} ;

//------------------------------------------------------------------------------
