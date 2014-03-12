//------------------------------------------------------------
//
// Implamantation of tke_Dialog_narah class.
// Created by Roman Mulyk 26.06.2008
//
//------------------------------------------------------------

#ifndef TKE_DIALOG_EDITOPERACIISTRINGS_H
#define TKE_DIALOG_EDITOPERACIISTRINGS_H
// QT standart includes
#include <QDialog>
#include <QtDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>
#include <QTextCodec>
#include <QDir>
#include <QSettings>
#include <QTableWidgetItem>

#include "ui_tke_Dialog_editOperaciiStrings.h"
#include <UStandardDelegate>
#include "../tkeOrganizacii/udiff_func.h"

class tke_Dialog_editOperaciiStrings : public QDialog
{
        Q_OBJECT

    public:
        tke_Dialog_editOperaciiStrings(int abonentRah=0, short year=2008, short month=10, QWidget *parent = 0);

		~tke_Dialog_editOperaciiStrings();
    public slots:
		void populateNarahStringsTable();
		void populateNarahStringsRow(int row, QSqlRecord record);
		void tableWidget_narahStrings_cellChanged(int row, int col);
		void insertNarahString();
		void deleteNarahString();
		void setColumns();
		
	signals:
		void requestReloadNarahuvannya();

    private:
		Ui::tke_Dialog_editOperaciiStrings ui;
		
		int rahunok;
		QTableWidgetItem *item;
		int colCount;
		QStringList columnsList;
};

#endif
