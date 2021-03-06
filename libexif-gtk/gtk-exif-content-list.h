/* gtk-exif-content-list.h
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

#ifndef __GTK_EXIF_CONTENT_LIST_H__
#define __GTK_EXIF_CONTENT_LIST_H__

#include <gtk/gtk.h>
#include <libexif/exif-data.h>

#define GTK_EXIF_TYPE_CONTENT_LIST     (gtk_exif_content_list_get_type())
#define GTK_EXIF_CONTENT_LIST(o)       (G_TYPE_CHECK_INSTANCE_CAST((o),GTK_EXIF_TYPE_CONTENT_LIST,GtkExifContentList))
#define GTK_EXIF_CONTENT_LIST_CLASS(k) (G_TYPE_CHECK_CLASS_CAST((k),GTK_EXIF_TYPE_CONTENT_LIST,GtkExifContentListClass))
#define GTK_EXIF_IS_CONTENT_LIST(o)    (G_TYPE_CHECK_INSTANCE_TYPE((o),GTK_EXIF_TYPE_CONTENT_LIST))

typedef struct _GtkExifContentList        GtkExifContentList;
typedef struct _GtkExifContentListPrivate GtkExifContentListPrivate;
typedef struct _GtkExifContentListClass   GtkExifContentListClass;

struct _GtkExifContentList
{
	GtkTreeView parent;

	ExifContent *content;

	GtkExifContentListPrivate *priv;
};

struct _GtkExifContentListClass
{
	GtkTreeViewClass parent_class;

	/* Signals */
	void (* entry_selected)  (GtkExifContentList *list, ExifEntry *);
	void (* entry_added)     (GtkExifContentList *list, ExifEntry *);
	void (* entry_changed)   (GtkExifContentList *list, ExifEntry *);
	void (* entry_removed)   (GtkExifContentList *list, ExifEntry *);
};

GType      gtk_exif_content_list_get_type (void);
GtkWidget *gtk_exif_content_list_new      (void);

void       gtk_exif_content_list_add_entry    (GtkExifContentList *,
					       ExifEntry *entry);
void       gtk_exif_content_list_update_entry (GtkExifContentList *,
					       ExifEntry *);
void       gtk_exif_content_list_remove_entry (GtkExifContentList *,
					       ExifEntry *);
void       gtk_exif_content_list_set_content  (GtkExifContentList *,
					       ExifContent *);

#endif /* __GTK_EXIF_CONTENT_LIST_H__ */
