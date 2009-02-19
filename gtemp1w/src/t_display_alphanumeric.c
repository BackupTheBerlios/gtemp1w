#include "t_display_alphanumeric.h"
#include "t_panel.h"
#include "popup_menu.h"
#include "t_plot.h"



void on_t_display_alphanumeric_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data) ;
gint on_t_display_alphanumeric_button_press_event(GtkWidget *widget, GdkEventButton *event, 
																									gpointer data) ;
void show_alpanumeric_menu(GdkEventButton *event) ;

//------------------------------------------------------------------------------
T_Display* t_display_new_alphanumeric (void) 
{
		T_Display *d ;
		
		d = g_malloc0 (sizeof(T_Display)) ;
		
		d->type = ALPHANUMERIC ;
		d->time_range = 10 ;
		
		//t_display_inicjuj (d) ; // to zbedne
		
		d->canvas = gtk_layout_new(NULL, NULL) ;
		g_signal_connect (G_OBJECT (d->canvas), "expose-event",
											G_CALLBACK (on_t_display_alphanumeric_expose_event), NULL);
		g_signal_connect (G_OBJECT (d->canvas), "button-press-event",
											G_CALLBACK (on_t_display_alphanumeric_button_press_event), NULL);
		
		GdkColor color;
		color.red = 0 ;
		color.green = 0 ;
		color.blue = 0 ;
		
		gtk_widget_modify_bg (d->canvas, GTK_STATE_NORMAL, &color);
		//gtk_layout_set_size((GtkLayout*)(d->canvas), 500, 300) ;		// to daje kropke..
		gtk_widget_set_size_request((GtkWidget*)(d->canvas),100,50) ;
		
		return d ;
} ;
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int t_display_get_longest_plot_name_length(T_Display *d)
{
		GList *l = d->plots ;
		int length = 0 ;
		
		while (l != NULL) {
				if (((T_Plot*)(l->data))->name->len > length) // zla miara dla UTF
						length = ((T_Plot*)(l->data))->name->len ;
				
				l = g_list_next(l) ;
		} ;
		
		return length ;
} ;
//------------------------------------------------------------------------------
// plot_area tylko dla wydajnosci
//~ void t_display_alphanumeric_draw_plot(T_Display *w, T_Plot *p)
//~ {
		//~ if (p->visible == FALSE)
				//~ return ;
		
		//~ GdkGC *gc ;
		//~ gint npoints ;
		
		//~ npoints = p->points->len ;
		//~ /*
				//~ printf ("t_display_draw_plot: GArray size %d\n", p->points->len) ;
		//~ unsigned i ;
		//~ for (i=0;i<npoints;i++) {
				//~ printf ("%d %d %f\n", i, g_array_index(p->points, T_Sample, i).czas,
										//~ g_array_index(p->points, T_Sample, i).wartosc) ;
				//~ } ;*/
				
				//~ if (npoints > 0) {
						
						//~ int longest_name = t_display_get_longest_plot_name_length(w) ;
						
						//~ // ustaw wlasciwy kolor i ogranicz obszar rysowania charakterystyk
								//~ gc = gdk_gc_new (GTK_LAYOUT(w->canvas)->bin_window);
						//~ gdk_gc_set_rgb_fg_color(gc,&(p->color)) ;
						//~ //gdk_gc_set_clip_region(gc,plot_area) ;
						
						//~ char ctmp [128] ;
						//~ PangoLayout *etykieta ;
						
						//~ snprintf (&(ctmp[0]), sizeof(ctmp), "%-*s    %7.4f", longest_name, p->name->str, 
													//~ (double)(g_array_index(p->points, T_Sample, p->points->len-1).wartosc)) ;
						//~ etykieta = gtk_widget_create_pango_layout (w->canvas,ctmp);
						
						//~ // rozmiar czcionki
								//~ //PangoContext* et_context = pango_layout_get_context (etykieta);
						
						
						//~ //pango_context_set_matrix (pango_layout_get_context(etykieta), &matrix);
						//~ //pango_layout_context_changed(etykieta) ;
						//~ //pango_layout_get_pixel_size (etykieta, &et_szer, &et_wys) ;
						
						//~ //label_x = LABEL_MARGIN ;
						//~ //label_y = tmp_pixel + (et_wys)/2 ;
						
						//~ gdk_draw_layout (GTK_LAYOUT(w->canvas)->bin_window, 
																 //~ //w->canvas->style->fg_gc[GTK_WIDGET_STATE (w->canvas)],
																 //~ gc,
																 //~ 0, 0, etykieta) ;
						
						//~ g_object_unref(etykieta) ;
						
						//~ g_object_unref(gc) ;
						
						//~ } ;
		
		//~ } ;
