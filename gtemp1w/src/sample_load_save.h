#ifndef _SAMPLE_LOAD_SAVE_H
#define _SAMPLE_LOAD_SAVE_H

/*
Na razie operacja zapisywania danych z czujnikow na dysk przeprowadzana jest 
w petli glownej programu. Pozniej moze wrzuce to do osobnego watku, ale na 
razie zostawie, jak jest. Nawet przy probkowaniu co 1 sek (niewykonalne dla 
wiekszej liczby czujnikow) powstaje teretycznie 3600 probek na godzine na
czujnik. Jedna probka to 4+4 bajty, a wiec maksymalnie 28800 bajtow na godzine
na czujnik, co daje niecale 700000 bajtow na dobe. Zapis nawet kilku MB nie 
powinien trwac jakos szczegolnie dlugo, a nawet jesli, to moge zgubić kilka 
sekund - na razie przezyje :)
*/

#include <glib.h>

G_BEGIN_DECLS

/*
Funkcja powinna byc wywolywana co okreslony czas.
Sprawdza, czy uplynelo wystarczajaco duzo czasu od ostatniego zapisu i jesli
tak, to zrzuca bufory wszystkich sensorow na dysk.
Blokuje watki za pomoca gdk_threads_enter() 
*/
gboolean check_and_store_samples (gpointer data);

/*
Funkcja zapisuje dane czujnika o przekazanej nazwie. 
Wewnatrz dane sa dzielone na jednodniowe fragmenty i zapisywane
do odpowiednich katalogow.
Funkcja usuwa zapisane fragmenty tablicy.
*/
gboolean store_samples(GArray *samples, const char *sensor_name) ;

GArray *load_samples (const char *sensor_name, time_t t_begin, time_t t_end) ;

G_END_DECLS

#endif /* _SAMPLE_LOAD_SAVE_H */

