//------------------------------------------------------------
//
// Implamantation of tke_Dialog_narah class.
// Created by Roman Mulyk 26.11.2006
//
//------------------------------------------------------------

#include "tke_Form_insert_opSubs.h"
#include "../tkeOrganizacii/udiff_func.h"
#include <xbase64.h>
#include <UMath>
#include <USqlAccessible>
#include <UStandardDelegate>

//------------------------------------------------------------
tke_Form_insert_opSubs::tke_Form_insert_opSubs(QDate dt, QWidget *parent)
        : QWidget(parent)
{
    ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose, true);
	
	dirModel = new QDirModel();
    dirModel->setSorting(QDir::DirsFirst | QDir::IgnoreCase | QDir::Name);
	dirModel->setFilter(QDir::AllDirs | QDir::Drives | QDir::Files | QDir::NoDotAndDotDot);
	dirModel->setNameFilters( QStringList() << "*.dbf" );
	ui.treeView_dirTree->setModel(dirModel);
	ui.treeView_dirTree->header()->resizeSection(0, 300 );
	ui.treeView_dirTree->header()->resizeSection(1, 70 );
	ui.treeView_dirTree->header()->setSectionHidden(2, true );
	ui.treeView_dirTree->header()->setStretchLastSection(true);
    ui.treeView_dirTree->header()->setSortIndicator(0, Qt::AscendingOrder);
    ui.treeView_dirTree->header()->setSortIndicatorShown(true);
    ui.treeView_dirTree->header()->setClickable(true);
	
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	QModelIndex ind = dirModel->index(settings->value("ImportOpSubs/DefaultSubsPath").toString());
	delete settings;
	
	ui.treeView_dirTree->expand(ind);
	ui.treeView_dirTree->scrollTo(ind);
	ui.treeView_dirTree->setCurrentIndex(ind);
	
	
	ui.tableWidget_opSubs->setColumnCount(4);
	ui.tableWidget_opSubs->setHorizontalHeaderLabels(QStringList()<<"Рахунок"<<"ОП ЦО"<<"ОП ГВ"<<"Місяць");
	
	ui.tableWidget_opSubs->setItemDelegateForColumn(0, new USpinBoxDelegate(-999999999, 999999999, ui.tableWidget_opSubs));
	ui.tableWidget_opSubs->setItemDelegateForColumn(1, new UDoubleSpinBoxDelegate(0, 999999999, 2, ui.tableWidget_opSubs));
	ui.tableWidget_opSubs->setItemDelegateForColumn(2, new UDoubleSpinBoxDelegate(0, 999999999, 2, ui.tableWidget_opSubs));
	ui.tableWidget_opSubs->setItemDelegateForColumn(3, new UDateTimeDelegate(ui.tableWidget_opSubs, "MM.yyyy"));
	
	
		//connectors 
	connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(close()));
	connect(ui.pushButton_reload, SIGNAL(clicked()), ui.treeView_dirTree, SLOT(refresh()));
	connect(ui.pushButton_add, SIGNAL(clicked()), this, SLOT(pushButton_add_clicked()));
	connect(ui.pushButton_del, SIGNAL(clicked()), this, SLOT(pushButton_del_clicked()));
	connect(ui.pushButton_insertOpSubs, SIGNAL(clicked()), this, SLOT(pushButton_insertOpSubs_clicked()));
	connect(ui.treeView_dirTree, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(readOpSubsFromFile()));
}
//------------------------------------------------------------
void tke_Form_insert_opSubs::pushButton_add_clicked()
{
	int row = ui.tableWidget_opSubs->rowCount();
	ui.tableWidget_opSubs->setRowCount(row+1);
	QTableWidgetItem * item;
	
	item = new QTableWidgetItem();
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget_opSubs->setItem(row, 0, item);
	
	item = new QTableWidgetItem();
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget_opSubs->setItem(row, 1, item);
	
	item = new QTableWidgetItem();
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget_opSubs->setItem(row, 2, item);
	
	item = new QTableWidgetItem();
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget_opSubs->setItem(row, 3, item);
	
	ui.tableWidget_opSubs->resizeRowToContents(row);
	
	item = ui.tableWidget_opSubs->item(row, 0);
	ui.tableWidget_opSubs->scrollToItem(item);
	ui.tableWidget_opSubs->setCurrentItem(item);
	ui.tableWidget_opSubs->editItem(item);
}
//------------------------------------------------------------
void tke_Form_insert_opSubs::pushButton_del_clicked()
{
	int selRow = ui.tableWidget_opSubs->currentRow();
	
	QModelIndexList lst = ui.tableWidget_opSubs->selectionModel()->selectedRows();
	foreach(QModelIndex ind, lst){
		int row = ind.row();
		if (selRow == row)
			selRow--;
		if (selRow < 0)
			selRow = 0;
		ui.tableWidget_opSubs->removeRow(ind.row());
	}
	
	if (ui.tableWidget_opSubs->rowCount() <= selRow)
		selRow = ui.tableWidget_opSubs->rowCount()-1;
	if (ui.tableWidget_opSubs->rowCount() != 0)
		ui.tableWidget_opSubs->setCurrentCell(selRow, 0);
}
//------------------------------------------------------------
void tke_Form_insert_opSubs::pushButton_insertOpSubs_clicked()
{
	int row = 0;
	bool okAll = true;
	for (row=0; row<ui.tableWidget_opSubs->rowCount(); row++){
		bool ok = insertOpRowToDatabase(ui.tableWidget_opSubs->item(row,3)->data(Qt::EditRole).toDate(), 
								ui.tableWidget_opSubs->item(row,0)->data(Qt::EditRole).toInt(), 
								ui.tableWidget_opSubs->item(row,1)->data(Qt::EditRole).toDouble(), 
								ui.tableWidget_opSubs->item(row,2)->data(Qt::EditRole).toDouble());
		if (ok){
			ui.tableWidget_opSubs->removeRow(row);
			--row;
		}
		else
			okAll = false;
	}
	if (!okAll)
		QMessageBox::information(0, "Помилка внесення", "Програма не змогла занести до бази даних стрічки,\n"
									"які залишились у таблиці");
}
//------------------------------------------------------------
void tke_Form_insert_opSubs::currentRowChanged()
{
	int curRow = ui.tableWidget_opSubs->currentRow();
	if (curRow>=0 && ui.tableWidget_opSubs->rowCount()>0)
		emit u_abonent_rahunok_changes(ui.tableWidget_opSubs->item(curRow,0)->data(Qt::EditRole).toInt());
}
//------------------------------------------------------------
void tke_Form_insert_opSubs::readOpSubsFromFile()
{
	if (dirModel->isDir(ui.treeView_dirTree->currentIndex()))
		return;
	
	disconnect(ui.tableWidget_opSubs->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &,const QModelIndex &)), this, SLOT(currentRowChanged()));
	
	QSqlQuery query;
	query.exec("SELECT strTemplate FROM slujb_znach WHERE id=15");
	query.next();
	QString rahColName = query.value(0).toString();
	query.exec("SELECT strTemplate FROM slujb_znach WHERE id=16");
	query.next();
	QString opCoColName = query.value(0).toString();
	query.exec("SELECT strTemplate FROM slujb_znach WHERE id=17");
	query.next();
	QString opGvColName = query.value(0).toString();
	
	QString fName = dirModel->fileInfo(ui.treeView_dirTree->currentIndex()).absoluteFilePath();
	
	xbXBase x;
	xbDbf dbf(&x);
	if (dbf.OpenDatabase(fName.toAscii().data()) != XB_NO_ERROR)
		QMessageBox::critical(0, "Помилка відкривання", "Програмі не вдалося відкрити файл:\n"+fName);
	ui.tableWidget_opSubs->setRowCount(dbf.NoOfRecords());
	QTableWidgetItem *item;
	int row=0;
	double opCO, opGV;
	while (dbf.GetNextRecord() == XB_NO_ERROR){
		opCO = QString(dbf.GetField(dbf.GetFieldNo(opCoColName.toAscii().data()))).toDouble();
		opGV = QString(dbf.GetField(dbf.GetFieldNo(opGvColName.toAscii().data()))).toDouble();
		
		if ((opCO != 0) || (opGV != 0)) {
			item = new QTableWidgetItem();
			item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			item->setData(Qt::EditRole, QString(dbf.GetField(dbf.GetFieldNo(rahColName.toAscii().data()))).toInt());
			ui.tableWidget_opSubs->setItem(row, 0, item);
			
			item = new QTableWidgetItem();
			item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			item->setData(Qt::EditRole, opCO);
			ui.tableWidget_opSubs->setItem(row, 1, item);
			
			item = new QTableWidgetItem();
			item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			item->setData(Qt::EditRole, opGV);
			ui.tableWidget_opSubs->setItem(row, 2, item);
			
			item = new QTableWidgetItem();
			item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			item->setData(Qt::EditRole, QDate::fromString(dbf.GetField(dbf.GetFieldNo("DAT1")), "yyyyMMdd"));
			ui.tableWidget_opSubs->setItem(row, 3, item);
			
			row++;
		}
	}
	ui.tableWidget_opSubs->setRowCount(row);
	dbf.CloseDatabase();
	ui.tableWidget_opSubs->resizeRowsToContents();
	
	currentRowChanged();
	connect(ui.tableWidget_opSubs->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &,const QModelIndex &)), this, SLOT(currentRowChanged()));
}
//------------------------------------------------------------
bool tke_Form_insert_opSubs::insertOpRowToDatabase(QDate dt, int rahunok, double opCo, double opGv)
{
	if (!dt.isValid())
		return false;
	
	QSqlQuery query;
	query.exec("SELECT Rahunok FROM abonent WHERE Rahunok="+sqlStr(rahunok));
	if (!query.next())
		return false;
	
	int rowId=0;
	query.exec("SELECT id FROM plataZgidnoSubs \
				WHERE Year="+sqlStr(dt.year())+" and Month="+sqlStr(dt.month())+" and Rahunok_ID="+sqlStr(rahunok));
	if (query.next())
		rowId = query.value(0).toInt();
	if (rowId==0){//Insert
		return query.exec("INSERT INTO plataZgidnoSubs (Rahunok_ID, Year, Month, Plata_CO, Plata_GV) \
					VALUES ("+sqlStr(rahunok)+", \
							"+sqlStr(dt.year())+", \
							"+sqlStr(dt.month())+", \
							"+sqlStr(opCo)+", \
							"+sqlStr(opGv)+")");
	}
	else{//Update
		return query.exec("UPDATE plataZgidnoSubs SET \
						Month="+sqlStr(dt.month())+" \
						"+(opCo>0.0099?QString(", Plata_CO=")+sqlStr(opCo)+QString(" "):QString(""))+" \
						"+(opGv>0.0099?QString(", Plata_GV=")+sqlStr(opGv)+QString(" "):QString(""))+" \
					WHERE id="+sqlStr(rowId));
	}
}
//------------------------------------------------------------
tke_Form_insert_opSubs::~tke_Form_insert_opSubs()
{
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	settings->beginGroup("ImportUni");
	settings->setValue("ImportOpSubs/DefaultSubsPath", QFileInfo(dirModel->filePath(ui.treeView_dirTree->currentIndex())).absolutePath());
	delete settings;
}
//------------------------------------------------------------
