/* gtk-exif-entry-user-comment.
 *
 * Copyright © 2002 Lutz Müller <lutz@users.sourceforge.net>
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
#include "gtk-exif-entry-user-comment.h"

#include <string.h>
#include <stdlib.h>

#include <gtk/gtklabel.h>
#include <gtk/gtkhbox.h>
#include <gtk/gtkentry.h>
#include <gtk/gtkcellrenderertext.h>
#include <gtk/gtkcelllayout.h>
#include <gtk/gtkcombobox.h>

#include "gtk-exif-util.h"

#include "gtk-extensions/gtk-options.h"

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

struct _GtkExifEntryUserCommentPrivate {
	ExifEntry *entry;

	GtkComboBox *menu;
	GtkEntry *entry_widget;
};

#define PARENT_TYPE GTK_EXIF_TYPE_ENTRY
static GtkExifEntryClass *parent_class;

static void
gtk_exif_entry_user_comment_destroy (GtkObject *object)
{
	GtkExifEntryUserComment *entry = GTK_EXIF_ENTRY_USER_COMMENT (object);

	if (entry->priv->entry) {
		exif_entry_unref (entry->priv->entry);
		entry->priv->entry = NULL;
	}

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

GTK_EXIF_FINALIZE (entry_user_comment, EntryUserComment)

static void
gtk_exif_entry_user_comment_class_init (gpointer g_class, gpointer class_data)
{
	GtkObjectClass *object_class;
	GObjectClass *gobject_class;

	object_class = GTK_OBJECT_CLASS (g_class);
	object_class->destroy  = gtk_exif_entry_user_comment_destroy;

	gobject_class = G_OBJECT_CLASS (g_class);
	gobject_class->finalize = gtk_exif_entry_user_comment_finalize;

	parent_class = g_type_class_peek_parent (g_class);
}

static void
gtk_exif_entry_user_comment_init (GTypeInstance *instance, gpointer g_class)
{
	GtkExifEntryUserComment *entry = GTK_EXIF_ENTRY_USER_COMMENT (instance);

	entry->priv = g_new0 (GtkExifEntryUserCommentPrivate, 1);
}

GTK_EXIF_CLASS (entry_user_comment, EntryUserComment, "EntryUserComment")

typedef enum _CharacterCode CharacterCode;
enum _CharacterCode {
	CHARACTER_CODE_ASCII,
	CHARACTER_CODE_JIS,
	CHARACTER_CODE_UNICODE,
	CHARACTER_CODE_UNDEFINED
};

static struct {
	CharacterCode code;
	const guchar *data;
} character_codes[] = {
	{CHARACTER_CODE_ASCII    , "ASCII\0\0\0"     },
	{CHARACTER_CODE_JIS      , "JIS\0\0\0\0\0"   },
	{CHARACTER_CODE_UNICODE  , "UNICODE\0"       },
	{CHARACTER_CODE_UNDEFINED, "\0\0\0\0\0\0\0\0"},
	{0, NULL}
};

static void
gtk_exif_entry_user_comment_load (GtkExifEntryUserComment *entry)
{
	guint i;
	gchar *s;
	GtkTreeIter iter;
	GtkTreeModel *tm;

	g_return_if_fail (GTK_EXIF_IS_ENTRY_USER_COMMENT (entry));

	tm = gtk_combo_box_get_model (entry->priv->menu);
	if (entry->priv->entry->size < 8) return;
	for (i = 0; character_codes[i].data &&
		    memcmp (character_codes[i].data,
			    entry->priv->entry->data, 8); i++);
	if (!memcmp (character_codes[i].data, entry->priv->entry->data, 8)) {
		gtk_tree_model_get_iter_from_option (tm,
					character_codes[i].code, &iter);
		gtk_combo_box_set_active_iter (entry->priv->menu, &iter);
	}

	if (entry->priv->entry->size < 9) return;
	s = g_new0 (gchar, entry->priv->entry->size - 8 + 1);
	if (!s) return;
	memcpy (s, entry->priv->entry->data + 8, entry->priv->entry->size - 8);
	gtk_entry_set_text (entry->priv->entry_widget, s);
	g_free (s);
}

static void
gtk_exif_entry_user_comment_save (GtkExifEntryUserComment *entry)
{
	guint i;
	char *d;
	unsigned int s;
	const gchar *t;
	GtkTreeIter iter;
	GtkTreeModel *tm;
	GValue v = {0,};

	tm = gtk_combo_box_get_model (entry->priv->menu);
	gtk_combo_box_get_active_iter (entry->priv->menu, &iter);
	gtk_tree_model_get_value (tm, &iter, GTK_OPTIONS_OPTION_COLUMN, &v);
	for (i = 0; character_codes[i].data &&
		    (g_value_get_int (&v) != character_codes[i].code); i++);
	if (g_value_get_int (&v) == character_codes[i].code) {

		/*
		 * Make sure we have enough data left to save
		 * the character code.
		 */
		if (entry->priv->entry->size < 8) {
			s = sizeof (char) * 8;
			d = realloc (entry->priv->entry->data, s);
			if (!d) return;
			entry->priv->entry->data = d;
			entry->priv->entry->size = s;
		}
		memcpy (entry->priv->entry->data,
			character_codes[i].data, 8);
	}

	/* Save the actual comment. */
	t = gtk_entry_get_text (entry->priv->entry_widget);
	if (!t)
		return;
	if (entry->priv->entry->size < strlen (t) + 8) {
		s = sizeof (char) * (strlen (t) + 8);
		d = realloc (entry->priv->entry->data, s);
		if (!d)
			return;
		entry->priv->entry->data = d;
		entry->priv->entry->size = s;
	}
	memcpy (entry->priv->entry->data + 8, t, strlen (t));

	gtk_exif_entry_changed (GTK_EXIF_ENTRY (entry), entry->priv->entry);
}

