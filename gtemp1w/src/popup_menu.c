

#include "popup_menu.h"
#include "t_display.h"
#include "display_add_plot.h"
#include "t_panel.h"


//
//	Aktualnie obrabiany wyswietlacz. UWAGA - globalny dla wielu funkcji....
//
T_Display *disp = NULL  ;


GtkMenu *menu_1 ;

GtkMenuItem				*time_axis ;
GtkMenuItem				*temp_axis ;
GtkMenuItem				*pop_menu_item_font_size ;

// dzialka czas
GtkCheckMenuItem *item_5min ;
GtkCheckMenuItem *item_10min ;
GtkCheckMenuItem *item_15min ;
GtkCheckMenuItem *item_30min ;
GtkCheckMenuItem *item_1godz ;
GtkCheckMenuItem *item_2godz ;
GtkCheckMenuItem *item_3godz ;
GtkCheckMenuItem *item_6godz ;
GtkCheckMenuItem *item_12godz ;
// czas trwania pomiaru
GtkCheckMenuItem *item_range_5min ;
GtkCheckMenuItem *item_range_1godz ;
GtkCheckMenuItem *item_range_2godz ;
GtkCheckMenuItem *item_range_3godz ;
GtkCheckMenuItem *item_range_6godz ;
GtkCheckMenuItem *item_range_12godz ;
GtkCheckMenuItem *item_range_24godz ;
GtkCheckMenuItem *item_range_48godz ;
GtkCheckMenuItem *item_range_7dni ;
GtkCheckMenuItem *item_range_14dni ;
GtkCheckMenuItem *item_range_1mies ;
GtkCheckMenuItem *item_range_2mies ;
// dzialka temperatura
GtkCheckMenuItem *item_temp_scale_1 ;
GtkCheckMenuItem *item_temp_scale_2 ;
GtkCheckMenuItem *item_temp_scale_5 ;
GtkCheckMenuItem *item_temp_scale_10 ;
// zakres temperatura
GtkDialog *input_temp_range_dialog ;
GtkSpinButton *input_temp_min ;
GtkSpinButton *input_temp_max ;
//					POZYCJA I ROZMIAR WYSWIETLACZA
GtkDialog *display_pos_dialog ;
GtkSpinButton *display_pos_x_input ;
GtkSpinButton *display_pos_y_input ;
GtkSpinButton *display_pos_width_input ;
GtkSpinButton *display_pos_height_input ;
GtkLayout *fixed1 ;
// legenda
GtkCheckMenuItem *item_display_legend ;

