/* gtk-exif-browser.c
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
#include "gtk-exif-browser.h"

#include <stdio.h>

#include <gtk/gtksignal.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkhbox.h>
#include <gtk/gtkvbox.h>
#include <gtk/gtkbutton.h>
#include <gtk/gtknotebook.h>
#include <gtk/gtkscrolledwindow.h>
#include <gtk/gtkpixmap.h>
#include <gtk/gtktooltips.h>
#include <gtk/gtkhbbox.h>
#include <gtk/gtkfilesel.h>
#include <gtk/gtkscrolledwindow.h>

#ifdef HAVE_GDK_PIXBUF
#  include <gdk-pixbuf/gdk-pixbuf-loader.h>
#endif

#include "gtk-exif-content-list.h"
#include "gtk-exif-entry-ascii.h"
#include "gtk-exif-entry-copyright.h"
#include "gtk-exif-entry-date.h"
#include "gtk-exif-entry-exposure.h"
#include "gtk-exif-entry-flash.h"
#include "gtk-exif-entry-generic.h"
#include "gtk-exif-entry-number.h"
#include "gtk-exif-entry-option.h"
#include "gtk-exif-entry-rational.h"
#include "gtk-exif-entry-resolution.h"
#include "gtk-exif-entry-version.h"

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

static void gtk_exif_browser_show_entry (GtkExifBrowser *, ExifEntry *);

struct _GtkExifBrowserPrivate {
	ExifData *data;

	GtkTooltips *tooltips;

	GtkWidget *empty, *current, *info;

	GtkContainer *thumb_box;
	GtkWidget    *thumb;

	GtkNotebook *notebook;
};

#define PARENT_TYPE gtk_hpaned_get_type()
static GtkHPanedClass *parent_class;

static void
gtk_exif_browser_destroy (GtkObject *object)
{
	GtkExifBrowser *browser = GTK_EXIF_BROWSER (object);

	if (browser->priv->data) {
		exif_data_unref (browser->priv->data);
		browser->priv->data = NULL;
	}

	if (browser->priv->empty) {
		gtk_widget_unref (browser->priv->empty);
		browser->priv->empty = NULL;
	}

	if (browser->priv->tooltips) {
		gtk_object_unref (GTK_OBJECT (browser->priv->tooltips));
		browser->priv->tooltips = NULL;
	}

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gtk_exif_browser_finalize (GtkObject *object)
{
	GtkExifBrowser *browser = GTK_EXIF_BROWSER (object);

	g_free (browser->priv);

	GTK_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_exif_browser_class_init (GtkExifBrowserClass *klass)
{
	GtkObjectClass *object_class;

	object_class = GTK_OBJECT_CLASS (klass);
	object_class->destroy  = gtk_exif_browser_destroy;
	object_class->finalize = gtk_exif_browser_finalize;

	parent_class = gtk_type_class (PARENT_TYPE);
}

static void
gtk_exif_browser_init (GtkExifBrowser *browser)
{
	browser->priv = g_new0 (GtkExifBrowserPrivate, 1);

	browser->priv->tooltips = gtk_tooltips_new ();

	/* Placeholder */
	browser->priv->empty = gtk_label_new ("Nothing selected.");
	gtk_widget_show (browser->priv->empty);
	gtk_object_ref (GTK_OBJECT (browser->priv->empty));
}

GtkType
gtk_exif_browser_get_type (void)
{
	static GtkType browser_type = 0;

	if (!browser_type) {
		static const GtkTypeInfo browser_info = {
			"GtkExifBrowser",
			sizeof (GtkExifBrowser),
			sizeof (GtkExifBrowserClass),
			(GtkClassInitFunc)  gtk_exif_browser_class_init,
			(GtkObjectInitFunc) gtk_exif_browser_init,
			NULL, NULL, NULL};
		browser_type = gtk_type_unique (PARENT_TYPE, &browser_info);
	}

	return (browser_type);
}

