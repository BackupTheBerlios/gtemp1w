/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
* main.c
* Copyright (C) tad 2009 <tad@>
* 
* main.c is free software.
* 
* You may redistribute it and/or modify it under the terms of the
* GNU General Public License, as published by the Free Software
* Foundation; either version 2 of the License, or (at your option)
* any later version.
* 
* main.c is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with main.c.  If not, write to:
* 	The Free Software Foundation, Inc.,
* 	51 Franklin Street, Fifth Floor
* 	Boston, MA  02110-1301, USA.
*/
/*
Zalozenia:


P O M I A R Y


Pomiar (odczyt danych z czujnika) realizowany jest co ustalony czas
przy pomocy funkcji g_timeout_add_seconds. Powinno to się odbywac w niezaleznym
watku, aby nie blokowac wyswietlania. Co wiecej, odczyt WSZYSTKICH czujnikow
powinien byc przeprowadzony kolejno przez jedna funkcje, aby znac mniej wiecej
dokladny czas pomiaru. Kazdy udany odczyt powinien zakonczyc sie wyslaniem
komunikatu zawierajacego odczytana wartosc i czas do odpowiedniej klasy 
zawierajacej dane konkretnego czujnika. Po otrzymaniu komunikatu klasa ta
powinna rozeslac polecenie dodania wartosci do odpowiednich wyswietlaczy.

Potrzebne jest wiec:

Jedna globalna tablica zawierajaca klasy odpowiedzialne za przechowanie wynikow
z konkretnych czujnikow. Kazda klasa powinna przechowywac wyniki od ostatniego 
zapisu na dysk. Dla kazdej klasy nalezy wiec zaimplementowac funkcje dodania
wartosci (z czasem odczytu), zapisu wartosci do plikow (lub moze lepiej pobrania
tablicy pomiarow do osobnej klasy odpowiedzialnej za zarzadzanie plikami), oraz
usuniecia wszystkich dotychczasowych pomiarow. 

UWAGA - Po przemysleniach wydaje sie, ze ten pomysl jest dobry, tzn. klasa 
przechowujaca wyniki pomiarow przechowuje wyniki od ostatniego zapisu na dysk. 
Jesli wykresy wymagaja danych wczesniejszych (np. po zmianie zakresow), to 
powinny same pobrac sobie dane z dysku. To podejscie ma te zalete, ze mozliwe 
bedzie wyswietlanie danych biezacych i historycznych przy pomocy tego samego 
kodu. Oczywyscie w tym wypadku klasa odpowiedzialna za zarzadzanie plikami
dyskowymi musi udostepniac funkcje pozwalajaca pobrac dla konkretnego czujnika
dane z pliku z okreslonego zakresu czasowego.

Z kazda klasa przechowujaca wyniki pomiarow musza byc zwiazane dwa identyfikatory
czujnika: numerek sprzetowy, ktory moze byc czesto zmieniany oraz identyfikator
uzywany w programie, ktory w oryginalnym Lampomittari jest uzywany m.in. jako
nazwa pliku. Z oczywistych powodow identyfikator wewnetrzny programu zmieniany
bedzie raczej rzadko, bo wymaga to zmiany nazw wszystkich plikow dyskowych. 
Identyfikator wewnetrzny powinien byc jedyna wartoscia identyfikujaca czujnik
wewnatrz programu. Numerki zaszyte w czujniku powinny byc uzywane jednynie
w funkcjach zajmujacych sie fizycznym odczytem czujnikow.
Dobrze by tez bylo przechowac dla kazdego czujnika jego wlasy interwal, 
w ktorym przeprowadzane sa pomiary, ale to skomplikuje znacznie kod, bo kazdy
pomiar bedzie musial byc wykonywany w osobnym watku. Dodatkowo za czas wykonania
pomiaru trzeba by brac odczyt PO powrocie z funkcji czytającej.

Globalna tablica klas przechowujacych wyniki pomiarow powinna udostepniac
interfejs pozwalajacy na pobranie liczby klas, pobranie z kazdej klasy 
identyfikatora czujnika oraz wskaznika funkcji wprowadzajacej dane do klasy
przechowujacej pomiary. Dane te beda niezbedne do odczytu wszystkich czujnikow
co okreslony przedzial czasu. 
UWAGA - pulapka. Nalezy zablokowac mozliwosc modyfikacji tablicy w trakcie
dzialania funkcji odczytujacej czujniki. Poniewaz odczyt zajmuje dosc dluga 
chwilke, to zmiany tablicy moga blokowac dzialanie programu.


W I Z U A L I Z A C J A


Wizualizacja danych z czujnika:
- powinna byc wykonana natychmiast po przeprowadzeniu pomiaru
- moze wskazywac stan magistrali
- kazdy element moze w rozny sposob prezentowac dane (wykres z pewnego
													  przedzialu czasowego badz chwilowa wartosc)
- kazdy element moze ukazywac dane z wielu czujnikow badz tylko z jednego
- kazdy element powinien miec mozliwosc niezaleznego umieszczenia na 
przestrzeni roboczej, najlepiej podczas wlaczania programu na podstawie
pliku konfiguracyjnego
- zawartosc elementu powinna byc uktualniana natychmiastowo po otrzymaniu 
sygnalu o zmianie zawartosci okna (maksymalizacja, zmiana rozmiaru itp.)
- wizualizacja moze byc przeprowadzana dla danych z przeszlosci


Pierwszy pomysl:

Wszystkie elementy powinny byc umieszczone w globalnej tablicy. Z kazdym elementem
powinna byc zwiazana lista identyfikatorow wyswietlanych czujnikow. Po wykonaniu
odczytu nalezy przejrzec wszystkie elementy i uaktualnic wlasciwe. Funkcja
przegladajaca moze byc jedna dla tablicy, a jej argumentami moga byc pomiar
(czas + wartosc) oraz identyfikator czujnika. Kazdy element musi wiec udostepniac
jednakowy interfejs pozwalajacy stwierdzic, czy jest zainteresowany dana 
wartoscia i na wprowadzenie jej. Jesli jakis element wymaga danych z pewnego 
przedzialu czasowego, powinien sam zadbac o ich uzyskanie i przechowywanie.





*/


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <config.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glade/glade.h>
#include <glib.h>
#include <pthread.h>

