/* gtk-exif-browser.c
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
#include "gtk-exif-browser.h"
#include "gtk-exif-content-list.h"
#include "gtk-exif-entry-ascii.h"
#include "gtk-exif-entry-copyright.h"
#include "gtk-exif-entry-date.h"
#include "gtk-exif-entry-flash.h"
#include "gtk-exif-entry-generic.h"
#include "gtk-exif-entry-number.h"
#include "gtk-exif-entry-option.h"
#include "gtk-exif-entry-rational.h"
#include "gtk-exif-entry-resolution.h"
#include "gtk-exif-entry-user-comment.h"
#include "gtk-exif-entry-version.h"
#include "gtk-exif-util.h"

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

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

static void gtk_exif_browser_show_entry (GtkExifBrowser *, ExifEntry *);

struct _GtkExifBrowserPrivate {
	ExifData *data;

#if !GTK_CHECK_VERSION(2,12,0)
	GtkTooltips *tooltips;
#endif

	GtkWidget *empty, *current, *info;

	GtkContainer *thumb_box;
	GtkWidget    *thumb;

	GtkNotebook *notebook;
};

#define PARENT_TYPE gtk_hpaned_get_type()
static GtkHPanedClass *parent_class;

static void
#if GTK_CHECK_VERSION(3,0,0)
gtk_exif_browser_destroy (GtkWidget *widget)
#else
gtk_exif_browser_destroy (GtkObject *object)
#endif
{
#if GTK_CHECK_VERSION(3,0,0)
	GtkExifBrowser *browser = GTK_EXIF_BROWSER (widget);
#else
	GtkExifBrowser *browser = GTK_EXIF_BROWSER (object);
#endif

	if (browser->priv->data) {
		exif_data_unref (browser->priv->data);
		browser->priv->data = NULL;
	}

	if (browser->priv->empty) {
		g_object_unref (browser->priv->empty);
		browser->priv->empty = NULL;
	}

#if !GTK_CHECK_VERSION(2,12,0)
	if (browser->priv->tooltips) {
		g_object_unref (G_OBJECT (browser->priv->tooltips));
		browser->priv->tooltips = NULL;
	}
#endif

#if GTK_CHECK_VERSION(3,0,0)
	GTK_WIDGET_CLASS (parent_class)->destroy (widget);
#else
	GTK_OBJECT_CLASS (parent_class)->destroy (object);
#endif
}

GTK_EXIF_FINALIZE (browser, Browser)

static void
gtk_exif_browser_class_init (gpointer g_class, gpointer class_data)
{
#if GTK_CHECK_VERSION(3,0,0)
	GtkWidgetClass *widget_class;
	GObjectClass *gobject_class;

	widget_class = GTK_WIDGET_CLASS (g_class);
	widget_class->destroy = gtk_exif_browser_destroy;
#else
	GtkObjectClass *object_class;
	GObjectClass *gobject_class;

	object_class = GTK_OBJECT_CLASS (g_class);
	object_class->destroy  = gtk_exif_browser_destroy;
#endif

	gobject_class = G_OBJECT_CLASS (g_class);
	gobject_class->finalize = gtk_exif_browser_finalize;

	parent_class = g_type_class_peek_parent (g_class);
}

static void
gtk_exif_browser_init (GTypeInstance *instance, gpointer g_class)
{
	GtkExifBrowser *browser = GTK_EXIF_BROWSER (instance);

	browser->priv = g_new0 (GtkExifBrowserPrivate, 1);

#if !GTK_CHECK_VERSION(2,12,0)
	browser->priv->tooltips = gtk_tooltips_new ();
	g_object_ref (G_OBJECT (browser->priv->tooltips));
	g_object_ref_sink (GTK_OBJECT (browser->priv->tooltips));
#endif

	/* Placeholder */
	browser->priv->empty = gtk_label_new (_("Nothing selected."));
	gtk_widget_show (browser->priv->empty);
	g_object_ref (G_OBJECT (browser->priv->empty));
}

GTK_EXIF_CLASS (browser, Browser, "Browser")

static GtkExifContentList *
gtk_exif_browser_get_content_list (GtkExifBrowser *b, ExifEntry *entry)
{
	guint n, i;
	GtkWidget *swin, *viewport;
	GtkExifContentList *list = NULL;

	g_return_val_if_fail (GTK_EXIF_IS_BROWSER (b), NULL);
	g_return_val_if_fail (entry != NULL, NULL);

	n = gtk_notebook_get_n_pages (b->priv->notebook);
	for (i = 0; i < n; i++) {
		swin = gtk_notebook_get_nth_page (b->priv->notebook, i);
		if (!GTK_IS_SCROLLED_WINDOW (swin)) continue;
		viewport = gtk_bin_get_child (GTK_BIN (swin));
		list = GTK_EXIF_CONTENT_LIST (gtk_bin_get_child (GTK_BIN (viewport)));
		if (list->content == entry->parent)
			break;
	}
	return (i == n) ? NULL : list;
}

