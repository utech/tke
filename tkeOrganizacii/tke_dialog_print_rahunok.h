//------------------------------------------------------------
//
// Declaration of tkeDialogPrintRahunok class.
// Created by Roman Mulyk 26.10.2007
//
//------------------------------------------------------------

#ifndef TKE_DIALOG_PRINT_RAHUNOK_H
#define TKE_DIALOG_PRINT_RAHUNOK_H
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

#include "ui_tke_dialog_print_rahunok.h"
#include <QtDebug>

// Classes
  class tkeDialogPrintRahunok : public QDialog
    {
        Q_OBJECT

    public:
        tkeDialogPrintRahunok(short year=2008, short month=10,QWidget *parent = 0);
		
		double suma();
		int rahunokType();
		int year();
		int month();
		
		~tkeDialogPrintRahunok();
    public slots:
		void rahTypeChanged(int index);
	
    private:
		Ui::tke_Dialog_print_rahunok ui;
};
#endif
