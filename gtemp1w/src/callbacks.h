/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
* callbacks.h
* Copyright (C) tad 2009 <tad@>
* 
* callbacks.h is free software.
* 
* You may redistribute it and/or modify it under the terms of the
* GNU General Public License, as published by the Free Software
* Foundation; either version 2 of the License, or (at your option)
* any later version.
* 
* callbacks.h is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with callbacks.h.  If not, write to:
* 	The Free Software Foundation, Inc.,
* 	51 Franklin Street, Fifth Floor
* 	Boston, MA  02110-1301, USA.
*/

#include <gtk/gtk.h>

void on_window_destroy_event(GtkWidget *widget) ;
void on_alignment1_size_allocate (GtkWidget     *widget,
								  GtkAllocation *allocation) ; 
gboolean  read_temp  () ;
void on_plotno_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data) ;
gint on_plotno_button_press_event(GtkWidget *widget, 
								  GdkEventButton *event, 
								  gpointer data) ;
void user_function  (GtkMenuItem *menuitem, gpointer     user_data) ;
