/* gtk-exif-util.h:
 *
 * Copyright (C) 2002 Lutz Müller <lutz@users.sourceforge.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
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

#ifndef __GTK_EXIF_UTIL_H__
#define __GTK_EXIF_UTIL_H__

#define GTK_EXIF_CLASS(n,N,s)						\
									\
GType									\
gtk_exif_##n##_get_type (void)						\
{									\
	static GType t = 0;						\
	GTypeInfo ti;							\
									\
	if (!t) {							\
		memset (&ti, 0, sizeof (GTypeInfo));			\
		ti.class_size    = sizeof (GtkExif##N##Class);		\
		ti.class_init    = gtk_exif_##n##_class_init;		\
		ti.instance_size = sizeof (GtkExif##N);			\
		ti.instance_init = gtk_exif_##n##_init;			\
		t = g_type_register_static (PARENT_TYPE, "GtkExif" s,	\
					    &ti, 0);			\
	}								\
									\
	return (t);							\
}

#define GTK_EXIF_FINALIZE(n,N)						\
									\
static void								\
gtk_exif_##n##_finalize (GObject *object)				\
{									\
	GtkExif##N *o = (GtkExif##N *) object;				\
									\
	g_free (o->priv);						\
									\
	G_OBJECT_CLASS (parent_class)->finalize (object);		\
}

#endif /* __GTK_EXIF_UTIL_H__ */
