//------------------------------------------------------------
//
// Implamantation of tke_Dialog_printNadannyaPoslugPoBudTaKot class.
// Created by Roman Mulyk 12.09.2006
//
//------------------------------------------------------------

#ifndef tke_Dialog_printNadannyaPoslugPoBudTaKot_H
#define tke_Dialog_printNadannyaPoslugPoBudTaKot_H

#include <QDialog>
#include <QDate>
#include <QtDebug>

#include "ui_tke_Dialog_printNadannyaPoslugPoBudTaKot.h"

class tke_Dialog_printNadannyaPoslugPoBudTaKot : public QDialog
{
        Q_OBJECT

    public:
        tke_Dialog_printNadannyaPoslugPoBudTaKot(QWidget *parent = 0, QString title="");
		
		int kotelnyaId();
		int budynokId();
		
		~tke_Dialog_printNadannyaPoslugPoBudTaKot();
    public slots:
		void populateBudynkyComboBox();
    private:
		Ui::tke_Dialog_printNadannyaPoslugPoBudTaKot ui;
		
};

#endif
