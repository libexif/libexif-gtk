noinst_PROGRAMS = test-libexif-gtk

test_libexif_gtk_CFLAGS = \
	$(AM_CFLAGS) $(CFLAGS)				\
	-I$(top_srcdir)					\
	$(LIBEXIF_CFLAGS)				\
	$(GTK_CFLAGS)
test_libexif_gtk_LDADD = \
	$(AM_LDFLAGS) $(LDFLAGS)			\
	$(top_builddir)/libexif-gtk/libexif-gtk.la	\
	$(LIBEXIF_LIBS)					\
	$(GTK_LIBS)					\
	$(INTLLIBS)