static GtkExifContentList *
gtk_exif_browser_get_content_list (GtkExifBrowser *b, ExifEntry *entry)
{
	guint n, i;
	GtkWidget *swin, *viewport;
	GtkExifContentList *list = NULL;

	n = g_list_length (b->priv->notebook->children);
	for (i = 0; i < n; i++) {
		swin = gtk_notebook_get_nth_page (b->priv->notebook, i);
		viewport = GTK_BIN (swin)->child;
		list = GTK_EXIF_CONTENT_LIST (GTK_BIN (viewport)->child);
		if (list->content == entry->parent)
			break;
	}
	if (i == n)
		return NULL;

	return (list);
}

static void
gtk_exif_browser_set_widget (GtkExifBrowser *browser, GtkWidget *w)
{
	if (browser->priv->current)
		gtk_container_remove (GTK_CONTAINER (browser->priv->info),
				      browser->priv->current);
	gtk_box_pack_start (GTK_BOX (browser->priv->info), w, TRUE, FALSE, 0);
	browser->priv->current = w;
}

static void
on_entry_changed (GtkExifEntry *entry, ExifEntry *e, GtkExifBrowser *b)
{
        GtkExifContentList *list;
	gint row;

	list = gtk_exif_browser_get_content_list (b, e);
	if (!list)
		return;

        row = gtk_clist_find_row_from_data (GTK_CLIST (list), e);
        gtk_clist_set_text (GTK_CLIST (list), row, 1,
			    exif_entry_get_value (e));
}

static void
on_entry_added (GtkExifEntry *entry, ExifEntry *e, GtkExifBrowser *b)
{
        GtkExifContentList *list;

	list = gtk_exif_browser_get_content_list (b, e);
	if (!list)
		return;

        gtk_exif_content_list_add_entry (list, e);
	gtk_exif_browser_show_entry (b, e);
}

static void
on_entry_removed (GtkExifEntry *entry, ExifEntry *e, GtkExifBrowser *b)
{
}

static void
on_entry_selected (GtkExifContentList *list, ExifEntry *entry,
		   GtkExifBrowser *browser)
{
	gtk_exif_browser_show_entry (browser, entry);
}

static void
gtk_exif_browser_show_entry (GtkExifBrowser *browser, ExifEntry *entry)
{
	GtkWidget *w;

	switch (entry->tag) {
	case EXIF_TAG_EXIF_VERSION:
	case EXIF_TAG_FLASH_PIX_VERSION:
		w = gtk_exif_entry_version_new (entry);
		break;
	case EXIF_TAG_COPYRIGHT:
		w = gtk_exif_entry_copyright_new (entry);
		break;
	case EXIF_TAG_FLASH:
		w = gtk_exif_entry_flash_new (entry);
		break;
	case EXIF_TAG_EXPOSURE_PROGRAM:
		w = gtk_exif_entry_exposure_new (entry);
		break;
	case EXIF_TAG_SENSING_METHOD:
	case EXIF_TAG_ORIENTATION:
	case EXIF_TAG_METERING_MODE:
	case EXIF_TAG_YCBCR_POSITIONING:
	case EXIF_TAG_COMPRESSION:
	case EXIF_TAG_LIGHT_SOURCE:
		w = gtk_exif_entry_option_new (entry);
		break;
	case EXIF_TAG_RESOLUTION_UNIT:
	case EXIF_TAG_X_RESOLUTION:
	case EXIF_TAG_Y_RESOLUTION:
		w = gtk_exif_entry_resolution_new (entry->parent, FALSE);
		break;
	case EXIF_TAG_FOCAL_PLANE_X_RESOLUTION:
	case EXIF_TAG_FOCAL_PLANE_Y_RESOLUTION:
	case EXIF_TAG_FOCAL_PLANE_RESOLUTION_UNIT:
		w = gtk_exif_entry_resolution_new (entry->parent, TRUE);
		break;
	case EXIF_TAG_MAKE:
	case EXIF_TAG_MODEL:
	case EXIF_TAG_IMAGE_DESCRIPTION:
	case EXIF_TAG_SOFTWARE:
	case EXIF_TAG_ARTIST:
		w = gtk_exif_entry_ascii_new (entry);
		break;
	case EXIF_TAG_DATE_TIME:
	case EXIF_TAG_DATE_TIME_ORIGINAL:
	case EXIF_TAG_DATE_TIME_DIGITIZED:
		w = gtk_exif_entry_date_new (entry);
		break;
	default:
		switch (entry->format) {
		case EXIF_FORMAT_RATIONAL:
		case EXIF_FORMAT_SRATIONAL:
			w = gtk_exif_entry_rational_new (entry);
			break;
		case EXIF_FORMAT_BYTE:
		case EXIF_FORMAT_SHORT:
		case EXIF_FORMAT_LONG:
		case EXIF_FORMAT_SLONG:
			w = gtk_exif_entry_number_new (entry);
			break;
		default:
			w = gtk_exif_entry_generic_new (entry);
			break;
		}
		break;
	}
	gtk_widget_show (w);
	gtk_exif_browser_set_widget (browser, w);
	gtk_signal_connect (GTK_OBJECT (w), "entry_added",
			    GTK_SIGNAL_FUNC (on_entry_added), browser);
	gtk_signal_connect (GTK_OBJECT (w), "entry_removed",
			    GTK_SIGNAL_FUNC (on_entry_removed), browser);
	gtk_signal_connect (GTK_OBJECT (w), "entry_changed",
			    GTK_SIGNAL_FUNC (on_entry_changed), browser);
}

