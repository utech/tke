//------------------------------------------------------------
//
// Declaration of tkeFormDaysOpalSimple class.
// Created by Roman Mulyk 16.11.2007
//
//------------------------------------------------------------

#ifndef TKE_FORM_DAYS_OPAL_SIMPLE_H
#define TKE_FORM_DAYS_OPAL_SIMPLE_H
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
  class tkeFormDaysOpalSimple : public QWidget
    {
        Q_OBJECT

    public:
        tkeFormDaysOpalSimple(QWidget *parent=0, int pidrozdilId=1, int year=2007, int month=10, QString pidrozdilName="");
		
		QSqlRelationalTableModel* dniOpalTableModel();
		
		~tkeFormDaysOpalSimple();
    public slots:
		void populateTableDniOpal();
		void pushButton_days_opal_lich_clicked();
		void actionAddString_activated();
		void actionDelString_activated();
		void tableModel_primeInsert(int row, QSqlRecord & sqlRecord);
		void tableModel_selectionModel_currentChanged();
    private:
		Ui::tkeFormDaysOpal ui;

		QSqlRelationalTableModel *tableModel;

		
		int pidrId;
		int curYear;
		int curMonth;
		unsigned char days_in_month[12];
		
		bool dniOpalEdited;
		
		QTextCodec *codec;
		
		//default values
		short d_OpalLich_Nolich;
};
#endif
