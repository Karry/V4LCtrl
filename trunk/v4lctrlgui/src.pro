SOURCES += v4lctrlgui.cpp \
           main.cpp
HEADERS += v4lctrlgui.h \
 ui_mainwindow.h
TEMPLATE = app
CONFIG += warn_on \
	  thread \
          qt
TARGET = v4lctrlgui
DESTDIR = ../bin
RESOURCES = application.qrc
FORMS += mainwindow.ui

