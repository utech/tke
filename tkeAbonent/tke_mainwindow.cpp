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
#include "tke_Dialog_editOperaciiStrings.h"
#include "tke_Form_insert_pokazn_lich_GV.h"
#include "tke_form_insert_subs.h"
#include "tke_form_insert_kvyt.h"
#include "tke_Dialog_saveDataBase.h"

#include "../tkeSqlBrowser/src/browser.h"

//------------------------------------------------------------
void tke_MainWindow::setOrganizSpacificSettings()
{
	if (organizID()==4){
		ui.tabWidget_monthlyOperacii->setTabText(1,"Списана дебіторська заборгованість");
	
		ui.action_printKvytanciiBeznaiynogoBorgu->setText("Друк квитанцій по cписаній дебіторській заборгованісті");
		ui.action_printKvytanciiBeznaiynogoBorguPotochnAbonent->setText("Друк квитанції по cписаній дебіторській заборгованісті для поточного абонента");
	}
}
//------------------------------------------------------------
void tke_MainWindow::exec_abonent_select_query()
{
	disconnect(ui.comboBox_Poshuk, SIGNAL(currentIndexChanged (int)), ui.spinBox_navigator, SLOT(setValue(int)));
	disconnect(ui.comboBox_Poshuk_rahunok, SIGNAL(currentIndexChanged (int)), ui.spinBox_navigator, SLOT(setValue(int)));
	abonent_select_query->exec();
	int query_size=0;
	ui.comboBox_Poshuk->clear();
	ui.comboBox_Poshuk->addItem("");
	ui.comboBox_Poshuk_rahunok->clear();
	ui.comboBox_Poshuk_rahunok->addItem("");
	abonent_select_query->seek( -1 );
	abonentsList.clear();
	while (abonent_select_query->next()){
		query_size++;
		ui.comboBox_Poshuk->addItem(abonent_select_query->value(2).toString() + " " +
		abonent_select_query->value(3).toString() + " " + abonent_select_query->value(4).toString() );
		ui.comboBox_Poshuk_rahunok->addItem(abonent_select_query->value(1).toString());
		abonentsList << abonent_select_query->value(1).toInt();
	}
	ui.spinBox_navigator->setRange( 1, query_size );
	ui.label_max_rows_abonent->setText(codec->toUnicode("із ") + QVariant(query_size).toString() + codec->toUnicode(".") );
	connect(ui.comboBox_Poshuk, SIGNAL(currentIndexChanged (int)), ui.spinBox_navigator, SLOT(setValue(int)));
	connect(ui.comboBox_Poshuk_rahunok, SIGNAL(currentIndexChanged (int)), ui.spinBox_navigator, SLOT(setValue(int)));
}
//------------------------------------------------------------
void tke_MainWindow::exec_diuch_taryf_select_query(short month_filtr, short year_filtr)
{
	disconnect(ui.tableWidget_diuch_taryf, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(diuch_taryf_widget_item_changed(QTableWidgetItem *)));
	
	unsigned char num_days_in_month=days_in_month[month_filtr-1];
	diuch_taryf_query->clear();
	QVariant year(year_filtr), month(month_filtr);
	diuch_taryf_query->exec("SELECT count(*) FROM budynky");
	diuch_taryf_query->seek(0);
	ui.tableWidget_diuch_taryf->setRowCount(diuch_taryf_query->value(0).toInt());
	diuch_taryf_query->exec("SELECT budynky.Vulycia, budynky.Bud_num, budynky.id, \
									diuch_taryf.id AS Diuch_taryf_ID, diuch_taryf.Opal_days, \
									diuch_taryf.Lich_poch, diuch_taryf.Lich_kin, diuch_taryf.Taryf, \
									budynky.Lichylnyk, opal_ploshcha_bud.ZagPloshcha, \
									opal_st_ploshcha_bud.ZagStPloshcha	\
							FROM ((budynky \
							LEFT JOIN (SELECT * FROM diuch_taryf \
										WHERE ((diuch_taryf.year="+year.toString()+") \
												and (diuch_taryf.month="+month.toString()+"))) \
										AS diuch_taryf ON budynky.id = diuch_taryf.Budynok_ID) \
							LEFT JOIN (SELECT Budynok_ID, sum(Ploshcha) AS ZagPloshcha \
										FROM abonent WHERE Opal=true GROUP BY Budynok_ID) \
										AS opal_ploshcha_bud ON budynky.id = opal_ploshcha_bud.Budynok_ID) \
							LEFT JOIN (SELECT abonent.Budynok_ID, sum(pok.Plos) AS ZagStPloshcha FROM (SELECT Rahunok_ID, Plos \
										FROM pokaznOpalStoyak \
										WHERE Year="+year.toString()+" and Month="+month.toString()+") AS pok \
										LEFT JOIN abonent ON abonent.Rahunok=pok.Rahunok_ID \
										GROUP BY abonent.Budynok_ID) AS opal_st_ploshcha_bud ON budynky.id = opal_st_ploshcha_bud.Budynok_ID \
							ORDER BY budynky.Vulycia, budynky.Bud_num");
	QStringList strList;
	QTableWidgetItem * item;
	QColor redColor("#ffc1b9");
	int row=0;
	while (diuch_taryf_query->next()){
		strList << diuch_taryf_query->value(0).toString() + " " + diuch_taryf_query->value(1).toString();
		ui.tableWidget_diuch_taryf->setItem(row, 0, new QTableWidgetItem(diuch_taryf_query->value(2).toString()));
		ui.tableWidget_diuch_taryf->setItem(row, 1, new QTableWidgetItem(diuch_taryf_query->value(3).toString()));
		
		if ((diuch_taryf_query->value(3).toInt()==0) && (diuch_taryf_query->value(8).toInt()!=1)){
			ui.tableWidget_diuch_taryf->setItem(row, 2, new QTableWidgetItem(QVariant(num_days_in_month).toString()));
			ui.tableWidget_diuch_taryf->item(row,2)->setBackgroundColor ( Qt::cyan );
			ui.tableWidget_diuch_taryf->item(row,2)->setTextAlignment( Qt::AlignVCenter | Qt::AlignRight );
		}
		else{
			ui.tableWidget_diuch_taryf->setItem(row, 2, new QTableWidgetItem(diuch_taryf_query->value(4).toString()));
			ui.tableWidget_diuch_taryf->item(row,2)->setTextAlignment( Qt::AlignVCenter | Qt::AlignRight );
		}
		
		item = new QTableWidgetItem();
		item->setData(Qt::DisplayRole, diuch_taryf_query->value(5).toDouble());
		ui.tableWidget_diuch_taryf->setItem(row, 3, item);
		ui.tableWidget_diuch_taryf->item(row,3)->setTextAlignment( Qt::AlignVCenter | Qt::AlignRight );
		item = new QTableWidgetItem();
		item->setData(Qt::DisplayRole, diuch_taryf_query->value(6).toDouble());
		ui.tableWidget_diuch_taryf->setItem(row, 4, item);
		ui.tableWidget_diuch_taryf->item(row,4)->setTextAlignment( Qt::AlignVCenter | Qt::AlignRight );
		if (diuch_taryf_query->value(8).toInt() != 1){
			ui.tableWidget_diuch_taryf->item(row,3)->setBackgroundColor( redColor );
			ui.tableWidget_diuch_taryf->item(row,4)->setBackgroundColor( redColor );
		}
		
		item = new QTableWidgetItem();
		item->setData(Qt::DisplayRole, diuch_taryf_query->value(7).toDouble());
		ui.tableWidget_diuch_taryf->setItem(row, 5, item);
		ui.tableWidget_diuch_taryf->item(row,5)->setTextAlignment( Qt::AlignVCenter | Qt::AlignRight );
		ui.tableWidget_diuch_taryf->setItem(row, 6, new QTableWidgetItem(diuch_taryf_query->value(8).toString()));
		
		item = new QTableWidgetItem();
		item->setData(Qt::DisplayRole, diuch_taryf_query->value(9).toDouble()+diuch_taryf_query->value(10).toDouble());
		ui.tableWidget_diuch_taryf->setItem(row, 7, item);
			ui.tableWidget_diuch_taryf->item(row,7)->setTextAlignment( Qt::AlignVCenter | Qt::AlignRight );
		row++;
	}
	ui.tableWidget_diuch_taryf->setVerticalHeaderLabels(strList);
	ui.tableWidget_diuch_taryf->resizeRowsToContents();
	strList.clear();
	
	connect(ui.tableWidget_diuch_taryf, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(diuch_taryf_widget_item_changed(QTableWidgetItem *)));
}
//------------------------------------------------------------
void tke_MainWindow::populate_comboboxes_abonent()
{
	//abonent_form_disconnect_editres();
	QString str;
	//inicializacaya budynkiv
	QSqlQuery query("SELECT id, Vulycia, Bud_num FROM budynky ORDER BY Vulycia, Bud_num");
	ui.comboBox_Budynok->clear();
	ui.comboBox_Budynok->addItem("",0);
        while (query.next()) {
            str = query.value(1).toString() + " " + query.value(2).toString();
            ui.comboBox_Budynok->addItem(str, query.value(0).toInt());
        }
	//inicializaciya pilg 
	query.clear();
	query.exec("SELECT id, Kategoriya, Vidsotok, Ploshsca FROM pilgy ORDER BY Kategoriya");
	ui.comboBox_Kategoria_Pilgy->clear();
	ui.comboBox_Kategoria_Pilgy_2->clear();
	pilgy_hash->clear();
	ui.comboBox_Kategoria_Pilgy->addItem("");
	ui.comboBox_Kategoria_Pilgy_2->addItem("");
	pilgy_hash->insert( "", 0 );
        while (query.next()) {
            str = query.value(1).toString() + " (" + query.value(2).toString() + "%)";
			pilgy_hash->insert( str, query.value(0).toInt() );
            ui.comboBox_Kategoria_Pilgy->addItem(str);
			ui.comboBox_Kategoria_Pilgy_2->addItem(str);
        }
	//inicializaciya Imen
	query.clear();
	query.exec("SELECT Imya FROM abonent GROUP BY Imya");
	ui.comboBox_Imya->clear();
        while (query.next()) {
            str  = query.value(0).toString();
            ui.comboBox_Imya->addItem(str);
        }
	//inicializaciya Po Batkovi
	query.clear();
	query.exec("SELECT Batk FROM abonent GROUP BY Batk");
	ui.comboBox_Batk->clear();
        while (query.next()) {
            str  = query.value(0).toString();
            ui.comboBox_Batk->addItem(str);
        }
		
	//abonent_form_connect_editres();
}
//------------------------------------------------------------
void tke_MainWindow::populate_abonent_form( int seek_query )
{
	abonent_form_disconnect_editres();
	disconnect(ui.spinBox_navigator, SIGNAL(valueChanged (int)), this, SLOT(populate_abonent_form(int)));
	
	if ( (new_record==1) && (record_edited==1) ){ // novyi zapys, yakyi bulo redagovano
		qWarning() << "Part novyi zapys, yakyi bulo redagovano";
		abonent_form_record_end_editing();
		abonent_form_insert_record();
		qWarning() << "Part befour abonent select query";
		exec_abonent_select_query();
		qWarning() << "Part after abonent select query";
		new_record = 0;
	}
	else if ( record_edited==1 ){ // zmina zapysu
		qWarning() << "Part zmina zapysu";
		abonent_form_record_end_editing();
		abonent_form_update_record();
		exec_abonent_select_query();
		ui.spinBox_navigator->setValue(seek_query);
		enable_navigation_buttons(ui.spinBox_navigator->value());
	}
	else if ( (new_record==1) && (record_edited==0) ){
		qWarning() << "Part novyi_neredagovanyi_zapys";
		exec_abonent_select_query();
		enable_navigation_buttons(ui.spinBox_navigator->value());
		ui.pushButton_new->setEnabled( TRUE );
		new_record = 0;
	}
	abonent_form_disconnect_editres();
	abonent_select_query->seek( seek_query-1 );
	//zapovnennya poliv
	ui.spinBox_Rahunok->setValue( abonent_select_query->value(1).toInt() );
	ui.spinBox_BankRahunok->setValue( abonent_select_query->value(21).toInt() );
	ui.lineEdit_Prizv->setText( abonent_select_query->value(2).toString() );
	ui.comboBox_Imya->setEditText( abonent_select_query->value(3).toString() );
	ui.comboBox_Batk->setEditText( abonent_select_query->value(4).toString() );
	ui.spinBox_Dilnycia->setValue( abonent_select_query->value(5).toInt() );
	
	ui.comboBox_Budynok->setCurrentIndex( ui.comboBox_Budynok->findData(abonent_select_query->value(6).toInt(),Qt::UserRole) );
	
	ui.spinBox_Kvartyra->setValue( abonent_select_query->value(7).toInt() );
	ui.lineEdit_kvartSufiks->setText( abonent_select_query->value(28).toString() );
	ui.doubleSpinBox_Ploshcha->setValue( abonent_select_query->value(8).toDouble() );
	ui.doubleSpinBox_Zaiva_Ploshcha->setValue( abonent_select_query->value(9).toDouble() );
	
	QString str = pilgy_hash->key( abonent_select_query->value(10).toInt() );
	ui.comboBox_Kategoria_Pilgy->setCurrentIndex ( ui.comboBox_Kategoria_Pilgy->findText ( str ) );
	str = pilgy_hash->key( abonent_select_query->value(17).toInt() );
	ui.comboBox_Kategoria_Pilgy_2->setCurrentIndex ( ui.comboBox_Kategoria_Pilgy_2->findText ( str ) );
	
	ui.spinBox_Pilga_for->setValue( abonent_select_query->value(11).toInt() );
	ui.spinBox_Pilga_for_2->setValue( abonent_select_query->value(18).toInt() );
	ui.lineEdit_Telefon->setText( abonent_select_query->value(12).toString() );
	ui.lineEdit_Identyf->setText( abonent_select_query->value(13).toString() );
	
	ui.spinBox_Kimn->setValue( abonent_select_query->value(14).toInt() );
	ui.spinBox_Ludy->setValue( abonent_select_query->value(15).toInt() );
	ui.checkBox_Opal->setChecked( abonent_select_query->value(16).toBool() );
	ui.checkBox_Zdano_Pilgy->setChecked( abonent_select_query->value(19).toBool() );
	ui.checkBox_GarVoda->setChecked( abonent_select_query->value(20).toBool() );
	ui.checkBox_narahStoyak->setChecked( abonent_select_query->value(27).toBool() );
	ui.checkBox_IsSamovilnyk->setChecked( abonent_select_query->value(30).toBool() );
	
	ui.groupBox_PilgsDatesLimits->setChecked( abonent_select_query->value(22).toBool() );
	ui.dateEdit_PilgaBeginDate->setDate( abonent_select_query->value(23).toDate() );
	ui.dateEdit_PilgaEndDate->setDate( abonent_select_query->value(24).toDate() );
	ui.dateEdit_Pilga_2BeginDate->setDate( abonent_select_query->value(25).toDate() );
	ui.dateEdit_Pilga_2EndDate->setDate( abonent_select_query->value(26).toDate() );
	
	ui.lineEdit_prymitka->setText( abonent_select_query->value(29).toString() );
	
	populate_subs_spl_zaborg_labels();
	populate_operacii_table();
	populateBeznadiynaDebitorskaZaborgTable();
	
	populateLichPokaznAbon();
	
	connect(ui.spinBox_navigator, SIGNAL(valueChanged (int)), this, SLOT(populate_abonent_form(int)));
	abonent_form_connect_editres();
}
//------------------------------------------------------------
void tke_MainWindow::populate_operacii_table()
{
	disconnect(ui.tableWidget_rozrahunky, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(tableWidget_rozrahunky_cellDoubleClicked(int,int)));
	
	int table_month;
	
	clear_operacii_table();
	QSqlQuery operacii_query;
		//Частина вхідного сальдо
	operacii_query.exec("SELECT Month, Vh_saldo FROM narahuvannya \
						WHERE (Rahunok="+ QVariant(ui.spinBox_Rahunok->value()).toString() +") \
								and (Year="+ QVariant(ui.spinBox_year_filtr->value()).toString() +") \
								and Status=0");
	while (operacii_query.next()){
		table_month = operacii_query.value(0).toInt();
		ui.tableWidget_rozrahunky->item( table_month-1, 0)->setText( uMToStr2(operacii_query.value(1).toDouble()));
	}
		//Частина нарахування за ЦО + пільга, ГВ + пільга, нарахування по Стояках
	operacii_query.exec("SELECT Month, sum(Narah), sum(Pilga_sum), sum(Narah_GV), sum(Pilga_sum_GV), sum(Narah_St) FROM narahuvannya \
						WHERE (Rahunok="+ QVariant(ui.spinBox_Rahunok->value()).toString() +") \
								and (Year="+ QVariant(ui.spinBox_year_filtr->value()).toString() +") \
						GROUP BY Month");
	while (operacii_query.next()){
		table_month = operacii_query.value(0).toInt();
		ui.tableWidget_rozrahunky->item( table_month-1, 1)->setText( uMToStr2(operacii_query.value(1).toDouble()));
		ui.tableWidget_rozrahunky->item( table_month-1, 2)->setText( uMToStr2(operacii_query.value(2).toDouble()));
		ui.tableWidget_rozrahunky->item( table_month-1, 3)->setText( uMToStr2(operacii_query.value(3).toDouble()));
		ui.tableWidget_rozrahunky->item( table_month-1, 4)->setText( uMToStr2(operacii_query.value(4).toDouble()));
		double stSum = operacii_query.value(5).toDouble();
		if (stSum > 0.009 || stSum < -0.009)
			ui.tableWidget_rozrahunky->item( table_month-1, 5)->setText( uMToStr2(stSum));
	}
		//Частина субсидій
	operacii_query.exec("SELECT Month, sum(Suma), sum(Suma_d), sum(Suma_GV), sum(Suma_d_GV) FROM subs \
						WHERE (Rahunok_ID="+ QVariant(ui.spinBox_Rahunok->value()).toString() +") \
								and (Year="+ QVariant(ui.spinBox_year_filtr->value()).toString() +") GROUP BY Month");
	while (operacii_query.next()){
		table_month = operacii_query.value(0).toInt();
		ui.tableWidget_rozrahunky->item( table_month-1, 6)->setText( uMToStr2(operacii_query.value(1).toDouble()));
		ui.tableWidget_rozrahunky->item( table_month-1, 7)->setText( uMToStr2(operacii_query.value(2).toDouble()));
		ui.tableWidget_rozrahunky->item( table_month-1, 8)->setText( uMToStr2(operacii_query.value(3).toDouble()));
		ui.tableWidget_rozrahunky->item( table_month-1, 9)->setText( uMToStr2(operacii_query.value(4).toDouble()));
	}
		//Частина квитанцій (оплати)
	operacii_query.exec("SELECT month(Plata_date), SUM(Suma) FROM (SELECT * FROM kvytancii  \
							WHERE (Rahunok_id="+ QVariant(ui.spinBox_Rahunok->value()).toString() +") and \
							(year(Plata_date)="+ QVariant(ui.spinBox_year_filtr->value()).toString() +")) GROUP BY month(Plata_date)");
	while (operacii_query.next())
		ui.tableWidget_rozrahunky->item( operacii_query.value(0).toInt()-1, 10)->setText( uMToStr2(operacii_query.value(1).toDouble()));
		//Частина коректури по площі
	operacii_query.exec("SELECT month, suma FROM korektura \
						WHERE (id="+ QVariant(ui.spinBox_Rahunok->value()).toString() +") \
						and (Year="+ QVariant(ui.spinBox_year_filtr->value()).toString() +")");
	while (operacii_query.next()){
		table_month = operacii_query.value(0).toInt();
		ui.tableWidget_rozrahunky->item( table_month-1, 11)->setText( uMToStr2(operacii_query.value(1).toDouble()));
	}
		//Частина коректури по сумі
	operacii_query.exec("SELECT month, sum(suma), sum(suma_GV), sum(suma_ST)  FROM korektura_s \
						WHERE (id="+ QVariant(ui.spinBox_Rahunok->value()).toString() +") \
							and (Year="+ QVariant(ui.spinBox_year_filtr->value()).toString() +") \
						GROUP BY month");
	while (operacii_query.next()){
		table_month = operacii_query.value(0).toInt();
		ui.tableWidget_rozrahunky->item( table_month-1, 12)->setText( uMToStr2(operacii_query.value(1).toDouble()));
		ui.tableWidget_rozrahunky->item( table_month-1, 13)->setText( uMToStr2(operacii_query.value(2).toDouble()));
		ui.tableWidget_rozrahunky->item( table_month-1, 14)->setText( uMToStr2(operacii_query.value(3).toDouble()));
	}
		//Частина переведення заборгованості
	operacii_query.exec("SELECT month(CDate), sum(Suma) FROM narahuvannyaPereved \
						WHERE Rahunok_id="+ui.spinBox_Rahunok->text()+" \
							and year(CDate)="+ui.spinBox_year_filtr->text()+" \
						GROUP BY month(CDate)");
	while (operacii_query.next()){
		table_month = operacii_query.value(0).toInt();
		ui.tableWidget_rozrahunky->item( table_month-1, 15)->setText( uMToStr2(operacii_query.value(1).toDouble()));
	}
	
	calc_operacii_table();
	
	connect(ui.tableWidget_rozrahunky, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(tableWidget_rozrahunky_cellDoubleClicked(int,int)));
}
//------------------------------------------------------------
void tke_MainWindow::tableWidget_rozrahunky_cellDoubleClicked(int row, int col)
{
	tke_Dialog_editOperaciiStrings * editNarahDialog = new tke_Dialog_editOperaciiStrings(ui.spinBox_Rahunok->value(), ui.spinBox_year_filtr->value(), row+1);
	connect(editNarahDialog, SIGNAL(requestReloadNarahuvannya()), this, SLOT(populate_operacii_table()));
	editNarahDialog->exec();
	delete editNarahDialog;
}
//------------------------------------------------------------
void tke_MainWindow::populate_subs_spl_zaborg_labels()
{
	//Заповнення комірок договорів субсидії і удод сплати заборгованості
	QSqlQuery subs_query;
	subs_query.exec("SELECT Nom_spravy, \
						Poch_month, Poch_year, Kin_month, Kin_year \
						FROM spravy_subs \
						WHERE (Rahunok_ID=" + abonent_select_query->value(1).toString() + ") \
						and (Kin_year=" + QVariant(ui.spinBox_year_filtr->value()).toString() + ")");
	QString subs_str(codec->toUnicode("Сбс:"));
	if (subs_query.next())
		subs_str += "N " + subs_query.value(0).toString() + " /" 
						+ subs_query.value(1).toString() + "."
						+ subs_query.value(2).toString() + "-"
						+ subs_query.value(3).toString() + "."
						+ subs_query.value(4).toString() + "/\n";
	else
		subs_str += " - - - \n";
	subs_query.exec("SELECT Nom_spravy, \
						Poch_month, Poch_year, Kin_month, Kin_year \
						FROM spravy_subs \
						WHERE (Rahunok_ID=" + abonent_select_query->value(1).toString() + ") \
						and (Poch_year=" + QVariant(ui.spinBox_year_filtr->value()).toString() + ")");
	if (subs_query.next())
		subs_str += "N " + subs_query.value(0).toString() + " /" 
						+ subs_query.value(1).toString() + "."
						+ subs_query.value(2).toString() + "-"
						+ subs_query.value(3).toString() + "."
						+ subs_query.value(4).toString() + "/";
	else
		subs_str += " - - - ";
	ui.label_sprava_subs->setText(subs_str);
	
	//Виведення відомостей по сплаті заборгованості
	QDate dt=QDate::currentDate();
	int year, month;
	if (ui.spinBox_year_filtr->value()>=dt.year()){
		year = dt.year();
		month = dt.month();
	}
	else{
		year = ui.spinBox_year_filtr->value();
		month = 1;
	}
	subs_query.exec("SELECT Ugoda, Suma, Poch_month, Poch_year, Kin_month, Kin_year From spl_zaborg \
						WHERE Rahunok_ID=" + abonent_select_query->value(1).toString() + " \
							and ((Kin_year>" + QVariant(year).toString() + ") \
							or ((Kin_year=" + QVariant(year).toString() + ") \
							and (Kin_month>=" + QVariant(month).toString() + ")))");
	if (subs_query.next())
		ui.label_zaborg->setText("N " + subs_query.value(0).toString() + " ("
						+ uMToStr2(subs_query.value(1).toDouble()) + codec->toUnicode("грн)/")
						+ subs_query.value(2).toString() + "."
						+ subs_query.value(3).toString() + "-"
						+ subs_query.value(4).toString() + "."
						+ subs_query.value(5).toString() + "/");
	else
		ui.label_zaborg->setText(" -- -- -- ");
}
//------------------------------------------------------------
void tke_MainWindow::populate_pilgovyky_filtr_comboboxes()
{
	t_filtr_select_query = new QSqlQuery();
	t_filtr_select_query->exec("SELECT id, Kategoriya FROM pilgy ORDER BY Kategoriya");
	pilgyList.clear();
	ui.comboBox_filtr_pigovyky_kategoriya->clear();
	while (t_filtr_select_query->next()){
		pilgyList << t_filtr_select_query->value(0).toInt();
		ui.comboBox_filtr_pigovyky_kategoriya->addItem(t_filtr_select_query->value(1).toString());
	}
	delete t_filtr_select_query;
}
//------------------------------------------------------------
void tke_MainWindow::enable_navigation_buttons(int index)
{
	ui.pushButton_first->setEnabled ( TRUE );
	ui.pushButton_previous->setEnabled ( TRUE );
	ui.pushButton_next->setEnabled ( TRUE );
	ui.pushButton_last->setEnabled ( TRUE );
	if ( index < 2 ){
		ui.pushButton_first->setEnabled ( FALSE );
		ui.pushButton_previous->setEnabled ( FALSE );
	}
	if ( index >= ui.spinBox_navigator->maximum() ){
		ui.pushButton_next->setEnabled ( FALSE );
		ui.pushButton_last->setEnabled ( FALSE );
	}
}
//------------------------------------------------------------
void tke_MainWindow::diuch_taryf_date_changed()
{
	int month = date.month();
	int year = date.year();

	if (ui.spinBox_year_diuch_taryf->value() == date.year()){
		month++;
		if (month == 13){
			month=12;
			year++;
		}
		ui.spinBox_month_diuch_taryf->setMaximum(month);
	}
	else if (ui.spinBox_year_diuch_taryf->value() < date.year())
		ui.spinBox_month_diuch_taryf->setMaximum(12);
	else
		ui.spinBox_month_diuch_taryf->setMaximum(1);
	ui.spinBox_year_diuch_taryf->setMaximum(year);
	exec_diuch_taryf_select_query(ui.spinBox_month_diuch_taryf->value(), ui.spinBox_year_diuch_taryf->value());
	//Заповнення колонок висвітлення нормативних тарифів для даного місяця
	UNasTaryfInfo tarNas = nasTaryf(ui.spinBox_year_diuch_taryf->value(), ui.spinBox_month_diuch_taryf->value());
	ui.diuch_taryf_label_norm_taryf_gkal->setText ( codec->toUnicode("ГКал.:") + uMToStr2(tarNas.vart_g_kal) );
	ui.diuch_taryf_label_norm_taryf_kvadrat->setText ( codec->toUnicode("Квадр.:") + uMToStr2(tarNas.normat_taryf_kvadr) );
}
//------------------------------------------------------------
void tke_MainWindow::subs_date_changed()
{
	if (ui.spinBox_year_subs->value() == date.year())
		ui.spinBox_month_subs->setMaximum(date.month());
	else
		ui.spinBox_month_subs->setMaximum(12);
	model_subs->setFilter( "year=" + QVariant(ui.spinBox_year_subs->value()).toString() + 
							" and month="  + QVariant(ui.spinBox_month_subs->value()).toString());
	model_subs->select();
}
//------------------------------------------------------------
void tke_MainWindow::korektura_date_changed()
{
	if (ui.spinBox_year_korektura->value() == date.year())
		ui.spinBox_month_korektura->setMaximum(date.month());
	else
		ui.spinBox_month_korektura->setMaximum(12);
	
	model_korektura->setFilter( "year=" + QVariant(ui.spinBox_year_korektura->value()).toString() + 
							" and month="  + QVariant(ui.spinBox_month_korektura->value()).toString());
	model_korektura->select();
}
//------------------------------------------------------------
void tke_MainWindow::korektura_s_date_changed()
{
	if (ui.spinBox_year_korektura_s->value() == date.year())
		ui.spinBox_month_korektura_s->setMaximum(date.month());
	else
		ui.spinBox_month_korektura_s->setMaximum(12);
	model_korektura_s->setFilter( "year=" + QVariant(ui.spinBox_year_korektura_s->value()).toString() + 
							" and month="  + QVariant(ui.spinBox_month_korektura_s->value()).toString());
	model_korektura_s->select();
}
//------------------------------------------------------------
void tke_MainWindow::kvyt_filtr_changed()
{
	//Налаштування максимумів для combo місяців
	if (ui.spinBox_kvyt_opl_year->value() == date.year())
		ui.spinBox_kvyt_opl_month->setMaximum(date.month());
	else
		ui.spinBox_kvyt_opl_month->setMaximum(12);
	if (ui.spinBox_kvyt_pereved_year->value() == date.year())
		ui.spinBox_kvyt_pereved_month->setMaximum(date.month());
	else
		ui.spinBox_kvyt_pereved_month->setMaximum(12);
	//Зміна фільтру
	QString filtrString;
	bool prev=false;
	if (ui.groupBox_kvyt_oplata->isChecked()){
		filtrString = "year(Plata_date)=" + QVariant(ui.spinBox_kvyt_opl_year->value()).toString() +
						" and month(Plata_date)=" + QVariant(ui.spinBox_kvyt_opl_month->value()).toString();
		prev = true;
	}
	if (ui.groupBox_kvyt_pereved->isChecked()){
		if (prev == true)
			filtrString += " and ";
		filtrString += "year(Perevid_date)=" + QVariant(ui.spinBox_kvyt_pereved_year->value()).toString() +
						" and month(Perevid_date)=" + QVariant(ui.spinBox_kvyt_pereved_month->value()).toString();
		prev = true;
	}
	if (ui.groupBox_kvyt_pachka->isChecked()){
		if (prev == true)
			filtrString += " and ";
		filtrString += "Pachka=" + QVariant(ui.spinBox_kvyt_pachka->value()).toString();
		prev = true;
	}
	if (prev){
		model_kvytancii->setFilter(filtrString);
		model_kvytancii->select();
	}
	else{
		model_kvytancii->setFilter("");
		model_kvytancii->select();
	}
}
//------------------------------------------------------------
void tke_MainWindow::pilgovyky_filtr_changed()
{
	QString filtrString;
	bool prev=false;
	if (ui.groupBox_filtr_kategor_pilgovyky->isChecked()){
		filtrString = "Pilga_ID=" + QVariant(pilgyList.at(ui.comboBox_filtr_pigovyky_kategoriya->currentIndex())).toString();
		prev = true;
	}
	model_pilgovyky->setSort(1, Qt::AscendingOrder);
	if (prev){
		model_pilgovyky->setFilter(filtrString);
		model_pilgovyky->select();
	}
	else{
		model_pilgovyky->setFilter("");
		model_pilgovyky->select();
	}
}
//------------------------------------------------------------
void tke_MainWindow::spravy_subs_order_changed(bool b_val)
{
	if (b_val){
		model_spravy_subs->setSort(1, Qt::AscendingOrder);
		model_spravy_subs->select();
	}
	else{
		model_spravy_subs->setSort(2, Qt::AscendingOrder);
		model_spravy_subs->select();
	}
}
//------------------------------------------------------------
void tke_MainWindow::splata_zaborg_order_changed(bool b_val)
{
	if (b_val){
		model_splata_zaborg->setSort(1, Qt::AscendingOrder);
		model_splata_zaborg->select();
	}
	else{
		model_splata_zaborg->setSort(2, Qt::AscendingOrder);
		model_splata_zaborg->select();
	}
}
//------------------------------------------------------------
void tke_MainWindow::kvyt_control_sum_changed()
{
	int i;
	int row = model_kvytancii->rowCount();
	double sum=0;
	for (i=0; i<row; i++){
		QModelIndex index = model_kvytancii->index(i, 2);
		sum +=index.data(Qt::DisplayRole).toDouble();
	}
	ui.label_kvyt_sum->setText(codec->toUnicode("Сума: ") + uMToStr2(sum) 
								+ codec->toUnicode(" грн.\nК-сть квит. ")
								+ QVariant(row).toString() + codec->toUnicode("."));
}
//------------------------------------------------------------
void tke_MainWindow::pilgovyky_control_sum_changed()
{
	ui.label_pilgovyky_sum->setText(codec->toUnicode("К-сть пільговиків - ") 
									+ QVariant(model_pilgovyky->rowCount()).toString() + codec->toUnicode("."));
}
//------------------------------------------------------------
void tke_MainWindow::checkBox_Opal_state_changed()
{
	if	( ! QMessageBox::question(
                this,
                codec->toUnicode("!!! Зміна підключення абонента до ЦО !!!"),
                codec->toUnicode("Ви дійсно бажаєте змінити стан підключення абонента до ЦО?"),
                codec->toUnicode("Так"),
				codec->toUnicode("Ні-ні-ні"),
                QString(), 1, 1)
		)
		abonent_form_record_editing();
	else{
		disconnect(ui.checkBox_Opal, SIGNAL(toggled(bool)), this, SLOT(checkBox_Opal_state_changed()));
		ui.checkBox_Opal->setChecked( ! ui.checkBox_Opal->isChecked() );
		connect(ui.checkBox_Opal, SIGNAL(toggled(bool)), this, SLOT(checkBox_Opal_state_changed()));
	}

}
//------------------------------------------------------------
void tke_MainWindow::checkBox_Zdano_Pilgy_state_changed()
{
	if	( ! QMessageBox::question(
                this,
                codec->toUnicode("!!! Зміна стану наявності документів пільг !!!"),
                codec->toUnicode("Ви дійсно бажаєте змінити стан наявності док.пільг для абонента?"),
                codec->toUnicode("Так"),
				codec->toUnicode("Ні-ні-ні"),
                QString(), 1, 1)
		)
		abonent_form_record_editing();
	else{
		disconnect(ui.checkBox_Zdano_Pilgy, SIGNAL(toggled(bool)), this, SLOT(checkBox_Zdano_Pilgy_state_changed()));
		ui.checkBox_Zdano_Pilgy->setChecked( ! ui.checkBox_Zdano_Pilgy->isChecked() );
		connect(ui.checkBox_Zdano_Pilgy, SIGNAL(toggled(bool)), this, SLOT(checkBox_Zdano_Pilgy_state_changed()));
	}
}
//------------------------------------------------------------
void tke_MainWindow::checkBox_GarVoda_state_changed()
{
	if	( ! QMessageBox::question(
                this,
                codec->toUnicode("!!! Зміна стану надання послуг гарячої води для абонента !!!"),
                codec->toUnicode("Ви дійсно бажаєте змінити стан\nнадання послуг ГВ для абонента?"),
                codec->toUnicode("Так"),
				codec->toUnicode("Ні-ні-ні"),
                QString(), 1, 1)
		)
		abonent_form_record_editing();
	else{
		disconnect(ui.checkBox_GarVoda, SIGNAL(toggled(bool)), this, SLOT(checkBox_GarVoda_state_changed()));
		ui.checkBox_GarVoda->setChecked( ! ui.checkBox_GarVoda->isChecked() );
		connect(ui.checkBox_GarVoda, SIGNAL(toggled(bool)), this, SLOT(checkBox_GarVoda_state_changed()));
	}
}
//------------------------------------------------------------
void tke_MainWindow::checkBox_narahStoyak_state_changed()
{
	if	( ! QMessageBox::question(
                this,
                "!!! Зміна стану нарахування за стояки !!!",
                "Ви дійсно бажаєте змінити стан\nнарахування за стояки для абонента?",
                "Так",
				"Ні-ні-ні",
                QString(), 1, 1)
		)
		abonent_form_record_editing();
	else{
		disconnect(ui.checkBox_narahStoyak, SIGNAL(toggled(bool)), this, SLOT(checkBox_narahStoyak_state_changed()));
		ui.checkBox_narahStoyak->setChecked( ! ui.checkBox_narahStoyak->isChecked() );
		connect(ui.checkBox_narahStoyak, SIGNAL(toggled(bool)), this, SLOT(checkBox_narahStoyak_state_changed()));
	}
}
//------------------------------------------------------------
void tke_MainWindow::checkBox_IsSamovilnyk_state_changed()
{
	if	( ! QMessageBox::question(
                this,
                "!!! Зміна стану самовільного відключення !!!",
                "Ви дійсно бажаєте змінити стан\nсамовільного відключення для абонента?",
                "Так",
				"Ні-ні-ні",
                QString(), 1, 1)
		)
		abonent_form_record_editing();
	else{
		disconnect(ui.checkBox_IsSamovilnyk, SIGNAL(toggled(bool)), this, SLOT(checkBox_IsSamovilnyk_state_changed()));
		ui.checkBox_IsSamovilnyk->setChecked( ! ui.checkBox_IsSamovilnyk->isChecked() );
		connect(ui.checkBox_IsSamovilnyk, SIGNAL(toggled(bool)), this, SLOT(checkBox_IsSamovilnyk_state_changed()));
	}
}
//------------------------------------------------------------
void tke_MainWindow::abonent_form_connect_editres()
{
						//abonent form connectors --> from editors (Values Changed)
	connect(ui.lineEdit_Prizv, SIGNAL(textEdited (const QString &)), this, SLOT(abonent_form_record_editing()));
	connect(ui.comboBox_Imya, SIGNAL(editTextChanged (const QString &)), this, SLOT(abonent_form_record_editing()));
	connect(ui.comboBox_Batk, SIGNAL(editTextChanged (const QString &)), this, SLOT(abonent_form_record_editing()));
	connect(ui.spinBox_Dilnycia, SIGNAL(valueChanged (int)), this, SLOT(abonent_form_record_editing()));
	connect(ui.comboBox_Budynok, SIGNAL(currentIndexChanged (int)), this, SLOT(abonent_form_record_editing()));
	connect(ui.spinBox_Kvartyra, SIGNAL(valueChanged (int)), this, SLOT(abonent_form_record_editing()));
	connect(ui.doubleSpinBox_Ploshcha, SIGNAL(valueChanged (double)), this, SLOT(abonent_form_record_editing()));
	connect(ui.doubleSpinBox_Zaiva_Ploshcha, SIGNAL(valueChanged (double)), this, SLOT(abonent_form_record_editing()));
	connect(ui.comboBox_Kategoria_Pilgy, SIGNAL(currentIndexChanged (int)), this, SLOT(abonent_form_record_editing()));
	connect(ui.spinBox_Pilga_for, SIGNAL(valueChanged (int)), this, SLOT(abonent_form_record_editing()));
	connect(ui.comboBox_Kategoria_Pilgy_2, SIGNAL(currentIndexChanged (int)), this, SLOT(abonent_form_record_editing()));
	connect(ui.spinBox_Pilga_for_2, SIGNAL(valueChanged (int)), this, SLOT(abonent_form_record_editing()));
	connect(ui.lineEdit_Telefon, SIGNAL(textEdited (const QString &)), this, SLOT(abonent_form_record_editing()));
	connect(ui.lineEdit_Identyf, SIGNAL(textEdited (const QString &)), this, SLOT(abonent_form_record_editing()));
	connect(ui.spinBox_Rahunok, SIGNAL(valueChanged (int)), this, SLOT(abonent_form_record_editing()));
	connect(ui.spinBox_Kimn, SIGNAL(valueChanged (int)), this, SLOT(abonent_form_record_editing()));
	connect(ui.spinBox_Ludy, SIGNAL(valueChanged (int)), this, SLOT(abonent_form_record_editing()));
	connect(ui.checkBox_Opal, SIGNAL(toggled(bool)), this, SLOT(checkBox_Opal_state_changed()));
	connect(ui.checkBox_Zdano_Pilgy, SIGNAL(toggled(bool)), this, SLOT(checkBox_Zdano_Pilgy_state_changed()));
	connect(ui.checkBox_IsSamovilnyk, SIGNAL(toggled(bool)), this, SLOT(checkBox_IsSamovilnyk_state_changed()));
	
	connect(ui.checkBox_GarVoda, SIGNAL(toggled(bool)), this, SLOT(checkBox_GarVoda_state_changed()));
	connect(ui.checkBox_narahStoyak, SIGNAL(toggled(bool)), this, SLOT(checkBox_narahStoyak_state_changed()));
	connect(ui.spinBox_BankRahunok, SIGNAL(valueChanged(int)), this, SLOT(abonent_form_record_editing()));
	connect(ui.groupBox_PilgsDatesLimits, SIGNAL(toggled(bool)), this, SLOT(abonent_form_record_editing()));
	
	connect(ui.dateEdit_PilgaBeginDate, SIGNAL(dateChanged(const QDate &)), this, SLOT(abonent_form_record_editing()));
	connect(ui.dateEdit_PilgaEndDate, SIGNAL(dateChanged(const QDate &)), this, SLOT(abonent_form_record_editing()));
	connect(ui.dateEdit_Pilga_2BeginDate, SIGNAL(dateChanged(const QDate &)), this, SLOT(abonent_form_record_editing()));
	connect(ui.dateEdit_Pilga_2EndDate, SIGNAL(dateChanged(const QDate &)), this, SLOT(abonent_form_record_editing()));
	
	connect(ui.lineEdit_kvartSufiks, SIGNAL(textEdited (const QString &)), this, SLOT(abonent_form_record_editing()));
	connect(ui.lineEdit_prymitka, SIGNAL(textEdited (const QString &)), this, SLOT(abonent_form_record_editing()));
	
	//частина обрахункових об'єднань
	connect(ui.doubleSpinBox_Ploshcha, SIGNAL(valueChanged (double)), this, SLOT(abonent_doubleSpinBox_Ploshcha_valueChanged()));
	connect(ui.spinBox_Ludy, SIGNAL(valueChanged (int)), this, SLOT(abonent_doubleSpinBox_Ploshcha_valueChanged()));
}
//------------------------------------------------------------
void tke_MainWindow::abonent_form_disconnect_editres()
{
	disconnect(ui.lineEdit_Prizv, SIGNAL(textEdited (const QString &)), this, SLOT(abonent_form_record_editing()));
	disconnect(ui.comboBox_Imya, SIGNAL(editTextChanged (const QString &)), this, SLOT(abonent_form_record_editing()));
	disconnect(ui.comboBox_Batk, SIGNAL(editTextChanged (const QString &)), this, SLOT(abonent_form_record_editing()));
	disconnect(ui.spinBox_Dilnycia, SIGNAL(valueChanged (int)), this, SLOT(abonent_form_record_editing()));
	disconnect(ui.comboBox_Budynok, SIGNAL(currentIndexChanged (int)), this, SLOT(abonent_form_record_editing()));
	disconnect(ui.spinBox_Kvartyra, SIGNAL(valueChanged (int)), this, SLOT(abonent_form_record_editing()));
	disconnect(ui.doubleSpinBox_Ploshcha, SIGNAL(valueChanged (double)), this, SLOT(abonent_form_record_editing()));
	disconnect(ui.doubleSpinBox_Zaiva_Ploshcha, SIGNAL(valueChanged (double)), this, SLOT(abonent_form_record_editing()));
	disconnect(ui.comboBox_Kategoria_Pilgy, SIGNAL(currentIndexChanged (int)), this, SLOT(abonent_form_record_editing()));
	disconnect(ui.spinBox_Pilga_for, SIGNAL(valueChanged (int)), this, SLOT(abonent_form_record_editing()));
	disconnect(ui.comboBox_Kategoria_Pilgy_2, SIGNAL(currentIndexChanged (int)), this, SLOT(abonent_form_record_editing()));
	disconnect(ui.spinBox_Pilga_for_2, SIGNAL(valueChanged (int)), this, SLOT(abonent_form_record_editing()));
	disconnect(ui.lineEdit_Telefon, SIGNAL(textEdited (const QString &)), this, SLOT(abonent_form_record_editing()));
	disconnect(ui.lineEdit_Identyf, SIGNAL(textEdited (const QString &)), this, SLOT(abonent_form_record_editing()));
	disconnect(ui.spinBox_Rahunok, SIGNAL(valueChanged (int)), this, SLOT(abonent_form_record_editing()));
	disconnect(ui.spinBox_Kimn, SIGNAL(valueChanged (int)), this, SLOT(abonent_form_record_editing()));
	disconnect(ui.spinBox_Ludy, SIGNAL(valueChanged (int)), this, SLOT(abonent_form_record_editing()));
	disconnect(ui.checkBox_Opal, SIGNAL(toggled(bool)), this, SLOT(checkBox_Opal_state_changed()));
	disconnect(ui.checkBox_Zdano_Pilgy, SIGNAL(toggled(bool)), this, SLOT(checkBox_Zdano_Pilgy_state_changed()));
	disconnect(ui.checkBox_IsSamovilnyk, SIGNAL(toggled(bool)), this, SLOT(checkBox_IsSamovilnyk_state_changed()));
	
	disconnect(ui.checkBox_GarVoda, SIGNAL(toggled(bool)), this, SLOT(checkBox_GarVoda_state_changed()));
	disconnect(ui.checkBox_narahStoyak, SIGNAL(toggled(bool)), this, SLOT(checkBox_narahStoyak_state_changed()));
	disconnect(ui.spinBox_BankRahunok, SIGNAL(valueChanged (int)), this, SLOT(abonent_form_record_editing()));
	disconnect(ui.groupBox_PilgsDatesLimits, SIGNAL(toggled(bool)), this, SLOT(abonent_form_record_editing()));
	
	disconnect(ui.dateEdit_PilgaBeginDate, SIGNAL(dateChanged(const QDate &)), this, SLOT(abonent_form_record_editing()));
	disconnect(ui.dateEdit_PilgaEndDate, SIGNAL(dateChanged(const QDate &)), this, SLOT(abonent_form_record_editing()));
	disconnect(ui.dateEdit_Pilga_2BeginDate, SIGNAL(dateChanged(const QDate &)), this, SLOT(abonent_form_record_editing()));
	disconnect(ui.dateEdit_Pilga_2EndDate, SIGNAL(dateChanged(const QDate &)), this, SLOT(abonent_form_record_editing()));
	
	disconnect(ui.lineEdit_kvartSufiks, SIGNAL(textEdited (const QString &)), this, SLOT(abonent_form_record_editing()));
	disconnect(ui.lineEdit_prymitka, SIGNAL(textEdited (const QString &)), this, SLOT(abonent_form_record_editing()));
	
		//частина обрахункових об'єднань
	disconnect(ui.doubleSpinBox_Ploshcha, SIGNAL(valueChanged (double)), this, SLOT(abonent_doubleSpinBox_Ploshcha_valueChanged()));
	disconnect(ui.spinBox_Ludy, SIGNAL(valueChanged (int)), this, SLOT(abonent_doubleSpinBox_Ploshcha_valueChanged()));
}
//------------------------------------------------------------
void tke_MainWindow::abonent_form_first()
{
	ui.spinBox_navigator->setValue( 1 );
}
//------------------------------------------------------------
void tke_MainWindow::abonent_form_previous()
{
	ui.spinBox_navigator->setValue( ui.spinBox_navigator->value()-1 );
}
//------------------------------------------------------------
void tke_MainWindow::abonent_form_next()
{
	ui.spinBox_navigator->setValue( ui.spinBox_navigator->value()+1 );
}
//------------------------------------------------------------
void tke_MainWindow::abonent_form_last()
{
	ui.spinBox_navigator->setValue( ui.spinBox_navigator->maximum() );
}
//------------------------------------------------------------
void tke_MainWindow::abonent_form_new()
{
#ifndef TKE_DEMO
	abonent_form_disconnect_editres();
	disconnect(ui.spinBox_navigator, SIGNAL(valueChanged (int)), this, SLOT(populate_abonent_form(int)));
	
	ui.pushButton_new->setEnabled( FALSE );
	if ( (new_record==1) && (record_edited==1) ){
		abonent_form_record_end_editing();
		abonent_form_insert_record();
		exec_abonent_select_query();
	}
	ui.spinBox_navigator->setMaximum( ui.spinBox_navigator->maximum()+1 );
	ui.spinBox_navigator->setValue( ui.spinBox_navigator->maximum() );
	//zadannya znachen za zamovchuvannyam
	abonent_temp_query->clear();
	abonent_temp_query->exec("SELECT max(Rahunok) FROM abonent");
	if (abonent_temp_query->next())
		ui.spinBox_Rahunok->setValue( abonent_temp_query->value(0).toInt()+1 );
	else ui.spinBox_Rahunok->setValue( 1 );
	abonent_temp_query->clear();
	ui.spinBox_BankRahunok->setValue( 0 );
	ui.lineEdit_Prizv->setText( "" );
	ui.comboBox_Imya->setEditText( "" );
	ui.comboBox_Batk->setEditText( "" );
	ui.spinBox_Dilnycia->setValue( 1 );
	ui.comboBox_Budynok->setCurrentIndex ( 0 );
	ui.spinBox_Kvartyra->setValue( 0 );
	ui.lineEdit_kvartSufiks->setText( "" );
	ui.doubleSpinBox_Ploshcha->setValue( 0 );
	ui.doubleSpinBox_Zaiva_Ploshcha->setValue( 0 );
	ui.comboBox_Kategoria_Pilgy->setCurrentIndex ( 0 );
	ui.comboBox_Kategoria_Pilgy_2->setCurrentIndex ( 0 );
	ui.spinBox_Pilga_for->setValue( 0 );
	ui.spinBox_Pilga_for_2->setValue( 0 );
	ui.lineEdit_Telefon->setText( "" );	
	ui.spinBox_Kimn->setValue( 0 );
	ui.spinBox_Ludy->setValue( 2 );
	ui.checkBox_Opal->setChecked( true );
	ui.checkBox_Zdano_Pilgy->setChecked( false );
	ui.lineEdit_Identyf->setText("");
	ui.checkBox_GarVoda->setChecked( false );
	ui.checkBox_narahStoyak->setChecked( false );
	ui.checkBox_IsSamovilnyk->setChecked( false );
	
	ui.groupBox_PilgsDatesLimits->setChecked( false );
	ui.dateEdit_PilgaBeginDate->setDate( date );
	ui.dateEdit_PilgaEndDate->setDate( date );
	ui.dateEdit_Pilga_2BeginDate->setDate( date);
	ui.dateEdit_Pilga_2EndDate->setDate( date );
	ui.lineEdit_prymitka->setText( "" );
	
	abonent_form_connect_editres();
	connect(ui.spinBox_navigator, SIGNAL(valueChanged (int)), this, SLOT(populate_abonent_form(int)));
	new_record = 1;
	record_edited=0;
#else
	QMessageBox::information(this,
				"!!! Демо-версія !!!",
				"З метою економії розміру у демо-версії не доступна ця функція.",
				QMessageBox::Ok,
				QMessageBox::NoButton);
#endif
}
//------------------------------------------------------------
void tke_MainWindow::abonent_form_record_editing()
{
	ui.line->setPalette ( *palette );
	ui.line_2->setPalette ( *palette );
	ui.line_3->setPalette ( *palette );
	ui.line_4->setPalette ( *palette );
	ui.pushButton_new->setEnabled( true );
	ui.pushButton_abonent_form_save->setEnabled( true );
	ui.pushButton_abonent_form_abort->setEnabled( true );
	record_edited = 1;
}
//------------------------------------------------------------
void tke_MainWindow::abonent_form_record_end_editing()
{
	ui.line->setPalette ( *default_palette );
	ui.line_2->setPalette ( *default_palette );
	ui.line_3->setPalette ( *default_palette );
	ui.line_4->setPalette ( *default_palette );
	ui.pushButton_abonent_form_save->setEnabled( false );
	ui.pushButton_abonent_form_abort->setEnabled( false );
	record_edited = 0;
}
//------------------------------------------------------------
void tke_MainWindow::abonent_form_update_record()
{
	//QSqlTableModel abonent_update_model;
	abonent_update_model->setTable("abonent");
	abonent_update_model->setFilter("id = " + abonent_select_query->value(0).toString());
	abonent_update_model->select();
	if (abonent_update_model->rowCount() == 1) {
		QSqlRecord record = abonent_update_model->record(0);
		record.setValue("Rahunok", ui.spinBox_Rahunok->value());
		record.setValue("BankRahunok", ui.spinBox_BankRahunok->value());
		record.setValue("Prizv", ui.lineEdit_Prizv->text());
		record.setValue("Imya", ui.comboBox_Imya->currentText());
		record.setValue("Batk", ui.comboBox_Batk->currentText());
		record.setValue("Dilnycia", ui.spinBox_Dilnycia->value());
		record.setValue("Budynok_ID", ui.comboBox_Budynok->itemData(ui.comboBox_Budynok->currentIndex(),Qt::UserRole).toInt());
		record.setValue("Kvartyra", ui.spinBox_Kvartyra->value());
		record.setValue("SufiksKvart", ui.lineEdit_kvartSufiks->text());
		record.setValue("Ploshcha", ui.doubleSpinBox_Ploshcha->value());
		record.setValue("Zaiva_ploshcha", ui.doubleSpinBox_Zaiva_Ploshcha->value());
		int bnd_val = pilgy_hash->value ( ui.comboBox_Kategoria_Pilgy->currentText() );
		record.setValue("Pilga_ID", bnd_val);
		record.setValue("Pilga_for", ui.spinBox_Pilga_for->value());
		bnd_val = pilgy_hash->value ( ui.comboBox_Kategoria_Pilgy_2->currentText() );
		record.setValue("Pilga_ID_2", bnd_val);
		record.setValue("Pilga_for_2", ui.spinBox_Pilga_for_2->value());
		record.setValue("tel", ui.lineEdit_Telefon->text());
		record.setValue("identyf", ui.lineEdit_Identyf->text());
		record.setValue("kimn", ui.spinBox_Kimn->value());
		record.setValue("ludy", ui.spinBox_Ludy->value());
		if (ui.checkBox_Opal->isChecked())
			record.setValue("opal", QVariant(1));
		else
			record.setValue("opal", QVariant(0));
		if (ui.checkBox_Zdano_Pilgy->isChecked())
			record.setValue("pilgy_zdani", QVariant(1));
		else
			record.setValue("pilgy_zdani", QVariant(0));
		if (ui.checkBox_GarVoda->isChecked())
			record.setValue("GarVoda", QVariant(1));
		else
			record.setValue("GarVoda", QVariant(0));
		if (ui.checkBox_narahStoyak->isChecked())
			record.setValue("NarahStoyak", QVariant(1));
		else
			record.setValue("NarahStoyak", QVariant(0));
		if (ui.checkBox_IsSamovilnyk->isChecked())
			record.setValue("SamovVidednCO", QVariant(1));
		else
			record.setValue("SamovVidednCO", QVariant(0));
		
		if (ui.groupBox_PilgsDatesLimits->isChecked())
			record.setValue("UsePilgyDateLimits", QVariant(1));
		else
			record.setValue("UsePilgyDateLimits", QVariant(0));
		
		record.setValue("PochPilgaDate", ui.dateEdit_PilgaBeginDate->date());
		record.setValue("KincPilgaDate", ui.dateEdit_PilgaEndDate->date());
		record.setValue("PochPilga_2Date", ui.dateEdit_Pilga_2BeginDate->date());
		record.setValue("KincPilga_2Date", ui.dateEdit_Pilga_2EndDate->date());
		record.setValue("Prymitka", ui.lineEdit_prymitka->text());
		
		abonent_update_model->setRecord(0, record);
		abonent_update_model->submitAll();
		abonent_update_model->clear();
	}
}
//------------------------------------------------------------
void tke_MainWindow::abonent_form_insert_record()
{
	QSqlQuery *query = new QSqlQuery();
	query->prepare("INSERT INTO abonent (Rahunok, Prizv, Imya, Batk, Dilnycia, Budynok_ID, Kvartyra, Ploshcha, Zaiva_ploshcha, Pilga_ID,\
										Pilga_for, Pilga_ID_2, Pilga_for_2, tel, identyf, kimn, ludy, opal, pilgy_zdani,\
										GarVoda, UsePilgyDateLimits, PochPilgaDate, KincPilgaDate, PochPilga_2Date, KincPilga_2Date, \
										SufiksKvart, Prymitka) \
					VALUES (:Rahunok, :Prizv, :Imya, :Batk, :Dilnycia, :Budynok_ID, :Kvartyra, :Ploshcha, :Zaiva_ploshcha, :Pilga_ID,\
										:Pilga_for, :Pilga_ID_2, :Pilga_for_2, :tel, :identyf, :kimn, :ludy, :opal, :pilgy_zdani, \
										:GarVoda, :UsePilgyDateLimits, :PochPilgaDate, :KincPilgaDate, :PochPilga_2Date, :KincPilga_2Date, \
										:SufiksKvart, :Prymitka)");
	query->bindValue(":Rahunok", ui.spinBox_Rahunok->value());
	query->bindValue(":BankRahunok", ui.spinBox_BankRahunok->value());
	query->bindValue(":Prizv", ui.lineEdit_Prizv->text());
	query->bindValue(":Imya", ui.comboBox_Imya->currentText());
	query->bindValue(":Batk", ui.comboBox_Batk->currentText());
	query->bindValue(":Dilnycia", ui.spinBox_Dilnycia->value());
	query->bindValue(":Budynok_ID", ui.comboBox_Budynok->itemData(ui.comboBox_Budynok->currentIndex(),Qt::UserRole).toInt());
	query->bindValue(":Kvartyra", ui.spinBox_Kvartyra->value());
	query->bindValue(":SufiksKvart", ui.lineEdit_kvartSufiks->text());
	query->bindValue(":Ploshcha", ui.doubleSpinBox_Ploshcha->value());
	query->bindValue(":Zaiva_ploshcha", ui.doubleSpinBox_Zaiva_Ploshcha->value());
	query->bindValue(":Pilga_ID", pilgy_hash->value ( ui.comboBox_Kategoria_Pilgy->currentText() ));
	query->bindValue(":Pilga_for", ui.spinBox_Pilga_for->value());
	query->bindValue(":Pilga_ID_2", pilgy_hash->value ( ui.comboBox_Kategoria_Pilgy_2->currentText() ));
	query->bindValue(":Pilga_for_2", ui.spinBox_Pilga_for_2->value());
	query->bindValue(":tel", ui.lineEdit_Telefon->text());
	query->bindValue(":identyf", ui.lineEdit_Identyf->text());
	query->bindValue(":kimn", ui.spinBox_Kimn->value());
	query->bindValue(":ludy", ui.spinBox_Ludy->value());
	if (ui.checkBox_Opal->isChecked())
		query->bindValue(":opal", -1);
	else
		query->bindValue(":opal", 0);
	if (ui.checkBox_Zdano_Pilgy->isChecked())
		query->bindValue(":pilgy_zdani", -1);
	else
		query->bindValue(":pilgy_zdani", 0);
	if (ui.checkBox_GarVoda->isChecked())
		query->bindValue(":GarVoda", QVariant(1));
	else
		query->bindValue(":GarVoda", QVariant(0));
	if (ui.checkBox_narahStoyak->isChecked())
		query->bindValue(":NarahStoyak", QVariant(1));
	else
		query->bindValue(":NarahStoyak", QVariant(0));
	if (ui.checkBox_IsSamovilnyk->isChecked())
		query->bindValue("SamovVidednCO", QVariant(1));
	else
		query->bindValue("SamovVidednCO", QVariant(0));
	if (ui.groupBox_PilgsDatesLimits->isChecked())
		query->bindValue(":UsePilgyDateLimits", QVariant(1));
	else
		query->bindValue(":UsePilgyDateLimits", QVariant(0));
	
	query->bindValue(":PochPilgaDate", ui.dateEdit_PilgaBeginDate->date());
	query->bindValue(":KincPilgaDate", ui.dateEdit_PilgaEndDate->date());
	query->bindValue(":PochPilga_2Date", ui.dateEdit_Pilga_2BeginDate->date());
	query->bindValue(":KincPilga_2Date", ui.dateEdit_Pilga_2EndDate->date());
	query->bindValue(":Prymitka", ui.lineEdit_prymitka->text());
	if (query->exec())
		QMessageBox::information(this,
				codec->toUnicode("!!! Додавання запису !!!"),
				codec->toUnicode("Запис успішно додано до бази даних."),
				QMessageBox::Ok,
				QMessageBox::NoButton);
	else
		QMessageBox::critical(this,
				codec->toUnicode("!!! Невдале додавання запису !!!"),
				codec->toUnicode("Програма не змогла додати запис до бази даних.\nПричина - ")
								+ query->lastError().text()+'\n'+query->lastQuery(),
				QMessageBox::Ok,
				QMessageBox::NoButton);
}
//------------------------------------------------------------
void tke_MainWindow::abonent_filtr_combobox_index_changed(int new_index)
{
	disconnect(ui.comboBox_Poshuk_rahunok, SIGNAL(currentIndexChanged (int)), ui.spinBox_navigator, SLOT(setValue(int))); //Обов'язковий
	disconnect(ui.comboBox_Poshuk_rahunok, SIGNAL(currentIndexChanged (int)), this, SLOT(abonent_filtr_combobox_index_changed(int)));
	
	ui.comboBox_Poshuk_rahunok->clearEditText();
	
	connect(ui.comboBox_Poshuk_rahunok, SIGNAL(currentIndexChanged (int)), ui.spinBox_navigator, SLOT(setValue(int))); //Обов'язковий
	connect(ui.comboBox_Poshuk_rahunok, SIGNAL(currentIndexChanged (int)), this, SLOT(abonent_filtr_combobox_index_changed(int)));
}
//------------------------------------------------------------
void tke_MainWindow::abonent_doubleSpinBox_Ploshcha_valueChanged()
{
	double zs = ui.doubleSpinBox_Ploshcha->value() - (ui.spinBox_Ludy->value() * 21 + 10.5);
	if (zs<0)
		zs=0;
	ui.doubleSpinBox_Zaiva_Ploshcha->setValue(zs);
}
//------------------------------------------------------------
void tke_MainWindow::pushButton_abonent_form_save_clicked()
{
	populate_abonent_form( ui.spinBox_navigator->value() );
}
//------------------------------------------------------------
void tke_MainWindow::pushButton_abonent_form_abort_clicked()
{
	abonent_form_record_end_editing();
	populate_abonent_form( ui.spinBox_navigator->value() );
}
//------------------------------------------------------------
void tke_MainWindow::diuch_taryf_widget_item_changed(QTableWidgetItem *item)
{
	item->setBackgroundColor( Qt::green );
}
//------------------------------------------------------------
void tke_MainWindow::diuch_taryf_save()
{
qDebug() << "void tke_MainWindow::diuch_taryf_save() loaded";
	ui.pushButton_diuch_taryf_save->setEnabled ( FALSE );
	diuch_taryf_update_model->setTable("diuch_taryf");
	for (int i=0; i<ui.tableWidget_diuch_taryf->rowCount(); i++){
		diuch_taryf_update_model->clear();
		diuch_taryf_update_model->setTable("diuch_taryf");
		if (ui.tableWidget_diuch_taryf->item( i, 1)->text().toInt() ==0 ){  //insert record
qDebug() << "insert record";
			diuch_taryf_update_model->insertRow(diuch_taryf_update_model->rowCount());
			QSqlRecord record = diuch_taryf_update_model->record(diuch_taryf_update_model->rowCount()-1);
			record.setValue("Budynok_ID", ui.tableWidget_diuch_taryf->item(i,0)->text());
			record.setValue("Year", ui.spinBox_year_diuch_taryf->value());
			record.setValue("Month", ui.spinBox_month_diuch_taryf->value());
			if (ui.tableWidget_diuch_taryf->item( i, 6)->text().toInt() !=0 ){ //nayavnist lichylnyka
qDebug() << "insert record->nayavnist lichylnyka";
				//record.setValue("Opal_days", QVariant(NULL));
				record.setValue("Lich_poch", ui.tableWidget_diuch_taryf->item( i, 3)->text().toDouble());
				record.setValue("Lich_kin", ui.tableWidget_diuch_taryf->item( i, 4)->text().toDouble());
			}
			else{ //vidsutnist lichylnyka
qDebug() << "insert record->vidsutnist lichylnyka";
				record.setValue("Opal_days", ui.tableWidget_diuch_taryf->item( i, 2)->text());
			}
			record.setValue("Taryf", ui.tableWidget_diuch_taryf->item( i, 5)->text().toDouble());
			record.setValue("ID", nextLichylnyk(QString("diuch_taryf")));
			diuch_taryf_update_model->setRecord(0, record);
			diuch_taryf_update_model->submitAll();
		}
		else{ //update record
qDebug() << "update record";
			diuch_taryf_update_model->setFilter("id = " + ui.tableWidget_diuch_taryf->item( i, 1)->text());
			diuch_taryf_update_model->select();
			if (diuch_taryf_update_model->rowCount() != 0) {
				QSqlRecord record = diuch_taryf_update_model->record(0);
				if (ui.tableWidget_diuch_taryf->item( i, 6)->text().toInt() !=0 ){ //nayavnist lichylnyka
qDebug() << "update record->nayavnist lichylnyka";
					record.setValue("Lich_poch", ui.tableWidget_diuch_taryf->item( i, 3)->text().toDouble());
					record.setValue("Lich_kin", ui.tableWidget_diuch_taryf->item( i, 4)->text().toDouble());		
				}
				else{ //vidsutnist lichylnyka
qDebug() << "update record->vidsutnist lichylnyka";
					record.setValue("Opal_days", ui.tableWidget_diuch_taryf->item( i, 2)->text());
				}
				record.setValue("Taryf", ui.tableWidget_diuch_taryf->item( i, 5)->text().toDouble());
				
				diuch_taryf_update_model->setRecord(0, record);
				diuch_taryf_update_model->submitAll();
			}
		}
	}
	diuch_taryf_update_model->clear();
	diuch_taryf_date_changed();
	ui.pushButton_diuch_taryf_save->setEnabled( TRUE );
}
//------------------------------------------------------------
void tke_MainWindow::diuch_taryf_calc()
{
qDebug() << "void tke_MainWindow::diuch_taryf_calc() loaded";
	QModelIndexList *modelIndexList = new QModelIndexList();
	*modelIndexList = ui.tableWidget_diuch_taryf->selectionModel()->selectedRows();
	ui.pushButton_diuch_taryf_calc->setEnabled( FALSE );
	UNasTaryfInfo taryfNas = nasTaryf(ui.spinBox_year_diuch_taryf->value(), ui.spinBox_month_diuch_taryf->value());
	double koef_pereved, zag_ploshcha, taryf, lich_poch, lich_kinc;
	for (int j=0; j<modelIndexList->size(); j++){
		taryf = 0;
		int i=modelIndexList->at(j).row();
		int cur_days;
		cur_days = ui.tableWidget_diuch_taryf->item( i, 2)->text().toInt();
		if ( !(cur_days < days_in_month[ui.spinBox_month_diuch_taryf->value()-1]) )
			taryf = taryfNas.normat_taryf_kvadr;  // taryf za povnyi misyac
		else
			taryf = taryfNas.normat_taryf_kvadr * cur_days / days_in_month[ui.spinBox_month_diuch_taryf->value()-1]; //taryf za nepovnyi misyac
		// Розрахунок частини тарифу по лічильнику
		abonent_temp_query->exec("SELECT Koef_pereved, Zag_ploshcha, Lichylnyk FROM budynky WHERE id=" + 
									ui.tableWidget_diuch_taryf->item(i, 0)->text());
		abonent_temp_query->next();
		if (abonent_temp_query->value(2).toInt() !=0 ){ //nayavnist lichylnyka
			koef_pereved = abonent_temp_query->value(0).toDouble();
			lich_poch = ui.tableWidget_diuch_taryf->item( i, 3)->text().toDouble();
			lich_kinc = ui.tableWidget_diuch_taryf->item( i, 4)->text().toDouble();
			zag_ploshcha = ui.tableWidget_diuch_taryf->item( i, 7)->text().toDouble();
			if (zag_ploshcha>0)
				taryf += (lich_kinc-lich_poch) * taryfNas.vart_g_kal / (koef_pereved*zag_ploshcha); //taryf po lichylnyku
		}
		ui.tableWidget_diuch_taryf->item(i, 5)->setData(Qt::DisplayRole, taryf);
	}
	abonent_temp_query->clear();
	delete modelIndexList;
	ui.pushButton_diuch_taryf_calc->setEnabled( TRUE );
}
//------------------------------------------------------------
void tke_MainWindow::diuch_taryf_print()
{
	UPrintDocs *printDocs = new UPrintDocs();
	printDocs->print_diuch_taryf(ui.spinBox_year_diuch_taryf->value() ,ui.spinBox_month_diuch_taryf->value());
	connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
	connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
	delete printDocs;
}
//------------------------------------------------------------
void tke_MainWindow::___pererah_saldo(int month, int year, bool all)
{
	bool done_ok=true;
	QSqlQuery *pererah_saldoQuery = new QSqlQuery();
	QSqlQuery *pererah_temp_saldoQuery = new QSqlQuery();
	QSqlQuery *pererah_select_abonentQuery = new QSqlQuery();
	int rahunok;
	double pr_vh_saldo, pr_narah, pr_narah_GV, pr_narah_ST, pr_subs, pr_subs_donarah, pr_oplach, pr_pereved;
	UPostupForm *postup_form = NULL;
	QString filtrString = "";
	
	if (all){
		pererah_select_abonentQuery->exec("SELECT count(*) FROM abonent");
		pererah_select_abonentQuery->seek(0);
		postup_form = new UPostupForm(0, pererah_select_abonentQuery->value(0).toInt() );
		postup_form->show();
	}
	else{
		filtrString = "WHERE Rahunok=" + QVariant(ui.spinBox_Rahunok->value()).toString(); 
	}
	pererah_select_abonentQuery->exec("SELECT Rahunok FROM abonent "+
										filtrString + " ORDER BY Rahunok");
	pererah_select_abonentQuery->seek(-1);
	while (pererah_select_abonentQuery->next()){
		rahunok = pererah_select_abonentQuery->value(0).toInt();
		if (!all){
			pererah_saldoQuery->exec("SELECT count(*) FROM narahuvannya \
							WHERE (Rahunok="+QVariant(rahunok).toString()+") \
							and ((year>"+QVariant(year).toString()+") \
							or	((year="+QVariant(year).toString()+") \
							and (month>="+QVariant(month).toString()+"))) \
							and (Status=0) \
							ORDER BY year, month");
			pererah_saldoQuery->seek(0);
			postup_form = new UPostupForm(0, pererah_saldoQuery->value(0).toInt() );
			postup_form->show();
		}
		
		pererah_saldoQuery->exec("SELECT id, Rahunok, Year, Month, Vh_Saldo, Narah, Narah_GV, Narah_St  \
						FROM narahuvannya \
						WHERE (Rahunok="+QVariant(rahunok).toString()+") \
						and ((year>"+QVariant(year).toString()+") \
						or	((year="+QVariant(year).toString()+") \
						and (month>="+QVariant(month).toString()+"))) \
						and (Status=0) \
						ORDER BY year, month");
		pererah_saldoQuery->seek(0);
		pr_vh_saldo = pererah_saldoQuery->value(4).toDouble();
		pererah_temp_saldoQuery->exec("SELECT sum(Narah), sum(Narah_GV), sum(Narah_St) FROM narahuvannya \
									WHERE Rahunok="+QVariant(rahunok).toString()+" \
										and year="+pererah_saldoQuery->value(2).toString()+" \
										and month="+pererah_saldoQuery->value(3).toString());
		if (pererah_temp_saldoQuery->seek(0)){
			pr_narah = pererah_temp_saldoQuery->value(0).toDouble();
			pr_narah_GV = pererah_temp_saldoQuery->value(1).toDouble();
			pr_narah_ST = pererah_temp_saldoQuery->value(2).toDouble();
		}
		else{
			pr_narah = 0;
			pr_narah_GV = 0;
			pr_narah_ST = 0;
		}
		//Обрахунок субсидії
		pererah_temp_saldoQuery->exec("SELECT sum(Suma), sum(Suma_d), sum(Suma_GV), sum(Suma_d_GV) FROM subs \
								WHERE (Rahunok_ID="+QVariant(rahunok).toString()+") \
									and (Month="+pererah_saldoQuery->value(3).toString()+") \
									and (Year="+pererah_saldoQuery->value(2).toString()+")");
		if (pererah_temp_saldoQuery->seek(0)){
			pr_subs = pererah_temp_saldoQuery->value(0).toDouble() + pererah_temp_saldoQuery->value(2).toDouble();
			pr_subs_donarah = pererah_temp_saldoQuery->value(1).toDouble() + pererah_temp_saldoQuery->value(3).toDouble();
		}
		else{
			pr_subs = 0;
			pr_subs_donarah = 0;
		}
		//обрахунок оплати
		pererah_temp_saldoQuery->exec("SELECT SUM(Suma) FROM (SELECT * FROM kvytancii \
								WHERE (Rahunok_id="+ QVariant(rahunok).toString() +") \
								and (year(Plata_date)="+ pererah_saldoQuery->value(2).toString() +") \
								and (month(Plata_date)="+ pererah_saldoQuery->value(3).toString() +")) \
								GROUP BY Rahunok_ID");
		
		if (pererah_temp_saldoQuery->seek(0))
			pr_oplach = pererah_temp_saldoQuery->value(0).toDouble();
		else
			pr_oplach = 0;
		
		//Читання перерахунку
		pererah_temp_saldoQuery->exec("SELECT sum(Suma) FROM narahuvannyaPereved \
							WHERE Rahunok_id="+sqlStr(rahunok)+" \
								and year(CDate)="+pererah_saldoQuery->value(2).toString()+" \
								and month(CDate)="+pererah_saldoQuery->value(3).toString());
		if (pererah_temp_saldoQuery->next())
			pr_pereved = uDToM(pererah_temp_saldoQuery->value(0).toDouble());
		else
			pr_pereved = 0;
			
		while (pererah_saldoQuery->next()){
			double vh_saldo = pr_vh_saldo + pr_narah + pr_narah_GV + pr_narah_ST - pr_subs - pr_subs_donarah - pr_oplach + pr_pereved;
			vh_saldo = d_to_m(vh_saldo);
			pererah_temp_saldoQuery->exec("SELECT id, Vh_saldo FROM narahuvannya WHERE (Rahunok=" + 
										QVariant(rahunok).toString() + ") and (year=" + pererah_saldoQuery->value(2).toString() + 
										") and (month=" + pererah_saldoQuery->value(3).toString() + ") and (Status=0)");
			if (pererah_temp_saldoQuery->seek(0)){
				if ( d_to_m(pererah_temp_saldoQuery->value(1).toDouble()) != vh_saldo ){
					bool done_operation = pererah_temp_saldoQuery->exec("UPDATE narahuvannya SET Vh_saldo="+sqlStr(vh_saldo)+" \
																		WHERE id="+pererah_temp_saldoQuery->value(0).toString());
					done_ok = done_ok && done_operation;
					if (!done_operation){
						qDebug() << "Не змінено вхідного сальдо абонента - " + QVariant(rahunok).toString() +
									", за " + pererah_saldoQuery->value(3).toString() + "." +
									pererah_saldoQuery->value(2).toString() + "\n" +
									"помилка - "+ pererah_temp_saldoQuery->lastError().text() +
									"\n" + uMToStr2(vh_saldo);
					}
				}
			}
		
			pr_vh_saldo = vh_saldo;
			
			pererah_temp_saldoQuery->exec("SELECT sum(Narah), sum(Narah_GV), sum(Narah_St) FROM narahuvannya \
										WHERE Rahunok="+QVariant(rahunok).toString()+" \
											and year="+pererah_saldoQuery->value(2).toString()+" \
											and month="+pererah_saldoQuery->value(3).toString());
			if (pererah_temp_saldoQuery->seek(0)){
				pr_narah = pererah_temp_saldoQuery->value(0).toDouble();
				pr_narah_GV = pererah_temp_saldoQuery->value(1).toDouble();
				pr_narah_ST = pererah_temp_saldoQuery->value(2).toDouble();
			}
			else{
				pr_narah = 0;
				pr_narah_GV = 0;
				pr_narah_ST = 0;
			}
			//Обрахунок субсидії
			pererah_temp_saldoQuery->exec("SELECT sum(Suma), sum(Suma_d), sum(Suma_GV), sum(Suma_d_GV) FROM subs \
								WHERE (Rahunok_ID="+QVariant(rahunok).toString()+") \
								and (Month="+pererah_saldoQuery->value(3).toString()+") \
								and (Year="+pererah_saldoQuery->value(2).toString()+")");
			if (pererah_temp_saldoQuery->seek(0)){
				pr_subs = pererah_temp_saldoQuery->value(0).toDouble() + pererah_temp_saldoQuery->value(2).toDouble();
				pr_subs_donarah = pererah_temp_saldoQuery->value(1).toDouble() + pererah_temp_saldoQuery->value(3).toDouble();
			}
			else{
				pr_subs = 0;
				pr_subs_donarah = 0;
			}
			//обрахунок оплати
			pererah_temp_saldoQuery->exec("SELECT SUM(Suma) FROM (SELECT * FROM kvytancii \
								WHERE (Rahunok_id="+ QVariant(rahunok).toString() +") \
								and (year(Plata_date)="+ pererah_saldoQuery->value(2).toString() +") \
								and (month(Plata_date)="+ pererah_saldoQuery->value(3).toString() +")) \
								GROUP BY Rahunok_ID");	
			if (pererah_temp_saldoQuery->seek(0))
				pr_oplach = pererah_temp_saldoQuery->value(0).toDouble();
			else
				pr_oplach = 0;
			
			//Читання перерахунку
			pererah_temp_saldoQuery->exec("SELECT sum(Suma) FROM narahuvannyaPereved \
								WHERE Rahunok_id="+sqlStr(rahunok)+" \
									and year(CDate)="+pererah_saldoQuery->value(2).toString()+" \
									and month(CDate)="+pererah_saldoQuery->value(3).toString());
			if (pererah_temp_saldoQuery->next())
				pr_pereved = uDToM(pererah_temp_saldoQuery->value(0).toDouble());
			else
				pr_pereved = 0;
			
			if (!all)
				postup_form->inc_pos();
		}
	if (all)
		postup_form->inc_pos();
	}
	if (done_ok)
		QMessageBox::information(this,
				codec->toUnicode("!!! Закінчено перерахунок вхідних сальдо !!!"),
				codec->toUnicode("Успішно закінчено перерахунок вхідних сальдо."),
				QMessageBox::Ok,
				QMessageBox::NoButton);
	else
		QMessageBox::critical(this,
				codec->toUnicode("!!! Невдалий перерахунок вхідних сальдо !!!"),
				codec->toUnicode("Програма не змогла записати одне(кілька) вхідних сальдо.\nСпробуйте ще раз."),
				QMessageBox::Ok,
				QMessageBox::NoButton);
	delete pererah_saldoQuery;
	delete pererah_temp_saldoQuery;
	delete postup_form;
}
//------------------------------------------------------------
void tke_MainWindow::curDateChanged()
{
	currentSysDate = dateEdit_PotochnaData->date();
	short curYear = currentSysDate.year();
	short curMonth = currentSysDate.month();
	ui.spinBox_year_filtr->setValue(curYear);
	
	ui.spinBox_month_diuch_taryf->setValue(curMonth);
	ui.spinBox_year_diuch_taryf->setValue(curYear);
	ui.spinBox_month_korektura->setValue(curMonth);
	ui.spinBox_year_korektura->setValue(curYear);
	ui.spinBox_month_subs->setValue(curMonth);
	ui.spinBox_year_subs->setValue(curYear);
	ui.spinBox_kvyt_opl_month->setValue(curMonth);
	ui.spinBox_kvyt_opl_year->setValue(curYear);
	ui.spinBox_kvyt_pereved_month->setValue(curMonth);
	ui.spinBox_kvyt_pereved_year->setValue(curYear);
	ui.spinBox_month_korektura_s->setValue(curMonth);
	ui.spinBox_year_korektura_s->setValue(curYear);
	
	d_Year = curYear;
	d_Month = curMonth;	
	d_Month_poch = curMonth;
	d_Year_poch = curYear;
	d_Month_end = curMonth;
	d_Year_end = curYear;
	d_Splata_date = currentSysDate;
	d_Perevid_Date = currentSysDate;
	
	showBeznDebitZabordFlag();
}
//------------------------------------------------------------
void tke_MainWindow::calculatorButton_clicked()
{
	QProcess::startDetached("calc");
}
//------------------------------------------------------------
void tke_MainWindow::populateLichPokaznAbon()
{
	//Заповнення інформації про лічильник ЦО
	QSqlQuery lichQuery;
	lichQuery.exec("SELECT Lichylnyk FROM (SELECT Budynok_ID FROM abonent \
										WHERE Rahunok="+ui.spinBox_Rahunok->text()+")  AS abonent \
					LEFT JOIN budynky ON budynky.id = abonent.Budynok_ID");
	lichQuery.seek(0);
	if (lichQuery.value(0).toInt() != 0){
		ui.toolButton_lichCOInfo->setText("Ліч.ЦО: Ліч");
		lichQuery.exec("SELECT diuch_taryf.Lich_poch, diuch_taryf.Lich_kin, abonent.Ploshcha, \
								budynky.Koef_pereved, abonent.Budynok_ID \
						FROM( (SELECT * FROM abonent \
								WHERE Rahunok="+ui.spinBox_Rahunok->text()+")  AS abonent \
						LEFT JOIN (SELECT * FROM diuch_taryf \
								WHERE year="+QString::number(currentSysDate.year())+" \
									and month="+QString::number(currentSysDate.month())+") AS diuch_taryf \
								ON diuch_taryf.Budynok_ID = abonent.Budynok_ID) \
						LEFT JOIN budynky ON budynky.id = abonent.Budynok_ID");
		lichQuery.seek(0);
		double pokaznLich = lichQuery.value(1).toDouble() - lichQuery.value(0).toDouble();
		double plosAbon = lichQuery.value(2).toDouble();
		double koef = lichQuery.value(3).toDouble();
		lichQuery.exec("SELECT opal_ploshcha_bud.ZagPloshcha, opal_st_ploshcha_bud.ZagStPloshcha \
								FROM (budynky \
								LEFT JOIN (SELECT Budynok_ID, sum(Ploshcha) AS ZagPloshcha \
											FROM abonent WHERE Opal=true GROUP BY Budynok_ID) \
											AS opal_ploshcha_bud ON budynky.id = opal_ploshcha_bud.Budynok_ID) \
								LEFT JOIN (SELECT abonent.Budynok_ID, sum(pok.Plos) AS ZagStPloshcha FROM (SELECT Rahunok_ID, Plos \
											FROM pokaznOpalStoyak \
											WHERE Year="+QString::number(currentSysDate.year())+" and Month="+QString::number(currentSysDate.month())+") AS pok \
											LEFT JOIN abonent ON abonent.Rahunok=pok.Rahunok_ID \
											GROUP BY abonent.Budynok_ID) AS opal_st_ploshcha_bud ON budynky.id = opal_st_ploshcha_bud.Budynok_ID \
								WHERE budynky.id="+lichQuery.value(4).toString());
		lichQuery.seek(0);
		double zagOpalPlos = lichQuery.value(0).toDouble() + lichQuery.value(1).toDouble();
		if (zagOpalPlos > 0.009){
			if (koef > 0.00009){
				double pokaznAbon = pokaznLich * plosAbon / (koef * zagOpalPlos);
				ui.toolButton_lichCOInfo->setText("Ліч.ЦО: "+QString::number(pokaznAbon, 'f', 6)+" ГКал");
			}
			else
				ui.toolButton_lichCOInfo->setText("Ліч.ЦО: Невірний коеф.ліч.");
		}
		else
			ui.toolButton_lichCOInfo->setText("Ліч.ЦО: Нульова опал.площа ГКал");
	}
	else
		ui.toolButton_lichCOInfo->setText("Ліч.ЦО: НЕМАЄ");
	//Заповнення інформації про лічильник ГВ
	d.nDate = currentSysDate;
	d.rah = ui.spinBox_Rahunok->value();
	d.ludy = ui.spinBox_Ludy->value();
	double pokaznGV = f_read_vykor_GV(&d);
	short budLichCount=0, abonLichCount=0;
	bool isPokaznBud=false, isPokaznAbon=false;
	lichQuery.exec("SELECT lichBud.IsLichylnyk, pokaznLichBudCur.Pokazn, pokaznLichBudCur.FactVykor, lichylnykyAbon.IsLichylnyk, \
						pokaznLichAbonCur.Pokazn, pokaznLichAbonCur.FactVykor \
					FROM ((((SELECT * FROM abonent WHERE Rahunok="+ui.spinBox_Rahunok->text()+") AS abon \
					LEFT JOIN (SELECT * FROM lichylnykyBudynkovi WHERE (SELECT max(Kvartyra) \
						FROM abonent WHERE Rahunok="+ui.spinBox_Rahunok->text()+") BETWEEN lichylnykyBudynkovi.StartKvNumber \
							AND lichylnykyBudynkovi.EndKvNumber)  AS lichBud ON lichBud.Budynok_id=abon.Budynok_id) \
					LEFT JOIN (SELECT * FROM pokaznLichBud WHERE year(pokaznDate)="+sqlStr(currentSysDate.year())+" \
							and month(pokaznDate)="+sqlStr(currentSysDate.month())+") AS pokaznLichBudCur ON pokaznLichBudCur.BudLich_id=lichBud.id) \
					LEFT JOIN (lichylnykyAbon) ON lichylnykyAbon.Rahunok=abon.Rahunok) \
					LEFT JOIN (SELECT * FROM pokaznLichAbon WHERE year(pokaznDate)="+sqlStr(currentSysDate.year())+" \
							and month(pokaznDate)="+sqlStr(currentSysDate.month())+") AS pokaznLichAbonCur ON pokaznLichAbonCur.AbonLich_id=lichylnykyAbon.id");
	while (lichQuery.next()){
		if (lichQuery.value(0).toBool())
			budLichCount++;
		if (lichQuery.value(3).toBool())
			abonLichCount++;
		if ((lichQuery.value(1).toDouble()>0.0000009) || (lichQuery.value(2).toDouble()>0.0000009))
			isPokaznBud = true;
		if ((lichQuery.value(4).toDouble()>0.0000009) || (lichQuery.value(5).toDouble()>0.0000009))
			isPokaznAbon = true;
	}
	ui.toolButton_lichGVInfo->setText("Ліч.ГВ: Буд: "+QString::number(budLichCount)+"X пок-"+(isPokaznBud?"Є":"Ні")+";  Абон: "+QString::number(abonLichCount)+"X пок-"+(isPokaznAbon?"Є":"Ні")+". Показн.: "+QString::number(pokaznGV, 'f', 4)+" м.куб.");	
}
//------------------------------------------------------------
void tke_MainWindow::showAbonLichCO()
{
	if (!ui.action_diuchyi_taryf->isChecked()){
		ui.action_diuchyi_taryf->setChecked( true );
		ui.dockWidget_diuch_taryf->setVisible( true );
	}
	
	ui.spinBox_year_diuch_taryf->setValue(currentSysDate.year());
	ui.spinBox_month_diuch_taryf->setValue(currentSysDate.month());
	
	int budynokId = ui.comboBox_Budynok->itemData(ui.comboBox_Budynok->currentIndex(),Qt::UserRole).toInt();
	for (int row=0; row<ui.tableWidget_diuch_taryf->rowCount(); row++)
		if (ui.tableWidget_diuch_taryf->item(row,0)->data(Qt::EditRole).toInt() == budynokId){
			ui.tableWidget_diuch_taryf->setFocus();
			ui.tableWidget_diuch_taryf->setCurrentCell(row, 2);
			ui.tableWidget_diuch_taryf->scrollToItem(ui.tableWidget_diuch_taryf->item(row,2));
			return;
		}
	
	QMessageBox::information(0,
				"!!! Невдалий пошук показника лічильника ЦО !!!",
				"Програмі не вдалося знайти необхідний показник будинкового лічильника ЦО.",
				QMessageBox::Ok,
				QMessageBox::NoButton);
}
//------------------------------------------------------------
void tke_MainWindow::showAbonLichGV()
{
	tke_Form_insert_pokazn_lich_GV * pokaznForm = NULL;
	bool neadCreateForm = true;
	foreach (QWidget *widget, QApplication::topLevelWidgets())
		if (widget->objectName()=="tke_Form_insert_pokazn_lich_GV"){
			pokaznForm = qobject_cast<tke_Form_insert_pokazn_lich_GV *>(widget);
			if (pokaznForm){
				neadCreateForm = false;
				break;
			}
		}
	if (neadCreateForm){
		pokaznForm = new tke_Form_insert_pokazn_lich_GV(currentSysDate.year(), currentSysDate.month());
		
		connect(this, SIGNAL(mainWindowClosed()), pokaznForm, SLOT(close()));
		connect(this, SIGNAL(closeLoginSignal()), pokaznForm, SLOT(close()));
		connect(pokaznForm, SIGNAL(requestRahunok(int)), this, SLOT(show_rahunok(int)));
	}
	pokaznForm->hide();
	pokaznForm->show();
	pokaznForm->findAbonentPokazn(ui.spinBox_Rahunok->value());
}
//------------------------------------------------------------
void tke_MainWindow::clear_operacii_table()
{
	for (int col=0; col<ui.tableWidget_rozrahunky->columnCount()-1; col++)
		for (int row=0; row<ui.tableWidget_rozrahunky->rowCount()-1; row++)
			ui.tableWidget_rozrahunky->item(row, col)->setText("-");
}
//------------------------------------------------------------
void tke_MainWindow::calc_operacii_table()
{
	int col, row;
	double sum;
	for (col=1; col<ui.tableWidget_rozrahunky->columnCount()-1; col++){  //Обчислення сум по нарахуваннях стовбцях таблиці
		sum = 0;
		for (row=0; row<ui.tableWidget_rozrahunky->rowCount()-1; row++){
			sum += ui.tableWidget_rozrahunky->item(row, col)->text().toDouble();
		}
		ui.tableWidget_rozrahunky->item(ui.tableWidget_rozrahunky->rowCount()-1, col)->setText(uMToStr2(sum));
	}
	for (row=0; row<ui.tableWidget_rozrahunky->rowCount()-1; row++){
		sum  = ui.tableWidget_rozrahunky->item(row, 0)->text().toDouble();
		sum += ui.tableWidget_rozrahunky->item(row, 1)->text().toDouble();
		sum += ui.tableWidget_rozrahunky->item(row, 3)->text().toDouble();
		sum += ui.tableWidget_rozrahunky->item(row, 5)->text().toDouble();
		sum -= ui.tableWidget_rozrahunky->item(row, 6)->text().toDouble();
		sum -= ui.tableWidget_rozrahunky->item(row, 7)->text().toDouble();
		sum -= ui.tableWidget_rozrahunky->item(row, 8)->text().toDouble();
		sum -= ui.tableWidget_rozrahunky->item(row, 9)->text().toDouble();
		sum -= ui.tableWidget_rozrahunky->item(row, 10)->text().toDouble();
		sum += ui.tableWidget_rozrahunky->item(row, 15)->text().toDouble();
		ui.tableWidget_rozrahunky->item(row, 16)->setText(uMToStr2(sum));
	}
}
//------------------------------------------------------------
void tke_MainWindow::buttonclick_add_budynky()
{
	int row = model_budynky->rowCount();
	model_budynky->insertRow(row);

	QModelIndex index = model_budynky->index(row, 1);
	ui.tableView_budynky->setCurrentIndex(index);
	ui.tableView_budynky->edit(index);
}
//------------------------------------------------------------
void tke_MainWindow::buttonclick_del_budynky()
{
	if	( ! QMessageBox::question(
                this,
                codec->toUnicode("!!! Видалення стрічки !!!"),
                codec->toUnicode("Ви дійсно бажаєте видалити сирічку?"),
                codec->toUnicode("Так"),
				codec->toUnicode("Ні-ні-ні"),
                QString(), 0, 1)
		)
		model_budynky->removeRow(ui.tableView_budynky->currentIndex().row());
}
//------------------------------------------------------------
void tke_MainWindow::buttonclick_add_pilgy()
{
	int row = model_pilgy->rowCount();
	model_pilgy->insertRow(row);
	QModelIndex index = model_pilgy->index(row, 1);
	ui.tableView_pilgy->setCurrentIndex(index);
	ui.tableView_pilgy->edit(index);
}
//------------------------------------------------------------
void tke_MainWindow::buttonclick_del_pilgy()
{
	if	( ! QMessageBox::question(
                this,
                codec->toUnicode("!!! Видалення стрічки !!!"),
                codec->toUnicode("Ви дійсно бажаєте видалити сирічку?"),
                codec->toUnicode("Так"),
				codec->toUnicode("Ні-ні-ні"),
                QString(), 0, 1)
		)
		model_pilgy->removeRow(ui.tableView_pilgy->currentIndex().row());
}
//------------------------------------------------------------
void tke_MainWindow::buttonclick_add_normat_taryf()
{
	int row = model_normat_taryf->rowCount();
	model_normat_taryf->insertRow(row);
	
	QSqlRecord record = model_normat_taryf->record(row);
	record.setValue("id", nextLichylnyk("normat_taryf"));
	record.setValue("year", QVariant(d_Year));
	record.setValue("month", QVariant(d_Month));
	model_normat_taryf->setRecord(row, record);
	
	QModelIndex index = model_normat_taryf->index(row, 1);
	ui.tableView_normat_taryf->setCurrentIndex(index);
	ui.tableView_normat_taryf->edit(index);
}
//------------------------------------------------------------
void tke_MainWindow::buttonclick_del_normat_taryf()
{
	if	( ! QMessageBox::question(
                this,
                codec->toUnicode("!!! Видалення стрічки !!!"),
                codec->toUnicode("Ви дійсно бажаєте видалити сирічку?"),
                codec->toUnicode("Так"),
				codec->toUnicode("Ні-ні-ні"),
                QString(), 0, 1)
		)
		model_normat_taryf->removeRow(ui.tableView_normat_taryf->currentIndex().row());
}
//------------------------------------------------------------
void tke_MainWindow::buttonclick_add_korektura()
{
	disconnect(ui.tableView_korektura->selectionModel(), SIGNAL(currentRowChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(look_abonent_info( const QModelIndex &, const QModelIndex & ))); //позначено запис --> висвітлення інформації про абонента
	int row = model_korektura->rowCount();
	model_korektura->insertRow(row);
	QModelIndex index = model_korektura->index(row, 4);
	model_korektura->setData(index, QVariant(ui.spinBox_month_korektura->value()));
	index = model_korektura->index(row, 5);
	model_korektura->setData(index, QVariant(ui.spinBox_year_korektura->value()));
	index = model_korektura->index(row, 1);
	ui.tableView_korektura->setCurrentIndex(index);
	ui.tableView_korektura->edit(index);
	connect(ui.tableView_korektura->selectionModel(), SIGNAL(currentRowChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(look_abonent_info( const QModelIndex &, const QModelIndex & ))); //позначено запис --> висвітлення інформації про абонента
}
//------------------------------------------------------------
void tke_MainWindow::buttonclick_del_korektura()
{
	if	( ! QMessageBox::question(
                this,
                codec->toUnicode("!!! Видалення стрічки !!!"),
                codec->toUnicode("Ви дійсно бажаєте видалити сирічку?"),
                codec->toUnicode("Так"),
				codec->toUnicode("Ні-ні-ні"),
                QString(), 0, 1)
		)
		model_korektura->removeRow(ui.tableView_korektura->currentIndex().row());
}
//------------------------------------------------------------
void tke_MainWindow::buttonclick_add_korektura_s()
{
	disconnect(ui.tableView_korektura_s->selectionModel(), SIGNAL(currentRowChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(look_abonent_info( const QModelIndex &, const QModelIndex & ))); //позначено запис --> висвітлення інформації про абонента
	int row = model_korektura_s->rowCount();
	model_korektura_s->insertRow(row);
	QModelIndex index = model_korektura_s->index(row, 5);
	model_korektura_s->setData(index, QVariant(ui.spinBox_month_korektura_s->value()));
	index = model_korektura_s->index(row, 6);
	model_korektura_s->setData(index, QVariant(ui.spinBox_year_korektura_s->value()));
	index = model_korektura_s->index(row, 1);
	ui.tableView_korektura_s->setCurrentIndex(index);
	ui.tableView_korektura_s->edit(index);
	connect(ui.tableView_korektura_s->selectionModel(), SIGNAL(currentRowChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(look_abonent_info( const QModelIndex &, const QModelIndex & ))); //позначено запис --> висвітлення інформації про абонента
}
//------------------------------------------------------------
void tke_MainWindow::buttonclick_del_korektura_s()
{
	if	( ! QMessageBox::question(
                this,
                codec->toUnicode("!!! Видалення стрічки !!!"),
                codec->toUnicode("Ви дійсно бажаєте видалити сирічку?"),
                codec->toUnicode("Так"),
				codec->toUnicode("Ні-ні-ні"),
                QString(), 0, 1)
		)
		model_korektura_s->removeRow(ui.tableView_korektura_s->currentIndex().row());
}
//------------------------------------------------------------
void tke_MainWindow::buttonclick_add_subs()
{
	disconnect(ui.tableView_subs->selectionModel(), SIGNAL(currentRowChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(look_abonent_info( const QModelIndex &, const QModelIndex & ))); //позначено запис --> висвітлення інформації про абонента
	int row = model_subs->rowCount();
	model_subs->insertRow(row);
	QModelIndex index = model_subs->index(row, 2);
	model_subs->setData(index, QVariant(ui.spinBox_year_subs->value()));
	index = model_subs->index(row, 3);
	model_subs->setData(index, QVariant(ui.spinBox_month_subs->value()));
	index = model_subs->index(row, 1);
	ui.tableView_subs->setCurrentIndex(index);
	ui.tableView_subs->edit(index);
	connect(ui.tableView_subs->selectionModel(), SIGNAL(currentRowChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(look_abonent_info( const QModelIndex &, const QModelIndex & ))); //позначено запис --> висвітлення інформації про абонента
}
//------------------------------------------------------------
void tke_MainWindow::buttonclick_del_subs()
{
	if	( ! QMessageBox::question(
                this,
                codec->toUnicode("!!! Видалення стрічки !!!"),
                codec->toUnicode("Ви дійсно бажаєте видалити сирічку?"),
                codec->toUnicode("Так"),
				codec->toUnicode("Ні-ні-ні"),
                QString(), 0, 1)
		)
		model_subs->removeRow(ui.tableView_subs->currentIndex().row());
}
//------------------------------------------------------------
void tke_MainWindow::buttonclick_open_insert_subs_form()
{
	tke_Form_insert_subs *formSubs = new tke_Form_insert_subs(0, ui.spinBox_year_subs->value(), ui.spinBox_month_subs->value());
	connect(formSubs, SIGNAL(u_abonent_rahunok_changes(int)), this, SLOT(show_rahunok(int)));
	formSubs->show();
}
//------------------------------------------------------------
void tke_MainWindow::buttonclick_open_insert_subs_narah_form()
{
	tke_Form_insert_subs *formSubs = new tke_Form_insert_subs(0, ui.spinBox_year_subs->value(), ui.spinBox_month_subs->value(),false);
	connect(formSubs, SIGNAL(u_abonent_rahunok_changes(int)), this, SLOT(show_rahunok(int)));
	formSubs->show();
}
//------------------------------------------------------------
void tke_MainWindow::buttonclick_add_kvyt()
{
	disconnect(ui.tableView_kvyt->selectionModel(), SIGNAL(currentRowChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(look_abonent_info( const QModelIndex &, const QModelIndex & ))); //позначено запис --> висвітлення інформації про абонента
	int row = model_kvytancii->rowCount();
	model_kvytancii->insertRow(row);

	QModelIndex index = model_kvytancii->index(row, 1);
	ui.tableView_kvyt->setCurrentIndex(index);
	ui.tableView_kvyt->edit(index);
	connect(ui.tableView_kvyt->selectionModel(), SIGNAL(currentRowChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(look_abonent_info( const QModelIndex &, const QModelIndex & ))); //позначено запис --> висвітлення інформації про абонента
}
//------------------------------------------------------------
void tke_MainWindow::buttonclick_del_kvyt()
{
	if	( ! QMessageBox::question(
                this,
                codec->toUnicode("!!! Видалення стрічки !!!"),
                codec->toUnicode("Ви дійсно бажаєте видалити сирічку?"),
                codec->toUnicode("Так"),
				codec->toUnicode("Ні-ні-ні"),
                QString(), 0, 1)
		)
		model_kvytancii->removeRow(ui.tableView_kvyt->currentIndex().row());
}
//------------------------------------------------------------
void tke_MainWindow::buttonclick_new_pack_kvyt()
{
	QSqlQuery *packQuery = new QSqlQuery();
	packQuery->exec("SELECT max(Pachka) FROM kvytancii WHERE year(Plata_date)="+QVariant(date.year()).toString());
	int newPack;
	if (packQuery->seek(0))
		newPack = packQuery->value(0).toInt() + 1;
	else 
		newPack = 1;
	ui.spinBox_kvyt_pachka->setValue(newPack);
	d_Pachka = newPack;
	ui.groupBox_kvyt_pachka->setChecked ( true );
	ui.groupBox_kvyt_oplata->setChecked ( true );
	delete packQuery;
}
//------------------------------------------------------------
void tke_MainWindow::buttonclick_insert_kvyt_form()
{
	tke_Form_insert_kvyt *formKvyt = new tke_Form_insert_kvyt(0, currentSysDate.year(), currentSysDate.month());
	connect(formKvyt, SIGNAL(u_abonent_rahunok_changes(int)), this, SLOT(show_rahunok(int)));
	formKvyt->show();
}
//------------------------------------------------------------
void tke_MainWindow::buttonclick_add_spravy_subs()
{
	disconnect(ui.tableView_spravy_subs, SIGNAL(clicked ( const QModelIndex & )), this, SLOT(look_abonent_info( const QModelIndex &, const QModelIndex & ))); //позначено запис --> висвітлення інформації про абонента
	int row = model_spravy_subs->rowCount();
	model_spravy_subs->insertRow(row);

	QModelIndex index = model_spravy_subs->index(row, 1);
	ui.tableView_spravy_subs->setCurrentIndex(index);
	ui.tableView_spravy_subs->edit(index);
	connect(ui.tableView_spravy_subs, SIGNAL(clicked ( const QModelIndex & )), this, SLOT(look_abonent_info( const QModelIndex &, const QModelIndex & ))); //позначено запис --> висвітлення інформації про абонента
}
//------------------------------------------------------------
void tke_MainWindow::buttonclick_del_spravy_subs()
{
	if	( ! QMessageBox::question(
                this,
                codec->toUnicode("!!! Видалення стрічки !!!"),
                codec->toUnicode("Ви дійсно бажаєте видалити сирічку?"),
                codec->toUnicode("Так"),
				codec->toUnicode("Ні-ні-ні"),
                QString(), 0, 1)
		)
		model_spravy_subs->removeRow(ui.tableView_spravy_subs->currentIndex().row());
}
//------------------------------------------------------------
void tke_MainWindow::buttonclick_add_splata_zaborg()
{
	disconnect(ui.tableView_splata_zaborg->selectionModel(), SIGNAL(currentRowChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(look_abonent_info( const QModelIndex &, const QModelIndex & ))); //позначено запис --> висвітлення інформації про абонента
	int row = model_splata_zaborg->rowCount();
	model_splata_zaborg->insertRow(row);

	QModelIndex index = model_splata_zaborg->index(row, 1);
	ui.tableView_splata_zaborg->setCurrentIndex(index);
	ui.tableView_splata_zaborg->edit(index);
	connect(ui.tableView_splata_zaborg->selectionModel(), SIGNAL(currentRowChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(look_abonent_info( const QModelIndex &, const QModelIndex & ))); //позначено запис --> висвітлення інформації про абонента
}
//------------------------------------------------------------
void tke_MainWindow::buttonclick_del_splata_zaborg()
{
	if	( ! QMessageBox::question(
                this,
                codec->toUnicode("!!! Видалення стрічки !!!"),
                codec->toUnicode("Ви дійсно бажаєте видалити сирічку?"),
                codec->toUnicode("Так"),
				codec->toUnicode("Ні-ні-ні"),
                QString(), 0, 1)
		)
		model_splata_zaborg->removeRow(ui.tableView_splata_zaborg->currentIndex().row());
}
//------------------------------------------------------------
void tke_MainWindow::buttonclick_add_kotelni()
{
	int row = model_kotelni->rowCount();
	model_kotelni->insertRow(row);

	QModelIndex index = model_kotelni->index(row, 1);
	ui.tableView_kotelni->setCurrentIndex(index);
	ui.tableView_kotelni->edit(index);
}
//------------------------------------------------------------
void tke_MainWindow::buttonclick_del_kotelni()
{
	if	( ! QMessageBox::question(
                this,
                codec->toUnicode("!!! Видалення стрічки !!!"),
                codec->toUnicode("Ви дійсно бажаєте видалити сирічку?"),
                codec->toUnicode("Так"),
				codec->toUnicode("Ні-ні-ні"),
                QString(), 0, 1)
		)
		model_kotelni->removeRow(ui.tableView_kotelni->currentIndex().row());
}
//------------------------------------------------------------
void tke_MainWindow::buttonclick_add_pilgovyky()
{
	disconnect(ui.tableView_pilgovyky->selectionModel(), SIGNAL(currentRowChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(look_abonent_info( const QModelIndex &, const QModelIndex & ))); //позначено запис --> висвітлення інформації про абонента
	int row = model_pilgovyky->rowCount();
	model_pilgovyky->insertRow(row);
	QModelIndex index = model_pilgovyky->index(row, 1);
	//ui.tableView_pilgovyky->selectRow (row);
	ui.tableView_pilgovyky->setCurrentIndex(index);
	ui.tableView_pilgovyky->edit(index);
	connect(ui.tableView_pilgovyky->selectionModel(), SIGNAL(currentRowChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(look_abonent_info( const QModelIndex &, const QModelIndex & ))); //позначено запис --> висвітлення інформації про абонента
}
//------------------------------------------------------------
void tke_MainWindow::buttonclick_del_pilgovyky()
{
	if	( ! QMessageBox::question(
                this,
                codec->toUnicode("!!! Видалення стрічки !!!"),
                codec->toUnicode("Ви дійсно бажаєте видалити сирічку?"),
                codec->toUnicode("Так"),
				codec->toUnicode("Ні-ні-ні"),
                QString(), 0, 1)
		)
		model_pilgovyky->removeRow(ui.tableView_pilgovyky->currentIndex().row());
}
//------------------------------------------------------------
void tke_MainWindow::pushButton_addOpString_clicked()
{
	if (!opTwExt->addRow("INSERT INTO plataZgidnoSubs (Rahunok_id, Year, Month) \
						VALUES (1,"+sqlStr(ui.dateEdit_opSubsFilter->date().year())+","+sqlStr(ui.dateEdit_opSubsFilter->date().month())+")"))
		QMessageBox::critical(0,"Помилка створення запису","Запис обов'язкової плати не створено");
}
//------------------------------------------------------------
void tke_MainWindow::pushButton_delOpString_clicked()
{
	if	( QMessageBox::question(
                this,
                "!!! Видалення стрічки !!!",
                "Ви дійсно бажаєте видалити сирічку?",
                "Так",
				"Ні-ні-ні",
                QString(), 0, 1)
		)
		return;
	if (!opTwExt->deleteSelectedRows())
		QMessageBox::critical(0,"Помикла видалення запису","Запис не видалено");
}
//------------------------------------------------------------
void tke_MainWindow::setOpDateFilter()
{
	opTwExt->setSqlFilter("Year="+sqlStr(ui.dateEdit_opSubsFilter->date().year())+" and Month="+sqlStr(ui.dateEdit_opSubsFilter->date().month()));
	opTwExt->populateTable();
}
//------------------------------------------------------------
void tke_MainWindow::pushButton_narahPereved_addString_clicked()
{
	if (!perevedTwExt->addRow("INSERT INTO narahuvannyaPereved (Rahunok_id, CDate) \
						VALUES (1,"+sqlStr(ui.dateEdit_narahPereved_Filter->date().toString("yyyy-MM-dd"))+")"))
		QMessageBox::critical(0,"Помилка створення запису","Запис переведення заборгованості не створено");
}
//------------------------------------------------------------
void tke_MainWindow::pushButton_narahPereved_delString_clicked()
{
	if	( QMessageBox::question(
                this,
                "!!! Видалення стрічки !!!",
                "Ви дійсно бажаєте видалити сирічку?",
                "Так",
				"Ні-ні-ні",
                QString(), 0, 1)
		)
		return;
	if (!perevedTwExt->deleteSelectedRows())
		QMessageBox::critical(0,"Помикла видалення запису","Запис не видалено");
}
//------------------------------------------------------------
void tke_MainWindow::setNarahPerevedDateFilter()
{
	perevedTwExt->setSqlFilter("year(CDate)="+sqlStr(ui.dateEdit_narahPereved_Filter->date().year())+" \
								and month(CDate)="+sqlStr(ui.dateEdit_narahPereved_Filter->date().month()));
	perevedTwExt->populateTable();
}
//------------------------------------------------------------
void tke_MainWindow::pilgovyky_prime_insert( int row, QSqlRecord & record)
{
	record.setValue("Pilga_ID", d_Kategoriya_pilgy_ID);
}
//------------------------------------------------------------
void tke_MainWindow::budynky_prime_insert( int row, QSqlRecord & record)
{
	record.setValue("Vulycia", QVariant(d_Vulycia));
	record.setValue("Koef_pereved", QVariant(d_Koef));
	record.setValue("Kotelnya_ID", QVariant(d_Kotelnya_ID));
}
//------------------------------------------------------------
void tke_MainWindow::kvytancii_prime_insert( int row, QSqlRecord & record)
{
	record.setValue("Plata_date", QVariant(d_Splata_date));
	record.setValue("Perevid_date", QVariant(d_Perevid_Date));
	record.setValue("Pachka", QVariant(d_Pachka));
}
//------------------------------------------------------------
void tke_MainWindow::splata_zaborg_prime_insert( int row, QSqlRecord & record)
{
	record.setValue("Ugoda", QVariant(d_Ugoda));
	record.setValue("Poch_month", QVariant(d_Month_poch));
	record.setValue("Poch_year", QVariant(d_Year_poch));
	record.setValue("Kin_month", QVariant(d_Month_end));
	record.setValue("Kin_year", QVariant(d_Year_end));
}
//------------------------------------------------------------
void tke_MainWindow::kotelni_prime_insert( int row, QSqlRecord & record)
{
	record.setValue("Nas_punkt", QVariant(d_City));
	record.setValue("Vulycia", QVariant(d_Vulycia));
}
//------------------------------------------------------------
void tke_MainWindow::spravy_subs_prime_insert( int row, QSqlRecord & record)
{
	record.setValue("Nom_spravy", QVariant(d_Sprava));
	record.setValue("Poch_month", QVariant(d_Month_poch));
	record.setValue("Poch_year", QVariant(d_Year_poch));
	record.setValue("Kin_month", QVariant(d_Month_end));
	record.setValue("Kin_year", QVariant(d_Year_end));
}
//------------------------------------------------------------
void tke_MainWindow::before_change_default(QSqlRecord & record)
{
	// провіряє усі внесені і змінені записи усіх таблиць на наявність у них 
	// відповідних полів для зміни значення по замовчуванню
	if (!record.isNull("Vulycia"))
		d_Vulycia = record.value("Vulycia").toString();
	if (!record.isNull("Koef_pereved"))
		d_Koef = record.value("Koef_pereved").toDouble();
	if (!record.isNull("Kotelnya_ID"))
		d_Kotelnya_ID = record.value("Kotelnya_ID").toInt();
	if (!record.isNull("Nas_punkt"))
		d_City = record.value("Nas_punkt").toString();
	if (!record.isNull("year"))
		d_Year = record.value("year").toInt();
	if (!record.isNull("month"))
		d_Month = record.value("month").toInt();
	if (!record.isNull("Nom_spravy"))
		d_Sprava = record.value("Nom_spravy").toInt();
	if (!record.isNull("Poch_month"))
		d_Month_poch = record.value("Poch_month").toInt();
	if (!record.isNull("Poch_year"))
		d_Year_poch = record.value("Poch_year").toInt();
	if (!record.isNull("Kin_month"))
		d_Month_end = record.value("Kin_month").toInt();
	if (!record.isNull("Kin_year"))
		d_Year_end = record.value("Kin_year").toInt();
	if (!record.isNull("Ugoda"))
		d_Ugoda = record.value("Ugoda").toInt();
	if (!record.isNull("Pilga_Id"))
		d_Kategoriya_pilgy_ID = record.value("Pilga_Id").toInt();
	if (!record.isNull("Plata_date"))
		d_Splata_date = record.value("Plata_date").toDate();
	if (!record.isNull("Perevid_date"))
		d_Perevid_Date = record.value("Perevid_date").toDate();
	if (!record.isNull("Pachka"))
		d_Pachka = record.value("Pachka").toInt();
}
//------------------------------------------------------------
void tke_MainWindow::before_change_default(int row, QSqlRecord & record)
{
	before_change_default(record);
}
//------------------------------------------------------------
void tke_MainWindow::setSettingsFromIni()
{
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	dateEdit_PotochnaData->setDate(settings->value("GUI_View/SystemCurDate").toDate());
	curDateChanged();
	delete settings;
}
//------------------------------------------------------------
void tke_MainWindow::saveSettingsToIni()
{
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	//Збереження вигляду GUI в іні-файл
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	settings->beginGroup("GUI_View");
	//settings->setValue("PilgsDatesLimitsVisible",ui.groupBox_PilgsDatesLimits->isVisible());
	settings->setValue("SystemCurDate",currentSysDate);
	delete settings;
}
//------------------------------------------------------------
void tke_MainWindow::populateControlObjectsHash()
{
	controlObjectsHash->clear();
	controlObjectsHash->insert( 10, dateEdit_PotochnaData);
	controlObjectsHash->insert( 11, ui.tab_debitorskaZaborg);
	controlObjectsHash->insert( 20, ui.toolButton_extPoshuk);
	controlObjectsHash->insert( 30, ui.spinBox_Rahunok);
	controlObjectsHash->insert( 40, ui.spinBox_BankRahunok);
	controlObjectsHash->insert( 50, ui.lineEdit_Prizv);
	controlObjectsHash->insert( 60, ui.comboBox_Imya);
	controlObjectsHash->insert( 70, ui.comboBox_Batk);
	controlObjectsHash->insert( 80, ui.spinBox_Dilnycia);
	controlObjectsHash->insert( 90, ui.comboBox_Budynok);
	controlObjectsHash->insert(100, ui.spinBox_Kvartyra);
	controlObjectsHash->insert(101, ui.lineEdit_kvartSufiks);
	controlObjectsHash->insert(102, ui.lineEdit_prymitka);
	controlObjectsHash->insert(110, ui.doubleSpinBox_Ploshcha);
	controlObjectsHash->insert(120, ui.spinBox_Kimn);
	controlObjectsHash->insert(130, ui.spinBox_Ludy);
	controlObjectsHash->insert(140, ui.lineEdit_Telefon);
	controlObjectsHash->insert(150, ui.lineEdit_Identyf);
	controlObjectsHash->insert(160, ui.comboBox_Kategoria_Pilgy);
	controlObjectsHash->insert(170, ui.comboBox_Kategoria_Pilgy_2);
	controlObjectsHash->insert(180, ui.spinBox_Pilga_for);
	controlObjectsHash->insert(190, ui.spinBox_Pilga_for_2);
	controlObjectsHash->insert(200, ui.doubleSpinBox_Zaiva_Ploshcha);
	controlObjectsHash->insert(210, ui.checkBox_Opal);
	controlObjectsHash->insert(220, ui.checkBox_GarVoda);
	controlObjectsHash->insert(221, ui.checkBox_narahStoyak);
	controlObjectsHash->insert(230, ui.checkBox_Zdano_Pilgy);
	controlObjectsHash->insert(240, ui.groupBox_PilgsDatesLimits);
	controlObjectsHash->insert(250, ui.spinBox_year_filtr);
	controlObjectsHash->insert(260, ui.pushButton_abonent_form_save);
	controlObjectsHash->insert(270, ui.pushButton_abonent_form_abort);
	controlObjectsHash->insert(280, ui.pushButton_new);
	controlObjectsHash->insert(290, ui.action_extSearch);
	controlObjectsHash->insert(300, ui.action_debuger_form);
	controlObjectsHash->insert(310, ui.action_sqlBrowser);
	controlObjectsHash->insert(320, ui.action_set_data_sources);
	controlObjectsHash->insert(330, ui.actionPosada);
	controlObjectsHash->insert(340, ui.action_edit_users);
	controlObjectsHash->insert(350, ui.action_Options);
	controlObjectsHash->insert(360, ui.action_structPostachGV);
	controlObjectsHash->insert(370, ui.action_budynky);
	controlObjectsHash->insert(380, ui.action_kotelni);
	controlObjectsHash->insert(390, ui.action_pilgy);
	controlObjectsHash->insert(400, ui.action_normat_taryf);
	controlObjectsHash->insert(410, ui.action_spravy_subs);
	controlObjectsHash->insert(420, ui.action_zaborg);
	controlObjectsHash->insert(430, ui.action_pilgovyky);
	controlObjectsHash->insert(440, ui.action_diuchyi_taryf);
	controlObjectsHash->insert(450, ui.action_pokaz_lich_GV);
	controlObjectsHash->insert(460, ui.action_korektura);
	controlObjectsHash->insert(470, ui.action_korektura_s);
	controlObjectsHash->insert(480, ui.action_subs);
	controlObjectsHash->insert(490, ui.action_kvytancii);
	controlObjectsHash->insert(491, ui.action_opSubs);
	controlObjectsHash->insert(492, ui.action_narahPereved);
	controlObjectsHash->insert(500, ui.action_narah);
	controlObjectsHash->insert(501, ui.action_narah_ST);
	controlObjectsHash->insert(510, ui.action_narah_GV);
	controlObjectsHash->insert(520, ui.action_pererahunok_saldo);
	controlObjectsHash->insert(521, ui.action_calcDebutorskaZaborgVhSaldo);
	controlObjectsHash->insert(530, ui.action_pererahunok_saldo_all);
	controlObjectsHash->insert(531, ui.action_znyattya_narahuv_vid_vidkluchenyh);
	controlObjectsHash->insert(540, ui.action_CheckDataBaseIsCorrect);
	controlObjectsHash->insert(550, ui.action_save_vidomist_pilg_to_file);
	controlObjectsHash->insert(551, ui.action_export_f2_v2007);
	controlObjectsHash->insert(552, ui.action_export_vidomist_narahuvannya_po_lich_dbf);
	controlObjectsHash->insert(560, ui.action_insert_dbf_kvyt_oshchadbank);
	controlObjectsHash->insert(561, ui.action_postDataBaseForAuthurAnalize);
	controlObjectsHash->insert(562, ui.action_fastPilgovykyDocInsert);
	controlObjectsHash->insert(563, ui.action_insertOpSubsFromFile);
	controlObjectsHash->insert(564, ui.action_fillSubsDbfSpojyvannyaCO);
	controlObjectsHash->insert(570, ui.action_insert_subs_dbf);
	controlObjectsHash->insert(580, ui.action_open_insert_narah_subs_form);
	controlObjectsHash->insert(590, ui.action_open_insert_donarah_subs_form);
	controlObjectsHash->insert(600, ui.action_clear_vidomosti_pro_zdani_dokumenty_napilgy);
	controlObjectsHash->insert(610, ui.action_save_data_base_toArchive);
	controlObjectsHash->insert(620, ui.action_print_kvytancii);
	controlObjectsHash->insert(621, ui.action_printKvytanciiBeznaiynogoBorgu);
	controlObjectsHash->insert(622, ui.action_printKvytanciiBeznaiynogoBorguPotochnAbonent);
	controlObjectsHash->insert(623, ui.action_printCustomKvyt);
	controlObjectsHash->insert(630, ui.action_print_kvytancii_for_abonent);
	controlObjectsHash->insert(640, ui.action_vidomist_pilg);
	controlObjectsHash->insert(650, ui.action_vidomist_pilgovykiv_kategor);
	controlObjectsHash->insert(660, ui.action_print_vidomist_narah);
	controlObjectsHash->insert(661, ui.action_printVidomistOtrymannyaPoslugPoBudynkah);
	controlObjectsHash->insert(662, ui.action_printVidomistOtrymannyaPoslugPoKotelnyah);
	controlObjectsHash->insert(663, ui.action_printNadannyaPoslugPoBudTaKot);
	controlObjectsHash->insert(664, ui.action_printVidomistSpojyvannyaCOZaRik);
	controlObjectsHash->insert(665, ui.action_printVidomistSpojyvannyaGVZaRik);
	controlObjectsHash->insert(670, ui.action_print_dovidka_oderj_subs);
	controlObjectsHash->insert(671, ui.action_print_dovidka_u_subs_2);
	controlObjectsHash->insert(680, ui.action_print_rahunok_abonenta);
	controlObjectsHash->insert(690, ui.action_print_dovidka_u_subs);
	controlObjectsHash->insert(700, ui.action_print_info_pro_spojyvannya);
	controlObjectsHash->insert(710, ui.action_print_borjnyky);
	controlObjectsHash->insert(711, ui.action_printVidomistBorjnykivOtrymSubs);
	controlObjectsHash->insert(712, ui.action_print_zaborg_za_periody);
	controlObjectsHash->insert(720, ui.action_print_vidomist_borjnykiv_riznyc);
	controlObjectsHash->insert(730, ui.action_print_vidomist_zdanyh_dokumentiv_na_pilgy);
	controlObjectsHash->insert(740, ui.action_print_povidomlennya_dlya_borjnykiv);
	controlObjectsHash->insert(750, ui.action_print_povidomlennya_dlya_pilgovykiv);
	controlObjectsHash->insert(760, ui.action_print_wrong_subs);
	controlObjectsHash->insert(770, ui.action_vidomist_narah_oplaty_za_CO_dla_viddilu_subs);
	controlObjectsHash->insert(780, ui.action_print_vidomost_narahuvannya_dlya_abon_z_lich);
	controlObjectsHash->insert(790, ui.action_print_forma_akt_vytrat_teploenergii);
	controlObjectsHash->insert(800, ui.action_zvit_pilg_kategor);
	controlObjectsHash->insert(810, ui.action_print_dovidnyk_abonentiv);
	controlObjectsHash->insert(811, ui.action_dovidnykAbonentivVRozriziNadannyaPoslug);
	controlObjectsHash->insert(820, ui.action_archiv_vidomosti_kvytanciy_ob);
	controlObjectsHash->insert(830, calculatorButton);
	controlObjectsHash->insert(840, ui.action_insert_electronic_kvytancii_UNI);
	controlObjectsHash->insert(850, ui.action_insert_electronic_subs_UNI);
	controlObjectsHash->insert(860, ui.action_print_zvit_forma_n_2_pilga_2007);
	controlObjectsHash->insert(861, ui.action_print_vidnoshennya_pro_vidshkoduvannya_pilg_CO);
	controlObjectsHash->insert(862, ui.action_print_vidomist_pererahunku_pilg_CO);
	controlObjectsHash->insert(863, ui.action_print_vidnoshennya_pro_vidshkoduvannya_pilg_GV);
	controlObjectsHash->insert(864, ui.action_print_vidomist_pererahunku_pilg_GV);
	controlObjectsHash->insert(865, ui.action_print_vidomist_samovilnoVidkluchAbon);
	controlObjectsHash->insert(866, ui.action_printDebetoveKredytoveSaldo);
	
	controlObjectsHash->insert(870, ui.action_zagalna_vidomist_narahuvannya_i_postuplennya);
	controlObjectsHash->insert(880, ui.tableWidget_rozrahunky);
	controlObjectsHash->insert(890, ui.action_open_insert_kvyt_form);
	controlObjectsHash->insert(900, ui.action_pokazn_opal_stoyak);
}
//------------------------------------------------------------
void tke_MainWindow::saveMode()
{
		//видалення змін, внесених в віджети форми абонентів
	pushButton_abonent_form_abort_clicked();
	
		//вимкнення усіх кнопок вікон
	ui.action_budynky->setChecked(false);
	ui.action_pilgy->setChecked(false);
	ui.action_normat_taryf->setChecked(false);
	ui.action_subs->setChecked(false);
	ui.action_korektura->setChecked(false);
	ui.action_korektura_s->setChecked(false);
	ui.action_zaborg->setChecked(false);
	ui.action_kvytancii->setChecked(false);
	ui.action_diuchyi_taryf->setChecked(false);
	ui.action_narah->setChecked(false);
	ui.action_spravy_subs->setChecked(false);
	ui.action_kotelni->setChecked(false);
	ui.action_pilgovyky->setChecked(false);
	ui.action_opSubs->setChecked(false);
	
	//Переведення в неактивний стан усіх віджетів програми
	QHashIterator<int, QObject *> iter(*controlObjectsHash);
	while (iter.hasNext())
		castControls(iter.next().value(), false);
	
	IRC = false;
	
	emit closeLoginSignal();
}
//------------------------------------------------------------
void tke_MainWindow::workMode(int mode)
{
	saveMode();
	
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT id FROM users_priv WHERE id<10000 and ["+QString::number(mode)+"]=true");
	while (query->next())
		castControls(controlObjectsHash->value(query->value(0).toInt()), true);
	
	query->exec("SELECT id FROM users_priv WHERE id=311 and ["+QString::number(mode)+"]=true");
	if (query->seek(0))
		IRC = true;
	
	delete query;
}
//------------------------------------------------------------
void tke_MainWindow::loadGUISettingsFromIni()
{
	bool boolVal;
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	ui.groupBox_PilgsDatesLimits->setVisible(settings->value("GUI_View/PilgsDatesLimitsVisible").toBool());
	boolVal = settings->value("GUI_View/ShowNarahCO").toBool();
	ui.tableWidget_rozrahunky->setColumnHidden(1, !boolVal );
	ui.tableWidget_rozrahunky->setColumnHidden(2, !boolVal );
	ui.tableWidget_rozrahunky->setColumnHidden(6, !boolVal );
	ui.tableWidget_rozrahunky->setColumnHidden(7, !boolVal );
	ui.tableWidget_rozrahunky->setColumnHidden(11, !boolVal );
	ui.tableWidget_rozrahunky->setColumnHidden(12, !boolVal );
	ui.label_Opal->setVisible(boolVal);
	ui.checkBox_Opal->setVisible(boolVal);
	ui.action_narah->setVisible(boolVal);
	ui.action_print_vidnoshennya_pro_vidshkoduvannya_pilg_CO->setVisible(boolVal);
	ui.action_print_vidomist_pererahunku_pilg_CO->setVisible(boolVal);
	ui.toolButton_lichCOInfo->setVisible(boolVal);
	ui.line_13->setVisible(boolVal);
	
	boolVal = settings->value("GUI_View/ShowNarahGV").toBool();
	ui.tableWidget_rozrahunky->setColumnHidden(3, !boolVal );
	ui.tableWidget_rozrahunky->setColumnHidden(4, !boolVal );
	ui.tableWidget_rozrahunky->setColumnHidden(8, !boolVal );
	ui.tableWidget_rozrahunky->setColumnHidden(9, !boolVal );
	ui.tableWidget_rozrahunky->setColumnHidden(13, !boolVal );
	ui.label_GarVoda->setVisible(boolVal);
	ui.checkBox_GarVoda->setVisible(boolVal);
	ui.action_narah_GV->setVisible(boolVal);
	ui.action_print_vidnoshennya_pro_vidshkoduvannya_pilg_GV->setVisible(boolVal);
	ui.action_print_vidomist_pererahunku_pilg_GV->setVisible(boolVal);
	ui.action_pokazn_opal_stoyak->setVisible(boolVal);
	ui.action_structPostachGV->setVisible(boolVal);
	ui.line_11->setVisible(boolVal);
	ui.toolButton_lichGVInfo->setVisible(boolVal);
	
	boolVal = settings->value("GUI_View/ShowNarahST").toBool();
	ui.tableWidget_rozrahunky->setColumnHidden(5, !boolVal );
	ui.tableWidget_rozrahunky->setColumnHidden(14, !boolVal );
	ui.label_narahStoyak->setVisible(boolVal);
	ui.checkBox_narahStoyak->setVisible(boolVal);
	ui.action_narah_ST->setVisible(boolVal);
	
	boolVal = settings->value("GUI_View/ShowPerevedZaborg").toBool();
	ui.tableWidget_rozrahunky->setColumnHidden(15, !boolVal );
	
	ui.tableWidget_diuch_taryf->setItemDelegateForColumn(5, new UDoubleSpinBoxDelegate(0,100000,tochnistDiuchogoTaryfu(),ui.tableWidget_diuch_taryf));
	
	delete settings;
}
//------------------------------------------------------------
void tke_MainWindow::look_abonent_info( const QModelIndex &  index, const QModelIndex &  index_ignored)
{
	
	const QAbstractItemModel *sqlTableModel = index.model();
	if (sqlTableModel){
		int abonPos, curRah = sqlTableModel->data(sqlTableModel->index(index.row(), 1), Qt::DisplayRole).toInt();
		abonPos = abonentsList.indexOf(curRah)+1;
		if (abonPos!=0)
			ui.spinBox_navigator->setValue(abonPos);
		else{
			QMessageBox::critical(
				this,
				codec->toUnicode("!!! Невірний рахунок абонента !!!"),
				codec->toUnicode("В базі даних немає абонента з рахунком ")+QVariant(curRah).toString()+".",
				QMessageBox::Ok,
				QMessageBox::NoButton,
				QMessageBox::NoButton);
		}
	}
}
//------------------------------------------------------------
void tke_MainWindow::activ_column_changes( const QModelIndex & current, const QModelIndex & previous )
{
	if (previous.column() == 1){
		look_abonent_info(current);
	}
}
//------------------------------------------------------------
void tke_MainWindow::show_rahunok(int rah){
	
	int ind = abonentsList.indexOf(rah)+1;
	if (ind!=0)
		ui.spinBox_navigator->setValue(ind);
	else{
		QMessageBox::critical(
            this,
            codec->toUnicode("!!! Невірний рахунок абонента !!!"),
            codec->toUnicode("В базі даних немає абонента з рахунком ")+QVariant(rah).toString()+".",
            QMessageBox::Ok,
            QMessageBox::NoButton,
			QMessageBox::NoButton);
	}
}
//------------------------------------------------------------
void tke_MainWindow::showDebugWindow()
{
	emit requestDebugWindow();
}
//------------------------------------------------------------
tke_MainWindow::~tke_MainWindow()
{
	saveSettingsToIni();
	qInstallMsgHandler(0);
	
	tke_Dialog_saveDataBase *saveDialog = new tke_Dialog_saveDataBase();
	connect(this, SIGNAL(requestSaveDataBaseWindow()), saveDialog, SLOT(show()));
	
	emit mainWindowClosed();
	emit requestSaveDataBaseWindow();
	
#ifndef TKE_DEMO
	delete abonent_select_query;
	delete narah_select_query;
	delete narah_temp_query;
	delete abonent_update_model;
	delete abonent_insert_model;
	delete diuch_taryf_update_model;
	delete abonent_temp_query;
	delete palette;
	delete diuch_taryf_query;
	
	delete model_budynky;
	delete model_pilgy;
	delete model_normat_taryf;
	delete model_korektura;
	delete model_subs;
	delete model_kvytancii;
	delete model_spravy_subs;
	delete model_splata_zaborg;
	delete model_kotelni;
	delete model_pilgovyky;
	delete narah_update_model;
	delete korektura_update_model;
	delete model_korektura_s;
	
	delete controlObjectsHash;
#endif
}
//------------------------------------------------------------
