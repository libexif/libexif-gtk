/* gtk-exif-tag-menu.c
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
#include "gtk-exif-tag-menu.h"

#include <string.h>

#include <gtk/gtksignal.h>

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

struct _GtkExifTagMenuPrivate {
};

#define PARENT_TYPE GTK_TYPE_MENU_OPTION
static GtkMenuOptionClass *parent_class;

enum {
	TAG_SELECTED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};

static void
gtk_exif_tag_menu_destroy (GtkObject *object)
{
	GtkExifTagMenu *menu = GTK_EXIF_TAG_MENU (object);

	menu = NULL;

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

GTK_EXIF_FINALIZE (tag_menu, TagMenu)

static void
gtk_exif_tag_menu_class_init (gpointer g_class, gpointer class_data)
{
	GtkObjectClass *object_class;
	GObjectClass *gobject_class;

	object_class = GTK_OBJECT_CLASS (g_class);
	object_class->destroy  = gtk_exif_tag_menu_destroy;

	gobject_class = G_OBJECT_CLASS (g_class);
	gobject_class->finalize = gtk_exif_tag_menu_finalize;

	signals[TAG_SELECTED] = g_signal_new ("tag_selected",
		G_TYPE_FROM_CLASS (g_class), G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (GtkExifTagMenuClass, tag_selected),
		NULL, NULL,
		g_cclosure_marshal_VOID__UINT, G_TYPE_NONE, 1, G_TYPE_UINT);

	parent_class = g_type_class_peek_parent (g_class);
}

static void
gtk_exif_tag_menu_init (GTypeInstance *instance, gpointer g_class)
{
	GtkExifTagMenu *menu = GTK_EXIF_TAG_MENU (instance);

	menu->priv = g_new0 (GtkExifTagMenuPrivate, 1);
}

GTK_EXIF_CLASS (tag_menu, TagMenu, "TagMenu")

static void
on_option_selected (GtkMenuOption *options, guint option, GtkExifTagMenu *menu)
{
	g_signal_emit (menu, signals[TAG_SELECTED], 0, option);
}

#define LIST_SIZE 1024

GtkWidget *
gtk_exif_tag_menu_new (void)
{
	GtkExifTagMenu *menu;
	GtkOptions tags[LIST_SIZE];
	guint i, t;
	const gchar *name;

	menu = g_object_new (GTK_EXIF_TYPE_TAG_MENU, NULL);

	t = i = 0;
	memset (tags, 0, sizeof (GtkOptions) * LIST_SIZE);
	while ((t < 0xffff) && (i < LIST_SIZE - 1)) {
		name = exif_tag_get_name (t);
		if (name) {
			tags[i].option = t;
			tags[i].name = name;
			i++;
		}
		t++;
	}

	gtk_menu_option_construct (GTK_MENU_OPTION (menu), tags);
	g_signal_connect (GTK_OBJECT (menu), "option_selected",
			  G_CALLBACK (on_option_selected), menu);

	return (GTK_WIDGET (menu));
}
