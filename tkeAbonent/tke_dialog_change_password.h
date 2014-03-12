//------------------------------------------------------------
//
// Implamantation of tke_Dialog_narah class.
// Created by Roman Mulyk 12.09.2006
//
//------------------------------------------------------------

#ifndef TKE_DIALOG_CHANGE_PASSWORD_H
#define TKE_DIALOG_CHANGE_PASSWORD_H

#include <QDialog>
#include <QtDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>
#include <QTextCodec>

#include "ui_tke_dialog_change_password.h"

class tke_Dialog_change_password : public QDialog
{
        Q_OBJECT

    public:
        tke_Dialog_change_password(QWidget *parent = 0, QString title="", int user_id=0);
		
		bool new_password_is_valid();
		QString newPassword();
		
		~tke_Dialog_change_password();
    public slots:
		void okButton_login_clicked();

    private:
		Ui::tke_Dialog_change_password ui;
		QSqlQuery query;
		QTextCodec *codec;
		int usId;

};

#endif
