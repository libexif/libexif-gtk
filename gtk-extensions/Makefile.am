noinst_LTLIBRARIES = libgtk-extensions.la

libgtk_extensions_la_SOURCES = \
	gtk-menu-option.c gtk-menu-option.h	\
	gtk-options.c gtk-options.h
libgtk_extensions_la_CFLAGS = \
	$(AM_CFLAGS) $(CFLAGS)		\
	-I$(top_srcdir)			\
	-DGTK_DISABLE_DEPRECATED	\
	$(GTK_CFLAGS)
libgtk_extensions_la_LIBADD = $(GTK_LIBS)
