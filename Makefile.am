SUBDIRS = m4m po gtk-extensions libexif-gtk tests

pkgconfigdir = $(libdir)/pkgconfig

pkgconfig_DATA = libexif-@LIBEXIF_GTK_EXTENSION@.pc
EXTRA_DIST     = libexif-gtk.pc.in libexif-gtk3.pc.in

noinst_DATA = libexif-@LIBEXIF_GTK_EXTENSION@-uninstalled.pc
EXTRA_DIST += libexif-gtk-uninstalled.pc.in libexif-gtk3-uninstalled.pc.in

ACLOCAL_AMFLAGS = -I auto-m4 -I m4m
