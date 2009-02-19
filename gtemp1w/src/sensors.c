#include "sensors.h"
#include "sample_load_save.h"
#include "sensor_read_thread.h"
#include "general_settings.h"
#include <sys/types.h>
#include <dirent.h>
#include <fnmatch.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <string.h>

//#define SENSOR_PATH_PREFIX "/var/1-Wire/uncached/"

GList *sensors = NULL ;		// wszystkie czujniki zarejestrowane w programie, tzn.
// posiadajace nazwe, a nie tylko id
//
//	U W A G A ! ! !  -  nazwy czujnikow musza byc unikalne
//

void rebuild_sensors_tree(gboolean force_rebuild) ;



//------------------------------------------------------------------------------
GList *destroy_sensor_list(GList *sensor_list) 
{
		GList *l = sensor_list ;
		
		while (l!=NULL) {
				
				t_sensor_destroy(l->data) ;
				l = g_list_next(l) ;
		} ;
		
		g_list_free(sensor_list) ;
		return NULL ;
} ;
//------------------------------------------------------------------------------
// Usuwanie wszystkich czujnikow z globalnej listy
void clear_sensor_list(void) 
{
		sensors = destroy_sensor_list(sensors) ;
} ;
//------------------------------------------------------------------------------
GList *find_sensor_with_name (const gchar *name) 
{
		GList *l = sensors ;
		GString *gname = g_string_new(name) ;
		
		while (l != NULL) {
				if (g_string_equal(gname, ((T_Sensor*)(l->data))->name )) {
						g_string_free(gname,FALSE) ;
						return l ;
				} ;
				
				l = g_list_next(l) ;
		} ;
		
		//printf ("find_sensor_with_name : not found\n") ;
		g_string_free(gname,FALSE) ;
		//return l ; // l = NULL
				return NULL ;
} ;
//------------------------------------------------------------------------------
void remove_sensor_with_name (const gchar *name)
{
		GList *l = find_sensor_with_name(name) ;
		
		if (l != NULL) {
				t_sensor_destroy(l->data) ;
				sensors =  g_list_remove(sensors, l->data) ;
		} ;
} ;
//------------------------------------------------------------------------------
void modify_sensor_with_name (const gchar *name, gchar *id, T_Resolution resolution, 
															time_t read_interval, gboolean cached)
{
		GList *l = find_sensor_with_name(name) ;
		T_Sensor *s ;
		
		if (l != NULL) {
				s = (T_Sensor*)(l->data) ;
				s->resolution = resolution ;
				s->read_interval = read_interval ;
				s->cached = cached ;
				
				g_string_free(s->sensor_id, TRUE) ;
				s->sensor_id = g_string_new(id) ;
		} else {
				printf ("Czujnik %s nie istnieje\n", name) ;
		} ;
} ;
//------------------------------------------------------------------------------
void add_sample_to_sensor(T_Sample sample, const char *sensor_name)
{
		GList *l = find_sensor_with_name(sensor_name) ;
		
		//printf ("dodawanie danej do czujnika %s  %p\n", sensor_name, l) ;
		if (l != NULL)
				t_sensor_add_sample((T_Sensor*)l->data, sample) ;
		//printf ("dana dodana\n") ;
} ;
//------------------------------------------------------------------------------
void add_sensor (const gchar *name, const gchar *id, T_Resolution resolution, 
								 time_t read_interval, gboolean cached) 
{
		if (find_sensor_with_name(name) == NULL) {
				sensors = g_list_append(sensors, t_sensor_new(name, id, resolution, 
																											read_interval, cached) 
																) ;
		} else {
				printf ("Czujnik %s jest juz dodany\n", name) ;
		} ;
} ;
//------------------------------------------------------------------------------
int sensors_save_configuration (config_setting_t *setting) 
{
		config_setting_t *s = NULL ;
		GList *tmp = NULL ;
		
		s = config_setting_add(setting, "Sensor_List", CONFIG_TYPE_LIST) ;
		
		tmp = sensors ;
		while (tmp != NULL) {
				
				t_sensor_save_configuration(s, (T_Sensor*)tmp->data) ;
				tmp = g_list_next(tmp) ;
		} ;
		
		return 0 ;
} ;
//------------------------------------------------------------------------------
void append_sensor_samples_to_array (GArray *samples, const char *s_name,
																		 time_t t_begin, time_t t_end) 
{
		GList *lst = find_sensor_with_name(s_name) ;
		
		if (lst == NULL) {
				//printf ("append_sensor_samples_to_array: nie znaleziono czujnika %s\n", s_name) ;
				return ;
		} ;
		
		T_Sensor *sensor = lst->data ;
		
		t_sensor_add_samples_to_array (sensor, samples, 
																	 t_begin, t_end) ;
} ;
//------------------------------------------------------------------------------
// UWAGA - istniejaca lista czujnikow JEST czyszczona !
// Dzieki temu kolejnosc czujnikow na liscie jest zgodna z kolejnoscia z pliku.
int sensors_load_configuration (config_setting_t *setting)
{
		config_setting_t *sensor_list = NULL, *sensor = NULL, *s = NULL ;
		unsigned int i ;
		const char *s_id = NULL, *s_name = NULL ;
		unsigned int s_res = 9, s_interval = 5 ;
		gboolean s_cached = FALSE ;
		
		sensor_list = config_setting_get_member(setting, "Sensor_List") ;
		
		clear_sensor_list() ;
		
		i=0 ;
		while ((sensor = config_setting_get_elem(sensor_list,i)) != NULL) {
				
				// ten kod powinien chyba byc w t_sensor.c
						s = config_setting_get_member(sensor, "sensor_id") ;
				if (s != NULL) s_id = config_setting_get_string(s) ;
				s = config_setting_get_member(sensor, "name") ;
				if (s != NULL) s_name = config_setting_get_string(s) ;
				s = config_setting_get_member(sensor, "resolution") ;
				if (s != NULL) s_res = config_setting_get_int(s) ;
				s = config_setting_get_member(sensor, "read_interval") ;
				if (s != NULL) s_interval = config_setting_get_int(s) ;
				s = config_setting_get_member(sensor, "cached") ;
				if (s != NULL) s_cached = config_setting_get_bool(s) ;
				
				add_sensor (s_name, s_id, s_res, s_interval, s_cached) ;
				
				i++ ;
		} ;	
		
		rebuild_sensors_tree(FALSE) ;
		return 0 ;
} ;
//------------------------------------------------------------------------------
GList *get_program_sensor_names(void) 
{
		GList *tmp, *result = NULL ;
		
		tmp = sensors ;
		while (tmp != NULL) {
				
				result = g_list_append(result, g_string_new(((T_Sensor*)(tmp->data))->name->str) ) ;
				
				tmp = g_list_next(tmp) ;
		} ;
		
		return result ;
} ;
//------------------------------------------------------------------------------
//	zwraca liste czujnikow wymagajacych odczytu danych
GList *get_sensor_to_read_list(void) 
{
		GList *l = sensors, *result = NULL ;
		T_Sensor *s ;
		
		//printf("->\n") ;
		while (l != NULL) {
				
				s = (T_Sensor *) l->data ;
				
				if (t_sensor_need_read(s) && is_sensor_present(*s)) {
						//printf ("\n") ;
						result = g_list_append(result,
																	 t_sensor_new(s->name->str, s->sensor_id->str, 
																								s->resolution, s->read_interval,
																								s->cached)
																	 ) ;
				} ;
				
				l = g_list_next(l) ;
		} ;
		//printf("<-\n") ;
		
		return result ;
} ;
//------------------------------------------------------------------------------
// Zwalnianie pamieci zajmowanej przez liste napisow typu GString
GList *destroy_gstring_list (GList *l) 
{
		GList *tmp ;
		
		tmp = l ;
		while (tmp != NULL) {
				g_string_free (tmp->data, TRUE) ;
				tmp = g_list_next(tmp) ;
		} ;
		
		g_list_free(l) ;
		return NULL ;
} ;
//------------------------------------------------------------------------------
void save_all_sensors_data (void) 
{
		GList *s = sensors ;
		GArray *sensor_data = NULL ;
		
		while (s != NULL) {
				
				sensor_data = t_sensor_remove_samples((T_Sensor*) s->data) ;
				
				// patrz store_samples_one_day() - moze watek zapisu zadzialal w tym miejscu ??
						// nie no, niemozliwe - przeciez wlasnie to jest watek zapisu....
						// nie rozumiem...
						
						store_samples (sensor_data, ((T_Sensor*)(s->data))->name->str) ;
				
				g_array_free(sensor_data, TRUE) ;
				s = g_list_next(s) ;
		} ;
} ;
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
GList *get_system_sensor_ids(void)
{
		GList *result = NULL ;
		DIR *dir ;
		struct dirent *entry  ;
		
		dir = opendir(path_1_wire_directory->str); 
		if (dir == NULL)
				return NULL ;
		
		while ((entry = readdir(dir)) != NULL) {
				
				if ( fnmatch("28*", entry->d_name, 0) == 0) { // to chyba oznacza wylacznie DS1820
								result = g_list_append(result, g_string_new(entry->d_name) ) ;
						//printf ("Czujnik %s OK\n", entry->d_name) ;
				} ;
				
		} ;
		
		closedir(dir) ;
		
		return result ;
} ;
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
//								Obsluga okna zarzadzajacego czujnikami
//
//------------------------------------------------------------------------------

