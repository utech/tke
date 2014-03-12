//------------------------------------------------------------
//
// Declaration of UDbScriptAccess class.
// Created by Roman Mulyk 07.07.2008
//
//------------------------------------------------------------

#ifndef UDBSCRIPTACCESS_H
#define UDBSCRIPTACCESS_H

#define BANSK_SCR_STR "banks"
#define FILETYPES_SCR_STR "fileTypes"
#define ONEPACKINFILE_SCR_STR "onePackInFile"
#define FUNCTIONNAMES_SCR_STR "functionNames"

#include <QWidget>
#include <QMessageBox>
#include <QTextCodec>
#include <QtSql>
#include <QVariant>
#include <QFile>
#include <QScriptEngine>
#include <QScriptValue>
#include <QHash>
#include <QScriptValueIterator>

#include <xbase64.h>

#include "../tkeOrganizacii/udiff_func.h"
#include <UStandardDelegate>

//------------------------------------------------------------
class UDbScriptAccess: public QObject
{
	Q_OBJECT
	public:
		UDbScriptAccess(QObject *parent=0, QTableWidget *tableWidget=0);
		virtual ~UDbScriptAccess();
		void setTableWidgetEdited(QTableWidget *tableWidgetEdited);
		void setScriptEngine(QString iniName, QString settingName);
		QString readScriptOrgName();
		
		Q_INVOKABLE bool copyFile(QString sourceFile, QString destFile);
		Q_INVOKABLE int execProcess(QString command);
		Q_INVOKABLE bool openDbfConnection(QString dbName);
		Q_INVOKABLE void closeDbfConnection();
		Q_INVOKABLE int queryNumRows();
		Q_INVOKABLE bool querySeek(int seek);
		Q_INVOKABLE bool queryNext();
		Q_INVOKABLE QString queryValue(int index);
		Q_INVOKABLE QString queryValue(QString fildName);
		Q_INVOKABLE QString queryDateValue(int index, QString df = "yyyy-MM-dd");
		Q_INVOKABLE QString queryDateValue(QString fildName, QString df = "yyyy-MM-dd");
		
		Q_INVOKABLE bool tkeQueryExec(QString q);
		Q_INVOKABLE bool tkeQuerySeek(int seek);
		Q_INVOKABLE bool tkeQueryNext();
		Q_INVOKABLE QString tkeQueryValue(int index);
		Q_INVOKABLE QString tkeQueryDateValue(int index);
		Q_INVOKABLE void tkeQueryClear();
		
		Q_INVOKABLE void populateTable(int row, int col, QString text, QString textColor = "#000000");
		Q_INVOKABLE void showMessage(QString title, QString message);
		Q_INVOKABLE QString binDirPath();
		Q_INVOKABLE	QString fileName(QString fullPath);
		Q_INVOKABLE	QString pureFileName(QString fullPath);
		Q_INVOKABLE	bool setCurrentDir(QString dirPath);
		
		Q_INVOKABLE	QString replaceStr(QString str, QString subStrBefour, QString subStrAfter);
	public slots:
		void setTableRowCount(int count);
	public:
		QScriptEngine engine;
		QScriptValue scrValSV;
		bool __scrDBOpend;
		QSqlQuery tkeQuery;
		
		xbXBase x;
		xbDbf *dbfFile;
		QTableWidget *__tableWidgetEdited;
};
//------------------------------------------------------------
#endif
