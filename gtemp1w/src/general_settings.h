#ifndef _GENERAL_SETTINGS_H
#define _GENERAL_SETTINGS_H

#include <glib.h>
#include <glade/glade.h>
#include <libconfig.h>

G_BEGIN_DECLS

extern GString *path_1_wire_directory ;
extern GString *path_data_directory ;
extern unsigned int write_samples_interval ;


int register_general_settings_dialog (GladeXML *xml) ;
void rebuild_general_settings_notebook_page(void) ;

int general_settings_save_configuration (config_setting_t *setting) ;
int general_settings_load_configuration (config_setting_t *setting) ;


G_END_DECLS

#endif /* _GENERAL_SETTINGS_H */

