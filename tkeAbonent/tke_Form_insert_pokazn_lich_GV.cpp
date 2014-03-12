//------------------------------------------------------------
//
// Implamantation of tke_Form_insert_pokazn_lich_GV class.
// Created by Roman Mulyk 16.06.2008
//
//------------------------------------------------------------

#include <USqlAccessible>
#include "tke_Form_insert_pokazn_lich_GV.h"

#define OPT_COL_WIDTH 70
//------------------------------------------------------------
tke_Form_insert_pokazn_lich_GV::tke_Form_insert_pokazn_lich_GV( int curYear, int curMonth, QWidget * parent) 
	: QWidget(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	
	ui.tableWidget_budLich->setColumnHidden(0,true);
	ui.tableWidget_budLich->setColumnHidden(1,true);
	ui.tableWidget_abonLich->setColumnHidden(0,true);
	ui.tableWidget_abonLich->setColumnHidden(1,true);
	
	ui.tableWidget_budLich->setItemDelegateForColumn(2, new UDoubleSpinBoxDelegate(0,1000000,5,ui.tableWidget_budLich));
	ui.tableWidget_budLich->setItemDelegateForColumn(3, new UDoubleSpinBoxDelegate(0,1000000,5,ui.tableWidget_budLich));
	ui.tableWidget_budLich->setItemDelegateForColumn(4, new UDoubleSpinBoxDelegate(-1000000,1000000,5,ui.tableWidget_budLich));
	ui.tableWidget_budLich->setItemDelegateForColumn(5, new UDoubleSpinBoxDelegate(0,1000000,5,ui.tableWidget_budLich));
	ui.tableWidget_budLich->setItemDelegateForColumn(6, new USpinBoxDelegate(0,1000000,ui.tableWidget_budLich));
	ui.tableWidget_budLich->setItemDelegateForColumn(7, new USpinBoxDelegate(0,1000000,ui.tableWidget_budLich));
	ui.tableWidget_budLich->setItemDelegateForColumn(8, new UDoubleSpinBoxDelegate(0,1000000,5,ui.tableWidget_budLich));
	
	ui.tableWidget_abonLich->setItemDelegateForColumn(2, new UFactSpojyvDelegate(ui.tableWidget_abonLich));
	//ui.tableWidget_abonLich->setItemDelegateForColumn(2, new UDoubleSpinBoxDelegate(0,1000000,5,ui.tableWidget_abonLich));
	ui.tableWidget_abonLich->setItemDelegateForColumn(3, new UDoubleSpinBoxDelegate(0,1000000,5,ui.tableWidget_abonLich));
	ui.tableWidget_abonLich->setItemDelegateForColumn(4, new UDoubleSpinBoxDelegate(-1000000,1000000,5,ui.tableWidget_abonLich));
	ui.tableWidget_abonLich->setItemDelegateForColumn(5, new UDoubleSpinBoxDelegate(0,1000000,5,ui.tableWidget_abonLich));
	ui.tableWidget_abonLich->setItemDelegateForColumn(6, new USpinBoxDelegate(0,1000000,ui.tableWidget_budLich));
	ui.tableWidget_abonLich->setItemDelegateForColumn(7, new USpinBoxDelegate(0,1000000,ui.tableWidget_budLich));
	
	ui.spinBox_curMonth->setValue(curMonth);
	ui.spinBox_curYear->setValue(curYear);
	dateChanged();
	
	connect(ui.toolButton_reload, SIGNAL(clicked()), this, SLOT(toolButton_reload_clicked()));
	connect(ui.toolButton_calcOsobaTaryf, SIGNAL(clicked()), this, SLOT(toolButton_calcOsobaTaryf_clicked()));
	connect(ui.toolButton_closeForm, SIGNAL(clicked()), this, SLOT(close()));
	
	connect(ui.spinBox_curMonth, SIGNAL(valueChanged(int)), this, SLOT(dateChanged()));
	connect(ui.spinBox_curYear, SIGNAL(valueChanged(int)), this, SLOT(dateChanged()));
	
	connect(ui.lineEdit_rahunok, SIGNAL(returnPressed()), this, SLOT(toolButton_findRahunok_clicked()));
	connect(ui.toolButton_findRahunok, SIGNAL(clicked()), this, SLOT(toolButton_findRahunok_clicked()));
}
//------------------------------------------------------------
void tke_Form_insert_pokazn_lich_GV::populateLichBudynkiv()
{
	disconnect(ui.tableWidget_budLich->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &,const QModelIndex &)), this, SLOT(populateLichAbonentiv()));
	disconnect(ui.tableWidget_budLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichBudynkiv(int,int)));
	
	int rowCount=0;
	int oldId=1;
	if (ui.tableWidget_budLich->rowCount()>0)
		oldId = ui.tableWidget_budLich->item(ui.tableWidget_budLich->currentRow(),0)->text().toInt();

	QSqlQuery *query = new QSqlQuery();
	QSqlQuery prevPokaznQuery;
	query->exec("SELECT count(*) FROM budynky INNER JOIN lichylnykyBudynkovi ON lichylnykyBudynkovi.Budynok_id=budynky.id");
	query->seek(0);
	rowCount = query->value(0).toInt();
	ui.tableWidget_budLich->setRowCount(rowCount);
	query->exec("SELECT budynky.Vulycia, budynky.Bud_num, lichylnykyBudynkovi.LichNum, lichylnykyBudynkovi.id, pokaznLichBudCur.id, \
					pokaznLichBudCur.Pokazn, pokaznLichBudCur.Korektura, \
					pokaznLichBudCur.FactVykor, pokaznLichBudCur.KilkistDnivBezLichylnyka, \
					pokaznLichBudCur.VykorVodyOsobaInMonth, pokaznLichBudCur.KilkistDnivBezGV \
				FROM (budynky \
				INNER JOIN lichylnykyBudynkovi ON lichylnykyBudynkovi.Budynok_id=budynky.id) \
				LEFT JOIN (SELECT * FROM pokaznLichBud \
							WHERE year(pokaznDate)="+QString::number(curDate.year())+" \
								and month(pokaznDate)="+QString::number(curDate.month())+") AS pokaznLichBudCur \
					ON pokaznLichBudCur.BudLich_id=lichylnykyBudynkovi.id \
				ORDER BY budynky.Vulycia, budynky.Bud_num, lichylnykyBudynkovi.LichNum");
	bool isSelected=false;
	for (int row=0; (row<rowCount) && query->next(); ++row){
		item = new QTableWidgetItem(query->value(3).toString());
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		ui.tableWidget_budLich->setItem(row, 0, item);
		item = new QTableWidgetItem(query->value(4).toString());
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		ui.tableWidget_budLich->setItem(row, 1, item);

		prevPokaznQuery.exec("SELECT Pokazn FROM pokaznLichBud \
								WHERE ((year(pokaznDate)="+QString::number(curDate.year())+" \
									and month(pokaznDate)<"+QString::number(curDate.month())+") \
									or year(pokaznDate)<"+QString::number(curDate.year())+") \
									and BudLich_ID="+query->value(3).toString()+" \
								ORDER BY pokaznDate DESC");							
		double prevPokaznOffDate=0;
		if (prevPokaznQuery.seek(0))
			prevPokaznOffDate = prevPokaznQuery.value(0).toDouble();
		item = new QTableWidgetItem(QString::number(prevPokaznOffDate));
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		item->setFlags(item->flags() ^ Qt::ItemIsEditable);
		ui.tableWidget_budLich->setItem(row, 2, item);
		item = new QTableWidgetItem(query->value(5).toString());
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		ui.tableWidget_budLich->setItem(row, 3, item);
		item = new QTableWidgetItem(query->value(6).toString());
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		ui.tableWidget_budLich->setItem(row, 4, item);
		item = new QTableWidgetItem(query->value(7).toString());
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		ui.tableWidget_budLich->setItem(row, 5, item);
		item = new QTableWidgetItem(query->value(8).toString());
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		ui.tableWidget_budLich->setItem(row, 6, item);
		item = new QTableWidgetItem(query->value(10).toString());
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		ui.tableWidget_budLich->setItem(row, 7, item);
		item = new QTableWidgetItem(uMToStr2(query->value(9).toDouble()));
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		ui.tableWidget_budLich->setItem(row, 8, item);
		
		ui.tableWidget_budLich->setVerticalHeaderItem(row,new QTableWidgetItem(query->value(0).toString()+
										" "+query->value(1).toString()+"->"+query->value(2).toString())); 
		
		if ((query->value(3).toInt()==oldId) && (oldId!=0)){
			isSelected = true;
			ui.tableWidget_budLich->setCurrentCell( row, 1);
		}
	}
	if (!isSelected && (ui.tableWidget_budLich->rowCount()>0))
		ui.tableWidget_budLich->setCurrentCell( 0, 1);
	ui.tableWidget_budLich->resizeColumnsToContents();
	ui.tableWidget_budLich->resizeRowsToContents();
	delete query;
	
	if (ui.tableWidget_budLich->columnWidth(2)<OPT_COL_WIDTH)
		ui.tableWidget_budLich->setColumnWidth(2,OPT_COL_WIDTH);
	if (ui.tableWidget_budLich->columnWidth(3)<OPT_COL_WIDTH)
		ui.tableWidget_budLich->setColumnWidth(3,OPT_COL_WIDTH);
	if (ui.tableWidget_budLich->columnWidth(4)<OPT_COL_WIDTH)
		ui.tableWidget_budLich->setColumnWidth(4,OPT_COL_WIDTH);
	if (ui.tableWidget_budLich->columnWidth(5)<OPT_COL_WIDTH)
		ui.tableWidget_budLich->setColumnWidth(5,OPT_COL_WIDTH);
	if (ui.tableWidget_budLich->columnWidth(6)<OPT_COL_WIDTH)
		ui.tableWidget_budLich->setColumnWidth(6,OPT_COL_WIDTH);
	
	showPidsumkyBud();
	
	populateLichAbonentiv();
	
	connect(ui.tableWidget_budLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichBudynkiv(int,int)));
	connect(ui.tableWidget_budLich->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &,const QModelIndex &)), this, SLOT(populateLichAbonentiv()));
}
//------------------------------------------------------------
void tke_Form_insert_pokazn_lich_GV::populateLichAbonentiv()
{
	disconnect(ui.tableWidget_abonLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichAbonentiv(int,int)));
	disconnect(ui.tableWidget_abonLich->selectionModel(), SIGNAL(currentRowChanged ( const QModelIndex &, const QModelIndex &)), this, SLOT(curAbonentChanged()));
	
	int rowCount=0;
	int oldId=1;
	if (ui.tableWidget_abonLich->rowCount()>0 && ui.tableWidget_abonLich->item(ui.tableWidget_abonLich->currentRow(),0))
		oldId = ui.tableWidget_abonLich->item(ui.tableWidget_abonLich->currentRow(),0)->text().toInt();
	if (ui.tableWidget_budLich->rowCount()>0)
		budLichId = ui.tableWidget_budLich->item(ui.tableWidget_budLich->currentRow(),0)->text().toInt();
	else
		budLichId = 0;
	if (budLichId > 0){
		QSqlQuery *query = new QSqlQuery();
		QSqlQuery prevPokaznQuery;
		query->exec("SELECT count(*) FROM lichylnykyAbon WHERE BudLich_ID="+QString::number(budLichId));
		query->seek(0);
		rowCount = query->value(0).toInt();
		ui.tableWidget_abonLich->setRowCount(rowCount);
		query->exec("SELECT abonent.Kvartyra, lichylnykyAbon.Rahunok, lichylnykyAbon.LichNum, lichylnykyAbon.id, \
						pokaznLichAbonCur.id, pokaznLichAbonCur.Pokazn AS PokaznCur, \
						pokaznLichAbonCur.Korektura, pokaznLichAbonCur.KilkistDnivBezLichylnyka, \
						pokaznLichAbonCur.FactVykor, pokaznLichAbonCur.KilkistDnivBezGV \
					FROM ((SELECT * FROM lichylnykyAbon WHERE BudLich_ID="+QString::number(budLichId)+") AS lichylnykyAbon \
					LEFT JOIN abonent ON abonent.Rahunok=lichylnykyAbon.Rahunok) \
					LEFT JOIN (SELECT * FROM pokaznLichAbon \
								WHERE year(pokaznDate)="+QString::number(curDate.year())+" \
									and month(pokaznDate)="+QString::number(curDate.month())+") AS pokaznLichAbonCur \
						ON pokaznLichAbonCur.AbonLich_id=lichylnykyAbon.id \
					ORDER BY abonent.Kvartyra");
		bool isSelected=false;
		for (int row=0; (row<rowCount) && query->next(); ++row){
			item = new QTableWidgetItem(query->value(3).toString());
			item->setTextAlignment(Qt::AlignRight);
			ui.tableWidget_abonLich->setItem(row, 0, item);
			item = new QTableWidgetItem(query->value(4).toString());
			item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			ui.tableWidget_abonLich->setItem(row, 1, item);
			prevPokaznQuery.exec("SELECT Pokazn FROM pokaznLichAbon \
									WHERE ((year(pokaznDate)="+QString::number(curDate.year())+" and month(pokaznDate)<"+QString::number(curDate.month())+") \
											or year(pokaznDate)<"+QString::number(curDate.year())+") \
											and AbonLich_ID="+query->value(3).toString()+" \
									ORDER BY pokaznDate DESC");
			double prevPokaznOffDate=0;
			if (prevPokaznQuery.seek(0))
				prevPokaznOffDate = prevPokaznQuery.value(0).toDouble();
			item = new QTableWidgetItem(QString::number(prevPokaznOffDate));
			item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			item->setFlags(item->flags() ^ Qt::ItemIsEditable);
			ui.tableWidget_abonLich->setItem(row, 2, item);
			item = new QTableWidgetItem(query->value(5).toString());
			item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			ui.tableWidget_abonLich->setItem(row, 3, item);
			item = new QTableWidgetItem(query->value(6).toString());
			item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			ui.tableWidget_abonLich->setItem(row, 4, item);
			item = new QTableWidgetItem(query->value(8).toString());
			item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			ui.tableWidget_abonLich->setItem(row, 5, item);
			item = new QTableWidgetItem(query->value(7).toString());
			item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			ui.tableWidget_abonLich->setItem(row, 6, item);
			item = new QTableWidgetItem(query->value(9).toString());
			item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			ui.tableWidget_abonLich->setItem(row, 7, item);
			
			ui.tableWidget_abonLich->setVerticalHeaderItem(row,new QTableWidgetItem("кв."+query->value(0).toString()+
										" /рах."+query->value(1).toString()+" /ліч."+query->value(2).toString())); 
			
			if ((query->value(3).toInt()==oldId) && (oldId!=0)){
				isSelected = true;
				ui.tableWidget_abonLich->setCurrentCell( row, 1);
			}
			
			calcPokaznLich(row);
		}
		
		if (!isSelected && (ui.tableWidget_abonLich->rowCount()>0))
			ui.tableWidget_abonLich->setCurrentCell( 0, 1);
		delete query;
	}
	else
		ui.tableWidget_abonLich->setRowCount(0);
	
	ui.tableWidget_abonLich->resizeColumnsToContents();
	ui.tableWidget_abonLich->resizeRowsToContents();
	
	if (ui.tableWidget_abonLich->columnWidth(2)<OPT_COL_WIDTH)
		ui.tableWidget_abonLich->setColumnWidth(2,OPT_COL_WIDTH);
	if (ui.tableWidget_abonLich->columnWidth(3)<OPT_COL_WIDTH)
		ui.tableWidget_abonLich->setColumnWidth(3,OPT_COL_WIDTH);
	if (ui.tableWidget_abonLich->columnWidth(4)<OPT_COL_WIDTH)
		ui.tableWidget_abonLich->setColumnWidth(4,OPT_COL_WIDTH);
	
	showPidsumkyAbon();
	
	connect(ui.tableWidget_abonLich->selectionModel(), SIGNAL(currentRowChanged ( const QModelIndex &, const QModelIndex &)), this, SLOT(curAbonentChanged()));
	connect(ui.tableWidget_abonLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichAbonentiv(int,int)));
}
//------------------------------------------------------------
void tke_Form_insert_pokazn_lich_GV::updateLichBudynkiv(int row, int col)
{
	disconnect(ui.tableWidget_budLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichBudynkiv(int,int)));
	int pokaznLichBudId = -1;
	bool isInsert=false;
	if (ui.tableWidget_budLich->rowCount()>0 && ui.tableWidget_budLich->item(row,1))
		pokaznLichBudId = ui.tableWidget_budLich->item(row,1)->text().toInt();
	if (pokaznLichBudId>0){
		adQuery.exec("UPDATE pokaznLichBud SET Pokazn="+ui.tableWidget_budLich->item(row,3)->text().replace(",",".")+", \
									Korektura="+ui.tableWidget_budLich->item(row,4)->text().replace(",",".")+", \
									FactVykor="+ui.tableWidget_budLich->item(row,5)->text().replace(",",".")+", \
									KilkistDnivBezLichylnyka="+ui.tableWidget_budLich->item(row,6)->text().replace(",",".")+", \
									KilkistDnivBezGV="+ui.tableWidget_budLich->item(row,7)->text().replace(",",".")+", \
									VykorVodyOsobaInMonth="+ui.tableWidget_budLich->item(row,8)->text().replace(",",".")+" \
					WHERE id="+ui.tableWidget_budLich->item(row,1)->text());
	}
	else if (pokaznLichBudId==0){
		isInsert = true;
		pokaznLichBudId = nextLichylnyk("pokaznLichBud");
		adQuery.prepare("INSERT INTO pokaznLichBud (id, pokaznDate, BudLich_id, Pokazn, Korektura, FactVykor, KilkistDnivBezLichylnyka, KilkistDnivBezGV, VykorVodyOsobaInMonth) \
							VALUES (:id, :pokaznDate, :BudLich_id, :Pokazn, :Korektura, :FactVykor, :KilkistDnivBezLichylnyka, :KilkistDnivBezGV, :VykorVodyOsobaInMonth)");
		adQuery.bindValue(":id",pokaznLichBudId);
		adQuery.bindValue(":pokaznDate",curDate);
		adQuery.bindValue(":BudLich_id",ui.tableWidget_budLich->item(row,0)->text().toInt());
		adQuery.bindValue(":Pokazn",ui.tableWidget_budLich->item(row,3)->text().toDouble());
		adQuery.bindValue(":Korektura",ui.tableWidget_budLich->item(row,4)->text().toDouble());
		adQuery.bindValue(":FactVykor",ui.tableWidget_budLich->item(row,5)->text().toDouble());
		adQuery.bindValue(":KilkistDnivBezLichylnyka",ui.tableWidget_budLich->item(row,6)->text().toInt());
		adQuery.bindValue(":KilkistDnivBezGV",ui.tableWidget_budLich->item(row,7)->text().toInt());
		adQuery.bindValue(":VykorVodyOsobaInMonth",d_to_m(ui.tableWidget_budLich->item(row,8)->text().toDouble()));
		adQuery.exec();
	}
	if (pokaznLichBudId>0){
		adQuery.exec("SELECT id, pokaznDate, BudLich_id, Pokazn, Korektura, FactVykor, KilkistDnivBezLichylnyka, KilkistDnivBezGV, VykorVodyOsobaInMonth FROM pokaznLichBud WHERE id="+QString::number(pokaznLichBudId));
		if (adQuery.seek(0)){
			if (isInsert){
				ui.tableWidget_budLich->setItem(row, 1, new QTableWidgetItem(adQuery.value(0).toString()));
			}
			item = new QTableWidgetItem(adQuery.value(3).toString());
			item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			ui.tableWidget_budLich->setItem(row, 3, item);
			item = new QTableWidgetItem(adQuery.value(4).toString());
			item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			ui.tableWidget_budLich->setItem(row, 4, item);
			item = new QTableWidgetItem(uMToStr2(adQuery.value(5).toDouble()));
			item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			ui.tableWidget_budLich->setItem(row, 5, item);
			item = new QTableWidgetItem(adQuery.value(6).toString());
			item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			ui.tableWidget_budLich->setItem(row, 6, item);
			item = new QTableWidgetItem(adQuery.value(7).toString());
			item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			ui.tableWidget_budLich->setItem(row, 7, item);
			item = new QTableWidgetItem(uMToStr2(adQuery.value(8).toDouble()));
			item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			ui.tableWidget_budLich->setItem(row, 8, item);
		}
	}
	else{
		ui.tableWidget_budLich->setItem(row, 1, new QTableWidgetItem("0"));
		item = new QTableWidgetItem("0");
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		ui.tableWidget_budLich->setItem(row, 3, item);
		item = new QTableWidgetItem("0");
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		ui.tableWidget_budLich->setItem(row, 4, item);
		item = new QTableWidgetItem("0");
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		ui.tableWidget_budLich->setItem(row, 5, item);
		item = new QTableWidgetItem("0");
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		ui.tableWidget_budLich->setItem(row, 6, item);
		item = new QTableWidgetItem("0");
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		ui.tableWidget_budLich->setItem(row, 7, item);
		item = new QTableWidgetItem("0");
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		ui.tableWidget_budLich->setItem(row, 8, item);
	}
	
	showPidsumkyBud();
	
	connect(ui.tableWidget_budLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichBudynkiv(int,int)));
}
//------------------------------------------------------------
void tke_Form_insert_pokazn_lich_GV::updateLichAbonentiv(int row, int col)
{
	disconnect(ui.tableWidget_abonLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichAbonentiv(int,int)));
	
	qDebug() << "void tke_Form_insert_pokazn_lich_GV::updateLichAbonentiv(int row, int col)";
	
	int pokaznLichAbonId = -1;
	bool isInsert=false;
	if (ui.tableWidget_abonLich->rowCount()>0 && ui.tableWidget_abonLich->item(ui.tableWidget_abonLich->currentRow(),1))
		pokaznLichAbonId = ui.tableWidget_abonLich->item(row,1)->text().toInt();
	if (pokaznLichAbonId>0){
		adQuery.exec("UPDATE pokaznLichAbon SET Pokazn="+sqlStr(ui.tableWidget_abonLich->item(row,3)->data(Qt::EditRole).toDouble(), false)+", \
									Korektura="+sqlStr(ui.tableWidget_abonLich->item(row,4)->data(Qt::EditRole).toDouble(), false)+", \
									FactVykor="+sqlStr(ui.tableWidget_abonLich->item(row,5)->data(Qt::EditRole).toDouble(), false)+", \
									KilkistDnivBezLichylnyka="+sqlStr(ui.tableWidget_abonLich->item(row,6)->data(Qt::EditRole).toInt())+", \
									KilkistDnivBezGV="+sqlStr(ui.tableWidget_abonLich->item(row,7)->data(Qt::EditRole).toInt())+" \
					WHERE id="+sqlStr(ui.tableWidget_abonLich->item(row,1)->data(Qt::EditRole).toInt()));
		qDebug() << adQuery.lastQuery();
		qDebug() << "lastError - " << adQuery.lastError().text();
	}
	else if (pokaznLichAbonId==0){
		isInsert = true;
		pokaznLichAbonId = nextLichylnyk("pokaznLichAbon");
		adQuery.prepare("INSERT INTO pokaznLichAbon (id, pokaznDate, AbonLich_id, Pokazn, Korektura, KilkistDnivBezLichylnyka, FactVykor, KilkistDnivBezGV) \
							VALUES (:id, :pokaznDate, :AbonLich_id, :Pokazn, :Korektura, :KilkistDnivBezLichylnyka, :FactVykor, :KilkistDnivBezGV)");
		adQuery.bindValue(":id",pokaznLichAbonId);
		adQuery.bindValue(":pokaznDate",curDate);
		adQuery.bindValue(":AbonLich_id",ui.tableWidget_abonLich->item(row,0)->text().toDouble());
		adQuery.bindValue(":Pokazn",ui.tableWidget_abonLich->item(row,3)->text().toDouble());
		adQuery.bindValue(":Korektura",ui.tableWidget_abonLich->item(row,4)->text().toDouble());
		adQuery.bindValue(":FactVykor",ui.tableWidget_abonLich->item(row,5)->text().toDouble());
		adQuery.bindValue(":KilkistDnivBezLichylnyka",ui.tableWidget_abonLich->item(row,6)->text().toDouble());
		adQuery.bindValue(":KilkistDnivBezGV",ui.tableWidget_abonLich->item(row,7)->text().toDouble());
		adQuery.exec();
	}
	if (pokaznLichAbonId>0){
		adQuery.exec("SELECT id, Pokazn, Korektura, KilkistDnivBezLichylnyka, FactVykor, KilkistDnivBezGV FROM pokaznLichAbon WHERE id="+QString::number(pokaznLichAbonId));
		if (adQuery.seek(0)){
			if (isInsert){
				ui.tableWidget_abonLich->setItem(row, 1, new QTableWidgetItem(adQuery.value(0).toString()));
			}
			item = new QTableWidgetItem(adQuery.value(1).toString());
			item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			ui.tableWidget_abonLich->setItem(row, 3, item);
			item = new QTableWidgetItem(adQuery.value(2).toString());
			item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			ui.tableWidget_abonLich->setItem(row, 4, item);
			
			item = new QTableWidgetItem(adQuery.value(4).toString());
			item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			ui.tableWidget_abonLich->setItem(row, 5, item);
			
			item = new QTableWidgetItem(adQuery.value(3).toString());
			item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			ui.tableWidget_abonLich->setItem(row, 6, item);
			
			item = new QTableWidgetItem(adQuery.value(5).toString());
			item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			ui.tableWidget_abonLich->setItem(row, 7, item);
		}
	}
	else{
		ui.tableWidget_abonLich->setItem(row, 1, new QTableWidgetItem("0"));
		item = new QTableWidgetItem("0");
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		ui.tableWidget_abonLich->setItem(row, 3, item);
		item = new QTableWidgetItem("0");
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		ui.tableWidget_abonLich->setItem(row, 4, item);
		item = new QTableWidgetItem("0");
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		ui.tableWidget_abonLich->setItem(row, 5, item);
		item = new QTableWidgetItem("0");
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		ui.tableWidget_abonLich->setItem(row, 6, item);
		item = new QTableWidgetItem("0");
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		ui.tableWidget_abonLich->setItem(row, 7, item);
	}
	
	showPidsumkyAbon();
	calcPokaznLich(row);
	
	connect(ui.tableWidget_abonLich, SIGNAL(cellChanged(int,int)), this, SLOT(updateLichAbonentiv(int,int)));
}
//------------------------------------------------------------
void tke_Form_insert_pokazn_lich_GV::toolButton_reload_clicked()
{
	populateLichBudynkiv();
}
//------------------------------------------------------------
void tke_Form_insert_pokazn_lich_GV::toolButton_calcOsobaTaryf_clicked()
{
	int rowCount = ui.tableWidget_budLich->rowCount();
	UPostupForm *postupForm = new UPostupForm(0, rowCount);
	postupForm->show();
	qDebug() << " -->> Розрахунок використання води для абонентів без лічильника ---";
	for (int row=0; row<rowCount; ++row){
		item = new QTableWidgetItem();
		item->setData(Qt::EditRole, rozrahVytratyTeplaVDenBezLich(row));
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		ui.tableWidget_budLich->setItem(row, 8, item);
		postupForm->inc_pos();
	}
	qDebug() << " <<-- Кінець розрахунку використання води для абонентів без лічильника ---";
	delete postupForm;
}
//------------------------------------------------------------
void tke_Form_insert_pokazn_lich_GV::dateChanged()
{
	curDate.setDate(ui.spinBox_curYear->value(), ui.spinBox_curMonth->value(), 1);
	prevDate = curDate.addMonths(-1);
	
	populateLichBudynkiv();
}
//------------------------------------------------------------
double tke_Form_insert_pokazn_lich_GV::rozrahVytratyTeplaVDenBezLich(int row)
{
	int curBudLichId = ui.tableWidget_budLich->item(row,0)->text().toInt();
	double pokaznLichBudPrev=0, pokaznLichBudCur=0, KorecturaBud=0, FactVykor=0,
			pokaznAbonSumPrev=0, pokaznAbonSumCur=0, KorecturaSumAbon=0;
	int diNi=0;
	double vykorOsobaVDen;
	double pokaznLichAbonRes=0;
	double x;
	pokaznLichBudPrev = ui.tableWidget_budLich->item(row,2)->text().toDouble();
	pokaznLichBudCur = ui.tableWidget_budLich->item(row,3)->text().toDouble();
	KorecturaBud = ui.tableWidget_budLich->item(row,4)->text().toDouble();
	FactVykor = ui.tableWidget_budLich->item(row,5)->text().toDouble();
	
	adQuery.exec("SELECT pokaznLichAbonCur.Pokazn, pokaznLichAbonCur.Korektura, lichylnykyAbon.id, \
					pokaznLichAbonCur.FactVykor, pokaznLichAbonCur.id, lichylnykyAbon.IsLichylnyk \
				FROM (SELECT * FROM lichylnykyAbon WHERE BudLich_ID="+QString::number(curBudLichId)+") AS lichylnykyAbon \
				LEFT JOIN (SELECT * FROM pokaznLichAbon \
							WHERE year(pokaznDate)="+QString::number(curDate.year())+" \
								and month(pokaznDate)="+QString::number(curDate.month())+") AS pokaznLichAbonCur \
					ON pokaznLichAbonCur.AbonLich_id=lichylnykyAbon.id");
	QSqlQuery prevPokaznQuery;
	//qDebug() << adQuery.lastQuery();
	while (adQuery.next()){
		if (adQuery.value(4).toInt() > 0 && adQuery.value(5).toBool()){
			prevPokaznQuery.exec("SELECT Pokazn FROM pokaznLichAbon \
								WHERE ((year(pokaznDate)="+QString::number(curDate.year())+" and month(pokaznDate)<"+QString::number(curDate.month())+") \
										or year(pokaznDate)<"+QString::number(curDate.year())+") \
										and AbonLich_ID="+adQuery.value(2).toString()+" \
								ORDER BY pokaznDate DESC");
			pokaznAbonSumPrev=0;
			if (prevPokaznQuery.seek(0))
				pokaznAbonSumPrev = prevPokaznQuery.value(0).toDouble();
			pokaznAbonSumCur = adQuery.value(0).toDouble();
			KorecturaSumAbon = adQuery.value(1).toDouble();
			x = pokaznAbonSumCur - pokaznAbonSumPrev - KorecturaSumAbon;
			if (x>0)
				pokaznLichAbonRes += x;
			if ( adQuery.value(3).toDouble()>0)
				pokaznLichAbonRes += adQuery.value(3).toDouble();
		}
	}
	//Вибірка рахунків абонентів, які безпосередньо входять в діапазон квартир, що відносяться до будинкового лічильника і обрахунок суми di * Ni
	adQuery.exec("SELECT abonLich.KilkistDnivBezLichylnyka, abon.Ludy, abon.Kvartyra, \
						abon.Rahunok, abonLich.Rahunok, abonLich.IsLichylnyk, abonLich.KilkistDnivBezGV \
				FROM (SELECT * FROM abonent WHERE GarVoda=true and budynok_id=(SELECT max(Budynok_id) FROM lichylnykyBudynkovi WHERE id="+QString::number(curBudLichId)+") \
											and (Kvartyra BETWEEN (SELECT max(StartKVNumber) FROM lichylnykyBudynkovi \
																						WHERE id="+QString::number(curBudLichId)+") \
															AND (SELECT max(EndKvNumber) FROM lichylnykyBudynkovi \
																						WHERE id="+QString::number(curBudLichId)+"))) \
					AS abon \
				LEFT JOIN (SELECT lichylnykyAbon.Rahunok, lichylnykyAbon.IsLichylnyk, \
									pokaznLichAb.KilkistDnivBezLichylnyka, pokaznLichAb.KilkistDnivBezGV FROM lichylnykyAbon \
							LEFT JOIN (SELECT * FROM pokaznLichAbon WHERE year(pokaznDate)="+QString::number(curDate.year())+" \
															and month(pokaznDate)="+QString::number(curDate.month())+") \
								AS pokaznLichAb \
								ON pokaznLichAb.AbonLich_id=lichylnykyAbon.id \
								WHERE lichylnykyAbon.BudLich_id="+QString::number(curBudLichId)+") \
					AS abonLich \
					ON abonLich.Rahunok = abon.Rahunok");
	//qDebug() << adQuery.lastQuery();
	while (adQuery.next()){
		int noGVDays = 0;
		if (adQuery.value(4).toInt() == 0 || (!adQuery.value(5).toBool())){
			noGVDays = adQuery.value(6).toInt();
			if (noGVDays < 0)
				noGVDays = 0;
			int isGVDaysNoLich = curDate.daysInMonth() - noGVDays;
			if (isGVDaysNoLich > 0)
				diNi += isGVDaysNoLich * adQuery.value(1).toInt();
		}
		else if ((adQuery.value(0).toInt()>0 || noGVDays>0) && adQuery.value(5).toBool()){
			int noLichDays = adQuery.value(0).toInt() - noGVDays;
			if (noLichDays > 0)
				diNi += noLichDays * adQuery.value(1).toInt();
		}
	}
	//   Вибірка рахунків абонентів, які не входять в діапазон квартир, що відносяться до будинкового лічильника, при цьому мають 
	//   лічильник і фізично під'єднані до поточного будинкового лічильника обрахунок суми di * Ni
	adQuery.exec("SELECT abonLich.KilkistDnivBezLichylnyka, abon.Ludy, abon.Kvartyra, \
						abon.Rahunok, abonLich.Rahunok, abonLich.IsLichylnyk, abonLich.KilkistDnivBezGV \
				FROM (SELECT * FROM abonent WHERE GarVoda=true and budynok_id=(SELECT max(Budynok_id) FROM lichylnykyBudynkovi WHERE id="+QString::number(curBudLichId)+") \
											and (Kvartyra NOT BETWEEN (SELECT max(StartKVNumber) FROM lichylnykyBudynkovi \
																						WHERE id="+QString::number(curBudLichId)+") \
															AND (SELECT max(EndKvNumber) FROM lichylnykyBudynkovi \
																						WHERE id="+QString::number(curBudLichId)+"))) \
					AS abon \
				LEFT JOIN (SELECT lichylnykyAbon.Rahunok, lichylnykyAbon.IsLichylnyk, \
									pokaznLichAb.KilkistDnivBezLichylnyka, pokaznLichAb.KilkistDnivBezGV FROM lichylnykyAbon \
							LEFT JOIN (SELECT * FROM pokaznLichAbon WHERE year(pokaznDate)="+QString::number(curDate.year())+" \
															and month(pokaznDate)="+QString::number(curDate.month())+") \
								AS pokaznLichAb \
								ON pokaznLichAb.AbonLich_id=lichylnykyAbon.id \
								WHERE lichylnykyAbon.BudLich_id="+QString::number(curBudLichId)+") \
					AS abonLich \
					ON abonLich.Rahunok = abon.Rahunok \
				WHERE abonLich.Rahunok>0");
	//qDebug() << adQuery.lastQuery();
	while (adQuery.next())
		if (adQuery.value(0).toInt()>0)
			diNi += adQuery.value(0).toInt() * adQuery.value(1).toInt();
	if (diNi < 1)
		vykorOsobaVDen = 0;
	else
		vykorOsobaVDen = ((FactVykor + pokaznLichBudCur - pokaznLichBudPrev - KorecturaBud) - pokaznLichAbonRes)*curDate.daysInMonth()/diNi;
	/*qDebug()	<< "  -- curBudLichId = " << curBudLichId
				<< "\n  -- pokaznLichBudCur = " << pokaznLichBudCur
				<< "\n  -- pokaznLichBudPrev = " << pokaznLichBudPrev
				<< "\n  -- KorecturaBud = " << KorecturaBud
				<< "\n  -- FactVykor = " << FactVykor
				<< "\n  -- pokaznLichAbonRes = " << pokaznLichAbonRes
				<< "\n  -- curDate.daysInMonth() = " << curDate.daysInMonth()
				<< "\n  -- diNi = " << diNi
				<< "\n  -- vykorOsobaVDen = " << vykorOsobaVDen;*/
	if (vykorOsobaVDen < 0)
		vykorOsobaVDen = 0;
	return vykorOsobaVDen;
}
//------------------------------------------------------------
void tke_Form_insert_pokazn_lich_GV::showPidsumkyBud()
{
	int rowCount = ui.tableWidget_budLich->rowCount();
	double pokaznLich=0, faktVykor=0, temp;
	for (int row=0; row<rowCount; ++row){
		if (ui.tableWidget_budLich->item(row,1)->data(Qt::EditRole).toDouble() > 0){
			temp = ui.tableWidget_budLich->item(row,3)->data(Qt::EditRole).toDouble() - ui.tableWidget_budLich->item(row,2)->data(Qt::EditRole).toDouble() - ui.tableWidget_budLich->item(row,4)->data(Qt::EditRole).toDouble();
			if (temp > 0)
				pokaznLich += temp;
			temp = ui.tableWidget_budLich->item(row,5)->data(Qt::EditRole).toDouble();
			if (temp > 0)
				faktVykor += temp;
		}
	}
	
	ui.label_countBudLich->setText("Кількість: "+QString::number(rowCount)+".\nСумарн.показн.: "+
									QString::number(pokaznLich, 'f', 3).replace('.',',')+"; сумарн.викор.: "+
									QString::number(faktVykor, 'f', 3).replace('.',',')+"; Всього: "+
									QString::number(pokaznLich+faktVykor, 'f', 3).replace('.',',')+".");
}
//------------------------------------------------------------
void tke_Form_insert_pokazn_lich_GV::showPidsumkyAbon()
{
	int rowCount = ui.tableWidget_abonLich->rowCount();
	double pokaznLich=0, faktVykor=0, temp;
	for (int row=0; row<rowCount; ++row){
		if (ui.tableWidget_abonLich->item(row,1)->data(Qt::EditRole).toDouble() > 0){
			temp = ui.tableWidget_abonLich->item(row,3)->data(Qt::EditRole).toDouble() - ui.tableWidget_abonLich->item(row,2)->data(Qt::EditRole).toDouble() - ui.tableWidget_abonLich->item(row,4)->data(Qt::EditRole).toDouble();
			if (temp > 0)
				pokaznLich += temp;
			temp = ui.tableWidget_abonLich->item(row,5)->data(Qt::EditRole).toDouble();
			if (temp > 0)
				faktVykor += temp;
		}
	}
	
	ui.label_countAbonLich->setText("Кількість: "+QString::number(rowCount)+".\nСумарн.показн.: "+
									QString::number(pokaznLich, 'f', 3).replace('.',',')+"; сумарн.викор.: "+
									QString::number(faktVykor, 'f', 3).replace('.',',')+"; Всього: "+
									QString::number(pokaznLich+faktVykor, 'f', 3).replace('.',',')+".");
}
//------------------------------------------------------------
void tke_Form_insert_pokazn_lich_GV::curAbonentChanged()
{
	if (ui.tableWidget_abonLich->rowCount()>0){
		int abonLichId = ui.tableWidget_abonLich->item(ui.tableWidget_abonLich->currentRow(), 0)->data(Qt::EditRole).toInt();
		QSqlQuery query("SELECT Rahunok FROM lichylnykyAbon WHERE id="+sqlStr(abonLichId));
		query.seek(0);
		abonLichId = query.value(0).toInt();
		if (abonLichId>0)
			emit requestRahunok(abonLichId);
	}
}
//------------------------------------------------------------
void tke_Form_insert_pokazn_lich_GV::findAbonentPokazn(int Rahunok)
{
	QSqlQuery query("SELECT BudLich_ID, id FROM lichylnykyAbon WHERE Rahunok="+sqlStr(Rahunok));
	if (query.seek(0)){
		int budLichId = query.value(0).toInt();
		for (int row=0; row<ui.tableWidget_budLich->rowCount(); row++)
			if (ui.tableWidget_budLich->item(row,0)->data(Qt::EditRole).toInt() == budLichId){
				ui.tableWidget_budLich->setCurrentCell(row, 3);
				ui.tableWidget_budLich->scrollToItem(ui.tableWidget_budLich->item(row,3));
				int abonLichId = query.value(1).toInt();
				for (row=0; row<ui.tableWidget_abonLich->rowCount(); row++)
					if (ui.tableWidget_abonLich->item(row,0)->data(Qt::EditRole).toInt() == abonLichId){
						ui.tableWidget_abonLich->setFocus();
						ui.tableWidget_abonLich->setCurrentCell(row, 3);
						ui.tableWidget_abonLich->scrollToItem(ui.tableWidget_abonLich->item(row,3));
						return;
					}
			}
	}
	
	QMessageBox::information(0,
				"!!! Невдалий пошук показника лічильника !!!",
				"Програмі не вдалося знайти лічильник абонента з Рахунком: "+QString::number(Rahunok)+".",
				QMessageBox::Ok,
				QMessageBox::NoButton);
}
//------------------------------------------------------------
void tke_Form_insert_pokazn_lich_GV::toolButton_findRahunok_clicked()
{
	int rahunok = ui.lineEdit_rahunok->text().toInt();
	ui.lineEdit_rahunok->clear();
	if (rahunok == 0)
		return;
	findAbonentPokazn(rahunok);
}
//------------------------------------------------------------
void tke_Form_insert_pokazn_lich_GV::calcPokaznLich(int row)
{
	double factVykor=0;
	if (ui.tableWidget_abonLich->item(row,3)->data(Qt::EditRole).toDouble()>0 
			|| ui.tableWidget_abonLich->item(row,5)->data(Qt::EditRole).toDouble()>0){
		factVykor = ui.tableWidget_abonLich->item(row,3)->data(Qt::EditRole).toDouble()
			- ui.tableWidget_abonLich->item(row,2)->data(Qt::EditRole).toDouble()
			- ui.tableWidget_abonLich->item(row,4)->data(Qt::EditRole).toDouble()
			+ ui.tableWidget_abonLich->item(row,5)->data(Qt::EditRole).toDouble();
	}
	ui.tableWidget_abonLich->item(row,2)->setData(Qt::UserRole, factVykor);
}
//------------------------------------------------------------
tke_Form_insert_pokazn_lich_GV::~tke_Form_insert_pokazn_lich_GV()
{
	
}
//------------------------------------------------------------
void UFactSpojyvDelegate::paint(QPainter *painter,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
	QStyleOptionViewItemV3 opt = setOptions(index, option);
	
	QString str = "/" + QString::number(index.data(Qt::UserRole).toDouble()) + 
			"/ " + QString::number(index.data(Qt::DisplayRole).toDouble());
	drawBackground(painter, opt, index);
	drawDisplay(painter, opt, opt.rect, str.replace('.',','));
	drawFocus(painter, opt, opt.rect);
}
//--------------------utech--------------------utech--------------------utech--------------------
