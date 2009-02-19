
#include "t_display.h"
#include "t_panel.h"
#include "popup_menu.h"
#include "t_display_alphanumeric.h"
#include <string.h>
#include <time.h>
#include <math.h>
#include <glib.h>



time_t full_hour (time_t t) ;
void t_display_przerysuj(T_Display *w) ;

//-----------------------------------------------------------------------------
//
//												Legenda wykresu
//
//-----------------------------------------------------------------------------
GList *t_display_get_plots_name_texts (T_Display *d) 
{
		GList *result = NULL ;
		GList *ltmp = d->plots ;
		
		while (ltmp != NULL) {
				result = g_list_append (result, t_plot_get_name_text(ltmp->data, 
																														 GTK_LAYOUT(d->canvas))
																);
				
				ltmp = g_list_next(ltmp) ;
		} ;
		
		return result ;
} ;
//-----------------------------------------------------------------------------
void delete_plots_name_text_list (GList *l)
{
		GList *tmp = l ;
		
		while (tmp != NULL) {
				g_object_unref(tmp->data) ;
				
				tmp = g_list_next(tmp) ;
		} ;
		
		g_list_free(l) ;
} ;
//-----------------------------------------------------------------------------
unsigned int get_total_text_width(GList *list)
{
		unsigned int result = 0 ;
		GList *ltmp = list ;
		int width, height ;
		
		while (ltmp != NULL) {
				
				pango_layout_get_pixel_size ((PangoLayout*)ltmp->data, &width, &height) ;
				result += width ;
				
				ltmp = g_list_next(ltmp) ;
		} ;
		
		return result ;
} ;
//-----------------------------------------------------------------------------
unsigned int get_max_text_height (GList *list)
{
		unsigned int result = 0 ;
		GList *ltmp = list ;
		int width, height ;
		
		while (ltmp != NULL) {
				
				pango_layout_get_pixel_size ((PangoLayout*)ltmp->data, &width, &height) ;
				if (height > result)
						result = height ;
				
				ltmp = g_list_next(ltmp) ;
		} ;
		
		return result ;
} ;
//-----------------------------------------------------------------------------
unsigned int get_max_text_width (GList *list)
{
		unsigned int result = 0 ;
		GList *ltmp = list ;
		int width, height ;
		
		while (ltmp != NULL) {
				
				pango_layout_get_pixel_size ((PangoLayout*)ltmp->data, &width, &height) ;
				if (width > result)
						result = width ;
				
				ltmp = g_list_next(ltmp) ;
		} ;
		
		return result ;
} ;
//-----------------------------------------------------------------------------

void t_display_draw_legend (T_Display *d) 
{
#define LEGEND_POS_GAP 20
#define LEGEND_X_MARGIN 5
#define LEGEND_Y_MARGIN 5
		
		if (d->show_legend == FALSE) {
				d->legend_height = 0 ;
				return ;
		} ;
		
		if (g_list_length(d->plots) == 0){
				d->legend_height = 0 ;
				return ;
		} ;
		
		GList *plot_names = t_display_get_plots_name_texts(d) ;
		
		//int total_width ;
		//total_width = get_total_text_width(plot_names) +			// bez marginesow
		//		(g_list_length(plot_names) -1) * LEGEND_POS_GAP  ;
		
		int row_height = get_max_text_height(plot_names) ;
		int name_width = get_max_text_width(plot_names) ;
		
		d->legend_height = row_height + 2*LEGEND_Y_MARGIN ;		// na razie tylko jeden wiersz :)
		
		int width = d->canvas->allocation.width ;
		int height = d->canvas->allocation.height ;
		
		//int n_rows = ceil(((float) total_width)/((float) width)) ;
		//printf ("t_display_draw_legend: %d wierszy\n", n_rows) ;
		
		int total_width = g_list_length(plot_names) * (name_width + LEGEND_POS_GAP) ;
		
		int x_pos = LEGEND_X_MARGIN + 
				(width - 2*LEGEND_X_MARGIN - total_width)/2 ;
		int y_pos = height - LEGEND_Y_MARGIN - row_height ;
		
		GList *lp = d->plots ;
		while (lp != NULL) {
				
				t_plot_draw_name_text(lp->data, GTK_LAYOUT(d->canvas), x_pos, y_pos) ;
				
				x_pos = x_pos + name_width + LEGEND_POS_GAP ;
				
				lp = g_list_next(lp) ;
		} ;
		
		
		
		delete_plots_name_text_list(plot_names) ;
} ;

