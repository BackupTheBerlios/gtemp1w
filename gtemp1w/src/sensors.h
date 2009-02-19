#ifndef _SENSORS_H
#define _SENSORS_H

#include <glib.h>
#include <glade/glade.h>
#include <libconfig.h>

#include "t_sensor.h"

G_BEGIN_DECLS


void register_sensors_dialog (GladeXML *xml) ;


/*
Funkcja zwaraca identyfikatory wszystkich dostepnych czujnikow. Identyfikatory
sa zapisane w postaci ciagow znakow typu GString.
*/
GList *get_system_sensor_ids(void) ;

GList *get_program_sensor_names(void) ;

GList *destroy_gstring_list(GList *l) ;


// listy nie wolno kasowac za pomoca destroy_gstring_list()
GList *get_sensor_to_read_list(void) ;
GList *destroy_sensor_list(GList *sensor_list) ;

void add_sample_to_sensor(T_Sample sample, const char *sensor_name) ;

void append_sensor_samples_to_array (GArray *samples, const char *s_name,
																		 time_t t_begin, time_t t_end) ;

int sensors_save_configuration (config_setting_t *setting) ;
int sensors_load_configuration (config_setting_t *setting) ;


void save_all_sensors_data (void) ;

G_END_DECLS

#endif /* _SENSORS_H */

