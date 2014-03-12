//------------------------------------------------------------
//
// Implamantation of UCalcDefValuesThread class.
// Created by Roman Mulyk 10.10.2007
//
//------------------------------------------------------------

#ifndef UCALC_DEF_VALUES_THREAD_H
#define UCALC_DEF_VALUES_THREAD_H
// QT standart includes
#include <QTableWidget>
#include <QThread>
#include <QSqlQuery>
#include <QDate>
#include <QHash>

#include "../tkeAbonent/tke_math.h"
#include "../src_other/replaceStrings/UReplaceStringsParser.h"
#include <QtDebug>

class UCalcDefValuesThread : public QThread
{
	Q_OBJECT
	public:
		UCalcDefValuesThread(QObject *parent = 0);
		void run();
		void setTableWidget(QTableWidget *tw);
		void setSpecRahunok(int rah);
		void setYear(int y);
	signals:
		void changeCell(int row, int col, double val);
	private:
		int specRahunok;
		int year;
		QTableWidget *tableWidget;
};

#endif
