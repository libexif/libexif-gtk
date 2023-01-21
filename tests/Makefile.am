# This is really a demo program and not a regression test, so build it
# during the main build and not just in the check target.
noinst_PROGRAMS = test-libexif-gtk

test_libexif_gtk_CFLAGS = 	\
	$(AM_CFLAGS) $(CFLAGS)	\
	-I$(top_srcdir)			\
	$(LIBEXIF_CFLAGS)		\
	$(GTK_CFLAGS)

test_libexif_gtk_LDADD = 		\
	$(AM_LDFLAGS) $(LDFLAGS)	\
	$(top_builddir)/libexif-gtk/libexif-@LIBEXIF_GTK_EXTENSION@.la	\
	$(LIBEXIF_LIBS)				\
	$(GTK_LIBS)					\
	$(INTLLIBS)

# dependency workaround for this recursive make build
EXTRA_test_libexif_gtk_DEPENDENCIES = \
	$(top_builddir)/libexif-gtk/libexif-@LIBEXIF_GTK_EXTENSION@.la-stamp
$(top_builddir)/libexif-gtk/libexif-@LIBEXIF_GTK_EXTENSION@.la-stamp:
	cd $(top_builddir)/libexif-gtk && $(MAKE) libexif-@LIBEXIF_GTK_EXTENSION@.la
