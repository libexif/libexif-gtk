/* gtk-exif-entry-version.c
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
#include "gtk-exif-entry-version.h"

#include <string.h>

#include <gtk/gtkcheckbutton.h>
#include <gtk/gtkradiobutton.h>
#include <gtk/gtkvbox.h>
#include <gtk/gtksignal.h>
#include <gtk/gtkframe.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkentry.h>
#include <gtk/gtkoptionmenu.h>
#include <gtk/gtkmenuitem.h>
#include <gtk/gtkmenu.h>
#include <gtk/gtkhbox.h>

#include "gtk-exif-util.h"

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

#include "gtk-extensions/gtk-option-menu-option.h"
#include "gtk-extensions/gtk-options.h"

#include <string.h>

struct _GtkExifEntryVersionPrivate {
	ExifEntry *entry;

	GtkOptionMenuOption *menu;
};

#define PARENT_TYPE GTK_EXIF_TYPE_ENTRY
static GtkExifEntryClass *parent_class;

static void
gtk_exif_entry_version_destroy (GtkObject *object)
{
	GtkExifEntryVersion *entry = GTK_EXIF_ENTRY_VERSION (object);

	if (entry->priv->entry) {
		exif_entry_unref (entry->priv->entry);
		entry->priv->entry = NULL;
	}

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

GTK_EXIF_FINALIZE (entry_version, EntryVersion)

static void
gtk_exif_entry_version_class_init (gpointer g_class, gpointer class_data)
{
	GtkObjectClass *object_class;
	GObjectClass *gobject_class;

	object_class = GTK_OBJECT_CLASS (g_class);
	object_class->destroy  = gtk_exif_entry_version_destroy;

	gobject_class = G_OBJECT_CLASS (g_class);
	gobject_class->finalize = gtk_exif_entry_version_finalize;

	parent_class = g_type_class_peek_parent (g_class);
}

static void
gtk_exif_entry_version_init (GTypeInstance *instance, gpointer g_class)
{
	GtkExifEntryVersion *entry = GTK_EXIF_ENTRY_VERSION (instance);

	entry->priv = g_new0 (GtkExifEntryVersionPrivate, 1);
}

GTK_EXIF_CLASS (entry_version, EntryVersion, "EntryVersion")

typedef enum _ExifVersion ExifVersion;
enum _ExifVersion {
	EXIF_VERSION_2_0,
	EXIF_VERSION_2_1,
	EXIF_VERSION_2_2
};

typedef enum _FlashPixVersion FlashPixVersion;
enum _FlashPixVersion {
	FLASH_PIX_VERSION_1
};

static GtkOptions exif_list[] = {
        {EXIF_VERSION_2_0, N_("Exif Format Version 2.0")},
        {EXIF_VERSION_2_1, N_("Exif Format Version 2.1")},
	{EXIF_VERSION_2_2, N_("Exif Format Version 2.2")},
        {0, NULL}
};

static GtkOptions flash_pix_list[] = {
        {FLASH_PIX_VERSION_1, N_("FlashPix Format Version 1.0")},
        {0, NULL}
};

static struct {
	ExifVersion version;
	const guchar *data;
} exif_versions[] = {
	{EXIF_VERSION_2_0, "0200"},
	{EXIF_VERSION_2_1, "0210"},
	{EXIF_VERSION_2_2, "0220"},
	{0, NULL}
};

static struct {
	FlashPixVersion version;
	const guchar *data;
} flash_pix_versions[] = {
	{FLASH_PIX_VERSION_1, "0100"},
	{0, NULL}
};

static void
gtk_exif_entry_version_load (GtkExifEntryVersion *entry)
{
	guint i;

	g_return_if_fail (GTK_EXIF_IS_ENTRY_VERSION (entry));

	switch (entry->priv->entry->tag) {
	case EXIF_TAG_EXIF_VERSION:
		for (i = 0; exif_versions[i].data; i++)
			if (!memcmp (exif_versions[i].data,
				     entry->priv->entry->data, 4)) {
				gtk_option_menu_option_set (entry->priv->menu,
						 exif_versions[i].version);
				break;
			}
		break;
	case EXIF_TAG_FLASH_PIX_VERSION:
		for (i = 0; flash_pix_versions[i].data; i++)
			if (!memcmp (flash_pix_versions[i].data, 
				     entry->priv->entry->data, 4)) {
				gtk_option_menu_option_set (entry->priv->menu, 
						 flash_pix_versions[i].version);
				break;
			}
		break;
	default:
		break;
	}
}

static void
gtk_exif_entry_version_save (GtkExifEntryVersion *entry)
{
	guint option, i;

	option = gtk_option_menu_option_get (entry->priv->menu);
	switch (entry->priv->entry->tag) {
	case EXIF_TAG_EXIF_VERSION:
		for (i = 0; exif_versions[i].data; i++)
			if (option == exif_versions[i].version) {
				memcpy (entry->priv->entry->data,
					exif_versions[i].data, 4);
				break;
		}
		break;
	case EXIF_TAG_FLASH_PIX_VERSION:
		for (i = 0; flash_pix_versions[i].data; i++)
			if (option == flash_pix_versions[i].version) {
				memcpy (entry->priv->entry->data,
					flash_pix_versions[i].data, 4);
				break;
			}
		break;
	default:
		break;
	}
	gtk_exif_entry_changed (GTK_EXIF_ENTRY (entry), entry->priv->entry);
}

static void
on_option_selected (GtkOptions *options, guint option, GtkExifEntryVersion *entry)
{
	gtk_exif_entry_version_save (entry);
}

GtkWidget *
gtk_exif_entry_version_new (ExifEntry *e)
{
	GtkExifEntryVersion *entry;
	GtkWidget *hbox, *label, *options;

	g_return_val_if_fail (e != NULL, NULL);
	g_return_val_if_fail ((e->tag == EXIF_TAG_EXIF_VERSION) ||
			      (e->tag == EXIF_TAG_FLASH_PIX_VERSION), NULL);
	g_return_val_if_fail (e->format == EXIF_FORMAT_UNDEFINED, NULL);
	g_return_val_if_fail (e->components == 4, NULL);
	g_return_val_if_fail (e->data != NULL, NULL);

	entry = g_object_new (GTK_EXIF_TYPE_ENTRY_VERSION, NULL);
	entry->priv->entry = e;
	exif_entry_ref (e);
	gtk_exif_entry_construct (GTK_EXIF_ENTRY (entry),
			exif_tag_get_title (e->tag),
			exif_tag_get_description (e->tag));

	hbox = gtk_hbox_new (FALSE, 5);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (entry), hbox, TRUE, FALSE, 0);
	label = gtk_label_new (_("Version:"));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	switch (e->tag) {
	case EXIF_TAG_EXIF_VERSION:
		options = gtk_option_menu_option_new (exif_list);
		break;
	case EXIF_TAG_FLASH_PIX_VERSION:
	default:
		options = gtk_option_menu_option_new (flash_pix_list);
		break;
	}
	gtk_widget_show (options);
	gtk_box_pack_start (GTK_BOX (hbox), options, FALSE, FALSE, 0);
	entry->priv->menu = GTK_OPTION_MENU_OPTION (options);
	g_signal_connect (GTK_OBJECT (options), "option_selected",
			  G_CALLBACK (on_option_selected), entry);

	gtk_exif_entry_version_load (entry);

	return (GTK_WIDGET (entry));
}
