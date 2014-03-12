//------------------------------------------------------------
//
// Implamantation of tke_Dialog_saveDataBase class.
// Created by Roman Mulyk 17.04.2008
//
//------------------------------------------------------------

#ifndef TKE_DIALOG_SAVEDATABASE_H
#define TKE_DIALOG_SAVEDATABASE_H

#include <QDialog>
#include <QTimer>

#include "ui_tke_Dialog_saveDataBase.h"

class tke_Dialog_saveDataBase : public QDialog
{
        Q_OBJECT

    public:
        tke_Dialog_saveDataBase(QWidget *parent = 0);
		~tke_Dialog_saveDataBase();
    public slots:
		void showTimeInfo();
		void dAccept();
		void dReject();

    private:
		Ui::tke_Dialog_saveDataBase ui;
		int iter;
		QTimer *timer;
};

#endif
