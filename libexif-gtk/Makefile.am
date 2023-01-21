lib_LTLIBRARIES = libexif-@LIBEXIF_GTK_EXTENSION@.la

libexif_gtkincludedir = $(includedir)/libexif-gtk
libexif_gtkinclude_HEADERS = \
	gtk-exif-browser.h			\
	gtk-exif-content-list.h		\
	gtk-exif-entry.h			\
	gtk-exif-entry-ascii.h		\
	gtk-exif-entry-copyright.h	\
	gtk-exif-entry-date.h		\
	gtk-exif-entry-generic.h	\
	gtk-exif-entry-flash.h		\
	gtk-exif-entry-number.h		\
	gtk-exif-entry-option.h		\
	gtk-exif-entry-resolution.h	\
	gtk-exif-entry-rational.h	\
	gtk-exif-entry-user-comment.h	\
	gtk-exif-entry-version.h	\
	gtk-exif-util.h
#	gtk-exif-tree.h				\
#	gtk-exif-tree-item.h

libexif_@LIBEXIF_GTK_EXTENSION@_la_SOURCES = \
	gtk-exif-browser.c				\
	gtk-exif-content-list.c			\
	gtk-exif-entry.c				\
	gtk-exif-entry-ascii.c			\
	gtk-exif-entry-copyright.c		\
	gtk-exif-entry-date.c			\
	gtk-exif-entry-flash.c			\
	gtk-exif-entry-generic.c		\
	gtk-exif-entry-number.c			\
	gtk-exif-entry-option.c			\
	gtk-exif-entry-resolution.c		\
	gtk-exif-entry-rational.c		\
	gtk-exif-entry-user-comment.c	\
	gtk-exif-entry-version.c
# These file were never added here when they were created and have now
# bit rotted. When they are fixed, enable them here.
#	gtk-exif-tree.c					\
#	gtk-exif-tree-item.c

libexif_@LIBEXIF_GTK_EXTENSION@_la_CFLAGS =	\
	$(AM_CPPFLAGS) $(CPPFLAGS)		\
	-I$(top_srcdir)					\
	-I$(top_srcdir)/gtk-extensions	\
	$(LIBEXIF_CFLAGS)				\
	$(GTK_CFLAGS)					\
	-DG_LOG_DOMAIN=\"libexif\"

libexif_@LIBEXIF_GTK_EXTENSION@_la_LDFLAGS = -version-info @LIBEXIF_GTK_VERSION_INFO@

libexif_@LIBEXIF_GTK_EXTENSION@_la_LIBADD = \
	$(AM_LDFLAGS) $(LDFLAGS)				\
	$(top_builddir)/gtk-extensions/libgtk-extensions.la	\
	$(LIBEXIF_LIBS)							\
	$(GTK_LIBS)

# dependency workaround for this recursive make build
EXTRA_libexif_@LIBEXIF_GTK_EXTENSION@_la_DEPENDENCIES = \
	$(top_builddir)/gtk-extensions/libgtk-extensions.la-stamp
$(top_builddir)/gtk-extensions/libgtk-extensions.la-stamp:
	cd $(top_builddir)/gtk-extensions && $(MAKE) libgtk-extensions.la
