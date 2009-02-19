## Process this file with automake to produce Makefile.in

## Created by Anjuta

gladedir = $(datadir)/linlamp/glade
glade_DATA = linlamp.glade

INCLUDES = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(LINLAMP_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = linlamp

linlamp_SOURCES = \
	callbacks.c \
	callbacks.h \
	main.c \
	t_display.c \
	t_display.h \
	t_plot.c \
	t_plot.h \
	t_panel.c \
	t_panel.h \
	popup_menu.c \
	popup_menu.h \
	display_add_plot.c \
	display_add_plot.h \
	configuration.c \
	configuration.h \
	t_sensor.c \
	t_sensor.h \
	measures.c \
	measures.h \
	sensors.c \
	sensors.h \
	sensor_read_thread.c \
	sensor_read_thread.h \
	sample_load_save.c \
	sample_load_save.h \
	t_display_alphanumeric.c \
	t_display_alphanumeric.h \
	general_settings.c \
	general_settings.h

linlamp_LDFLAGS = \
	-export-dynamic

linlamp_LDADD = $(LINLAMP_LIBS)

EXTRA_DIST = $(glade_DATA)
