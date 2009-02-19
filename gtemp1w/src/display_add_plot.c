
// plik powinien sie chyba nazywac display_manage_plots

#include <gtk/gtk.h>

#include "display_add_plot.h"
#include "t_display.h"
#include "popup_menu.h"
#include "sensors.h"

//
//		Okienko dodawania nowego wykresu do wyswietlacza
//
GtkDialog 				*display_add_plot_dialog ;
GtkColorButton		*add_plot_input_color ;
GtkEntry					*add_plot_entry_name ;		
GtkTable					*display_add_plot_table ;
GtkComboBox				*add_plot_name_combo = NULL ;
GtkCheckButton		*add_plot_visible_button ;
GtkSpinButton			*add_plot_n_average_spin ;
GtkRadioButton		*add_plot_lines_button ;
GtkRadioButton		*add_plot_points_button ;
GtkRadioButton		*add_plot_diamonds_button ;

//
//		Okienko zarzadzania wykresami danego wyswietlacza
//
GtkDialog					*display_manage_plots_dialog ;
GtkTreeView 			*tree_plot_list ;

void rebuild_plots_tree (void) ;
int tree_plot_list_get_selected_index(void) ;

//------------------------------------------------------------------------------
void register_display_add_plot(GladeXML *xml) 
{
		display_add_plot_dialog = (GtkDialog*) glade_xml_get_widget (xml, "display_add_plot_dialog") ;
		add_plot_input_color		= (GtkColorButton*) glade_xml_get_widget (xml, "add_plot_input_color") ;
		add_plot_entry_name 		= (GtkEntry*) glade_xml_get_widget (xml, "add_plot_entry_name") ;
		display_add_plot_table 		= (GtkTable*) glade_xml_get_widget (xml, "display_add_plot_table") ;
		add_plot_visible_button 		= (GtkCheckButton*) glade_xml_get_widget (xml, "add_plot_visible_button") ;
		add_plot_n_average_spin	= (GtkSpinButton*) glade_xml_get_widget (xml, "add_plot_n_average_spin") ;
		add_plot_lines_button	= (GtkRadioButton*) glade_xml_get_widget (xml, "add_plot_lines_button") ;
		add_plot_points_button	= (GtkRadioButton*) glade_xml_get_widget (xml, "add_plot_points_button") ;
		add_plot_diamonds_button = (GtkRadioButton*) glade_xml_get_widget (xml, "add_plot_diamonds_button") ;
		
		display_manage_plots_dialog = (GtkDialog*) glade_xml_get_widget (xml, "display_manage_plots_dialog") ;
		tree_plot_list = (GtkTreeView*) glade_xml_get_widget (xml, "tree_plot_list") ;
		
} ;
//------------------------------------------------------------------------------

//
//	bardzo proste rozwiazanie ze zmienna globalna i unikam przepinania funkcji
//
enum {
		ADD_PLOT,
		MODIFY_PLOT
} ;

int plot_operation = ADD_PLOT ;


