/* gtk-exif-entry-rational.c
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
#include "gtk-exif-entry-rational.h"
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

struct _GtkExifEntryRationalPrivate {
	ExifEntry *entry;

	GPtrArray *ap, *aq;
};

#define PARENT_TYPE GTK_EXIF_TYPE_ENTRY
static GtkExifEntryClass *parent_class;

static void
#if GTK_CHECK_VERSION(3,0,0)
gtk_exif_entry_rational_destroy (GtkWidget *widget)
#else
gtk_exif_entry_rational_destroy (GtkObject *object)
#endif
{
#if GTK_CHECK_VERSION(3,0,0)
	GtkExifEntryRational *entry = GTK_EXIF_ENTRY_RATIONAL (widget);
#else
	GtkExifEntryRational *entry = GTK_EXIF_ENTRY_RATIONAL (object);
#endif

	if (entry->priv->entry) {
		exif_entry_unref (entry->priv->entry);
		entry->priv->entry = NULL;
	}

	if (entry->priv->ap) {
		g_ptr_array_free (entry->priv->ap, TRUE);
		entry->priv->ap = NULL;
	}

	if (entry->priv->aq) {
		g_ptr_array_free (entry->priv->aq, TRUE);
		entry->priv->aq = NULL;
	}

#if GTK_CHECK_VERSION(3,0,0)
	GTK_WIDGET_CLASS (parent_class)->destroy (widget);
#else
	GTK_OBJECT_CLASS (parent_class)->destroy (object);
#endif
}

GTK_EXIF_FINALIZE (entry_rational, EntryRational)

static void
gtk_exif_entry_rational_class_init (gpointer g_class, gpointer class_data)
{
#if GTK_CHECK_VERSION(3,0,0)
	GtkWidgetClass *widget_class;
	GObjectClass *gobject_class;

	widget_class = GTK_WIDGET_CLASS (g_class);
	widget_class->destroy  = gtk_exif_entry_rational_destroy;
#else
	GtkObjectClass *object_class;
	GObjectClass *gobject_class;

	object_class = GTK_OBJECT_CLASS (g_class);
	object_class->destroy  = gtk_exif_entry_rational_destroy;
#endif

	gobject_class = G_OBJECT_CLASS (g_class);
	gobject_class->finalize = gtk_exif_entry_rational_finalize;

	parent_class = g_type_class_peek_parent (g_class);
}

static void
gtk_exif_entry_rational_init (GTypeInstance *instance, gpointer g_class)
{
	GtkExifEntryRational *entry = GTK_EXIF_ENTRY_RATIONAL (instance);

	entry->priv = g_new0 (GtkExifEntryRationalPrivate, 1);
	entry->priv->ap = g_ptr_array_new ();
	entry->priv->aq = g_ptr_array_new ();
}

GTK_EXIF_CLASS (entry_rational, EntryRational, "EntryRational")

static void
gtk_exif_entry_rational_load (GtkExifEntryRational *entry)
{
	ExifRational r;
	ExifSRational sr;
	GtkAdjustment *ap, *aq;
	ExifEntry *e;
	guint i;
	ExifByteOrder o;

	g_return_if_fail (GTK_EXIF_IS_ENTRY_RATIONAL (entry));

	o = exif_data_get_byte_order (entry->priv->entry->parent->parent);
	e = entry->priv->entry;
	for (i = 0; i < e->components; i++) {
		ap = entry->priv->ap->pdata[i];
		aq = entry->priv->aq->pdata[i];
		g_signal_handlers_block_matched (G_OBJECT (ap),
				G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, entry);
		g_signal_handlers_block_matched (G_OBJECT (aq),
				G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, entry);
		switch (e->format) {
		case EXIF_FORMAT_RATIONAL:
			r = exif_get_rational (e->data + 8 * i, o);
			gtk_adjustment_set_value (ap, r.numerator);
			gtk_adjustment_set_value (aq, r.denominator);
			break;
		case EXIF_FORMAT_SRATIONAL:
			sr = exif_get_srational (e->data + 8 * i, o);
			gtk_adjustment_set_value (ap, sr.numerator);
			gtk_adjustment_set_value (aq, sr.denominator);
			break;
		default:
			g_warning ("Invalid format!");
			break;
		}
		g_signal_handlers_unblock_matched (G_OBJECT (ap),
				G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, entry);
		g_signal_handlers_unblock_matched (G_OBJECT (aq),
				G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, entry);
	}
}

static void
gtk_exif_entry_rational_save (GtkExifEntryRational *entry)
{
	ExifRational r;
	ExifSRational sr;
	ExifEntry *e;
	GtkAdjustment *ap, *aq;
	guint i;
	ExifByteOrder o;

	g_return_if_fail (GTK_EXIF_IS_ENTRY_RATIONAL (entry));

	o = exif_data_get_byte_order (entry->priv->entry->parent->parent);
	e = entry->priv->entry;
	for (i = 0; i < e->components; i++) {
		ap = entry->priv->ap->pdata[i];
		aq = entry->priv->aq->pdata[i];
		switch (e->format) {
		case EXIF_FORMAT_RATIONAL:
			r.numerator = gtk_adjustment_get_value (ap);
			r.denominator = gtk_adjustment_get_value (aq);
			exif_set_rational (e->data + 8 * i, o, r);
			break;
		case EXIF_FORMAT_SRATIONAL:
			sr.numerator = gtk_adjustment_get_value (ap);
			sr.denominator = gtk_adjustment_get_value (aq);
			exif_set_srational (e->data + 8 * i, o, sr);
			break;
		default:
			g_warning ("Invalid format!");
			return;
		}
	}
	g_signal_emit_by_name (G_OBJECT (entry), "entry_changed", e);
}

static void
on_adjustment_value_changed (GtkAdjustment *adj, GtkExifEntryRational *entry)
{
	gtk_exif_entry_rational_save (entry);
}

GtkWidget *
gtk_exif_entry_rational_new (ExifEntry *e)
{
	GtkExifEntryRational *entry;
	GtkWidget *table, *label, *spin;
#if GTK_CHECK_VERSION(3,0,0)
	GtkAdjustment *a;
#else
	GtkObject *a;
#endif
	gchar *txt;
	guint i;

	g_return_val_if_fail (e != NULL, NULL);
	g_return_val_if_fail ((e->format == EXIF_FORMAT_RATIONAL) ||
			      (e->format == EXIF_FORMAT_SRATIONAL), NULL);

	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");

	entry = g_object_new (GTK_EXIF_TYPE_ENTRY_RATIONAL, NULL);
	entry->priv->entry = e;
	exif_entry_ref (e);
	gtk_exif_entry_construct (GTK_EXIF_ENTRY (entry),
		exif_tag_get_title (e->tag),
		exif_tag_get_description (e->tag));

	table = gtk_table_new (4, 1, FALSE);
	gtk_widget_show (table);
	gtk_box_pack_start (GTK_BOX (entry), table, TRUE, TRUE, 0);
	gtk_table_set_col_spacings (GTK_TABLE (table), 5);
	gtk_table_set_row_spacings (GTK_TABLE (table), 5);

	g_ptr_array_set_size (entry->priv->ap, e->components);
	g_ptr_array_set_size (entry->priv->aq, e->components);
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
		entry->priv->ap->pdata[i] = a;
		g_signal_connect (a, "value_changed",
			G_CALLBACK (on_adjustment_value_changed), entry);

		label = gtk_label_new ("/");
		gtk_widget_show (label);
		gtk_table_attach (GTK_TABLE (table), label, 2, 3, i, i + 1,
				  0, 0, 0, 0);

		a = gtk_adjustment_new (0, 0, 0xffff, 1, 0xff, 0);
		spin = gtk_spin_button_new (GTK_ADJUSTMENT (a), 0, 0);
		gtk_widget_show (spin);
		gtk_table_attach (GTK_TABLE (table), spin, 3, 4, i, i + 1,
				  GTK_FILL | GTK_EXPAND, 0, 0, 0);
		entry->priv->aq->pdata[i] = a;
		g_signal_connect (a, "value_changed",
			G_CALLBACK (on_adjustment_value_changed), entry);
	}

	gtk_exif_entry_rational_load (entry);

	return (GTK_WIDGET (entry));
}
