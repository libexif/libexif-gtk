/* gtk-exif-entry-ascii.c
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

#include <config.h>
#include "gtk-exif-entry-ascii.h"

#include <gtk/gtkcheckbutton.h>
#include <gtk/gtkradiobutton.h>
#include <gtk/gtkvbox.h>
#include <gtk/gtksignal.h>
#include <gtk/gtkframe.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkentry.h>

#include <string.h>

struct _GtkExifEntryAsciiPrivate {
	ExifEntry *entry;

	GtkEntry *gtk_entry;
};

#define PARENT_TYPE GTK_EXIF_TYPE_ENTRY
static GtkExifEntryClass *parent_class;

static void
gtk_exif_entry_ascii_destroy (GtkObject *object)
{
	GtkExifEntryAscii *entry = GTK_EXIF_ENTRY_ASCII (object);

	if (entry->priv->entry) {
		exif_entry_unref (entry->priv->entry);
		entry->priv->entry = NULL;
	}

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gtk_exif_entry_ascii_finalize (GObject *object)
{
	GtkExifEntryAscii *entry = GTK_EXIF_ENTRY_ASCII (object);

	g_free (entry->priv);

	G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_exif_entry_ascii_class_init (gpointer g_class, gpointer class_data)
{
	GtkObjectClass *object_class;
	GObjectClass *gobject_class;

	object_class = GTK_OBJECT_CLASS (g_class);
	object_class->destroy  = gtk_exif_entry_ascii_destroy;

	gobject_class = G_OBJECT_CLASS (g_class);
	gobject_class->finalize = gtk_exif_entry_ascii_finalize;

	parent_class = g_type_class_peek_parent (g_class);
}

static void
gtk_exif_entry_ascii_init (GTypeInstance *instance, gpointer g_class)
{
	GtkExifEntryAscii *entry = GTK_EXIF_ENTRY_ASCII (instance);

	entry->priv = g_new0 (GtkExifEntryAsciiPrivate, 1);
}

GType
gtk_exif_entry_ascii_get_type (void)
{
        static GType t = 0;

        if (!t) {
                GTypeInfo ti;

                memset (&ti, 0, sizeof (GTypeInfo));
                ti.class_size    = sizeof (GtkExifEntryAsciiClass);
                ti.class_init    = gtk_exif_entry_ascii_class_init;
                ti.instance_size = sizeof (GtkExifEntryAscii);
                ti.instance_init = gtk_exif_entry_ascii_init;

                t = g_type_register_static (PARENT_TYPE, "GtkExifEntryAscii",
					    &ti, 0);
        }

        return (t);
}

static void
on_text_changed (GtkEditable *editable, GtkExifEntryAscii *entry)
{
	gchar *txt;

	txt = gtk_editable_get_chars (editable, 0, -1);
	g_free (entry->priv->entry->data);
	entry->priv->entry->data = txt;
	entry->priv->entry->size = strlen (txt) + 1;
	entry->priv->entry->components = entry->priv->entry->size;
	g_signal_emit_by_name (GTK_OBJECT (entry), "entry_changed",
				 entry->priv->entry);
}

GtkWidget *
gtk_exif_entry_ascii_new (ExifEntry *e)
{
	GtkExifEntryAscii *entry;
	GtkWidget *widget;

	g_return_val_if_fail (e != NULL, NULL);
	g_return_val_if_fail (e->format == EXIF_FORMAT_ASCII, NULL);

	entry = g_object_new (GTK_EXIF_TYPE_ENTRY_ASCII, NULL);
	entry->priv->entry = e;
	exif_entry_ref (e);
	gtk_exif_entry_construct (GTK_EXIF_ENTRY (entry),
		exif_tag_get_title (e->tag),
		exif_tag_get_description (e->tag));

	widget = gtk_entry_new ();
	gtk_widget_show (widget);
	gtk_box_pack_start (GTK_BOX (entry), widget, TRUE, FALSE, 0);
	gtk_entry_set_text (GTK_ENTRY (widget), e->data);
	g_signal_connect (G_OBJECT (widget), "changed",
			  G_CALLBACK (on_text_changed), entry);

	return (GTK_WIDGET (entry));
}
