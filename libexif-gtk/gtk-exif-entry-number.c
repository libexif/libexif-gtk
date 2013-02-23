/* gtk-exif-entry-number.c
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
#include "gtk-exif-entry-number.h"
#include "gtk-exif-util.h"

#include <string.h>
#include <gtk/gtk.h>
#include <libexif/exif-utils.h>

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

struct _GtkExifEntryNumberPrivate {
	ExifEntry *entry;

	GPtrArray *a;
};

#define PARENT_TYPE GTK_EXIF_TYPE_ENTRY
static GtkExifEntryClass *parent_class;

static void
#if GTK_CHECK_VERSION(3,0,0)
gtk_exif_entry_number_destroy (GtkWidget *widget)
#else
gtk_exif_entry_number_destroy (GtkObject *object)
#endif
{
#if GTK_CHECK_VERSION(3,0,0)
	GtkExifEntryNumber *entry = GTK_EXIF_ENTRY_NUMBER (widget);
#else
	GtkExifEntryNumber *entry = GTK_EXIF_ENTRY_NUMBER (object);
#endif

	if (entry->priv->entry) {
		exif_entry_unref (entry->priv->entry);
		entry->priv->entry = NULL;
	}

	if (entry->priv->a) {
		g_ptr_array_free (entry->priv->a, TRUE);
		entry->priv->a = NULL;
	}

#if GTK_CHECK_VERSION(3,0,0)
	GTK_WIDGET_CLASS (parent_class)->destroy (widget);
#else
	GTK_OBJECT_CLASS (parent_class)->destroy (object);
#endif
}

GTK_EXIF_FINALIZE (entry_number, EntryNumber)

static void
gtk_exif_entry_number_class_init (gpointer g_class, gpointer class_data)
{
#if GTK_CHECK_VERSION(3,0,0)
	GtkWidgetClass *widget_class;
	GObjectClass *gobject_class;

	widget_class = GTK_WIDGET_CLASS (g_class);
	widget_class->destroy = gtk_exif_entry_number_destroy;
#else
	GtkObjectClass *object_class;
	GObjectClass *gobject_class;

	object_class = GTK_OBJECT_CLASS (g_class);
	object_class->destroy  = gtk_exif_entry_number_destroy;
#endif

	gobject_class = G_OBJECT_CLASS (g_class);
	gobject_class->finalize = gtk_exif_entry_number_finalize;

	parent_class = g_type_class_peek_parent (g_class);
}

static void
gtk_exif_entry_number_init (GTypeInstance *instance, gpointer g_class)
{
	GtkExifEntryNumber *entry = GTK_EXIF_ENTRY_NUMBER (instance);

	entry->priv = g_new0 (GtkExifEntryNumberPrivate, 1);
	entry->priv->a = g_ptr_array_new ();
}

GTK_EXIF_CLASS (entry_number, EntryNumber, "EntryNumber")

static void
gtk_exif_entry_number_load (GtkExifEntryNumber *entry)
{
	ExifByte v_byte;
	ExifShort v_short;
	ExifLong v_long;
	ExifSLong v_slong;
	GtkAdjustment *a;
	ExifEntry *e;
	guint i;
	ExifByteOrder o;

	g_return_if_fail (GTK_EXIF_IS_ENTRY_NUMBER (entry));

	o = exif_data_get_byte_order (entry->priv->entry->parent->parent);
	e = entry->priv->entry;
	for (i = 0; i < e->components; i++) {
		a = entry->priv->a->pdata[i];
		g_signal_handlers_block_matched (G_OBJECT (a),
				G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, entry);
		switch (e->format) {
		case EXIF_FORMAT_BYTE:
			v_byte = e->data[i];
			gtk_adjustment_set_value (a, v_byte);
			break;
		case EXIF_FORMAT_SHORT:
			v_short = exif_get_short (e->data + 2 * i, o);
			gtk_adjustment_set_value (a, v_short);
			break;
		case EXIF_FORMAT_LONG:
			v_long = exif_get_long (e->data + 4 * i, o);
			gtk_adjustment_set_value (a, v_long);
			break;
		case EXIF_FORMAT_SLONG:
			v_slong = exif_get_slong (e->data + 4 * i, o);
			gtk_adjustment_set_value (a, v_slong);
			break;
		default:
			g_warning ("Invalid format!");
			break;
		}
		g_signal_handlers_unblock_matched (G_OBJECT (a),
				G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, entry);
	}
}

static void
gtk_exif_entry_number_save (GtkExifEntryNumber *entry)
{
	ExifEntry *e;
	GtkAdjustment *a;
	guint i;
	ExifByteOrder o;

	g_return_if_fail (GTK_EXIF_IS_ENTRY_NUMBER (entry));

	o = exif_data_get_byte_order (entry->priv->entry->parent->parent);
	e = entry->priv->entry;
	for (i = 0; i < e->components; i++) {
		a = entry->priv->a->pdata[i];
		switch (e->format) {
		case EXIF_FORMAT_BYTE:
			e->data[i] = gtk_adjustment_get_value (a);
			break;
		case EXIF_FORMAT_SHORT:
			exif_set_short (e->data + 2 * i, o, gtk_adjustment_get_value (a));
			break;
		case EXIF_FORMAT_LONG:
			exif_set_long (e->data + 4 * i, o, gtk_adjustment_get_value (a));
			break;
		case EXIF_FORMAT_SLONG:
			exif_set_slong (e->data + 4 * i, o, gtk_adjustment_get_value (a));
			break;
		default:
			g_warning ("Invalid format!");
			return;
		}
	}
	g_signal_emit_by_name (G_OBJECT (entry), "entry_changed", e);
}

static void
on_adjustment_value_changed (GtkAdjustment *adj, GtkExifEntryNumber *entry)
{
	gtk_exif_entry_number_save (entry);
}

GtkWidget *
gtk_exif_entry_number_new (ExifEntry *e)
{
	GtkExifEntryNumber *entry;
	GtkWidget *table, *label, *spin;
#if GTK_CHECK_VERSION(3,0,0)
	GtkAdjustment *a;
#else
	GtkObject *a;
#endif
	gchar *txt;
	guint i;

	g_return_val_if_fail (e != NULL, NULL);
	g_return_val_if_fail ((e->format == EXIF_FORMAT_BYTE) ||
			      (e->format == EXIF_FORMAT_SHORT) ||
			      (e->format == EXIF_FORMAT_LONG) ||
			      (e->format == EXIF_FORMAT_SLONG), NULL);

	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");

	entry = g_object_new (GTK_EXIF_TYPE_ENTRY_NUMBER, NULL);
	entry->priv->entry = e;
	exif_entry_ref (e);
	gtk_exif_entry_construct (GTK_EXIF_ENTRY (entry),
		exif_tag_get_title_in_ifd (e->tag, exif_content_get_ifd(e->parent)),
		exif_tag_get_description_in_ifd (e->tag, exif_content_get_ifd(e->parent)));

	table = gtk_table_new (2, 1, FALSE);
	gtk_widget_show (table);
	gtk_box_pack_start (GTK_BOX (entry), table, TRUE, TRUE, 0);
	gtk_table_set_col_spacings (GTK_TABLE (table), 5);
	gtk_table_set_row_spacings (GTK_TABLE (table), 5);

	g_ptr_array_set_size (entry->priv->a, e->components);
	for (i = 0; i < e->components; i++) {
		if (e->components > 1)
			txt = g_strdup_printf (_("Value %i:"), i + 1);
		else
			txt = g_strdup (_("Value:"));
		label = gtk_label_new (txt);
		g_free (txt);
		gtk_widget_show (label);
		gtk_table_attach (GTK_TABLE (table), label, 0, 1, i, i + 1,
				  GTK_FILL, 0, 0, 0);
		gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
		gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);

		a = gtk_adjustment_new (0, 0, 0xffff, 1, 0xff, 0);
		spin = gtk_spin_button_new (GTK_ADJUSTMENT (a), 0, 0);
		gtk_widget_show (spin);
		gtk_table_attach (GTK_TABLE (table), spin, 1, 2, i, i + 1,
				  GTK_FILL | GTK_EXPAND, 0, 0, 0);
		entry->priv->a->pdata[i] = a;
		g_signal_connect (a, "value_changed",
			G_CALLBACK (on_adjustment_value_changed), entry);
	}

	gtk_exif_entry_number_load (entry);

	return (GTK_WIDGET (entry));
}
