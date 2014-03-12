//------------------------------------------------------------
//
// Implamantation of tke_Dialog_narah class.
// Created by Roman Mulyk 26.11.2006
//
//------------------------------------------------------------

#ifndef TKE_FORM_INSERT_KVYT_DBF_OB_H
#define TKE_FORM_INSERT_KVYT_DBF_OB_H

#include <QDialog>
#include <QDate>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QTextCodec>
#include <QTextCursor>
#include <QTextTable>
#include <QStandardItemModel>
#include <QStringList>
#include <QDir>
#include <QMessageBox>
#include <QProcess>
#include <QPrinter>
#include <QPrintDialog>

#include "ui_tke_form_insert_kvyt_dbf_ob.h"

#include "tke_math.h"
#include "../tkeOrganizacii/udiff_func.h"

// Srtuctures

// Classes
class tke_Form_insert_kvyt_dbf_ob : public QWidget
{
        Q_OBJECT

    public:
        tke_Form_insert_kvyt_dbf_ob(QWidget *parent, QString dir_name="");
		
		int get_Month();
		int get_Year();
		
		~tke_Form_insert_kvyt_dbf_ob();
		//friend class tke_MainWindow;
    public slots:
		void show_sum();
		void open_input_dbf(const QModelIndex & index);
		void insert_kvyt_into_db();
		void createPrintDock(int packet);
		void changedTab(int curTab);
		void action_add_row_activated();
		void action_del_row_activated();
		void printDoc();
		void printDocOn();
	signals:
		
    private:
		Ui::tke_form_insert_kvyt_ob ui;
		
		QModelIndex index;
		QStringList filesList, errorList;
		QString dirName;
		QString fileName;
		QString appPath;
		QString curDateString;
		QString curDateStringExt;
		QTextCodec *codec;
		QAbstractItemModel *arjModel;
		QSqlTableModel *tableModel;
		QSqlQuery * query;
		QSqlRecord record;
		QModelIndex curIndex;
		double sumaOplaty;
		QPrinter *prnt;
};

#endif
