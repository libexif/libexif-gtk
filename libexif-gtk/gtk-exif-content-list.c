/* gtk-exif-content-list.c
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
#include "gtk-exif-content-list.h"

#include <string.h>

#include <gtk/gtkmenu.h>
#include <gtk/gtkmenuitem.h>
#include <gtk/gtkliststore.h>
#include <gtk/gtkcellrenderertext.h>
#include <gtk/gtktreeselection.h>
#include <gtk/gtktreestore.h>

#include "gtk-exif-tag-menu.h"

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

struct _GtkExifContentListPrivate {
	GtkListStore *store;
};

#define PARENT_TYPE GTK_TYPE_TREE_VIEW
static GtkTreeViewClass *parent_class;

enum {
	NAME_COLUMN = 0,
	VALUE_COLUMN,
	ENTRY_COLUMN,
	NUM_COLUMNS
};

enum {
	ENTRY_ADDED,
	ENTRY_REMOVED,
	ENTRY_CHANGED,
	ENTRY_SELECTED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};

static void
gtk_exif_content_list_destroy (GtkObject *object)
{
	GtkExifContentList *list = GTK_EXIF_CONTENT_LIST (object);

	if (list->content) {
		exif_content_unref (list->content);
		list->content = NULL;
	}

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gtk_exif_content_list_finalize (GObject *object)
{
	GtkExifContentList *list = GTK_EXIF_CONTENT_LIST (object);

	g_free (list->priv);

	G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_exif_content_list_class_init (gpointer g_class, gpointer class_data)
{
	GtkObjectClass *object_class;
	GObjectClass *gobject_class;

	object_class = GTK_OBJECT_CLASS (g_class);
	object_class->destroy  = gtk_exif_content_list_destroy;

	gobject_class = G_OBJECT_CLASS (g_class);
	gobject_class->finalize = gtk_exif_content_list_finalize;

	signals[ENTRY_SELECTED] = g_signal_new ("entry_selected",
		G_SIGNAL_RUN_LAST, G_TYPE_FROM_CLASS (g_class),
		G_STRUCT_OFFSET (GtkExifContentListClass, entry_selected),
		NULL, NULL,
		g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1,
		G_TYPE_POINTER);
	signals[ENTRY_ADDED] = g_signal_new ("entry_added",
		G_SIGNAL_RUN_FIRST, G_TYPE_FROM_CLASS (g_class),
		G_STRUCT_OFFSET (GtkExifContentListClass, entry_added),
		NULL, NULL,
		g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1,
		G_TYPE_POINTER);
	signals[ENTRY_CHANGED] = g_signal_new ("entry_changed",
		G_SIGNAL_RUN_FIRST, G_TYPE_FROM_CLASS (g_class),
		G_STRUCT_OFFSET (GtkExifContentListClass, entry_changed),
		NULL, NULL,
		g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1,
		G_TYPE_POINTER);
	signals[ENTRY_REMOVED] = g_signal_new ("entry_removed",
		G_SIGNAL_RUN_FIRST, G_TYPE_FROM_CLASS (g_class),
		G_STRUCT_OFFSET (GtkExifContentListClass, entry_removed),
		NULL, NULL,
		g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1,
		G_TYPE_POINTER);

	parent_class = g_type_class_peek_parent (g_class);
}

static gboolean
selection_func (GtkTreeSelection *sel, GtkTreeModel *model,
		GtkTreePath *path, gboolean path_cur_selected,
		gpointer data)
{
	GtkExifContentList *list = GTK_EXIF_CONTENT_LIST (data);
	GValue value = {0};
	GtkTreeIter iter;

	if (path_cur_selected)
		return (TRUE);

	gtk_tree_model_get_iter (model, &iter, path);
	gtk_tree_model_get_value (model, &iter, ENTRY_COLUMN, &value);
	g_signal_emit (G_OBJECT (list), signals[ENTRY_SELECTED],
		0, g_value_peek_pointer (&value));
	g_value_unset (&value);

	return (TRUE);
}

static void
gtk_exif_content_list_init (GTypeInstance *instance, gpointer g_class)
{
	GtkExifContentList *list = GTK_EXIF_CONTENT_LIST (instance);
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *col;
	GtkTreeSelection *sel;

	/* Column for tags */
	renderer = gtk_cell_renderer_text_new ();
	col = gtk_tree_view_column_new_with_attributes (_("Tag"), renderer,
						"text", NAME_COLUMN, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (list), col);

	/* Column for values */
	renderer = gtk_cell_renderer_text_new ();
	col = gtk_tree_view_column_new_with_attributes (_("Value"), renderer,
						"text", VALUE_COLUMN, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (list), col);

	/* Catch selections */
	sel = gtk_tree_view_get_selection (GTK_TREE_VIEW (list));
	gtk_tree_selection_set_select_function (sel, selection_func, list,
						NULL);

	list->priv = g_new0 (GtkExifContentListPrivate, 1);

	list->priv->store = gtk_list_store_new (NUM_COLUMNS,
				G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);
}

GType
gtk_exif_content_list_get_type (void)
{
	static GtkType t = 0;

	if (!t) {
		GTypeInfo ti;

		memset (&ti, 0, sizeof (GTypeInfo));
		ti.class_size     = sizeof (GtkExifContentListClass);
		ti.class_init     = gtk_exif_content_list_class_init;
		ti.instance_size  = sizeof (GtkExifContentList);
		ti.instance_init  = gtk_exif_content_list_init;

		t = g_type_register_static (PARENT_TYPE,
					    "GtkExifContentList", &ti, 0);
	}

	return (t);
}

