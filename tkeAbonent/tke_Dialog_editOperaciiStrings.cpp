//------------------------------------------------------------
//
// Implamantation of tke_Dialog_editOperaciiStrings class.
// Created by Roman Mulyk 26.06.2008
//
//------------------------------------------------------------

#include "tke_Dialog_editOperaciiStrings.h"

#include <USqlAccessible>
#include <QSqlError>

#define MIN_COLUMN_WIDTH 60
//------------------------------------------------------------
tke_Dialog_editOperaciiStrings::tke_Dialog_editOperaciiStrings(int abonentRah, short year, short month, QWidget *parent)
        : QDialog(parent)
{
    ui.setupUi(this);
	
	rahunok = abonentRah;
	ui.dateEdit_potochnaData->setDate(QDate(year, month, 1));
	
	colCount = ui.tableWidget_narahStrings->columnCount();
	ui.tableWidget_narahStrings->setColumnHidden(0,true);
	
	ui.tableWidget_narahStrings->setItemDelegateForColumn(1, new UDoubleSpinBoxDelegate(-99999999, 99999999, 2, ui.tableWidget_narahStrings));
	ui.tableWidget_narahStrings->setItemDelegateForColumn(2, new UDoubleSpinBoxDelegate(-99999999, 99999999, 2, ui.tableWidget_narahStrings));
	ui.tableWidget_narahStrings->setItemDelegateForColumn(3, new UDoubleSpinBoxDelegate(-99999999, 99999999, 2, ui.tableWidget_narahStrings));
	ui.tableWidget_narahStrings->setItemDelegateForColumn(4, new USpinBoxDelegate(0, 400, ui.tableWidget_narahStrings));
	ui.tableWidget_narahStrings->setItemDelegateForColumn(5, new USpinBoxDelegate(0, 30, ui.tableWidget_narahStrings));
	ui.tableWidget_narahStrings->setItemDelegateForColumn(6, new UDoubleSpinBoxDelegate(0, 1000, 6, ui.tableWidget_narahStrings));
	ui.tableWidget_narahStrings->setItemDelegateForColumn(7, new UDoubleSpinBoxDelegate(-99999999, 99999999, 2, ui.tableWidget_narahStrings));
	ui.tableWidget_narahStrings->setItemDelegateForColumn(8, new UDoubleSpinBoxDelegate(-99999999, 99999999, 2, ui.tableWidget_narahStrings));
	ui.tableWidget_narahStrings->setItemDelegateForColumn(9, new USpinBoxDelegate(0, 400, ui.tableWidget_narahStrings));
	ui.tableWidget_narahStrings->setItemDelegateForColumn(10, new USpinBoxDelegate(0, 30, ui.tableWidget_narahStrings));
	ui.tableWidget_narahStrings->setItemDelegateForColumn(11, new UDoubleSpinBoxDelegate(0, 1000, 6, ui.tableWidget_narahStrings));
	ui.tableWidget_narahStrings->setItemDelegateForColumn(12, new UDoubleSpinBoxDelegate(-99999999, 99999999, 2, ui.tableWidget_narahStrings));
	ui.tableWidget_narahStrings->setItemDelegateForColumn(13, new USpinBoxDelegate(0, 2100, ui.tableWidget_narahStrings));
	ui.tableWidget_narahStrings->setItemDelegateForColumn(14, new USpinBoxDelegate(0, 12, ui.tableWidget_narahStrings));
	ui.tableWidget_narahStrings->setItemDelegateForColumn(15, new USpinBoxDelegate(0, 31, ui.tableWidget_narahStrings));
	ui.tableWidget_narahStrings->setItemDelegateForColumn(16, new USpinBoxDelegate(0, 31, ui.tableWidget_narahStrings));
	ui.tableWidget_narahStrings->setItemDelegateForColumn(17, new USpinBoxDelegate(0, 31, ui.tableWidget_narahStrings));
	ui.tableWidget_narahStrings->setItemDelegateForColumn(18, new USpinBoxDelegate(0, 31, ui.tableWidget_narahStrings));
	ui.tableWidget_narahStrings->setItemDelegateForColumn(19, new USpinBoxDelegate(0, 100, ui.tableWidget_narahStrings));
	
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	bool boolVal;
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	boolVal = settings->value("GUI_View/ShowNarahCO").toBool();
	ui.tableWidget_narahStrings->setColumnHidden(2,!boolVal);
	ui.tableWidget_narahStrings->setColumnHidden(3,!boolVal);
	ui.tableWidget_narahStrings->setColumnHidden(4,!boolVal);
	ui.tableWidget_narahStrings->setColumnHidden(5,!boolVal);
	ui.tableWidget_narahStrings->setColumnHidden(6,!boolVal);
	boolVal = settings->value("GUI_View/ShowNarahGV").toBool();
	ui.tableWidget_narahStrings->setColumnHidden(7,!boolVal);
	ui.tableWidget_narahStrings->setColumnHidden(8,!boolVal);
	ui.tableWidget_narahStrings->setColumnHidden(9,!boolVal);
	ui.tableWidget_narahStrings->setColumnHidden(10,!boolVal);
	ui.tableWidget_narahStrings->setColumnHidden(11,!boolVal);
	boolVal = settings->value("GUI_View/ShowNarahST").toBool();
	ui.tableWidget_narahStrings->setColumnHidden(12,!boolVal);
	ui.checkBox_allColumns->setChecked(settings->value("GUI_View/ShowAllColumns").toBool());
	delete settings;
	
	columnsList << "id"
				<< "Vh_saldo"
				<< "Narah"
				<< "Pilga_sum"
				<< "Pilga_ID"
				<< "Pilga_for"
				<< "Pilga_plos"
				<< "Narah_GV"
				<< "Pilga_sum_GV"
				<< "Pilga_ID_GV"
				<< "Pilga_for_GV"
				<< "Obsyag_GV"
				<< "Narah_St"
				<< "Year_pilg_pererah"
				<< "Month_pilg_pererah"
				<< "PochDayCO"
				<< "KincDayCO"
				<< "PochDayGV"
				<< "KincDayGV"
				<< "Status";
	
	populateNarahStringsTable();
	setColumns();
	
	connect(ui.toolButton_closeDialog, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui.toolButton_addOperaciiString, SIGNAL(clicked()), this, SLOT(insertNarahString()));
	connect(ui.toolButton_delOperaciiString, SIGNAL(clicked()), this, SLOT(deleteNarahString()));
	connect(ui.dateEdit_potochnaData, SIGNAL(dateChanged(const QDate &)), this, SLOT(populateNarahStringsTable()));
	connect(ui.checkBox_allColumns, SIGNAL(stateChanged(int)), this, SLOT(setColumns()));
	
}
//------------------------------------------------------------
void tke_Dialog_editOperaciiStrings::populateNarahStringsTable()
{
	disconnect(ui.tableWidget_narahStrings, SIGNAL(cellChanged(int,int)), this, SLOT(tableWidget_narahStrings_cellChanged(int,int)));
	
	int row, rowCount, col;
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT count(*) FROM narahuvannya \
				WHERE Rahunok="+QString::number(rahunok)+" \
					and year="+QString::number(ui.dateEdit_potochnaData->date().year())+" \
					and month="+QString::number(ui.dateEdit_potochnaData->date().month()));
	query->seek(0);
	rowCount = query->value(0).toInt();
	ui.tableWidget_narahStrings->setRowCount(rowCount);
	
	query->exec("SELECT id, Vh_saldo, \
						Narah, Pilga_sum, Pilga_ID, Pilga_for, Pilga_plos, \
						Narah_GV, Pilga_sum_GV, Pilga_ID_GV, Pilga_for_GV, Obsyag_GV, \
						Narah_St, \
						Year_pilg_pererah, Month_pilg_pererah, \
						PochDayCO, KincDayCO, PochDayGV, KincDayGV, \
						Status \
				FROM narahuvannya \
				WHERE Rahunok="+QString::number(rahunok)+" \
					and year="+QString::number(ui.dateEdit_potochnaData->date().year())+" \
					and month="+QString::number(ui.dateEdit_potochnaData->date().month())+" \
				ORDER BY Status");
	for (row=0; row<rowCount && query->next(); row++){
		populateNarahStringsRow(row, query->record());
	}
	
	delete query;
	
	ui.tableWidget_narahStrings->resizeColumnsToContents();
	for (col=0; col<colCount; ++col){
		if (ui.tableWidget_narahStrings->columnWidth(col) < MIN_COLUMN_WIDTH)
			ui.tableWidget_narahStrings->setColumnWidth( col, MIN_COLUMN_WIDTH );
	}
	ui.tableWidget_narahStrings->resizeRowsToContents();
	
	if (rowCount<1)
		ui.toolButton_delOperaciiString->setEnabled(false);
	else
		ui.toolButton_delOperaciiString->setEnabled(true);
	
	connect(ui.tableWidget_narahStrings, SIGNAL(cellChanged(int,int)), this, SLOT(tableWidget_narahStrings_cellChanged(int,int)));
}
//------------------------------------------------------------
void tke_Dialog_editOperaciiStrings::populateNarahStringsRow(int row, QSqlRecord record)
{
	item = new QTableWidgetItem(record.value(0).toString());
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget_narahStrings->setItem(row, 0, item);
	item = new QTableWidgetItem(uMToStr2(record.value(1).toDouble()));
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget_narahStrings->setItem(row, 1, item);
	item = new QTableWidgetItem(uMToStr2(record.value(2).toDouble()));
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget_narahStrings->setItem(row, 2, item);
	item = new QTableWidgetItem(uMToStr2(record.value(3).toDouble()));
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget_narahStrings->setItem(row, 3, item);
	item = new QTableWidgetItem(record.value(4).toString());
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget_narahStrings->setItem(row, 4, item);
	item = new QTableWidgetItem(record.value(5).toString());
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget_narahStrings->setItem(row, 5, item);
	item = new QTableWidgetItem(record.value(6).toString());
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget_narahStrings->setItem(row, 6, item);
	item = new QTableWidgetItem(uMToStr2(record.value(7).toDouble()));
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget_narahStrings->setItem(row, 7, item);
	item = new QTableWidgetItem(uMToStr2(record.value(8).toDouble()));
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget_narahStrings->setItem(row, 8, item);
	item = new QTableWidgetItem(record.value(9).toString());
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget_narahStrings->setItem(row, 9, item);
	item = new QTableWidgetItem(record.value(10).toString());
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget_narahStrings->setItem(row, 10, item);
	item = new QTableWidgetItem(record.value(11).toString());
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget_narahStrings->setItem(row, 11, item);
	item = new QTableWidgetItem(uMToStr2(record.value(12).toDouble()));
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget_narahStrings->setItem(row, 12, item);
	item = new QTableWidgetItem(record.value(13).toString());
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget_narahStrings->setItem(row, 13, item);
	item = new QTableWidgetItem(record.value(14).toString());
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget_narahStrings->setItem(row, 14, item);
	item = new QTableWidgetItem(record.value(15).toString());
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget_narahStrings->setItem(row, 15, item);
	item = new QTableWidgetItem(record.value(16).toString());
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget_narahStrings->setItem(row, 16, item);
	item = new QTableWidgetItem(record.value(17).toString());
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget_narahStrings->setItem(row, 17, item);
	item = new QTableWidgetItem(record.value(18).toString());
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget_narahStrings->setItem(row, 18, item);
	item = new QTableWidgetItem(record.value(19).toString());
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget_narahStrings->setItem(row, 19, item);
}
//------------------------------------------------------------
void tke_Dialog_editOperaciiStrings::tableWidget_narahStrings_cellChanged(int row, int col)
{
	disconnect(ui.tableWidget_narahStrings, SIGNAL(cellChanged(int,int)), this, SLOT(tableWidget_narahStrings_cellChanged(int,int)));
	
	QSqlQuery *query = new QSqlQuery();
	query->exec("UPDATE narahuvannya SET "+columnsList.at(col)+"="+ui.tableWidget_narahStrings->item(row,col)->text().replace(",",".")+" \
				WHERE id="+ui.tableWidget_narahStrings->item(row,0)->text());
	query->exec("SELECT id, Vh_saldo, \
						Narah, Pilga_sum, Pilga_ID, Pilga_for, Pilga_plos, \
						Narah_GV, Pilga_sum_GV, Pilga_ID_GV, Pilga_for_GV, Obsyag_GV, \
						Narah_St, \
						Year_pilg_pererah, Month_pilg_pererah, \
						PochDayCO, KincDayCO, PochDayGV, KincDayGV, \
						Status \
				FROM narahuvannya \
				WHERE id="+ui.tableWidget_narahStrings->item(row,0)->text());
	query->seek(0);
	
	populateNarahStringsRow(row, query->record());
	
	emit requestReloadNarahuvannya();
	
	delete query;
	
	connect(ui.tableWidget_narahStrings, SIGNAL(cellChanged(int,int)), this, SLOT(tableWidget_narahStrings_cellChanged(int,int)));
}
//------------------------------------------------------------
void tke_Dialog_editOperaciiStrings::insertNarahString()
{
	disconnect(ui.tableWidget_narahStrings, SIGNAL(cellChanged(int,int)), this, SLOT(tableWidget_narahStrings_cellChanged(int,int)));
	
	int id = nextLichylnyk("narahuvannya");
	QSqlQuery *query = new QSqlQuery();
	short daysInCurMonth = ui.dateEdit_potochnaData->date().daysInMonth();
	QString statusStr = "11";
	if (organizID() == 4){ //Для Стебника статус 0
		statusStr = "0";
	}
	if (query->exec("INSERT INTO narahuvannya (id, Rahunok, Year, Month, Vh_saldo, Narah, Pilga_sum, Pilga_ID, Pilga_for, Pilga_plos, \
						Narah_GV, Pilga_sum_GV, Pilga_ID_GV, Pilga_for_GV, Obsyag_GV, Narah_St, \
						Year_pilg_pererah, Month_pilg_pererah, PochDayCO, KincDayCO, PochDayGV, KincDayGV, \
						Status) \
						VALUES ("+QString::number(id)+","+
								QString::number(rahunok)+", "+
								QString::number(ui.dateEdit_potochnaData->date().year())+", "+
								QString::number(ui.dateEdit_potochnaData->date().month())+
								",0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,"+QString::number(daysInCurMonth)+",1,"+QString::number(daysInCurMonth)+","+statusStr+")")){
		query->exec("SELECT id, Vh_saldo, \
						Narah, Pilga_sum, Pilga_ID, Pilga_for, Pilga_plos, \
						Narah_GV, Pilga_sum_GV, Pilga_ID_GV, Pilga_for_GV, Obsyag_GV, \
						Narah_St, \
						Year_pilg_pererah, Month_pilg_pererah, \
						PochDayCO, KincDayCO, PochDayGV, KincDayGV, \
						Status \
				FROM narahuvannya \
				WHERE id="+QString::number(id));
		query->seek(0);
		int row = ui.tableWidget_narahStrings->rowCount();
		ui.tableWidget_narahStrings->setRowCount(row+1);
		
		populateNarahStringsRow(row, query->record());
		
		ui.tableWidget_narahStrings->resizeRowToContents(row);
		
		ui.tableWidget_narahStrings->setCurrentCell(row,1);
		ui.tableWidget_narahStrings->editItem(ui.tableWidget_narahStrings->item(row,1));
		
		emit requestReloadNarahuvannya();
	}
	else{
		qDebug() << "Помилка створення стрічки нарахування: " << query->lastError().text() << "\nзапит: " << query->lastQuery();
		QMessageBox::critical(
            this,
            "!!! Помилка запису до бази даних !!!",
            "Програма не змогла додати запис до бази даних.",
            QMessageBox::Ok,
            QMessageBox::NoButton,
			QMessageBox::NoButton);
	}
	delete query;
	
	if (ui.tableWidget_narahStrings->rowCount() < 1)
		ui.toolButton_delOperaciiString->setEnabled(false);
	else
		ui.toolButton_delOperaciiString->setEnabled(true);
	
	connect(ui.tableWidget_narahStrings, SIGNAL(cellChanged(int,int)), this, SLOT(tableWidget_narahStrings_cellChanged(int,int)));
}
//------------------------------------------------------------
void tke_Dialog_editOperaciiStrings::deleteNarahString()
{
	int row = ui.tableWidget_narahStrings->currentRow();
	if (ui.tableWidget_narahStrings->item(row,19)->text().toInt() == 0){
		QMessageBox::critical(
            this,
            "!!! Невірна операція !!!",
            "Ви не можете видалити стрічку зі статусом \"0\",\nоскільки це негативно вплине на структуру бази даних",
            QMessageBox::Ok,
            QMessageBox::NoButton,
			QMessageBox::NoButton);
		return;
	}
	
	disconnect(ui.tableWidget_narahStrings, SIGNAL(cellChanged(int,int)), this, SLOT(tableWidget_narahStrings_cellChanged(int,int)));
	
	QSqlQuery *query = new QSqlQuery();
	if (query->exec("DELETE FROM narahuvannya WHERE id="+ui.tableWidget_narahStrings->item(row,0)->text())){
		ui.tableWidget_narahStrings->removeRow(row);
		emit requestReloadNarahuvannya();
	}
	else
		QMessageBox::critical(
            this,
            "!!! Помилка видалення !!!",
            "Програма не змогла видалити запис з бази даних.",
            QMessageBox::Ok,
            QMessageBox::NoButton,
			QMessageBox::NoButton);
	delete query;
	
	if (ui.tableWidget_narahStrings->rowCount() < 1)
		ui.toolButton_delOperaciiString->setEnabled(false);
	else
		ui.toolButton_delOperaciiString->setEnabled(true);
	
	connect(ui.tableWidget_narahStrings, SIGNAL(cellChanged(int,int)), this, SLOT(tableWidget_narahStrings_cellChanged(int,int)));
}
//------------------------------------------------------------
void tke_Dialog_editOperaciiStrings::setColumns()
{
	bool vis = ui.checkBox_allColumns->isChecked();
	ui.tableWidget_narahStrings->setColumnHidden(13, !vis);
	ui.tableWidget_narahStrings->setColumnHidden(14, !vis);
	ui.tableWidget_narahStrings->setColumnHidden(15, !vis);
	ui.tableWidget_narahStrings->setColumnHidden(16, !vis);
	ui.tableWidget_narahStrings->setColumnHidden(17, !vis);
	ui.tableWidget_narahStrings->setColumnHidden(18, !vis);
}
//------------------------------------------------------------
tke_Dialog_editOperaciiStrings::~tke_Dialog_editOperaciiStrings()
{
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	settings->beginGroup("GUI_View");
	settings->setValue("ShowAllColumns",ui.checkBox_allColumns->isChecked());
	delete settings;
}
//------------------------------------------------------------
