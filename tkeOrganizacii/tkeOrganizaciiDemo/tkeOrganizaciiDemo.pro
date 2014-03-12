DEFINES += TKE_DEMO

include(../tkeOrganizacii.pro)

DEPENDPATH += . ../ \
				../../tkeAbonent \
				../../tkeSqlBrowser/src \
				../../tkeSqlBrowser/ui \
				../../src_other/moneyToStrConverter \
				../../src_other/replaceStrings \
				../../install/dbUpdater \
				../../src_other/UPopulateTableCells \
				../../tkeAbonent/tke

VPATH += ../tke ../ ../../tkeAbonent
INCLUDEPATH +=   .

RC_FILE =   ../prog_icon_org.rc
DESTDIR =   ../../bin

OBJECTS_DIR = ../../tmp/demo
MOC_DIR = ../../tmp/demo
UI_HEADERS_DIR = ../../tmp/demo
UI_SOURCES_DIR = ../../tmp/demo
RCC_DIR = ../../tmp/demo
