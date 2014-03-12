//------------------------------------------------------------
//
// Declaration of tke_Form_uni_insert_kvytancii class.
// Created by Roman Mulyk 30.06.2008
//
//------------------------------------------------------------

#ifndef TKE_FORM_UNI_INSERT_KVYTANCII_H
#define TKE_FORM_UNI_INSERT_KVYTANCII_H

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
#include "ui_tke_Form_uni_insert_kvytancii.h"
#include <UStandardDelegate>
#include "UDbScriptAccess.h"
#include "uprint_docs.h"
#include <UPostupForm>

//------------------------------------------------------------
class tke_Form_uni_insert_kvytancii : public QWidget
{
	Q_OBJECT
	public:
		tke_Form_uni_insert_kvytancii(short year=2008, short month=3, QWidget * parent = 0);
		~tke_Form_uni_insert_kvytancii();
		
		Q_INVOKABLE int nextPackNumber();
	public slots:
		void populateBanksComboBox();
		void comboBox_vybirBanku_currentIndexChanged(int index);
		void treeView_fileMeneger_doubleClicked(const QModelIndex & index);
		
		void toolButton_addKvyt_clicked();
		void toolButton_dellKvyt_clicked();
		void toolButton_insertKvytToDB_clicked();
		void setPackNumbers();
		void deleteNulRahKvyt();
		void tableWidget_kvytancii_cellChanged(int row, int col);
		
		void toolButton_print_clicked();
		void toolButton_printOn_clicked();
		void tabWidget_kvyt_currentChanged(int index);
		
		void toolButton_reloadDirTree_clicked();
		
		void toolButton_reloadScript_clicked();
	signals:
		void requestRahunok(int);
		void requestDebugWindow();
	
	private:
		Ui::tke_Form_uni_insert_kvytancii ui;

		QDirModel *dirModel;
		short curYear;
		short curMonth;
		QDate curOplataDate;
		QString curKasaString;
		QList<bool> onePackInFileList;
		QList<QString> functionNamesList;
		int rowCount;
		UDbScriptAccess *dbfScript;
		QPrinter prnt;
};
//------------------------------------------------------------
#endif
