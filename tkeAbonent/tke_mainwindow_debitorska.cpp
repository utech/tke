//------------------------------------------------------------
//
// Implamantation of tke_MainWindow class.
// Created by Roman Mulyk 29.06.2009
//
//------------------------------------------------------------

#include "tke_mainwindow.h"
#include <UStandardDelegate>
#include <USqlAccessible>
#include "tke_dialog_narah.h"

//------------------------------------------------------------
void tke_MainWindow::initBeznadiynaDebitorskaZaborgTable()
{
	ui.tableWidget_debZaborg->setItemDelegateForColumn(5, new UDateEditDelegate(QDate(2000,1,1), QDate(2100,12,31), ui.tableWidget_debZaborg));
	ui.tableWidget_debZaborg->setItemDelegateForColumn(6, new UDoubleSpinBoxDelegate(-1000000, 1000000, 2, ui.tableWidget_debZaborg));
	
	for (int col=0; col<ui.tableWidget_debZaborg->columnCount(); col++){
		for (int row=0; row<ui.tableWidget_debZaborg->rowCount(); row++){
			QTableWidgetItem *tableWidgetItem = new QTableWidgetItem();
			tableWidgetItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			if (col==6 || col==2)
				tableWidgetItem->setFlags(tableWidgetItem->flags() & (~Qt::ItemIsEditable));
			ui.tableWidget_debZaborg->setItem(row, col, tableWidgetItem );
		}
		ui.tableWidget_debZaborg->setColumnWidth(col,100);
	}
	ui.tableWidget_debZaborg->setColumnHidden(0,true);
	ui.tableWidget_debZaborg->resizeRowsToContents();
	ui.tableWidget_debZaborg->verticalHeader()->setDefaultAlignment(Qt::AlignRight);
	clearBeznadiynaDebitorskaZaborgTable();
	
	connect(ui.tableWidget_debZaborg, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(beznadiynaDebitorskaZaborgTableWidgetItemChanged(QTableWidgetItem *)));
}
//------------------------------------------------------------
void tke_MainWindow::clearBeznadiynaDebitorskaZaborgTable()
{	
	for (int col=0; col<ui.tableWidget_debZaborg->columnCount(); col++)
		for (int row=0; row<ui.tableWidget_debZaborg->rowCount(); row++)
			ui.tableWidget_debZaborg->item(row, col)->setText("-");
}
//------------------------------------------------------------
void tke_MainWindow::populateBeznadiynaDebitorskaZaborgTable()
{
	disconnect(ui.tableWidget_debZaborg, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(beznadiynaDebitorskaZaborgTableWidgetItemChanged(QTableWidgetItem *)));
	
	clearBeznadiynaDebitorskaZaborgTable();
	
	QSqlQuery query;
	query.exec("SELECT month(CDate), id, Vh_saldo, ZaborgAdd, OplataSuma, OplataDate \
				FROM debitorskaZaborg \
				WHERE Rahunok="+ui.spinBox_Rahunok->text()+" \
					and Year(CDate)="+ui.spinBox_year_filtr->text());
	while (query.next()){
		int row = query.value(0).toInt()-1;
		ui.tableWidget_debZaborg->item(row, 0)->setText(query.value(1).toString());
		ui.tableWidget_debZaborg->item(row, 1)->setText(uMToStr2(query.value(2).toDouble()));
		ui.tableWidget_debZaborg->item(row, 3)->setText(uMToStr2(query.value(3).toDouble()));
		ui.tableWidget_debZaborg->item(row, 4)->setText(uMToStr2(query.value(4).toDouble()));
		ui.tableWidget_debZaborg->item(row, 5)->setData(Qt::EditRole, query.value(5).toDate());
	}
	
	query.exec("SELECT month(CDate), sum(Suma) FROM narahuvannyaPereved \
				WHERE Rahunok_id="+ui.spinBox_Rahunok->text()+" \
					and year(CDate)="+ui.spinBox_year_filtr->text()+" \
					and PerevedType_id=2 \
				GROUP BY month(CDate)");
	while (query.next())
		ui.tableWidget_debZaborg->item(query.value(0).toInt()-1, 2)->setText(uMToStr2(-query.value(1).toDouble()));
	
	calcBeznadiynaDebitorskaZaborgTable();
	showBeznDebitZabordFlag();
	
	connect(ui.tableWidget_debZaborg, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(beznadiynaDebitorskaZaborgTableWidgetItemChanged(QTableWidgetItem *)));
}
//------------------------------------------------------------
void tke_MainWindow::calcBeznadiynaDebitorskaZaborgTable()
{
	double sum = 0;
	for (int col=2; col<ui.tableWidget_debZaborg->columnCount()-2; col++){
		sum = 0;
		for (int row=0; row<ui.tableWidget_debZaborg->rowCount()-1; row++)
			sum += ui.tableWidget_debZaborg->item(row, col)->data(Qt::EditRole).toDouble();
		ui.tableWidget_debZaborg->item(12,col)->setData(Qt::EditRole, sum);
	}
	
	for (int row=0; row<12; row++){
		sum = ui.tableWidget_debZaborg->item(row, 1)->data(Qt::EditRole).toDouble();
		sum += ui.tableWidget_debZaborg->item(row, 2)->data(Qt::EditRole).toDouble();
		sum += ui.tableWidget_debZaborg->item(row, 3)->data(Qt::EditRole).toDouble();
		sum -= ui.tableWidget_debZaborg->item(row, 4)->data(Qt::EditRole).toDouble();
		
		ui.tableWidget_debZaborg->item(row,6)->setData(Qt::EditRole, sum);
	}
}
//------------------------------------------------------------
void tke_MainWindow::beznadiynaDebitorskaZaborgTableWidgetItemChanged(QTableWidgetItem * item)
{
	qDebug() << "void tke_MainWindow::beznadiynaDebitorskaZaborgTableWidgetItemChanged(QTableWidgetItem * item)";
	disconnect(ui.tableWidget_debZaborg, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(beznadiynaDebitorskaZaborgTableWidgetItemChanged(QTableWidgetItem *)));
	
	int row = item->row();
	int id = ui.tableWidget_debZaborg->item(row, 0)->data(Qt::EditRole).toInt();
	QSqlQuery query;
	QString str;
	QDate date = ui.tableWidget_debZaborg->item(row, 5)->data(Qt::EditRole).toDate();
	if (date.isValid())
		str = " '"+date.toString("yyyy-MM-dd")+"' ";
	else
		str = " null ";
	//qDebug() << str;
	if (id > 0){
		query.exec("UPDATE debitorskaZaborg SET \
						Vh_saldo="+sqlStr(ui.tableWidget_debZaborg->item(row, 1)->data(Qt::EditRole).toDouble())+", \
						ZaborgAdd="+sqlStr(ui.tableWidget_debZaborg->item(row, 3)->data(Qt::EditRole).toDouble())+", \
						OplataSuma="+sqlStr(ui.tableWidget_debZaborg->item(row, 4)->data(Qt::EditRole).toDouble())+", \
						OplataDate="+str+" \
					WHERE id="+QString::number(id));
	}
	else
		query.exec("INSERT INTO debitorskaZaborg (id, Rahunok, CDate, \
						Vh_saldo, ZaborgAdd, OplataSuma, OplataDate) \
					VALUES \
						("+QString::number(nextLichylnyk("debitorskaZaborg"))+", "+ui.spinBox_Rahunok->text()+", \
						'"+QDate(ui.spinBox_year_filtr->value(),row+1,1).toString("yyyy-MM-dd")+"', \
						"+sqlStr(ui.tableWidget_debZaborg->item(row, 1)->data(Qt::EditRole).toDouble())+", \
						"+sqlStr(ui.tableWidget_debZaborg->item(row, 3)->data(Qt::EditRole).toDouble())+", \
						"+sqlStr(ui.tableWidget_debZaborg->item(row, 4)->data(Qt::EditRole).toDouble())+", \
						"+str+")");
	//qDebug() << query.lastQuery();
	populateBeznadiynaDebitorskaZaborgTable();
}
//------------------------------------------------------------
void tke_MainWindow::action_calcDebitorskaZaborgVhSaldo_activated()
{
	tke_Dialog_narah * d = new tke_Dialog_narah(this, "Розрахунок вхідного сальдо по безнадійній дебюторській заборгованості", currentSysDate.year(), currentSysDate.month());
	if (d->exec()){
		UPostupForm * pForm = new UPostupForm(0,0);
		pForm->show();
		calcDebVhSaldoInMonth(d->get_Year(), d->get_Month(), pForm);
		delete pForm;
	}
	delete d;
	populateBeznadiynaDebitorskaZaborgTable();
}
//------------------------------------------------------------
void tke_MainWindow::calcDebVhSaldoInMonth(short year, short month, UPostupForm * pForm)
{
	QSqlQuery query, updQuery;
	
	QDate cDate(year, month, 1);
	QDate prevDate(year, month, 1);
	prevDate = prevDate.addMonths(-1);
	query.exec("SELECT count(*) FROM (SELECT Rahunok FROM debitorskaZaborg GROUP BY Rahunok) rahCount");
	query.next();
	pForm->setCurrentPos(0);
	pForm->setNumPos(query.value(0).toInt());
	query.exec("SELECT Rah FROM \
					(SELECT Rahunok AS Rah FROM debitorskaZaborg GROUP BY Rahunok \
					UNION \
					SELECT Rahunok_id AS Rah FROM narahuvannyaPereved \
					WHERE year(CDate)="+sqlStr(year)+" \
						and month(CDate)="+sqlStr(month)+" \
						and PerevedType_id=2 \
					UNION \
					SELECT Rahunok_id AS Rah FROM narahuvannyaPereved \
					WHERE year(CDate)="+sqlStr(prevDate.year())+" \
						and month(CDate)="+sqlStr(prevDate.month())+" \
						and PerevedType_id=2 \
					) \
				GROUP BY Rah \
				ORDER BY Rah");
	while (query.next()){
		//qDebug() << "rah: " << query.value(0).toInt();
		updQuery.exec("SELECT Vh_saldo, ZaborgAdd, OplataSuma, CDate FROM debitorskaZaborg \
						WHERE CDate < cDate('"+cDate.toString("yyyy-MM-dd")+"') and Rahunok="+query.value(0).toString()+" \
						ORDER BY CDate DESC");
		updQuery.next();
		double saldo = uDToM(updQuery.value(0).toDouble() + updQuery.value(1).toDouble() - updQuery.value(2).toDouble());
		QDate lastDate = query.value(3).toDate();
		if (!lastDate.isValid())
			lastDate = prevDate;
		
		updQuery.exec("SELECT sum(Suma) FROM narahuvannyaPereved \
				WHERE Rahunok_id="+query.value(0).toString()+" \
					and (CDate BETWEEN cdate("+sqlStr(lastDate)+") AND cdate("+sqlStr(prevDate)+")) \
					and PerevedType_id=2");
		//qDebug() << "q: " << updQuery.lastQuery();
		if (updQuery.next())
			saldo -= uDToM(updQuery.value(0).toDouble());
		
		updQuery.exec("SELECT id FROM debitorskaZaborg \
						WHERE year(CDate)="+QString::number(year)+" and month(CDate)="+QString::number(month)+" \
							and Rahunok="+query.value(0).toString());
		updQuery.next();
		int id = updQuery.value(0).toInt();
		if (id > 0)
			updQuery.exec("UPDATE debitorskaZaborg SET Vh_saldo="+uMToStr2Krapka(saldo)+" \
							WHERE id="+QString::number(id));
		else if (saldo != 0)
			updQuery.exec("INSERT INTO debitorskaZaborg (id, Rahunok, CDate, \
								Vh_saldo, ZaborgAdd, OplataSuma) \
							VALUES \
								("+QString::number(nextLichylnyk("debitorskaZaborg"))+", "+query.value(0).toString()+", \
								'"+QDate(year,month,1).toString("yyyy-MM-dd")+"', \
								"+uMToStr2Krapka(saldo)+", \
								0, \
								0)");
		pForm->inc_pos();
	}
}
//------------------------------------------------------------
void tke_MainWindow::showBeznDebitZabordFlag()
{
	QSqlQuery query;
	query.exec("SELECT Vh_saldo, ZaborgAdd, OplataSuma FROM debitorskaZaborg \
						WHERE CDate <= cDate('"+QDate(currentSysDate.year(), currentSysDate.month(), currentSysDate.daysInMonth()).toString("yyyy-MM-dd")+"') and Rahunok="+ui.spinBox_Rahunok->text()+" \
						ORDER BY CDate DESC");
	query.next();
	ui.tabWidget_monthlyOperacii->setIconSize(QSize(16,16));
	if (0 < uDToM(query.value(0).toDouble() + query.value(1).toDouble() - query.value(2).toDouble())){
		ui.tabWidget_monthlyOperacii->setTabIcon(1, QIcon(":/pictures/pictures/finances_section.png"));
	}
	else {
		ui.tabWidget_monthlyOperacii->setTabIcon(1, QIcon(":/pictures/pictures/button_cancel.png"));
	}
}
//------------------------------------------------------------
