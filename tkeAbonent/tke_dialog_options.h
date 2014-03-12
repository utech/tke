//------------------------------------------------------------
//
// Implamantation of tke_Dialog_narah class.
// Created by Roman Mulyk 26.06.2008
//
//------------------------------------------------------------

#ifndef TKE_DIALOG_OPTIONS_H
#define TKE_DIALOG_OPTIONS_H

#include <QDialog>
#include <QtDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>
#include <QTextCodec>
#include <QDir>
#include <QSettings>
#include <QButtonGroup>

#include "ui_tke_dialog_options.h"

class tke_dialog_options : public QDialog
{
        Q_OBJECT

    public:
        tke_dialog_options(QWidget *parent = 0);

		void connectEditres();
		void disconnectEditres();
		
		~tke_dialog_options();
    public slots:
		void populateOptions();
		void saveOptions();
		void pushButton_accept_clicked();
		void pushButton_cancel_clicked();
		void pushButton_zastosuvaty_clicked();
		void optionsEdited();
		void toolButton_pochatkoviZnachennya_clicked();
		void tabWidget_Options_currentChanged(int index);
		void toolButton_oldValues_clicked();
		void toolButton_findKvytScript_clicked();
		void toolButton_findSubsScript_clicked();
		void toolButton_arjDatabasePathFind_clicked();
		
	signals:
		void requestReloadIniSettings();

    private:
		Ui::tke_dialog_options ui;
		
		QButtonGroup *buttonGroup;
};

#endif
