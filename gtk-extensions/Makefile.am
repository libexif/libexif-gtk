noinst_LTLIBRARIES = libgtk-extensions.la

libgtk_extensions_la_SOURCES = \
	gtk-menu-option.c gtk-menu-option.h	\
	gtk-options.c gtk-options.h
# We don't set -DGTK_DISABLE_DEPRECATED here because
# our code is quite old and hasn't been updated to the
# newest stuff yet.
libgtk_extensions_la_CFLAGS = \
	$(AM_CFLAGS) $(CFLAGS)		\
	-I$(top_srcdir)			\
	$(GTK_CFLAGS)
libgtk_extensions_la_LIBADD = $(GTK_LIBS)