static void
on_character_code_changed (GtkComboBox *cb, GtkExifEntryUserComment *entry)
{
	gtk_exif_entry_user_comment_save (entry);
}

static GtkOptions character_codes_list[] = {
	{CHARACTER_CODE_ASCII    , N_("ASCII"    )},
	{CHARACTER_CODE_JIS      , N_("JIS"      )},
	{CHARACTER_CODE_UNICODE  , N_("Unicode"  )},
	{CHARACTER_CODE_UNDEFINED, N_("Undefined")},
	{0, NULL}
};

static void
on_changed (GtkEntry *w, GtkExifEntryUserComment *entry)
{
	gtk_exif_entry_user_comment_save (entry);
}

GtkWidget *
gtk_exif_entry_user_comment_new (ExifEntry *e)
{
	GtkExifEntryUserComment *entry;
	GtkWidget *hbox, *w;
	GtkCellRenderer *cell;

	g_return_val_if_fail (e != NULL, NULL);
	g_return_val_if_fail (e->format == EXIF_FORMAT_UNDEFINED, NULL);
	g_return_val_if_fail (e->tag == EXIF_TAG_USER_COMMENT, NULL);

	entry = g_object_new (GTK_EXIF_TYPE_ENTRY_USER_COMMENT, NULL);
	entry->priv->entry = e;
	exif_entry_ref (e);
	gtk_exif_entry_construct (GTK_EXIF_ENTRY (entry),
		exif_tag_get_title (e->tag),
		exif_tag_get_description (e->tag));

	hbox = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (entry), hbox, FALSE, FALSE, 5);
	w = gtk_label_new (_("Character Code:"));
	gtk_widget_show (w);
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	w = gtk_combo_box_new_with_model (
		gtk_tree_model_new_from_options (character_codes_list));
	gtk_widget_show (w);
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	entry->priv->menu = GTK_COMBO_BOX (w);
	cell = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (w), cell, TRUE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (w), cell,
			"text", GTK_OPTIONS_NAME_COLUMN, NULL);
	g_signal_connect (G_OBJECT (w), "changed",
			  G_CALLBACK (on_character_code_changed), entry);
	w = gtk_entry_new ();
	gtk_box_pack_start (GTK_BOX (entry), w, FALSE, FALSE, 5);
	gtk_widget_show (w);
	g_signal_connect (w, "changed", G_CALLBACK (on_changed), entry);
	entry->priv->entry_widget = GTK_ENTRY (w);

	gtk_exif_entry_user_comment_load (entry);

	return (GTK_WIDGET (entry));
}
