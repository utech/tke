//------------------------------------------------------------
//
// Implamantation of tke_MainWindow class.
// Created by Roman Mulyk 08.08.2006
//
//------------------------------------------------------------

#include <USqlAccessible>
#include "tke_mainwindow.h"
#include "ubudynky_delegate.h"
#include "unormat_taryf_delegate.h"
#include "udiuch_taryf_delegate.h"
#include "usubs_delegate.h"
#include "ukvyt_delegate.h"
#include "uspravy_subs_delegate.h"
#include "ukorektura_delegate.h"
#include "usplata_zaborg_delegate.h"
#include "upilgovyky_delegate.h"
#include "uoperacii_delegate.h"
#include "ukorektura_s_delegate.h"
#include "usettings.h"
#include "tke_dialog_filtr_borjnykiv.h"
#include "../tkeSqlBrowser/src/browser.h"
#include "../tkeOrganizacii/tke_dialog_date_interval.h"
#include "tke_Form_fastPilgovykyDocInsert.h"
#include "tke_Dialog_printNadannyaPoslugPoBudTaKot.h"
#include "tke_Form_insert_opSubs.h"
#include "tke_dialog_narah.h"
#include "tke_dialog_login.h"
#include "tke_dialog_change_password.h"
#include "tke_form_extendedSearch.h"
#include "tke_form_struct_GV.h"
#include "tke_Form_insert_pokazn_lich_GV.h"
#include "tke_Form_uni_insert_kvytancii.h"
#include "tke_Form_uni_insert_subs.h"
#include "tke_dialog_print_kvytancii.h"
#include "tke_form_insert_kvyt_dbf_ob.h"
#include "tke_form_kvytancii_doc.h"
#include "tke_dialog_pokaznyky_lichylnyka.h"
#include "tke_form_print_poshtu_dlya_borjnyk.h"
#include "tke_form_print_poshtu_dlya_pilgovykiv.h"
#include "tke_dialog_options.h"
#include "tke_Form_edit_pokazn_lich.h"
#include "tke_form_insert_kvyt.h"
#include "tke_form_insert_subs.h"
#include "tke_form_posada.h"
#include "tke_form_edit_users.h"
#include "tke_Form_sendDataBase.h"
#include "UForm_fillSubsDbfSpojyvannyaCO.h"
#include "tke_form_printCustomKvyt.h"
#include "tke_form_recalc_obsyag_gazu.h"
#include "tke_dialog_budynok_or_abonent.h"

