lib_LTLIBRARIES = libexif-gtk.la

libexif_gtkincludedir = $(includedir)/libexif-gtk
libexif_gtkinclude_HEADERS =		\
	gtk-exif-browser.h		\
	gtk-exif-content-list.h		\
	gtk-exif-entry.h		\
	gtk-exif-entry-ascii.h		\
	gtk-exif-entry-copyright.h	\
	gtk-exif-entry-date.h		\
	gtk-exif-entry-generic.h	\
	gtk-exif-entry-flash.h		\
	gtk-exif-entry-number.h		\
	gtk-exif-entry-option.h		\
	gtk-exif-entry-rational.h	\
	gtk-exif-entry-resolution.h	\
	gtk-exif-entry-user-comment.h	\
	gtk-exif-entry-version.h

libexif_gtk_la_SOURCES =			\
	gtk-exif-browser.c			\
	gtk-exif-content-list.c			\
	gtk-exif-entry.c			\
	gtk-exif-entry-ascii.c			\
	gtk-exif-entry-copyright.c		\
	gtk-exif-entry-date.c			\
	gtk-exif-entry-generic.c		\
	gtk-exif-entry-flash.c			\
	gtk-exif-entry-number.c			\
	gtk-exif-entry-option.c			\
	gtk-exif-entry-rational.c		\
	gtk-exif-entry-resolution.c		\
	gtk-exif-entry-user-comment.c		\
	gtk-exif-entry-version.c		\
	gtk-exif-util.h

# We cannot define -DGTK_DISABLE_DEPRECATED here
# as long as we're still using our old code with
# the old deprecated GTK stuff.

libexif_gtk_la_CFLAGS =	\
	$(AM_CPPFLAGS) $(CPPFLAGS)			\
	-I$(top_srcdir)					\
	-I$(top_srcdir)/gtk-extensions			\
	$(LIBEXIF_CFLAGS)				\
	$(GTK_CFLAGS)					\
	-DG_LOG_DOMAIN=\"libexif\"
libexif_gtk_la_LDFLAGS = -version-info @LIBEXIF_GTK_VERSION_INFO@
libexif_gtk_la_LIBADD = \
	$(AM_LDFLAGS) $(LDFLAGS)				\
	$(top_builddir)/gtk-extensions/libgtk-extensions.la	\
	$(LIBEXIF_LIBS)						\
	$(GTK_LIBS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libexif-gtk.pc
EXTRA_DIST = libexif-gtk.pc.in
