//------------------------------------------------------------
//
// Implamantation of tke_Dialog_narah class.
// Created by Roman Mulyk 12.09.2006
//
//------------------------------------------------------------

#ifndef TKE_DIALOG_LOGIN_H
#define TKE_DIALOG_LOGIN_H

#include <QDialog>
#include <QtDebug>
#include <QSqlQuery>
#include <QMessageBox>
#include <QTextCodec>

#include "ui_tke_dialog_login.h"

class tke_Dialog_login : public QDialog
{
        Q_OBJECT

    public:
        tke_Dialog_login(QWidget *parent = 0);
		
		bool validPassword();
		int dialogResult();
		int userId();
		QString userLogin();
		
		~tke_Dialog_login();
    public slots:
		void passwordEdited( const QString & pswrdString);
		void okButton_login_clicked();
		void pushButton_change_password_clicked();
    private:
		Ui::tke_Dialog_login ui;
		
		QSqlQuery loginQuery;
		int i;
		QTextCodec *codec;
		int group_sys_id, dRes, user_Id;
};

#endif