//-----------------------------------------------------------------------------
/*GString *t_plot_last_data_string(T_Plot *p, int name_len) 
{
		GString *result = g_string_new("") ;
		char stmp[128] ;
		
		if (p->points->len > 0)
				snprintf (stmp, sizeof(stmp), "%-*s        %7.4f", name_len, p->name->str, 
									(double)(g_array_index(p->points, T_Sample, p->points->len-1).wartosc)) ;		
		else
				snprintf (stmp, sizeof(stmp), "%-*s    --------", name_len, p->name->str) ;		
		
		g_string_append(result, stmp) ;
		
		return result ;
} ;*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/*void t_plot_draw_last_value_text(T_Plot *plot, GtkLayout *layout, int name_len,
																	 int x, int y)
{
		GdkGC *gc = gdk_gc_new (layout->bin_window);
		gdk_gc_set_rgb_fg_color(gc,&(plot->color)) ;
		
		GString *string = t_plot_last_data_string(plot, name_len) ;
		PangoLayout *etykieta = gtk_widget_create_pango_layout ((GtkWidget*)layout,
																														string->str);
		
		gdk_draw_layout (layout->bin_window, 
										 gc,
										 x, y, etykieta) ;		
		
		
		g_object_unref(etykieta) ;
		g_string_free(string, TRUE) ;
		g_object_unref(gc) ;
} ;*/
//-----------------------------------------------------------------------------
/*void t_plot_draw_name_text(T_Plot *plot, GtkLayout *layout, int x, int y)
{
		GdkGC *gc = gdk_gc_new (layout->bin_window);
		gdk_gc_set_rgb_fg_color(gc,&(plot->color)) ;
		
		GString *string = g_string_new(plot->name->str) ;
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
} ;*/
//-----------------------------------------------------------------------------
void t_display_alphanumeric_draw_plots(T_Display *w)
{
#define Y_OFFSET 2
#define X_OFFSET 2
#define X_GAP 20
		
		int name_height, name_width, value_width, value_height ;
		int longest_name = t_display_get_longest_plot_name_length(w) ;
		
		
		// wyznaczenie rozmiaru napisow .... nie lubie pango
				GString *str = g_string_new("") ;
		int i ;
		for (i=0;i<longest_name;i++) {
				g_string_append(str,"W") ;
		} ;
		g_string_append(str,"j") ;
		PangoLayout *etykieta = gtk_widget_create_pango_layout (w->canvas, str->str);									
		PangoContext *pc = gtk_widget_get_pango_context (w->canvas) ;
		PangoFontDescription* pfd = pango_context_get_font_description (pc) ;
		//printf ("czcionka %d\n", pango_font_description_get_size(pfd)) ;
		pango_font_description_set_size (pfd, w->font_size * PANGO_SCALE) ;
		pango_layout_context_changed(etykieta) ;
		//pango_context_set_font_description  (pc, pfd) ;
		
		pango_layout_get_pixel_size (etykieta, &name_width, &name_height) ;						
		g_object_unref(etykieta) ;
		g_string_set_size(str,0) ;
		
		g_string_append_printf(str,"%7.4f", -99.9999) ;
		etykieta = gtk_widget_create_pango_layout (w->canvas, str->str);									
		pango_layout_get_pixel_size (etykieta, &value_width, &value_height) ;						
		g_object_unref(etykieta) ;
		g_string_free(str, TRUE) ;
		
		int y = Y_OFFSET ;
		GList *plot = w->plots ;
		while (plot != NULL) {
				
				t_plot_draw_name_text(plot->data, GTK_LAYOUT(w->canvas), X_OFFSET, y) ;
				t_plot_draw_last_value_text(plot->data, GTK_LAYOUT(w->canvas), 
																		X_OFFSET + name_width + X_GAP, y) ;
				
				y = y + name_height + Y_OFFSET ;
				plot = g_list_next(plot) ;
		} ;
		
		if (w->plots != NULL) {
				gtk_widget_set_size_request(w->canvas,name_width+ 2*X_OFFSET + X_GAP + value_width, 
																		g_list_length(w->plots) * (name_height + Y_OFFSET) + Y_OFFSET) ;
		} ;
		
} ;
//------------------------------------------------------------------------------
void t_display_alphanumeric_redraw(T_Display *w)
{
		t_display_alphanumeric_draw_plots(w) ;
} ;
//------------------------------------------------------------------------------
void on_t_display_alphanumeric_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
		t_display_alphanumeric_redraw(get_display_with_layout((GtkLayout*)widget)) ;
} ;
//------------------------------------------------------------------------------
gint on_t_display_alphanumeric_button_press_event(GtkWidget *widget, GdkEventButton *event, 
																									gpointer data)
{
		if (event->type == GDK_BUTTON_PRESS) {
				switch_display(get_display_with_layout((GtkLayout*)(widget))) ;
				show_alpanumeric_menu(event) ;
				
				/* Tell calling code that we have handled this event; the buck
						* stops here. */
						return TRUE;
		}
		
		/* Tell calling code that we have not handled this event; pass it on. */
				return FALSE;
} ;
//------------------------------------------------------------------------------
void show_alpanumeric_menu(GdkEventButton *event)
{
		show_menu_display_alphanumeric(event) ;
} ;