//-----------------------------------------------------------------------------
int t_display_inicjuj (T_Display *w)
{
		
		w->time_range = 3600*24 ;			// domyslnie doba
		w->last_time = time(NULL) ;	// koniec wykresu w chwili biezacej
		w->time_step = 2*3600 ;					// domyslnie co 2 godziny
		
		
		w->temp_min = -12 ;
		w->temp_max = 33 ;
		w->temp_step = 10 ;
		
		w->font_size = 10 ;
		
		/*T_Sample pom ;
		GdkColor col ;
		col.blue = 65000 ;
		col.red = 0 ;
		col.green = 0 ;
		
		T_Plot *plot = t_plot_new("",col) ;		// !!! NIGDY MALLOC !!!
		pom.czas = full_hour(w->last_time) - 3600 ; pom.wartosc = 0;
		t_plot_add_point(plot, pom) ;
		pom.czas = full_hour(w->last_time)  ; pom.wartosc = 10;
		t_plot_add_point(plot, pom) ;
		
		t_display_add_plot(w, plot) ;*/
		
		return 0 ;
} ;
//-----------------------------------------------------------------------------
void on_t_display_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
		t_display_przerysuj(get_display_with_layout((GtkLayout*)widget)) ;
} ;
//-----------------------------------------------------------------------------
gint on_t_display_button_press_event(GtkWidget *widget, GdkEventButton *event, 
																		 gpointer data)
{
		if (event->type == GDK_BUTTON_PRESS) {
				switch_display(get_display_with_layout((GtkLayout*)(widget))) ;
				show_menu_display_graphical(event) ;
				//printf ("PRESS button %d at %f, %f\n", event->button, event->x, event->y) ;
				//gtk_menu_popup (menu_1, NULL, NULL, NULL, NULL, event->button, event->time) ;
				/* Tell calling code that we have handled this event; the buck
				* stops here. */
				return TRUE;
		}
		
		/* Tell calling code that we have not handled this event; pass it on. */
		return FALSE;
} ;
//-----------------------------------------------------------------------------
T_Display* t_display_new (void) 
{
		T_Display *d ;
		
		d = g_malloc0 (sizeof(T_Display)) ;
		
		d->type = GRAPHICAL ;
		
		t_display_inicjuj (d) ;
		d->canvas = gtk_layout_new(NULL, NULL) ;
		g_signal_connect (G_OBJECT (d->canvas), "expose-event",
											G_CALLBACK (on_t_display_expose_event), NULL);
		g_signal_connect (G_OBJECT (d->canvas), "button-press-event",
											G_CALLBACK (on_t_display_button_press_event), NULL);
		
		GdkColor color;
		color.red = 0 ;
		color.green = 0 ;
		color.blue = 0 ;
		
		gtk_widget_modify_bg (d->canvas, GTK_STATE_NORMAL, &color);
		//gtk_layout_set_size((GtkLayout*)(d->canvas), 500, 300) ;		// to daje kropke..
		gtk_widget_set_size_request((GtkWidget*)(d->canvas),500,300) ;
		
		return d ;
} ;
//-----------------------------------------------------------------------------
void t_display_destroy (T_Display *w)
{
		gtk_widget_destroy(w->canvas) ;		// juz zostala wywolana gtk_container_remove
		
		GList *tmp  = w->plots ;
		while (tmp != NULL) {
				
				t_plot_destroy(tmp->data) ;
				
				tmp = g_list_next(tmp) ;
		} ;
		g_free (w) ;
} ;
//-----------------------------------------------------------------------------
//
//											Zarzadzanie wykresem
//
//-----------------------------------------------------------------------------
//	Szerokosc i wysokosc przestrzeni przeznaczonej na wykresy (po odjeciu opisow
//	osi i ewentualnie legendy)
int t_display_graph_pixel_width(T_Display *w)
{
		return w->canvas->allocation.width - w->x_pixel_offset ;
} ;
//-----------------------------------------------------------------------------
int t_display_graph_pixel_height(T_Display *w)
{
		return w->canvas->allocation.height - w->y_pixel_offset ;
} ;
//-----------------------------------------------------------------------------

#define LABEL_MARGIN 5		// liczba pikseli dodawanych do szerokosci i wysokosci etykiet

