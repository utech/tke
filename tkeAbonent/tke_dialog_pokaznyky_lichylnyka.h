//------------------------------------------------------------
//
// Implamantation of tke_Dialog_pokaznyky_lichylnyka.
// Created by Roman Mulyk 18.06.2007
//
//------------------------------------------------------------

#ifndef TKE_DIALOG_POKAZNYKY_LICHYKNYKA_H
#define TKE_DIALOG_POKAZNYKY_LICHYKNYKA_H
// QT standart includes#include <QDialog>
#include <QtDebug>
#include <QMessageBox>

#include "ui_tke_dialog_pokaznyky_lichylnyka.h"

class tke_Dialog_pokaznyky_lichylnyka : public QDialog
{
        Q_OBJECT

    public:
        tke_Dialog_pokaznyky_lichylnyka(QWidget *parent = 0);
		
		double pochZnachLich();
		double kincZnachLich();

		
		~tke_Dialog_pokaznyky_lichylnyka();
    public slots:
		void setPochZnachLich(double znachennya);
		void setKincZnachLich(double znachennya);
		void valueEdited();
	
    private:
		Ui::tke_form_pokaznyky_lichylnyka ui;
};

#endif
