/* gtk-exif-entry-copyright.c
 *
 * Copyright © 2001 Lutz Müller <lutz@users.sourceforge.net>
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
#include "gtk-exif-entry-copyright.h"
#include "gtk-exif-util.h"

#include <string.h>
#include <gtk/gtk.h>

#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (GETTEXT_PACKAGE, String)
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

struct _GtkExifEntryCopyrightPrivate {
	ExifEntry *entry;

	GtkEntry *photographer, *editor;
};

#define PARENT_TYPE GTK_EXIF_TYPE_ENTRY
static GtkExifEntryClass *parent_class;

static void
#if GTK_CHECK_VERSION(3,0,0)
gtk_exif_entry_copyright_destroy (GtkWidget *widget)
#else
gtk_exif_entry_copyright_destroy (GtkObject *object)
#endif
{
#if GTK_CHECK_VERSION(3,0,0)
	GtkExifEntryCopyright *entry = GTK_EXIF_ENTRY_COPYRIGHT (widget);
#else
	GtkExifEntryCopyright *entry = GTK_EXIF_ENTRY_COPYRIGHT (object);
#endif

	if (entry->priv->entry) {
		exif_entry_unref (entry->priv->entry);
		entry->priv->entry = NULL;
	}

#if GTK_CHECK_VERSION(3,0,0)
	GTK_WIDGET_CLASS (parent_class)->destroy (widget);
#else
	GTK_OBJECT_CLASS (parent_class)->destroy (object);
#endif
}

GTK_EXIF_FINALIZE (entry_copyright, EntryCopyright)

static void
gtk_exif_entry_copyright_class_init (gpointer g_class, gpointer class_data)
{
#if GTK_CHECK_VERSION(3,0,0)
	GtkWidgetClass *widget_class;
	GObjectClass *gobject_class;

	widget_class = GTK_WIDGET_CLASS (g_class);
	widget_class->destroy = gtk_exif_entry_copyright_destroy;
#else
	GtkObjectClass *object_class;
	GObjectClass *gobject_class;

	object_class = GTK_OBJECT_CLASS (g_class);
	object_class->destroy  = gtk_exif_entry_copyright_destroy;
#endif

	gobject_class = G_OBJECT_CLASS (g_class);
	gobject_class->finalize = gtk_exif_entry_copyright_finalize;

	parent_class = g_type_class_peek_parent (g_class);
}

static void
gtk_exif_entry_copyright_init (GTypeInstance *instance, gpointer g_class)
{
	GtkExifEntryCopyright *entry = GTK_EXIF_ENTRY_COPYRIGHT (instance);

	entry->priv = g_new0 (GtkExifEntryCopyrightPrivate, 1);
}

GTK_EXIF_CLASS (entry_copyright, EntryCopyright, "EntryCopyright")

static void
on_text_changed (GtkEditable *editable, GtkExifEntryCopyright *entry)
{
	gchar *photographer, *editor;

	if (entry->priv->entry->data) {
		g_free (entry->priv->entry->data);
		entry->priv->entry->data = NULL;
		entry->priv->entry->size = 0;
	}

	photographer = gtk_editable_get_chars (
			GTK_EDITABLE (entry->priv->photographer), 0, -1);
	editor = gtk_editable_get_chars (
			GTK_EDITABLE (entry->priv->editor), 0, -1);
	entry->priv->entry->data = g_strdup_printf ("%s %s", photographer,
						    editor);
	entry->priv->entry->data[strlen (photographer)] = '\0';
	entry->priv->entry->size = strlen (photographer) + 1 +
				   strlen (editor) + 1;
	entry->priv->entry->components = entry->priv->entry->size;
	g_free (photographer);
	g_free (editor);
	gtk_exif_entry_changed (GTK_EXIF_ENTRY (entry), entry->priv->entry);
}

GtkWidget *
gtk_exif_entry_copyright_new (ExifEntry *e)
{
	GtkExifEntryCopyright *entry;
	GtkWidget *widget, *label, *table;

	g_return_val_if_fail (e != NULL, NULL);
	g_return_val_if_fail (e->format == EXIF_FORMAT_ASCII, NULL);
	g_return_val_if_fail (e->tag == EXIF_TAG_COPYRIGHT, NULL);

	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");

	entry = g_object_new (GTK_EXIF_TYPE_ENTRY_COPYRIGHT, NULL);
	entry->priv->entry = e;
	exif_entry_ref (e);
	gtk_exif_entry_construct (GTK_EXIF_ENTRY (entry),
		exif_tag_get_title_in_ifd (e->tag, exif_content_get_ifd(e->parent)),
		exif_tag_get_description_in_ifd (e->tag, exif_content_get_ifd(e->parent)));

	table = gtk_table_new (2, 2, FALSE);
	gtk_widget_show (table);
	gtk_table_set_col_spacings (GTK_TABLE (table), 5);
	gtk_table_set_row_spacings (GTK_TABLE (table), 5);
	gtk_box_pack_start (GTK_BOX (entry), table, TRUE, FALSE, 0);
	label = gtk_label_new (_("Photographer:"));
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1,
			  GTK_FILL, 0, 0, 0);
	gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	label = gtk_label_new (_("Editor:"));
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2,
			  GTK_FILL, 0, 0, 0);
	gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	widget = gtk_entry_new ();
	gtk_widget_show (widget);
	gtk_table_attach (GTK_TABLE (table), widget, 1, 2, 0, 1,
			  GTK_FILL | GTK_EXPAND, 0, 0, 0);
	gtk_entry_set_text (GTK_ENTRY (widget), e->data);
	g_signal_connect (G_OBJECT (widget), "changed",
			    G_CALLBACK (on_text_changed), entry);
	entry->priv->photographer = GTK_ENTRY (widget);
	widget = gtk_entry_new ();
	gtk_widget_show (widget);
	gtk_table_attach (GTK_TABLE (table), widget, 1, 2, 1, 2,
			  GTK_FILL | GTK_EXPAND, 0, 0, 0);
	gtk_entry_set_text (GTK_ENTRY (widget), e->data + strlen (e->data) + 1);
	g_signal_connect (G_OBJECT (widget), "changed",
			    G_CALLBACK (on_text_changed), entry);
	entry->priv->editor = GTK_ENTRY (widget);

	return (GTK_WIDGET (entry));
}
