/* gtk-option-menu-option.c
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
#include "gtk-option-menu-option.h"

#include <string.h>

#include <gtk/gtksignal.h>

#include "gtk-menu-option.h"

struct _GtkOptionMenuOptionPrivate {
};

#define PARENT_TYPE GTK_TYPE_OPTION_MENU
static GtkOptionMenuClass *parent_class;

enum {
	OPTION_SELECTED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};

static void
gtk_option_menu_option_destroy (GtkObject *object)
{
	GtkOptionMenuOption *menu = GTK_OPTION_MENU_OPTION (object);

	menu = NULL;

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gtk_option_menu_option_finalize (GObject *object)
{
	GtkOptionMenuOption *menu = GTK_OPTION_MENU_OPTION (object);

	g_free (menu->priv);

	G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_option_menu_option_class_init (gpointer g_class, gpointer class_data)
{
	GtkObjectClass *object_class;
	GObjectClass *gobject_class;

	object_class = GTK_OBJECT_CLASS (g_class);
	object_class->destroy  = gtk_option_menu_option_destroy;

	gobject_class = G_OBJECT_CLASS (g_class);
	gobject_class->finalize = gtk_option_menu_option_finalize;

	signals[OPTION_SELECTED] = g_signal_new ("option_selected",
		G_TYPE_FROM_CLASS (g_class), G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (GtkOptionMenuOptionClass, option_selected),
		NULL, NULL,
		g_cclosure_marshal_VOID__UINT, G_TYPE_NONE, 1, G_TYPE_UINT);

	parent_class = g_type_class_peek_parent (g_class);
}

static void
gtk_option_menu_option_init (GTypeInstance *instance, gpointer g_class)
{
	GtkOptionMenuOption *menu = GTK_OPTION_MENU_OPTION (instance);

	menu->priv = g_new0 (GtkOptionMenuOptionPrivate, 1);
}

GType
gtk_option_menu_option_get_type (void)
{
	static GType t = 0;
	GTypeInfo ti;

	if (!t) {
		memset (&ti, 0, sizeof (GTypeInfo));
		ti.class_size    = sizeof (GtkOptionMenuOptionClass);
		ti.class_init    = gtk_option_menu_option_class_init;
		ti.instance_size = sizeof (GtkOptionMenuOption);
		ti.instance_init = gtk_option_menu_option_init;

		t = g_type_register_static (PARENT_TYPE, "GtkOptionMenuOption",
					    &ti, 0);
	}

	return (t);
}

static void
on_option_set (GtkMenuOption *m, guint option, GtkOptionMenuOption *menu)
{
	guint index;

	index = gtk_menu_option_get_index (m, option);
	gtk_option_menu_set_history (GTK_OPTION_MENU (menu), index);
}

static void
on_option_selected (GtkMenuOption *m, guint option, GtkOptionMenuOption *menu)
{
	g_signal_emit (G_OBJECT (menu), signals[OPTION_SELECTED], 0, option);
}

GtkWidget *
gtk_option_menu_option_new (GtkOptions *options)
{
	GtkOptionMenuOption *menu;
	GtkWidget *m;

	g_return_val_if_fail (options != NULL, NULL);

	menu = g_object_new (GTK_TYPE_OPTION_MENU_OPTION, NULL);

	m = gtk_menu_option_new (options);
	gtk_widget_show (m);
	gtk_option_menu_set_menu (GTK_OPTION_MENU (menu), m);
	g_signal_connect (G_OBJECT (m), "option_set",
			  G_CALLBACK (on_option_set), menu);
	g_signal_connect (G_OBJECT (m), "option_selected",
			  G_CALLBACK (on_option_selected), menu);

	return (GTK_WIDGET (menu));
}

guint
gtk_option_menu_option_get (GtkOptionMenuOption *menu)
{
	GtkWidget *m;

	g_return_val_if_fail (GTK_IS_OPTION_MENU_OPTION (menu), 0);

	m = gtk_option_menu_get_menu (GTK_OPTION_MENU (menu));
	return (gtk_menu_option_get (GTK_MENU_OPTION (m)));
}

void
gtk_option_menu_option_set (GtkOptionMenuOption *menu, guint option)
{
	GtkWidget *m;

	g_return_if_fail (GTK_IS_OPTION_MENU_OPTION (menu));

	m = gtk_option_menu_get_menu (GTK_OPTION_MENU (menu));
	gtk_menu_option_set (GTK_MENU_OPTION (m), option);
}
