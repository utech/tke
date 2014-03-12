CONFIG		+= release
CONFIG		-= debug debug_and_release debug_and_release_target

TEMPLATE = app
TARGET = 
DEPENDPATH += . ../../tkeAbonent \
				../../tkeOrganizacii \
				../../tkeSqlBrowser/src \
				../../tkeSqlBrowser/ui \
				../../src_other/moneyToStrConverter \
				../../src_other/replaceStrings \
				../../
INCLUDEPATH +=   . ../
QT +=   sql

HEADERS += tke_form_updateDataBase.h \
           ../../tkeOrganizacii/udebug_form.h \
           ../../tkeAbonent/umath.h \
           ../../tkeAbonent/db_own.h \
           ../../tkeOrganizacii/udiff_func.h \
		   UDbUpdaterFunctions.h
FORMS += tke_form_updateDataBase.ui \
           ../../tkeOrganizacii/udebug_form.ui
SOURCES += mainDbU.cpp \
           tke_form_updateDataBase.cpp \
           ../../tkeOrganizacii/udebug_form.cpp \
           ../../tkeAbonent/umath.cpp \
           ../../tkeAbonent/db_own.cpp \
           ../../tkeOrganizacii/udiff_func.cpp \
		   UDbUpdaterFunctions.cpp

DESTDIR =   ../../bin
RESOURCES +=   ../../tkeAbonent/tke/tke_rc

OBJECTS_DIR = ../../tmp
MOC_DIR = ../../tmp
UI_HEADERS_DIR = ../../tmp
UI_SOURCES_DIR = ../../tmp
RCC_DIR = ../../tmp