/* gtk-exif-entry-resolution.h
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

#ifndef __GTK_EXIF_ENTRY_RESOLUTION_H__
#define __GTK_EXIF_ENTRY_RESOLUTION_H__

#include <libexif-gtk/gtk-exif-entry.h>

#define GTK_EXIF_TYPE_ENTRY_RESOLUTION     (gtk_exif_entry_resolution_get_type())
#define GTK_EXIF_ENTRY_RESOLUTION(o)       (G_TYPE_CHECK_INSTANCE_CAST((o),GTK_EXIF_TYPE_ENTRY_RESOLUTION,GtkExifEntryResolution))
#define GTK_EXIF_ENTRY_RESOLUTION_CLASS(k) (G_TYPE_CHECK_CLASS_CAST((k),GTK_EXIF_TYPE_ENTRY_RESOLUTION,GtkExifEntryResolutionClass))
#define GTK_EXIF_IS_ENTRY_RESOLUTION(o)    (G_TYPE_CHECK_INSTANCE_TYPE((o),GTK_EXIF_TYPE_ENTRY_RESOLUTION))

typedef struct _GtkExifEntryResolution        GtkExifEntryResolution;
typedef struct _GtkExifEntryResolutionPrivate GtkExifEntryResolutionPrivate;
typedef struct _GtkExifEntryResolutionClass   GtkExifEntryResolutionClass;

struct _GtkExifEntryResolution
{
	GtkExifEntry parent;

	GtkExifEntryResolutionPrivate *priv;
};

struct _GtkExifEntryResolutionClass
{
	GtkExifEntryClass parent_class;
};

GType      gtk_exif_entry_resolution_get_type (void);
GtkWidget *gtk_exif_entry_resolution_new      (ExifContent *content,
					       gboolean focal_plane);

#endif /* __GTK_EXIF_ENTRY_RESOLUTION_H__ */