static void
gtk_exif_browser_add_content (GtkExifBrowser *browser,
			      const gchar *name, ExifContent *content)
{
	GtkWidget *swin, *label, *et;

	label = gtk_label_new (name);
	gtk_widget_show (label);

	swin = gtk_scrolled_window_new (NULL, NULL);
	gtk_container_set_border_width (GTK_CONTAINER (swin), 5);
	gtk_widget_show (swin);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (swin),
				GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	gtk_notebook_append_page (browser->priv->notebook, swin, label);

        /* List */
        et = gtk_exif_content_list_new ();
        gtk_widget_show (et);
	gtk_exif_content_list_set_content (GTK_EXIF_CONTENT_LIST (et), content);
        gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (swin), et);
        gtk_signal_connect (GTK_OBJECT (et), "entry_selected",
                            GTK_SIGNAL_FUNC (on_entry_selected), browser);
}

GtkWidget *
gtk_exif_browser_new (void)
{
	GtkWidget *vbox, *notebook;
	GtkExifBrowser *browser;

	browser = gtk_type_new (GTK_EXIF_TYPE_BROWSER);
	gtk_widget_set_sensitive (GTK_WIDGET (browser), FALSE);

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
	gtk_paned_pack1 (GTK_PANED (browser), vbox, TRUE, TRUE);

	/* Notebook */
	notebook = gtk_notebook_new ();
	gtk_widget_show (notebook);
	gtk_box_pack_start (GTK_BOX (vbox), notebook, TRUE, TRUE, 0);
	browser->priv->notebook = GTK_NOTEBOOK (notebook);

	/* Info */
	browser->priv->info = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (browser->priv->info);
	gtk_paned_pack2 (GTK_PANED (browser), browser->priv->info, TRUE, FALSE);

	/* Set placeholder */
	gtk_exif_browser_set_widget (browser, browser->priv->empty);

	return (GTK_WIDGET (browser));
}

