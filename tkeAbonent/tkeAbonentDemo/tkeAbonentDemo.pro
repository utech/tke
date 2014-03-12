DEFINES += TKE_DEMO

include(../tkeAbonent.pro)

DEPENDPATH += . ../ \
				../../tkeOrganizacii \
				../../tkeSqlBrowser/src \
				../../tkeSqlBrowser/ui \
				../../src_other/moneyToStrConverter \
				../../src_other/replaceStrings \
				../tke \
				../../install/dbUpdater \
				../../src_other/UPopulateTableCells \
				../../
VPATH += ../tke ../ ../../tkeOrganizacii ../../
INCLUDEPATH +=   . ../../ ../
				
RC_FILE =   ../prog_icon_abon.rc
DESTDIR =   ../../bin

OBJECTS_DIR = ../../tmp/demo
MOC_DIR = ../../tmp/demo
UI_HEADERS_DIR = ../../tmp/demo
UI_SOURCES_DIR = ../../tmp/demo
RCC_DIR = ../../tmp/demo
