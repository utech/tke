//------------------------------------------------------------
//
// Declaration of tkeFormDaysOpal class.
// Created by Roman Mulyk 15.03.2007
//
//------------------------------------------------------------

#ifndef TKE_FORM_DAYS_OPAL_H
#define TKE_FORM_DAYS_OPAL_H
// QT standart includes
#include <QTSql>
#include <QHeaderView>
#include <QDate>
#include <QTextCodec>
#include <QMessageBox>

#include "ui_tke_form_days_opal.h"
#include "udni_opal_delegate.h"
#include "udiff_func.h"
#include <QtDebug>

// Classes
  class tkeFormDaysOpal : public QWidget
    {
        Q_OBJECT

    public:
        tkeFormDaysOpal(QWidget *parent = 0, QSqlQueryModel *sqlQueryModel = new QSqlQueryModel(), 
						QTableView *tableView = new QTableView(), int year=2000, int month=1);
		
		QSqlRelationalTableModel* dniOpalTableModel();
		void setChangePidrozdilButtons(bool active);
		
		~tkeFormDaysOpal();
    public slots:
		void populateTableDniOpal();
		void pushButton_org_prev_clicked();
		void pushButton_org_next_clicked();
		void pushButton_days_opal_lich_clicked();
		void actionAddString_activated();
		void actionDelString_activated();
		void tableModel_primeInsert(int row, QSqlRecord & sqlRecord);
		void tableModel_selectionModel_currentChanged();
    private:
		Ui::tkeFormDaysOpal ui;
		
		QSqlQueryModel *usqlQueryModel;
		QSqlRelationalTableModel *tableModel;
		QTableView *utableView;
		QModelIndex curIndex;
		
		int curYear;
		int curMonth;
		int pidrozdilyCount;
		unsigned char days_in_month[12];
		
		bool dniOpalEdited;
		
		QTextCodec *codec;
		
		//default values
		short d_OpalLich_Nolich;
};
#endif