static void
gtk_exif_browser_show_thumbnail (GtkExifBrowser *b)
{
	gchar *tip;

	g_return_if_fail (GTK_EXIF_IS_BROWSER (b));

	if (b->priv->thumb) {
		gtk_container_remove (b->priv->thumb_box, b->priv->thumb);
		b->priv->thumb = NULL;
	}

	if (!b->priv->data->data) {
		b->priv->thumb = gtk_label_new (_("No thumbnail available."));
	} else {
#ifdef HAVE_GDK_PIXBUF
		GdkPixbufLoader *loader;
		GdkPixmap *pixmap;
		GdkBitmap *bitmap;
		GtkWidget *image;

		loader = gdk_pixbuf_loader_new ();
		if (!gdk_pixbuf_loader_write (loader,
				b->priv->data->data, b->priv->data->size)) {
			b->priv->thumb = gtk_label_new (_("Could not parse "
							"thumbnail data."));
		} else {
			gdk_pixbuf_loader_close (loader);
			gdk_pixbuf_render_pixmap_and_mask (
				gdk_pixbuf_loader_get_pixbuf (loader),
				&pixmap, &bitmap, 127);
			image = gtk_pixmap_new (pixmap, bitmap);
			gtk_widget_show (image);
			if (pixmap)
				gdk_pixmap_unref (pixmap);
			if (bitmap)
				gdk_bitmap_unref (bitmap);
			b->priv->thumb = gtk_scrolled_window_new (NULL, NULL);
			gtk_scrolled_window_set_policy (
				GTK_SCROLLED_WINDOW (b->priv->thumb),
				GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
			gtk_scrolled_window_add_with_viewport (
				GTK_SCROLLED_WINDOW (b->priv->thumb), image);
		}
		gtk_object_unref (GTK_OBJECT (loader));
#else
		b->priv->thumb = gtk_label_new (_("Compiled without "
					"support for gdk-pixbuf. Can not "
					"display thumbnail."));
#endif
		tip = g_strdup_printf (_("Size: %i byte(s)."),
				       b->priv->data->size);
		gtk_tooltips_set_tip (b->priv->tooltips, b->priv->thumb,
				      tip, NULL);
		g_free (tip);
	}
	gtk_widget_show (b->priv->thumb);
	gtk_box_pack_start (GTK_BOX (b->priv->thumb_box), b->priv->thumb,
			    TRUE, TRUE, 0);
}

static void
on_cancel_clicked (GtkButton *button, GtkFileSelection *fsel)
{
	gtk_object_destroy (GTK_OBJECT (fsel));
}

static void
on_load_ok_clicked (GtkButton *button, GtkExifBrowser *b)
{
	GtkWidget *fsel;
	const gchar *path;
	FILE *f;
	unsigned int size, read;

	g_return_if_fail (GTK_EXIF_IS_BROWSER (b));

	fsel = gtk_widget_get_ancestor (GTK_WIDGET (button),
					GTK_TYPE_FILE_SELECTION);
	path = gtk_file_selection_get_filename (GTK_FILE_SELECTION (fsel));
	f = fopen (path, "rb");
	if (!f) {
		g_warning ("Can not open file '%s'.", path);
		return;
	}
	fseek (f, 0, SEEK_END);
	size = ftell (f);
	rewind (f);
	if (b->priv->data->data) {
		g_free (b->priv->data->data);
		b->priv->data->data = NULL;
		b->priv->data->size = 0;
	}
	if (size) {
		b->priv->data->data = g_new0 (char, size);
		if (!b->priv->data->data) {
			g_warning ("Could not allocate %i bytes!", size);
			fclose (f);
			return;
		}
		b->priv->data->size = size;
		read = fread (b->priv->data->data, 1, size, f);
		if ((read != size) || ferror (f)) {
			g_warning ("Could not read %i bytes!", size);
			fclose (f);
			return;
		}
	}
	fclose (f);
	gtk_object_destroy (GTK_OBJECT (fsel));

	gtk_exif_browser_show_thumbnail (b);
}

static void
on_load_clicked (GtkButton *button, GtkExifBrowser *b)
{
	GtkWidget *fsel;

	fsel = gtk_file_selection_new (_("Load..."));
	gtk_widget_show (fsel);
	gtk_signal_connect (GTK_OBJECT (fsel), "delete_event",
			    GTK_SIGNAL_FUNC (gtk_object_destroy), NULL);
	gtk_signal_connect (
		GTK_OBJECT (GTK_FILE_SELECTION (fsel)->cancel_button),
		"clicked", GTK_SIGNAL_FUNC (on_cancel_clicked), fsel);
	gtk_signal_connect (
		GTK_OBJECT (GTK_FILE_SELECTION (fsel)->ok_button),
		"clicked", GTK_SIGNAL_FUNC (on_load_ok_clicked), b);
}

static void
on_save_ok_clicked (GtkButton *button, GtkExifBrowser *b)
{
	GtkWidget *fsel;
	const gchar *path;
	FILE *f;

	g_return_if_fail (GTK_EXIF_IS_BROWSER (b));

	fsel = gtk_widget_get_ancestor (GTK_WIDGET (button),
					GTK_TYPE_FILE_SELECTION);
	path = gtk_file_selection_get_filename (GTK_FILE_SELECTION (fsel));

	f = fopen (path, "wb");
	if (!f) {
		g_warning ("Could not open '%s'.", path);
		return;
	}
	fwrite (b->priv->data->data, 1, b->priv->data->size, f);
	fclose (f);
	gtk_object_destroy (GTK_OBJECT (fsel));
}

static void
on_save_clicked (GtkButton *button, GtkExifBrowser *b)
{
	GtkWidget *fsel;

	fsel = gtk_file_selection_new (_("Save As..."));
	gtk_widget_show (fsel);
	gtk_signal_connect (GTK_OBJECT (fsel), "delete_event",
			    GTK_SIGNAL_FUNC (gtk_object_destroy), NULL);
	gtk_signal_connect (
		GTK_OBJECT (GTK_FILE_SELECTION (fsel)->cancel_button),
		"clicked", GTK_SIGNAL_FUNC (on_cancel_clicked), fsel);
	gtk_signal_connect (
		GTK_OBJECT (GTK_FILE_SELECTION (fsel)->ok_button),
		"clicked", GTK_SIGNAL_FUNC (on_save_ok_clicked), b);
}

static void
on_delete_clicked (GtkButton *button, GtkExifBrowser *b)
{
	g_return_if_fail (GTK_EXIF_IS_BROWSER (b));

	if (b->priv->data->data) {
		g_free (b->priv->data->data);
		b->priv->data->data = NULL;
	}
	b->priv->data->size = 0;
	gtk_exif_browser_show_thumbnail (b);
}

void
gtk_exif_browser_set_data (GtkExifBrowser *b, ExifData *data)
{
	GtkWidget *label, *vbox, *bbox, *button, *hbox;
	gint n;

	g_return_if_fail (GTK_EXIF_IS_BROWSER (b));
	g_return_if_fail (data != NULL);

	if (b->priv->data)
		exif_data_unref (b->priv->data);
	b->priv->data = data;
	exif_data_ref (data);

	while ((n = gtk_notebook_get_current_page (b->priv->notebook)) >= 0)
		gtk_notebook_remove_page (b->priv->notebook, n);

	gtk_exif_browser_add_content (b, _("IFD 0"), data->ifd0);
	gtk_exif_browser_add_content (b, _("IFD 1"), data->ifd1);
	gtk_exif_browser_add_content (b, _("Exif IFD"), data->ifd_exif);
	gtk_exif_browser_add_content (b, _("GPS IFD"), data->ifd_gps);
	gtk_exif_browser_add_content (b, _("Interoperability IFD"),
				      data->ifd_interoperability);

	/* Create the thumbnail page */
	vbox = gtk_vbox_new (FALSE, 5);
	gtk_widget_show (vbox);
	label = gtk_label_new (_("Thumbnail"));
	gtk_widget_show (label);
	gtk_notebook_append_page (b->priv->notebook, vbox, label);

	/* Thumbnail */
	hbox = gtk_hbox_new (FALSE, 5);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);
	b->priv->thumb_box = GTK_CONTAINER (hbox);

	/* Buttons */
	bbox = gtk_hbutton_box_new ();
	gtk_widget_show (bbox);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (bbox), GTK_BUTTONBOX_SPREAD);
	gtk_container_set_border_width (GTK_CONTAINER (bbox), 5);
	gtk_button_box_set_spacing (GTK_BUTTON_BOX (bbox), 5);
	gtk_box_pack_end (GTK_BOX (vbox), bbox, FALSE, FALSE, 0);
	button = gtk_button_new_with_label (_("Load"));
	gtk_widget_show (button);
	gtk_container_add (GTK_CONTAINER (bbox), button);
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
			    GTK_SIGNAL_FUNC (on_load_clicked), b);
	button = gtk_button_new_with_label (_("Save"));
	gtk_widget_show (button);
	gtk_container_add (GTK_CONTAINER (bbox), button);
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
			    GTK_SIGNAL_FUNC (on_save_clicked), b);
	button = gtk_button_new_with_label (_("Delete"));
	gtk_widget_show (button);
	gtk_container_add (GTK_CONTAINER (bbox), button);
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
			    GTK_SIGNAL_FUNC (on_delete_clicked), b);

	/* Show the current thumbnail */
	gtk_exif_browser_show_thumbnail (b);

	gtk_widget_set_sensitive (GTK_WIDGET (b), TRUE);
}
