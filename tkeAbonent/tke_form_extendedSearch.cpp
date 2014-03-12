//------------------------------------------------------------
//
// Implamantation of tke_form_extendedSearch class.
// Created by Roman Mulyk 14.06.2008
//
//------------------------------------------------------------

#include <QMessageBox>
#include <QTextCodec>
#include <QtSql>
#include <QVariant>
#include <QSettings>

#include "tke_form_extendedSearch.h"
//------------------------------------------------------------
tke_form_extendedSearch::tke_form_extendedSearch( QWidget * parent) 
	: QWidget(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowFlags(Qt::Tool ); //| Qt::WindowStaysOnTopHint
	
	//setMaximumWidth(width());
	//setMinimumWidth(width());
	//setMaximumHeight(height());
	//setMinimumHeight(height());
	
	ui.checkBox_CO->setCheckState(Qt::PartiallyChecked);
	ui.checkBox_GV->setCheckState(Qt::PartiallyChecked);
	
	ui.tableWidget_abonents->setAlternatingRowColors(true);
	
	connect(ui.toolButton_poshuk, SIGNAL(clicked ()), this, SLOT(searchAbonents()));
	connect(ui.pushButton_openRahunok, SIGNAL(clicked()), this, SLOT(pushButton_openRahunok_clicked()));
	connect(ui.tableWidget_abonents, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(pushButton_openRahunok_clicked()));
	
	connect(ui.pushButton_closeForm, SIGNAL(clicked()), this, SLOT(close()));
}
//------------------------------------------------------------
void tke_form_extendedSearch::searchAbonents()
{
	ui.tableWidget_abonents->setSortingEnabled(false);
	ui.tableWidget_abonents->setRowCount(0);
	QSqlQuery *query = new QSqlQuery();
	query->setForwardOnly(true);
	query->exec("SELECT abonent.Rahunok AS Rahunok, \
							abonent.Prizv & ' ' & abonent.Imya & ' ' & abonent.Batk AS Pib, \
							budynky.Vulycia & ' ' & budynky.Bud_num & ' кв.' & abonent.Kvartyra AS Adresa,  \
							abonent.Opal, abonent.GarVoda, \
							pilgy1.Kategoriya & ' ' & pilgy2.Kategoriya AS Pilgy,  \
							abonent.BankRahunok, budynky.Bud_num, abonent.Kvartyra\
				FROM ((abonent \
				LEFT JOIN budynky ON budynky.id=abonent.budynok_id) \
				LEFT JOIN (SELECT * FROM pilgy) AS pilgy1 ON  pilgy1.id=abonent.Pilga_id) \
				LEFT JOIN (SELECT * FROM pilgy) AS pilgy2 ON  pilgy2.id=abonent.Pilga_id_2 \
				ORDER BY budynky.Vulycia, budynky.Bud_num, abonent.Kvartyra");
	QString str;
	int row;
	while (query->next()){
			//Провірка на валідність стрічки з бази даних
		str = ui.lineEdit_PIB->text();
		if ((str.size()!=0) && !query->value(1).toString().contains(str,Qt::CaseInsensitive))
			goto nextQueryRow;
		str = ui.lineEdit_adresa->text();
		if ((str.size()!=0) && !query->value(2).toString().contains(str,Qt::CaseInsensitive))
			goto nextQueryRow;
		if ((ui.checkBox_CO->checkState()!=Qt::PartiallyChecked) && ((ui.checkBox_CO->checkState()==Qt::Checked) ^ query->value(3).toBool()))
			goto nextQueryRow;
		if ((ui.checkBox_GV->checkState()!=Qt::PartiallyChecked) && ((ui.checkBox_GV->checkState()==Qt::Checked) ^ query->value(4).toBool()))
			goto nextQueryRow;
		str = ui.lineEdit_pilga->text();
		if ((str.size()!=0) && !query->value(5).toString().contains(str,Qt::CaseInsensitive))
			goto nextQueryRow;
		str = ui.spinBox_bankRahunok->text();
		if ((str.size()!=0) && !query->value(6).toString().contains(str,Qt::CaseInsensitive))
			goto nextQueryRow;
		str = ui.lineEdit_budNum->text();
		if ((str.size()!=0) && !query->value(7).toString().contains(str,Qt::CaseInsensitive))
			goto nextQueryRow;
		str = ui.lineEdit_kvartNum->text();
		if ((str.size()!=0) && !query->value(8).toString().contains(str,Qt::CaseInsensitive))
			goto nextQueryRow;
		
		//Запис стрічки до таблиці
		row = ui.tableWidget_abonents->rowCount();
		ui.tableWidget_abonents->insertRow(row);
		ui.tableWidget_abonents->setItem(row, 0, new QTableWidgetItem(query->value(0).toString()));
		ui.tableWidget_abonents->setItem(row, 1, new QTableWidgetItem(query->value(1).toString()));
		ui.tableWidget_abonents->setItem(row, 2, new QTableWidgetItem(query->value(2).toString()));
		if (query->value(3).toBool())
			ui.tableWidget_abonents->setItem(row, 3, new QTableWidgetItem("Так"));
		else
			ui.tableWidget_abonents->setItem(row, 3, new QTableWidgetItem("Ні"));
		if (query->value(4).toBool())
			ui.tableWidget_abonents->setItem(row, 4, new QTableWidgetItem("Так"));
		else
			ui.tableWidget_abonents->setItem(row, 4, new QTableWidgetItem("Ні"));
		ui.tableWidget_abonents->setItem(row, 5, new QTableWidgetItem(query->value(5).toString()));
		ui.tableWidget_abonents->setItem(row, 6, new QTableWidgetItem(query->value(6).toString()));
		
nextQueryRow: 
		row=0;
	}

	ui.tableWidget_abonents->resizeColumnsToContents();
	ui.tableWidget_abonents->setSortingEnabled(true);
	if (ui.tableWidget_abonents->rowCount()>0){
		ui.tableWidget_abonents->setCurrentCell(0,1);
		ui.pushButton_openRahunok->setEnabled(true);
	}
	else
		ui.pushButton_openRahunok->setEnabled(false);
	delete query;
}
//------------------------------------------------------------
void tke_form_extendedSearch::pushButton_openRahunok_clicked()
{
	if (ui.tableWidget_abonents->rowCount()>0)
		requestRahunok(ui.tableWidget_abonents->item(ui.tableWidget_abonents->currentRow(),0)->text().toInt());
}
//------------------------------------------------------------
tke_form_extendedSearch::~tke_form_extendedSearch()
{
	
}
//------------------------------------------------------------