//-----------------------------------------------------------------------------
void switch_display(T_Display *d)
{
		disp = d ;
} ;
//-----------------------------------------------------------------------------
void show_menu_display_graphical(GdkEventButton *event)
{
		gtk_widget_show((GtkWidget*)time_axis) ;
		gtk_widget_show((GtkWidget*)temp_axis) ;
		
		if (disp->show_legend == TRUE) {
				gtk_check_menu_item_set_active (item_display_legend, TRUE) ;
		} else {
				gtk_check_menu_item_set_active (item_display_legend, FALSE) ;
		} ;
		gtk_widget_show((GtkWidget*)item_display_legend) ;
		gtk_menu_popup (menu_1, NULL, NULL, NULL, NULL, event->button, event->time) ;
} ;
//-----------------------------------------------------------------------------
void show_menu_display_alphanumeric(GdkEventButton *event)
{
		gtk_widget_hide((GtkWidget*)time_axis) ;
		gtk_widget_hide((GtkWidget*)temp_axis) ;
		//gtk_widget_show((GtkWidget*)pop_menu_item_font_size) ;
		gtk_widget_hide((GtkWidget*)item_display_legend) ;
		gtk_menu_popup (menu_1, NULL, NULL, NULL, NULL, event->button, event->time) ;
} ;
//-----------------------------------------------------------------------------
int register_popup_menu (GladeXML *xml) {
		
		menu_1 = (GtkMenu*)glade_xml_get_widget (xml, "display_popup_menu");
		time_axis = (GtkMenuItem*)glade_xml_get_widget (xml, "time_axis");
		temp_axis = (GtkMenuItem*)glade_xml_get_widget (xml, "temp_axis");
		pop_menu_item_font_size = (GtkMenuItem*)glade_xml_get_widget (xml, "pop_menu_item_font_size");
		//
				//												DZIALKA CZAS
				//		
				item_5min = (GtkCheckMenuItem*) glade_xml_get_widget (xml, "min5");
		item_10min = (GtkCheckMenuItem*) glade_xml_get_widget (xml, "min10");
		item_15min = (GtkCheckMenuItem*) glade_xml_get_widget (xml, "min15");
		item_30min = (GtkCheckMenuItem*) glade_xml_get_widget (xml, "min30");
		item_1godz = (GtkCheckMenuItem*) glade_xml_get_widget (xml, "godz1");
		item_2godz = (GtkCheckMenuItem*) glade_xml_get_widget (xml, "godz2");
		item_3godz = (GtkCheckMenuItem*) glade_xml_get_widget (xml, "godz3");
		item_6godz = (GtkCheckMenuItem*) glade_xml_get_widget (xml, "godz6");
		item_12godz= (GtkCheckMenuItem*) glade_xml_get_widget (xml, "godz12");
		//
				//										CZAS TRWANIA POMIARU
				//		
				item_range_5min  	= (GtkCheckMenuItem*) glade_xml_get_widget (xml, "item_range_5min" );
		item_range_1godz  = (GtkCheckMenuItem*) glade_xml_get_widget (xml, "item_range_1godz" );
		item_range_2godz  = (GtkCheckMenuItem*) glade_xml_get_widget (xml, "item_range_2godz" );
		item_range_3godz  = (GtkCheckMenuItem*) glade_xml_get_widget (xml, "item_range_3godz" );
		item_range_6godz  = (GtkCheckMenuItem*) glade_xml_get_widget (xml, "item_range_6godz" );
		item_range_12godz = (GtkCheckMenuItem*) glade_xml_get_widget (xml, "item_range_12godz");
		item_range_24godz = (GtkCheckMenuItem*) glade_xml_get_widget (xml, "item_range_24godz");
		item_range_48godz = (GtkCheckMenuItem*) glade_xml_get_widget (xml, "item_range_48godz");
		item_range_7dni   = (GtkCheckMenuItem*) glade_xml_get_widget (xml, "item_range_7dni"  );
		item_range_14dni  = (GtkCheckMenuItem*) glade_xml_get_widget (xml, "item_range_14dni" );
		item_range_1mies  = (GtkCheckMenuItem*) glade_xml_get_widget (xml, "item_range_1mies" );
		item_range_2mies  = (GtkCheckMenuItem*) glade_xml_get_widget (xml, "item_range_2mies" );
		//
				//										DZIALKA TEMPERATURA
				//
				item_temp_scale_1  = (GtkCheckMenuItem*) glade_xml_get_widget (xml, "item_temp_scale_1") ;
		item_temp_scale_2  = (GtkCheckMenuItem*) glade_xml_get_widget (xml, "item_temp_scale_2") ;
		item_temp_scale_5  = (GtkCheckMenuItem*) glade_xml_get_widget (xml, "item_temp_scale_5") ;
		item_temp_scale_10 = (GtkCheckMenuItem*) glade_xml_get_widget (xml, "item_temp_scale_10") ;
		//
				//									ZAKRES TEMPERATURA
				//
				input_temp_range_dialog = (GtkDialog*) glade_xml_get_widget (xml, "input_temp_range_dialog") ;
		input_temp_min 		= (GtkSpinButton*) glade_xml_get_widget (xml, "input_temp_min") ;
		input_temp_max 		= (GtkSpinButton*) glade_xml_get_widget (xml, "input_temp_max") ;
		//
				//					POZYCJA I ROZMIAR WYSWIETLACZA
				//
				display_pos_dialog				= (GtkDialog*) glade_xml_get_widget (xml, "display_pos_dialog") ;
		display_pos_x_input       = (GtkSpinButton*) glade_xml_get_widget (xml, "display_pos_x_input") ;  
		display_pos_y_input       = (GtkSpinButton*) glade_xml_get_widget (xml, "display_pos_y_input") ; 
		display_pos_width_input   = (GtkSpinButton*) glade_xml_get_widget (xml, "display_pos_width_input") ;
		display_pos_height_input  = (GtkSpinButton*) glade_xml_get_widget (xml, "display_pos_height_input") ;
		fixed1 = (GtkLayout*) glade_xml_get_widget (xml, "fixed1") ;
		//
				//									DODAWANIE WYKRESOW
				//
				register_display_add_plot(xml) ;		
		//		
				//											LEGENDA
				//
				item_display_legend	 = (GtkCheckMenuItem*) glade_xml_get_widget (xml, "item_display_legend") ;
		
		return 0 ;       
} ;
//-----------------------------------------------------------------------------
//
//												DZIALKA CZAS
//
//-----------------------------------------------------------------------------
void reset_menu_dzialka_czas()
{
		item_5min->active = FALSE ;
		item_10min->active = FALSE ; 	
		item_15min->active = FALSE ; 	
		item_30min->active = FALSE ; 	
		item_1godz->active = FALSE ; 	
		item_2godz->active = FALSE ; 	
		item_3godz->active = FALSE ; 	
		item_6godz->active = FALSE ; 	
		item_12godz->active = FALSE ;	
} ;
//-----------------------------------------------------------------------------
void on_min5_toggled (GtkCheckMenuItem *checkmenuitem, gpointer user_data) 
{
		reset_menu_dzialka_czas() ;
		item_5min->active = TRUE ;
		t_display_set_time_scale(disp, 5*60) ;
} ;
//-----------------------------------------------------------------------------
void on_min10_toggled (GtkCheckMenuItem *checkmenuitem, gpointer user_data) 
{
		reset_menu_dzialka_czas() ;
		item_10min->active = TRUE ;
		t_display_set_time_scale(disp, 10*60) ;
} ;
//-----------------------------------------------------------------------------
void on_min15_toggled (GtkCheckMenuItem *checkmenuitem, gpointer user_data) 
{
		reset_menu_dzialka_czas() ;
		item_15min->active = TRUE ;
		t_display_set_time_scale(disp, 15*60) ;
} ;
//-----------------------------------------------------------------------------
void on_min30_toggled (GtkCheckMenuItem *checkmenuitem, gpointer user_data) 
{
		reset_menu_dzialka_czas() ;
		item_30min->active = TRUE ;
		t_display_set_time_scale(disp, 30*60) ;
} ;
//-----------------------------------------------------------------------------
void on_godz1_toggled (GtkCheckMenuItem *checkmenuitem, gpointer user_data) 
{
		//printf ("on_godz1_toggled: %d\n", user_data) ;
		reset_menu_dzialka_czas() ;
		item_1godz->active = TRUE ;
		t_display_set_time_scale(disp, 60*60) ;
} ;
//-----------------------------------------------------------------------------
void on_godz2_toggled (GtkCheckMenuItem *checkmenuitem, gpointer user_data) 
{
		reset_menu_dzialka_czas() ;
		item_2godz->active = TRUE ;
		t_display_set_time_scale(disp, 2*60*60) ;
} ;
//-----------------------------------------------------------------------------
void on_godz3_toggled (GtkCheckMenuItem *checkmenuitem, gpointer user_data) 
{
		reset_menu_dzialka_czas() ;
		item_3godz->active = TRUE ;
		t_display_set_time_scale(disp, 3*60*60) ;
} ;
//-----------------------------------------------------------------------------
void on_godz6_toggled (GtkCheckMenuItem *checkmenuitem, gpointer user_data) 
{
		reset_menu_dzialka_czas() ;
		item_6godz->active = TRUE ;
		t_display_set_time_scale(disp, 6*60*60) ;
} ;
//-----------------------------------------------------------------------------
void on_godz12_toggled (GtkCheckMenuItem *checkmenuitem, gpointer user_data) 
{
		reset_menu_dzialka_czas() ;
		item_12godz->active = TRUE ;
		t_display_set_time_scale(disp, 12*60*60) ;
} ;
//-----------------------------------------------------------------------------
//
//										CZAS TRWANIA POMIARU
//		
//-----------------------------------------------------------------------------
void reset_menu_czas_trwania_pomiaru()
{
		item_range_5min->active = FALSE ;
		item_range_1godz->active = FALSE ; 	
		item_range_2godz->active = FALSE ; 
		item_range_3godz->active = FALSE ; 
		item_range_6godz->active = FALSE ; 
		item_range_12godz->active = FALSE ;
		item_range_24godz->active = FALSE ;
		item_range_48godz->active = FALSE ;
		item_range_7dni->active = FALSE ;  
		item_range_14dni->active = FALSE ; 
		item_range_1mies->active = FALSE ; 
		item_range_2mies->active = FALSE ; 
} ;
//-----------------------------------------------------------------------------
void on_item_range_5min_toggled (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		reset_menu_czas_trwania_pomiaru() ;
		item_range_5min->active = TRUE ;
		t_display_set_time_range(disp, 5*60) ;
} ;
//-----------------------------------------------------------------------------
void on_item_range_1godz_toggled (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		reset_menu_czas_trwania_pomiaru() ;
		item_range_1godz->active = TRUE ;
		t_display_set_time_range(disp, 1*60*60) ;
} ;
//-----------------------------------------------------------------------------
void on_item_range_2godz_toggled (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		reset_menu_czas_trwania_pomiaru() ;
		item_range_2godz->active = TRUE ;
		t_display_set_time_range(disp, 2*60*60) ;
} ;
//-----------------------------------------------------------------------------
void on_item_range_3godz_toggled (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		reset_menu_czas_trwania_pomiaru() ;
		item_range_3godz->active = TRUE ;
		t_display_set_time_range(disp, 3*60*60) ;
} ;
//-----------------------------------------------------------------------------
void on_item_range_6godz_toggled (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		reset_menu_czas_trwania_pomiaru() ;
		item_range_6godz->active = TRUE ;
		t_display_set_time_range(disp, 6*60*60) ;
} ;
//-----------------------------------------------------------------------------
void on_item_range_12godz_toggled (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		reset_menu_czas_trwania_pomiaru() ;
		item_range_12godz->active = TRUE ;
		t_display_set_time_range(disp, 12*60*60) ;
} ;
//-----------------------------------------------------------------------------
void on_item_range_24godz_toggled (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		reset_menu_czas_trwania_pomiaru() ;
		item_range_24godz->active = TRUE ;
		t_display_set_time_range(disp, 24*60*60) ;
} ;
//-----------------------------------------------------------------------------
void on_item_range_48godz_toggled (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		reset_menu_czas_trwania_pomiaru() ;
		item_range_48godz->active = TRUE ;
		t_display_set_time_range(disp, 48*60*60) ;
} ;
//-----------------------------------------------------------------------------
void on_item_range_7dni_toggled (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		reset_menu_czas_trwania_pomiaru() ;
		item_range_7dni->active = TRUE ;
		t_display_set_time_range(disp, 7*24*60*60) ;
} ;
//-----------------------------------------------------------------------------
void on_item_range_14dni_toggled (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		reset_menu_czas_trwania_pomiaru() ;
		item_range_14dni->active = TRUE ;
		t_display_set_time_range(disp, 14*24*60*60) ;
} ;
//-----------------------------------------------------------------------------
void on_item_range_1mies_toggled (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		reset_menu_czas_trwania_pomiaru() ;
		item_range_1mies->active = TRUE ;
		t_display_set_time_range(disp, 31*24*60*60) ;
} ;
//-----------------------------------------------------------------------------
void on_item_range_2mies_toggled (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		reset_menu_czas_trwania_pomiaru() ;
		item_range_2mies->active = TRUE ;
		t_display_set_time_range(disp, 2*31*24*60*60) ;
} ;
//-----------------------------------------------------------------------------
//
//											DZIALKA TEMPERATURA
//
//-----------------------------------------------------------------------------
void reset_menu_dzialka_temperatura()
{
		item_temp_scale_1->active = FALSE ;  
		item_temp_scale_2->active = FALSE ;  
		item_temp_scale_5->active = FALSE ;  
		item_temp_scale_10->active = FALSE ; 
} ;
//-----------------------------------------------------------------------------
void on_item_temp_scale_1_toggled (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		reset_menu_dzialka_temperatura() ;
		item_temp_scale_1->active = TRUE ;
		t_display_set_temp_scale(disp, 1) ;
} ;
//-----------------------------------------------------------------------------
void on_item_temp_scale_2_toggled (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		reset_menu_dzialka_temperatura() ;
		item_temp_scale_2->active = TRUE ;
		t_display_set_temp_scale(disp, 2) ;
} ;
//-----------------------------------------------------------------------------
void on_item_temp_scale_5_toggled (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		reset_menu_dzialka_temperatura() ;
		item_temp_scale_5->active = TRUE ;
		t_display_set_temp_scale(disp, 5) ;
} ;
//-----------------------------------------------------------------------------
void on_item_temp_scale_10_toggled (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		reset_menu_dzialka_temperatura() ;
		item_temp_scale_10->active = TRUE ;
		t_display_set_temp_scale(disp, 10) ;
} ;
//-----------------------------------------------------------------------------
//
//												ZAKRES TEMPERATURA
//
//-----------------------------------------------------------------------------
void on_temp_range_activate (GtkMenuItem *menuitem, gpointer user_data)
{
		gtk_spin_button_set_value(input_temp_min, disp->temp_min) ;
		gtk_spin_button_set_value(input_temp_max, disp->temp_max) ;
		gtk_widget_show((GtkWidget*)input_temp_range_dialog) ;
} ;
//-----------------------------------------------------------------------------
void on_input_temp_range_dialog_close (GtkDialog *dialog, gpointer user_data)
{
		gtk_widget_hide((GtkWidget*)dialog) ;
} ;
//-----------------------------------------------------------------------------
void on_input_temp_range_dialog_response (GtkDialog *dialog, gint arg1, gpointer user_data)
{
		if (arg1 == 1) {  // OK button, patrz glade
						disp->temp_max = gtk_spin_button_get_value_as_int(input_temp_max) ;
				disp->temp_min = gtk_spin_button_get_value_as_int(input_temp_min) ;
		} else if (arg1 == 2) {  // Cancel button
						gtk_widget_hide((GtkWidget*)dialog) ;
		} ;
		
		gtk_widget_hide(disp->canvas) ;
		gtk_widget_show(disp->canvas) ;		
} ;
//-----------------------------------------------------------------------------
//
//					POZYCJA I ROZMIAR WYSWIETLACZA
//
//-----------------------------------------------------------------------------

