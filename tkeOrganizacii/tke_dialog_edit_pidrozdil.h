//------------------------------------------------------------
//
// Implamantation of tke_MainWindow class.
// Created by Roman Mulyk 25.02.2007
//
//------------------------------------------------------------

#ifndef TKE_DIALOG_EDIT_PIDROZDIL_H
#define TKE_DIALOG_EDIT_PIDROZDIL_H
// QT standart includes
#include <QDialog>
#include <QTSql>
#include <QHeaderView>
#include <QList>
#include <QDate>
#include <QVector>
#include <QTextCodec>
#include <QMessageBox>
#include <QSqlError>

#include "ui_tke_dialog_edit_pidrozdil.h"
#include <QtDebug>

// Classes
  class tkeDialogEditPidrozdil : public QDialog
    {
        Q_OBJECT

    public:
        tkeDialogEditPidrozdil(QWidget *parent = 0, int pidrozdilId = 0);
		
		~tkeDialogEditPidrozdil();
    public slots:
		void recordChanges();
		void acceptAndSave();
	
    private:
		Ui::tke_Dialog_edit_pidrozdily ui;
		
		QTextCodec *codec;
		
		QList<int> kotelniList;
		int idPidrozdil;

};
#endif
