//------------------------------------------------------------
//
// Implamantation of UDbScriptAccess class.
// Created by Roman Mulyk 07.07.2008
//
//------------------------------------------------------------

#include "UDbScriptAccess.h"

//------------------------------------------------------------
QScriptValue print(QScriptContext *sC=0, QScriptEngine *sE=0)
{
	if (sC->argumentCount()>0){
		QString str = sC->argument(0).toString();
		qDebug() << str;
		return QScriptValue(sE, str);
	}
	else
		return QScriptValue(sE, "");
}
//--------------------utech--------------------utech--------------------utech--------------------
UDbScriptAccess::UDbScriptAccess(QObject *parent, QTableWidget *tableWidget)
	: QObject(parent)
{
	dbfFile = new xbDbf( &x );
	
	__scrDBOpend = false;
	__tableWidgetEdited = tableWidget;
	
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	
	//setScriptEngine();
}
//------------------------------------------------------------
void UDbScriptAccess::setTableWidgetEdited(QTableWidget *tableWidgetEdited)
{
	__tableWidgetEdited = tableWidgetEdited;
}
//------------------------------------------------------------
void UDbScriptAccess::setScriptEngine(QString iniName, QString settingName)
{
	//Оголошення змінних, доступних з скрипта
	scrValSV = engine.newQObject(this);
	engine.globalObject().setProperty("form", scrValSV);
	
	scrValSV = engine.newFunction(print);
	engine.globalObject().setProperty("print", scrValSV);
	
	engine.globalObject().setProperty("kvytDir", QScriptValue(&engine, KVYT_ARJ_DIR));
	
	//Визначення кольорів
	engine.globalObject().setProperty("clBlack", QScriptValue(&engine, "#000000"));
	engine.globalObject().setProperty("clWhite", QScriptValue(&engine, "#ffffff"));
	engine.globalObject().setProperty("clRed", QScriptValue(&engine, "#ff0000"));
	engine.globalObject().setProperty("clDarkRed", QScriptValue(&engine, "#800000"));
	engine.globalObject().setProperty("clGreen", QScriptValue(&engine, "#00ff00"));
	engine.globalObject().setProperty("clDarkGreen", QScriptValue(&engine, "#008000"));
	engine.globalObject().setProperty("clBlue", QScriptValue(&engine, "#0000ff"));
	engine.globalObject().setProperty("clDarkBlue", QScriptValue(&engine, "#000080"));
	engine.globalObject().setProperty("clMagenta", QScriptValue(&engine, "#ff00ff"));
	engine.globalObject().setProperty("clDarkMagenta", QScriptValue(&engine, "#800080"));
	engine.globalObject().setProperty("clYellow", QScriptValue(&engine, "#ffff00"));
	engine.globalObject().setProperty("clGray", QScriptValue(&engine, "#a0a0a4"));
	engine.globalObject().setProperty("clDarkGray", QScriptValue(&engine, "#808080"));
	
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QSettings *settings = new QSettings(iniName, QSettings::IniFormat);
	QString scriptFileName = settings->value(settingName).toString();
	delete settings;
	
	QFile scriptFile(scriptFileName);
    if (!scriptFile.open(QIODevice::ReadOnly))
		QMessageBox::critical(0,
				"!!! Відсутність скрипта !!!",
				"Програма не змогла відкрити файл скрипта.\nІм'я файлу:"+scriptFileName+
				"\nПеревірте правильність налаштування шляху до файла.\n",
				QMessageBox::Ok,
				QMessageBox::NoButton);
    scrValSV = engine.evaluate(scriptFile.readAll());
	if (engine.hasUncaughtException())
			qDebug() << "Помилка виконання скрипта. Стрічка " << engine.uncaughtExceptionLineNumber() << ":" << scrValSV.toString();
    scriptFile.close();
}
//------------------------------------------------------------
QString UDbScriptAccess::readScriptOrgName()
{
	scrValSV = engine.globalObject().property("thisOrganizName");
	if (scrValSV.isString())
		return scrValSV.toString();
	else
		return "У скрипті не визначено назву організації";
}
//------------------------------------------------------------
void UDbScriptAccess::setTableRowCount(int count)
{
	__tableWidgetEdited->setRowCount(count);
	__tableWidgetEdited->resizeRowsToContents();
}
//------------------------------------------------------------
bool UDbScriptAccess::copyFile(QString sourceFile, QString destFile)
{
	if (QFile::exists(destFile))
		QFile::remove(destFile);
	return QFile::copy(sourceFile, destFile);
}
//------------------------------------------------------------
int UDbScriptAccess::execProcess(QString command)
{
	return QProcess::execute(command);
}
//------------------------------------------------------------
bool UDbScriptAccess::openDbfConnection(QString dbName)
{
	if (dbfFile->OpenDatabase(dbName.toAscii()) != XB_NO_ERROR){
		__scrDBOpend = false;
		return __scrDBOpend;
		}
	__scrDBOpend = true;
	dbfFile->AutoLockOff();
	return __scrDBOpend;
}
//------------------------------------------------------------
void UDbScriptAccess::closeDbfConnection()
{
	dbfFile->CloseDatabase();
}
//------------------------------------------------------------
int UDbScriptAccess::queryNumRows()
{
	if (!__scrDBOpend)
		return -1;
	return dbfFile->NoOfRecords();
}
//------------------------------------------------------------
bool UDbScriptAccess::querySeek(int seek)
{
	if (!__scrDBOpend)
		return false;
	if (dbfFile->GetRecord(seek) != XB_NO_ERROR)
		return false;
	return true;
}
//------------------------------------------------------------
bool UDbScriptAccess::queryNext()
{
	if (!__scrDBOpend)
		return false;
	if (dbfFile->GetNextRecord() != XB_NO_ERROR)
		return false;
	return true;
}
//------------------------------------------------------------
QString UDbScriptAccess::queryValue(int index)
{
	if (!__scrDBOpend)
		return "";
	return dbfFile->GetField(index);
}
//------------------------------------------------------------
QString UDbScriptAccess::queryValue(QString fildName)
{
	if (!__scrDBOpend)
		return "";
	//return dbfFile->GetField(fildName.data()->toAscii());
	return dbfFile->GetField(dbfFile->GetFieldNo(fildName.toAscii()));
}
//------------------------------------------------------------
QString UDbScriptAccess::queryDateValue(int index, QString df)
{
	if (!__scrDBOpend)
		return "";
	return QDate::fromString(dbfFile->GetField(index), "yyyyMMdd").toString(df);
}
//------------------------------------------------------------
QString UDbScriptAccess::queryDateValue(QString fildName, QString df)
{
	if (!__scrDBOpend)
		return "";
	return QDate::fromString(dbfFile->GetField(dbfFile->GetFieldNo(fildName.toAscii())), "yyyyMMdd").toString(df);
}
//------------------------------------------------------------
bool UDbScriptAccess::tkeQueryExec(QString q)
{
	return tkeQuery.exec(q);
}
//------------------------------------------------------------
bool UDbScriptAccess::tkeQuerySeek(int seek)
{
	return tkeQuery.seek(seek);
}
//------------------------------------------------------------
bool UDbScriptAccess::tkeQueryNext()
{
	return tkeQuery.next();
}
//------------------------------------------------------------
QString UDbScriptAccess::tkeQueryValue(int index)
{
	return tkeQuery.value(index).toString();
}
//------------------------------------------------------------
QString UDbScriptAccess::tkeQueryDateValue(int index)
{
	return tkeQuery.value(index).toDate().toString("dd.MM.yyyy");
}
//------------------------------------------------------------
void UDbScriptAccess::tkeQueryClear()
{
	tkeQuery.clear();
}
//------------------------------------------------------------
void UDbScriptAccess::populateTable(int row, int col, QString text, QString textColor)
{
	QTableWidgetItem *item = new QTableWidgetItem("");
	item->setData(2, text);
	item->setForeground(QBrush(QColor(textColor)));
	item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	__tableWidgetEdited->blockSignals(true);
	__tableWidgetEdited->setItem(row, col, item);
	__tableWidgetEdited->blockSignals(false);
}
//------------------------------------------------------------
void UDbScriptAccess::showMessage(QString title, QString message)
{
	QMessageBox::information(0,
				title,
				message,
				QMessageBox::Ok,
				QMessageBox::NoButton);
}
//------------------------------------------------------------
QString UDbScriptAccess::binDirPath()
{
	return QApplication::applicationDirPath();
}
//------------------------------------------------------------
QString UDbScriptAccess::fileName(QString fullPath)
{
	fullPath = fullPath.replace("\\","/");
	int ind = fullPath.lastIndexOf('/');
	if (ind == -1)
		return fullPath;
	return fullPath.right(fullPath.size()-ind-1);
}
//------------------------------------------------------------
QString UDbScriptAccess::pureFileName(QString fullPath)
{
	fullPath = fullPath.replace("\\","/");
	int ind = fullPath.lastIndexOf('/');
	if (ind == -1)
		return fullPath;
	QString n = fullPath.right(fullPath.size()-ind-1);
	ind = n.lastIndexOf('.');
	if (ind == -1)
		return n;
	return n.left(ind);
}
//------------------------------------------------------------
bool UDbScriptAccess::setCurrentDir(QString dirPath)
{
	return QDir::setCurrent(dirPath);
}
//------------------------------------------------------------
QString UDbScriptAccess::replaceStr(QString str, QString subStrBefour, QString subStrAfter)
{
	return str.replace(subStrBefour, subStrAfter);
}
//------------------------------------------------------------
UDbScriptAccess::~UDbScriptAccess()
{

}
//------------------------------------------------------------
