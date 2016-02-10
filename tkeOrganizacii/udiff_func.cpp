/****************************************************************************
**
** Created: Roman Mulyk 4.03.2006
**
****************************************************************************/

#include "udiff_func.h"
#include <QDebug>

#include <QDir>
#include <QCoreApplication>
#include <QProcess>
#include <QDateTime>

//------------------------------------------------------------
QString TkeProgramVersion()
{
    return "2.5.1";
}
//------------------------------------------------------------
int TkeDbVersion()
{
    return 25;
}
//------------------------------------------------------------
QString PrizvIB(int posadaId)
{
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT Prizv, Imya, Batk FROM this_posadovci WHERE This_organiz_id="+
					QVariant(organizID()).toString()+" and Posada_id="+QVariant(posadaId).toString());
	query->seek(0);
	QString rezString = query->value(0).toString()+" "+query->value(1).toString().at(0)+
						"."+query->value(2).toString().at(0)+".";
	delete query;
	return rezString;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString OrganizName()
{
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT this_organiz_name FROM this_organiz WHERE id="+QVariant(organizID()).toString());
	query->seek(0);
	QString rezString = query->value(0).toString();
	delete query;
	return rezString;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString OrganizCity()
{
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT this_organiz_sity FROM this_organiz WHERE id="+QVariant(organizID()).toString());
	query->seek(0);
	QString rezString = query->value(0).toString();
	delete query;
	return rezString;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString OrganizEDRPOU()
{
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT this_organiz_edrpou FROM this_organiz WHERE id="+QVariant(organizID()).toString());
	query->seek(0);
	QString rezString = query->value(0).toString();
	delete query;
	return rezString;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString OrganizAdresa()
{
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT this_organiz_adresa FROM this_organiz WHERE id="+QVariant(organizID()).toString());
	query->seek(0);
	QString rezString = query->value(0).toString();
	delete query;
	return rezString;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString OrganizTel()
{
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT this_organiz_tel FROM this_organiz WHERE id="+QVariant(organizID()).toString());
	query->seek(0);
	QString rezString = query->value(0).toString();
	delete query;
	return rezString;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString OrganizRozrahRahunok()
{
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT this_organiz_rozrah_rahunok FROM this_organiz WHERE id="+QVariant(organizID()).toString());
	query->seek(0);
	QString rezString = query->value(0).toString();
	delete query;
	return rezString;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString OrganizBank()
{
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT this_organiz_bank FROM this_organiz WHERE id="+QVariant(organizID()).toString());
	query->seek(0);
	QString rezString = query->value(0).toString();
	delete query;
	return rezString;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString OrganizEMail()
{
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT this_email FROM this_organiz WHERE id="+QVariant(organizID()).toString());
	query->seek(0);
	QString rezString = query->value(0).toString();
	delete query;
	return rezString;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString DocsPosadaPidpysText(int posadaId)
{
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT Docs_text FROM this_posadovci WHERE This_organiz_id="+
					QVariant(organizID()).toString()+" and Posada_id="+QVariant(posadaId).toString());
	query->seek(0);
	QString rezString = query->value(0).toString();
	delete query;
	return rezString;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double OrganizPdv()
{
	QSqlQuery query("SELECT value FROM slujb_znach WHERE id=1");
	query.seek(0);
	return query.value(0).toDouble();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int organizID()
{
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	int organizID = settings->value("dataBase/Organiz_id").toInt();
	delete settings;
	return organizID;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
short tochnistDiuchogoTaryfu()
{
	QSqlQuery query("SELECT value FROM slujb_znach WHERE id=8");
	query.seek(0);
	short iVal = query.value(0).toDouble();
	if (iVal < 2) iVal = 2;
	if (iVal > 6) iVal = 6;
	return iVal;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString encrypt(QString str)
{
	return str.toLocal8Bit().toBase64();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString decrypt(QString str)
{
	return QByteArray::fromBase64(str.toLocal8Bit());
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString saveDataBase(bool wait)
{
	QString dirPath = QCoreApplication::applicationDirPath().replace('/','\\');
	QDir::setCurrent(dirPath);
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	QProcess *process = new QProcess;
	QString pathString = settings->value("dataBase/path").toString().replace('/','\\').simplified();
	if (pathString.startsWith(".\\"))
		pathString = dirPath + '\\' + pathString.remove(0, 2);
	else if (pathString.startsWith("..\\"))
		pathString = dirPath + '\\' + pathString.remove(0, 3);
	else if (!pathString.contains('\\'))
		pathString = dirPath + '\\' + pathString;
	
	QString tempPath = QDir::tempPath();
	QFileInfo fInfo(pathString);
	QString dbFileName = fInfo.fileName();
	if (QFile::exists(tempPath+'\\'+dbFileName))
		if (!QFile::remove(tempPath+'\\'+dbFileName))
			QMessageBox::critical(0,"Помилка","Помилка видалення попереднього temp файлу");
	if (!QFile::copy(pathString, tempPath+'\\'+dbFileName))
		QMessageBox::critical(0,"Помилка","Помилка копіювання бази до тимчасової папки");
	//arj a -e -y ../save_db/08030124.arj d:\tke_db\tke_data.mdb
	//7z a -t7z "\\romans-note\Обм?нна папка\tke_zol.7z"  "d:\tke_db\tke_zol.mdb"
	QString pathToSave = settings->value("dataBase/savePath").toString().replace('/','\\');
	if (pathToSave.isEmpty())
		pathToSave = "..\\save_db";
	QString backupFileName = pathToSave+'\\'+QDateTime::currentDateTime().toString("yyMMddhhmm")+".7z";
	//QString str = "arj a -e -y \""+backupFileName+"\" \""+pathString+"\"";
	QString str = "7z a -t7z \""+backupFileName+"\" \""+tempPath+'\\'+dbFileName+"\" -y";
	//qDebug() << "SaveDB string - " << str;
	if (!wait)
		process->startDetached(str);
	else
		process->execute(str);
	delete process;
	delete settings;
	return backupFileName;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void castControls(QObject *obj, bool isAct)
{
	QAction *action = qobject_cast<QAction *>(obj);
	if (action){
		action->setEnabled(isAct);
		return;
	}
	
	QComboBox *comboBox = qobject_cast<QComboBox *>(obj);
	if (comboBox){
		comboBox->setEnabled(isAct);
		return;
	}
	
	QSpinBox *spinBox = qobject_cast<QSpinBox *>(obj);
	if (spinBox){
		spinBox->setReadOnly(!isAct);
		return;
	}
	
	QLineEdit *lineEdit = qobject_cast<QLineEdit *>(obj);
	if (lineEdit){
		lineEdit->setReadOnly(!isAct);
		return;
	}
	
	QDoubleSpinBox *doubleSpinBox = qobject_cast<QDoubleSpinBox *>(obj);
	if (doubleSpinBox){
		doubleSpinBox->setReadOnly(!isAct);
		return;
	}
	
	QCheckBox *checkBox = qobject_cast<QCheckBox *>(obj);
	if (checkBox){
		checkBox->setEnabled(isAct);
		return;
	}
	
	QToolButton *toolButton = qobject_cast<QToolButton *>(obj);
	if (toolButton){
		toolButton->setEnabled(isAct);
		return;
	}
	
	QDateEdit *dateEdit = qobject_cast<QDateEdit *>(obj);
	if (dateEdit){
		dateEdit->setEnabled(isAct);
		return;
	}
	
	QPushButton *pushButton = qobject_cast<QPushButton *>(obj);
	if (pushButton){
		pushButton->setEnabled(isAct);
		return;
	}
	
	QGroupBox *groupBox = qobject_cast<QGroupBox *>(obj);
	if (groupBox){
		groupBox->setEnabled(isAct);
		return;
	}
	
	QTableWidget *tableWidget = qobject_cast<QTableWidget *>(obj);
	if (tableWidget){
		tableWidget->blockSignals(!isAct);
		return;
	}
	
	QTableView *tableView = qobject_cast<QTableView *>(obj);
	if (tableView){
		tableView->setEnabled(isAct);
		return;
	}
	
	QWidget *widget = qobject_cast<QWidget *>(obj);
	if (widget){
		widget->setEnabled(isAct);
		return;
	}
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UOrgFinanceType financeTypeForPidrozdil(int iPidrozdilID)
{
	QSqlQuery query("SELECT TOP 1 Finans FROM organizacii \
					 RIGHT JOIN pidrozdily \
						ON organizacii.Ugoda = pidrozdily.Ugoda\
					 WHERE pidrozdily.id = " + QVariant(iPidrozdilID).toString());
	
	if (!query.next())
		return FinaceTypeAny;
	
	switch (query.value(0).toInt())
	{
	case FinanceMiskBudget:
	case FinanceRyonBudget:
	case FinanceOblBudjget:
	case FinanceStateBudget:
	case FinanceGosprozrah:
	case FinanceNasel:
		return (UOrgFinanceType) query.value(0).toInt();
	default:
		return FinaceTypeAny;
	}
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UOrgFinanceType financeTypeForOrganiz(int iUgodaNum)
{
	QSqlQuery query("SELECT TOP 1 Finans FROM organizacii WHERE Ugoda = " + QVariant(iUgodaNum).toString());
	
	if (!query.next())
		return FinaceTypeAny;
	
	switch (query.value(0).toInt())
	{
	case FinanceMiskBudget:
	case FinanceRyonBudget:
	case FinanceOblBudjget:
	case FinanceStateBudget:
	case FinanceGosprozrah:
	case FinanceNasel:
		return (UOrgFinanceType) query.value(0).toInt();
	default:
		return FinaceTypeAny;
	}
}
