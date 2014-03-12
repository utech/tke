CONFIG		+= release
CONFIG		-= debug debug_and_release debug_and_release_target

include(d:\devel\UtechLib\UtechLib.pri)

TEMPLATE = app
TARGET = 
DEPENDPATH += . ../../tkeAbonent ../../tkeOrganizacii
INCLUDEPATH += .
QT += sql
# Input
HEADERS += qtwizard.h \
			usettings.h \
			settings.h \
			db_own.h \
			udiff_func.h
SOURCES += mainC.cpp \
			qtwizard.cpp \
			usettings.cpp \
			settings.cpp \
			db_own.cpp \
			udiff_func.cpp

DESTDIR +=   ../../bin

RESOURCES +=   tke/tke_rc

RC_FILE +=   prog_icon_conf.rc

OBJECTS_DIR = ../../tmp
MOC_DIR = ../../tmp
UI_HEADERS_DIR = ../../tmp
UI_SOURCES_DIR = ../../tmp
RCC_DIR = ../../tmp
