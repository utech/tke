CONFIG		+= release
CONFIG		-= debug debug_and_release debug_and_release_target

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += UForm_tkeDisk.h
FORMS += UForm_tkeDisk.ui
SOURCES += main.cpp UForm_tkeDisk.cpp
RESOURCES += tkeDisk.qrc
RC_FILE +=   prog_icon.rc

DESTDIR = ../bin

