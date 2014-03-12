//------------------------------------------------------------
//
// Implamantation of tke_Dialog_narah class.
// Created by Roman Mulyk 26.11.2006
//
//------------------------------------------------------------

#ifndef TKE_FORM_INSERT_OPSUBS_H
#define TKE_FORM_INSERT_OPSUBS_H

#include <QWidget>
#include <QDate>
#include <QSqlQuery>
#include <QTextCursor>
#include <QDirModel>
#include <QMessageBox>

#include "ui_tke_Form_insert_opSubs.h"

class tke_Form_insert_opSubs : public QWidget
{
        Q_OBJECT

    public:
        tke_Form_insert_opSubs(QDate dt, QWidget *parent = 0);
		
		static bool insertOpRowToDatabase(QDate dt, int rahunok, double opCo, double opGv);
		
		~tke_Form_insert_opSubs();
    public slots:
		void pushButton_add_clicked();
		void pushButton_del_clicked();
		void pushButton_insertOpSubs_clicked();
		void currentRowChanged();
		void readOpSubsFromFile();
	signals:
        void u_abonent_rahunok_changes(int rahunok);
    private:
		Ui::tke_Form_insert_opSubs ui;
		QDirModel *dirModel;
};

#endif
