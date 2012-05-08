#!/bin/sh

# recreate autotools files
autoreconf --install --force

# check minimum required versions
#if which gtkdoc-depscan &> /dev/null; then
#  echo "Minimum required versions:"
#  gtkdoc-depscan --book=glib --book=gdk-pixbuf --book=gdk \
#				 --book=gtk gtk-extensions/* | cut -d ' ' -f 2  | sort | uniq
#fi
#
#if which gtkdoc-depscan &> /dev/null; then
#  echo "Minimum required versions:"
#  gtkdoc-depscan --book=glib --book=gdk-pixbuf --book=gdk \
#				 --book=gtk libexif-gtk/* | cut -d ' ' -f 2  | sort | uniq
#fi
