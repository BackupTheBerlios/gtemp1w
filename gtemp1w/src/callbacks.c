/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * callbacks.c
 * Copyright (C) tad 2009 <tad@>
 * 
 * callbacks.c is free software.
 * 
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 * callbacks.c is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with callbacks.c.  If not, write to:
 * 	The Free Software Foundation, Inc.,
 * 	51 Franklin Street, Fifth Floor
 * 	Boston, MA  02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>



#include "callbacks.h"
#include "t_display.h"
#include "popup_menu.h"

//extern T_Display display ;


//-----------------------------------------------------------------------------
void on_window_destroy_event(GtkWidget *widget)
{
	printf("EXIT\n") ;
	gtk_main_quit();
} ;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


void on_plotno1_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{  
	//printf ("on_plotno_expose_event\n") ;
	/*
	GdkColor color;
	color.red = 383;
	color.green = 383;
	color.blue = 383;
	*/
	
	/*	gdk_draw_arc (GTK_LAYOUT(widget)->bin_window,
	widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
	TRUE,
	0, 0, widget->allocation.width, widget->allocation.height,
	0, 64 * 360);
	*/
	
	//t_display_przerysuj(&display) ;
	
	//  return TRUE;
} ;
//-----------------------------------------------------------------------------

//extern GtkMenu *menu_1 ;

//~ gint on_plotno1_button_press_event(GtkWidget *widget, GdkEventButton *event, 
		   						  //~ gpointer data)
//~ {
	  //~ if (event->type == GDK_BUTTON_PRESS) {
		//~ //printf ("PRESS button %d at %f, %f\n", event->button, event->x, event->y) ;
		  //~ switch_display(&display) ;
 		//~ gtk_menu_popup (menu_1, NULL, NULL, NULL, NULL, event->button, event->time) ;
        //~ /* Tell calling code that we have handled this event; the buck
         //~ * stops here. */
        //~ return TRUE;
    //~ }

    //~ /* Tell calling code that we have not handled this event; pass it on. */
    //~ return FALSE;
//~ } ;

