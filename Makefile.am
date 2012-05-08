SUBDIRS = m4m po gtk-extensions libexif-gtk tests

pkgconfigdir = $(libdir)/pkgconfig

pkgconfig_DATA = libexif-@LIBEXIF_GTK_EXTENSION@.pc
EXTRA_DIST     = libexif-@LIBEXIF_GTK_EXTENSION@.pc.in

noinst_DATA = libexif-@LIBEXIF_GTK_EXTENSION@-uninstalled.pc
EXTRA_DIST += libexif-@LIBEXIF_GTK_EXTENSION@-uninstalled.pc.in

ACLOCAL_AMFLAGS = -I auto-m4 -I m4m
