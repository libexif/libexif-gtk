/* gtk-exif-entry.h
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

#ifndef __GTK_EXIF_ENTRY_H__
#define __GTK_EXIF_ENTRY_H__

#include <gtk/gtk.h>
#include <libexif/exif-entry.h>

#define GTK_EXIF_TYPE_ENTRY     (gtk_exif_entry_get_type())
#define GTK_EXIF_ENTRY(o)       (G_TYPE_CHECK_INSTANCE_CAST((o),GTK_EXIF_TYPE_ENTRY,GtkExifEntry))
#define GTK_EXIF_ENTRY_CLASS(k) (G_TYPE_CHECK_CLASS_CAST((k),GTK_EXIF_TYPE_ENTRY,GtkExifEntryClass))
#define GTK_EXIF_IS_ENTRY(o)    (G_TYPE_CHECK_INSTANCE_TYPE((o),GTK_EXIF_TYPE_ENTRY))

typedef struct _GtkExifEntry        GtkExifEntry;
typedef struct _GtkExifEntryPrivate GtkExifEntryPrivate;
typedef struct _GtkExifEntryClass   GtkExifEntryClass;

struct _GtkExifEntry
{
	GtkVBox parent;

	GtkExifEntryPrivate *priv;
};

struct _GtkExifEntryClass
{
	GtkVBoxClass parent_class;

	/* Signals */
	void (* entry_changed) (GtkExifEntry *entry, ExifEntry *e);
	void (* entry_removed) (GtkExifEntry *entry, ExifEntry *e);
	void (* entry_added)   (GtkExifEntry *entry, ExifEntry *e);
};

GType  gtk_exif_entry_get_type  (void);
void   gtk_exif_entry_construct (GtkExifEntry *, const gchar *name,
				 const gchar *description);

/* Signals */
void   gtk_exif_entry_changed   (GtkExifEntry *, ExifEntry *);
void   gtk_exif_entry_added     (GtkExifEntry *, ExifEntry *);
void   gtk_exif_entry_removed   (GtkExifEntry *, ExifEntry *);

#endif /* __GTK_EXIF_ENTRY_H__ */