void rebuild_and_show_add_plot_dialog()
{
		GList *l, *ltmp ;
		
		if (add_plot_name_combo != NULL) {
				gtk_widget_destroy((GtkWidget*)add_plot_name_combo) ;
				add_plot_name_combo = NULL ;
		} ;
		
		if (plot_operation == ADD_PLOT) {
				
				gtk_window_set_title ((GtkWindow*)display_add_plot_dialog, "Dodaj charakterystykę") ;
				
				// parametry domyslne tworzonego wykresu
						gtk_toggle_button_set_active((GtkToggleButton *)add_plot_visible_button, TRUE) ;
				gtk_toggle_button_set_active ((GtkToggleButton *)add_plot_lines_button, TRUE) ;
				gtk_spin_button_set_value(add_plot_n_average_spin, 0.0) ;
				
				gtk_widget_hide((GtkWidget*)add_plot_entry_name) ;
				
				// utworz puste combo
						add_plot_name_combo = (GtkComboBox*)gtk_combo_box_new_text() ;
				gtk_table_attach_defaults (display_add_plot_table,
																	 (GtkWidget*)add_plot_name_combo,
																	 1, 2, 0, 1 ) ;
				// dodaj do combo wszystkie czujniki zarejestrowane w programie
						l = get_program_sensor_names() ;
				ltmp = l ;
				while (ltmp != NULL) {
						gtk_combo_box_append_text(add_plot_name_combo,
																			((GString*)(ltmp->data))->str) ;
						ltmp = g_list_next(ltmp) ;
				} ;
				destroy_gstring_list(l) ;
				
				gtk_widget_show((GtkWidget*)add_plot_name_combo) ;
				
		} else if (plot_operation == MODIFY_PLOT) {
				
				gtk_window_set_title ((GtkWindow*)display_add_plot_dialog, "Modyfikuj charakterystykę") ;
				
				T_Plot *p = g_list_nth_data (disp->plots, tree_plot_list_get_selected_index()) ;
				if (p == NULL) {
						return ;
				} ;
				
				gtk_entry_set_text (add_plot_entry_name, p->name->str) ;
				gtk_editable_set_editable ((GtkEditable*)add_plot_entry_name, FALSE) ;
				gtk_widget_show ((GtkWidget*)add_plot_entry_name) ;
				gtk_toggle_button_set_active((GtkToggleButton *)add_plot_visible_button, p->visible) ;
				gtk_spin_button_set_value(add_plot_n_average_spin, p->n_average) ;
				if (p->style == LINES) {
						gtk_toggle_button_set_active ((GtkToggleButton *)add_plot_lines_button, TRUE);
						//gtk_toggle_button_set_active ((GtkToggleButton *)add_plot_points_button, FALSE) ;
				} else if (p->style == POINTS) {
						gtk_toggle_button_set_active ((GtkToggleButton *)add_plot_points_button, TRUE) ;
						//gtk_toggle_button_set_active ((GtkToggleButton *)add_plot_lines_button, FALSE);
				} else if (p->style == DIAMONDS) {
						gtk_toggle_button_set_active ((GtkToggleButton *)add_plot_diamonds_button, TRUE) ;
				} ;						
				
				gtk_color_button_set_color(add_plot_input_color, &(p->color)) ;
				
		} else 
				gtk_window_set_title ((GtkWindow*)display_add_plot_dialog, "????????") ;
		
		gtk_widget_show((GtkWidget*)display_add_plot_dialog) ;
} ;
//------------------------------------------------------------------------------
void on_item_display_add_plot_activate (GtkMenuItem *menuitem, gpointer user_data) 
{
		plot_operation = ADD_PLOT ;
		rebuild_and_show_add_plot_dialog() ;
} ;
//------------------------------------------------------------------------------
void on_display_add_plot_dialog_close (GtkDialog *dialog, gpointer user_data)
{
		gtk_widget_hide((GtkWidget*)dialog) ;
} ;
//-----------------------------------------------------------------------------
void add_plot_from_dialog(void) 
{
		T_Plot *plot ;
		GdkColor color ;
		T_PlotStyle style = LINES ;
		
		if (gtk_combo_box_get_active(add_plot_name_combo) > -1) {
				
				gtk_color_button_get_color (add_plot_input_color, &color) ;
				if (gtk_toggle_button_get_active ((GtkToggleButton *)add_plot_lines_button))
						style = LINES ;
				else if (gtk_toggle_button_get_active ((GtkToggleButton *)add_plot_points_button))
						style = POINTS ;
				else if (gtk_toggle_button_get_active ((GtkToggleButton *)add_plot_diamonds_button))
						style = DIAMONDS ;
				
				plot = t_plot_new(gtk_combo_box_get_active_text(add_plot_name_combo), color,
													gtk_toggle_button_get_active((GtkToggleButton *)add_plot_visible_button),
													gtk_spin_button_get_value_as_int(add_plot_n_average_spin),
													style) ;
				
				t_display_add_plot(disp, plot) ;
				
				t_display_recalculate_picture(disp) ;	// FIXME: to moze by gdzies przeniesc ?
						
		} else {
				printf ("Dodawanie czujnikow niezarejestrowanych w programie zabronione\n") ;
		} ;
} ;
//-----------------------------------------------------------------------------
void modify_plot_from_dialog(void)
{
		// linie ponizej trzeba by zmienic, zeby umozliwic wielokrotna zmiane
				//	parametrow pojedynczego wykresu
				T_Plot *p = g_list_nth_data (disp->plots, tree_plot_list_get_selected_index()) ;
		
		if (p == NULL)
				return ;
		
		GdkColor color ;				
		gtk_color_button_get_color (add_plot_input_color, &color) ;	
		
		t_plot_set_color (p, color) ;
		t_plot_set_visible (p, gtk_toggle_button_get_active((GtkToggleButton *)add_plot_visible_button)) ;
		t_plot_set_n_average (p, gtk_spin_button_get_value_as_int(add_plot_n_average_spin)) ;
		if (gtk_toggle_button_get_active ((GtkToggleButton *)add_plot_lines_button))
				t_plot_set_style (p,LINES) ;
		else if (gtk_toggle_button_get_active ((GtkToggleButton *)add_plot_points_button))
				t_plot_set_style (p,POINTS) ;
		else if (gtk_toggle_button_get_active ((GtkToggleButton *)add_plot_diamonds_button))
				t_plot_set_style (p,DIAMONDS) ;
		
		
		t_display_recalculate_picture(disp) ;	// FIXME: to moze by gdzies przeniesc ?
} ;
//-----------------------------------------------------------------------------


