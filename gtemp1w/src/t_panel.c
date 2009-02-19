#include "t_panel.h"
#include "t_display_alphanumeric.h"
#include "configuration.h"

#include <math.h>


// podloze calego panelu
GtkLayout *panel_background ;

// lista wyswietlaczy na panelu, kazdy wyswietlacz typu T_Display
//
GList *displays = NULL ;	


// menu
//
GtkMenu *panel_menu ;
//GtkMenuItem *panel_menu_item_add_chart ;

int x,y ;		// Wspolrzedne pierwszego klikniecia na panelu uzywane do dodania wykresu, 
// poniewaz klikniecie uruchamia menu.

//T_Display display ;		// tymczasowo
//GtkLayout *box;


//------------------------------------------------------------------------------
int register_panel (GladeXML *xml)
{
		//box = (GtkLayout *)glade_xml_get_widget (xml, "plotno1");
		panel_background = (GtkLayout *)glade_xml_get_widget (xml, "fixed1");
		panel_menu = (GtkMenu *) glade_xml_get_widget(xml, "panel_popup_menu") ;
		//panel_menu_item_add_chart = (GtkMenuItem *) glade_xml_get_widget (xml, "panel_menu_item_add_chart") ;
		
		/*
		GdkColor color;
		color.red = 383;
		color.green = 383;
		color.blue = 383;
		*/
		//gtk_widget_modify_bg ((GtkWidget*)box, GTK_STATE_NORMAL, &color);
		//display.canvas = (GtkWidget*)box ;
		//t_display_inicjuj (&display) ;
		return 0 ;
} ;
//------------------------------------------------------------------------------
void t_panel_clear(void)
{
		GList *disp = displays ;
		
		while (disp != NULL) {
				t_display_destroy ((T_Display*) disp->data);
				
				disp = g_list_next(disp) ;
		} ;
		
		g_list_free (displays) ;
		displays = NULL ;
} ;
//------------------------------------------------------------------------------
//
// 	Oblicz istniejace granice tak, aby zmiescily sie wszystkie dotychczasowe
//	wyswietlacze. W zasadzie ta funkcja moze byc nadmiarowa, bo rozmiar panelu
//	powinien byc uaktualniany po kazdej zmianie wyswietlaczy, ale na poczatek
//	bedzie chyba bezpieczniej i pewniej...
//
//	UWAGA - nie do konca rozumiem, jak dziala mechanizm zmiany rozmiaru.
//	Dlaczego nie mozna wywolac tej funkcji bezposrednio po gtk_layout_put() ???
//
//	chyba juz rozumiem :)   ->   gtk_container_resize_children (GTK_CONTAINER(fixed1)) ;
//
void t_panel_recalculate_size(void) 
{
		gtk_container_resize_children (GTK_CONTAINER(panel_background)) ;
		
		guint width=0, height=0, new_w, new_h ;
		
		//width = ((GtkWidget *)panel_background)->allocation.width ;
		//height = ((GtkWidget *)panel_background)->allocation.height ;
		//printf ("old size w/h (%d, %d) at (%d, %d)\n", width, height,
		//				((GtkWidget *)panel_background)->allocation.x, 
		//				((GtkWidget *)panel_background)->allocation.y) ;		
		
		GtkWidget *ltmp ;
		GList *tmp = displays ;
		while (tmp != NULL) {
				
				ltmp = (GtkWidget*)(((T_Display*)(tmp->data))->canvas) ;
				
				new_w = ltmp->allocation.width + ltmp->allocation.x ;
				new_h = ltmp->allocation.height + ltmp->allocation.y ;
				
				if (width < new_w) width = new_w ;
				if (height < new_h) height = new_h ;
				
				//printf ("checking size (%d, %d) at (%d, %d)\n",ltmp->allocation.width,
				//				ltmp->allocation.height, ltmp->allocation.x, ltmp->allocation.y) ;
				
				tmp = g_list_next(tmp) ;
		} ;		
		
		//printf ("setting new width, height to (%d, %d)\n", width, height) ;
		gtk_layout_set_size(panel_background, width, height) ;
} ;
//------------------------------------------------------------------------------
// Ta funkcja moze byc prostsza, jesli rysowanie widgetow bedzie w obsludze expose-event
// dla panel_background, tylko ze chyba nie bedzie to zbyt wydajne...
int t_panel_add_display(T_Display *d, unsigned x, unsigned y)
{
		gtk_layout_put(panel_background,d->canvas,x,y) ;
		gtk_widget_show(GTK_WIDGET(d->canvas)) ;
		
		displays = g_list_append(displays, d) ;
		
		// moze dodac t_panel_recalculate_size po kazdym nowym wyswietlaczu ?
		
		return 0 ;
} ;
//------------------------------------------------------------------------------
gboolean on_fixed1_button_press_event (GtkWidget *widget, GdkEventButton *event, 
																			 gpointer user_data)
{
		
		if (event->type == GDK_BUTTON_PRESS) {
				x = ceil(event->x) ; y = ceil(event->y) ;
				gtk_menu_popup (panel_menu, NULL, NULL, NULL, NULL, event->button, event->time) ;
				/* Tell calling code that we have handled this event; the buck
				* stops here. */
				return TRUE;
		} ;
		
		return FALSE ;
} ;
//------------------------------------------------------------------------------
T_Display* get_display_with_layout (GtkLayout *l) 
{
		GList *tmp ;
		
		tmp = displays ;
		while (tmp != NULL) {
				if (((T_Display*)(tmp->data))->canvas == (GtkWidget*)l) {
						return (T_Display*)(tmp->data) ;
				} ;
				
				tmp = g_list_next(tmp) ;
		} ;
		printf ("get_display_with_layout: no display found.\n") ;
		return NULL ;
} ;
//------------------------------------------------------------------------------
void on_panel_menu_item_add_chart_activate (GtkCheckMenuItem *checkmenuitem, gpointer user_data) 
{
		t_panel_add_display(t_display_new(), x, y) ;
		t_panel_recalculate_size() ;
} ;//------------------------------------------------------------------------------
void on_panel_menu_item_add_alphanumeric_activate (GtkCheckMenuItem *checkmenuitem, gpointer user_data) 
{
		t_panel_add_display(t_display_new_alphanumeric(), x, y) ;
		t_panel_recalculate_size() ;
} ;
//------------------------------------------------------------------------------
void t_panel_remove_display (T_Display *d) 
{	
		// ponizsza linia zbedna, bo gtk_widget_destroy podobno niszczy wszystkie odwolania
		//gtk_container_remove(GTK_CONTAINER(panel_background), d->canvas) ; 
		
		t_display_destroy(d) ;
		displays = g_list_remove(displays, d) ;
		
		t_panel_recalculate_size() ;
} ;
//------------------------------------------------------------------------------
void on_panel_menu_item_save_configuration_activate (GtkCheckMenuItem *checkmenuitem, gpointer user_data) 
{
		save_configuration_to_file("config.txt") ;
} ;
//------------------------------------------------------------------------------
int t_panel_save_configuration (config_setting_t *setting) 
{
		config_setting_t *s = NULL ;
		GList *tmp = NULL ;
		
		s = config_setting_add(setting, "Displays", CONFIG_TYPE_LIST) ;
		
		tmp = displays ;
		while (tmp != NULL) {
				
				t_display_save_configuration(s, (T_Display*)tmp->data) ;
				tmp = g_list_next(tmp) ;
		} ;
		
		return 0 ;
} ;
//------------------------------------------------------------------------------
void on_panel_menu_item_load_configuration_activate (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		load_configuration_from_file("config.txt") ;
} ;
//------------------------------------------------------------------------------
int t_panel_load_configuration (config_setting_t *panel) 
{
		config_setting_t *disp_list = NULL, *disp = NULL, *s = NULL ;
		unsigned int i,x,y ;
		
		disp_list = config_setting_get_member(panel, "Displays") ;
		if (disp_list == NULL)
				return -1 ;
		
		t_panel_clear() ;
		
		i=0 ;
		while ((disp = config_setting_get_elem(disp_list,i)) != NULL) {
				
				s = config_setting_get_member(disp, "position_x") ;
				x = config_setting_get_int(s) ;
				s = config_setting_get_member(disp, "position_y") ;
				y = config_setting_get_int(s) ;
				
				t_panel_add_display(t_display_new_from_configuration(disp),x,y) ;
				
				i++ ;
		} ;	
		
		t_panel_recalculate_size() ;
		return 0 ;
} ;
//------------------------------------------------------------------------------
int t_panel_add_point (const char *sensor_name, T_Sample *point)
{
		GList *d = displays ;
		
		while (d != NULL) {
				
				t_display_dodaj_wartosc((T_Display*)d->data, sensor_name, point) ;
				
				d = g_list_next(d) ;
		} ;
		
		return TRUE ;
} ;
