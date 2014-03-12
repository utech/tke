//------------------------------------------------------------
//
// Implamantation of tke_Dialog_narah class.
// Created by Roman Mulyk 12.09.2006
//
//------------------------------------------------------------

#ifndef TKE_DIALOG_NARAH_H
#define TKE_DIALOG_NARAH_H

#include <QDialog>
#include <QDate>
#include <QtDebug>

#include "ui_tke_dialog_narah.h"

class tke_Dialog_narah : public QDialog
{
        Q_OBJECT

    public:
        tke_Dialog_narah(QWidget *parent = 0, QString title="", short year=2006, short month=10);
		
		int get_Month();
		int get_Year();
		
		~tke_Dialog_narah();
    public slots:


    private:
		Ui::tke_Dialog_narah ui;
		
		QDate date;
};

#endif
