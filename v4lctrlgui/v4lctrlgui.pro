SOURCES += main.cpp \
v4lctrlgui.cpp \
 cfunct.cpp \
 controlwrappers.cpp \
 selectdevicedialog.cpp \
 errordialog.cpp
HEADERS += ui_mainwindow.h \
v4lctrlgui.h \
 cfunct.h \
 controlwrappers.h \
 ui_selectdevice.h \
 selectdevicedialog.h \
 errordialog.h \
 ui_errordialog.h
FORMS += mainwindow.ui \
 selectdevice.ui \
 errordialog.ui

RESOURCES += application.qrc

TEMPLATE = app

LIBS += -L../v4lctrl \
  -lv4lctrl \
  -L/media/data/cecko/v4lCtrlGUI/v4lctrl

TARGETDEPS += ../v4lctrl/libv4lctrl.so

CONFIG -= release

CONFIG += debug

INSTALLS += target \
v4lctrlgui
target.path = /usr/local/bin

v4lctrlgui.path = /usr/local/bin/

