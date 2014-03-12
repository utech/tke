//------------------------------------------------------------
//
// Implamantation of tke_form_struct_GV_organiz class.
// Created by Roman Mulyk 16.06.2008
//
//------------------------------------------------------------

#include <UStandardDelegate>
#include <USqlAccessible>
#include "tke_form_struct_GV_organiz.h"
//------------------------------------------------------------
tke_form_struct_GV_organiz::tke_form_struct_GV_organiz( QWidget * parent) 
	: QWidget(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	
	ui.tableWidget_kotelni->setColumnHidden(0,true);
	ui.tableWidget_zagLich->setColumnHidden(0,true);
	ui.tableWidget_pidrozdLich->setColumnHidden(0,true);
	
	ui.tableWidget_zagLich->setItemDelegateForColumn(4, new UBoolComboDelegate(ui.tableWidget_zagLich));
	ui.tableWidget_pidrozdLich->setItemDelegateForColumn(4, new UBoolComboDelegate(ui.tableWidget_pidrozdLich));
	
	zagLichModel = ui.tableWidget_zagLich->model();
	pidrozdLichModel = ui.tableWidget_pidrozdLich->model();
	
	populateKotelni();
	 
	connect(ui.tableWidget_kotelni, SIGNAL(cellChanged(int,int)), this, SLOT(updateKotelni(int,int)));
	connect(ui.tableWidget_zagLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichBudynkiv(int,int)));
	connect(ui.tableWidget_pidrozdLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichAbonentiv(int,int)));
	
	connect(ui.toolButton_addKotelni, SIGNAL(clicked()), this, SLOT(toolButton_addKotelni_clicked()));
	connect(ui.toolButton_delKotelni, SIGNAL(clicked()), this, SLOT(toolButton_delKotelni_clicked()));
	
	connect(ui.toolButton_addBudLich, SIGNAL(clicked()), this, SLOT(toolButton_addBudLich_clicked()));
	connect(ui.toolButton_delBudLich, SIGNAL(clicked()), this, SLOT(toolButton_delBudLich_clicked()));
	
	connect(ui.toolButton_addAbonLich, SIGNAL(clicked()), this, SLOT(toolButton_addAbonLich_clicked()));
	connect(ui.toolButton_delAbonLich, SIGNAL(clicked()), this, SLOT(toolButton_delAbonLich_clicked()));
	
	connect(ui.toolButton_reload, SIGNAL(clicked()), this, SLOT(toolButton_reload_clicked()));
	connect(ui.toolButton_closeForm, SIGNAL(clicked()), this, SLOT(close()));
}
//------------------------------------------------------------
void tke_form_struct_GV_organiz::populateKotelni()
{
	disconnect(ui.tableWidget_kotelni->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &,const QModelIndex &)), this, SLOT(populateBudynky()));
	disconnect(ui.tableWidget_kotelni, SIGNAL(cellChanged(int,int)), this, SLOT(updateKotelni(int,int)));
	
	int rowCount=0;
	int oldId=1;
	if (ui.tableWidget_kotelni->rowCount()>0){
		oldId = ui.tableWidget_kotelni->item(ui.tableWidget_kotelni->currentRow(),0)->text().toInt();
	}
	
	QSqlQuery *query = new QSqlQuery();
	
	query->exec("SELECT count(*) FROM kotelni");
	query->seek(0);
	rowCount = query->value(0).toInt();
	ui.tableWidget_kotelni->setRowCount(rowCount);
	query->exec("SELECT id, Kot_num, Vulycia, Nomer, Nas_punkt, Tel FROM kotelni");
	bool isSelected=false;
	for (int row=0; (row<rowCount) && query->next(); ++row){
		ui.tableWidget_kotelni->setItem(row, 0, new QTableWidgetItem(query->value(0).toString()));
		ui.tableWidget_kotelni->setItem(row, 1, new QTableWidgetItem(query->value(1).toString()));
		ui.tableWidget_kotelni->setItem(row, 2, new QTableWidgetItem(query->value(2).toString()));
		ui.tableWidget_kotelni->setItem(row, 3, new QTableWidgetItem(query->value(3).toString()));
		ui.tableWidget_kotelni->setItem(row, 4, new QTableWidgetItem(query->value(4).toString()));
		ui.tableWidget_kotelni->setItem(row, 5, new QTableWidgetItem(query->value(5).toString()));
		
		if (query->value(0).toInt() == oldId){
			isSelected = true;
			ui.tableWidget_kotelni->setCurrentCell( row, 1);
		}
	}
	if (!isSelected && (ui.tableWidget_kotelni->rowCount()>0))
		ui.tableWidget_kotelni->setCurrentCell( 0, 1);
	ui.tableWidget_kotelni->resizeColumnsToContents();
	ui.tableWidget_kotelni->resizeRowsToContents();
	delete query;
	
	ui.label_countKotelni->setText("Всього: "+QString::number(rowCount)+".");
	
	populateLichZagaln();
	
	connect(ui.tableWidget_kotelni, SIGNAL(cellChanged(int,int)), this, SLOT(updateKotelni(int,int)));
	connect(ui.tableWidget_kotelni->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &,const QModelIndex &)), this, SLOT(populateBudynky()));
}
//------------------------------------------------------------
void tke_form_struct_GV_organiz::populateLichZagaln()
{
	disconnect(ui.tableWidget_zagLich->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &,const QModelIndex &)), this, SLOT(populateLichAbonentiv()));
	disconnect(ui.tableWidget_zagLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichBudynkiv(int,int)));
	
	int rowCount=0;
	int oldId=1;
	
	if (ui.tableWidget_zagLich->rowCount()>0)
		oldId = ui.tableWidget_zagLich->item(ui.tableWidget_zagLich->currentRow(),0)->text().toInt();
	if (kotelnyaId > 0){
		QSqlQuery *query = new QSqlQuery();
		
		query->exec("SELECT count(*) FROM lichylnykyBudynkovi WHERE Budynok_id="+QString::number(kotelnyaId));
		query->seek(0);
		rowCount = query->value(0).toInt();
		ui.tableWidget_zagLich->setRowCount(rowCount);
		query->exec("SELECT id, LichNum, StartKvNumber, EndKvNumber, IsLichylnyk, TestDate FROM lichylnykyBudynkovi WHERE Budynok_id="+QString::number(kotelnyaId));
		bool isSelected=false;
		for (int row=0; (row<rowCount) && query->next(); ++row){
			ui.tableWidget_zagLich->setItem(row, 0, new QTableWidgetItem(query->value(0).toString()));
			ui.tableWidget_zagLich->setItem(row, 1, new QTableWidgetItem(query->value(1).toString()));
			ui.tableWidget_zagLich->setItem(row, 2, new QTableWidgetItem(query->value(2).toString()));
			ui.tableWidget_zagLich->setItem(row, 3, new QTableWidgetItem(query->value(3).toString()));
			zagLichModel->setData(zagLichModel->index(row, 4), query->value(4).toBool(), Qt::EditRole );
			ui.tableWidget_zagLich->setItem(row, 5, new QTableWidgetItem(query->value(5).toDate().toString("dd.MM.yyyy")));
		
			if (query->value(0).toInt() == oldId){
				isSelected = true;
				ui.tableWidget_zagLich->setCurrentCell( row, 1);
			}
		}
		if (!isSelected && (ui.tableWidget_zagLich->rowCount()>0))
			ui.tableWidget_zagLich->setCurrentCell( 0, 1);
		ui.tableWidget_zagLich->resizeColumnsToContents();
		ui.tableWidget_zagLich->resizeRowsToContents();
		delete query;
	}
	else
		ui.tableWidget_zagLich->setRowCount(0);
	
	if (rowCount==0){
		ui.toolButton_addAbonLich->setEnabled(false);
		ui.toolButton_delAbonLich->setEnabled(false);
	}
	else{
		ui.toolButton_addAbonLich->setEnabled(true);
		ui.toolButton_delAbonLich->setEnabled(true);
	}
	
	ui.label_countBudLich->setText("Всього: "+QString::number(rowCount)+".");
	
	populateLichPidrozdily();
	
	connect(ui.tableWidget_zagLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichBudynkiv(int,int)));
	connect(ui.tableWidget_zagLich->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &,const QModelIndex &)), this, SLOT(populateLichAbonentiv()));
}
//------------------------------------------------------------
void tke_form_struct_GV_organiz::populateLichPidrozdily()
{
	disconnect(ui.tableWidget_pidrozdLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichAbonentiv(int,int)));
	
	int rowCount=0;
	int oldId=1;
	if (ui.tableWidget_pidrozdLich->rowCount()>0)
		oldId = ui.tableWidget_pidrozdLich->item(ui.tableWidget_pidrozdLich->currentRow(),0)->text().toInt();
	if (ui.tableWidget_zagLich->rowCount()>0)
		budLichId = ui.tableWidget_zagLich->item(ui.tableWidget_zagLich->currentRow(),0)->text().toInt();
	else budLichId = 0;
	if (budLichId > 0){
		QSqlQuery *query = new QSqlQuery();
		query->exec("SELECT count(*) FROM lichylnykyAbon WHERE BudLich_ID="+QString::number(budLichId));
		query->seek(0);
		rowCount = query->value(0).toInt();
		ui.tableWidget_pidrozdLich->setRowCount(rowCount);
		query->exec("SELECT id, Rahunok, LichNum, Koef, IsLichylnyk, DataPr FROM lichylnykyAbon WHERE BudLich_ID="+QString::number(budLichId)+" ORDER BY Rahunok");
		bool isSelected=false;
		for (int row=0; (row<rowCount) && query->next(); ++row){
			ui.tableWidget_pidrozdLich->setItem(row, 0, new QTableWidgetItem(query->value(0).toString()));
			ui.tableWidget_pidrozdLich->setItem(row, 1, new QTableWidgetItem(query->value(1).toString()));
			ui.tableWidget_pidrozdLich->setItem(row, 2, new QTableWidgetItem(query->value(2).toString()));
			ui.tableWidget_pidrozdLich->setItem(row, 3, new QTableWidgetItem(query->value(3).toString()));
			pidrozdLichModel->setData(pidrozdLichModel->index(row, 4), query->value(4).toBool(), Qt::EditRole );
			ui.tableWidget_pidrozdLich->setItem(row, 5, new QTableWidgetItem(query->value(5).toDate().toString("dd.MM.yyyy")));
			if (query->value(0).toInt() == oldId){
				isSelected = true;
				ui.tableWidget_pidrozdLich->setCurrentCell( row, 1);
			}
		}
		if (!isSelected && (ui.tableWidget_pidrozdLich->rowCount()>0))
			ui.tableWidget_pidrozdLich->setCurrentCell( 0, 1);
		delete query;
	}
	else
		ui.tableWidget_pidrozdLich->setRowCount(0);
	
	ui.label_countAbonLich->setText("Всього: "+QString::number(rowCount)+".");
	
	ui.tableWidget_pidrozdLich->resizeColumnsToContents();
	ui.tableWidget_pidrozdLich->resizeRowsToContents();
	
	connect(ui.tableWidget_pidrozdLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichAbonentiv(int,int)));
}
//------------------------------------------------------------
void tke_form_struct_GV_organiz::updateKotelni(int row, int col)
{
	disconnect(ui.tableWidget_kotelni, SIGNAL(cellChanged(int,int)), this, SLOT(updateKotelni(int,int)));
	
	adQuery.exec("UPDATE kotelni SET Kot_num="+ui.tableWidget_kotelni->item(row,1)->text()+", \
									Vulycia='"+ui.tableWidget_kotelni->item(row,2)->text()+"', \
									Nomer='"+ui.tableWidget_kotelni->item(row,3)->text()+"', \
									Nas_punkt='"+ui.tableWidget_kotelni->item(row,4)->text()+"', \
									Tel='"+ui.tableWidget_kotelni->item(row,5)->text()+"' \
					WHERE id="+ui.tableWidget_kotelni->item(row,0)->text());
	adQuery.exec("SELECT Kot_num, Vulycia, Nomer, Nas_punkt, Tel FROM kotelni WHERE id="+ui.tableWidget_kotelni->item(row,0)->text());
	if (adQuery.seek(0)){
		ui.tableWidget_kotelni->setItem(row, 1, new QTableWidgetItem(adQuery.value(0).toString()));
		ui.tableWidget_kotelni->setItem(row, 2, new QTableWidgetItem(adQuery.value(1).toString()));
		ui.tableWidget_kotelni->setItem(row, 3, new QTableWidgetItem(adQuery.value(2).toString()));
		ui.tableWidget_kotelni->setItem(row, 4, new QTableWidgetItem(adQuery.value(3).toString()));
		ui.tableWidget_kotelni->setItem(row, 5, new QTableWidgetItem(adQuery.value(4).toString()));
	}
	
	connect(ui.tableWidget_kotelni, SIGNAL(cellChanged(int,int)), this, SLOT(updateKotelni(int,int)));
}
//------------------------------------------------------------
void tke_form_struct_GV_organiz::updateLichZagaln(int row, int col)
{
	disconnect(ui.tableWidget_zagLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichBudynkiv(int,int)));

	QString testDateStr = ui.tableWidget_zagLich->item(row,5)->text();
	if (testDateStr.size()==0)
		testDateStr="null";
	else
		testDateStr='\''+testDateStr+'\'';
	bool isLich = zagLichModel->data(zagLichModel->index(row,4)).toBool();
	QString lichStr;
	if (isLich)
		lichStr = "true";
	else
		lichStr = "false";
	adQuery.exec("UPDATE lichylnykyBudynkovi SET LichNum='"+ui.tableWidget_zagLich->item(row,1)->text()+"', \
									StartKvNumber="+ui.tableWidget_zagLich->item(row,2)->text()+", \
									EndKvNumber="+ui.tableWidget_zagLich->item(row,3)->text()+", \
									IsLichylnyk="+lichStr+", \
									TestDate="+testDateStr+" \
					WHERE id="+ui.tableWidget_zagLich->item(row,0)->text());
	adQuery.exec("SELECT LichNum, StartKvNumber, EndKvNumber, IsLichylnyk, TestDate FROM lichylnykyBudynkovi WHERE id="+ui.tableWidget_zagLich->item(row,0)->text());
	if (adQuery.seek(0)){
		ui.tableWidget_zagLich->setItem(row, 1, new QTableWidgetItem(adQuery.value(0).toString()));
		ui.tableWidget_zagLich->setItem(row, 2, new QTableWidgetItem(adQuery.value(1).toString()));
		ui.tableWidget_zagLich->setItem(row, 3, new QTableWidgetItem(adQuery.value(2).toString()));
		zagLichModel->setData(zagLichModel->index(row, 4), adQuery.value(3).toBool(), Qt::EditRole );
		ui.tableWidget_zagLich->setItem(row, 5, new QTableWidgetItem(adQuery.value(4).toDate().toString("dd.MM.yyyy")));
	}
	
	connect(ui.tableWidget_zagLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichBudynkiv(int,int)));
}
//------------------------------------------------------------
void tke_form_struct_GV_organiz::updateLichPidrozdiliv(int row, int col)
{
	disconnect(ui.tableWidget_pidrozdLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichAbonentiv(int,int)));
	
	QString testDateStr = ui.tableWidget_pidrozdLich->item(row,5)->text();
	if (testDateStr.size()==0)
		testDateStr="null";
	else
		testDateStr='\''+testDateStr+'\'';
	bool isLich = pidrozdLichModel->data(pidrozdLichModel->index(row,4)).toBool();
	QString lichStr;
	if (isLich)
		lichStr = "true";
	else
		lichStr = "false";
	adQuery.exec("UPDATE lichylnykyAbon SET Rahunok="+ui.tableWidget_pidrozdLich->item(row,1)->text()+", \
									LichNum='"+ui.tableWidget_pidrozdLich->item(row,2)->text()+"', \
									Koef="+ui.tableWidget_pidrozdLich->item(row,3)->text().replace(',','.')+", \
									IsLichylnyk="+lichStr+", \
									DataPr="+testDateStr+" \
					WHERE id="+ui.tableWidget_pidrozdLich->item(row,0)->text());
	adQuery.exec("SELECT Rahunok, LichNum, Koef, IsLichylnyk, DataPr FROM lichylnykyAbon WHERE id="+ui.tableWidget_pidrozdLich->item(row,0)->text());
	if (adQuery.seek(0)){
		ui.tableWidget_pidrozdLich->setItem(row, 1, new QTableWidgetItem(adQuery.value(0).toString()));
		ui.tableWidget_pidrozdLich->setItem(row, 2, new QTableWidgetItem(adQuery.value(1).toString()));
		ui.tableWidget_pidrozdLich->setItem(row, 3, new QTableWidgetItem(adQuery.value(2).toString()));
		pidrozdLichModel->setData(pidrozdLichModel->index(row, 4), adQuery.value(3).toBool(), Qt::EditRole );
		ui.tableWidget_pidrozdLich->setItem(row, 5, new QTableWidgetItem(adQuery.value(4).toDate().toString("dd.MM.yyyy")));
	}
	
	connect(ui.tableWidget_pidrozdLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichAbonentiv(int,int)));
}
//------------------------------------------------------------
void tke_form_struct_GV_organiz::toolButton_reload_clicked()
{
	populateKotelni();
}
//------------------------------------------------------------
void tke_form_struct_GV_organiz::toolButton_addKotelni_clicked()
{
	disconnect(ui.tableWidget_kotelni, SIGNAL(cellChanged(int,int)), this, SLOT(updateKotelni(int,int)));
	
	nextId = nextLichylnyk("kotelni"); 
	if (adQuery.exec("INSERT INTO kotelni (id, Vulycia, Nomer, Nas_punkt, Tel) \
					VALUES ("+QString::number(nextId)+",'вул.',1,'"+OrganizCity()+"','')")){
		int row = ui.tableWidget_kotelni->rowCount();
		ui.tableWidget_kotelni->insertRow(row);
		adQuery.exec("SELECT id, Kot_num, Vulycia, Nomer, Nas_punkt, Tel FROM kotelni WHERE id="+QString::number(nextId));
		adQuery.seek(0);
		ui.tableWidget_kotelni->setItem(row, 0, new QTableWidgetItem(adQuery.value(0).toString()));
		ui.tableWidget_kotelni->setItem(row, 1, new QTableWidgetItem(adQuery.value(1).toString()));
		ui.tableWidget_kotelni->setItem(row, 2, new QTableWidgetItem(adQuery.value(2).toString()));
		ui.tableWidget_kotelni->setItem(row, 3, new QTableWidgetItem(adQuery.value(3).toString()));
		ui.tableWidget_kotelni->setItem(row, 4, new QTableWidgetItem(adQuery.value(4).toString()));
		ui.tableWidget_kotelni->setItem(row, 5, new QTableWidgetItem(adQuery.value(5).toString()));
		
		ui.tableWidget_kotelni->resizeRowToContents(row);
		ui.tableWidget_kotelni->setCurrentCell(row, 1);
		ui.tableWidget_kotelni->editItem(ui.tableWidget_kotelni->item(row, 1));
	}
	
	connect(ui.tableWidget_kotelni, SIGNAL(cellChanged(int,int)), this, SLOT(updateKotelni(int,int)));
}
//------------------------------------------------------------
void tke_form_struct_GV_organiz::toolButton_delKotelni_clicked()
{
	if	( ! QMessageBox::question(
                this,
                "!!! Видалення стрічки !!!",
                "Ви дійсно бажаєте видалити котельню?",
                "Так",
				"Ні-ні-ні",
                QString(), 0, 1)
		){
		disconnect(ui.tableWidget_kotelni, SIGNAL(cellChanged(int,int)), this, SLOT(updateKotelni(int,int)));
		
		if (adQuery.exec("DELETE FROM kotelni \
							WHERE id="+QString::number(ui.tableWidget_kotelni->item(ui.tableWidget_kotelni->currentRow(),0)->text().toInt())))
			ui.tableWidget_kotelni->removeRow(ui.tableWidget_kotelni->currentRow());
		
		connect(ui.tableWidget_kotelni, SIGNAL(cellChanged(int,int)), this, SLOT(updateKotelni(int,int)));
	}
}
//------------------------------------------------------------
void tke_form_struct_GV_organiz::toolButton_addZagLich_clicked()
{
	disconnect(ui.tableWidget_zagLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichBudynkiv(int,int)));
	
	nextId = nextLichylnyk("lichylnykyBudynkovi"); 
	if (adQuery.exec("INSERT INTO lichylnykyBudynkovi (id, LichNum, StartKvNumber, EndKvNumber, Budynok_id) \
					VALUES ("+QString::number(nextId)+",'',1,1,"+QString::number(kotelnyaId)+")")){
		int row = ui.tableWidget_zagLich->rowCount();
		ui.tableWidget_zagLich->insertRow(row);
		adQuery.exec("SELECT id, LichNum, StartKvNumber, EndKvNumber, IsLichylnyk, TestDate FROM lichylnykyBudynkovi \
						WHERE id="+QString::number(nextId)+" \
							and Budynok_id="+QString::number(kotelnyaId));
		adQuery.seek(0);
		ui.tableWidget_zagLich->setItem(row, 0, new QTableWidgetItem(adQuery.value(0).toString()));
		ui.tableWidget_zagLich->setItem(row, 1, new QTableWidgetItem(adQuery.value(1).toString()));
		ui.tableWidget_zagLich->setItem(row, 2, new QTableWidgetItem(adQuery.value(2).toString()));
		ui.tableWidget_zagLich->setItem(row, 3, new QTableWidgetItem(adQuery.value(3).toString()));
		zagLichModel->setData(zagLichModel->index(row, 4), adQuery.value(4).toBool(), Qt::EditRole );
		ui.tableWidget_zagLich->setItem(row, 5, new QTableWidgetItem(adQuery.value(5).toDate().toString("dd.MM.yyyy")));
		
		ui.tableWidget_zagLich->resizeRowToContents(row);
		ui.tableWidget_zagLich->setCurrentCell( row, 1);
		ui.tableWidget_zagLich->editItem(ui.tableWidget_zagLich->item(row, 1));
	}
	
	connect(ui.tableWidget_zagLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichBudynkiv(int,int)));
}
//------------------------------------------------------------
void tke_form_struct_GV_organiz::toolButton_delZagLich_clicked()
{
	if	( ! QMessageBox::question(
                this,
                "!!! Видалення стрічки !!!",
                "Ви дійсно бажаєте видалити будинковий лічильник?",
                "Так",
				"Ні-ні-ні",
                QString(), 0, 1)
		){
		disconnect(ui.tableWidget_zagLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichBudynkiv(int,int)));
		
		if (adQuery.exec("DELETE FROM lichylnykyBudynkovi \
							WHERE id="+QString::number(ui.tableWidget_zagLich->item(ui.tableWidget_zagLich->currentRow(),0)->text().toInt())))
			ui.tableWidget_zagLich->removeRow(ui.tableWidget_zagLich->currentRow());
		
		connect(ui.tableWidget_zagLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichBudynkiv(int,int)));
	}
}
//------------------------------------------------------------
void tke_form_struct_GV_organiz::toolButton_addPidrozdLich_clicked()
{
	disconnect(ui.tableWidget_pidrozdLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichAbonentiv(int,int)));
	
	nextId = nextLichylnyk("lichylnykyAbon"); 
	if (adQuery.exec("INSERT INTO lichylnykyAbon (id, Rahunok, LichNum, Koef, BudLich_ID) \
					VALUES ("+QString::number(nextId)+",1,'',1,"+QString::number(budLichId)+")")){
		int row = ui.tableWidget_pidrozdLich->rowCount();
		ui.tableWidget_pidrozdLich->insertRow(row);
		adQuery.exec("SELECT id, Rahunok, LichNum, Koef, IsLichylnyk, IsLichylnyk FROM lichylnykyAbon \
						WHERE id="+QString::number(nextId)+" \
							and BudLich_ID="+QString::number(budLichId));
		adQuery.seek(0);
		ui.tableWidget_pidrozdLich->setItem(row, 0, new QTableWidgetItem(adQuery.value(0).toString()));
		ui.tableWidget_pidrozdLich->setItem(row, 1, new QTableWidgetItem(adQuery.value(1).toString()));
		ui.tableWidget_pidrozdLich->setItem(row, 2, new QTableWidgetItem(adQuery.value(2).toString()));
		ui.tableWidget_pidrozdLich->setItem(row, 3, new QTableWidgetItem(adQuery.value(3).toString()));
		pidrozdLichModel->setData(pidrozdLichModel->index(row, 4), adQuery.value(4).toBool(), Qt::EditRole );
		ui.tableWidget_pidrozdLich->setItem(row, 5, new QTableWidgetItem(adQuery.value(5).toDate().toString("dd.MM.yyyy")));
		
		ui.tableWidget_pidrozdLich->resizeRowToContents(row);
		ui.tableWidget_pidrozdLich->setCurrentCell( row, 1);
		ui.tableWidget_pidrozdLich->editItem(ui.tableWidget_pidrozdLich->item(row, 1));
	}
	
	connect(ui.tableWidget_pidrozdLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichAbonentiv(int,int)));
}
//------------------------------------------------------------
void tke_form_struct_GV_organiz::toolButton_delPidrozdLich_clicked()
{
	if	( ! QMessageBox::question(
                this,
                "!!! Видалення стрічки !!!",
                "Ви дійсно бажаєте видалити абонентський лічильник?",
                "Так",
				"Ні-ні-ні",
                QString(), 0, 1)
		){
		disconnect(ui.tableWidget_pidrozdLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichAbonentiv(int,int)));
		if (adQuery.exec("DELETE FROM lichylnykyAbon \
							WHERE id="+QString::number(ui.tableWidget_pidrozdLich->item(ui.tableWidget_pidrozdLich->currentRow(),0)->text().toInt())))
			ui.tableWidget_pidrozdLich->removeRow(ui.tableWidget_pidrozdLich->currentRow());
		connect(ui.tableWidget_pidrozdLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichAbonentiv(int,int)));
	}
}
//------------------------------------------------------------
tke_form_struct_GV_organiz::~tke_form_struct_GV_organiz()
{
	
}
//------------------------------------------------------------
