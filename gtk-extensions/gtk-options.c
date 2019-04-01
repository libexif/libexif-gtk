/* gtk-options.c
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
#include "gtk-options.h"

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

void
gtk_options_sort (GtkOptions *options)
{
	GtkOptions entry;
	guint i = 0;

	while (options[i+1].name) {
		if (strcmp (options[i].name, options[i + 1].name) > 0) {
			entry = options[i];
			options[i] = options[i + 1];
			options[i + 1] = entry;
			if (i)
				i--;
		} else
			i++;
	}
}

GtkTreeModel *
gtk_tree_model_new_from_options (GtkOptions *options)
{
	GtkListStore *ls;
	guint i;
	GtkTreeIter iter;

	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");

	ls = gtk_list_store_new (GTK_OPTIONS_N_COLUMNS, G_TYPE_INT,
				 G_TYPE_STRING);
	for (i = 0; options[i].name; i++) {
		gtk_list_store_append (ls, &iter);
		gtk_list_store_set (ls, &iter,
			GTK_OPTIONS_OPTION_COLUMN, options[i].option,
			GTK_OPTIONS_NAME_COLUMN, _(options[i].name), -1);
	}

	return GTK_TREE_MODEL (ls);
}

gboolean
gtk_tree_model_get_iter_from_option (GtkTreeModel *tm, guint option,
				     GtkTreeIter *iter)
{
	GValue v = {0,};

	g_return_val_if_fail (GTK_IS_TREE_MODEL (tm), FALSE);
	g_return_val_if_fail (iter != NULL, FALSE);

	/* Search the option in the list store. */
	if (!gtk_tree_model_get_iter_first (tm, iter)) return FALSE;
	do {
		gtk_tree_model_get_value (tm, iter, GTK_OPTIONS_OPTION_COLUMN,
					  &v);
		if ((int)option == g_value_get_int (&v)) break;
		g_value_unset (&v);
	} while (gtk_tree_model_iter_next (tm, iter));

	return ((int)option == g_value_get_int (&v));
}
