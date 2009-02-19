#ifndef __POPUP_MENU_H__
#define __POPUP_MENU_H__

#include <glade/glade.h>

#include "t_display.h"

extern T_Display *disp ;

int register_popup_menu (GladeXML *xml) ;
void switch_display(T_Display *d) ;
void show_menu_display_graphical(GdkEventButton *event) ;
void show_menu_display_alphanumeric(GdkEventButton *event) ;


#endif
