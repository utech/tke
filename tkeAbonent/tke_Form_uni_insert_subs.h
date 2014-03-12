//------------------------------------------------------------
//
// Declaration of tke_Form_uni_insert_subs class.
// Created by Roman Mulyk 08.07.2008
//
//------------------------------------------------------------

#ifndef TKE_FORM_UNI_INSERT_SUBS_H
#define TKE_FORM_UNI_INSERT_SUBS_H

#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QDirModel>
#include <QMessageBox>
#include <QTextCodec>
#include <QtSql>
#include <QVariant>
#include <QSettings>
#include <QFile>
#include <QScriptEngine>
#include <QScriptValue>
#include <QHash>
#include <QScriptValueIterator>
#include <QTextDocument>
#include <QPrintDialog>
#include <QPrinter>
#include <QTextBlockFormat>
#include <QTextTableCell>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextTable>


#include "../tkeOrganizacii/udiff_func.h"
#include "ui_tke_Form_uni_insert_subs.h"
#include <UStandardDelegate>
#include "UDbScriptAccess.h"
#include "uprint_docs.h"
#include <UPostupForm>

//------------------------------------------------------------
class tke_Form_uni_insert_subs : public QWidget
{
	Q_OBJECT
	public:
		tke_Form_uni_insert_subs(short year=2008, short month=3, QWidget * parent = 0);
		~tke_Form_uni_insert_subs();
		
	public slots:
		void treeView_fileMeneger_doubleClicked(const QModelIndex & index);
		
		void toolButton_addSubs_clicked();
		void toolButton_dellSubs_clicked();
		void toolButton_insertSubsToDB_clicked();
		void tableWidget_subsydii_cellChanged(int row, int col);
		
		void toolButton_reloadDirTree_clicked();
		
		void toolButton_reloadScript_clicked();
	signals:
		void requestRahunok(int);
		void requestDebugWindow();
	
	private:
		Ui::tke_Form_uni_insert_subs ui;

		QDirModel *dirModel;
		QString subsInsertFunctionName;
		int rowCount;
		UDbScriptAccess *dbfScript;
};
//------------------------------------------------------------
#endif
