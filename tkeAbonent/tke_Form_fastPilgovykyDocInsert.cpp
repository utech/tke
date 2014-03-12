//------------------------------------------------------------
//
// Implamantation of tke_Form_fastPilgovykyDocInsert class.
// Created by Roman Mulyk 26.01.2009
//
//------------------------------------------------------------

#include "UDbScriptAccess.h"
#include <USqlAccessible>
#include "tke_Form_fastPilgovykyDocInsert.h"

//------------------------------------------------------------
tke_Form_fastPilgovykyDocInsert::tke_Form_fastPilgovykyDocInsert( QWidget * parent) 
	: QWidget(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	
	ui.tableWidget_pilgovykyDocs->setColumnHidden(0, true);
	
	QSqlQuery query("SELECT id, Kategoriya FROM pilgy ORDER BY Kategoriya");
	ui.comboBox_pilgKategorFiltr->addItem("Усі пільги", 0);
	while (query.next())
		ui.comboBox_pilgKategorFiltr->addItem(query.value(1).toString()+" - "+query.value(0).toString(), query.value(0).toInt());
	populatePilgovykyTable();
	
	ui.tableWidget_pilgovykyDocs->setColumnWidth( 1, 55 );
	ui.tableWidget_pilgovykyDocs->setColumnWidth( 2, 50 );
	ui.tableWidget_pilgovykyDocs->setColumnWidth( 3, 130 );
	ui.tableWidget_pilgovykyDocs->setColumnWidth( 4, 180 );
	ui.tableWidget_pilgovykyDocs->setColumnWidth( 5, 200 );
	//ui.tableWidget_pilgovykyDocs->setColumnWidth( 6, 200 );
	ui.tableWidget_pilgovykyDocs->setItemDelegateForColumn(1, new UCheckBoxDelegate(ui.tableWidget_pilgovykyDocs,"Так", "-", "здано"));
	
	connect(ui.comboBox_pilgKategorFiltr,SIGNAL(currentIndexChanged(int)),this,SLOT(populatePilgovykyTable()));
}
//------------------------------------------------------------
void tke_Form_fastPilgovykyDocInsert::populatePilgovykyTable()
{
	disconnect(ui.tableWidget_pilgovykyDocs,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(saveItem(QTableWidgetItem *)));
	
	QString filtrStr;
	int curPilgaId = ui.comboBox_pilgKategorFiltr->itemData(ui.comboBox_pilgKategorFiltr->currentIndex(),Qt::UserRole).toInt();
	if (curPilgaId > 0)
		filtrStr = " WHERE abonent.Pilga_id="+sqlStr(curPilgaId)+" or abonent.Pilga_id_2="+sqlStr(curPilgaId)+" ";
	else
		filtrStr = " WHERE abonent.Pilga_id>0 or abonent.Pilga_id_2>0 ";
	QSqlQuery query;
	query.exec("SELECT count(*) \
					FROM ((abonent \
					LEFT JOIN (SELECT * FROM pilgy) AS pilgy1 ON pilgy1.id=abonent.Pilga_id) \
					LEFT JOIN (SELECT * FROM pilgy) AS pilgy2 ON pilgy2.id=abonent.Pilga_id_2) \
					LEFT JOIN budynky ON abonent.Budynok_ID = budynky.id \
					"+filtrStr);
	query.seek(0);
	int rowCount = query.value(0).toInt();
	ui.tableWidget_pilgovykyDocs->setRowCount(rowCount);
	query.exec("SELECT abonent.id, abonent.Rahunok, abonent.Prizv, abonent.Imya, abonent.Batk, \
							budynky.Vulycia, budynky.Bud_num, abonent.Kvartyra, abonent.Pilga_id, \
							pilgy1.Kategoriya, abonent.Pilga_for, abonent.Pilga_id_2, \
							pilgy2.Kategoriya, abonent.Pilga_for_2, abonent.Pilgy_zdani \
					FROM ((abonent \
					LEFT JOIN (SELECT * FROM pilgy) AS pilgy1 ON pilgy1.id=abonent.Pilga_id) \
					LEFT JOIN (SELECT * FROM pilgy) AS pilgy2 ON pilgy2.id=abonent.Pilga_id_2) \
					LEFT JOIN budynky ON abonent.Budynok_ID = budynky.id \
					"+filtrStr);
	QColor redColor("#ffc1b9");
	for (int row=0; (row<rowCount)&&query.next(); row++){
		QTableWidgetItem * item;
		item = new QTableWidgetItem(query.value(0).toString());
		item->setFlags(item->flags() ^ Qt::ItemIsEditable);
		ui.tableWidget_pilgovykyDocs->setItem(row, 0, item);
		
		item = new QTableWidgetItem();
		item->setData(Qt::EditRole, query.value(14).toBool());
		ui.tableWidget_pilgovykyDocs->setItem(row, 1, item);
		
		item = new QTableWidgetItem();
		item->setData(Qt::EditRole, query.value(1).toInt());
		item->setFlags(item->flags() ^ Qt::ItemIsEditable);
		item->setBackgroundColor( redColor );
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		ui.tableWidget_pilgovykyDocs->setItem(row, 2, item);
		
		item = new QTableWidgetItem(query.value(2).toString());
		item->setFlags(item->flags() ^ Qt::ItemIsEditable);
		item->setBackgroundColor( redColor );
		ui.tableWidget_pilgovykyDocs->setItem(row, 3, item);
		item = new QTableWidgetItem(query.value(3).toString()+" "+query.value(4).toString());
		item->setFlags(item->flags() ^ Qt::ItemIsEditable);
		item->setBackgroundColor( redColor );
		ui.tableWidget_pilgovykyDocs->setItem(row, 4, item);
		item = new QTableWidgetItem(query.value(5).toString()+" "+query.value(6).toString()+(query.value(7).toInt()>0 ? " кв."+query.value(8).toString() : ""));
		item->setFlags(item->flags() ^ Qt::ItemIsEditable);
		item->setBackgroundColor( redColor );
		ui.tableWidget_pilgovykyDocs->setItem(row, 5, item);
		item = new QTableWidgetItem( "1) "+(query.value(8).toInt()>0 ? query.value(9).toString()+" на "+query.value(10).toString()+"чол." : " немає") +"\n"+
									"2) "+(query.value(11).toInt()>0 ? query.value(12).toString()+" на "+query.value(13).toString()+"чол." : " немає")
									);
		item->setFlags(item->flags() ^ Qt::ItemIsEditable);
		item->setBackgroundColor( redColor );
		ui.tableWidget_pilgovykyDocs->setItem(row, 6, item);
	}
	
	ui.tableWidget_pilgovykyDocs->resizeColumnToContents(6);
	ui.label_pilgovykyCount->setText("Всього абонентів-пільговиків: "+QString::number(rowCount)+".");
	
	connect(ui.tableWidget_pilgovykyDocs,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(saveItem(QTableWidgetItem *)));
}
//------------------------------------------------------------
void tke_Form_fastPilgovykyDocInsert::saveItem(QTableWidgetItem *item)
{
	if (item->column() == 1){
		disconnect(ui.tableWidget_pilgovykyDocs,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(saveItem(QTableWidgetItem *)));
		QSqlQuery query;
		query.exec("UPDATE abonent SET Pilgy_zdani="+sqlStr(item->data(Qt::EditRole).toBool())+" WHERE id="+sqlStr(ui.tableWidget_pilgovykyDocs->item(item->row(),0)->data(Qt::EditRole).toInt()));
		qDebug() << query.lastQuery();
		query.exec("SELECT Pilgy_zdani FROM abonent WHERE id="+sqlStr(ui.tableWidget_pilgovykyDocs->item(item->row(),0)->data(Qt::EditRole).toInt()));
		qDebug() << query.lastQuery();
		query.seek(0);
		item->setData(Qt::EditRole, query.value(0).toBool());
		connect(ui.tableWidget_pilgovykyDocs,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(saveItem(QTableWidgetItem *)));
	}
}
//------------------------------------------------------------
tke_Form_fastPilgovykyDocInsert::~tke_Form_fastPilgovykyDocInsert()
{
	
}
//------------------------------------------------------------
