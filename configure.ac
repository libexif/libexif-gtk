AC_PREREQ([2.69])
AC_INIT([EXIF GTK widget library],
        [0.5.1git],
        [libexif-devel@lists.sourceforge.net],
        [libexif-gtk],
        [https://libexif.github.io/])
AC_CONFIG_SRCDIR([libexif-gtk/gtk-exif-browser.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([auto-m4])
AM_INIT_AUTOMAKE([
  -Wall
  gnu
  1.14.1
  dist-xz
  dist-bzip2
  check-news
])
AM_SILENT_RULES([yes])

GP_CHECK_SHELL_ENVIRONMENT
GP_CONFIG_MSG([Build])
GP_CONFIG_MSG([Source code location], [${srcdir}])

AC_PROG_CC
AM_PROG_CC_STDC
AM_PROG_AR
LT_INIT
AC_HEADER_STDC
PKG_PROG_PKG_CONFIG

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
LIBEXIF_GTK_REVISION=1
LIBEXIF_GTK_CURRENT=5
AC_SUBST([LIBEXIF_GTK_AGE])
AC_SUBST([LIBEXIF_GTK_REVISION])
AC_SUBST([LIBEXIF_GTK_CURRENT])
LIBEXIF_GTK_VERSION_INFO=`expr $LIBEXIF_GTK_CURRENT + $LIBEXIF_GTK_REVISION`:$LIBEXIF_GTK_AGE:$LIBEXIF_GTK_REVISION
AC_SUBST([LIBEXIF_GTK_VERSION_INFO])

GP_CONFIG_MSG([Compiler], [${CC}])

dnl GP_CONFIG_MSG([Features])


# ---------------------------------------------------------------------------
# conditional libraries
# ---------------------------------------------------------------------------
AC_ARG_WITH([gtk3],
            [AS_HELP_STRING([--with-gtk3],[build with gtk3 (default is gtk2)])],
            [LIBEXIF_GTK_EXTENSION=gtk3],
            [LIBEXIF_GTK_EXTENSION=gtk])

AS_VAR_IF([LIBEXIF_GTK_EXTENSION],
          [gtk3],
          [GP_CHECK_LIBRARY([GTK], [gtk+-3.0])],
          [GP_CHECK_LIBRARY([GTK], [gtk+-2.0], [>= 2.4])])

AC_SUBST([LIBEXIF_GTK_EXTENSION])dnl


# ---------------------------------------------------------------------------
# i18n support
# ---------------------------------------------------------------------------
ALL_LINGUAS="de es fr pl ru ka"
AM_PO_SUBDIRS
AS_VAR_IF([LIBEXIF_GTK_EXTENSION],
          [gtk],
          [LIBEXIF_GTK_FULLNAME=libexif-$LIBEXIF_GTK_EXTENSION$LIBEXIF_GTK_CURRENT],
          [LIBEXIF_GTK_FULLNAME=libexif-$LIBEXIF_GTK_EXTENSION-$LIBEXIF_GTK_CURRENT])
GP_GETTEXT_HACK([${LIBEXIF_GTK_FULLNAME}], [Lutz Müller and others])
AM_GNU_GETTEXT_VERSION([0.18.3])
AM_GNU_GETTEXT([external])
AM_ICONV
GP_GETTEXT_FLAGS


# ---------------------------------------------------------------------------
# libraries needed
# ---------------------------------------------------------------------------
GP_CHECK_LIBRARY([LIBEXIF], [libexif])dnl


# ---------------------------------------------------------------------------
# compiler/Linker options and warnings
# ---------------------------------------------------------------------------
AM_CPPFLAGS="$AM_CPPFLAGS -I\$(top_srcdir)"
AM_CPPFLAGS="$AM_CPPFLAGS -I\$(top_builddir)"
AM_LDFLAGS="$LDFLAGS"
AS_VAR_IF([GCC], [yes], [dnl
    AM_CFLAGS="$AM_CFLAGS -Wall -Wchar-subscripts -Wmissing-declarations -Wmissing-prototypes -Wnested-externs -Wpointer-arith"
    AM_CXXFLAGS="$AM_CXXFLAGS -ansi -pedantic-error"
    AM_CPPFLAGS="$AM_CPPFLAGS -g -Wall -Wmissing-declarations -Wmissing-prototypes"
    AM_LDFLAGS="$AM_LDFLAGS -g -Wall"
])

AC_SUBST([AM_CPPFLAGS])dnl
AC_SUBST([AM_LDFLAGS])dnl
AC_SUBST([AM_CFLAGS])dnl
AC_SUBST([AM_LDFLAGS])dnl


# ---------------------------------------------------------------------------
# Output files
# ---------------------------------------------------------------------------
AC_CONFIG_FILES([
  po/Makefile.in
  Makefile
  libexif-gtk/Makefile
  libexif-gtk.pc
  libexif-gtk3.pc
  libexif-gtk-uninstalled.pc
  libexif-gtk3-uninstalled.pc
  gtk-extensions/Makefile
  tests/Makefile
  m4m/Makefile
])
AC_OUTPUT

GP_CONFIG_OUTPUT