//
//	kontrolki glownego okna ustawien
//
GtkDialog 		*settings_dialog ;		// to przeniesc pozniej w inne miejsce...

GtkButton			*sensors_manage_close_button ;

GtkButton			*sensors_manage_change_button ;
GtkButton			*sensors_manage_delete_button ;
GtkButton			*sensors_manage_new_button ;

GtkTreeView		*sensors_manage_display_tree ;

GtkNotebook		*settings_notebook ;

//
//		kontrolki okienka ustawiajacego parametry pojedynczego czujnika
//
GtkDialog 		*sensor_operations_dialog ;

GtkButton			*sensor_operation_dialog_ok_button ;
GtkButton			*sensor_operation_dialog_cancel_button ;

GtkTable			*sensor_settings_table ;

GtkEntry			*sensors_manage_name_entry ;
GtkComboBox		*sensors_manage_id_combo = NULL ;
GtkComboBox		*sensors_manage_resolution_combo ;
GtkSpinButton	*sensors_manage_refresh_time_entry ;
GtkCheckButton *sensors_manage_cached_button ;



//------------------------------------------------------------------------------
void reset_sensor_settings_combos(void) 
{
		if (sensors_manage_id_combo != NULL)
				gtk_widget_destroy((GtkWidget*)sensors_manage_id_combo) ;
		
		sensors_manage_id_combo = (GtkComboBox*)gtk_combo_box_new_text() ;
		gtk_table_attach_defaults (sensor_settings_table,
															 (GtkWidget*)sensors_manage_id_combo,
															 1, 2, 1, 2 ) ;
		gtk_widget_show((GtkWidget*)sensors_manage_id_combo) ;
} ;
//------------------------------------------------------------------------------
void register_sensors_dialog (GladeXML *xml) 
{
		//
				//	kontrolki glownego okna ustawien
				//
				settings_dialog = (GtkDialog*) glade_xml_get_widget (xml, "settings_dialog") ;
		
		sensors_manage_close_button 		= (GtkButton*) glade_xml_get_widget (xml, "sensors_manage_close_button") ;
		
		sensors_manage_change_button 		= (GtkButton*) glade_xml_get_widget (xml, "sensors_manage_change_button") ;
		sensors_manage_delete_button 		= (GtkButton*) glade_xml_get_widget (xml, "sensors_manage_delete_button") ;
		sensors_manage_new_button 			= (GtkButton*) glade_xml_get_widget (xml, "sensors_manage_new_button") ;
		
		sensors_manage_name_entry 			=	(GtkEntry*) glade_xml_get_widget (xml, "sensors_manage_name_entry") ;
		
		// trzeba utworzyc nowe combo, bo glade nie obsluguje combo tekstowych
				//reset_sensor_settings_combos() ;
		
		sensors_manage_display_tree			= (GtkTreeView*) glade_xml_get_widget (xml, "sensors_manage_display_tree") ;
		
		settings_notebook								= (GtkNotebook*) glade_xml_get_widget (xml, "settings_notebook") ;
		
		//
				//		kontrolki okienka ustawiajacego parametry pojedynczego czujnika
				//		
				sensor_operations_dialog = (GtkDialog*) glade_xml_get_widget (xml, "sensor_operations_dialog") ;
		
		sensor_operation_dialog_cancel_button = (GtkButton*) glade_xml_get_widget (xml, "sensor_operation_dialog_cancel_button") ;
		sensor_operation_dialog_ok_button = (GtkButton*) glade_xml_get_widget (xml, "sensor_operation_dialog_ok_button") ;
		
		sensor_settings_table =	(GtkTable*) glade_xml_get_widget (xml, "sensor_settings_table") ;
		
		sensors_manage_resolution_combo	= (GtkComboBox*) glade_xml_get_widget (xml, "sensors_manage_resolution_combo") ;
		sensors_manage_refresh_time_entry = (GtkSpinButton*) glade_xml_get_widget (xml, "sensors_manage_refresh_time_entry") ;
		sensors_manage_cached_button = (GtkCheckButton*) glade_xml_get_widget (xml, "sensors_manage_cached_button") ;
		
		//
				//	Tylko dla testow
				//
				/*add_sensor("aaaaa", "111", BIT_12) ;
		add_sensor("bbb", "111", BIT_12) ;
		add_sensor("ccc", "111", BIT_12) ;
		add_sensor("ddd", "111", BIT_12) ;
		add_sensor("eee", "111", BIT_12) ;
		add_sensor("fff", "111", BIT_12) ;*/
				
				// A to potrzebne !!!
				//rebuild_sensors_settings_tab() ;
		rebuild_sensors_tree(FALSE) ;
		
} ;


