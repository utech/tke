CONFIG		+= release
CONFIG		-= debug debug_and_release debug_and_release_target

include(../../../UtechLib/UtechLib.pri)

TEMPLATE = app
TARGET = configure
DEPENDPATH += . ../../tkeAbonent ../../tkeOrganizacii
INCLUDEPATH += .
QT += sql

CONFIG(debug, debug|release) {
    win32: TARGET = $$join(TARGET,,,d)
    unix: TARGET = $$join(TARGET,,,_debug)
}

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

CONFIG(debug, debug|release) {
    OBJECTS_DIR = ../../tmpd/configure
    MOC_DIR = ../../tmpd/configure
    UI_HEADERS_DIR = ../../tmpd/configure
    UI_SOURCES_DIR = ../../tmpd/configure
    RCC_DIR = ../../tmpd/configure
}
else {
    OBJECTS_DIR = ../../tmp/configure
    MOC_DIR = ../../tmp/configure
    UI_HEADERS_DIR = ../../tmp/configure
    UI_SOURCES_DIR = ../../tmp/configure
    RCC_DIR = ../../tmp/configure
}

