AC_PREREQ(2.59)
AC_INIT([EXIF GTK widget library],[0.3.6],[libexif-devel@lists.sourceforge.net],[libexif-gtk])
AC_CONFIG_SRCDIR([libexif-gtk/gtk-exif-browser.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([gnu 1.8 dist-bzip2])

GP_CHECK_SHELL_ENVIRONMENT
GP_CONFIG_MSG([Build])
GP_CONFIG_MSG([Source code location],[${srcdir}])

AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC

GP_PKG_CONFIG()dnl


dnl ---------------------------------------------------------------------------
dnl Versioning (libexif-gtk-AGE.CURRENT.REVISION):
dnl  - AGE (Micro):      Increment if any interfaces have been added; set to 0
dnl                      if any interfaces have been removed. Removal has 
dnl                      precedence over adding, so set to 0 if both happened.
dnl  - REVISION (Minor): Increment any time the source changes; set to 
dnl                      0 if you incremented CURRENT.
dnl  - CURRENT (Major):  Increment if the interface has additions, changes,
dnl                      removals.
dnl ---------------------------------------------------------------------------
LIBEXIF_GTK_AGE=0
LIBEXIF_GTK_REVISION=0
LIBEXIF_GTK_CURRENT=5
AC_SUBST(LIBEXIF_GTK_AGE)
AC_SUBST(LIBEXIF_GTK_REVISION)
AC_SUBST(LIBEXIF_GTK_CURRENT)
LIBEXIF_GTK_VERSION_INFO=`expr $LIBEXIF_GTK_CURRENT + $LIBEXIF_GTK_REVISION`:$LIBEXIF_GTK_AGE:$LIBEXIF_GTK_REVISION
AC_SUBST(LIBEXIF_GTK_VERSION_INFO)

AM_PROG_LIBTOOL
GP_CONFIG_MSG([Compiler],[${CC}])

dnl GP_CONFIG_MSG([Features])


# ---------------------------------------------------------------------------
# i18n support
# ---------------------------------------------------------------------------
GP_GETTEXT_HACK([${PACKAGE}-${LIBEXIF_GTK_CURRENT}],
                [Lutz Müller and others])
ALL_LINGUAS="es fr ru"
AM_GNU_GETTEXT_VERSION([0.14.1])
AM_GNU_GETTEXT([external])
AM_PO_SUBDIRS()
AM_ICONV()
GP_GETTEXT_FLAGS()

dnl No one is calling bindtextdomain(), so we don't need this.
dnl
dnl We cannot use AC_DEFINE_UNQUOTED() for these definitions, as
dnl we require make to do insert the proper $(datadir) value
dnl AC_SUBST([localedir],["\$(datadir)/locale"])
dnl AM_CFLAGS="$AM_CFLAGS -DLOCALEDIR=\\\"${localedir}\\\""


# ---------------------------------------------------------------------------
# libraries needed
# ---------------------------------------------------------------------------
GP_CHECK_LIBRARY([LIBEXIF],[libexif])

PKG_CHECK_MODULES([GTK], [gtk+-2.0])
AC_SUBST([GTK_LIBS])
AC_SUBST([GTK_CFLAGS])


dnl The HAVE_EXIF_MEM macro hasn't been used anywhere yet, so this section
dnl is commented out
dnl
dnl # ---------------------------------------------------------------------------
dnl # There are still some old version of libexif around. For now, don't force
dnl # users to upgrade
dnl # ---------------------------------------------------------------------------
dnl CPPFLAGS_save="$CPPFLAGS"
dnl CPPFLAGS="$LIBEXIF_CFLAGS"
dnl CPPFLAGS="$(echo "$LIBEXIF_CFLAGS" | sed "s|\$(top_builddir)|.|g;s|\$(top_srcdir)|${srcdir}|g;")"
dnl AC_MSG_CHECKING([libexif test CPPFLAGS])
dnl AC_MSG_RESULT([${CPPFLAGS}])
dnl AC_CHECK_HEADER([libexif/exif-mem.h],[
dnl 	libexif_msg="with exif-mem.h (>= 0.6.12)"
dnl 	AC_DEFINE([HAVE_EXIF_MEM],[1],[whether our libexif has exif-mem.h, i.e. version >= 0.6.12])
dnl ],[
dnl 	libexif_msg="without exif-mem.h (< 0.6.12; think about upgrading)"
dnl ])
dnl CPPFLAGS="$CPPFLAGS_save"
dnl GP_CONFIG_MSG([libexif],[${libexif_msg}])


# ---------------------------------------------------------------------------
# Warnings: If we have GCC, be paranoid.
# ---------------------------------------------------------------------------
if test "x$GCC" = "xyes"; then
    AM_CFLAGS="$AM_CFLAGS -Wall -Wchar-subscripts -Wmissing-declarations -Wmissing-prototypes -Wnested-externs -Wpointer-arith"
    AM_LDFLAGS="$AM_LDFLAGS -g -Wall"
fi

AC_SUBST([AM_CFLAGS])dnl
AC_SUBST([AM_LDFLAGS])dnl


# ---------------------------------------------------------------------------
# Output files
# ---------------------------------------------------------------------------
AC_CONFIG_FILES([
  po/Makefile.in
  Makefile
  libexif-gtk/Makefile
  libexif-gtk/libexif-gtk.pc
  gtk-extensions/Makefile
  tests/Makefile
  m4m/Makefile
])
AC_OUTPUT()dnl

GP_CONFIG_OUTPUT()dnl