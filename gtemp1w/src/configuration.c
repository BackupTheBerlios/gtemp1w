
#include <stdio.h>
#include <libconfig.h>
#include <glib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "configuration.h"
#include "t_panel.h"
#include "sensors.h"
#include "general_settings.h"
config_t config ;


//------------------------------------------------------------------------------
int make_config_file_backup(const char *f_name)
{
		int err_code = 0 ;
		
		time_t t = time(NULL) ;
		struct tm *lt ;
		lt = localtime(&t) ;	
		char time_str[250] ;
		
		// przyrostek zalezny od momentu utworzenia kopii zapasowej
		strftime(time_str, sizeof(time_str), "%F_%T", lt) ;
		// tworzenie nowej nazwy pliku kopii
		GString *new_name = g_string_new(f_name) ;
		g_string_append_printf(new_name,"_bckp_%s", time_str) ;
		
		// kopiowanie zawartosci pliku
		int f_old = -1, f_new = -1 ;
		f_old = open(f_name, O_RDONLY) ;
		if (f_old < 0) {
				perror("make_config_file_backup 1") ;
				err_code = -1 ;
				goto exception ;
		} ;
		f_new = open(new_name->str,O_WRONLY | O_TRUNC | O_CREAT, S_IREAD | S_IWRITE) ;
		if (f_new < 0) {
				perror("make_config_file_backup 2") ;
				err_code = -1 ;
				goto exception ;
		} ;
		
		char c ;
		while (read(f_old, &c, 1) > 0) {	// 1 bezpieczniej niz sizeof(char)
				write(f_new,&c,1) ;
		} ;
		
		
		exception:
				close(f_old) ;
		close(f_new) ;
		g_string_free(new_name, TRUE) ;
		return err_code ;
} ;
//------------------------------------------------------------------------------
int save_configuration_to_file(const char *f_name) 
{
		config_setting_t *setting = NULL;
		
		
		config_init(&config) ;
		
		setting = config_setting_add(config.root, "Panel", CONFIG_TYPE_GROUP) ;
		t_panel_save_configuration (setting) ;
		
		setting = config_setting_add(config.root, "Sensors", CONFIG_TYPE_GROUP) ;
		sensors_save_configuration (setting) ;
		
		setting = config_setting_add(config.root, "General_Settings", CONFIG_TYPE_GROUP) ;
		general_settings_save_configuration (setting) ;
		
		
		make_config_file_backup(f_name) ;
		config_write_file (&config, f_name) ;
		
		config_destroy (&config) ;
		return 0 ;
} ;
//------------------------------------------------------------------------------
int load_configuration_from_file (const char *f_name) 
{
		config_setting_t *panel = NULL ;
		config_setting_t *sensors = NULL ;
		config_setting_t *general_settings = NULL ;
		
		config_init(&config) ;
		config_read_file(&config, f_name) ;
		
		general_settings = config_lookup(&config, "General_Settings") ;
		if (general_settings == NULL) {
				printf ("load_configuration_from_file: nie mozna zaladowac konfiguracji ogólnej\n") ;
				//config_destroy (&config) ;
				//return -1 ;
		} ;
		general_settings_load_configuration(general_settings) ;	// to musi byc wywolane zawsze !!!	
				
				panel = config_lookup(&config, "Panel") ;
		if (panel == NULL) {
				printf ("load_configuration_from_file: nie mozna zaladowac konfiguracji panelu\n") ;
				//config_destroy (&config) ;
				//return -1 ;
		} ;
		t_panel_load_configuration(panel) ;
		
		sensors = config_lookup(&config, "Sensors") ;
		if (sensors == NULL) {
				printf ("load_configuration_from_file: nie mozna zaladowac konfiguracji czujników\n") ;
				//config_destroy (&config) ;
				//return -1 ;
		} ;
		sensors_load_configuration(sensors) ;
		
		
		
		
		config_destroy (&config) ;
		return 0 ;
} ;
//------------------------------------------------------------------------------
