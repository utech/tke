//------------------------------------------------------------
//
// Declaration of UCalcDefValuesThread class.
// Created by Roman Mulyk 10.10.2007
//
//------------------------------------------------------------
#include <QLabel>

#include "ucalc_def_values_thread.h"
//------------------------------------------------------------
UCalcDefValuesThread::UCalcDefValuesThread(QObject *parent)
		: QThread(parent)
{
	
} 
//------------------------------------------------------------
void UCalcDefValuesThread::run()
{
	QHash<QString, QString> hash;
	QSqlQuery query;
	
	UReplaceStringParser strPaser;
	strPaser.setBeginTagChar('[');
	strPaser.setEndTagChar(']');
	strPaser.setDelWrongTag(true);
	
	query.exec("SELECT Narah_def_query, Oplata_def_query FROM slujb_rahunky WHERE Rahunok="+QVariant(specRahunok).toString());
	query.seek(0);
	QString defOplataQuery, defNarahQuery;
	bool takeDefOplata, takeDefNarah;
	defNarahQuery = query.value(0).toString();
	defOplataQuery = query.value(1).toString();
	
	if (defNarahQuery.size()<8)
		takeDefNarah = false;
	else
		takeDefNarah = true;
	if (defOplataQuery.size()<8)
		takeDefOplata = false;
	else
		takeDefOplata = true;
	for (int table_month=12; table_month>0; table_month--){
		bool ok;
		tableWidget->item(table_month-1,1)->text().toDouble(&ok);
		if (takeDefNarah && ok){
			hash.clear();
			hash.insert( "month", QVariant(table_month).toString() );
			hash.insert( "year", QVariant(year).toString() );
			QDate xDate(year, table_month, 1);
			hash.insert( "beginMonthDate", xDate.toString("dd.MM.yyyy") );
			hash.insert( "endMonthDate", QDate(year, table_month, xDate.daysInMonth()).toString("dd.MM.yyyy") );
			hash.insert( "rahunok_ID", QVariant(specRahunok).toString() );
			strPaser.setInputString(defNarahQuery);
			strPaser.setTagsStringsHash(hash);
			strPaser.convertString();
			query.exec(strPaser.curOutputString());
			if (query.seek(0)){
				tableWidget->blockSignals(true);
				emit changeCell(table_month-1, 2, query.value(0).toDouble());
				tableWidget->blockSignals(false);
			}
		}
		if (takeDefOplata && ok){
			hash.clear();
			hash.insert( "month", QVariant(table_month).toString() );
			hash.insert( "year", QVariant(year).toString() );
			QDate xDate(year, table_month, 1);
			hash.insert( "beginMonthDate", xDate.toString("dd.MM.yyyy") );
			hash.insert( "endMonthDate", QDate(year, table_month, xDate.daysInMonth()).toString("dd.MM.yyyy") );
			hash.insert( "rahunok_ID", QVariant(specRahunok).toString() );
			strPaser.setInputString(defOplataQuery);
			strPaser.setTagsStringsHash(hash);
			strPaser.convertString();
			query.exec(strPaser.curOutputString());
			if (query.seek(0)){
				tableWidget->blockSignals(true);
				emit changeCell(table_month-1, 4, query.value(0).toDouble());
				tableWidget->blockSignals(false);
			}
		}
	}
}
//------------------------------------------------------------
void UCalcDefValuesThread::setTableWidget(QTableWidget *tw)
{
	tableWidget	= tw;
}
//------------------------------------------------------------
void UCalcDefValuesThread::setSpecRahunok(int rah)
{
	specRahunok = rah;
}
//------------------------------------------------------------
void UCalcDefValuesThread::setYear(int y)
{
	year = y;
}
//------------------------------------------------------------
