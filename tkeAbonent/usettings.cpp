/****************************************************************************
**
** Created: Roman Mulyk 2.01.2007
**
****************************************************************************/

#include "usettings.h"

#include <QMessageBox>
#include <QCoreApplication>
#include <QUrl>
#include <QFile>

//------------------------------------------------------------
bool setTkeDbfSettings()
{
	bool done_ok=true;
	//������� ������������
	//QTextCodec *codec = QTextCodec::codecForName("CP1251");
	QString dsnPathDbf;
	
	//��������� ������������
	QSettings *winRegister = new QSettings("HKEY_LOCAL_MACHINE\\SOFTWARE\\ODBC\\ODBC.INI\\ODBC File DSN",QSettings::NativeFormat);
	QString defDSNPath;
	defDSNPath = winRegister->value( "DefaultDSNDir", "C:\\Program Files\\Common Files\\ODBC\\Data Sources" ).toString();

	///������������ ������� ����� ��� ���� �� ���������� � dbf
	QSettings *dsnSettings = new QSettings(defDSNPath+QString("/tke_zvit_pilgovyky.dsn"), QSettings::IniFormat);
	dsnPathDbf = QCoreApplication::applicationDirPath() + "/Temp_dbf";
	//dsnPathDbf.replace('/', '\\');
	dsnSettings->beginGroup("ODBC");
	dsnSettings->setValue("DRIVER", QVariant("Driver do Microsoft dBase (*.dbf)"));
	dsnSettings->setValue("UID", QVariant("admin"));
	dsnSettings->setValue("UserCommitSync", QVariant("Yes"));
	dsnSettings->setValue("Threads", QVariant(3));
	dsnSettings->setValue("Statistics", QVariant(0));
	dsnSettings->setValue("SafeTransactions", QVariant(0));
	dsnSettings->setValue("PageTimeout", QVariant(5));
	dsnSettings->setValue("MaxScanRows", QVariant(8));
	dsnSettings->setValue("MaxBufferSize", QVariant(2048));
	dsnSettings->setValue("FIL", QVariant("dBase III"));
	dsnSettings->setValue("DriverId", QVariant(21));
	dsnSettings->setValue("Deleted", QVariant(1));
	dsnSettings->setValue("DefaultDir", QVariant(dsnPathDbf));
	dsnSettings->setValue("DBQ", QVariant(dsnPathDbf));
	dsnSettings->setValue("CollatingSequence", QVariant("ASCII"));
	dsnSettings->endGroup();
	if (dsnSettings->status() != QSettings::NoError){
		done_ok = false;
	}
	delete dsnSettings;
	
	///������������ ������� ����� ��� ������� ��������� � dbf �������� �����
	dsnSettings = new QSettings(defDSNPath+QString("/tke_kvytancii_dbf_ob.dsn"), QSettings::IniFormat);
	dsnPathDbf = QCoreApplication::applicationDirPath() + "/kvyt_ob";
	//dsnPathDbf.replace('/', '\\');
	dsnSettings->beginGroup("ODBC");
	dsnSettings->setValue("DRIVER", QVariant("Driver do Microsoft dBase (*.dbf)"));
	dsnSettings->setValue("UID", QVariant("admin"));
	dsnSettings->setValue("UserCommitSync", QVariant("Yes"));
	dsnSettings->setValue("Threads", QVariant(3));
	dsnSettings->setValue("Statistics", QVariant(0));
	dsnSettings->setValue("SafeTransactions", QVariant(0));
	dsnSettings->setValue("PageTimeout", QVariant(5));
	dsnSettings->setValue("MaxScanRows", QVariant(8));
	dsnSettings->setValue("MaxBufferSize", QVariant(2048));
	dsnSettings->setValue("FIL", QVariant("dBase III"));
	dsnSettings->setValue("DriverId", QVariant(21));
	dsnSettings->setValue("Deleted", QVariant(1));
	dsnSettings->setValue("DefaultDir", QVariant(dsnPathDbf));
	dsnSettings->setValue("DBQ", QVariant(dsnPathDbf));
	dsnSettings->setValue("CollatingSequence", QVariant("ASCII"));
	dsnSettings->endGroup();
	if (dsnSettings->status() != QSettings::NoError){
		done_ok = false;
	}
	delete dsnSettings;

	//������������ ������� ��� ���������� ����������� �������
	dsnSettings = new QSettings(defDSNPath+QString("/tke_subs_dbf.dsn"), QSettings::IniFormat);
	dsnPathDbf = QCoreApplication::applicationDirPath() + "/subs_dbf";
	//dsnPathDbf.replace('/', '\\');
	dsnSettings->beginGroup("ODBC");
	dsnSettings->setValue("DRIVER", QVariant("Driver do Microsoft dBase (*.dbf)"));
	dsnSettings->setValue("UID", QVariant("admin"));
	dsnSettings->setValue("UserCommitSync", QVariant("Yes"));
	dsnSettings->setValue("Threads", QVariant(3));
	dsnSettings->setValue("Statistics", QVariant(0));
	dsnSettings->setValue("SafeTransactions", QVariant(0));
	dsnSettings->setValue("PageTimeout", QVariant(5));
	dsnSettings->setValue("MaxScanRows", QVariant(8));
	dsnSettings->setValue("MaxBufferSize", QVariant(2048));
	dsnSettings->setValue("FIL", QVariant("dBase III"));
	dsnSettings->setValue("DriverId", QVariant(21));
	dsnSettings->setValue("Deleted", QVariant(1));
	dsnSettings->setValue("DefaultDir", QVariant(dsnPathDbf));
	dsnSettings->setValue("DBQ", QVariant(dsnPathDbf));
	dsnSettings->setValue("CollatingSequence", QVariant("ASCII"));
	dsnSettings->endGroup();
	if (dsnSettings->status() != QSettings::NoError){
		done_ok = false;
	}
	delete dsnSettings;
	
	delete winRegister;
		
	return done_ok;
}
//------------------------------------------------------------
QString readAbonentODBCPath()
{
	//��������� ������������
	QSettings *winRegister = new QSettings("HKEY_LOCAL_MACHINE\\SOFTWARE\\ODBC\\ODBC.INI\\ODBC File DSN",QSettings::NativeFormat);
	QString defDSNPath;
	defDSNPath = winRegister->value( "DefaultDSNDir", "C:\\Program Files\\Common Files\\ODBC\\Data Sources" ).toString();

	///������������ ������� ����� ��� ���� �� ���������� � dbf
	QSettings *dsnSettings = new QSettings(defDSNPath+QString("/tke_db.dsn"), QSettings::IniFormat);
	
	
	delete winRegister;
	delete dsnSettings;
}
//------------------------------------------------------------
QString readOrganizODBCPath()
{

}
//------------------------------------------------------------
bool setAbonentODBCPath()
{

}
//------------------------------------------------------------
bool setOrganizODBCPath()
{

}
//------------------------------------------------------------