//------------------------------------------------------------
void tke_MainWindow::action_registracion_activated()
{
	int dialogResult;
	tke_Dialog_login *dialogLogin = new tke_Dialog_login();
	if (dialogLogin->exec() == 1){
		dialogResult = dialogLogin->dialogResult();
		if (dialogResult > 100){
			tke_Dialog_change_password * dlgChPswrd = new tke_Dialog_change_password(0, dialogLogin->userLogin(), dialogLogin->userId());
			dlgChPswrd->exec();
			delete dlgChPswrd;
			dialogResult -= 100;
		}
		ui.centralwidget->parentWidget()->setWindowTitle(codec->toUnicode("ТКЕнерго менеджер :: абонент --> ") + dialogLogin->userLogin() + " (" + OrganizName() + "). Версія "+TkeProgramVersion());
		workMode(dialogResult);
	}
	delete dialogLogin;
}
//------------------------------------------------------------
void tke_MainWindow::action_extSearch_activated()
{
	tke_form_extendedSearch *extSearch = new tke_form_extendedSearch(this);
	extSearch->show();
	
	connect(this, SIGNAL(mainWindowClosed()), extSearch, SLOT(close()));
	connect(this, SIGNAL(closeLoginSignal()), extSearch, SLOT(close()));
	connect(extSearch, SIGNAL(requestRahunok(int)), this, SLOT(show_rahunok(int)));
}
//------------------------------------------------------------
void tke_MainWindow::action_structPostachGV_activated()
{
	tke_form_struct_GV *structGV = new tke_form_struct_GV();
	structGV->show();
	
	connect(this, SIGNAL(mainWindowClosed()), structGV, SLOT(close()));
	connect(this, SIGNAL(closeLoginSignal()), structGV, SLOT(close()));
	connect(structGV, SIGNAL(requestRahunok(int)), this, SLOT(show_rahunok(int)));
}
//------------------------------------------------------------
void tke_MainWindow::action_calcObsyagTeplEnerg_activated()
{
	tke_form_recalc_obsyag_gazu * form = new tke_form_recalc_obsyag_gazu(currentSysDate);
	form->show();
	connect(this, SIGNAL(mainWindowClosed()), form, SLOT(close()));
	connect(this, SIGNAL(closeLoginSignal()), form, SLOT(close()));
}
//------------------------------------------------------------
void tke_MainWindow::action_close_login_activated()
{
	ui.centralwidget->parentWidget()->setWindowTitle("ТКЕнерго менеджер :: абонент --> загальний (" + OrganizName() + "). Версія "+TkeProgramVersion());
	saveMode();
}
//------------------------------------------------------------
void tke_MainWindow::action_budynky_activated()
{ 
	if (ui.action_budynky->isChecked()){
		ui.dockWidget_budynky->setVisible( true );
		model_budynky->setTable("budynky");
		model_budynky->setRelation(3, QSqlRelation("int_values", "id", "lich_values"));
		model_budynky->setRelation(6, QSqlRelation("t_kotelni_inf", "id", "kotelnya_inf"));
		   //model_budynky->setSort(1, Qt::AscendingOrder);
		model_budynky->setHeaderData(1, Qt::Horizontal, codec->toUnicode("Вулиця") );
		model_budynky->setHeaderData(2, Qt::Horizontal, codec->toUnicode("Номер буд.") );
		model_budynky->setHeaderData(3, Qt::Horizontal, codec->toUnicode("Лічильник") );
		model_budynky->setHeaderData(4, Qt::Horizontal, codec->toUnicode("Коеф.") );
		model_budynky->setHeaderData(5, Qt::Horizontal, codec->toUnicode("Заг. площа") );
		model_budynky->setHeaderData(6, Qt::Horizontal, codec->toUnicode("Котельня") );
		model_budynky->select();
		model_budynky->setEditStrategy(QSqlTableModel::OnRowChange);
				
		ui.tableView_budynky->setModel(model_budynky);
		ui.tableView_budynky->setColumnHidden(0, true);
		ui.tableView_budynky->setSelectionMode(QAbstractItemView::SingleSelection);
		ui.tableView_budynky->setSelectionBehavior(QAbstractItemView::SelectRows);
			//ui.tableView_budynky->setItemDelegate(new QSqlRelationalDelegate(ui.tableView_budynky));
		ui.tableView_budynky->setItemDelegate(new UBudynky_Delegate(ui.tableView_budynky));
		ui.tableView_budynky->resizeColumnsToContents();
	}
	else{
		ui.dockWidget_budynky->setVisible( false );
		model_budynky->clear();
	}
}
//------------------------------------------------------------
void tke_MainWindow::action_pilgy_activated()
{
	if (ui.action_pilgy->isChecked()){
		ui.dockWidget_pilgy->setVisible( true );
		model_pilgy->setTable("pilgy");
			//model_pilgy->setSort(1, Qt::AscendingOrder);
		model_pilgy->setHeaderData(1, Qt::Horizontal, codec->toUnicode("Категорія пільг") );
		model_pilgy->setHeaderData(2, Qt::Horizontal, codec->toUnicode("Відсоток") );
		model_pilgy->setHeaderData(3, Qt::Horizontal, codec->toUnicode("Площа") );
		model_pilgy->setHeaderData(4, Qt::Horizontal, codec->toUnicode("ІД") );
		model_pilgy->setHeaderData(5, Qt::Horizontal, codec->toUnicode("Групи") );
		model_pilgy->select();
		model_pilgy->setEditStrategy(QSqlTableModel::OnRowChange);
		
		ui.tableView_pilgy->setModel(model_pilgy);
		ui.tableView_pilgy->setColumnHidden(0, true);
		ui.tableView_pilgy->setSelectionMode(QAbstractItemView::SingleSelection);
		ui.tableView_pilgy->setSelectionBehavior(QAbstractItemView::SelectRows);
		//ui.tableView_pilgy->setItemDelegate(new QSqlRelationalDelegate(ui.tableView_pilgy));
		ui.tableView_pilgy->resizeColumnsToContents();
	}
	else{
		ui.dockWidget_pilgy->setVisible( false );
		model_pilgy->clear();
	}
}
//------------------------------------------------------------
void tke_MainWindow::action_normat_taryf_activated()
{
	if (ui.action_normat_taryf->isChecked()){
		ui.dockWidget_normat_taryf->setVisible( true );
		model_normat_taryf->setTable("normat_taryf");
		    //model_normat_taryf->setSort(1, Qt::AscendingOrder);
		model_normat_taryf->setHeaderData(1, Qt::Horizontal, codec->toUnicode("Рік") );
		model_normat_taryf->setHeaderData(2, Qt::Horizontal, codec->toUnicode("Місяць") );
		model_normat_taryf->setHeaderData(3, Qt::Horizontal, codec->toUnicode("Варт.Гкал.ЦО") );
		model_normat_taryf->setHeaderData(4, Qt::Horizontal, codec->toUnicode("Варт.Квадр.ЦО.") );
		model_normat_taryf->setHeaderData(5, Qt::Horizontal, codec->toUnicode("Варт.Куб.ГВ") );
		model_normat_taryf->setHeaderData(6, Qt::Horizontal, codec->toUnicode("Варт.Особ.ГВ") );
		model_normat_taryf->setHeaderData(7, Qt::Horizontal, codec->toUnicode("Норм.ГВ.на особу") );
		model_normat_taryf->select();
		model_normat_taryf->setEditStrategy(QSqlTableModel::OnRowChange);
		ui.tableView_normat_taryf->setModel(model_normat_taryf);
		ui.tableView_normat_taryf->setColumnHidden(0, true);
		ui.tableView_normat_taryf->setSelectionMode(QAbstractItemView::SingleSelection);
		ui.tableView_normat_taryf->setSelectionBehavior(QAbstractItemView::SelectRows);
		ui.tableView_normat_taryf->setItemDelegate(new UNormat_taryf_Delegate(ui.tableView_normat_taryf));
		ui.tableView_normat_taryf->resizeColumnsToContents();
	}
	else{
		ui.dockWidget_normat_taryf->setVisible( false );
		model_normat_taryf->clear();
	}
}
//------------------------------------------------------------
void tke_MainWindow::action_diuchyi_taryf_activated()
{
	ui.dockWidget_diuch_taryf->setVisible( ui.action_diuchyi_taryf->isChecked() );
}
//------------------------------------------------------------
void tke_MainWindow::action_pokaz_lich_GV_activated()
{
	tke_Form_insert_pokazn_lich_GV *pokazLichGV = new tke_Form_insert_pokazn_lich_GV(currentSysDate.year(), currentSysDate.month());
	pokazLichGV->show();
	
	connect(this, SIGNAL(mainWindowClosed()), pokazLichGV, SLOT(close()));
	connect(this, SIGNAL(closeLoginSignal()), pokazLichGV, SLOT(close()));
	connect(pokazLichGV, SIGNAL(requestRahunok(int)), this, SLOT(show_rahunok(int)));
}
//------------------------------------------------------------
void tke_MainWindow::action_korektura_activated()
{
	if (ui.action_korektura->isChecked()){
		ui.dockWidget_korektura->setVisible( true );
		model_korektura->setTable("korektura");
		model_korektura->setEditStrategy(QSqlTableModel::OnRowChange);
			//model_korektura->setRelation(1, QSqlRelation("t_abonent_inf", "id", "abon_inf"));
		model_korektura->setSort(1, Qt::AscendingOrder);
		model_korektura->setHeaderData(1, Qt::Horizontal, codec->toUnicode("Рахунок") );
		model_korektura->setHeaderData(2, Qt::Horizontal, codec->toUnicode("Площа") );
		model_korektura->setHeaderData(3, Qt::Horizontal, codec->toUnicode("Відс.") );
		korektura_date_changed();
		ui.tableView_korektura->setModel(model_korektura);
		ui.tableView_korektura->setColumnHidden(0, true);
		ui.tableView_korektura->setColumnHidden(4, true);
		ui.tableView_korektura->setColumnHidden(5, true);
		ui.tableView_korektura->setColumnHidden(6, true);
		ui.tableView_korektura->setSelectionMode(QAbstractItemView::SingleSelection);
		ui.tableView_korektura->setSelectionBehavior(QAbstractItemView::SelectRows);
		ui.tableView_korektura->resizeColumnsToContents();
		ui.tableView_korektura->setItemDelegate(new UKorektura_Delegate(ui.tableView_korektura));
		
		connect(ui.tableView_korektura->selectionModel(), SIGNAL(currentRowChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(look_abonent_info( const QModelIndex &, const QModelIndex & ))); //позначено запис --> висвітлення інформації про абонента
		connect(ui.tableView_korektura->selectionModel(), SIGNAL(currentColumnChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(activ_column_changes( const QModelIndex &, const QModelIndex &)));
	}
	else{
		ui.dockWidget_korektura->setVisible( false );
		model_korektura->clear();
	}
}
//------------------------------------------------------------
void tke_MainWindow::action_korektura_s_activated()
{
	if (ui.action_korektura_s->isChecked()){
		ui.dockWidget_korektura_s->setVisible( true );
		model_korektura_s->setTable("korektura_s");
		//model_korektura_s->setRelation(1, QSqlRelation("t_abonent_inf", "id", "abon_inf"));
		model_korektura_s->setHeaderData(1, Qt::Horizontal, codec->toUnicode("Рахунок") );
		model_korektura_s->setHeaderData(2, Qt::Horizontal, codec->toUnicode("Сума ЦО") );
		model_korektura_s->setHeaderData(3, Qt::Horizontal, codec->toUnicode("Сума ГВ") );
		model_korektura_s->setHeaderData(4, Qt::Horizontal, codec->toUnicode("Сума СТ") );
		model_korektura_s->setHeaderData(7, Qt::Horizontal, codec->toUnicode("Призначення") );
		korektura_s_date_changed();
		ui.tableView_korektura_s->setModel(model_korektura_s);
		ui.tableView_korektura_s->setColumnHidden(0, true);
		ui.tableView_korektura_s->setColumnHidden(5, true);
		ui.tableView_korektura_s->setColumnHidden(6, true);
		ui.tableView_korektura_s->setSelectionMode(QAbstractItemView::SingleSelection);
		ui.tableView_korektura_s->setSelectionBehavior(QAbstractItemView::SelectRows);
		ui.tableView_korektura_s->setItemDelegate(new UKorektura_s_Delegate(ui.tableView_korektura_s));
		ui.tableView_korektura_s->resizeColumnsToContents();
	
		connect(ui.tableView_korektura_s->selectionModel(), SIGNAL(currentRowChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(look_abonent_info( const QModelIndex &, const QModelIndex & ))); //позначено запис --> висвітлення інформації про абонента
		connect(ui.tableView_korektura_s->selectionModel(), SIGNAL(currentColumnChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(activ_column_changes( const QModelIndex &, const QModelIndex &)));
	}
	else{
		ui.dockWidget_korektura_s->setVisible( false );
		model_korektura_s->clear();
	}
}
//------------------------------------------------------------
void tke_MainWindow::action_print_vidomist_pilg()
{
	tke_Dialog_narah *dialog_pilgy = new tke_Dialog_narah(this, codec->toUnicode("Зведена відомість пільг"), currentSysDate.year(), currentSysDate.month() );
	if (dialog_pilgy->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_vidomist_pilg(dialog_pilgy->get_Year(), dialog_pilgy->get_Month());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete dialog_pilgy;
}
//------------------------------------------------------------
void tke_MainWindow::action_print_vidomist_pilg_kategor()
{
	tke_Dialog_narah *dialog_pilgy = new tke_Dialog_narah(this, codec->toUnicode("Відомість пільговиків по категоріях"), currentSysDate.year(), currentSysDate.month() );
	if (dialog_pilgy->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_vidomist_pilg_kategor(dialog_pilgy->get_Year(), dialog_pilgy->get_Month());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete dialog_pilgy;
}
//------------------------------------------------------------
void tke_MainWindow::action_print_zvit_pilg_kategor()
{
	tke_Dialog_narah *dialog_pilgy = new tke_Dialog_narah(this, codec->toUnicode("Відомість пільговиків по категоріях"), currentSysDate.year(), currentSysDate.month() );
	if (dialog_pilgy->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_zvit_pilg_kategor(dialog_pilgy->get_Year(), dialog_pilgy->get_Month());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete dialog_pilgy;
}
//------------------------------------------------------------
void tke_MainWindow::action_print_vidomist_narah_activated()
{
	tke_Dialog_narah *dialog_narah = new tke_Dialog_narah(this, codec->toUnicode("Зведена відомість пільг"), currentSysDate.year(), currentSysDate.month() );
	if (dialog_narah->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_vidomist_narah(dialog_narah->get_Year(), dialog_narah->get_Month());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete dialog_narah;
}
//------------------------------------------------------------
void tke_MainWindow::action_print_dovidka_oderj_subs_activated()
{
	tke_Dialog_narah *dialog_narah = new tke_Dialog_narah(this, codec->toUnicode("Зведена відомість пільг"), currentSysDate.year(), currentSysDate.month() );
	if (dialog_narah->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_dovidka_oderj_subs(dialog_narah->get_Year(), dialog_narah->get_Month());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete dialog_narah;
}
//------------------------------------------------------------
void tke_MainWindow::action_print_forma_akt_vytrat_teploenergii_activated()
{
	UPrintDocs * printDocs = new UPrintDocs();
	printDocs->print_forma_akt_vytrat_teploenergii();
	connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
	connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
	delete printDocs;
}
//------------------------------------------------------------
void tke_MainWindow::action_print_kvytancii_activated()
{
	tke_Dialog_print_kvytancii *filtr_dialog = new tke_Dialog_print_kvytancii(0, codec->toUnicode("Фільтр друку квитанцій"), currentSysDate.year(), currentSysDate.month());
	if (filtr_dialog->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_kvytancii(filtr_dialog->get_Month(), filtr_dialog->get_Year(), false, filtr_dialog->get_Bud_id());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete filtr_dialog;
}
//------------------------------------------------------------
void tke_MainWindow::action_print_kvytancii_for_abonent_activated()
{
	tke_Dialog_narah *dialog = new tke_Dialog_narah(0, codec->toUnicode("Друк квитанції за місяць:"), currentSysDate.year(), currentSysDate.month());
	if (dialog->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_kvytancii(dialog->get_Month(), dialog->get_Year(), true, ui.spinBox_Rahunok->value());
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete dialog;
}
//------------------------------------------------------------
void tke_MainWindow::action_printCustomKvyt_activated()
{
	tke_form_printCustomKvyt *w = new tke_form_printCustomKvyt(currentSysDate);
	connect(this, SIGNAL(closeLoginSignal()), w, SLOT(mainWindowClosed()));
	connect(this, SIGNAL(mainWindowClosed()), w, SLOT(mainWindowClosed()));
	connect(this, SIGNAL(mainWindowClosed()), w, SLOT(close()));
	connect(this, SIGNAL(closeLoginSignal()), w, SLOT(close()));
	w->show();
}
//------------------------------------------------------------
void tke_MainWindow::action_subs_activated()
{
	if (ui.action_subs->isChecked()){
		ui.dockWidget_subs->setVisible( true );
		model_subs->setTable("subs");
		//model_subs->setRelation(1, QSqlRelation("t_abonent_inf", "id", "abon_inf"));
		//model_subs->setSort(1, Qt::AscendingOrder);
		model_subs->setHeaderData(1, Qt::Horizontal, codec->toUnicode("Рахунок - Особа") );
		model_subs->setHeaderData(2, Qt::Horizontal, codec->toUnicode("Рік") );
		model_subs->setHeaderData(3, Qt::Horizontal, codec->toUnicode("Місяць") );
		model_subs->setHeaderData(4, Qt::Horizontal, codec->toUnicode("Сума ЦО") );
		model_subs->setHeaderData(5, Qt::Horizontal, codec->toUnicode("Сума д. ЦО") );
		model_subs->setHeaderData(6, Qt::Horizontal, codec->toUnicode("Сума ГВ") );
		model_subs->setHeaderData(7, Qt::Horizontal, codec->toUnicode("Сума д. ГВ") );
		subs_date_changed();
		ui.tableView_subs->setModel(model_subs);
		ui.tableView_subs->setColumnHidden(0, true);
		ui.tableView_subs->setColumnHidden(2, true);
		ui.tableView_subs->setColumnHidden(3, true);
		ui.tableView_subs->setSelectionMode(QAbstractItemView::SingleSelection);
		ui.tableView_subs->setSelectionBehavior(QAbstractItemView::SelectRows);
		ui.tableView_subs->setItemDelegate(new USubs_Delegate(ui.tableView_subs));
		ui.tableView_subs->resizeColumnsToContents();
		
		connect(ui.tableView_subs->selectionModel(), SIGNAL(currentRowChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(look_abonent_info( const QModelIndex &, const QModelIndex &))); //позначено запис --> висвітлення інформації про абонента
		connect(ui.tableView_subs->selectionModel(), SIGNAL(currentColumnChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(activ_column_changes( const QModelIndex &, const QModelIndex &)));//змінено стовпець --> висвітлення інформації про абонента
	}
	else{
		ui.dockWidget_subs->setVisible( false );
		model_subs->clear();
	}
}
//------------------------------------------------------------
void tke_MainWindow::action_kvytancii_activated()
{
	if (ui.action_kvytancii->isChecked()){
		ui.dockWidget_kvyt->setVisible( true );
		model_kvytancii->setTable("kvytancii");
		//model_kvytancii->setRelation(1, QSqlRelation("t_abonent_inf", "id", "abon_inf"));
		//model_kvytancii->setSort(1, Qt::AscendingOrder);
		model_kvytancii->setHeaderData(1, Qt::Horizontal, codec->toUnicode("Рахунок"));
		model_kvytancii->setHeaderData(2, Qt::Horizontal, codec->toUnicode("Сума"));
		model_kvytancii->setHeaderData(3, Qt::Horizontal, codec->toUnicode("Дата оплат."));
		model_kvytancii->setHeaderData(4, Qt::Horizontal, codec->toUnicode("Дата перев."));
		model_kvytancii->setHeaderData(5, Qt::Horizontal, codec->toUnicode("№ пакету"));
		model_kvytancii->setHeaderData(8, Qt::Horizontal, codec->toUnicode("Вид опл."));
		//model_kvytancii->setFilter( "year=" + QVariant(date.year()).toString() + 
		//						" and month="  + QVariant(date.month()).toString());
		model_kvytancii->select();
		ui.tableView_kvyt->setModel(model_kvytancii);
		ui.tableView_kvyt->setColumnHidden(0, true);
		ui.tableView_kvyt->setColumnHidden(6, true);
		ui.tableView_kvyt->setColumnHidden(7, true);
		ui.tableView_kvyt->setSelectionMode(QAbstractItemView::SingleSelection);
		ui.tableView_kvyt->setSelectionBehavior(QAbstractItemView::SelectRows);
		ui.tableView_kvyt->setItemDelegate(new UKvyt_Delegate(ui.tableView_kvyt));
		ui.tableView_kvyt->resizeColumnsToContents();
		kvyt_control_sum_changed();
		
		connect(ui.tableView_kvyt->selectionModel(), SIGNAL(currentRowChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(look_abonent_info( const QModelIndex &, const QModelIndex & ))); //позначено запис --> висвітлення інформації про абонента
		connect(ui.tableView_kvyt->selectionModel(), SIGNAL(currentColumnChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(activ_column_changes( const QModelIndex &, const QModelIndex &)));
	}
	else{
		ui.dockWidget_kvyt->setVisible( false );
		model_kvytancii->clear();
	}
}
//------------------------------------------------------------
void tke_MainWindow::action_spravy_subs_activated()
{
	if (ui.action_spravy_subs->isChecked()){
		ui.dockWidget_spravy_subs->setVisible( true );
		model_spravy_subs->setTable("spravy_subs");
		//model_spravy_subs->setRelation(1, QSqlRelation("t_abonent_inf", "id", "abon_inf"));
		//model_spravy_subs->setSort(1, Qt::AscendingOrder);
		model_spravy_subs->setHeaderData(1, Qt::Horizontal, codec->toUnicode("Рахунок - Особа"));
		model_spravy_subs->setHeaderData(2, Qt::Horizontal, codec->toUnicode("№ справи"));
		model_spravy_subs->setHeaderData(3, Qt::Horizontal, codec->toUnicode("Міс.поч."));
		model_spravy_subs->setHeaderData(4, Qt::Horizontal, codec->toUnicode("Рік поч."));
		model_spravy_subs->setHeaderData(5, Qt::Horizontal, codec->toUnicode("Міс.кін."));
		model_spravy_subs->setHeaderData(6, Qt::Horizontal, codec->toUnicode("Рік кін."));
		//model_spravy_subs->setFilter( "year=" + QVariant(date.year()).toString() + 
		//						" and month="  + QVariant(date.month()).toString());
		model_spravy_subs->select();
		ui.tableView_spravy_subs->setModel(model_spravy_subs);
		ui.tableView_spravy_subs->setColumnHidden(0, true);
		ui.tableView_spravy_subs->setSelectionMode(QAbstractItemView::SingleSelection);
		ui.tableView_spravy_subs->setSelectionBehavior(QAbstractItemView::SelectRows);
		ui.tableView_spravy_subs->setItemDelegate(new USpravy_subs_Delegate(ui.tableView_subs));
		ui.tableView_spravy_subs->resizeColumnsToContents();
	
		connect(ui.tableView_spravy_subs->selectionModel(), SIGNAL(currentRowChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(look_abonent_info( const QModelIndex &, const QModelIndex & ))); //позначено запис --> висвітлення інформації про абонента
		connect(ui.tableView_spravy_subs->selectionModel(), SIGNAL(currentColumnChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(activ_column_changes( const QModelIndex &, const QModelIndex &)));
	}
	else{
		ui.dockWidget_spravy_subs->setVisible( false );
		model_spravy_subs->clear();
	}
}
//------------------------------------------------------------
void tke_MainWindow::action_zaborg_activated()
{
	if (ui.action_zaborg->isChecked()){
		ui.dockWidget_splata_zaborg->setVisible( true );
		model_splata_zaborg->setTable("spl_zaborg");
		//model_splata_zaborg->setRelation(1, QSqlRelation("t_abonent_inf", "id", "abon_inf"));
		//model_splata_zaborg->setSort(1, Qt::AscendingOrder);
		model_splata_zaborg->setHeaderData(1, Qt::Horizontal, codec->toUnicode("Рахунок - Особа"));
		model_splata_zaborg->setHeaderData(2, Qt::Horizontal, codec->toUnicode("№ угоди"));
		model_splata_zaborg->setHeaderData(3, Qt::Horizontal, codec->toUnicode("Сума в міс."));
		model_splata_zaborg->setHeaderData(4, Qt::Horizontal, codec->toUnicode("Поч.міс."));
		model_splata_zaborg->setHeaderData(5, Qt::Horizontal, codec->toUnicode("Поч.рік"));
		model_splata_zaborg->setHeaderData(6, Qt::Horizontal, codec->toUnicode("Кін.міс."));
		model_splata_zaborg->setHeaderData(7, Qt::Horizontal, codec->toUnicode("Кін.рік"));
		model_splata_zaborg->select();
		ui.tableView_splata_zaborg->setModel(model_splata_zaborg);
		ui.tableView_splata_zaborg->setColumnHidden(0, true);
		ui.tableView_splata_zaborg->setSelectionMode(QAbstractItemView::SingleSelection);
		ui.tableView_splata_zaborg->setSelectionBehavior(QAbstractItemView::SelectRows);
		ui.tableView_splata_zaborg->setItemDelegate(new USplata_zaborg_Delegate(ui.tableView_splata_zaborg));
		ui.tableView_splata_zaborg->resizeColumnsToContents();
	
		connect(ui.tableView_splata_zaborg->selectionModel(), SIGNAL(currentRowChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(look_abonent_info( const QModelIndex &, const QModelIndex & ))); //позначено запис --> висвітлення інформації про абонента
		connect(ui.tableView_splata_zaborg->selectionModel(), SIGNAL(currentColumnChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(activ_column_changes( const QModelIndex &, const QModelIndex &)));
	}
	else{
		ui.dockWidget_splata_zaborg->setVisible( false );
		model_splata_zaborg->clear();
	}
}
//------------------------------------------------------------
void tke_MainWindow::action_kotelni_activated()
{
	if (ui.action_kotelni->isChecked()){
		ui.dockWidget_kotelni->setVisible( true );
		model_kotelni->setTable("kotelni");
		//model_kotelni->setSort(1, Qt::AscendingOrder);
		model_kotelni->setHeaderData(1, Qt::Horizontal, codec->toUnicode("№ кот.") );
		model_kotelni->setHeaderData(2, Qt::Horizontal, codec->toUnicode("Нас.пункт") );
		model_kotelni->setHeaderData(3, Qt::Horizontal, codec->toUnicode("Вулиця") );
		model_kotelni->setHeaderData(4, Qt::Horizontal, codec->toUnicode("№ буд.") );
		model_kotelni->setHeaderData(5, Qt::Horizontal, codec->toUnicode("Тел.") );
		model_kotelni->select();
		ui.tableView_kotelni->setModel(model_kotelni);
		ui.tableView_kotelni->setColumnHidden(0, true);
		ui.tableView_kotelni->setSelectionMode(QAbstractItemView::SingleSelection);
		ui.tableView_kotelni->setSelectionBehavior(QAbstractItemView::SelectRows);
		//ui.tableView_normat_taryf->setItemDelegate(new UNormat_taryf_Delegate(ui.tableView_kotelni));
		ui.tableView_kotelni->resizeColumnsToContents();
	}
	else{
		ui.dockWidget_kotelni->setVisible( false );
		model_kotelni->clear();
	}
}
//------------------------------------------------------------
void tke_MainWindow::action_pilgovyky_activated()
{
	if (ui.action_pilgovyky->isChecked()){
		ui.dockWidget_pilgovyky->setVisible( true );
		model_pilgovyky->setTable("pilgovyky");
		//model_pilgovyky->setRelation(1, QSqlRelation("t_abonent_inf", "id", "abon_inf"));
		//model_pilgovyky->setRelation(6, QSqlRelation("t_pilgy_inf", "id", "Kategoriya"));
		//model_pilgovyky->setSort(1, Qt::AscendingOrder);
		model_pilgovyky->setHeaderData(1, Qt::Horizontal, codec->toUnicode("Рахунок - Особа"));
		model_pilgovyky->setHeaderData(2, Qt::Horizontal, codec->toUnicode("Прізвище"));
		model_pilgovyky->setHeaderData(3, Qt::Horizontal, codec->toUnicode("Ім'я"));
		model_pilgovyky->setHeaderData(4, Qt::Horizontal, codec->toUnicode("По-батькові"));
		model_pilgovyky->setHeaderData(5, Qt::Horizontal, codec->toUnicode("Ідент.код"));
		model_pilgovyky->setHeaderData(6, Qt::Horizontal, codec->toUnicode("Катег.пільги"));
		model_pilgovyky->setHeaderData(7, Qt::Horizontal, codec->toUnicode("Серія Посв."));
		model_pilgovyky->setHeaderData(8, Qt::Horizontal, codec->toUnicode("Номер Посв."));
		model_pilgovyky->setHeaderData(9, Qt::Horizontal, codec->toUnicode("Група"));
		model_pilgovyky->select();
		ui.tableView_pilgovyky->setModel(model_pilgovyky);
		ui.tableView_pilgovyky->setColumnHidden(0, true);
		ui.tableView_pilgovyky->setSelectionMode(QAbstractItemView::SingleSelection);
		ui.tableView_pilgovyky->setSelectionBehavior(QAbstractItemView::SelectRows);
		ui.tableView_pilgovyky->setItemDelegate(new UPilgovyky_Delegate(ui.tableView_pilgovyky));
		ui.tableView_pilgovyky->resizeColumnsToContents();
		pilgovyky_control_sum_changed();
		
		connect(ui.tableView_pilgovyky->selectionModel(), SIGNAL(currentRowChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(look_abonent_info( const QModelIndex &, const QModelIndex & ))); //позначено запис --> висвітлення інформації про абонента
		connect(ui.tableView_pilgovyky->selectionModel(), SIGNAL(currentColumnChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(activ_column_changes( const QModelIndex &, const QModelIndex &)));
	}
	else{
		ui.dockWidget_pilgovyky->setVisible( false );
		model_pilgovyky->clear();
	}
	
}
//------------------------------------------------------------
void tke_MainWindow::action_opSubs_activated()
{
	if (ui.action_opSubs->isChecked()){
		ui.dateEdit_opSubsFilter->setDate(currentSysDate);
		
		ui.tableWidget_opSubs->setColumnCount(4);
		QStringList hl;
		hl << "id" << "Рахунок" << "ОП ЦО" << "ОП ГВ";
		ui.tableWidget_opSubs->setHorizontalHeaderLabels(hl);
		
		opTwExt = new UTableWidgetSqlExt(ui.tableWidget_opSubs, "plataZgidnoSubs");
		opTwExt->setSortingEnabled(true);
		opTwExt->colNamesList << "Id"
						<< "Rahunok_ID"
						<< "Plata_CO"
						<< "Plata_GV";
		opTwExt->setItemDelegateForColumn("Rahunok_ID", new USpinBoxDelegate(0,1000000,ui.tableWidget_opSubs));
		opTwExt->setItemDelegateForColumn("Plata_CO", new UDoubleSpinBoxDelegate(0,1000000,2,ui.tableWidget_opSubs));
		opTwExt->setItemDelegateForColumn("Plata_GV", new UDoubleSpinBoxDelegate(0,1000000,2,ui.tableWidget_opSubs));
		
		ui.tableWidget_opSubs->setColumnHidden(0, true);
		opTwExt->setColumnWidth("Rahunok_ID",80);
		opTwExt->setColumnWidth("Plata_CO",80);
		opTwExt->setColumnWidth("Plata_GV",80);
		setOpDateFilter();
		
		connect(ui.pushButton_addOpString, SIGNAL(clicked()), this, SLOT(pushButton_addOpString_clicked()));
		connect(ui.pushButton_delOpString, SIGNAL(clicked()), this, SLOT(pushButton_delOpString_clicked()));
		connect(ui.dateEdit_opSubsFilter, SIGNAL(dateChanged(const QDate &)), this, SLOT(setOpDateFilter()));
		connect(ui.pushButton_reload, SIGNAL(clicked()), opTwExt,SLOT(populateTable()));
		
		ui.dockWidget_opSubs->setVisible( true );
	}
	else{
		disconnect(ui.pushButton_addOpString, SIGNAL(clicked()), this, SLOT(pushButton_addOpString_clicked()));
		disconnect(ui.pushButton_delOpString, SIGNAL(clicked()), this, SLOT(pushButton_delOpString_clicked()));
		disconnect(ui.dateEdit_opSubsFilter, SIGNAL(dateChanged(const QDate &)), this, SLOT(setOpDateFilter()));
		disconnect(ui.pushButton_reload, SIGNAL(clicked()), opTwExt,SLOT(populateTable()));
	
		ui.dockWidget_opSubs->setVisible( false );
		ui.tableWidget_opSubs->setRowCount(0);
		delete opTwExt;
	}
}
//------------------------------------------------------------
void tke_MainWindow::action_narahPereved_activated()
{
	if (ui.action_narahPereved->isChecked()){
		ui.dateEdit_narahPereved_Filter->setDate(currentSysDate);
		
		QStringList hl;
		hl << "id" << "Рахунок" << "Дата" << "Сума" << "Тип перев." << "Примітка";
		ui.tableWidget_narahPereved->setColumnCount(hl.size());
		ui.tableWidget_narahPereved->setHorizontalHeaderLabels(hl);
		
		perevedTwExt = new UTableWidgetSqlExt(ui.tableWidget_narahPereved, "narahuvannyaPereved");
		perevedTwExt->setSortingEnabled(true);
		perevedTwExt->colNamesList << "Id"
						<< "Rahunok_ID"
						<< "CDate"
						<< "Suma"
						<< "PerevedType_id"
						<< "Prymitka";
		perevedTwExt->setItemDelegateForColumn("Rahunok_ID", new USpinBoxDelegate(0,1000000,ui.tableWidget_narahPereved));
		ui.tableWidget_narahPereved->setItemDelegateForColumn(perevedTwExt->columnNumber("CDate"), new UDateTimeDelegate(ui.tableWidget_narahPereved, "dd.MM.yyyy"));
		perevedTwExt->setItemDelegateForColumn("Suma", new UDoubleSpinBoxDelegate(-1000000,1000000,2,ui.tableWidget_narahPereved));
		//Делегат типу переведення
		QHash<int, QString> delegateHash;
		delegateHash.clear();
		delegateHash.insert(0, "");
		QSqlQuery query;
		query.exec("SELECT id, Name, BuhRahunok FROM perevedType ORDER BY Name, BuhRahunok");
		while (query.next()){
			delegateHash.insert(query.value(0).toInt(), query.value(1).toString()+" /"+query.value(2).toString()+"/");
		}
		perevedTwExt->setItemDelegateForColumn("PerevedType_id", new UHashComboDelegate(ui.tableWidget_narahPereved, delegateHash));
		
		perevedTwExt->addColAlignment("PerevedType_id", Qt::AlignLeft | Qt::AlignVCenter);
		perevedTwExt->addColAlignment("Prymitka", Qt::AlignLeft | Qt::AlignVCenter);
		
		ui.tableWidget_narahPereved->setColumnHidden(0, true);
		ui.tableWidget_narahPereved->setColumnHidden(2, true);
		perevedTwExt->setColumnWidth("Rahunok_ID",80);
		perevedTwExt->setColumnWidth("CDate",80);
		perevedTwExt->setColumnWidth("Suma",100);
		perevedTwExt->setColumnWidth("PerevedType_id",150);
		perevedTwExt->setColumnWidth("Prymitka",130);
		setNarahPerevedDateFilter();
		
		connect(ui.pushButton_narahPereved_addString, SIGNAL(clicked()), this, SLOT(pushButton_narahPereved_addString_clicked()));
		connect(ui.pushButton_narahPereved_delString, SIGNAL(clicked()), this, SLOT(pushButton_narahPereved_delString_clicked()));
		connect(ui.dateEdit_narahPereved_Filter, SIGNAL(dateChanged(const QDate &)), this, SLOT(setNarahPerevedDateFilter()));
		connect(ui.pushButton_narahPereved_reload, SIGNAL(clicked()), perevedTwExt,SLOT(populateTable()));
		
		ui.dockWidget_narahPereved->setVisible( true );
	}
	else{
		disconnect(ui.pushButton_narahPereved_addString, SIGNAL(clicked()), this, SLOT(pushButton_narahPereved_addString_clicked()));
		disconnect(ui.pushButton_narahPereved_delString, SIGNAL(clicked()), this, SLOT(pushButton_narahPereved_delString_clicked()));
		disconnect(ui.dateEdit_narahPereved_Filter, SIGNAL(dateChanged(const QDate &)), this, SLOT(setNarahPerevedDateFilter()));
		disconnect(ui.pushButton_narahPereved_reload, SIGNAL(clicked()), perevedTwExt,SLOT(populateTable()));
		
		ui.dockWidget_narahPereved->setVisible( false );
		ui.tableWidget_opSubs->setRowCount(0);
		delete perevedTwExt;
	}
}
//------------------------------------------------------------
void tke_MainWindow::action_print_rahunok_abonenta_activated()
{
	int year = date.year();
	int month = date.month();
	UPrintForm * printform = new UPrintForm(0, codec->toUnicode("Абонентська відомість"));
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat;
	//Створення документа нарахування оплати за ЦО за X місяць, Y рік
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 7 );
	cursor.insertText("Сформовано ПК \"ТКЕнерго менеджер\" (супровід Компанія \"УТЕХ\". веб: www.utech.in.ua). Час формування: "+QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy")+"р." , textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	blockFormat.setAlignment( Qt::AlignLeft );
	textCharFormat.setFontPointSize( 9 );
	cursor.insertText(codec->toUnicode("Рахунок ") + QVariant(ui.spinBox_Rahunok->value()).toString(), textCharFormat);
	
		//iнформація про абонента
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 9 );
	cursor.insertText(codec->toUnicode("1. ПІБ - ") + ui.lineEdit_Prizv->text() + " "
							+ ui.comboBox_Imya->currentText() + " "
							+ ui.comboBox_Batk->currentText() + ".", textCharFormat);
	cursor.insertText(codec->toUnicode("  Адреса - ") + ui.comboBox_Budynok->currentText(), textCharFormat);
	if (ui.spinBox_Kvartyra->value() != 0)
		cursor.insertText(codec->toUnicode(" кв. ") + QVariant(ui.spinBox_Kvartyra->value()).toString() + ".", textCharFormat);
	else
		cursor.insertText( ".", textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("2. Площа - ") + uMToStr2(ui.doubleSpinBox_Ploshcha->value()) 
						+ codec->toUnicode(" м.кв."), textCharFormat);	
	cursor.insertText(codec->toUnicode("  Проживає - ") + QVariant(ui.spinBox_Ludy->value()).toString() 
						+ codec->toUnicode(" чол."), textCharFormat);
	cursor.insertText(codec->toUnicode(".  Однокімнатна кв. - "), textCharFormat);
	if (ui.spinBox_Kimn->value() == 0)
		cursor.insertText(codec->toUnicode("так."), textCharFormat);
	else
		cursor.insertText(codec->toUnicode("ні."), textCharFormat);
	cursor.insertText(codec->toUnicode(".  Зайва площа - ") + uMToStr2(ui.doubleSpinBox_Zaiva_Ploshcha->value())
						+ codec->toUnicode(" м.кв."), textCharFormat);
	QSqlQuery *abonent_temp_query = new QSqlQuery();
	abonent_temp_query->clear();
	abonent_temp_query->exec("SELECT vart_g_kal, normat_taryf_kvadr FROM normat_taryf \
								WHERE (year=(SELECT max(year) FROM (SELECT * FROM normat_taryf WHERE year<="
								+ QVariant(year).toString() +"))) \
								and ( month =  (SELECT max(month) FROM \
								(SELECT * FROM normat_taryf WHERE (year="+ QVariant(year).toString() +" \
								and month<="+ QVariant(month).toString() +") or (year<"+ QVariant(year).toString() +") ) \
								WHERE year=(SELECT max(year) FROM (SELECT * FROM normat_taryf WHERE year<="+ QVariant(year).toString() +")) ))" );
	abonent_temp_query->seek(0);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("3. Тариф за м.кв. - ") 
							+ uMToStr2(abonent_temp_query->value(1).toDouble()) 
							+ codec->toUnicode("грн."), textCharFormat);
	
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("4. Пільги:"), textCharFormat);
	if (ui.comboBox_Kategoria_Pilgy->currentIndex() > 0){
		cursor.insertBlock(blockFormat);
		cursor.insertText(codec->toUnicode("		а) ") + ui.comboBox_Kategoria_Pilgy->currentText()
									+ codec->toUnicode(" для ") + QVariant(ui.spinBox_Pilga_for->value()).toString()
									+ codec->toUnicode(" чол."), textCharFormat);
		if (ui.comboBox_Kategoria_Pilgy_2->currentIndex() > 0){
			cursor.insertBlock(blockFormat);
			cursor.insertText(codec->toUnicode("		б) ") + ui.comboBox_Kategoria_Pilgy_2->currentText()
									+ codec->toUnicode(" для ") + QVariant(ui.spinBox_Pilga_for_2->value()).toString()
									+ codec->toUnicode(" чол."), textCharFormat);
		}
	}
	else
		cursor.insertText(codec->toUnicode(" --- "), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("Операції за "+ui.spinBox_year_filtr->text()+" рік", textCharFormat);
	
	
	//Читання з іні-файлу необхідності використання ГВ і ЦО
	bool boolCO, boolGV, boolST, boolPerevZaborg;
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	boolCO = settings->value("GUI_View/ShowNarahCO").toBool();
	boolGV = settings->value("GUI_View/ShowNarahGV").toBool();
	boolST = settings->value("GUI_View/ShowNarahST").toBool();
	boolPerevZaborg = settings->value("GUI_View/ShowPerevedZaborg").toBool();
	delete settings;
	
		//створення і налаштування таблиці
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
	constraints << QTextLength(QTextLength::FixedLength, 20);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(2);
	int rowCount=4;
	if (boolCO)
		rowCount+=4;
	if (boolGV)
		rowCount+=4;
	if (boolST)
		rowCount+=1;
	if (boolPerevZaborg)
		rowCount+=1;
	QTextTable *table = cursor.insertTable(13, rowCount, tableFormat);
	
	int iter=0;
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
		//заповнення шапки
	cell = table->cellAt(0, iter++);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Міс."), textCharFormat);
	
	cell = table->cellAt(0, iter++);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Вх.сальдо, грн."), textCharFormat);
	
	if (boolCO){
		cell = table->cellAt(0, iter++);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Нарах. ЦО, грн."), textCharFormat);
		
		cell = table->cellAt(0, iter++);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Пільга. ЦО, грн."), textCharFormat);
	}
	
	if (boolGV){
		cell = table->cellAt(0, iter++);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Нарах. ГВ, грн."), textCharFormat);
		
		cell = table->cellAt(0, iter++);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Пільга. ГВ, грн."), textCharFormat);
	}
	
	if (boolST){
		cell = table->cellAt(0, iter++);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Нарах. СТ, грн."), textCharFormat);
	}
	
	if (boolCO){
		cell = table->cellAt(0, iter++);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Субсидія ЦО, грн."), textCharFormat);
	
		cell = table->cellAt(0, iter++);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Д.Субсидії ЦО, грн."), textCharFormat);
	}
	
	if (boolGV){
		cell = table->cellAt(0, iter++);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Субсидія ГВ, грн."), textCharFormat);
	
		cell = table->cellAt(0, iter++);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Д.Субсидії ГВ, грн."), textCharFormat);
	}
	
	cell = table->cellAt(0, iter++);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Оплачено, грн."), textCharFormat);
	
	if (boolPerevZaborg){
		cell = table->cellAt(0, iter++);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Перев. заборг., грн."), textCharFormat);
	}
	
	cell = table->cellAt(0, iter++);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Вих.сальдо, грн."), textCharFormat);
	
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignTop);
		//заповнення комірок таблиці
	for (int i=0; i< 12; i++){
		iter=0;
			//№ п/п
		cell = table->cellAt(i+1, iter++);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QVariant(i+1).toString(), textCharFormat);
			//Вх.сальдо
		cell = table->cellAt(i+1, iter++);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(ui.tableWidget_rozrahunky->item(i, 0)->text(), textCharFormat);
			//Нараховано за ЦО
		if (boolCO){
			cell = table->cellAt(i+1, iter++);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(ui.tableWidget_rozrahunky->item(i, 1)->text(), textCharFormat);
			
			cell = table->cellAt(i+1, iter++);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(ui.tableWidget_rozrahunky->item(i, 2)->text(), textCharFormat);
		}
		
			//Нараховано за ГВ
		if (boolGV){
			cell = table->cellAt(i+1, iter++);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(ui.tableWidget_rozrahunky->item(i, 3)->text(), textCharFormat);
			
			cell = table->cellAt(i+1, iter++);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(ui.tableWidget_rozrahunky->item(i, 4)->text(), textCharFormat);
		}
		
			//Нараховано за СТ
		if (boolST){
			cell = table->cellAt(i+1, iter++);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(ui.tableWidget_rozrahunky->item(i, 5)->text(), textCharFormat);
		}
		
		double sum_subs=0;
		if (boolCO){
			//Субсидія ЦО
			cell = table->cellAt(i+1, iter++);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			sum_subs = ui.tableWidget_rozrahunky->item(i, 6)->text().toDouble();
			if (sum_subs != 0){
				cellCursor.insertText(uMToStr2(sum_subs), textCharFormat);
			}
			else
				cellCursor.insertText("-", textCharFormat);
				//Д.Субсидія ЦО
			cell = table->cellAt(i+1, iter++);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			sum_subs = ui.tableWidget_rozrahunky->item(i, 7)->text().toDouble();
			if (sum_subs != 0){
				cellCursor.insertText(uMToStr2(sum_subs), textCharFormat);
			}
			else
				cellCursor.insertText("-", textCharFormat);
		}
		
		if (boolGV){
			//Субсидія ГВ
			cell = table->cellAt(i+1, iter++);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			sum_subs = ui.tableWidget_rozrahunky->item(i, 8)->text().toDouble();
			if (sum_subs != 0){
				cellCursor.insertText(uMToStr2(sum_subs), textCharFormat);
			}
			else
				cellCursor.insertText("-", textCharFormat);
				//Д.Субсидія ГВ
			cell = table->cellAt(i+1, iter++);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			sum_subs = ui.tableWidget_rozrahunky->item(i, 9)->text().toDouble();
			if (sum_subs != 0){
				cellCursor.insertText(uMToStr2(sum_subs), textCharFormat);
			}
			else
				cellCursor.insertText("-", textCharFormat);
		}
		
			//Оплачено
		cell = table->cellAt(i+1, iter++);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(ui.tableWidget_rozrahunky->item(i, 10)->text(), textCharFormat);
		
		if (boolPerevZaborg){
			//Перевед. заборг.
			cell = table->cellAt(i+1, iter++);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			double sum_pereved = ui.tableWidget_rozrahunky->item(i, 15)->text().toDouble();
			if (sum_subs != 0){
				cellCursor.insertText(uMToStr2(sum_pereved), textCharFormat);
			}
			else
				cellCursor.insertText("-", textCharFormat);
		}
		
			//Вих. сальдо
		cell = table->cellAt(i+1, iter++);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(ui.tableWidget_rozrahunky->item(i, 16)->text(), textCharFormat);
	}
	printform->show();
	connect(this, SIGNAL(closeLoginSignal()), printform, SLOT(close()));
	connect(this, SIGNAL(mainWindowClosed()), printform, SLOT(close()));
}
//------------------------------------------------------------
void tke_MainWindow::action_pererahunok_saldo_activated()
{
	tke_Dialog_narah *dialog_saldo = new tke_Dialog_narah(this, codec->toUnicode("Перерахунок вхідних сальдо починаючи з дати:"), currentSysDate.year(), 1 );
	if (dialog_saldo->exec() == 1){
		int year = dialog_saldo->get_Year();
		int month = dialog_saldo->get_Month();
		delete dialog_saldo;
		___pererah_saldo(month, year, false);
	}
	populate_operacii_table();
}
//------------------------------------------------------------
void tke_MainWindow::action_pererahunok_saldo_all_activated()
{
	tke_Dialog_narah *dialog_saldo = new tke_Dialog_narah(this, codec->toUnicode("Перерахунок вхідних сальдо починаючи з дати:"), currentSysDate.year(), 1 );
	if (dialog_saldo->exec() == 1){
		int year = dialog_saldo->get_Year();
		int month = dialog_saldo->get_Month();
		delete dialog_saldo;
		___pererah_saldo(month, year, true);
	}
	populate_operacii_table();
}
//------------------------------------------------------------
void tke_MainWindow::action_save_vidomist_pilg_to_file_activated()
{
	tke_Dialog_narah *dialog_narah = new tke_Dialog_narah(this, codec->toUnicode("Вибір місяця формування пільг"), currentSysDate.year(), currentSysDate.month());
	if (dialog_narah->exec() == 1){
		QString dir_name = QFileDialog::getExistingDirectory ( this, codec->toUnicode("Вибір папки для запису файла пільг"));
		if (dir_name.size()>0){
			UExportInfoPilgovyky *ep = new UExportInfoPilgovyky(dir_name, dialog_narah->get_Year(), dialog_narah->get_Month());
			ep->save_to_file();
		}
	}
	delete dialog_narah;
}
//------------------------------------------------------------
void tke_MainWindow::action_insert_dbf_kvyt_oshchadbank_activated()
{
	QString dir_name = QFileDialog::getExistingDirectory ( this, codec->toUnicode("Вибір папки з архівами квитанцій"));
	if (dir_name.size()>0){
		tke_Form_insert_kvyt_dbf_ob * form_ob_kvyt = new tke_Form_insert_kvyt_dbf_ob(0, dir_name);
		form_ob_kvyt->show();
		connect(this, SIGNAL(mainWindowClosed()), form_ob_kvyt, SLOT(close()));
		connect(form_ob_kvyt, SIGNAL(u_abonent_rahunok_changes(int)), this, SLOT(show_rahunok(int)));
	}
}
//------------------------------------------------------------
void tke_MainWindow::action_set_data_sources_activated()
{
	//Налаштування джерел даних dbf
	if (setTkeDbfSettings()){
		QMessageBox::information(this,
				codec->toUnicode("!!! Налаштування джерел даних !!!"),
				codec->toUnicode("Успішно закінчено налаштування джерел даних dbf."),
				QMessageBox::Ok,
				QMessageBox::NoButton);
	}
	else{
		QMessageBox::critical(
            this,
            codec->toUnicode("!!! Помилка налаштування джерел даних !!!"),
            codec->toUnicode("Програмі не вдалося налаштувати джерела даних dbf на цій машині"),
            QMessageBox::Ok,
            QMessageBox::NoButton,
			QMessageBox::NoButton);
	}
}
//------------------------------------------------------------
void tke_MainWindow::action_print_borjnyky_activated()
{
	tke_Dialog_filtr_borjnykiv * filtrBorjnykiv = new tke_Dialog_filtr_borjnykiv();
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT max(year), max(month) \
					FROM (SELECT year, month FROM narahuvannya WHERE year=(SELECT max(year) FROM narahuvannya))");
	query->seek(0);
	filtrBorjnykiv->setKincDate(QDate(query->value(0).toInt(),query->value(1).toInt(),1));
	query->exec("SELECT min(year), min(month) \
					FROM (SELECT year, month FROM narahuvannya WHERE year=(SELECT min(year) FROM narahuvannya))");
	query->seek(0);
	filtrBorjnykiv->setPochDate(QDate(query->value(0).toInt(),query->value(1).toInt(),1));
	delete query;
	if (filtrBorjnykiv->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_vidomist_borjnykiv(filtrBorjnykiv->getPochDate(), filtrBorjnykiv->getKincDate(), 
					filtrBorjnykiv->minBorg(), filtrBorjnykiv->get_Bud_id(), filtrBorjnykiv->abonType());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete filtrBorjnykiv;
}
//------------------------------------------------------------
void tke_MainWindow::action_debuger_form_activated()
{
	emit requestDebugWindow();
}
//------------------------------------------------------------
void tke_MainWindow::action_archiv_vidomosti_kvytanciy_ob_activated()
{
	tke_form_kvytancii_doc *form_kvytancii_doc = new tke_form_kvytancii_doc();
	form_kvytancii_doc->show();
	
	connect(this, SIGNAL(closeLoginSignal()), form_kvytancii_doc, SLOT(close()));
	connect(this, SIGNAL(mainWindowClosed()), form_kvytancii_doc, SLOT(close()));
}
//------------------------------------------------------------
void tke_MainWindow::action_print_vidomist_borjnykiv_riznyc_activated()
{
	tke_Dialog_filtr_borjnykiv * filtrBorjnykiv = new tke_Dialog_filtr_borjnykiv();
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT max(year), max(month) \
					FROM (SELECT year, month FROM narahuvannya WHERE year=(SELECT max(year) FROM narahuvannya))");
	query->seek(0);
	filtrBorjnykiv->setKincDate(QDate(query->value(0).toInt(),query->value(1).toInt(),1));
	query->exec("SELECT min(year), min(month) \
					FROM (SELECT year, month FROM narahuvannya WHERE year=(SELECT min(year) FROM narahuvannya))");
	query->seek(0);
	filtrBorjnykiv->setPochDate(QDate(query->value(0).toInt(),query->value(1).toInt(),1));
	delete query;

	if (filtrBorjnykiv->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_vidomist_borjnykiv_riznyc(filtrBorjnykiv->getPochDate(), filtrBorjnykiv->getKincDate(), filtrBorjnykiv->minBorg(), filtrBorjnykiv->get_Bud_id());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete filtrBorjnykiv;
}
//------------------------------------------------------------
void tke_MainWindow::action_print_wrong_subs()
{
	tke_Dialog_narah *dialogWrongSubsDate = new tke_Dialog_narah(this, "Відомість помилково нарахованих субсидій", currentSysDate.year(), currentSysDate.month() );
	if (dialogWrongSubsDate->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_vidomist_wrong_subs(dialogWrongSubsDate->get_Year(), dialogWrongSubsDate->get_Month());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete dialogWrongSubsDate;
}
//------------------------------------------------------------
void tke_MainWindow::action_print_dovidnyk_abonentiv_activated()
{
	UPrintDocs * printDocs = new UPrintDocs();
	printDocs->print_dovidnyk_abonentiv();
	connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
	connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
	delete printDocs;
}
//------------------------------------------------------------
void tke_MainWindow::action_dovidnykAbonentivVRozriziNadannyaPoslug_activated()
{
	tke_Dialog_print_kvytancii d(0, "Вибір будинка", currentSysDate.year(), currentSysDate.month(), true);
	d.setDateHidden(true);
	if (d.exec()) {
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_dovidnykAbonentivVRozriziNadannyaPoslug(d.get_Bud_id());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
}
//------------------------------------------------------------
void tke_MainWindow::action_insert_subs_dbf_activated()
{
	QString appPath = QCoreApplication::applicationDirPath();
	QString fileName = QFileDialog::getOpenFileName(
                    this,
                    codec->toUnicode("Вибір файлу-dbf з нарахуванням субсидій"),
                    "",
                    codec->toUnicode("Файли dBase (*.dbf)"));
	if (fileName.size()>0){
		QFileInfo subsFileInfo(fileName);
		QString odbcFileName = appPath + "/subs_dbf/" + subsFileInfo.fileName();
		if (QFile::exists(odbcFileName))
			QFile::remove(odbcFileName);
		if (QFile::copy(fileName, odbcFileName)){
			QSqlDatabase db = QSqlDatabase::addDatabase("QODBC", "insert_subs");
			db.setDatabaseName("tke_subs_dbf.dsn");
			if (db.open()){
				QSqlQuery *subsReadQuery = new QSqlQuery(db);
			
				subsReadQuery->exec("SELECT count(*), sum(subs) FROM " + subsFileInfo.baseName());
				subsReadQuery->next();
				int monthSubs = subsFileInfo.baseName().mid(4,2).toInt();
				int yearSubs = ("20" + subsFileInfo.baseName().mid(6,2)).toInt();
				int subsRowCount = subsReadQuery->value(0).toInt();
				if ( 0 == QMessageBox::question( this, 
								codec->toUnicode("??? Підтвердження внесення субсидій ???"), 
								codec->toUnicode("У вибраному Вами файлі знаходиться нарахування субсидії \n   за ") 
								+ QVariant(monthSubs).toString() +
								'.' + QVariant(yearSubs).toString() + codec->toUnicode("р.\n") +
								codec->toUnicode("Кількість записів - ") +
								QVariant(subsRowCount).toString() + '\n' +
								codec->toUnicode("Загальна сума нарахування субсидії - ") +
								uMToStr2(subsReadQuery->value(1).toDouble()) + codec->toUnicode("грн."), 
								codec->toUnicode("Втягнути"), 
								codec->toUnicode("Вихід"), 
								"", 0, 1 )){
					UPostupForm *postup_form = new UPostupForm(0, subsRowCount );
					postup_form->show();
					QSqlQuery *subsInsertQuery = new QSqlQuery();
					
					bool q_ok=true;
					QSqlDatabase::database().transaction();
					subsReadQuery->exec("SELECT RASH, NUMB, DAT1, DAT2, SUBS FROM " + subsFileInfo.baseName());
					while (subsReadQuery->next()){
						subsInsertQuery->prepare("INSERT INTO subs (id, Rahunok_ID, Year, Month, Suma) "
										"VALUES (:id, :Rahunok_ID, :Year, :Month, :Suma)");
						subsInsertQuery->bindValue(":id", nextLichylnyk("subs"));
						subsInsertQuery->bindValue(":Rahunok_ID", subsReadQuery->value(0));
						subsInsertQuery->bindValue(":Year", yearSubs);
						subsInsertQuery->bindValue(":Month", monthSubs);
						subsInsertQuery->bindValue(":Suma", subsReadQuery->value(4));
						q_ok = q_ok && subsInsertQuery->exec();

						postup_form->inc_pos();
					}
					if (q_ok){
						q_ok = QSqlDatabase::database().commit();
					}
					else{
						QSqlDatabase::database().rollback();
					}
					if ( !q_ok){
						QMessageBox::critical(
							this,
							codec->toUnicode("!!! Помилка запису арахування субсидій !!!"),
							codec->toUnicode("Програмі не вдалося записати нарахування субсидій."),
							QMessageBox::Ok,
							QMessageBox::NoButton,
							QMessageBox::NoButton);	
					}
					else{
						QMessageBox::information(
							this,
							codec->toUnicode("!!! Закінчено внесення субсидій !!!"),
							codec->toUnicode("Програма успішно втягнула нарахування субсидій з dbf-файла."),
							QMessageBox::Ok,
							QMessageBox::NoButton,
							QMessageBox::NoButton);	
					}
					delete subsInsertQuery;
					delete postup_form;
				}
				delete subsReadQuery;
				QSqlDatabase::removeDatabase( "insert_subs" );
			}
			else{
				QMessageBox::critical(
					this,
					codec->toUnicode("!!! Помилка підєднання до dbf файла !!!"),
					codec->toUnicode("Не вдалося скопіювати файл")+
					codec->toUnicode("   Порада - Закрийте усі програми, які працюють з dbf-кою субсидій")+
					codec->toUnicode("   Порада - Переналаштуйте джерела даних програми (Меню\"Налаштування\"->Джерела даних)"),
					QMessageBox::Ok,
					QMessageBox::NoButton,
					QMessageBox::NoButton);
			}
		}
		else{
			QMessageBox::critical(
				this,
				codec->toUnicode("!!! Помилка копіювання файла !!!"),
				codec->toUnicode("Не вдалося скопіювати файл")+
				codec->toUnicode("Порада - Закрийте усі програми, які працюють з dbf-кою субсидій"),
				QMessageBox::Ok,
				QMessageBox::NoButton,
				QMessageBox::NoButton);
		}
	}
}
//------------------------------------------------------------
void tke_MainWindow::action_diuch_taryf_obedn_budynky_activated()
{
	QModelIndexList *modelIndexList = new QModelIndexList();
	*modelIndexList = ui.tableWidget_diuch_taryf->selectionModel()->selectedRows();
	double sumPloshcha=0;
	bool done_ok = true;
	for (int j=0; j<modelIndexList->size(); j++){
		int i=modelIndexList->at(j).row();
		sumPloshcha += ui.tableWidget_diuch_taryf->item( i, 7)->text().toDouble();
		if (ui.tableWidget_diuch_taryf->item( i, 3)->text() == "-")
			done_ok = false;
	}
	
	if (done_ok){
		tke_Dialog_pokaznyky_lichylnyka *lichDialog = new tke_Dialog_pokaznyky_lichylnyka();
		if (lichDialog->exec() == 1)
			for (int j=0; j<modelIndexList->size(); j++){
				int i=modelIndexList->at(j).row();
				ui.tableWidget_diuch_taryf->item( i, 7)->setText(uMToStr2(sumPloshcha));
				ui.tableWidget_diuch_taryf->item( i, 3)->setText(uMToStr2(lichDialog->pochZnachLich()));
				ui.tableWidget_diuch_taryf->item( i, 4)->setText(uMToStr2(lichDialog->kincZnachLich()));
			}
		delete lichDialog;
	}
	else{
		QMessageBox::critical(this,
			codec->toUnicode("!!! Невірне позначення !!!"),
			codec->toUnicode("Ви позначили для об'єднання будинки, один з яких не має лічильника.\nСпробуйте ще раз."),
			QMessageBox::Ok,
			QMessageBox::NoButton);
	}
	delete modelIndexList;
}
//------------------------------------------------------------
void tke_MainWindow::action_sqlBrowser_activated()
{
	Browser* browser = new Browser(0,0);
	browser->setRoot(IRC);	
	browser->show();
	
	connect(this, SIGNAL(mainWindowClosed()), browser, SLOT(close()));
	connect(this, SIGNAL(closeLoginSignal()), browser, SLOT(close()));
}
//------------------------------------------------------------
void tke_MainWindow::action_posada_activated()
{
#ifndef TKE_DEMO
	PosadaForm* posadaForm  = new PosadaForm();
	posadaForm->setAttribute(Qt::WA_DeleteOnClose);
	posadaForm->setWindowModality(Qt::ApplicationModal);
	posadaForm->show();
#else
	QMessageBox::information(this,
				"!!! Демо-версія !!!",
				"З метою економії розміру у демо-версії не доступна ця функція.",
				QMessageBox::Ok,
				QMessageBox::NoButton);
#endif
}
//------------------------------------------------------------
void tke_MainWindow::action_assistent_activated()
{
	/*QStringList list;
	list << "-profile" << "../docs/tkeAssistant.adp";
	if (assistant == NULL){
		assistant = new QAssistantClient("./", this);
		assistant->setArguments(list);
		connect(this, SIGNAL(mainWindowClosed()), assistant, SLOT(closeAssistant()));
	}
	assistant->openAssistant();*/
	QProcess *process = new QProcess(this);
	QString app = QCoreApplication::applicationDirPath() + "./assistant";

	process->start(app, QStringList() << QLatin1String("-enableRemoteControl"));
	if (!process->waitForStarted()) {
		QMessageBox::critical(this, "Помилка відкривання","Не можу відкрити програму "+app+".");
             return;
	}
	// show index page
	QTextStream str(process);
	//str << QLatin1String("SetSource qthelp://mycompany.com/doc/index.html")
	//	<< QLatin1Char('\0') << endl;
	str << "-showUrl" << QCoreApplication::applicationDirPath()+"../docs/index.html";
}
//------------------------------------------------------------
void tke_MainWindow::action_about_activated()
{
	QDir::setCurrent(QCoreApplication::applicationDirPath());
#ifndef TKE_DEMO
	QFile *aboutFile = new QFile("about.html");
#else
	QFile *aboutFile = new QFile("aboutDemo.html");
#endif
	aboutFile->open(QFile::ReadOnly);
	QTextStream *aboutText = new QTextStream(aboutFile);
	QMessageBox::about (this,codec->toUnicode("Про програму"),aboutText->readAll());
	delete aboutFile;
	delete aboutText;
}
//------------------------------------------------------------
void tke_MainWindow::action_edit_users_activated()
{
#ifndef TKE_DEMO
	tkeFormEditUsers *formEditUsers = new tkeFormEditUsers();
	formEditUsers->exec();
	delete formEditUsers;
#else
	QMessageBox::information(this,
				"!!! Демо-версія !!!",
				"З метою економії розміру у демо-версії не доступна ця функція.",
				QMessageBox::Ok,
				QMessageBox::NoButton);
#endif
}
//------------------------------------------------------------
void tke_MainWindow::action_print_povidomlennya_dlya_borjnykiv_activated()
{
	tkeFormPrintPoshtaDlyaBorjnykiv *form = new tkeFormPrintPoshtaDlyaBorjnykiv();
	form->exec();
	delete form;
}
//------------------------------------------------------------
void tke_MainWindow::action_print_povidomlennya_dlya_pilgovykiv_activated()
{
	tkeFormPrintPoshtaDlyaPilgovykiv *form = new tkeFormPrintPoshtaDlyaPilgovykiv();
	form->exec();
	delete form;
}
//------------------------------------------------------------
void tke_MainWindow::action_vidomist_narah_oplaty_za_CO_dla_viddilu_subs_activated()
{
	tke_Dialog_narah *dialogWrongSubsDate = new tke_Dialog_narah(this, codec->toUnicode("Відомість нарах. опл. за ЦО для в.субсидій"), currentSysDate.year(), currentSysDate.month() );
	if (dialogWrongSubsDate->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_vidomist_narah_opl_za_CO_dlya_viddilu_subs(dialogWrongSubsDate->get_Year(), dialogWrongSubsDate->get_Month());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete dialogWrongSubsDate;
}
//------------------------------------------------------------
void tke_MainWindow::action_open_insert_narah_subs_form_activated()
{
	tke_Form_insert_subs *formSubs = new tke_Form_insert_subs(0, currentSysDate.year(), currentSysDate.month(),false);
	connect(formSubs, SIGNAL(u_abonent_rahunok_changes(int)), this, SLOT(show_rahunok(int)));
	formSubs->show();
	connect(this, SIGNAL(mainWindowClosed()), formSubs, SLOT(close()));
	connect(this, SIGNAL(closeLoginSignal()), formSubs, SLOT(close()));
}
//------------------------------------------------------------
void tke_MainWindow::action_open_insert_donarah_subs_form_activated()
{
	tke_Form_insert_subs *formSubs = new tke_Form_insert_subs(0, currentSysDate.year(), currentSysDate.month());
	connect(formSubs, SIGNAL(u_abonent_rahunok_changes(int)), this, SLOT(show_rahunok(int)));
	formSubs->show();
	connect(this, SIGNAL(mainWindowClosed()), formSubs, SLOT(close()));
	connect(this, SIGNAL(closeLoginSignal()), formSubs, SLOT(close()));
}
//------------------------------------------------------------
void tke_MainWindow::action_clear_vidomosti_pro_zdani_dokumenty_napilgy_activated()
{
	if	( ! QMessageBox::question(
                this,
                codec->toUnicode("!!! Очищення відомостей про здані документи на пільги !!!"),
                codec->toUnicode("Ви дійсно бажаєте очистити відомості про здані документи на пільги?\nВідміна операції неможлива!"),
                codec->toUnicode("Так"),
				codec->toUnicode("Ні-ні-ні"),
                QString(), 1, 1)
		){
		bool done_ok=true;
		QSqlQuery *query = new QSqlQuery();
		query->setForwardOnly(true);
		query->exec("SELECT count(*) FROM abonent WHERE Pilgy_zdani=true");
		query->seek(0);
		UPostupForm *postup_form = new UPostupForm(0, query->value(0).toInt() );
		postup_form->show();
		QSqlQuery *updateQuery = new QSqlQuery();
		query->exec("SELECT id FROM abonent WHERE Pilgy_zdani=true");
		while (query->next()){
			done_ok = done_ok && updateQuery->exec("UPDATE abonent SET Pilgy_zdani=false WHERE id="+query->value(0).toString());
			postup_form->inc_pos();
		}
		delete query;
		delete updateQuery;
		delete postup_form;
		if (done_ok)
			QMessageBox::information(this,
				codec->toUnicode("!!! Закінчено очищення відомостей про здані документи на пільги !!!"),
				codec->toUnicode("Успішно закінчено очищення відомостей про здані документи на пільги."),
				QMessageBox::Ok,
				QMessageBox::NoButton);
		else
			QMessageBox::critical(this,
				codec->toUnicode("!!! Невдале очищення відомостей про здані документи на пільги !!!"),
				codec->toUnicode("Програма не змогла очистити відомості про здані документи на пільги.\nСпробуйте ще раз."),
				QMessageBox::Ok,
				QMessageBox::NoButton);
	}
}
//------------------------------------------------------------
void tke_MainWindow::action_print_vidomist_zdanyh_dokumentiv_na_pilgy_activated()
{
	UPrintDocs * printDocs = new UPrintDocs();
	printDocs->print_vidomist_podachi_documentiv_na_pilgy();
	connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
	connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
	delete printDocs;
}
//------------------------------------------------------------
void tke_MainWindow::action_print_dovidka_u_subs_activated()
{
	tke_Dialog_narah *dialogDate = new tke_Dialog_narah(this, "Довідка у відділ субсидії", currentSysDate.year(), currentSysDate.month() );
	if (dialogDate->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_dovidka_u_viddil_subs(ui.spinBox_Rahunok->value(), dialogDate->get_Year(), dialogDate->get_Month());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete dialogDate;
}
//------------------------------------------------------------
void tke_MainWindow::action_print_dovidka_u_subs_2_activated()
{
	tke_Dialog_narah *dialogDate = new tke_Dialog_narah(this, "Довідка у відділ субсидії #2", currentSysDate.year(), currentSysDate.month() );
	if (dialogDate->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_dovidka_u_viddil_subs_2(ui.spinBox_Rahunok->value(), dialogDate->get_Year(), dialogDate->get_Month());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete dialogDate;
}
//------------------------------------------------------------
void tke_MainWindow::action_print_vidomost_narahuvannya_dlya_abon_z_lich_activated()
{
	tke_Dialog_narah *dialogDate = new tke_Dialog_narah(this, "Відомість нарахування для абонентів з лічильниками", currentSysDate.year(), currentSysDate.month() );
	if (dialogDate->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_vidomost_narahuvannya_dlya_abon_z_lich(dialogDate->get_Year(), dialogDate->get_Month());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete dialogDate;
}
//------------------------------------------------------------
void tke_MainWindow::action_print_info_pro_spojyvannya_activated()
{

}
//------------------------------------------------------------
void tke_MainWindow::action_CheckDataBaseIsCorrect_activated()
{

}
//------------------------------------------------------------
void tke_MainWindow::action_Options_activated()
{
	tke_dialog_options * dOptions = new tke_dialog_options();
	connect(dOptions, SIGNAL(requestReloadIniSettings()), this, SLOT(loadGUISettingsFromIni()));
	dOptions->exec();
	delete dOptions;
}
//------------------------------------------------------------
void tke_MainWindow::action_insert_electronic_kvytancii_UNI_activated()
{
	tke_Form_uni_insert_kvytancii *insKvytUniForm = new tke_Form_uni_insert_kvytancii(currentSysDate.year(), currentSysDate.month());
	connect(this, SIGNAL(closeLoginSignal()), insKvytUniForm, SLOT(close()));
	connect(this, SIGNAL(mainWindowClosed()), insKvytUniForm, SLOT(close()));
	connect(insKvytUniForm, SIGNAL(requestDebugWindow()), this, SLOT(showDebugWindow()));
	
	insKvytUniForm->show();
}
//------------------------------------------------------------
void tke_MainWindow::action_insert_electronic_subs_UNI_activated()
{
	tke_Form_uni_insert_subs *insSubsUniForm = new tke_Form_uni_insert_subs(currentSysDate.year(), currentSysDate.month());
	connect(this, SIGNAL(closeLoginSignal()), insSubsUniForm, SLOT(close()));
	connect(this, SIGNAL(mainWindowClosed()), insSubsUniForm, SLOT(close()));
	connect(insSubsUniForm, SIGNAL(requestDebugWindow()), this, SLOT(showDebugWindow()));
	
	insSubsUniForm->show();
}
//------------------------------------------------------------
void tke_MainWindow::action_print_zvit_forma_n_2_pilga_2007_activated()
{
	tke_Dialog_narah *dialogDate = new tke_Dialog_narah(this, codec->toUnicode("Звіт по формі № 2-пільга (2007р.)"), currentSysDate.year(), currentSysDate.month() );
	if (dialogDate->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_zvit_forma_n_2_pilga_2007(dialogDate->get_Year(), dialogDate->get_Month());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete dialogDate;
}
//------------------------------------------------------------
void tke_MainWindow::action_print_vidnoshennya_pro_vidshkoduvannya_pilg_CO_activated()
{
	tke_Dialog_narah *dialogDate = new tke_Dialog_narah(this, "Відношення про відшкодування наданих пільг по ЦО", currentSysDate.year(), currentSysDate.month() );
	if (dialogDate->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_vidnoshennya_pro_vidshkoduvannya_pilg_CO(dialogDate->get_Year(), dialogDate->get_Month());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete dialogDate;
}
//------------------------------------------------------------
void tke_MainWindow::action_print_vidomist_pererahunku_pilg_CO_activated()
{
	tke_Dialog_narah *dialogDate = new tke_Dialog_narah(this, "Відомість перерахунку суми пільг по ЦО", currentSysDate.year(), currentSysDate.month() );
	if (dialogDate->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_vidomist_pererahunku_pilg_CO(dialogDate->get_Year(), dialogDate->get_Month());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete dialogDate;
}
//------------------------------------------------------------
void tke_MainWindow::action_print_vidnoshennya_pro_vidshkoduvannya_pilg_GV_activated()
{
	tke_Dialog_narah *dialogDate = new tke_Dialog_narah(this, "Відношення про відшкодування наданих пільг по ГВ", currentSysDate.year(), currentSysDate.month() );
	if (dialogDate->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_vidnoshennya_pro_vidshkoduvannya_pilg_GV(dialogDate->get_Year(), dialogDate->get_Month());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete dialogDate;
}
//------------------------------------------------------------
void tke_MainWindow::action_print_vidomist_pererahunku_pilg_GV_activated()
{
	tke_Dialog_narah *dialogDate = new tke_Dialog_narah(this, "Відомість перерахунку суми пільг по ГВ", currentSysDate.year(), currentSysDate.month() );
	if (dialogDate->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_vidomist_pererahunku_pilg_GV(dialogDate->get_Year(), dialogDate->get_Month());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete dialogDate;
}
//------------------------------------------------------------
void tke_MainWindow::action_printVidomistOtrymannyaPoslugPoBudynkah_activated()
{
	tke_Dialog_narah *dialogDate = new tke_Dialog_narah(this, "Відомість отримання послуг по будинках", currentSysDate.year(), currentSysDate.month() );
	if (dialogDate->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_vidomistOtrymannyaPoslugPoBudynkah(dialogDate->get_Year(), dialogDate->get_Month());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete dialogDate;
}
//------------------------------------------------------------
void tke_MainWindow::action_printVidomistOtrymannyaPoslugPoKotelnyah_activated()
{
	tke_Dialog_narah *dialogDate = new tke_Dialog_narah(this, "Відомість отримання послуг по котельнях", currentSysDate.year(), currentSysDate.month() );
	if (dialogDate->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_vidomistOtrymannyaPoslugPoKotelnyah(dialogDate->get_Year(), dialogDate->get_Month());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete dialogDate;
}
//------------------------------------------------------------
void tke_MainWindow::action_printVidomistSpojyvannyaCOZaRik_activated()
{
	tke_dialog_budynok_or_abonent *fd = new tke_dialog_budynok_or_abonent
			("Друк відомості споживання ЦО", currentSysDate, true);
	if (fd->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_vidomistSpojyvannyaCOZaRik(fd->date().year(), fd->date().month(), fd->budId(), fd->rahunok());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete fd;
}
//------------------------------------------------------------
void tke_MainWindow::action_printVidomistSpojyvannyaGVZaRik_activated()
{
	tke_dialog_budynok_or_abonent *fd = new tke_dialog_budynok_or_abonent
			("Друк відомості споживання ГВ", currentSysDate, true);
	if (fd->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_vidomistSpojyvannyaGVZaRik(fd->date().year(), fd->date().month(), fd->budId(), fd->rahunok());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete fd;
}
//------------------------------------------------------------
void tke_MainWindow::action_print_vidomist_samovilnoVidkluchAbon_activated()
{
	tke_Dialog_narah *dialogDate = new tke_Dialog_narah(this, "Відомість самовільно відключених", currentSysDate.year(), currentSysDate.month() );
	if (dialogDate->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_vidomistSamovilnoVidkluchenyh(dialogDate->get_Year(), dialogDate->get_Month());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete dialogDate;
}
//------------------------------------------------------------
void tke_MainWindow::action_printNadannyaPoslugPoBudTaKot_activated()
{
	tke_Dialog_printNadannyaPoslugPoBudTaKot *d = new tke_Dialog_printNadannyaPoslugPoBudTaKot(0,"Відомість надання послуг");
	if (d->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_vidomistNadannyaPoslugPoBudTaKot(d->kotelnyaId(), d->budynokId());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete d;
}
//------------------------------------------------------------
void tke_MainWindow::action_printDebetoveKredytoveSaldo_activated()
{
	tke_Dialog_narah *dialogDate = new tke_Dialog_narah(this, "Відомість вихідного дебетового/кредитового сальдо", currentSysDate.year(), currentSysDate.month() );
	if (dialogDate->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_vidomistDebetoveKredytoveSaldo(dialogDate->get_Year(), dialogDate->get_Month());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete dialogDate;
}
//------------------------------------------------------------
void tke_MainWindow::action_printKvytanciiBeznaiynogoBorgu_activated()
{
	tke_Dialog_print_kvytancii *filtr_dialog = new tke_Dialog_print_kvytancii(0, codec->toUnicode("Фільтр квитанцій безнад.заборг."), currentSysDate.year(), currentSysDate.month());
	if (filtr_dialog->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_kvytanciiDlyaBeznadiynoiZaborg(filtr_dialog->get_Month(), filtr_dialog->get_Year(), false, filtr_dialog->get_Bud_id());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete filtr_dialog;
}
//------------------------------------------------------------
void tke_MainWindow::action_printKvytanciiBeznaiynogoBorguPotochnAbonent_activated()
{
	tke_Dialog_narah *dialog = new tke_Dialog_narah(0, codec->toUnicode("Друк квитанції за місяць:"), currentSysDate.year(), currentSysDate.month());
	if (dialog->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_kvytanciiDlyaBeznadiynoiZaborg(dialog->get_Month(), dialog->get_Year(), true, ui.spinBox_Rahunok->value());
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete dialog;
}
//------------------------------------------------------------
void tke_MainWindow::action_printVidomistBorjnykivOtrymSubs_activated()
{
	tke_Dialog_narah *dialog = new tke_Dialog_narah(0, "Вибір місяця", currentSysDate.year(), currentSysDate.month());
	if (dialog->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_printVidomistBorjnykivOtrymSubs(QDate(dialog->get_Year(),dialog->get_Month(),1));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete dialog;
}
//------------------------------------------------------------
void tke_MainWindow::action_print_zaborg_za_periody_activated()
{
	tke_Dialog_narah *dialog = new tke_Dialog_narah(0, "Вибір місяця", currentSysDate.year(), currentSysDate.month());
	if (dialog->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_printVidomistZaborgPoPeriodah(QDate(dialog->get_Year(),dialog->get_Month(),1));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete dialog;
}
//------------------------------------------------------------
void tke_MainWindow::action_zagalna_vidomist_narahuvannya_i_postuplennya_activated()
{
	tke_Dialog_narah *dialogDate = new tke_Dialog_narah(this, codec->toUnicode("Відомість нарахування, поступлення і боргу по населенню"), currentSysDate.year(), currentSysDate.month() );
	if (dialogDate->exec() == 1){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_zagalna_vidomist_narahuvannya_i_postuplennya(dialogDate->get_Year(), dialogDate->get_Month());
		connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
		delete printDocs;
	}
	delete dialogDate;
}
//------------------------------------------------------------
void tke_MainWindow::action_open_insert_kvyt_form_activated()
{
	tke_Form_insert_kvyt *formKvyt = new tke_Form_insert_kvyt(0, currentSysDate.year(), currentSysDate.month());
	connect(formKvyt, SIGNAL(u_abonent_rahunok_changes(int)), this, SLOT(show_rahunok(int)));
	formKvyt->show();
}
//------------------------------------------------------------
void tke_MainWindow::action_export_f2_v2007_activated()
{
	tke_Dialog_narah *dialog_narah = new tke_Dialog_narah(this, codec->toUnicode("Вибір місяця формування пільг"), currentSysDate.year(), currentSysDate.month());
	if (dialog_narah->exec() == 1){
		QSqlQuery query;
		query.exec("SELECT [value] FROM slujb_znach WHERE id=21");
		query.next();
		int feType = qRound(query.value(0).toDouble());
		QString dir_name = QFileDialog::getExistingDirectory( this, "Вибір папки для запису файла пільг");
		if (dir_name.size()>0){
			UExportInfoPilgovyky ep(dir_name, dialog_narah->get_Year(), dialog_narah->get_Month(), feType);
			ep.save_to_file_v2007();
		}
	}
	delete dialog_narah;
}
//------------------------------------------------------------
void tke_MainWindow::action_save_data_base_toArchive_activated()
{
	saveDataBase();
}
//------------------------------------------------------------
void tke_MainWindow::action_pokazn_opal_stoyak_activated()
{
	tke_Form_edit_pokazn_lich * form = new tke_Form_edit_pokazn_lich(0,currentSysDate.year(),currentSysDate.month());
	form->show();
	connect(this, SIGNAL(mainWindowClosed()), form, SLOT(close()));
	connect(form, SIGNAL(needRahunok(int)), this, SLOT(show_rahunok(int)));
}
//------------------------------------------------------------
void tke_MainWindow::action_znyattya_narahuv_vid_vidkluchenyh_activated()
{
	tkeDialogDateInterval *dialog = new tkeDialogDateInterval;
	dialog->setPochDate(currentSysDate.addMonths(-2));
	dialog->setKincDate(currentSysDate);
	dialog->show();
	if (dialog->exec()){
		bool doneOk = znyattya_narahuv_vid_vidkluchenyh(ui.spinBox_Rahunok->value(), dialog->pochDate(), dialog->kincDate().addMonths(-1));
		populate_operacii_table();
		if (doneOk){
			QMessageBox::information(this,"!!! Зняття нарахування за попередні місяці !!!",
				"Успішно закінчено зняття нарахування за попередні місяці.",QMessageBox::Ok);
		}
		else{
			QMessageBox::critical(this,"!!! Зняття нарахування за попередні місяці !!!",
            "Зняття нарахування за попередні місяці пройшло з можливими помилками.\nПрогляньте, будь-ласка резальтати виконання в таблиці операцій.\nПерелік помилок можна знайти в вікні \"Програмні повідомлення\".",QMessageBox::Ok);
		}
	}
}
//------------------------------------------------------------
void tke_MainWindow::action_postDataBaseForAuthurAnalize_activated()
{
	tke_Form_sendDataBase *f = new tke_Form_sendDataBase();
	f->show();
	
	connect(this, SIGNAL(mainWindowClosed()), f, SLOT(close()));
	connect(this, SIGNAL(closeLoginSignal()), f, SLOT(close()));
}
//------------------------------------------------------------
void tke_MainWindow::action_fastPilgovykyDocInsert_activated()
{
	tke_Form_fastPilgovykyDocInsert * form = new tke_Form_fastPilgovykyDocInsert();
	form->show();
	
	connect(this, SIGNAL(mainWindowClosed()), form, SLOT(close()));
	connect(form, SIGNAL(needRahunok(int)), this, SLOT(show_rahunok(int)));
}
//------------------------------------------------------------
void tke_MainWindow::action_export_vidomist_narahuvannya_po_lich_dbf_activated()
{
	tke_Dialog_narah *dialog_narah = new tke_Dialog_narah(this, "Вибір місяця формування відомості", currentSysDate.year(), currentSysDate.month());
	if (dialog_narah->exec() == 1){
		QDir::setCurrent(QCoreApplication::applicationDirPath());
		QSettings settings("./tke.ini", QSettings::IniFormat);
		QString fileToSave = QFileDialog::getSaveFileName(0, "Збереження dbf файлу", 
				settings.value("LastLoaded/LastSaveExportVidomistNarahPoLich").toString()+"\\"+"Відомість нар.по ліч. за "+QString::number(dialog_narah->get_Month()).rightJustified(2,'0')+"."+QString::number(dialog_narah->get_Year())+".dbf", "*.dbf");
		if (fileToSave.size()>1){
			QFileInfo fInfo(fileToSave);
			export_vidomist_narahuvannya_po_lichylnyku(dialog_narah->get_Year(), dialog_narah->get_Month(), fileToSave);
			settings.beginGroup("LastLoaded");
			settings.setValue("LastSaveExportVidomistNarahPoLich",fInfo.filePath());
		}
	}
	delete dialog_narah;
}
//------------------------------------------------------------
void tke_MainWindow::action_insertOpSubsFromFile_activated()
{
	tke_Form_insert_opSubs *f = new tke_Form_insert_opSubs(currentSysDate);
	connect(this, SIGNAL(closeLoginSignal()), f, SLOT(close()));
	connect(this, SIGNAL(mainWindowClosed()), f, SLOT(close()));
	connect(f, SIGNAL(u_abonent_rahunok_changes(int)), this, SLOT(show_rahunok(int)));
	f->show();
}
//------------------------------------------------------------
void tke_MainWindow::action_fillSubsDbfSpojyvannyaCO_activated()
{
	UForm_fillSubsDbfSpojyvannyaCO *f = new UForm_fillSubsDbfSpojyvannyaCO();
	connect(this, SIGNAL(closeLoginSignal()), f, SLOT(close()));
	connect(this, SIGNAL(mainWindowClosed()), f, SLOT(close()));
	f->show();
}
//------------------------------------------------------------
