/* test-libexif-gtk.c
 *
 * Copyright © 2002 Lutz Müller <lutz@users.sourceforge.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "config.h"

#include <gtk/gtk.h>

#include <libexif-gtk/gtk-exif-browser.h>

int
main (int argc, char **argv)
{
	GtkWidget *w, *b;
	ExifData *ed;

	if (argc != 2)
		g_error ("You need to supply exactly one file to load.");

	g_log_set_always_fatal (G_LOG_LEVEL_CRITICAL);
	gtk_init (&argc, &argv);

	w = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_show (w);
	b = gtk_exif_browser_new ();
	ed = exif_data_new_from_file (argv[1]);
	if (!ed)
		g_error ("Could not load EXIF data from '%s'.", argv[1]);
	gtk_exif_browser_set_data (GTK_EXIF_BROWSER (b), ed);
	gtk_widget_show (b);
	gtk_container_add (GTK_CONTAINER (w), b);
	g_signal_connect (G_OBJECT (w), "delete_event",
			  G_CALLBACK (gtk_main_quit), NULL);

	gtk_main ();

	return (0);
}
