SOURCES += struct-dump.c \
struct-v4l2.c \
v4lctrlcli.c
HEADERS += struct-dump.h \
struct-v4l2.h \
v4lctrlcli.h
TEMPLATE = app

LIBS += -L../v4lctrl \
  -lv4lctrl \
  -L/media/data/cecko/v4lCtrlGUI/v4lctrl

INSTALLS += v4lctrlcli \
target
v4lctrlcli.path = /usr/local/bin/

target.path = /usr/local/bin

