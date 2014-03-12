//------------------------------------------------------------
//
// Declaration of tkeDialogDateInterval class.
// Created by Roman Mulyk 21.11.2007
//
//------------------------------------------------------------

#ifndef TKE_DIALOG_DATE_INTERVAL_H
#define TKE_DIALOG_DATE_INTERVAL_H
// QT standart includes
#include <QDialog>
#include <QTSql>
#include <QHeaderView>
#include <QList>
#include <QDate>
#include <QVector>
#include <QTextCodec>
#include <QMessageBox>
#include <QSqlError>
#include <QDate>

#include "ui_tke_dialog_date_interval.h"
#include <QtDebug>

// Classes
  class tkeDialogDateInterval : public QDialog
    {
        Q_OBJECT

    public:
        tkeDialogDateInterval(QWidget *parent = 0);

		void setPochDate(QDate date);
		void setKincDate(QDate date);
		QDate pochDate();
		QDate kincDate();
		void setMinDate(QDate date);
		void setMaxDate(QDate date);		
		
		~tkeDialogDateInterval();
    public slots:
		void tryAccept();
	
    private:
		Ui::tkeDialogDateInterval ui;
};
#endif