//	Wyznaczenie obszarów wymaganego do umieszczenia opisow osi. Szerokosci obszarow
//	sa obliczane na podstawie zgrubnych oszacowan maksymalnych wielkosci etykiet.
//	Z zalozenia opisy osi pionowej sa z lewej strony, a opisy osi poziomej u dolu.
//
int t_display_calculate_borders (T_Display *w)
{
		PangoLayout *etykieta, *et_temp ;
		int et_szer, et_wys ;
		unsigned int n_labels_x ;
		
		// rozmiar opisu osi pionowej
		et_temp = gtk_widget_create_pango_layout (w->canvas,"-99");	// temperatura
		pango_layout_get_pixel_size (et_temp, &et_szer, &et_wys) ;
		w->x_pixel_offset = et_szer + 2*LABEL_MARGIN ;	// z lewej strony opisy zawsze poziomo
		
		//rozmiar opisu osi poziomej
		etykieta = gtk_widget_create_pango_layout (w->canvas,"99:99"); // czas
		pango_layout_get_pixel_size (etykieta, &et_szer, &et_wys) ;
		
		// czy trzeba obracac opisy osi poziomej
		n_labels_x = w->time_range / w->time_step ;
		if (n_labels_x*(et_szer + LABEL_MARGIN) > t_display_graph_pixel_width(w)) {
				w->y_pixel_offset = et_szer + 2*LABEL_MARGIN ;		// opisy pionowo
				w->x_labels_vert = TRUE ;
		} else {
				w->y_pixel_offset = et_wys + 2*LABEL_MARGIN ;
				w->x_labels_vert = FALSE ;
		} ;
		
		/*printf ("t_display_calculate_borders: x_offset = %d, y_offset = %d, vertical = ",
		/				w->x_pixel_offset, w->y_pixel_offset) ;
		if (w->x_labels_vert == TRUE) 
		printf ("TRUE\n") ;
		else
		printf ("FALSE\n") ;
		*/
		
		w->y_pixel_offset += w->legend_height ;
		
		g_object_unref(etykieta) ;
		g_object_unref(et_temp) ;
		
		return 0 ;
} ;
//-----------------------------------------------------------------------------
double get_units_per_pixel_x (T_Display *w) 
{
		int szer = t_display_graph_pixel_width(w) ;
		double units_per_pixel_x = (double)w->time_range / ((double)szer) ;
		
		//printf ("get_units_per_pixel_x: %f\n", units_per_pixel_x) ;
		return units_per_pixel_x ;
} ;
//-----------------------------------------------------------------------------
double get_units_per_pixel_y (T_Display *w) 
{
		int wys = t_display_graph_pixel_height(w) ;
		int temp_range = w->temp_max - w->temp_min ;
		double units_per_pixel_y = (double)temp_range / ((double)wys) ;
		
		//printf ("get_units_per_pixel_y: %f\n", units_per_pixel_y) ;
		return units_per_pixel_y ;
} ;
//-----------------------------------------------------------------------------
//	Obliczenie wspolrzednych piksela odpowiadajacych konkretnym wartosciom.
int time_to_pixel_x (time_t t, T_Display *w)
{
		// FIXME: moze lepiej round ?
		int result = w->x_pixel_offset + 
				ceil((t - (w->last_time - w->time_range))/get_units_per_pixel_x(w)) ;
		//round((double)((t - (w->last_time - w->time_range))/get_units_per_pixel_x(w))) ;
		
		//printf ("time_to_pixel_x: time = %d, x_pixel = %d\n", t, result) ;
		return result ;
} ;
//-----------------------------------------------------------------------------
//
// 	UWAGA - pamietac, ze aby dostac wspolrzedna y dla ukladu z zerem u DOLU, nalezy
//	wynik temp_to_pixel_y odjac od wysokosci okna
//
int temp_to_pixel_y (double t, T_Display *w)
{
		int result ;
		
		result = w->y_pixel_offset + 
				ceil((t - w->temp_min)/get_units_per_pixel_y(w)) ;
		//round((double)((t - w->temp_min)/get_units_per_pixel_y(w))) ;
		
		//printf ("time_to_pixel_x: time = %d, x_pixel = %d\n", t, result) ;
		return result ;
} ;
//-----------------------------------------------------------------------------
void t_display_recalculate_picture(T_Display *w)
{
		gtk_widget_hide(w->canvas) ;		// Wywolanie metody t_display_przerysuj
		gtk_widget_show(w->canvas) ;		// przez expose_event                  
} ;
//-----------------------------------------------------------------------------
void t_display_set_time_scale(T_Display *w, unsigned int seconds) 
{
		w->time_step = seconds ;
		
		t_display_recalculate_picture(w) ;		
} ;
//-----------------------------------------------------------------------------
void t_display_set_time_range(T_Display *w, unsigned int seconds) 
{
		GList *lst_plot = w->plots;
		
		if (seconds > w->time_range) {
				w->time_range = seconds ;
				
				while (lst_plot != NULL) {
						t_plot_load_data(lst_plot->data, w->last_time - seconds, w->last_time) ;
						lst_plot = g_list_next(lst_plot) ;
				} ;
				
		} else {
				w->time_range = seconds ;
				
				while (lst_plot != NULL) {
						t_plot_remove_tail(lst_plot->data, w->last_time - seconds) ;
						lst_plot = g_list_next(lst_plot) ;
				} ;
		} ;
		
		t_display_recalculate_picture(w) ;
} ;
//-----------------------------------------------------------------------------
void t_display_set_temp_scale(T_Display *w, int scale) 
{
		w->temp_step = scale ;
		
		t_display_recalculate_picture(w) ;
} ;
//-----------------------------------------------------------------------------

