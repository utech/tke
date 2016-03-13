//------------------------------------------------------------
//
// Implamantation of tke_Dialog_narah class.
// Created by Roman Mulyk 12.09.2006
//
//------------------------------------------------------------

#ifndef TKE_DIALOG_FILTR_BORJNYKIV_H
#define TKE_DIALOG_FILTR_BORJNYKIV_H

#include <QDialog>
#include <QtDebug>
#include <QSqlQuery>
#include <QDate>
#include <QTextCodec>

#include "ui_tke_dialog_filtr_borjnykiv.h"

class tke_Dialog_filtr_borjnykiv : public QDialog
{
        Q_OBJECT

    public:
        tke_Dialog_filtr_borjnykiv(QString, QString, QWidget *parent = 0);
		
		int get_Bud_id();
		QDate getPochDate();
		QDate getKincDate();
		void setPochDate(QDate date);
		void setKincDate(QDate date);
		double minBorg();
		unsigned char abonType();
		void setAbonType(unsigned char type);
		
		~tke_Dialog_filtr_borjnykiv();
    public slots:


    private:
		Ui::tke_Dialog_filtr_borjnykiv ui;
		
		QList<int> budList;
};

#endif
