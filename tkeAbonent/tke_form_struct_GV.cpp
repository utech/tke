//------------------------------------------------------------
//
// Implamantation of tke_form_struct_GV class.
// Created by Roman Mulyk 16.06.2008
//
//------------------------------------------------------------

#include <UStandardDelegate>
#include "tke_form_struct_GV.h"
#include <USqlAccessible>

//------------------------------------------------------------
tke_form_struct_GV::tke_form_struct_GV( QWidget * parent) 
	: QWidget(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	
	ui.tableWidget_kotelni->setColumnHidden(0,true);
	ui.tableWidget_budynky->setColumnHidden(0,true);
	ui.tableWidget_budLich->setColumnHidden(0,true);
	ui.tableWidget_abonLich->setColumnHidden(0,true);
	
	ui.tableWidget_budLich->setItemDelegateForColumn(4, new UBoolComboDelegate(ui.tableWidget_budLich));
	ui.tableWidget_abonLich->setItemDelegateForColumn(4, new UBoolComboDelegate(ui.tableWidget_abonLich));
	
	budLichModel = ui.tableWidget_budLich->model();
	abonLichModel = ui.tableWidget_abonLich->model();
	
	populateKotelni();
	 
	connect(ui.tableWidget_kotelni, SIGNAL(cellChanged(int,int)), this, SLOT(updateKotelni(int,int)));
	connect(ui.tableWidget_budynky, SIGNAL(cellChanged(int,int)), this, SLOT(updateBudynky(int,int)));
	connect(ui.tableWidget_budLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichBudynkiv(int,int)));
	connect(ui.tableWidget_abonLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichAbonentiv(int,int)));
	
	connect(ui.toolButton_addKotelni, SIGNAL(clicked()), this, SLOT(toolButton_addKotelni_clicked()));
	connect(ui.toolButton_delKotelni, SIGNAL(clicked()), this, SLOT(toolButton_delKotelni_clicked()));
	
	connect(ui.toolButton_addBudynky, SIGNAL(clicked()), this, SLOT(toolButton_addBudynky_clicked()));
	connect(ui.toolButton_delBudynky, SIGNAL(clicked()), this, SLOT(toolButton_delBudynky_clicked()));
	
	connect(ui.toolButton_addBudLich, SIGNAL(clicked()), this, SLOT(toolButton_addBudLich_clicked()));
	connect(ui.toolButton_delBudLich, SIGNAL(clicked()), this, SLOT(toolButton_delBudLich_clicked()));
	
	connect(ui.toolButton_addAbonLich, SIGNAL(clicked()), this, SLOT(toolButton_addAbonLich_clicked()));
	connect(ui.toolButton_delAbonLich, SIGNAL(clicked()), this, SLOT(toolButton_delAbonLich_clicked()));
	
	connect(ui.toolButton_reload, SIGNAL(clicked()), this, SLOT(toolButton_reload_clicked()));
	connect(ui.toolButton_closeForm, SIGNAL(clicked()), this, SLOT(close()));
}
//------------------------------------------------------------
void tke_form_struct_GV::populateKotelni()
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
	
	if (rowCount==0){
		ui.toolButton_addBudynky->setEnabled(false);
		ui.toolButton_delBudynky->setEnabled(false);
	}
	else{
		ui.toolButton_addBudynky->setEnabled(true);
		ui.toolButton_delBudynky->setEnabled(true);
	}
	
	populateBudynky();
	
	connect(ui.tableWidget_kotelni, SIGNAL(cellChanged(int,int)), this, SLOT(updateKotelni(int,int)));
	connect(ui.tableWidget_kotelni->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &,const QModelIndex &)), this, SLOT(populateBudynky()));
}
//------------------------------------------------------------
void tke_form_struct_GV::populateBudynky()
{
	disconnect(ui.tableWidget_budynky->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &,const QModelIndex &)), this, SLOT(populateLichBudynkiv()));
	disconnect(ui.tableWidget_budynky, SIGNAL(cellChanged(int,int)), this, SLOT(updateBudynky(int,int)));
	
	int rowCount=0;
	int oldId=1;
	if (ui.tableWidget_budynky->rowCount()>0)
		oldId = ui.tableWidget_budynky->item(ui.tableWidget_budynky->currentRow(),0)->text().toInt();
	if (ui.tableWidget_kotelni->rowCount()>0)
		kotelnyaId = ui.tableWidget_kotelni->item(ui.tableWidget_kotelni->currentRow(),0)->text().toInt();
	else kotelnyaId = 0;
	
	if (kotelnyaId > 0){
		QSqlQuery *query = new QSqlQuery();
		query->exec("SELECT count(*) FROM budynky WHERE Kotelnya_ID="+QString::number(kotelnyaId));
		query->seek(0);
		rowCount = query->value(0).toInt();
		ui.tableWidget_budynky->setRowCount(rowCount);
		query->exec("SELECT id, Vulycia, Bud_num FROM budynky WHERE Kotelnya_ID="+QString::number(kotelnyaId));
		bool isSelected=false;
		for (int row=0; (row<rowCount) && query->next(); ++row){
			ui.tableWidget_budynky->setItem(row, 0, new QTableWidgetItem(query->value(0).toString()));
			ui.tableWidget_budynky->setItem(row, 1, new QTableWidgetItem(query->value(1).toString()));
			ui.tableWidget_budynky->setItem(row, 2, new QTableWidgetItem(query->value(2).toString()));
		
			if (query->value(0).toInt() == oldId){
				isSelected = true;
				ui.tableWidget_budynky->setCurrentCell( row, 1);
			}
		}
		if (!isSelected && (ui.tableWidget_budynky->rowCount()>0))
			ui.tableWidget_budynky->setCurrentCell( 0, 1);
		ui.tableWidget_budynky->resizeColumnsToContents();
		ui.tableWidget_budynky->resizeRowsToContents();
		delete query;
	}
	else
		ui.tableWidget_budynky->setRowCount(0);
	
	if (rowCount==0){
		ui.toolButton_addBudLich->setEnabled(false);
		ui.toolButton_delBudLich->setEnabled(false);
	}
	else{
		ui.toolButton_addBudLich->setEnabled(true);
		ui.toolButton_delBudLich->setEnabled(true);
	}
	
	ui.label_countBudynky->setText("Всього: "+QString::number(rowCount)+".");
	
	populateLichBudynkiv();

	connect(ui.tableWidget_budynky, SIGNAL(cellChanged(int,int)), this, SLOT(updateBudynky(int,int)));
	connect(ui.tableWidget_budynky->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &,const QModelIndex &)), this, SLOT(populateLichBudynkiv()));
}
//------------------------------------------------------------
void tke_form_struct_GV::populateLichBudynkiv()
{
	disconnect(ui.tableWidget_budLich->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &,const QModelIndex &)), this, SLOT(populateLichAbonentiv()));
	disconnect(ui.tableWidget_budLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichBudynkiv(int,int)));
	
	int rowCount=0;
	int oldId=1;
	
	if (ui.tableWidget_budLich->rowCount()>0)
		oldId = ui.tableWidget_budLich->item(ui.tableWidget_budLich->currentRow(),0)->text().toInt();
	if (ui.tableWidget_budynky->rowCount()>0)
		budynokId = ui.tableWidget_budynky->item(ui.tableWidget_budynky->currentRow(),0)->text().toInt();
	else
		budynokId = 0;
	if (budynokId > 0){
		QSqlQuery *query = new QSqlQuery();
		
		query->exec("SELECT count(*) FROM lichylnykyBudynkovi WHERE Budynok_id="+QString::number(budynokId));
		query->seek(0);
		rowCount = query->value(0).toInt();
		ui.tableWidget_budLich->setRowCount(rowCount);
		query->exec("SELECT id, LichNum, StartKvNumber, EndKvNumber, IsLichylnyk, TestDate FROM lichylnykyBudynkovi WHERE Budynok_id="+QString::number(budynokId));
		bool isSelected=false;
		for (int row=0; (row<rowCount) && query->next(); ++row){
			ui.tableWidget_budLich->setItem(row, 0, new QTableWidgetItem(query->value(0).toString()));
			ui.tableWidget_budLich->setItem(row, 1, new QTableWidgetItem(query->value(1).toString()));
			ui.tableWidget_budLich->setItem(row, 2, new QTableWidgetItem(query->value(2).toString()));
			ui.tableWidget_budLich->setItem(row, 3, new QTableWidgetItem(query->value(3).toString()));
			budLichModel->setData(budLichModel->index(row, 4), query->value(4).toBool(), Qt::EditRole );
			ui.tableWidget_budLich->setItem(row, 5, new QTableWidgetItem(query->value(5).toDate().toString("dd.MM.yyyy")));
		
			if (query->value(0).toInt() == oldId){
				isSelected = true;
				ui.tableWidget_budLich->setCurrentCell( row, 1);
			}
		}
		if (!isSelected && (ui.tableWidget_budLich->rowCount()>0))
			ui.tableWidget_budLich->setCurrentCell( 0, 1);
		ui.tableWidget_budLich->resizeColumnsToContents();
		ui.tableWidget_budLich->resizeRowsToContents();
		delete query;
	}
	else
		ui.tableWidget_budLich->setRowCount(0);
	
	if (rowCount==0){
		ui.toolButton_addAbonLich->setEnabled(false);
		ui.toolButton_delAbonLich->setEnabled(false);
	}
	else{
		ui.toolButton_addAbonLich->setEnabled(true);
		ui.toolButton_delAbonLich->setEnabled(true);
	}
	
	ui.label_countBudLich->setText("Всього: "+QString::number(rowCount)+".");
	
	populateLichAbonentiv();
	
	connect(ui.tableWidget_budLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichBudynkiv(int,int)));
	connect(ui.tableWidget_budLich->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &,const QModelIndex &)), this, SLOT(populateLichAbonentiv()));
}
//------------------------------------------------------------
void tke_form_struct_GV::populateLichAbonentiv()
{
	disconnect(ui.tableWidget_abonLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichAbonentiv(int,int)));
	
	int rowCount=0;
	int oldId=1;
	if (ui.tableWidget_abonLich->rowCount()>0)
		oldId = ui.tableWidget_abonLich->item(ui.tableWidget_abonLich->currentRow(),0)->text().toInt();
	if (ui.tableWidget_budLich->rowCount()>0)
		budLichId = ui.tableWidget_budLich->item(ui.tableWidget_budLich->currentRow(),0)->text().toInt();
	else budLichId = 0;
	if (budLichId > 0){
		QSqlQuery *query = new QSqlQuery();
		query->exec("SELECT count(*) FROM lichylnykyAbon WHERE BudLich_ID="+QString::number(budLichId));
		query->seek(0);
		rowCount = query->value(0).toInt();
		ui.tableWidget_abonLich->setRowCount(rowCount);
		query->exec("SELECT id, Rahunok, LichNum, Koef, IsLichylnyk, DataPr FROM lichylnykyAbon WHERE BudLich_ID="+QString::number(budLichId)+" ORDER BY Rahunok");
		bool isSelected=false;
		for (int row=0; (row<rowCount) && query->next(); ++row){
			ui.tableWidget_abonLich->setItem(row, 0, new QTableWidgetItem(query->value(0).toString()));
			ui.tableWidget_abonLich->setItem(row, 1, new QTableWidgetItem(query->value(1).toString()));
			ui.tableWidget_abonLich->setItem(row, 2, new QTableWidgetItem(query->value(2).toString()));
			ui.tableWidget_abonLich->setItem(row, 3, new QTableWidgetItem(query->value(3).toString()));
			abonLichModel->setData(abonLichModel->index(row, 4), query->value(4).toBool(), Qt::EditRole );
			ui.tableWidget_abonLich->setItem(row, 5, new QTableWidgetItem(query->value(5).toDate().toString("dd.MM.yyyy")));
			if (query->value(0).toInt() == oldId){
				isSelected = true;
				ui.tableWidget_abonLich->setCurrentCell( row, 1);
			}
		}
		if (!isSelected && (ui.tableWidget_abonLich->rowCount()>0))
			ui.tableWidget_abonLich->setCurrentCell( 0, 1);
		delete query;
	}
	else
		ui.tableWidget_abonLich->setRowCount(0);
	
	ui.label_countAbonLich->setText("Всього: "+QString::number(rowCount)+".");
	
	ui.tableWidget_abonLich->resizeColumnsToContents();
	ui.tableWidget_abonLich->resizeRowsToContents();
	
	connect(ui.tableWidget_abonLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichAbonentiv(int,int)));
}
//------------------------------------------------------------
void tke_form_struct_GV::updateKotelni(int row, int col)
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
void tke_form_struct_GV::updateBudynky(int row, int col)
{
	disconnect(ui.tableWidget_budynky, SIGNAL(cellChanged(int,int)), this, SLOT(updateBudynky(int,int)));
	
	adQuery.exec("UPDATE budynky SET Vulycia='"+ui.tableWidget_budynky->item(row,1)->text()+"', \
									Bud_num='"+ui.tableWidget_budynky->item(row,2)->text()+"' \
					WHERE id="+ui.tableWidget_budynky->item(row,0)->text());
	adQuery.exec("SELECT Vulycia, Bud_num FROM budynky WHERE id="+ui.tableWidget_budynky->item(row,0)->text());
	if (adQuery.seek(0)){
		ui.tableWidget_budynky->setItem(row, 1, new QTableWidgetItem(adQuery.value(0).toString()));
		ui.tableWidget_budynky->setItem(row, 2, new QTableWidgetItem(adQuery.value(1).toString()));
	}
	
	connect(ui.tableWidget_budynky, SIGNAL(cellChanged(int,int)), this, SLOT(updateBudynky(int,int)));
}
//------------------------------------------------------------
void tke_form_struct_GV::updateLichBudynkiv(int row, int col)
{
	disconnect(ui.tableWidget_budLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichBudynkiv(int,int)));

	QString testDateStr = ui.tableWidget_budLich->item(row,5)->text();
	if (testDateStr.size()==0)
		testDateStr="null";
	else
		testDateStr='\''+testDateStr+'\'';
	bool isLich = budLichModel->data(budLichModel->index(row,4)).toBool();
	QString lichStr;
	if (isLich)
		lichStr = "true";
	else
		lichStr = "false";
	adQuery.exec("UPDATE lichylnykyBudynkovi SET LichNum='"+ui.tableWidget_budLich->item(row,1)->text()+"', \
									StartKvNumber="+ui.tableWidget_budLich->item(row,2)->text()+", \
									EndKvNumber="+ui.tableWidget_budLich->item(row,3)->text()+", \
									IsLichylnyk="+lichStr+", \
									TestDate="+testDateStr+" \
					WHERE id="+ui.tableWidget_budLich->item(row,0)->text());
	adQuery.exec("SELECT LichNum, StartKvNumber, EndKvNumber, IsLichylnyk, TestDate FROM lichylnykyBudynkovi WHERE id="+ui.tableWidget_budLich->item(row,0)->text());
	if (adQuery.seek(0)){
		ui.tableWidget_budLich->setItem(row, 1, new QTableWidgetItem(adQuery.value(0).toString()));
		ui.tableWidget_budLich->setItem(row, 2, new QTableWidgetItem(adQuery.value(1).toString()));
		ui.tableWidget_budLich->setItem(row, 3, new QTableWidgetItem(adQuery.value(2).toString()));
		budLichModel->setData(budLichModel->index(row, 4), adQuery.value(3).toBool(), Qt::EditRole );
		ui.tableWidget_budLich->setItem(row, 5, new QTableWidgetItem(adQuery.value(4).toDate().toString("dd.MM.yyyy")));
	}
	
	connect(ui.tableWidget_budLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichBudynkiv(int,int)));
}
//------------------------------------------------------------
void tke_form_struct_GV::updateLichAbonentiv(int row, int col)
{
	disconnect(ui.tableWidget_abonLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichAbonentiv(int,int)));
	
	QString testDateStr = ui.tableWidget_abonLich->item(row,5)->text();
	if (testDateStr.size()==0)
		testDateStr="null";
	else
		testDateStr='\''+testDateStr+'\'';
	bool isLich = abonLichModel->data(abonLichModel->index(row,4)).toBool();
	QString lichStr;
	if (isLich)
		lichStr = "true";
	else
		lichStr = "false";
	adQuery.exec("UPDATE lichylnykyAbon SET Rahunok="+ui.tableWidget_abonLich->item(row,1)->text()+", \
									LichNum='"+ui.tableWidget_abonLich->item(row,2)->text()+"', \
									Koef="+ui.tableWidget_abonLich->item(row,3)->text().replace(',','.')+", \
									IsLichylnyk="+lichStr+", \
									DataPr="+testDateStr+" \
					WHERE id="+ui.tableWidget_abonLich->item(row,0)->text());
	adQuery.exec("SELECT Rahunok, LichNum, Koef, IsLichylnyk, DataPr FROM lichylnykyAbon WHERE id="+ui.tableWidget_abonLich->item(row,0)->text());
	if (adQuery.seek(0)){
		ui.tableWidget_abonLich->setItem(row, 1, new QTableWidgetItem(adQuery.value(0).toString()));
		ui.tableWidget_abonLich->setItem(row, 2, new QTableWidgetItem(adQuery.value(1).toString()));
		ui.tableWidget_abonLich->setItem(row, 3, new QTableWidgetItem(adQuery.value(2).toString()));
		abonLichModel->setData(abonLichModel->index(row, 4), adQuery.value(3).toBool(), Qt::EditRole );
		ui.tableWidget_abonLich->setItem(row, 5, new QTableWidgetItem(adQuery.value(4).toDate().toString("dd.MM.yyyy")));
	}
	
	connect(ui.tableWidget_abonLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichAbonentiv(int,int)));
}
//------------------------------------------------------------
void tke_form_struct_GV::toolButton_reload_clicked()
{
	populateKotelni();
}
//------------------------------------------------------------
void tke_form_struct_GV::toolButton_addKotelni_clicked()
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
void tke_form_struct_GV::toolButton_delKotelni_clicked()
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
void tke_form_struct_GV::toolButton_addBudynky_clicked()
{
	disconnect(ui.tableWidget_budynky, SIGNAL(cellChanged(int,int)), this, SLOT(updateBudynky(int,int)));
	
	nextId = nextLichylnyk("budynky"); 
	if (adQuery.exec("INSERT INTO budynky (id, Vulycia, Bud_num, Kotelnya_ID) \
					VALUES ("+QString::number(nextId)+",'вул.','1',"+QString::number(kotelnyaId)+")")){
		int row = ui.tableWidget_budynky->rowCount();
		ui.tableWidget_budynky->insertRow(row);
		adQuery.exec("SELECT id, Vulycia, Bud_num FROM budynky \
						WHERE id="+QString::number(nextId)+" \
							and Kotelnya_ID="+QString::number(kotelnyaId));
		adQuery.seek(0);
		ui.tableWidget_budynky->setItem(row, 0, new QTableWidgetItem(adQuery.value(0).toString()));
		ui.tableWidget_budynky->setItem(row, 1, new QTableWidgetItem(adQuery.value(1).toString()));
		ui.tableWidget_budynky->setItem(row, 2, new QTableWidgetItem(adQuery.value(2).toString()));
		
		ui.tableWidget_budynky->resizeRowToContents(row);
		ui.tableWidget_budynky->setCurrentCell( row, 1);
		ui.tableWidget_budynky->editItem(ui.tableWidget_budynky->item(row, 1));
	}
	
	connect(ui.tableWidget_budynky, SIGNAL(cellChanged(int,int)), this, SLOT(updateBudynky(int,int)));
}
//------------------------------------------------------------
void tke_form_struct_GV::toolButton_delBudynky_clicked()
{
	if	( ! QMessageBox::question(
                this,
                "!!! Видалення стрічки !!!",
                "Ви дійсно бажаєте видалити будинок?",
                "Так",
				"Ні-ні-ні",
                QString(), 0, 1)
		){
		disconnect(ui.tableWidget_budynky, SIGNAL(cellChanged(int,int)), this, SLOT(updateBudynky(int,int)));
		
		if (adQuery.exec("DELETE FROM budynky \
							WHERE id="+QString::number(ui.tableWidget_budynky->item(ui.tableWidget_budynky->currentRow(),0)->text().toInt())))
			ui.tableWidget_budynky->removeRow(ui.tableWidget_budynky->currentRow());
		
		connect(ui.tableWidget_budynky, SIGNAL(cellChanged(int,int)), this, SLOT(updateBudynky(int,int)));
	}
}
//------------------------------------------------------------
void tke_form_struct_GV::toolButton_addBudLich_clicked()
{
	disconnect(ui.tableWidget_budLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichBudynkiv(int,int)));
	
	nextId = nextLichylnyk("lichylnykyBudynkovi"); 
	if (adQuery.exec("INSERT INTO lichylnykyBudynkovi (id, LichNum, StartKvNumber, EndKvNumber, Budynok_id) \
					VALUES ("+QString::number(nextId)+",'',1,1,"+QString::number(budynokId)+")")){
		int row = ui.tableWidget_budLich->rowCount();
		ui.tableWidget_budLich->insertRow(row);
		adQuery.exec("SELECT id, LichNum, StartKvNumber, EndKvNumber, IsLichylnyk, TestDate FROM lichylnykyBudynkovi \
						WHERE id="+QString::number(nextId)+" \
							and Budynok_id="+QString::number(budynokId));
		adQuery.seek(0);
		ui.tableWidget_budLich->setItem(row, 0, new QTableWidgetItem(adQuery.value(0).toString()));
		ui.tableWidget_budLich->setItem(row, 1, new QTableWidgetItem(adQuery.value(1).toString()));
		ui.tableWidget_budLich->setItem(row, 2, new QTableWidgetItem(adQuery.value(2).toString()));
		ui.tableWidget_budLich->setItem(row, 3, new QTableWidgetItem(adQuery.value(3).toString()));
		budLichModel->setData(budLichModel->index(row, 4), adQuery.value(4).toBool(), Qt::EditRole );
		ui.tableWidget_budLich->setItem(row, 5, new QTableWidgetItem(adQuery.value(5).toDate().toString("dd.MM.yyyy")));
		
		ui.tableWidget_budLich->resizeRowToContents(row);
		ui.tableWidget_budLich->setCurrentCell( row, 1);
		ui.tableWidget_budLich->editItem(ui.tableWidget_budLich->item(row, 1));
	}
	
	connect(ui.tableWidget_budLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichBudynkiv(int,int)));
}
//------------------------------------------------------------
void tke_form_struct_GV::toolButton_delBudLich_clicked()
{
	if	( ! QMessageBox::question(
                this,
                "!!! Видалення стрічки !!!",
                "Ви дійсно бажаєте видалити будинковий лічильник?",
                "Так",
				"Ні-ні-ні",
                QString(), 0, 1)
		){
		disconnect(ui.tableWidget_budLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichBudynkiv(int,int)));
		
		if (adQuery.exec("DELETE FROM lichylnykyBudynkovi \
							WHERE id="+QString::number(ui.tableWidget_budLich->item(ui.tableWidget_budLich->currentRow(),0)->text().toInt())))
			ui.tableWidget_budLich->removeRow(ui.tableWidget_budLich->currentRow());
		
		connect(ui.tableWidget_budLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichBudynkiv(int,int)));
	}
}
//------------------------------------------------------------
void tke_form_struct_GV::toolButton_addAbonLich_clicked()
{
	disconnect(ui.tableWidget_abonLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichAbonentiv(int,int)));
	
	nextId = nextLichylnyk("lichylnykyAbon"); 
	if (adQuery.exec("INSERT INTO lichylnykyAbon (id, Rahunok, LichNum, Koef, BudLich_ID) \
					VALUES ("+QString::number(nextId)+",1,'',1,"+QString::number(budLichId)+")")){
		int row = ui.tableWidget_abonLich->rowCount();
		ui.tableWidget_abonLich->insertRow(row);
		adQuery.exec("SELECT id, Rahunok, LichNum, Koef, IsLichylnyk, IsLichylnyk FROM lichylnykyAbon \
						WHERE id="+QString::number(nextId)+" \
							and BudLich_ID="+QString::number(budLichId));
		adQuery.seek(0);
		ui.tableWidget_abonLich->setItem(row, 0, new QTableWidgetItem(adQuery.value(0).toString()));
		ui.tableWidget_abonLich->setItem(row, 1, new QTableWidgetItem(adQuery.value(1).toString()));
		ui.tableWidget_abonLich->setItem(row, 2, new QTableWidgetItem(adQuery.value(2).toString()));
		ui.tableWidget_abonLich->setItem(row, 3, new QTableWidgetItem(adQuery.value(3).toString()));
		abonLichModel->setData(abonLichModel->index(row, 4), adQuery.value(4).toBool(), Qt::EditRole );
		ui.tableWidget_abonLich->setItem(row, 5, new QTableWidgetItem(adQuery.value(5).toDate().toString("dd.MM.yyyy")));
		
		ui.tableWidget_abonLich->resizeRowToContents(row);
		ui.tableWidget_abonLich->setCurrentCell( row, 1);
		ui.tableWidget_abonLich->editItem(ui.tableWidget_abonLich->item(row, 1));
	}
	
	connect(ui.tableWidget_abonLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichAbonentiv(int,int)));
}
//------------------------------------------------------------
void tke_form_struct_GV::toolButton_delAbonLich_clicked()
{
	if	( ! QMessageBox::question(
                this,
                "!!! Видалення стрічки !!!",
                "Ви дійсно бажаєте видалити абонентський лічильник?",
                "Так",
				"Ні-ні-ні",
                QString(), 0, 1)
		){
		disconnect(ui.tableWidget_abonLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichAbonentiv(int,int)));
		if (adQuery.exec("DELETE FROM lichylnykyAbon \
							WHERE id="+QString::number(ui.tableWidget_abonLich->item(ui.tableWidget_abonLich->currentRow(),0)->text().toInt())))
			ui.tableWidget_abonLich->removeRow(ui.tableWidget_abonLich->currentRow());
		connect(ui.tableWidget_abonLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichAbonentiv(int,int)));
	}
}
//------------------------------------------------------------
tke_form_struct_GV::~tke_form_struct_GV()
{
	
}
//------------------------------------------------------------
