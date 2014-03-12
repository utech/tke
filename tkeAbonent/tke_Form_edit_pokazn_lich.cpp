//------------------------------------------------------------
//
// Implamantation of tke_Form_edit_pokazn_lich class.
// Created by Roman Mulyk 10.11.2008
//
//------------------------------------------------------------

#include "tke_Form_edit_pokazn_lich.h"
#include <UPostupForm>
#include <USqlAccessible>

//------------------------------------------------------------
tke_Form_edit_pokazn_lich::tke_Form_edit_pokazn_lich( QWidget * parent, short year, short month ) 
	: QWidget(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	
	ui.spinBox_year->setValue(year);
	ui.spinBox_month->setValue(month);
	
	loadPokazn();
	
	connect(ui.toolButton_closeForm,SIGNAL(clicked()),this,SLOT(close()));
	connect(ui.toolButton_reload,SIGNAL(clicked()),this,SLOT(loadPokazn()));
	connect(ui.spinBox_year,SIGNAL(valueChanged(int)),this,SLOT(loadPokazn()));
	connect(ui.spinBox_month,SIGNAL(valueChanged(int)),this,SLOT(loadPokazn()));
	connect(ui.toolButton_loadFromPrevMonth,SIGNAL(clicked()),this,SLOT(loadPokaznFromPrevMonth()));
}
//------------------------------------------------------------
void tke_Form_edit_pokazn_lich::loadPokazn()
{
	disconnect(ui.tableWidget,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(saveItemValue(QTableWidgetItem *)));
	disconnect(ui.tableWidget->selectionModel(),SIGNAL(currentRowChanged(const QModelIndex &,const QModelIndex &)),this,SLOT(tableWidgetSelectionChanged()));
	
	int curRahunok;
	int curRow;
	if (ui.tableWidget->rowCount() > 0){
		curRahunok = ui.tableWidget->item(ui.tableWidget->currentRow(),0)->data(Qt::DisplayRole).toInt();
		curRow = ui.tableWidget->currentRow();
	}
	else{
		curRahunok = 0;
		curRow = 0;
	}
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT count(*) FROM (SELECT pok1OpalStoyak.Rahunok_ID, pok1OpalStoyak.Plos, pok1OpalStoyak.GKal, abonent.NarahStoyak \
				FROM (SELECT * FROM pokaznOpalStoyak WHERE (pokaznOpalStoyak.Year="+ui.spinBox_year->text()+") \
						AND (pokaznOpalStoyak.Month="+ui.spinBox_month->text()+") ) AS pok1OpalStoyak \
				LEFT JOIN abonent ON pok1OpalStoyak.Rahunok_ID = abonent.Rahunok \
				UNION \
				SELECT abonent.Rahunok, pok2OpalStoyak.Plos, pok2OpalStoyak.GKal, abonent.NarahStoyak \
				FROM (SELECT * FROM pokaznOpalStoyak WHERE (pokaznOpalStoyak.Year="+ui.spinBox_year->text()+") \
					AND (pokaznOpalStoyak.Month="+ui.spinBox_month->text()+") ) AS pok2OpalStoyak \
				RIGHT JOIN abonent ON pok2OpalStoyak.Rahunok_ID = abonent.Rahunok \
				WHERE abonent.NarahStoyak=true)");
	query->seek(0);
	ui.tableWidget->setRowCount(query->value(0).toInt());
	
	query->exec("SELECT pok1OpalStoyak.Rahunok_ID, pok1OpalStoyak.Plos, pok1OpalStoyak.GKal, abonent.NarahStoyak \
				FROM (SELECT * FROM pokaznOpalStoyak WHERE (pokaznOpalStoyak.Year="+ui.spinBox_year->text()+") \
						AND (pokaznOpalStoyak.Month="+ui.spinBox_month->text()+") ) AS pok1OpalStoyak \
				LEFT JOIN abonent ON pok1OpalStoyak.Rahunok_ID = abonent.Rahunok \
				UNION \
				SELECT abonent.Rahunok, pok2OpalStoyak.Plos, pok2OpalStoyak.GKal, abonent.NarahStoyak \
				FROM (SELECT * FROM pokaznOpalStoyak WHERE (pokaznOpalStoyak.Year="+ui.spinBox_year->text()+") \
					AND (pokaznOpalStoyak.Month="+ui.spinBox_month->text()+") ) AS pok2OpalStoyak \
				RIGHT JOIN abonent ON pok2OpalStoyak.Rahunok_ID = abonent.Rahunok \
				WHERE abonent.NarahStoyak=true \
				ORDER BY pok1OpalStoyak.Rahunok_ID");
	int row=0;
	while (query->next()){
		populateRow(row, query->record());
		row++;
	}
	ui.tableWidget->resizeRowsToContents();
	delete query;
		//Позначення стрічки (вибір того ж рахунку)
	if (ui.tableWidget->rowCount() > 0){
		bool rahFinded=false;
		for (row=0; row<ui.tableWidget->rowCount(); row++){
			if (ui.tableWidget->item(row,0)->data(Qt::DisplayRole).toInt() == curRahunok){
				ui.tableWidget->setCurrentCell(row,0);
				rahFinded=true;
				break;
			}
		}
		if (!rahFinded){
			if (curRow < ui.tableWidget->rowCount())
				ui.tableWidget->setCurrentCell(curRow,0);
			else
				ui.tableWidget->setCurrentCell(0,0);
		}
	}
	
	
	connect(ui.tableWidget->selectionModel(),SIGNAL(currentRowChanged(const QModelIndex &,const QModelIndex &)),this,SLOT(tableWidgetSelectionChanged()));
	connect(ui.tableWidget,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(saveItemValue(QTableWidgetItem *)));
}
//------------------------------------------------------------
void tke_Form_edit_pokazn_lich::loadPokaznFromPrevMonth()
{
	QDate prevDate(ui.spinBox_year->value(), ui.spinBox_month->value(), 1);
	prevDate = prevDate.addMonths(-1);
	tke_Dialog_narah * dialog = new tke_Dialog_narah(0, "Вибір місяця завантаження показників",prevDate.year(), prevDate.month());
	if (dialog->exec() == QDialog::Accepted){
		int prevYear=dialog->get_Year(), prevMonth=dialog->get_Month();
		int year = ui.spinBox_year->value(), month=ui.spinBox_month->value();
		delete dialog;
		int pokaznInserted=0;
		int pokaznUpdated=0;
		int pokaznError=0;
		QSqlQuery *query = new QSqlQuery();
		QSqlQuery *tQuery = new QSqlQuery();
		query->exec("SELECT count(*) FROM pokaznOpalStoyak \
					INNER JOIN abonent ON abonent.Rahunok=pokaznOpalStoyak.Rahunok_ID \
					WHERE Year="+QString::number(prevYear)+" \
						and Month="+QString::number(prevMonth)+" \
						and NarahStoyak=true");
		query->seek(0);
		UPostupForm postup(0, query->value(0).toInt());
		postup.show();
		query->exec("SELECT Rahunok_ID ,Plos, GKal FROM pokaznOpalStoyak \
					INNER JOIN abonent ON abonent.Rahunok=pokaznOpalStoyak.Rahunok_ID \
					WHERE Year="+QString::number(prevYear)+" \
						and Month="+QString::number(prevMonth)+" \
						and NarahStoyak=true");
		while (query->next()){
			tQuery->exec("SELECT id FROM pokaznOpalStoyak \
						WHERE Rahunok_ID="+query->value(0).toString()+" \
							and Year="+QString::number(year)+" \
							and Month="+QString::number(month));
			if (tQuery->seek(0)){//Зміна запису
				int id = tQuery->value(0).toInt();
				if (tQuery->exec("UPDATE pokaznOpalStoyak \
								SET Plos="+query->value(1).toString()+", \
									GKal="+query->value(2).toString()+ " \
								WHERE id="+QString::number(id)))
					pokaznUpdated++;
				else
					pokaznError++;
			}
			else{//Створення запису
				if (tQuery->exec("INSERT INTO pokaznOpalStoyak (id, Rahunok_ID, Year, Month, Plos, GKal) \
							VALUES ("+QString::number(nextLichylnyk("pokaznOpalStoyak"))+", \
							"+query->value(0).toString()+", \
							"+QString::number(year)+", \
							"+QString::number(month)+", \
							"+query->value(1).toString()+", \
							"+query->value(2).toString()+")"))
					pokaznInserted++;
				else
					pokaznError++;
			}
			postup.inc_pos();
		}
		delete query;
		
		QMessageBox::information(this,
				"!!! Завершено завантаження показників !!!",
				"Програма завантажила показників: "+QString::number(pokaznInserted+pokaznUpdated+pokaznError)+
				"\n   в т.ч. створено записів:"+QString::number(pokaznInserted)+
				"\n   в т.ч.  змінено записів:"+QString::number(pokaznUpdated)+
				"\n   в т.ч.  помилки  запису:"+QString::number(pokaznError));
		loadPokazn();
		postup.hide();
	}
	else
		delete dialog;
}
//------------------------------------------------------------
void tke_Form_edit_pokazn_lich::saveItemValue(QTableWidgetItem *item)
{
	disconnect(ui.tableWidget,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(saveItemValue(QTableWidgetItem *)));
	disconnect(ui.tableWidget->selectionModel(),SIGNAL(currentRowChanged(const QModelIndex &,const QModelIndex &)),this,SLOT(tableWidgetSelectionChanged()));
	
	QString colName;
	if (item->column() == 1)
		colName = "Plos";
	else
		colName = "GKal";
	QSqlQuery *query = new QSqlQuery;	
	int rah = ui.tableWidget->item(item->row(),0)->data(Qt::DisplayRole).toInt();
	query->exec("SELECT id FROM pokaznOpalStoyak WHERE year="+ui.spinBox_year->text()+" \
					and month="+ui.spinBox_month->text()+" \
					and Rahunok_ID="+QString::number(rah));
	query->seek(0);
	int id = query->value(0).toInt();
	if (id > 0)
		query->exec("UPDATE pokaznOpalStoyak SET "+colName+"="+item->text().replace(",",".")+
					" WHERE id="+QString::number(id));
	else
		query->exec("INSERT INTO pokaznOpalStoyak("+colName+", Rahunok_ID, year, month) \
					VALUES ("+item->text().replace(",",".")+","+QString::number(rah)+","+ui.spinBox_year->text()+","+ui.spinBox_month->text()+")");
	query->exec("SELECT "+colName+" FROM pokaznOpalStoyak \
					WHERE year="+ui.spinBox_year->text()+" \
						and month="+ui.spinBox_month->text()+" \
						and Rahunok_ID="+QString::number(rah));
	query->seek(0);
	item->setText(query->value(0).toString());
	delete query;
	
	connect(ui.tableWidget->selectionModel(),SIGNAL(currentRowChanged(const QModelIndex &,const QModelIndex &)),this,SLOT(tableWidgetSelectionChanged()));
	connect(ui.tableWidget,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(saveItemValue(QTableWidgetItem *)));
}
//------------------------------------------------------------
void tke_Form_edit_pokazn_lich::tableWidgetSelectionChanged()
{
	if (ui.tableWidget->rowCount() == 0)
		return;
	emit needRahunok(ui.tableWidget->item(ui.tableWidget->currentRow(),0)->data(Qt::DisplayRole).toInt());
}
//------------------------------------------------------------
void tke_Form_edit_pokazn_lich::populateRow(int row, QSqlRecord record)
{
	QTableWidgetItem *item;
	QColor itemColor;
	if (record.value(3).toBool()){
		itemColor.setNamedColor("#ddffe0");
	}
	else{
		itemColor.setNamedColor("#ffe3e3");
	}
	item = new QTableWidgetItem(record.value(0).toString());
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	item->setBackgroundColor(QColor(itemColor));
	ui.tableWidget->setItem(row,0,item);
	item = new QTableWidgetItem(record.value(1).toString());
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	item->setBackgroundColor(QColor(itemColor));
	ui.tableWidget->setItem(row,1,item);
	item = new QTableWidgetItem(record.value(2).toString());
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	item->setBackgroundColor(QColor(itemColor));
	ui.tableWidget->setItem(row,2,item);
}
//------------------------------------------------------------
tke_Form_edit_pokazn_lich::~tke_Form_edit_pokazn_lich()
{
	
}
//------------------------------------------------------------
