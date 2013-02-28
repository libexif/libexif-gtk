/* gtk-exif-entry-flash.c
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
#include "gtk-exif-entry-flash.h"
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

struct _GtkExifEntryFlashPrivate {
	ExifEntry *entry;

	GtkToggleButton *c;
	GtkToggleButton *r1, *r2, *r3;
};

#define PARENT_TYPE GTK_EXIF_TYPE_ENTRY
static GtkExifEntryClass *parent_class;

static void
#if GTK_CHECK_VERSION(3,0,0)
gtk_exif_entry_flash_destroy (GtkWidget *widget)
#else
gtk_exif_entry_flash_destroy (GtkObject *object)
#endif
{
#if GTK_CHECK_VERSION(3,0,0)
	GtkExifEntryFlash *entry = GTK_EXIF_ENTRY_FLASH (widget);
#else
	GtkExifEntryFlash *entry = GTK_EXIF_ENTRY_FLASH (object);
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

GTK_EXIF_FINALIZE (entry_flash, EntryFlash)

static void
gtk_exif_entry_flash_class_init (gpointer g_class, gpointer class_data)
{
#if GTK_CHECK_VERSION(3,0,0)
	GtkWidgetClass *widget_class;
	GObjectClass *gobject_class;

	widget_class = GTK_WIDGET_CLASS (g_class);
	widget_class->destroy = gtk_exif_entry_flash_destroy;
#else
	GtkObjectClass *object_class;
	GObjectClass *gobject_class;

	object_class = GTK_OBJECT_CLASS (g_class);
	object_class->destroy  = gtk_exif_entry_flash_destroy;
#endif

	gobject_class = G_OBJECT_CLASS (g_class);
	gobject_class->finalize = gtk_exif_entry_flash_finalize;

	parent_class = g_type_class_peek_parent (g_class);
}

static void
gtk_exif_entry_flash_init (GTypeInstance *instance, gpointer g_class)
{
	GtkExifEntryFlash *entry = GTK_EXIF_ENTRY_FLASH (instance);

	entry->priv = g_new0 (GtkExifEntryFlashPrivate, 1);
}

GTK_EXIF_CLASS (entry_flash, EntryFlash, "EntryFlash")

static void
on_value_changed (GtkToggleButton *toggle, GtkExifEntryFlash *entry)
{
	unsigned char *d;
	ExifByteOrder o;
	ExifShort v;
	g_return_if_fail (GTK_EXIF_IS_ENTRY_FLASH (entry));

	d = entry->priv->entry->data;
	o = exif_data_get_byte_order (entry->priv->entry->parent->parent);
	v = exif_get_short(d,o);

	v &= 0xfe;
	if (gtk_toggle_button_get_active (entry->priv->c))
		v |= 0x01;

	v &= 0xf9;
	if (gtk_toggle_button_get_active (entry->priv->r2))
		v |= 0x04;
	else if (gtk_toggle_button_get_active (entry->priv->r3))
		v |= 0x06;
	exif_set_short(d, o, v);
	g_signal_emit_by_name (G_OBJECT (entry), "entry_changed",
				 entry->priv->entry);
}

GtkWidget *
gtk_exif_entry_flash_new (ExifEntry *e)
{
	GtkExifEntryFlash *entry;
	GtkWidget *check, *frame, *vbox, *radio;
	GSList *group;
	ExifByteOrder o;

	g_return_val_if_fail (e != NULL, NULL);
	g_return_val_if_fail (e->tag == EXIF_TAG_FLASH, NULL);
	g_return_val_if_fail (e->format == EXIF_FORMAT_SHORT, NULL);

	entry = g_object_new (GTK_EXIF_TYPE_ENTRY_FLASH, NULL);
	entry->priv->entry = e;
	exif_entry_ref (e);
	o = exif_data_get_byte_order (e->parent->parent);
	gtk_exif_entry_construct (GTK_EXIF_ENTRY (entry),
		exif_tag_get_title_in_ifd (e->tag, exif_content_get_ifd(e->parent)),
		exif_tag_get_description_in_ifd (e->tag, exif_content_get_ifd(e->parent)));

	check = gtk_check_button_new_with_label (_("Flash fired"));
	gtk_widget_show (check);
	gtk_box_pack_start (GTK_BOX (entry), check, FALSE, FALSE, 0);
	if (exif_get_short(e->data, o) & (1 << 0))
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check), TRUE);
	g_signal_connect (G_OBJECT (check), "toggled",
			    G_CALLBACK (on_value_changed), entry);
	entry->priv->c = GTK_TOGGLE_BUTTON (check);

	frame = gtk_frame_new (_("Return light"));
	gtk_widget_show (frame);
	gtk_box_pack_start (GTK_BOX (entry), frame, FALSE, FALSE, 0);
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
	gtk_container_add (GTK_CONTAINER (frame), vbox);

	/* No strobe return detection function */
	radio = gtk_radio_button_new_with_label (NULL, _("No strobe return "
						 "detection function"));
	gtk_widget_show (radio);
	gtk_box_pack_start (GTK_BOX (vbox), radio, FALSE, FALSE, 0);
	if (!(exif_get_short(e->data, o) & (1 << 1)) && !(exif_get_short(e->data, o) & (1 << 2)))
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radio), TRUE);
	g_signal_connect (G_OBJECT (radio), "toggled",
			    G_CALLBACK (on_value_changed), entry);
	entry->priv->r1 = GTK_TOGGLE_BUTTON (radio);

	/* Stobe return light not detected */
	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radio));
	radio = gtk_radio_button_new_with_label (group,
				_("Strobe return light not detected"));
	gtk_widget_show (radio);
	gtk_box_pack_start (GTK_BOX (vbox), radio, FALSE, FALSE, 0);
	if (!(exif_get_short(e->data, o) & (1 << 1)) && (exif_get_short(e->data, o) & (1 << 2)))
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radio), TRUE);
	g_signal_connect (G_OBJECT (radio), "toggled",
			    G_CALLBACK (on_value_changed), entry);
	entry->priv->r2 = GTK_TOGGLE_BUTTON (radio);

	/* Strobe return light detected */
	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radio));
	radio = gtk_radio_button_new_with_label (group,
					_("Strobe return light detected"));
	gtk_widget_show (radio);
	gtk_box_pack_start (GTK_BOX (vbox), radio, FALSE, FALSE, 0);
	if ((exif_get_short(e->data, o) & (1 << 1)) && (exif_get_short(e->data, o) & (1 << 2)))
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radio), TRUE);
	g_signal_connect (G_OBJECT (radio), "toggled",
			    G_CALLBACK (on_value_changed), entry);
	entry->priv->r3 = GTK_TOGGLE_BUTTON (radio);

	return (GTK_WIDGET (entry));
}