gboolean block_display_move = TRUE ;

void on_display_size_and_pos_activate (GtkMenuItem *menuitem, gpointer user_data)
{
		//guint width, height ;
		//gtk_layout_get_size(display.canvas, &width, &height) ; // nie uzywac, chyba nie rozumiem....
				
				block_display_move = TRUE ;
		gtk_spin_button_set_value(display_pos_x_input, disp->canvas->allocation.x) ;
		gtk_spin_button_set_value(display_pos_y_input, disp->canvas->allocation.y) ;
		gtk_spin_button_set_value(display_pos_width_input, disp->canvas->allocation.width) ;
		gtk_spin_button_set_value(display_pos_height_input, disp->canvas->allocation.height) ;
		gtk_widget_show((GtkWidget*)display_pos_dialog) ;
		block_display_move = FALSE ;
} ;
//-----------------------------------------------------------------------------
void on_display_pos_dialog_close  (GtkDialog *dialog, gpointer user_data)
{
		gtk_widget_hide((GtkWidget*)dialog) ;
} ;
//-----------------------------------------------------------------------------
void t_panel_move_active_display(void)
{		
		gtk_widget_set_size_request(disp->canvas, 
																gtk_spin_button_get_value_as_int(display_pos_width_input), 
																gtk_spin_button_get_value_as_int(display_pos_height_input)) ;
		
		gtk_layout_move(fixed1,
										disp->canvas,
										gtk_spin_button_get_value_as_int(display_pos_x_input),
										gtk_spin_button_get_value_as_int(display_pos_y_input)) ;
		
		t_panel_recalculate_size() ;
		gtk_widget_hide((GtkWidget*)fixed1) ;
		gtk_widget_show((GtkWidget*)fixed1) ;		
} ;
//-----------------------------------------------------------------------------
void on_display_pos_dialog_response (GtkDialog *dialog, gint arg1, gpointer user_data)
{
		if (arg1 == 1) {  // OK button, patrz glade
						
						t_panel_move_active_display() ;
				
		} else if (arg1 == 2) {  // Cancel button
						gtk_widget_hide((GtkWidget*)dialog) ;
		} ;
		
		gtk_widget_hide((GtkWidget*)fixed1) ;
		gtk_widget_show((GtkWidget*)fixed1) ;	
} ;
//-----------------------------------------------------------------------------
void on_display_move_callback (void) 
{
		if (block_display_move == FALSE)
				t_panel_move_active_display() ;
} ;
//-----------------------------------------------------------------------------
void on_item_display_delete_plot_activate (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		t_panel_remove_display(disp) ;
} ;
//-----------------------------------------------------------------------------
//
//												ROZMIAR CZCIONKI
//
//-----------------------------------------------------------------------------
void on_item_font_size_8_activate (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		disp->font_size = 8 ;
		t_display_recalculate_picture(disp) ; // bardzo brzydko, ale nie rozumiem, dlaczego
				// wyswietlacz graficzny nie zmienia czcionki....
} ;
//-----------------------------------------------------------------------------
void on_item_font_size_10_activate (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		disp->font_size = 10 ;
		t_display_recalculate_picture(disp) ; // bardzo brzydko, ale nie rozumiem, dlaczego
				// wyswietlacz graficzny nie zmienia czcionki....
} ;
//-----------------------------------------------------------------------------
void on_item_font_size_12_activate (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		disp->font_size = 12 ;
		t_display_recalculate_picture(disp) ; // bardzo brzydko, ale nie rozumiem, dlaczego
				// wyswietlacz graficzny nie zmienia czcionki....
} ;
//-----------------------------------------------------------------------------
void on_item_font_size_14_activate (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		disp->font_size = 14 ;
		t_display_recalculate_picture(disp) ; // bardzo brzydko, ale nie rozumiem, dlaczego
				// wyswietlacz graficzny nie zmienia czcionki....
} ;
//-----------------------------------------------------------------------------
void on_item_font_size_16_activate (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		disp->font_size = 16 ;
		t_display_recalculate_picture(disp) ; // bardzo brzydko, ale nie rozumiem, dlaczego
				// wyswietlacz graficzny nie zmienia czcionki....
} ;
//-----------------------------------------------------------------------------
void on_item_font_size_20_activate (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		disp->font_size = 20 ;
		t_display_recalculate_picture(disp) ; // bardzo brzydko, ale nie rozumiem, dlaczego
				// wyswietlacz graficzny nie zmienia czcionki....
} ;
//-----------------------------------------------------------------------------
void on_item_font_size_24_activate (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		disp->font_size = 24 ;
		t_display_recalculate_picture(disp) ; // bardzo brzydko, ale nie rozumiem, dlaczego
				// wyswietlacz graficzny nie zmienia czcionki....
} ;
//-----------------------------------------------------------------------------
void on_item_font_size_28_activate (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		disp->font_size = 28 ;
		t_display_recalculate_picture(disp) ; // bardzo brzydko, ale nie rozumiem, dlaczego
				// wyswietlacz graficzny nie zmienia czcionki....
} ;
//-----------------------------------------------------------------------------
void on_item_font_size_32_activate (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		disp->font_size = 32 ;
		t_display_recalculate_picture(disp) ; // bardzo brzydko, ale nie rozumiem, dlaczego
				// wyswietlacz graficzny nie zmienia czcionki....
} ;
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void on_item_display_legend_toggled(GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		//printf("toggled\n") ;
		if (gtk_check_menu_item_get_active(checkmenuitem) == TRUE)
				disp->show_legend = TRUE ;
		else
				disp->show_legend = FALSE ;
		
		
		t_display_recalculate_picture(disp) ;
} ;
//-----------------------------------------------------------------------------
