CONFIG		+= release
CONFIG		-= debug debug_and_release debug_and_release_target

include(../../UtechLib/UtechLib.pri)

TEMPLATE =   app
TARGET = tkeOrganizacii
DEPENDPATH += . ../tkeAbonent \
				../tkeSqlBrowser/src \
				../tkeSqlBrowser/ui \
				../src_other/replaceStrings \
				../install/dbUpdater \
				../tkeAbonent/tke
INCLUDEPATH +=   . \
                ../src_other/replaceStrings
QT +=   sql
CONFIG += assistant

CONFIG(debug, debug|release) {
    win32: TARGET = $$join(TARGET,,,d)
    unix: TARGET = $$join(TARGET,,,_debug)
}

HEADERS +=   db_own.h \
  tke_mainwindow_org.h \
  tke_dialog_edit_pidrozdil.h \
  tke_math.h \
  utaryf_delegate.h \
  upokaz_lich_opal_delegate.h \
  udiff_func.h \
  tke_form_days_opal.h \
  tke_form_days_opal_simple.h \
  udni_opal_delegate.h \
  udni_opal_query_model_delegate.h \
  tke_dialog_narah.h \
  uoplata_delegate.h \
  browser.h \
  highlighter.h \
  uprint_docs_organiz.h \
  UReplaceStringsParser.h \
  ucalc_def_values_thread.h \
  tke_dialog_print_rahunok.h \
  tke_dialog_filtr_borjnykiv_organiz.h \
  tke_dialog_date_interval.h \
  tke_dialog_login.h \
  tke_dialog_change_password.h \
  tke_dialog_options.h \
  tke_Dialog_saveDataBase.h \
  UDbUpdaterFunctions.h \
  tke_dialog_print_nakladna.h \
  tke_form_struct_GV_organiz.h \
  tke_dialog_edit_bank_rahunky.h
FORMS +=   tke_dialog_edit_pidrozdil.ui \
  tke_mainwindow_org.ui \
  tke_form_days_opal.ui \
  tke_dialog_narah.ui \
  mainwindow.ui \
  redag.ui \
  tke_dialog_print_rahunok.ui \
  tke_dialog_filtr_borjnykiv_organiz.ui \
  tke_dialog_date_interval.ui \
  tke_dialog_login.ui \
  tke_dialog_change_password.ui \
  tke_dialog_options.ui \
  tke_Dialog_saveDataBase.ui \
  tke_dialog_print_nakladna.ui \
  tke_form_struct_GV_organiz.ui \
  tke_dialog_edit_bank_rahunky.ui
SOURCES +=  db_own.cpp \
  mainO.cpp \
  tke_mainwindow_org.cpp \
  tke_dialog_edit_pidrozdil.cpp \
  tke_math.cpp \
  utaryf_delegate.cpp \
  upokaz_lich_opal_delegate.cpp \
  udiff_func.cpp \
  tke_form_days_opal.cpp \
  tke_form_days_opal_simple.cpp \
  udni_opal_delegate.cpp \
  udni_opal_query_model_delegate.cpp \
  tke_dialog_narah.cpp \
  uoplata_delegate.cpp \
  highlighter.cpp \
  browser.cpp \
  uprint_docs_organiz.cpp \
  uprint_docs_organiz_2.cpp \
  UReplaceStringsParser.cpp \
  ucalc_def_values_thread.cpp \
  tke_dialog_print_rahunok.cpp \
  tke_dialog_filtr_borjnykiv_organiz.cpp \
  tke_dialog_date_interval.cpp \
  tke_dialog_login.cpp \
  tke_dialog_change_password.cpp \
  tke_dialog_options.cpp \
  tke_Dialog_saveDataBase.cpp \
  UDbUpdaterFunctions.cpp \
  tke_dialog_print_nakladna.cpp \
  tke_form_struct_GV_organiz.cpp \
  tke_dialog_edit_bank_rahunky.cpp

RESOURCES +=   tke_rc
RC_FILE =   prog_icon_org.rc
DESTDIR =   ../bin

CONFIG(debug, debug|release) {
    OBJECTS_DIR = ../tmpd
    MOC_DIR = ../tmpd
    UI_HEADERS_DIR = ../tmpd
    UI_SOURCES_DIR = ../tmpd
    RCC_DIR = ../tmpd
}
else {
    OBJECTS_DIR = ../tmp
    MOC_DIR = ../tmp
    UI_HEADERS_DIR = ../tmp
    UI_SOURCES_DIR = ../tmp
    RCC_DIR = ../tmp
}