//------------------------------------------------------------------------------
void on_settings_dialog_close (GtkDialog *dialog, gpointer user_data)
{
		gtk_widget_hide((GtkWidget*)dialog) ;
} ;
//------------------------------------------------------------------------------
void on_sensors_manage_close_button_clicked (GtkButton *button, gpointer user_data)
{
		gtk_widget_hide((GtkWidget*)settings_dialog) ;
} ;
//-----------------------------------------------------------------------------



enum {
		ID,
		NAME,
		RESOLUTION,
		REFRESH_TIME,
		CACHED,
		N_POINTS_BUFFERED,
		SENSOR_PRESENT,
		N_COLUMNS
} ;


//-----------------------------------------------------------------------------

GtkListStore *t_sensor_list_new_store(GList *list)
{
		GtkListStore *store ;
		GtkTreeIter   iter;
		unsigned int i = 0 ;
		
		store = gtk_list_store_new (N_COLUMNS,       /* Total number of columns */
																G_TYPE_STRING,
																G_TYPE_STRING,
																G_TYPE_INT,
																G_TYPE_INT,
																G_TYPE_BOOLEAN,
																G_TYPE_INT,
																G_TYPE_BOOLEAN);
		
		GList *sensor = list ;
		while (sensor != NULL) {
				
				gtk_list_store_append (store, &iter);  
				gtk_list_store_set (store, &iter,
														ID, ((T_Sensor *)(sensor->data))->sensor_id->str,
														NAME, ((T_Sensor *)(sensor->data))->name->str,
														RESOLUTION, ((T_Sensor *)(sensor->data))->resolution,
														REFRESH_TIME, ((T_Sensor *)(sensor->data))->read_interval,
														CACHED, ((T_Sensor *)(sensor->data))->cached,
														N_POINTS_BUFFERED, ((T_Sensor *)(sensor->data))->sample_buffer->len,
														SENSOR_PRESENT, is_sensor_present(*((T_Sensor *)(sensor->data))),
														-1);			
				
				
				
				sensor = g_list_next(sensor) ;
				i++ ;
		} ;
		
		return store ;
} ;
//-----------------------------------------------------------------------------
void rebuild_sensors_tree(gboolean force_rebuild) 
{
		if (force_rebuild ||
				(GTK_WIDGET_VISIBLE((GtkWidget*)settings_dialog) &&
				 gtk_notebook_get_current_page(settings_notebook) == 1)) 		// FIXME: uwazac na numerki
		{
				//printf("Przebudowa wyswietlacza listy czujnikow\n") ;
				// usun istniejace kolumny
						GList *columns = gtk_tree_view_get_columns(sensors_manage_display_tree) ;
				GList *col = g_list_last(columns) ;
				while (col != NULL) {
						gtk_tree_view_remove_column(sensors_manage_display_tree, col->data) ;
						col = g_list_previous(col) ;
				} ;
				g_list_free(columns) ;
				gtk_tree_view_set_model(sensors_manage_display_tree, NULL) ;		// chyba nadmiar
						
						// utworz model
						GtkListStore *store ;
				store = t_sensor_list_new_store(sensors) ;
				
				gtk_tree_view_set_model(sensors_manage_display_tree, GTK_TREE_MODEL(store)) ;
				
				///////////////////////////////////
						GtkCellRenderer *renderer ;
				GtkTreeViewColumn *column ;
				
				renderer = gtk_cell_renderer_text_new ();
				column = gtk_tree_view_column_new_with_attributes ("Identyfikator",
																													 renderer,
																													 "text", ID,
																													 NULL);
				gtk_tree_view_append_column (GTK_TREE_VIEW (sensors_manage_display_tree), column);
				
				renderer = gtk_cell_renderer_text_new ();
				column = gtk_tree_view_column_new_with_attributes ("Nazwa",
																													 renderer,
																													 "text", NAME,
																													 NULL);
				gtk_tree_view_append_column (GTK_TREE_VIEW (sensors_manage_display_tree), column);
				
				renderer = gtk_cell_renderer_text_new ();
				column = gtk_tree_view_column_new_with_attributes ("Rozdzielczość",
																													 renderer,
																													 "text", RESOLUTION,
																													 NULL);
				gtk_tree_view_append_column (GTK_TREE_VIEW (sensors_manage_display_tree), column);
				
				renderer = gtk_cell_renderer_text_new ();
				column = gtk_tree_view_column_new_with_attributes ("Odświeżanie",
																													 renderer,
																													 "text", REFRESH_TIME,
																													 NULL);
				gtk_tree_view_append_column (GTK_TREE_VIEW (sensors_manage_display_tree), column);
				
				renderer = gtk_cell_renderer_toggle_new ();
				column = gtk_tree_view_column_new_with_attributes ("Buforowany",
																													 renderer,
																													 "active", CACHED,
																													 NULL);
				gtk_tree_view_append_column (GTK_TREE_VIEW (sensors_manage_display_tree), column);
				
				renderer = gtk_cell_renderer_text_new ();
				column = gtk_tree_view_column_new_with_attributes ("Liczba punktów",
																													 renderer,
																													 "text", N_POINTS_BUFFERED,
																													 NULL);
				gtk_tree_view_append_column (GTK_TREE_VIEW (sensors_manage_display_tree), column);
				
				renderer = gtk_cell_renderer_toggle_new ();
				column = gtk_tree_view_column_new_with_attributes ("Dostępny",
																													 renderer,
																													 "active", SENSOR_PRESENT,
																													 NULL);
				gtk_tree_view_append_column (GTK_TREE_VIEW (sensors_manage_display_tree), column);
				
				// usuniecie selekcji
						GtkTreeSelection *selection;		
				selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(sensors_manage_display_tree));
				
				gtk_tree_selection_unselect_all (selection) ;
				gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE) ;
				
				//g_object_unref(renderer) ;		// chyba nie mozna, doczytac !!!
						g_object_unref(store) ;	
		} ;
} ;
//-----------------------------------------------------------------------------
void on_panel_menu_item_settings_activate (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
		gtk_notebook_set_current_page(settings_notebook, 0) ;
		rebuild_general_settings_notebook_page() ;
		gtk_widget_show((GtkWidget*)settings_dialog) ;
		rebuild_sensors_tree(FALSE) ;
} ;
//-----------------------------------------------------------------------------

