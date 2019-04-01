/* gtk-exif-entry-generic.c
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
#include "gtk-exif-entry-generic.h"
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

struct _GtkExifEntryGenericPrivate {
	ExifEntry *entry;

	GtkToggleButton *c;
	GtkToggleButton *r1, *r2, *r3;
};

#define PARENT_TYPE GTK_EXIF_TYPE_ENTRY
static GtkExifEntryClass *parent_class;

static void
#if GTK_CHECK_VERSION(3,0,0)
gtk_exif_entry_generic_destroy (GtkWidget *widget)
#else
gtk_exif_entry_generic_destroy (GtkObject *object)
#endif
{
#if GTK_CHECK_VERSION(3,0,0)
	GtkExifEntryGeneric *entry = GTK_EXIF_ENTRY_GENERIC (widget);
#else
	GtkExifEntryGeneric *entry = GTK_EXIF_ENTRY_GENERIC (object);
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

GTK_EXIF_FINALIZE (entry_generic, EntryGeneric)

static void
gtk_exif_entry_generic_class_init (gpointer g_class, gpointer class_data G_GNUC_UNUSED)
{
#if GTK_CHECK_VERSION(3,0,0)
	GtkWidgetClass *widget_class;
	GObjectClass *gobject_class;

	widget_class = GTK_WIDGET_CLASS (g_class);
	widget_class->destroy = gtk_exif_entry_generic_destroy;
#else
	GtkObjectClass *object_class;
	GObjectClass *gobject_class;

	object_class = GTK_OBJECT_CLASS (g_class);
	object_class->destroy  = gtk_exif_entry_generic_destroy;
#endif

	gobject_class = G_OBJECT_CLASS (g_class);
	gobject_class->finalize = gtk_exif_entry_generic_finalize;

	parent_class = g_type_class_peek_parent (g_class);
}

static void
gtk_exif_entry_generic_init (GTypeInstance *instance, gpointer g_class G_GNUC_UNUSED)
{
	GtkExifEntryGeneric *entry = GTK_EXIF_ENTRY_GENERIC (instance);

	entry->priv = g_new0 (GtkExifEntryGenericPrivate, 1);
}

GTK_EXIF_CLASS (entry_generic, EntryGeneric, "EntryGeneric")

GtkWidget *
gtk_exif_entry_generic_new (ExifEntry *e)
{
	GtkExifEntryGeneric *entry;
	GtkWidget *table, *label;
	gchar *txt, s[1024];

	g_return_val_if_fail (e != NULL, NULL);

	entry = g_object_new (GTK_EXIF_TYPE_ENTRY_GENERIC, NULL);
	entry->priv->entry = e;
	exif_entry_ref (e);
	gtk_exif_entry_construct (GTK_EXIF_ENTRY (entry),
		exif_tag_get_title_in_ifd (e->tag, exif_content_get_ifd(e->parent)),
		exif_tag_get_description_in_ifd (e->tag, exif_content_get_ifd(e->parent)));

	table = gtk_table_new (2, 4, FALSE);
	gtk_widget_show (table);
	gtk_box_pack_start (GTK_BOX (entry), table, TRUE, TRUE, 0);
	gtk_table_set_col_spacings (GTK_TABLE (table), 5);
	gtk_table_set_row_spacings (GTK_TABLE (table), 5);

	label = gtk_label_new (_("Format:"));
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, 0, 0, 0, 0);
	txt = g_strdup_printf ("%i ('%s')", e->format,
			       exif_format_get_name (e->format));
	label = gtk_label_new (txt);
	g_free (txt);
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, 0, 0, 0, 0);

	label = gtk_label_new (_("Components:"));
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, 0, 0, 0, 0);
	txt = g_strdup_printf ("%i", (int) e->components);
	label = gtk_label_new (txt);
	g_free (txt);
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table), label, 1, 2, 1, 2, 0, 0, 0, 0);

	label = gtk_label_new (_("Size:"));
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3, 0, 0, 0, 0);
	txt = g_strdup_printf ("%i", e->size);
	label = gtk_label_new (txt);
	g_free (txt);
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table), label, 1, 2, 2, 3, 0, 0, 0, 0);

	label = gtk_label_new (_("Value:"));
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 3, 4, 0, 0, 0, 0);
	label = gtk_label_new (exif_entry_get_value (e, s, sizeof (s)));
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table), label, 1, 2, 3, 4, 0, 0, 0, 0);

	return (GTK_WIDGET (entry));
}
