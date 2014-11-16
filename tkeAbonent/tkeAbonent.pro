#CONFIG		+= release
#CONFIG		-= debug debug_and_release debug_and_release_target

include(../../UtechLib/UtechLib.pri)

TEMPLATE =   app
TARGET = tkeAbonent
DEPENDPATH += . ../tkeOrganizacii \
				../tkeSqlBrowser/src \
				../tkeSqlBrowser/ui \
				../src_other/moneyToStrConverter \
				../src_other/replaceStrings \
				./tke \
				../install/dbUpdater \
				../src_other/UPopulateTableCells \
				../
INCLUDEPATH +=   . ../
QT +=   sql script network

CONFIG(debug, debug|release) {
    win32: TARGET = $$join(TARGET,,,d)
    unix: TARGET = $$join(TARGET,,,_debug)
}

CONFIG += assistant
HEADERS +=   db_own.h \
  tke_dialog_change_password.h \
  tke_dialog_login.h \
  tke_dialog_narah.h \
  tke_dialog_print_kvytancii.h \
  tke_form_insert_kvyt.h \
  tke_form_insert_subs.h \
  tke_mainwindow.h \
  ubudynky_delegate.h \
  udiuch_taryf_delegate.h \
  uexport_info_pilgovyky.h \
  ukorektura_delegate.h \
  ukorektura_s_delegate.h \
  ukvyt_delegate.h \
  tke_math.h \
  unormat_taryf_delegate.h \
  uoperacii_delegate.h \
  upilgovyky_delegate.h \
  usplata_zaborg_delegate.h \
  uspravy_subs_delegate.h \
  usubs_delegate.h \
  tke_form_insert_kvyt_dbf_ob.h \
  usettings.h \
  uinsert_kvyt_ob_delegate.h \
  tke_dialog_filtr_borjnykiv.h \
  uprint_docs.h \
  tke_form_kvytancii_doc.h \
  uform_kvytancii_doc_delegate.h \
  tke_form_posada.h \
  udiff_func.h \
  browser.h \
  highlighter.h \
  tke_dialog_pokaznyky_lichylnyka.h \
  tke_form_edit_users.h \
  UReplaceStringsParser.h \
  tke_form_print_poshtu_dlya_borjnyk.h \
  tke_form_print_poshtu_dlya_pilgovykiv.h \
  tke_Dialog_saveDataBase.h \
  tke_form_extendedSearch.h \
  tke_form_struct_GV.h \
  tke_Form_insert_pokazn_lich_GV.h \
  tke_dialog_options.h \
  tke_Dialog_editOperaciiStrings.h \
  tke_Form_uni_insert_kvytancii.h \
  UDbScriptAccess.h \
  UTkeGlobal.h \
  tke_Form_uni_insert_subs.h \
  tke_Form_edit_pokazn_lich.h \
  tke_dialog_date_interval.h \
  UAbonentFunctions.h \
  UDbUpdaterFunctions.h \
  tke_Form_fastPilgovykyDocInsert.h \
  tke_Dialog_printNadannyaPoslugPoBudTaKot.h \
  tke_Form_insert_opSubs.h \
  tke_Form_sendDataBase.h \
  UForm_fillSubsDbfSpojyvannyaCO.h \
  tke_form_printCustomKvyt.h \
  tke_form_recalc_obsyag_gazu.h \
  tke_dialog_budynok_or_abonent.h \
  tke_form_fill_subs_narah.h
