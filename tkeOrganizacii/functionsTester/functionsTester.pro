
include(../tkeOrganizacii.pro)
include(../../tkeAbonent/tkeAbonent.pro)

DEPENDPATH += . ../ \
				../../tkeAbonent \
				../../tkeSqlBrowser/src \
				../../tkeSqlBrowser/ui \
				../../src_other/moneyToStrConverter \
				../../src_other/replaceStrings \
				../../install/dbUpdater \
				../../src_other/UPopulateTableCells \
				../../tkeAbonent/tke \
				../..

VPATH += ../tke ../ ../../tkeAbonent
INCLUDEPATH +=   .

SOURCES +=  mainTester.cpp
SOURCES -=  mainO.cpp mainA.cpp

RC_FILE =   ../prog_icon_org.rc
DESTDIR =   ../../bin

OBJECTS_DIR = ./tmp
MOC_DIR = ./tmp
UI_HEADERS_DIR = ./tmp
UI_SOURCES_DIR = ./tmp
RCC_DIR = ./tmp

CONFIG += console