#include "callbacks.h"

#include "t_display.h"
#include "popup_menu.h"
#include "t_panel.h"

#include "configuration.h"
#include "sensors.h"
#include "sensor_read_thread.h"
#include "sample_load_save.h"
#include "general_settings.h"

/*
* Standard gettext macros.
*/
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif




/* For testing propose use the local (not installed) glade file */
/* #define GLADE_FILE PACKAGE_DATA_DIR"/linlamp/glade/linlamp.glade" */
#define GLADE_FILE "linlamp.glade"

GladeXML *gxml;

GtkWidget*
create_window (void)
{
	GtkWidget *window;
	//GladeXML *gxml;
	
	gxml = glade_xml_new (GLADE_FILE, NULL, NULL);
	
	/* This is important */
		glade_xml_signal_autoconnect (gxml);
	window = glade_xml_get_widget (gxml, "window");
	
	return window;
}




int
main (int argc, char *argv[])
{
	//make_config_file_backup("aaa")  ;
	//destroy_system_sensor_list( get_system_sensors() ) ;
	
	GtkWidget *window;
	
	
#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif
	
	
	g_thread_init(NULL) ;
	gdk_threads_init() ;
	
	if (g_thread_supported()) {
		//printf ("Thread subsytem ON.\n") ;
	} else {
		printf ("Thread subsytem FAILED.\n") ;
		return -1 ;
	}
	
	gdk_threads_enter ();
	
	
	gtk_set_locale ();
	gtk_init (&argc, &argv);
	
	window = create_window ();
	
	register_popup_menu(gxml) ;
	register_panel(gxml) ;
	register_sensors_dialog(gxml) ;
	register_general_settings_dialog(gxml) ;
	
	gtk_widget_show (window);
	
	load_configuration_from_file("config.txt") ;
	
	//GThread *t_read = 
		g_thread_create ((GThreadFunc)read_thread_func, NULL, FALSE, NULL ) ;
	
	
	g_timeout_add_seconds (1, (GSourceFunc)check_and_store_samples, NULL) ;
	
	
	gtk_main ();
	
	save_all_sensors_data() ;
	
	gdk_threads_leave ();	
	
	return 0;
}
