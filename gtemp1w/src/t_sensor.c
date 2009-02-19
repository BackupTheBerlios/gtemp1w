#include "t_sensor.h"
#include "t_panel.h"

//------------------------------------------------------------------------------
T_Sensor *t_sensor_new(const char *name, const char *id, T_Resolution res, 
											 time_t read_interval, gboolean cached)
{
		T_Sensor *s = g_malloc0(sizeof(T_Sensor)) ;
		
		s->sensor_id = g_string_new(id) ;
		s->name = g_string_new(name) ;
		
		s->resolution = res ;
		s->read_interval = read_interval ;
		s->cached = cached ;
		
		s->sample_buffer = g_array_new(FALSE, TRUE, sizeof(T_Sample)) ;
		s->last_read_time = 0 ;
		
		return s ;
} ;
//------------------------------------------------------------------------------
void t_sensor_destroy (T_Sensor *s)
{
		g_array_free(s->sample_buffer, TRUE) ;
		
		g_string_free(s->name, TRUE) ;
		g_string_free(s->sensor_id, TRUE) ;
		
		g_free(s) ;
} ;
//------------------------------------------------------------------------------
int t_sensor_save_configuration (config_setting_t *setting, T_Sensor *sensor)
{
		config_setting_t *s, *sp = NULL ;
		
		s = config_setting_add(setting, NULL, CONFIG_TYPE_GROUP) ;
		
		sp = config_setting_add(s, "sensor_id", CONFIG_TYPE_STRING) ;
		config_setting_set_string(sp, sensor->sensor_id->str);
		sp = config_setting_add(s, "name", CONFIG_TYPE_STRING) ;
		config_setting_set_string(sp, sensor->name->str);
		sp = config_setting_add(s, "resolution", CONFIG_TYPE_INT) ;
		config_setting_set_int(sp, sensor->resolution);
		sp = config_setting_add(s, "read_interval", CONFIG_TYPE_INT) ;
		config_setting_set_int(sp, sensor->read_interval);
		sp = config_setting_add(s, "cached", CONFIG_TYPE_BOOL) ;
		config_setting_set_bool(sp, sensor->cached);
		
		return 0 ;
} ;
//------------------------------------------------------------------------------
gboolean t_sensor_need_read(T_Sensor *s)
{
		if (s->read_interval < 0)
				return FALSE ;
		
		time_t now = time(NULL) ;
		
		if (s->last_read_time + s->read_interval <= now) 
				return TRUE ;
		else
				return FALSE ;
} ;
//------------------------------------------------------------------------------
int t_sensor_add_sample(T_Sensor *sensor, T_Sample sample) 
{
		/*if (sensor == NULL) {
				printf ("t_sensor_add_sample : NULL pointer\n") ;
				return FALSE ;
		} ;*/
				
				g_array_append_val(sensor->sample_buffer, sample) ;
		sensor->last_read_time = sample.czas ;
		
		// tutaj trzeba chyba jeszcze przejsc po wszystkich wyswietlaczach....
				t_panel_add_point(sensor->name->str, &sample) ;
		
		return TRUE ;
} ;
//------------------------------------------------------------------------------
GArray *t_sensor_remove_samples(T_Sensor *sensor) 
{
		GArray *result = sensor->sample_buffer ;
		sensor->sample_buffer = g_array_new(FALSE, TRUE, sizeof(T_Sample)) ;
		
		return result ;
} ;
//------------------------------------------------------------------------------
void t_sensor_add_samples_to_array (const T_Sensor* sensor, GArray *samples, 
																		time_t t_begin, time_t t_end) 
{
		unsigned i ;
		T_Sample *smpl ;
		//printf ("t_sensor_add_samples_to_array: ENTER\n") ;
		
		for (i=0; i< sensor->sample_buffer->len; i++) {
				
				smpl = &g_array_index(sensor->sample_buffer, T_Sample, i) ;
				
				if (smpl->czas > t_end)
						break ;
				
				if (smpl->czas >= t_begin) {
						//printf (".") ;
						g_array_append_val(samples, *smpl) ;
				} ;
				
		} ;
		//printf ("EXIT\n") ;
} ;