static void
on_hide (GtkWidget *widget, GtkMenu *menu)
{
	g_object_unref (G_OBJECT (menu));
}

static void
remove_foreach_func (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter,
		     gpointer data)
{
	GValue value = {0};
	ExifEntry *entry;
	GtkExifContentList *list = GTK_EXIF_CONTENT_LIST (data);

	gtk_tree_model_get_value (model, iter, ENTRY_COLUMN, &value);
	g_assert (G_VALUE_HOLDS (&value, G_TYPE_POINTER));
	entry = g_value_peek_pointer (&value);
	exif_entry_ref (entry);
	g_value_unset (&value);
	gtk_list_store_remove (list->priv->store, iter);

	g_signal_emit (G_OBJECT (list), signals[ENTRY_REMOVED], 0, entry);
	exif_entry_unref (entry);
}

static void
on_remove_activate (GtkMenuItem *item, GtkExifContentList *list)
{
	gtk_tree_selection_selected_foreach (
			gtk_tree_view_get_selection (GTK_TREE_VIEW (list)),
			remove_foreach_func, NULL);
}

static void
on_tag_selected (GtkExifTagMenu *menu, ExifTag tag, GtkExifContentList *list)
{
	ExifEntry *entry;

	entry = exif_entry_new ();
	exif_content_add_entry (list->content, entry);
	exif_entry_initialize (entry, tag);
	gtk_exif_content_list_add_entry (list, entry);
	exif_entry_unref (entry);
}

static gint
on_button_press_event (GtkWidget *widget, GdkEventButton *event, 
		       GtkExifContentList *list)
{
	GtkWidget *menu, *item, *smenu;

	g_return_val_if_fail (GTK_EXIF_IS_CONTENT_LIST (list), FALSE);

	switch (event->button) {
	case 3:
		menu = gtk_menu_new ();
		gtk_widget_show (menu);
		gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL,
				event->button, event->time);
		g_signal_connect (GTK_OBJECT (menu), "hide",
				    GTK_SIGNAL_FUNC (on_hide), menu);

		/* Add */
		item = gtk_menu_item_new_with_label ("Add");
		gtk_widget_show (item);
		gtk_container_add (GTK_CONTAINER (menu), item);
		smenu = gtk_exif_tag_menu_new ();
		gtk_menu_item_set_submenu (GTK_MENU_ITEM (item), smenu);
		g_signal_connect (GTK_OBJECT (smenu), "tag_selected",
				GTK_SIGNAL_FUNC (on_tag_selected), list);

		/* Remove */
		item = gtk_menu_item_new_with_label ("Remove");
		gtk_widget_show (item);
		gtk_container_add (GTK_CONTAINER (menu), item);
		g_signal_connect (GTK_OBJECT (item), "activate",
				GTK_SIGNAL_FUNC (on_remove_activate), list);

		return (TRUE);
	default:
		return (FALSE);
	}
}

GtkWidget *
gtk_exif_content_list_new (void)
{
	GtkExifContentList *list;

	list = g_object_new (GTK_EXIF_TYPE_CONTENT_LIST, NULL);
	g_signal_connect (G_OBJECT (list), "button_press_event",
			  G_CALLBACK (on_button_press_event), list);

	return (GTK_WIDGET (list));
}

static gboolean
update_foreach_func (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter,
		     void *data)
{
	ExifEntry *e = data;
	GValue value = {0};

	gtk_tree_model_get_value (model, iter, ENTRY_COLUMN, &value);
	if (g_value_peek_pointer (&value) == e) {
		g_value_unset (&value);
		gtk_list_store_set (GTK_LIST_STORE (model), iter,
				VALUE_COLUMN, exif_entry_get_value (e), NULL);
		return (TRUE);
	}
	g_value_unset (&value);
	return (FALSE);
}

void
gtk_exif_content_list_update_entry (GtkExifContentList *list, ExifEntry *entry)
{
	g_return_if_fail (GTK_EXIF_IS_CONTENT_LIST (list));
	g_return_if_fail (entry != NULL);

	gtk_tree_model_foreach (GTK_TREE_MODEL (list->priv->store),
				update_foreach_func, entry);
}

void
gtk_exif_content_list_add_entry (GtkExifContentList *list, ExifEntry *entry)
{
	GtkTreeIter iter;

	g_return_if_fail (GTK_EXIF_IS_CONTENT_LIST (list));

	gtk_list_store_append (list->priv->store, &iter);
	gtk_tree_store_set (GTK_TREE_STORE (list->priv->store), &iter,
			NAME_COLUMN, exif_tag_get_name (entry->tag),
			VALUE_COLUMN, exif_entry_get_value (entry),
			ENTRY_COLUMN, entry);
	g_signal_emit (G_OBJECT (list), signals[ENTRY_ADDED], 0, entry);
}

void
gtk_exif_content_list_set_content (GtkExifContentList *list,
				   ExifContent *content)
{
	guint i;

	g_return_if_fail (GTK_EXIF_IS_CONTENT_LIST (list));
	g_return_if_fail (content != NULL);

	if (list->content)
		exif_content_unref (list->content);
	list->content = content;
	exif_content_ref (content);

	gtk_list_store_clear (list->priv->store);

	for (i = 0; i < content->count; i++)
		gtk_exif_content_list_add_entry (list, content->entries[i]);
}
