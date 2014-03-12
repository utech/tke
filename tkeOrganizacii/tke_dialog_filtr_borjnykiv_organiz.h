//------------------------------------------------------------
//
// Declaration of tkeDialogFiltrBorjnykivOrganiz class.
// Created by Roman Mulyk 13.11.2007
//
//------------------------------------------------------------

#ifndef TKE_DIALOG_FILTR_BORJNYKIV_ORGANIZ_H
#define TKE_DIALOG_FILTR_BORJNYKIV_ORGANIZ_H
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

#include "ui_tke_dialog_filtr_borjnykiv_organiz.h"
#include <QtDebug>

// Classes
  class tkeDialogFiltrBorjnykivOrganiz : public QDialog
    {
        Q_OBJECT

    public:
        tkeDialogFiltrBorjnykivOrganiz(QWidget *parent = 0);

		void setPochDate(QDate date);
		void setKincDate(QDate date);
		QDate pochDate();
		QDate kincDate();
		int curFinansFromId();
		bool neadMinSum();
		double minSum();
		void setMinDate(QDate date);
		void setMaxDate(QDate date);		
		
		~tkeDialogFiltrBorjnykivOrganiz();
    public slots:
		void tryAccept();
	
    private:
		Ui::tke_Dialog_filtr_borjnykiv_organiz ui;
		QHash<int,int> finasHash;
};
#endif