extern GtkButton 							*settings_manage_apply_button ;

void on_notebook1_switch_page (GtkNotebook     *notebook,
															 GtkNotebookPage *page,
															 guint            page_num,
															 gpointer         user_data) 
{
																	 if (page_num == 1) {		// FIXME: uwazac na te numerki...
																					 
																					 gtk_widget_hide ((GtkWidget*)settings_manage_apply_button) ;
																			 rebuild_sensors_tree(TRUE) ;
																			 
																	 } else if (page_num == 0) {
																			 
																			 gtk_widget_show ((GtkWidget*)settings_manage_apply_button) ;
																			 rebuild_general_settings_notebook_page() ;
																	 } ;
															 } ;
//-----------------------------------------------------------------------------
// po uzyciu wynik funkcji zwolnic jawnie za pomoca g_free()
gchar *display_tree_get_selected_name(void)
{
		GtkTreeSelection *selection;
		GtkTreeModel *model;
		GtkTreeIter iter;
		/* This will only work in single or browse selection mode! */
				selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(sensors_manage_display_tree));
		
		gchar *name = NULL ;
		if (gtk_tree_selection_get_selected(selection, &model, &iter))
		{
				gtk_tree_model_get (model, &iter, NAME, &name, -1);
		} ;
		
		return name ;
} ;


