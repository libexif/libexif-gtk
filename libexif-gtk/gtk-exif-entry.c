/* gtk-exif-entry.c
 *
 * Copyright (C) 2001 Lutz Müller <lutz@users.sourceforge.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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
#include "gtk-exif-entry.h"

#include <string.h>

#include <gtk/gtksignal.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkscrolledwindow.h>
#include <gtk/gtktable.h>
#include <gtk/gtkhseparator.h>

#include "gtk-exif-util.h"

struct _GtkExifEntryPrivate {
};

#define PARENT_TYPE GTK_TYPE_VBOX
static GtkVBoxClass *parent_class;

enum {
	ENTRY_ADDED,
	ENTRY_REMOVED,
	ENTRY_CHANGED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};

static void
gtk_exif_entry_destroy (GtkObject *object)
{
	GtkExifEntry *entry = GTK_EXIF_ENTRY (object);

	entry = NULL;

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

GTK_EXIF_FINALIZE (entry, Entry)

static void
gtk_exif_entry_class_init (gpointer g_class, gpointer class_data)
{
	GtkObjectClass *object_class;
	GObjectClass *gobject_class;

	object_class = GTK_OBJECT_CLASS (g_class);
	object_class->destroy  = gtk_exif_entry_destroy;

	gobject_class = G_OBJECT_CLASS (g_class);
	gobject_class->finalize = gtk_exif_entry_finalize;

	signals[ENTRY_ADDED] = g_signal_new ("entry_added",
		G_TYPE_FROM_CLASS (g_class), G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (GtkExifEntryClass, entry_added), NULL, NULL,
		g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1,
		G_TYPE_POINTER);
	signals[ENTRY_REMOVED] = g_signal_new ("entry_removed",
		G_TYPE_FROM_CLASS (g_class), G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (GtkExifEntryClass, entry_removed), NULL, NULL,
		g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1,
		G_TYPE_POINTER);
	signals[ENTRY_CHANGED] = g_signal_new ("entry_changed",
		G_TYPE_FROM_CLASS (g_class), G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (GtkExifEntryClass, entry_changed), NULL, NULL,
		g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1,
		G_TYPE_POINTER);

	parent_class = g_type_class_peek_parent (g_class);
}

static void
gtk_exif_entry_init (GTypeInstance *instance, gpointer g_class)
{
	GtkExifEntry *entry = GTK_EXIF_ENTRY (instance);

	entry->priv = g_new0 (GtkExifEntryPrivate, 1);
}

GTK_EXIF_CLASS (entry, Entry, "Entry")

void
gtk_exif_entry_construct (GtkExifEntry *entry,
			  const gchar *name, const gchar *description)
{
	GtkWidget *label, *separator;

	g_return_if_fail (GTK_EXIF_IS_ENTRY (entry));
	g_return_if_fail (name != NULL);
	g_return_if_fail (description != NULL);

	gtk_box_set_spacing (GTK_BOX (entry), 5);
	gtk_box_set_homogeneous (GTK_BOX (entry), FALSE);
	gtk_container_set_border_width (GTK_CONTAINER (entry), 5);

	label = gtk_label_new (name);
	gtk_widget_show (label);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_label_set_line_wrap (GTK_LABEL (label), FALSE);
	gtk_box_pack_start (GTK_BOX (entry), label, TRUE, FALSE, 0);

	separator = gtk_hseparator_new ();
	gtk_widget_show (separator);
	gtk_box_pack_start (GTK_BOX (entry), separator, TRUE, FALSE, 0);

#if 0
	swin = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (swin);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (swin),
				GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start (GTK_BOX (entry), swin, TRUE, TRUE, 0);
#endif
	label = gtk_label_new (description);
	gtk_widget_show (label);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
	gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
#if 0
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (swin),
					       label);
	gtk_viewport_set_shadow_type (
		GTK_VIEWPORT (GTK_BIN (swin)->child), GTK_SHADOW_NONE);
#endif
	gtk_box_pack_start (GTK_BOX (entry), label, TRUE, FALSE, 0);
}

void
gtk_exif_entry_changed (GtkExifEntry *entry, ExifEntry *e)
{
	g_return_if_fail (GTK_EXIF_IS_ENTRY (entry));

	g_signal_emit (G_OBJECT (entry), signals[ENTRY_CHANGED], 0, e);
}

void
gtk_exif_entry_added (GtkExifEntry *entry, ExifEntry *e)
{
	g_return_if_fail (GTK_EXIF_IS_ENTRY (entry));

	g_signal_emit (G_OBJECT (entry), signals[ENTRY_ADDED], 0, e);
}

void
gtk_exif_entry_removed (GtkExifEntry *entry, ExifEntry *e)
{
	g_return_if_fail (GTK_EXIF_IS_ENTRY (entry));
	
	g_signal_emit (G_OBJECT (entry), signals[ENTRY_REMOVED], 0, e);
}