static void
gtk_exif_browser_set_widget (GtkExifBrowser *browser, GtkWidget *w)
{
	if (browser->priv->current)
		gtk_container_remove (GTK_CONTAINER (browser->priv->info),
				      browser->priv->current);
	if (!w) return;

	gtk_box_pack_start (GTK_BOX (browser->priv->info), w, TRUE, FALSE, 0);
	browser->priv->current = w;
}

static void
on_entry_changed (GtkExifEntry *entry, ExifEntry *e, GtkExifBrowser *b)
{
	GtkExifContentList *list;

	g_return_if_fail (GTK_EXIF_IS_BROWSER (b));

	list = gtk_exif_browser_get_content_list (b, e);
	if (!list)
		return;
	gtk_exif_content_list_update_entry (list, e);
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
	GtkExifContentList *list;

	list = gtk_exif_browser_get_content_list (b, e);
	if (!list) return;

	switch (e->tag) {
	case EXIF_TAG_RESOLUTION_UNIT:
	case EXIF_TAG_X_RESOLUTION:
	case EXIF_TAG_Y_RESOLUTION:
		/* Do nothing. */
		break;
	default:
		gtk_exif_browser_set_widget (b, b->priv->empty);
		break;
	}

	gtk_exif_content_list_remove_entry (list, e);
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

	if (!entry) {
		gtk_exif_browser_set_widget (browser, browser->priv->empty);
		return;
	}

	switch (entry->tag) {
	case EXIF_TAG_EXIF_VERSION:
	case EXIF_TAG_FLASH_PIX_VERSION:
		w = gtk_exif_entry_version_new (entry);
		break;
	case EXIF_TAG_USER_COMMENT:
		w = gtk_exif_entry_user_comment_new (entry);
		break;
	case EXIF_TAG_COPYRIGHT:
		w = gtk_exif_entry_copyright_new (entry);
		break;
	case EXIF_TAG_FLASH:
		w = gtk_exif_entry_flash_new (entry);
		break;
	case EXIF_TAG_EXPOSURE_PROGRAM:
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
	g_signal_connect (G_OBJECT (w), "entry_added",
			    G_CALLBACK (on_entry_added), browser);
	g_signal_connect (G_OBJECT (w), "entry_removed",
			    G_CALLBACK (on_entry_removed), browser);
	g_signal_connect (G_OBJECT (w), "entry_changed",
			    G_CALLBACK (on_entry_changed), browser);
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
        g_signal_connect (G_OBJECT (et), "entry_selected",
                            G_CALLBACK (on_entry_selected), browser);
}

GtkWidget *
gtk_exif_browser_new (void)
{
	GtkWidget *vbox, *notebook;
	GtkExifBrowser *browser;

	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");

	browser = g_object_new (GTK_EXIF_TYPE_BROWSER, NULL);
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
		GdkPixbufLoader *loader;
		GtkWidget *image;

		loader = gdk_pixbuf_loader_new ();
		if (!gdk_pixbuf_loader_write (loader,
			b->priv->data->data, b->priv->data->size, NULL)) {
			b->priv->thumb = gtk_label_new (_("Could not parse "
							"thumbnail data."));
		} else {
			gdk_pixbuf_loader_close (loader, NULL);
			image = gtk_image_new_from_pixbuf (
					gdk_pixbuf_loader_get_pixbuf (loader));
			gtk_widget_show (image);
			b->priv->thumb = gtk_scrolled_window_new (NULL, NULL);
			gtk_scrolled_window_set_policy (
				GTK_SCROLLED_WINDOW (b->priv->thumb),
				GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
			gtk_scrolled_window_add_with_viewport (
				GTK_SCROLLED_WINDOW (b->priv->thumb), image);
		}
		g_object_unref (G_OBJECT (loader));
		tip = g_strdup_printf (_("Size: %i byte(s)."),
				       b->priv->data->size);
#if !GTK_CHECK_VERSION(2,12,0)
		gtk_tooltips_set_tip (b->priv->tooltips, b->priv->thumb, tip, NULL);
#else
		gtk_widget_set_tooltip_text (b->priv->thumb, tip);
#endif
		g_free (tip);
	}
	gtk_widget_show (b->priv->thumb);
	gtk_box_pack_start (GTK_BOX (b->priv->thumb_box), b->priv->thumb,
			    TRUE, TRUE, 0);
}

static void
on_load_ok_clicked (GtkWidget *fchoser, GtkExifBrowser *b)
{
	gchar *filename;
	FILE *f;
	unsigned int size, read;

	g_return_if_fail (GTK_EXIF_IS_BROWSER (b));

	filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (fchoser));
	f = fopen (filename, "rb");
	if (!f) {
		g_warning ("Can not open file '%s'.", filename);
		return;
	}
	g_free(filename);
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
	gtk_exif_browser_show_thumbnail (b);
}