void on_display_add_plot_dialog_response (GtkDialog *dialog, gint arg1, gpointer user_data)
{
		if (arg1 == 1) {  // OK button, patrz glade
						
						if (plot_operation == ADD_PLOT) {
								
								add_plot_from_dialog() ;
								
						} else if (plot_operation == MODIFY_PLOT) {
								
								modify_plot_from_dialog() ;
								
						} else { // strange error
										printf ("on_display_add_plot_dialog_response: UNKNOWN operation %d\n",plot_operation) ;
						} ;
				
				rebuild_plots_tree() ;
				
		} else if (arg1 == 2) {  // Cancel button
		} ;
		
		gtk_widget_hide((GtkWidget*)dialog) ;
		
} ;
//-----------------------------------------------------------------------------
//
//		
//
//-----------------------------------------------------------------------------
void rebuild_plots_tree()
{
		if (GTK_WIDGET_VISIBLE((GtkWidget*)display_manage_plots_dialog)) 
		{
				//printf ("rebuild_plots_tree\n") ;
				
				// Wyczyszczenie wyswietlanej listy
						//
						GList *columns = gtk_tree_view_get_columns(tree_plot_list) ;
				GList *col = g_list_last(columns) ;
				while (col != NULL) {
						gtk_tree_view_remove_column(tree_plot_list, col->data) ;
						col = g_list_previous(col) ;
				} ;
				g_list_free(columns) ;
				gtk_tree_view_set_model(tree_plot_list, NULL) ;		// chyba nadmiar
						
						// zaladowanie nowych danych
						//
						GtkListStore *store = t_display_new_store_with_plots(disp) ;
				gtk_tree_view_set_model(tree_plot_list, GTK_TREE_MODEL(store)) ;
				
				//	utworzenie rendererow
						//
						GtkCellRenderer *renderer ;
				GtkTreeViewColumn *column ;
				
				renderer = gtk_cell_renderer_text_new ();
				column = gtk_tree_view_column_new_with_attributes ("Indeks",
																													 renderer,
																													 "text", INDEX,
																													 NULL);
				gtk_tree_view_append_column (GTK_TREE_VIEW (tree_plot_list), column);
				
				renderer = gtk_cell_renderer_text_new ();
				column = gtk_tree_view_column_new_with_attributes ("Nazwa",
																													 renderer,
																													 "text", NAME,
																													 NULL);
				gtk_tree_view_append_column (GTK_TREE_VIEW (tree_plot_list), column);
				
				renderer = gtk_cell_renderer_text_new ();
				column = gtk_tree_view_column_new_with_attributes ("Kolor",
																													 renderer,
																													 "cell-background", COLOR,
																													 NULL);
				gtk_tree_view_append_column (GTK_TREE_VIEW (tree_plot_list), column);
				
				renderer = gtk_cell_renderer_text_new ();
				column = gtk_tree_view_column_new_with_attributes ("Styl",
																													 renderer,
																													 "text", STYLE,
																													 NULL);
				gtk_tree_view_append_column (GTK_TREE_VIEW (tree_plot_list), column);
				
				renderer = gtk_cell_renderer_text_new ();
				column = gtk_tree_view_column_new_with_attributes ("Uśrednianie",
																													 renderer,
																													 "text", N_AVERAGE,
																													 NULL);
				gtk_tree_view_append_column (GTK_TREE_VIEW (tree_plot_list), column);
				
				
				renderer = gtk_cell_renderer_text_new ();
				column = gtk_tree_view_column_new_with_attributes ("Liczba punktów",
																													 renderer,
																													 "text", N_POINTS,
																													 NULL);
				gtk_tree_view_append_column (GTK_TREE_VIEW (tree_plot_list), column);
				
				renderer = gtk_cell_renderer_toggle_new ();
				column = gtk_tree_view_column_new_with_attributes ("Widoczny",
																													 renderer,
																													 "active", ALLOW_DISPLAY,
																													 NULL);
				gtk_tree_view_append_column (GTK_TREE_VIEW (tree_plot_list), column);
				
				// usuniecie selekcji
						//
						GtkTreeSelection *selection;		
				selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_plot_list));
				
				gtk_tree_selection_unselect_all (selection) ;
				gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE) ;
				
				gtk_widget_show((GtkWidget*)tree_plot_list) ;
				
				//g_object_unref(renderer) ;		// chyba nie mozna, doczytac !!!
						g_object_unref(store) ;
		} ;
} ;
//-----------------------------------------------------------------------------
void on_item_display_manage_plots_activate (GtkMenuItem *menuitem, gpointer user_data) 
{
		gtk_widget_show((GtkWidget*)display_manage_plots_dialog) ;
		rebuild_plots_tree() ;
} ;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int tree_plot_list_get_selected_index(void)
{
		GtkTreeSelection *selection;
		GtkTreeModel *model;
		GtkTreeIter iter;
		/* This will only work in single or browse selection mode! */
				selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_plot_list));
		
		int result = -1 ;
		if (gtk_tree_selection_get_selected(selection, &model, &iter))
		{
				gtk_tree_model_get (model, &iter, INDEX, &result, -1);
		} ;
		
		return result ;
} ;
//-----------------------------------------------------------------------------
void on_manage_plots_delete_button_clicked (GtkButton *button, gpointer user_data)
{
		int idx = tree_plot_list_get_selected_index() ;
		
		if (idx > -1) {	// ten warunek chyba nawet zbedny...
						t_display_remove_plot_with_index(disp,idx) ;
				rebuild_plots_tree() ;
				t_display_recalculate_picture(disp) ;
		} ;
} ;
//-----------------------------------------------------------------------------
void on_manage_plot_add_button_clicked (GtkButton *button, gpointer user_data)
{
		plot_operation = ADD_PLOT ;
		rebuild_and_show_add_plot_dialog() ;
} ;
//-----------------------------------------------------------------------------
void on_manage_plots_modify_button_clicked (GtkButton *button, gpointer user_data)
{
		plot_operation = MODIFY_PLOT ;
		rebuild_and_show_add_plot_dialog() ;
} ;
//-----------------------------------------------------------------------------
void on_manage_plots_close_button_clicked (GtkButton *button, gpointer user_data)
{
		gtk_widget_hide((GtkWidget*)display_manage_plots_dialog) ;
} ;
//-----------------------------------------------------------------------------
