/* gtk-exif-entry-option.c
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
#include "gtk-exif-entry-option.h"
#include "gtk-exif-util.h"
#include "gtk-extensions/gtk-options.h"

#include <string.h>
#include <gtk/gtk.h>
#include <libexif/exif-utils.h>

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

struct _GtkExifEntryOptionPrivate {
	ExifEntry *entry;

	GtkComboBox *menu;
};

#define PARENT_TYPE GTK_EXIF_TYPE_ENTRY
static GtkExifEntryClass *parent_class;

static void
#if GTK_CHECK_VERSION(3,0,0)
gtk_exif_entry_option_destroy (GtkWidget *widget)
#else
gtk_exif_entry_option_destroy (GtkObject *object)
#endif
{
#if GTK_CHECK_VERSION(3,0,0)
	GtkExifEntryOption *entry = GTK_EXIF_ENTRY_OPTION (widget);
#else
	GtkExifEntryOption *entry = GTK_EXIF_ENTRY_OPTION (object);
#endif

	if (entry->priv->entry) {
		exif_entry_unref (entry->priv->entry);
		entry->priv->entry = NULL;
	}

#if GTK_CHECK_VERSION(3,0,0)
	GTK_WIDGET_CLASS (parent_class)->destroy (widget);
#else
	GTK_OBJECT_CLASS (parent_class)->destroy (object);
#endif
}

GTK_EXIF_FINALIZE (entry_option, EntryOption)

static void
gtk_exif_entry_option_class_init (gpointer g_class, gpointer class_data)
{
#if GTK_CHECK_VERSION(3,0,0)
	GtkWidgetClass *widget_class;
	GObjectClass *gobject_class;

	widget_class = GTK_WIDGET_CLASS (g_class);
	widget_class->destroy = gtk_exif_entry_option_destroy;
#else
	GtkObjectClass *object_class;
	GObjectClass *gobject_class;

	object_class = GTK_OBJECT_CLASS (g_class);
	object_class->destroy  = gtk_exif_entry_option_destroy;
#endif

	gobject_class = G_OBJECT_CLASS (g_class);
	gobject_class->finalize = gtk_exif_entry_option_finalize;

	parent_class = g_type_class_peek_parent (g_class);
}

static void
gtk_exif_entry_option_init (GTypeInstance *instance, gpointer g_class)
{
	GtkExifEntryOption *entry = GTK_EXIF_ENTRY_OPTION (instance);

	entry->priv = g_new0 (GtkExifEntryOptionPrivate, 1);
}

GTK_EXIF_CLASS (entry_option, EntryOption, "EntryOption")

static void
gtk_exif_entry_option_load (GtkExifEntryOption *entry)
{
	ExifShort value;
	ExifByteOrder o;
	GtkTreeIter iter;
	GtkTreeModel *tm;

	g_return_if_fail (GTK_EXIF_IS_ENTRY_OPTION (entry));

	o = exif_data_get_byte_order (entry->priv->entry->parent->parent);
	value = exif_get_short (entry->priv->entry->data, o);
	tm = gtk_combo_box_get_model (entry->priv->menu);
	if (gtk_tree_model_get_iter_from_option (tm, value, &iter))
		gtk_combo_box_set_active_iter (entry->priv->menu, &iter);
}

static void
on_changed (GtkComboBox *cb, GtkExifEntryOption *e)
{
	GValue v = {0,};
	ExifByteOrder o;
	GtkTreeModel *tm = gtk_combo_box_get_model (cb);
	GtkTreeIter iter;

	gtk_combo_box_get_active_iter (cb, &iter);
	gtk_tree_model_get_value (tm, &iter, GTK_OPTIONS_OPTION_COLUMN, &v);
	o = exif_data_get_byte_order (e->priv->entry->parent->parent);
	exif_set_short (e->priv->entry->data, o, g_value_get_int (&v));
	gtk_exif_entry_changed (GTK_EXIF_ENTRY (e), e->priv->entry);
}

static GtkOptions options_sensing_method[] = {
	{  1, N_("Not defined")},
	{  2, N_("One-chip color area sensor")},
	{  3, N_("Two-chip color area sensor")},
	{  4, N_("Three-chip color area sensor")},
	{  5, N_("Color sequential area sensor")},
	{  7, N_("Trilinear sensor")},
	{  8, N_("Color sequential linear sensor")},
	{  0, NULL}
};

static GtkOptions options_compression[] = {
	{1, N_("Uncompressed")},
	{6, N_("JPEG compression")},
	{0, NULL}
};

static GtkOptions options_metering_mode[] = {
        {  0, N_("Unknown")},
        {  1, N_("Average")},
        {  2, N_("Center-Weighted Average")},
        {  3, N_("Spot")},
        {  4, N_("Multi Spot")},
        {  5, N_("Pattern")},
        {  6, N_("Partial")},
        {255, N_("Other")},
        {  0, NULL}
};

static GtkOptions options_light_source[] = {
        {  0, N_("Unknown")},
        {  1, N_("Daylight")},
        {  2, N_("Fluorescent")},
        {  3, N_("Tungsten")},
        {  4, N_("Flash")},
        {  9, N_("Fine weather")},
        { 10, N_("Cloudy weather")},
        { 11, N_("Shade")},
        { 12, N_("Daylight fluorescent")},
        { 13, N_("Day white fluorescent")},
        { 14, N_("Cool white fluorescent")},
        { 15, N_("White fluorescent")},
        { 17, N_("Standard light A")},
        { 18, N_("Standard light B")},
        { 19, N_("Standard light C")},
        { 20, N_("D55")},
        { 21, N_("D65")},
        { 22, N_("D75")},
        { 24, N_("ISO studio tungsten")},
        {255, N_("Other")},
        {  0, NULL}
};

static GtkOptions options_orientation[] = {
        {1, N_("top - left")},
        {2, N_("top - right")},
        {3, N_("bottom - right")},
        {4, N_("bottom - left")},
        {5, N_("left - top")},
        {6, N_("right - top")},
        {7, N_("right - bottom")},
        {8, N_("left - bottom")},
        {0, NULL}
};

static GtkOptions options_ycbcr_positioning[] = {
        {  1, N_("centered")},
        {  2, N_("co-sited")},
        {  0, NULL}
};

static GtkOptions options_exposure_program[] = {
	{0, N_("Not defined")},
	{1, N_("Manual")},
	{2, N_("Normal program")},
	{3, N_("Aperture priority")},
	{4, N_("Shutter priority")},
	{5, N_("Creative program (biased toward depth of field)")},
	{6, N_("Action program (biased toward fast shutter speed)")},
	{7, N_("Portrait mode (for closeup photos with the "
	       "background out of focus)")},
	{8, N_("Landscape mode (for landscape photos with the "
	       "background in focus)")},
	{0, NULL}
};

GtkWidget *
gtk_exif_entry_option_new (ExifEntry *e)
{
	GtkExifEntryOption *entry;
	GtkWidget *hbox, *label, *menu;
	GtkOptions *options;
	const gchar *title;
	GtkCellRenderer *cell;

	g_return_val_if_fail (e != NULL, NULL);
	g_return_val_if_fail (e->format == EXIF_FORMAT_SHORT, NULL);
	g_return_val_if_fail ((e->tag == EXIF_TAG_SENSING_METHOD) ||
			      (e->tag == EXIF_TAG_METERING_MODE) ||
			      (e->tag == EXIF_TAG_LIGHT_SOURCE) ||
			      (e->tag == EXIF_TAG_ORIENTATION) ||
			      (e->tag == EXIF_TAG_YCBCR_POSITIONING) ||
			      (e->tag == EXIF_TAG_EXPOSURE_PROGRAM) ||
			      (e->tag == EXIF_TAG_COMPRESSION), NULL);

	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");

	switch (e->tag) {
	case EXIF_TAG_SENSING_METHOD:
		title = N_("Sensing method:");
		options = options_sensing_method;
		break;
	case EXIF_TAG_ORIENTATION:
		title = N_("0-th row - 0-th column:");
		options = options_orientation;
		break;
	case EXIF_TAG_LIGHT_SOURCE:
		title = N_("Light source:");
		options = options_light_source;
		break;
	case EXIF_TAG_METERING_MODE:
		title = N_("Metering mode:");
		options = options_metering_mode;
		break;
	case EXIF_TAG_COMPRESSION:
		title = N_("Compression scheme:");
		options = options_compression;
		break;
	case EXIF_TAG_YCBCR_POSITIONING:
		title = N_("YCbCr Positioning:");
		options = options_ycbcr_positioning;
		break;
	case EXIF_TAG_EXPOSURE_PROGRAM:
		title = N_("Exposure Program:");
		options = options_exposure_program;
		break;
	default:
		return (NULL);
	}

	entry = g_object_new (GTK_EXIF_TYPE_ENTRY_OPTION, NULL);
	entry->priv->entry = e;
	exif_entry_ref (e);
	gtk_exif_entry_construct (GTK_EXIF_ENTRY (entry),
			exif_tag_get_title_in_ifd (e->tag, exif_content_get_ifd(e->parent)),
			exif_tag_get_description_in_ifd (e->tag, exif_content_get_ifd(e->parent)));

	hbox = gtk_hbox_new (FALSE, 5);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (entry), hbox, TRUE, FALSE, 0);
	label = gtk_label_new (_(title));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	menu = gtk_combo_box_new_with_model (
			gtk_tree_model_new_from_options (options));
	gtk_widget_show (menu);
	gtk_box_pack_start (GTK_BOX (hbox), menu, FALSE, FALSE, 0);
	entry->priv->menu = GTK_COMBO_BOX (menu);
	cell = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (menu), cell, TRUE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (menu), cell,
			"text", GTK_OPTIONS_NAME_COLUMN, NULL);
	g_signal_connect (G_OBJECT (menu), "changed",
			  G_CALLBACK (on_changed), entry);

	gtk_exif_entry_option_load (entry);

	return (GTK_WIDGET (entry));
}