static void
on_load_clicked (GtkButton *button, GtkExifBrowser *b)
{
	GtkWidget *fchoser;
	GtkWidget *fchoser_parent;

	fchoser_parent = gtk_widget_get_ancestor (GTK_WIDGET (b), GTK_TYPE_WINDOW);
	fchoser = gtk_file_chooser_dialog_new (_("Load..."),
					GTK_WINDOW(fchoser_parent),
					GTK_FILE_CHOOSER_ACTION_OPEN,
					GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
					NULL);

	if (gtk_dialog_run (GTK_DIALOG (fchoser)) == GTK_RESPONSE_ACCEPT)
		on_load_ok_clicked (fchoser, b);

	gtk_widget_destroy (fchoser);
}

static void
on_save_ok_clicked (GtkWidget *fchoser, GtkExifBrowser *b)
{
	gchar *filename;
	FILE *f;

	g_return_if_fail (GTK_EXIF_IS_BROWSER (b));

	filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (fchoser));
	f = fopen (filename, "wb");
	if (!f) {
		g_warning ("Could not open '%s'.", filename);
		return;
	}
	g_free(filename);
	fwrite (b->priv->data->data, 1, b->priv->data->size, f);
	fclose (f);
}

static void
on_save_clicked (GtkButton *button, GtkExifBrowser *b)
{
	GtkWidget *fchoser;
	GtkWidget *fchoser_parent;

	fchoser_parent = gtk_widget_get_ancestor (GTK_WIDGET (b), GTK_TYPE_WINDOW);
	fchoser = gtk_file_chooser_dialog_new (_("Save As..."),
					GTK_WINDOW(fchoser_parent),
					GTK_FILE_CHOOSER_ACTION_SAVE,
					GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
					NULL);

	switch (gtk_dialog_run (GTK_DIALOG (fchoser)) == GTK_RESPONSE_ACCEPT)
		on_save_ok_clicked (fchoser, b);

	gtk_widget_destroy (fchoser);
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
	guint i;

	g_return_if_fail (GTK_EXIF_IS_BROWSER (b));
	g_return_if_fail (data != NULL);

	if (b->priv->data)
		exif_data_unref (b->priv->data);
	b->priv->data = data;
	exif_data_ref (data);

	/* sequence of tabs removal is important to avoid segfault */
	gtk_notebook_set_current_page (b->priv->notebook, -1);
	n = gtk_notebook_get_n_pages (b->priv->notebook);
	for (i = 0; i < n; i++)
		gtk_notebook_remove_page (b->priv->notebook, 0);

	/* thumbnail has disappeared with the removal of the related page */
	b->priv->thumb_box = NULL;
	b->priv->thumb     = NULL;

	for (i = 0; i < EXIF_IFD_COUNT; i++)
		gtk_exif_browser_add_content (b, exif_ifd_get_name (i),
					      data->ifd[i]);

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
	gtk_box_set_spacing (GTK_BOX (bbox), 5);
	gtk_box_pack_end (GTK_BOX (vbox), bbox, FALSE, FALSE, 0);
	button = gtk_button_new_with_label (_("Load"));
	gtk_widget_show (button);
	gtk_container_add (GTK_CONTAINER (bbox), button);
	g_signal_connect (G_OBJECT (button), "clicked",
			    G_CALLBACK (on_load_clicked), b);
	button = gtk_button_new_with_label (_("Save"));
	gtk_widget_show (button);
	gtk_container_add (GTK_CONTAINER (bbox), button);
	g_signal_connect (G_OBJECT (button), "clicked",
			    G_CALLBACK (on_save_clicked), b);
	button = gtk_button_new_with_label (_("Delete"));
	gtk_widget_show (button);
	gtk_container_add (GTK_CONTAINER (bbox), button);
	g_signal_connect (G_OBJECT (button), "clicked",
			    G_CALLBACK (on_delete_clicked), b);

	/* Show the current thumbnail */
	gtk_exif_browser_show_thumbnail (b);

	gtk_widget_set_sensitive (GTK_WIDGET (b), TRUE);
}
