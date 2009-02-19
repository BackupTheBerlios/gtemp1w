#ifndef _SENSOR_READ_THREAD_H
#define _SENSOR_READ_THREAD_H

#include <glib.h>
#include "t_sensor.h"


G_BEGIN_DECLS

gpointer *read_thread_func (gpointer data) ;

//
//	Uwaga - funkcja moze byc wywolywana z innych watkow
// 	parametr przez wartosc 
//
gboolean is_sensor_present(const T_Sensor sensor) ;

G_END_DECLS

#endif /* _SENSOR_READ_THREAD_H */

