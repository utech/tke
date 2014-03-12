//------------------------------------------------------------
//
// Declaration of tke_dialog_print_nakladna class.
// Created by Roman Mulyk 26.10.2007
//
//------------------------------------------------------------

#ifndef tke_dialog_print_nakladna_H
#define tke_dialog_print_nakladna_H
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

#include "ui_tke_dialog_print_nakladna.h"
#include <QtDebug>

// Classes
  class tke_dialog_print_nakladna : public QDialog
    {
        Q_OBJECT

    public:
        tke_dialog_print_nakladna(short year=2008, short month=10,QWidget *parent = 0);
		
		double sumaCO();
		double sumaGV();
		int year();
		int month();
		
		~tke_dialog_print_nakladna();
    public slots:
		void calcSum();
		void readCoGvSum();
	
    private:
		Ui::tke_dialog_print_nakladna ui;
};
#endif
