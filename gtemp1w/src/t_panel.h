#ifndef __T_PANEL_H__
#define __T_PANEL_H__

#include "t_display.h"
#include <glade/glade.h>
#include <libconfig.h>

/*

*/

int register_panel (GladeXML *xml) ;

int t_panel_add_display(T_Display *d,unsigned x, unsigned y) ;
T_Display* get_display_with_layout (GtkLayout *l) ;

void t_panel_recalculate_size(void) ;

void t_panel_remove_display (T_Display *d)  ;

int t_panel_add_point (const char *sensor_name, T_Sample *point) ;

int t_panel_save_configuration (config_setting_t *setting) ;
int t_panel_load_configuration (config_setting_t *panel) ;

#endif
