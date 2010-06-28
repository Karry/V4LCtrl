TEMPLATE = lib

CONFIG += dll

CONFIG -= exceptions \
stl \
qt
SOURCES += libv4lctrl.c

HEADERS += libv4lctrl.h

INSTALLS += libv4lctrl.so.1.0.0 \
libv4lctrl.so.1.0 \
libv4lctrl.so.1 \
libv4lctrl.so \
target
libv4lctrl.so.1.0.0.path = /usr/local/lib/

libv4lctrl.so.1.0.path = /usr/local/lib/

libv4lctrl.so.1.path = /usr/local/bin/

libv4lctrl.so.path = /usr/local/lib/

target.path = /usr/local/lib

