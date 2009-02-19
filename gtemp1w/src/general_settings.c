#include "general_settings.h"

#include <gtk/gtk.h>


// ustawienia globalne

#define SENSOR_PATH_PREFIX "/var/1-Wire/"
GString *path_1_wire_directory ;

#define DATA_STOTE_PATH "/home/tad/tmp/data/"
GString *path_data_directory ;

#define WRITE_SAMPLES_INTERVAL 10
unsigned int write_samples_interval ;


GtkButton 							*settings_manage_apply_button ;

GtkSpinButton 					*settings_write_interval_sec_spin ;
GtkSpinButton 					*settings_write_interval_min_spin ;
GtkSpinButton 					*settings_write_interval_hour_spin ;

GtkFileChooserButton		*settings_data_path_button ;
GtkEntry 								*settings_data_path_entry ;

GtkFileChooserButton 		*settings_1_wire_path_button ;
GtkEntry 								*settings_1_wire_path_entry ;






//------------------------------------------------------------------------------
int init_general_settings(void)
{
		path_data_directory = g_string_new(DATA_STOTE_PATH) ;
		path_1_wire_directory = g_string_new(SENSOR_PATH_PREFIX) ;
		write_samples_interval = WRITE_SAMPLES_INTERVAL ;
		
		return 0 ;
} ;
//------------------------------------------------------------------------------
int register_general_settings_dialog (GladeXML *xml) 
{
		settings_manage_apply_button	= (GtkButton*)glade_xml_get_widget (xml, "settings_manage_apply_button");
		settings_data_path_button 		= (GtkFileChooserButton*)glade_xml_get_widget (xml, "settings_data_path_button");
		settings_data_path_entry 			= (GtkEntry*)glade_xml_get_widget (xml, "settings_data_path_entry"); 						
		settings_1_wire_path_button		= (GtkFileChooserButton*)glade_xml_get_widget (xml, "settings_1_wire_path_button");
		settings_1_wire_path_entry 		= (GtkEntry*)glade_xml_get_widget (xml, "settings_1_wire_path_entry"); 						
		
		settings_write_interval_sec_spin	= (GtkSpinButton*)glade_xml_get_widget (xml, "settings_write_interval_sec_spin");
		settings_write_interval_min_spin 	= (GtkSpinButton*)glade_xml_get_widget (xml, "settings_write_interval_min_spin");
		settings_write_interval_hour_spin	= (GtkSpinButton*)glade_xml_get_widget (xml, "settings_write_interval_hour_spin");
		
		return 0 ;
} ;
//------------------------------------------------------------------------------
void rebuild_general_settings_notebook_page(void)
{
		gtk_entry_set_text (settings_data_path_entry, path_data_directory->str) ;
		gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER(settings_data_path_button), path_data_directory->str) ;
		
		
		gtk_entry_set_text (settings_1_wire_path_entry, path_1_wire_directory->str) ;
		gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER(settings_1_wire_path_button), path_1_wire_directory->str) ;
		
		unsigned int sec, min, hour ;
		
		sec = write_samples_interval % 60 ;
		hour = (write_samples_interval) / 3600 ;
		min = (write_samples_interval - hour*3600 - sec) / 60 ;
		
		gtk_spin_button_set_value (settings_write_interval_sec_spin, sec) ;
		gtk_spin_button_set_value (settings_write_interval_min_spin, min) ;
		gtk_spin_button_set_value (settings_write_interval_hour_spin, hour) ;
		
} ;
//------------------------------------------------------------------------------
int set_general_settings_from_notebook (void)
{
		g_string_assign(path_1_wire_directory, gtk_entry_get_text (settings_1_wire_path_entry)) ;
		g_string_assign(path_data_directory, gtk_entry_get_text (settings_data_path_entry)) ;
		
		write_samples_interval = 
				gtk_spin_button_get_value (settings_write_interval_sec_spin) +
				gtk_spin_button_get_value (settings_write_interval_min_spin) * 60 + 
				gtk_spin_button_get_value (settings_write_interval_hour_spin) * 3600 ;
		
		return 0 ;
} ;
//------------------------------------------------------------------------------
void on_settings_manage_apply_button_clicked (void)
{
		set_general_settings_from_notebook() ;
		rebuild_general_settings_notebook_page() ;
} ;
//------------------------------------------------------------------------------
void on_settings_data_path_button_current_folder_changed (GtkFileChooser *filechooser,
																													gpointer user_data)
{
		gchar *data_folder = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(settings_data_path_button)) ;
		
		gtk_entry_set_editable (settings_data_path_entry, TRUE) ;
		gtk_entry_set_text (settings_data_path_entry, data_folder) ;
		int pos=-1 ;
		gtk_editable_insert_text  ((GtkEditable *)settings_data_path_entry,"/",1, &pos) ;
		gtk_entry_set_editable (settings_data_path_entry, FALSE) ;
		
		g_free(data_folder) ;
} ;
//------------------------------------------------------------------------------
void on_settings_1_wire_path_button_current_folder_changed (GtkFileChooser *filechooser,
																														gpointer user_data)
{
		gchar *one_w_folder = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(settings_1_wire_path_button)) ;
		
		gtk_entry_set_editable (settings_1_wire_path_entry, TRUE) ;
		gtk_entry_set_text (settings_1_wire_path_entry, one_w_folder) ;
		int pos=-1 ;
		gtk_editable_insert_text  ((GtkEditable *)settings_1_wire_path_entry,"/",1, &pos) ;
		gtk_entry_set_editable (settings_1_wire_path_entry, FALSE) ;
		
		g_free(one_w_folder) ;
} ;
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int general_settings_save_configuration (config_setting_t *setting) 
{
		config_setting_t *s = NULL ;
		
		s = config_setting_add(setting, "path_1_wire_directory", CONFIG_TYPE_STRING) ;
		if (s != NULL) config_setting_set_string(s, path_1_wire_directory->str);
		s = config_setting_add(setting, "path_data_directory", CONFIG_TYPE_STRING) ;
		if (s != NULL) config_setting_set_string(s, path_data_directory->str);
		s = config_setting_add(setting, "write_samples_interval", CONFIG_TYPE_INT) ;
		if (s != NULL) config_setting_set_int(s, write_samples_interval);
		
		return 0 ;
} ;
//------------------------------------------------------------------------------
int general_settings_load_configuration (config_setting_t *setting)
{
		config_setting_t *s = NULL ;
		
		init_general_settings() ;
		
		s = config_setting_get_member(setting, "path_1_wire_directory") ;
		if (s != NULL) {
				g_string_assign (path_1_wire_directory, config_setting_get_string(s)) ;
		} ;
		s = config_setting_get_member(setting, "path_data_directory") ;
		if (s != NULL) {
				g_string_assign (path_data_directory, config_setting_get_string(s)) ;
		} ;
		s = config_setting_get_member(setting, "write_samples_interval") ;
		if (s != NULL) write_samples_interval = config_setting_get_int(s) ;
		
		return 0 ;
} ;