//-----------------------------------------------------------------------------
void rebuild_sensor_settings_win(void) 
{
		GList *l, *ltmp ;
		
		reset_sensor_settings_combos() ;
		
		l = get_system_sensor_ids() ;
		ltmp = l ;
		while (ltmp != NULL) {
				gtk_combo_box_append_text(sensors_manage_id_combo,
																	((GString*)(ltmp->data))->str) ;
				ltmp = g_list_next(ltmp) ;
		} ;
		destroy_gstring_list(l) ;
} ;
//-----------------------------------------------------------------------------

gulong signal_handler ;		// dla usuwania callbackow okna zarzadzania czujnikiem


void modify_sensor_from_dialog_win(void)
{
		modify_sensor_with_name (gtk_entry_get_text(sensors_manage_name_entry), 
														 gtk_combo_box_get_active_text(sensors_manage_id_combo), 
														 gtk_combo_box_get_active(sensors_manage_resolution_combo) + 9, // FIXME: ugly hack
														 gtk_spin_button_get_value_as_int(sensors_manage_refresh_time_entry),
														 gtk_toggle_button_get_active((GtkToggleButton *)sensors_manage_cached_button) ) ;
		rebuild_sensors_tree(TRUE) ;
} ;
//-----------------------------------------------------------------------------
void on_sensors_manage_change_button_clicked (GtkButton *button, gpointer user_data)
{
		gtk_window_set_title ((GtkWindow*)sensor_operations_dialog,"Modyfikuj czujnik") ;
		
		gchar *name = display_tree_get_selected_name() ;
		
		if (name != NULL) {
				
				GList *s = find_sensor_with_name(name) ;
				
				gtk_entry_set_text		(sensors_manage_name_entry, ((T_Sensor*)(s->data))->name->str) ;
				gtk_editable_set_editable ((GtkEditable*)sensors_manage_name_entry, FALSE) ;
				
				gtk_spin_button_set_value(sensors_manage_refresh_time_entry, ((T_Sensor*)(s->data))->read_interval) ;
				
				// FIXME: ugly hack
						gtk_combo_box_set_active(sensors_manage_resolution_combo, ((T_Sensor*)(s->data))->resolution - 9) ;
				
				gtk_toggle_button_set_active((GtkToggleButton *)sensors_manage_cached_button, ((T_Sensor*)(s->data))->cached) ;
				
				// dla biezacego sensor_id znalezc odpowiadajacy indeks w combo albo
						// dodac biezacy sensor_id do combo, co pozwoli zachowac identyfikatory
						// czujnikow chwilowo nieobecnych w systemie
						rebuild_sensor_settings_win() ;
				GtkTreeModel *m = gtk_combo_box_get_model (sensors_manage_id_combo);
				
				int idx = -1, i=0 ;
				GtkTreeIter iter;			
				gboolean valid = gtk_tree_model_get_iter_first (m, &iter);
				while (valid) {
						/* Walk through the list, reading each row */
								gchar *str_data;
						
						/* Make sure you terminate calls to gtk_tree_model_get()
								* with a '-1' value
								*/
								gtk_tree_model_get (m, &iter, 
																		0, &str_data,
																		-1);
						
						/* Do something with the data */
								//printf("%s\n",str_data) ;
						if (strcmp(str_data, ((T_Sensor*)(s->data))->sensor_id->str) == 0) {
								idx = i ;
						} ;
						
						g_free (str_data);
						
						valid = gtk_tree_model_iter_next (m, &iter);
						i++ ;
				} ;
				
				if (idx > -1) {
						gtk_combo_box_set_active(sensors_manage_id_combo, idx) ;
				} else {
						gtk_combo_box_append_text(sensors_manage_id_combo, 
																			((T_Sensor*)(s->data))->sensor_id->str) ;
						gtk_combo_box_set_active(sensors_manage_id_combo, i) ; // ostatni
				} ;
				//-------------------------------
						
						signal_handler = g_signal_connect (G_OBJECT (sensor_operation_dialog_ok_button), "clicked",
																							 G_CALLBACK (modify_sensor_from_dialog_win), NULL);		
				
				gtk_widget_show((GtkWidget*)sensor_operations_dialog) ;
				
				g_free(name) ;
		} ;
} ;
//-----------------------------------------------------------------------------
void on_sensors_manage_delete_button_clicked (GtkButton *button, gpointer user_data)
{
		gchar *name ;
		
		name = display_tree_get_selected_name() ;
		
		remove_sensor_with_name(name) ;
		
		g_free(name);
		
		rebuild_sensors_tree(TRUE) ;
} ;
//-----------------------------------------------------------------------------
void add_sensor_from_dialog_win(void)
{
		add_sensor (gtk_entry_get_text(sensors_manage_name_entry), 
								gtk_combo_box_get_active_text(sensors_manage_id_combo), 
								gtk_combo_box_get_active(sensors_manage_resolution_combo) + 9, // FIXME: ugly hack
								gtk_spin_button_get_value_as_int(sensors_manage_refresh_time_entry),
								gtk_toggle_button_get_active((GtkToggleButton *)sensors_manage_cached_button) ) ; 
		rebuild_sensors_tree(TRUE) ;
} ;
//-----------------------------------------------------------------------------
void on_sensors_manage_new_button_clicked (GtkButton *button, gpointer user_data)
{
		gtk_window_set_title ((GtkWindow*)sensor_operations_dialog,"Nowy czujnik") ;
		
		rebuild_sensor_settings_win() ;
		gtk_entry_set_text		(sensors_manage_name_entry, "") ;
		gtk_editable_set_editable ((GtkEditable*)sensors_manage_name_entry, TRUE) ;
		gtk_combo_box_set_active(sensors_manage_resolution_combo, 0) ;		
		gtk_spin_button_set_value(sensors_manage_refresh_time_entry, 5.0) ;
		gtk_toggle_button_set_active((GtkToggleButton *)sensors_manage_cached_button, FALSE) ;
		
		signal_handler = g_signal_connect (G_OBJECT (sensor_operation_dialog_ok_button), 
																			 "clicked",
																			 G_CALLBACK (add_sensor_from_dialog_win), NULL);		
		
		gtk_widget_show((GtkWidget*)sensor_operations_dialog) ;
} ;
//-----------------------------------------------------------------------------
void on_sensor_operations_dialog_close (GtkDialog *dialog, gpointer user_data)
{
		// dziwne, ale kolejnosc chyba ma znaczenie...
				if (g_signal_handler_is_connected (G_OBJECT (sensor_operation_dialog_ok_button), 
																					 signal_handler))
				g_signal_handler_disconnect (G_OBJECT (sensor_operation_dialog_ok_button), 
																		 signal_handler);
		gtk_widget_hide((GtkWidget*)sensor_operations_dialog) ;
} ;
//------------------------------------------------------------------------------