void t_display_rysuj_siatke_czas (T_Display *w) ;
void t_display_rysuj_siatke_temp (T_Display *w) ;
void t_display_draw_plots(T_Display *w) ;

//		Narysowanie zawartosci calego okna (z legenda, opisami osi i charakterystykami)
void t_display_przerysuj(T_Display *w)
{
		int szer, wys ;
		double units_per_pixel_x ;
		//double units_per_pixel_y ;
		
		szer = w->canvas->allocation.width ;
		wys = w->canvas->allocation.height ;
		
		units_per_pixel_x = (double)w->time_range / ((double)szer) ;
		
		//PangoLayout *etykieta = gtk_widget_create_pango_layout (w->canvas, "");									
		PangoContext *pc = gtk_widget_get_pango_context (w->canvas) ;
		PangoFontDescription* pfd = pango_context_get_font_description (pc) ;
		//printf ("czcionka %d\n", pango_font_description_get_size(pfd)) ;
		pango_font_description_set_size (pfd, w->font_size * PANGO_SCALE) ;
		//pango_layout_context_changed(etykieta) ;
		
		
		t_display_draw_legend(w) ;
		t_display_calculate_borders(w) ;
		t_display_rysuj_siatke_czas(w) ;
		t_display_rysuj_siatke_temp(w) ;
		t_display_draw_plots(w) ;
		
} ;
//-----------------------------------------------------------------------------
//
//															Czas
//
//-----------------------------------------------------------------------------
time_t full_hour (time_t t)
{
		struct tm *lt ;
		
		lt = localtime(&t) ;
		lt->tm_sec = 0 ;
		lt->tm_min = 0 ;
		
		return mktime(lt) ;
} ;
//-----------------------------------------------------------------------------
time_t prev_hour (time_t t)
{
		struct tm *lt ;
		
		lt = localtime(&t) ;
		lt->tm_sec = 0 ;
		lt->tm_min = 0 ;
		lt->tm_hour -- ;
		
		return mktime(lt) ;
} ;
//-----------------------------------------------------------------------------
int get_hour (time_t t)
{
		struct tm *lt ;
		
		lt = localtime(&t) ;
		return lt->tm_hour ;
} ;
//-----------------------------------------------------------------------------
int get_minute (time_t t)
{
		struct tm *lt ;
		
		lt = localtime(&t) ;
		return lt->tm_min ;
} ;
//-----------------------------------------------------------------------------
int get_seconds (time_t t)
{
		struct tm *lt ;
		
		lt = localtime(&t) ;
		return lt->tm_sec ;
} ;
//-----------------------------------------------------------------------------
int gen_time_label_string (char *string, int str_len, time_t t, T_Display *w)
{
		const char *ROMAN_TABLE = "I.  \0II. \0III.\0IV. \0V.  \0VI. \0VII.\0VIII\0IX. \0X.  \0XI. \0XII.\0" ;
		//const char *ROMAN_TABLE = "sty. \0lut. \0mar. \0kwi. \0maj  \0cze. \0lip. \0sie. \0wrz. \0paź. \0lis. \0gru. \0" ;
		struct tm *lt ;
		
		lt = localtime(&t) ;
		
		if (lt->tm_sec != 0) {	// bardzo dziwna sytuacja, nie powinna sie zdarzyc
				snprintf (&(string[0]), str_len, "%.2d::%.2d", lt->tm_min, lt->tm_sec) ;
		} else if (lt->tm_min != 0) {
				snprintf (&(string[0]), str_len, "%d:%.2d", lt->tm_hour, lt->tm_min) ;
		} else if (lt->tm_hour != 0) {
				snprintf (&(string[0]), str_len, "%d:%.2d", lt->tm_hour, lt->tm_min) ;
		} else if (lt->tm_hour == 0) {
				snprintf (&(string[0]), str_len, "%.2d.%s", lt->tm_mday, 
									&(ROMAN_TABLE[5*lt->tm_mon])) ;
		} else {
				snprintf (&(string[0]), str_len, "?????") ;
		} ;
		
		return 0 ;
} ;
//-----------------------------------------------------------------------------
void t_display_rysuj_siatke_czas (T_Display *w)
{
		int wys ;
		char ctmp [20] ;
		
		wys = w->canvas->allocation.height ;
		
		// kolor siatki i opisow
		GdkGC *gc = gdk_gc_new (GTK_LAYOUT(w->canvas)->bin_window);
		GdkColor color ;
		color.red = 0;
		color.green = 40000;
		color.blue = 0;		
		gdk_gc_set_rgb_fg_color(gc,&color) ;
		
		time_t hours = w->last_time - w->last_time % w->time_step ;
		
		int dzialka_godz = w->time_step / 3600 ;
		
		//printf ("%d %d %d\n",hours, dzialka_godz, w->last_time % w->time_step) ;
		if (w->time_step > 60*60) { // dla dzialki powyzej godziny utrzymac polnoc :)
				hours = full_hour(w->last_time) ;
				while ( ((get_hour(hours) % dzialka_godz) != 0) &&
							 (hours > (w->last_time - w->time_range)) )
				{
						hours -= 3600 ;		// czasami nie pokazuje najpozniejszej godziny dla dzialki 3h
						// pewnie przez jakies dziwne zjawiska z czasem (lata przestepne ?)
				} ;
		} ;
		//printf ("%d\n",hours) ;
		
		int x_hours ;
		PangoLayout *etykieta ;
		int et_szer, et_wys ;
		int label_x, label_y ;
		
		while (hours > (w->last_time - w->time_range))
		{
				x_hours = time_to_pixel_x(hours, w) ;
				
				//snprintf (&(ctmp[0]), 20, "%.2d:%.2d", get_hour(hours), get_minute(hours)) ;
				gen_time_label_string(&(ctmp[0]), 20, hours, w) ;
				etykieta = gtk_widget_create_pango_layout (w->canvas,ctmp);
				
				pango_layout_get_pixel_size (etykieta, &et_szer, &et_wys) ;
				label_x = x_hours-et_szer/2 ;
				label_y = et_wys+LABEL_MARGIN/2 ;
				
				PangoMatrix matrix = PANGO_MATRIX_INIT;
				if (w->x_labels_vert == TRUE) 
				{
						pango_matrix_rotate (&matrix, 90.);
						label_x = x_hours - (et_wys)/2 ;
						label_y = et_szer + LABEL_MARGIN ;
				} ;
				pango_context_set_matrix (pango_layout_get_context(etykieta), &matrix);
				pango_layout_context_changed(etykieta) ;
				
				// rysowanie dzialek i etykiet
				gdk_draw_layout (GTK_LAYOUT(w->canvas)->bin_window, 
												 //w->canvas->style->fg_gc[GTK_WIDGET_STATE (w->canvas)],
												 gc,
												 label_x, wys-label_y - w->legend_height, etykieta) ;
				gdk_draw_line (GTK_LAYOUT(w->canvas)->bin_window,
											 //w->canvas->style->fg_gc[GTK_WIDGET_STATE (w->canvas)],
											 gc,
											 x_hours,wys-w->y_pixel_offset,x_hours,wys-wys) ;
				
				// pogrubienie wybranych dzialek
				if ((get_seconds(hours) == 0) && 
						(get_minute(hours) == 0)  && 
						(get_hour(hours) == 0) )
				{
						gdk_draw_line (GTK_LAYOUT(w->canvas)->bin_window,
													 //w->canvas->style->fg_gc[GTK_WIDGET_STATE (w->canvas)],
													 gc,
													 x_hours+1,wys-w->y_pixel_offset,x_hours+1,wys-wys) ;
				} ;
				
				g_object_unref(etykieta) ;
				
				hours -= w->time_step ;
		} ;
		
		g_object_unref(gc) ;
		
} ;
//------------------------------------------------------------------------------
//
//															Temperatura
//
//-----------------------------------------------------------------------------
void t_display_rysuj_siatke_temp (T_Display *w)
{
		int i_tmp ;
		int tmp_pixel ;
		int wys = w->canvas->allocation.height ;
		char ctmp [16] ;
		PangoLayout *etykieta ;
		int et_szer, et_wys ;
		int label_x, label_y ;
		PangoMatrix matrix = PANGO_MATRIX_INIT;
		
		// kolor siatki i opisow
		GdkGC *gc = gdk_gc_new (GTK_LAYOUT(w->canvas)->bin_window);
		GdkColor color ;
		color.red = 0;
		color.green = 40000;
		color.blue = 0;		
		gdk_gc_set_rgb_fg_color(gc,&color) ;
		
		// najwyzsza wartosc dzialki
		i_tmp = w->temp_max - (w->temp_max % w->temp_step) ;
		
		while (i_tmp >= w->temp_min) {
				
				tmp_pixel = temp_to_pixel_y(i_tmp,w) ;
				
				// rysuj dzialke
				gdk_draw_line (GTK_LAYOUT(w->canvas)->bin_window,
											 //w->canvas->style->fg_gc[GTK_WIDGET_STATE (w->canvas)],
											 gc,
											 w->x_pixel_offset,
											 wys - tmp_pixel,
											 w->canvas->allocation.width,
											 wys- tmp_pixel);
				
				if (i_tmp == 0) {		// ewentualne pogrubienie, tylko dla 0 stopni
						gdk_draw_line (GTK_LAYOUT(w->canvas)->bin_window,
													 //w->canvas->style->fg_gc[GTK_WIDGET_STATE (w->canvas)],
													 gc,
													 w->x_pixel_offset,
													 wys - tmp_pixel-1,
													 w->canvas->allocation.width,
													 wys- tmp_pixel-1);
				} ;
				
				// etykieta				
				snprintf (&(ctmp[0]), 16, "%d", i_tmp) ;
				etykieta = gtk_widget_create_pango_layout (w->canvas,ctmp);
				pango_context_set_matrix (pango_layout_get_context(etykieta), &matrix);
				pango_layout_context_changed(etykieta) ;
				pango_layout_get_pixel_size (etykieta, &et_szer, &et_wys) ;
				
				label_x = LABEL_MARGIN ;
				label_y = tmp_pixel + (et_wys)/2 ;
				
				gdk_draw_layout (GTK_LAYOUT(w->canvas)->bin_window, 
												 //w->canvas->style->fg_gc[GTK_WIDGET_STATE (w->canvas)],
												 gc,
												 label_x, wys-label_y, etykieta) ;
				
				g_object_unref(etykieta) ;
				
				i_tmp -= w->temp_step ;
		} ;
		
		g_object_unref(gc) ;
} ;
//-----------------------------------------------------------------------------
//
//																Wykresy
//
//-----------------------------------------------------------------------------
int t_display_dodaj_wartosc (T_Display *wyswietlacz, const char *sensor_name, T_Sample *wartosc) 
{
		GList *plot_list = wyswietlacz->plots ;
		T_Plot *ptmp ;
		gboolean display_changed = FALSE ;
		
		while (plot_list != NULL) {
				
				ptmp = (T_Plot*) plot_list->data ;
				
				if (strcmp(ptmp->name->str, sensor_name) == 0) {
						
						t_plot_add_point(ptmp, *wartosc, 
														 wyswietlacz->last_time - wyswietlacz->time_range) ;
						display_changed = TRUE ;
				} ;
				
				plot_list = g_list_next(plot_list) ;
		} ;
		
		if (display_changed) {
				if (wartosc->czas > wyswietlacz->last_time) {	
						wyswietlacz->last_time = wartosc->czas ;
				} ;
				t_display_recalculate_picture(wyswietlacz) ;
		} ;
		
		return 0 ; 
} ;
//-----------------------------------------------------------------------------
// plot_area tylko dla wydajnosci
void t_display_draw_plot(T_Display *w, T_Plot *p, GdkRegion *plot_area)
{
		if (p->visible == FALSE)
				return ;
		
		GdkGC *gc ;
		GdkPoint *points ;
		gint npoints ;
		unsigned int i ;
		int wys = w->canvas->allocation.height ;
		
		npoints = p->points->len ;
		//printf ("t_display_draw_plot: GArray size %d\n", p->points->len) ;
		
		if (npoints > 0) {
				
				points = g_malloc(npoints*sizeof(GdkPoint)) ;
				
				// oblicz wspolrzedne pikseli
				for (i=0;i<npoints;i++) {
						points[i].x = time_to_pixel_x( g_array_index(p->points, T_Sample, i).czas, w) ;
						points[i].y = wys - temp_to_pixel_y( g_array_index(p->points, T_Sample, i).wartosc, w) ;
				} ;
				
				// ustaw wlasciwy kolor i ogranicz obszar rysowania charakterystyk
				gc = gdk_gc_new (GTK_LAYOUT(w->canvas)->bin_window);
				gdk_gc_set_rgb_fg_color(gc,&(p->color)) ;
				gdk_gc_set_clip_region(gc,plot_area) ;
				
				if (p->style == LINES)
						
						gdk_draw_lines (GTK_LAYOUT(w->canvas)->bin_window,
														gc,
														points, npoints) ;
				
				else if (p->style == POINTS) {
						
						gdk_draw_points (GTK_LAYOUT(w->canvas)->bin_window,
														 gc,
														 points, npoints) ;
						
						
						
				} else if (p->style == DIAMONDS) {
						
						GdkPoint p_diam[4] ;
						
						for (i=0;i<npoints;i++) {
								p_diam[0].x = points[i].x - 3 ; p_diam[0].y = points[i].y ;	// lewy
								p_diam[1].x = points[i].x ; p_diam[1].y = points[i].y -3 ;	// gorny
								p_diam[2].x = points[i].x + 3 ; p_diam[2].y = points[i].y ;	// prawy
								p_diam[3].x = points[i].x ; p_diam[3].y = points[i].y +3 ;	// dolny
								
								gdk_draw_polygon (GTK_LAYOUT(w->canvas)->bin_window,
																	gc,
																	FALSE,
																	p_diam,
																	4);
						} ;
				} ;
				
				g_object_unref(gc) ;
				g_free(points) ;
		} ;
		
} ;
//-----------------------------------------------------------------------------
void t_display_draw_plots(T_Display *w)
{
		GdkRectangle plot_area ;	// potrzebne, zeby charakterystyki nie zachodzily
		// na opisy osi
		
		plot_area.x = w->x_pixel_offset ;
		plot_area.y = 0 ;
		plot_area.width = t_display_graph_pixel_width(w) ;
		plot_area.height = t_display_graph_pixel_height(w) ;
		
		GdkRegion *plot_area_region = gdk_region_rectangle(&plot_area) ;
		
		GList *plot = w->plots ;
		while (plot != NULL) {
				t_display_draw_plot(w, plot->data, plot_area_region) ;
				plot = g_list_next(plot) ;
		} ;
		
		gdk_region_destroy(plot_area_region) ;
} ;
//-----------------------------------------------------------------------------
int t_display_add_plot(T_Display *w, T_Plot *p) 
{
		//gdk_threads_enter ();	// zeby watek czytajacy czujniki nie zmienil maksymalnego czasu
		// wyglada na to, ze ta funkcja jest wywolywana z wnetrza main_loop i gdk_threads_enter()
		// powoduje deadlock :)
		
		t_plot_load_data(p,w->last_time - w->time_range, w->last_time) ;
		w->plots = g_list_append (w->plots, p) ;
		
		//gdk_threads_leave ();		
		
		return 0 ;
} ;
//-----------------------------------------------------------------------------
int t_display_remove_plot_with_index(T_Display *w, int index) 
{
		w->plots = g_list_remove (w->plots, g_list_nth_data (w->plots, index) ) ;
		return 0 ;
} ;
//-----------------------------------------------------------------------------
//
//					Interfejs do zarzadzania z poziomu okna
//
//-----------------------------------------------------------------------------
// 
//	Dlaczego ta funkcja jest tutaj, a nie w pliku t_plots.c ???
//
GtkListStore *t_display_new_store_with_plots(T_Display *w) 
{
		GtkListStore *store ;
		GtkTreeIter   iter;
		unsigned int i = 0 ;
		
		store = gtk_list_store_new (N_COLUMNS,       /* Total number of columns */
																G_TYPE_INT,
																G_TYPE_STRING,
																G_TYPE_STRING,
																G_TYPE_INT,
																G_TYPE_STRING,
																G_TYPE_INT,
																G_TYPE_BOOLEAN);
		
		GList *plot = w->plots ;
		T_Plot *tmp_plot ;
		const char style_names[4][12] = { "   Linie   ",
				"   Punkty  ",
				"  Diamenty ",
				"    ???    "
		} ;
		//char* style_lines_str =  "  Linie  " ;
		//char* style_points_str = "  Punkty " ;
		const char *style_str ; //= 			 "   ???   " ;
		
		while (plot != NULL) {
				
				tmp_plot = (T_Plot *)(plot->data) ;
				style_str = style_names[tmp_plot->style] ;
				/*if (tmp_plot->style == LINES)
				style_str = style_lines_str ;
				else if (tmp_plot->style == POINTS)
				style_str = style_points_str ;*/
				
				gtk_list_store_append (store, &iter);  
				gtk_list_store_set (store, &iter,
														INDEX, i,
														NAME, tmp_plot->name->str,
														COLOR, gdk_color_to_string(&(tmp_plot->color)),
														N_AVERAGE, tmp_plot->n_average,
														STYLE, style_names[tmp_plot->style],
														N_POINTS, tmp_plot->points->len,
														ALLOW_DISPLAY, tmp_plot->visible,
														-1);			
				
				
				
				plot = g_list_next(plot) ;
				i++ ;
		} ;
		
		//return GTK_TREE_MODEL (store) ;
		return store ;
} ;
//-----------------------------------------------------------------------------
int t_display_save_configuration (config_setting_t *setting, T_Display *d)
{
		config_setting_t *s, *sp = NULL ;
		
		s = config_setting_add(setting, NULL, CONFIG_TYPE_GROUP) ;
		
		sp = config_setting_add(s, "type", CONFIG_TYPE_INT) ;
		config_setting_set_int(sp, d->type);
		sp = config_setting_add(s, "width", CONFIG_TYPE_INT) ;
		config_setting_set_int(sp, d->canvas->allocation.width);
		sp = config_setting_add(s, "height", CONFIG_TYPE_INT) ;
		config_setting_set_int(sp, d->canvas->allocation.height);
		sp = config_setting_add(s, "position_x", CONFIG_TYPE_INT) ;
		config_setting_set_int(sp, d->canvas->allocation.x);
		sp = config_setting_add(s, "position_y", CONFIG_TYPE_INT) ;
		config_setting_set_int(sp, d->canvas->allocation.y);
		
		sp = config_setting_add(s, "time_range", CONFIG_TYPE_INT) ;
		config_setting_set_int(sp, d->time_range);
		sp = config_setting_add(s, "last_time", CONFIG_TYPE_INT) ;
		config_setting_set_int(sp, d->last_time);
		sp = config_setting_add(s, "time_step", CONFIG_TYPE_INT) ;
		config_setting_set_int(sp, d->time_step);
		sp = config_setting_add(s, "temp_max", CONFIG_TYPE_INT) ;
		config_setting_set_int(sp, d->temp_max);
		sp = config_setting_add(s, "temp_min", CONFIG_TYPE_INT) ;
		config_setting_set_int(sp, d->temp_min);
		sp = config_setting_add(s, "temp_step", CONFIG_TYPE_INT) ;
		config_setting_set_int(sp, d->temp_step);
		sp = config_setting_add(s, "font_size", CONFIG_TYPE_INT) ;
		config_setting_set_int(sp, d->font_size);
		sp = config_setting_add(s, "show_legend", CONFIG_TYPE_BOOL) ;
		config_setting_set_bool(sp, d->show_legend);
		
		//		sp = config_setting_add(s, "n_plots", CONFIG_TYPE_INT) ;
		//		config_setting_set_int(sp, g_list_length(d->plots));
		sp = config_setting_add(s, "Plots", CONFIG_TYPE_LIST) ;
		
		GList *tmp = d->plots ;
		while (tmp != NULL) {
				
				t_plot_save_configuration(sp, (T_Plot*)tmp->data) ;
				
				tmp = g_list_next(tmp) ;
		} ;
		
		return 0 ;
} ;
//-----------------------------------------------------------------------------
T_Display *t_display_new_from_configuration(config_setting_t *disp)
{
		T_Display *d ;
		config_setting_t *s, *plot_list, *p ;
		unsigned width = 200, height = 200, i ;
		T_DisplayType type = GRAPHICAL ;
		
		s = config_setting_get_member(disp, "type") ;
		if (s != NULL) type = config_setting_get_int(s) ;
		
		if (type == ALPHANUMERIC)
				d = t_display_new_alphanumeric() ;
		else
				d = t_display_new() ;
		
		s = config_setting_get_member(disp, "width") ;
		if (s != NULL) width = config_setting_get_int(s) ;
		s = config_setting_get_member(disp, "height") ;
		if (s != NULL) height = config_setting_get_int(s) ;
		gtk_widget_set_size_request((GtkWidget*)(d->canvas),width,height) ;
		
		s = config_setting_get_member(disp, "time_range") ;
		if (s != NULL) d->time_range = config_setting_get_int(s) ;
		//s = config_setting_get_member(disp, "last_time") ;	// to chyba usunac 
		//d->last_time = config_setting_get_int(s) ;
		s = config_setting_get_member(disp, "time_step") ;
		if (s != NULL) d->time_step = config_setting_get_int(s) ;
		s = config_setting_get_member(disp, "temp_max") ;
		if (s != NULL) d->temp_max = config_setting_get_int(s) ;
		s = config_setting_get_member(disp, "temp_min") ;
		if (s != NULL) d->temp_min = config_setting_get_int(s) ;
		s = config_setting_get_member(disp, "temp_step") ;
		if (s != NULL) d->temp_step = config_setting_get_int(s) ;
		d->font_size = 10 ;
		s = config_setting_get_member(disp, "font_size") ;
		if (s != NULL) d->font_size = config_setting_get_int(s) ;
		d->show_legend = TRUE ;
		s = config_setting_get_member(disp, "show_legend") ;
		if (s != NULL) d->show_legend = config_setting_get_bool(s) ;
		
		plot_list = config_setting_get_member(disp, "Plots") ;
		i=0 ;
		while ((p = config_setting_get_elem(plot_list,i)) != NULL) {
				
				t_display_add_plot(d,t_plot_new_from_configuration(p)) ;
				
				i++ ;
		} ;		
		
		return d ;
} ;
//-----------------------------------------------------------------------------