FORMS +=   tke_dialog_change_password.ui \
  tke_form_posada.ui \
  tke_dialog_login.ui \
  tke_dialog_narah.ui \
  tke_dialog_print_kvytancii.ui \
  tke_form_insert_kvyt.ui \
  tke_form_insert_subs.ui \
  tke_mainwindow.ui \
  tke_form_insert_kvyt_dbf_ob.ui \
  tke_dialog_filtr_borjnykiv.ui \
  tke_form_kvytancii_doc.ui \
  mainwindow.ui \
  redag.ui \
  tke_dialog_pokaznyky_lichylnyka.ui \
  tke_form_edit_users.ui \
  tke_form_print_poshtu_dlya_borjnyk.ui \
  tke_form_print_poshtu_dlya_pilgovykiv.ui \
  tke_Dialog_saveDataBase.ui \
  tke_form_extendedSearch.ui \
  tke_form_struct_GV.ui \
  tke_Form_insert_pokazn_lich_GV.ui \
  tke_dialog_options.ui \
  tke_Dialog_editOperaciiStrings.ui \
  tke_Form_uni_insert_kvytancii.ui \
  tke_Form_uni_insert_subs.ui \
  tke_Form_edit_pokazn_lich.ui \
  tke_dialog_date_interval.ui \
  tke_Form_fastPilgovykyDocInsert.ui \
  tke_Dialog_printNadannyaPoslugPoBudTaKot.ui \
  tke_Form_insert_opSubs.ui \
  tke_Form_sendDataBase.ui \
  UForm_fillSubsDbfSpojyvannyaCO.ui \
  tke_form_printCustomKvyt.ui \
  tke_form_recalc_obsyag_gazu.ui \
  tke_dialog_budynok_or_abonent.ui \
  tke_form_fill_subs_narah.ui
SOURCES +=   db_own.cpp \
  mainA.cpp \
  tke_dialog_change_password.cpp \
  tke_form_posada.cpp \
  tke_dialog_login.cpp \
  tke_dialog_narah.cpp \
  tke_dialog_print_kvytancii.cpp \
  tke_form_insert_kvyt.cpp \
  tke_form_insert_subs.cpp \
  tke_mainwindow.cpp \
  ubudynky_delegate.cpp \
  udiuch_taryf_delegate.cpp \
  uexport_info_pilgovyky.cpp \
  ukorektura_delegate.cpp \
  ukorektura_s_delegate.cpp \
  ukvyt_delegate.cpp \
  tke_math.cpp \
  unormat_taryf_delegate.cpp \
  uoperacii_delegate.cpp \
  upilgovyky_delegate.cpp \
  usplata_zaborg_delegate.cpp \
  uspravy_subs_delegate.cpp \
  usubs_delegate.cpp \
  tke_form_insert_kvyt_dbf_ob.cpp \
  usettings.cpp \
  uinsert_kvyt_ob_delegate.cpp \
  tke_dialog_filtr_borjnykiv.cpp \
  uprint_docs.cpp \
  uprint_docs_2.cpp \
  uprint_docs_kvytancii.cpp \
  tke_form_kvytancii_doc.cpp \
  uform_kvytancii_doc_delegate.cpp \
  udiff_func.cpp \
  browser.cpp \
  highlighter.cpp \
  tke_dialog_pokaznyky_lichylnyka.cpp \
  tke_form_edit_users.cpp \
  UReplaceStringsParser.cpp \
  tke_form_print_poshtu_dlya_borjnyk.cpp \
  tke_form_print_poshtu_dlya_pilgovykiv.cpp \
  tke_Dialog_saveDataBase.cpp \
  tke_form_extendedSearch.cpp \
  tke_form_struct_GV.cpp \
  tke_Form_insert_pokazn_lich_GV.cpp \
  tke_dialog_options.cpp \
  tke_Dialog_editOperaciiStrings.cpp \
  tke_Form_uni_insert_kvytancii.cpp \
  UDbScriptAccess.cpp \
  tke_Form_uni_insert_subs.cpp \
  tke_mainwindow_actions.cpp \
  tke_mainwindow_narah.cpp \
  tke_mainwindow_constructor.cpp \
  tke_mainwindow_debitorska.cpp \
  tke_Form_edit_pokazn_lich.cpp \
  tke_dialog_date_interval.cpp \
  UAbonentFunctions.cpp \
  UDbUpdaterFunctions.cpp \
  tke_Form_fastPilgovykyDocInsert.cpp \
  tke_Dialog_printNadannyaPoslugPoBudTaKot.cpp \
  tke_Form_insert_opSubs.cpp \
  tke_Form_sendDataBase.cpp \
  UForm_fillSubsDbfSpojyvannyaCO.cpp \
  tke_form_printCustomKvyt.cpp \
  tke_form_recalc_obsyag_gazu.cpp \
  tke_dialog_budynok_or_abonent.cpp \
  tke_form_fill_subs_narah.cpp
RESOURCES +=   tke_rc
RC_FILE =   prog_icon_abon.rc
DESTDIR =   ../bin

#CONFIG +=console

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
