/****************************************************************************
**
** Created: Roman Mulyk 29.07.2005
**
****************************************************************************/

#include "db_own.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
U_Db::U_Db()
{
    QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	db			= settings->value("dataBase/path").toString();
	user        = settings->value("dataBase/user").toString();
    password 	= decrypt(settings->value("dataBase/password").toString());
    host        = "localhost";
	delete settings;
    
	codec = QTextCodec::codecForName("CP1251");
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
U_Db::U_Db(QString dbName, QString dbUser, QString dbPassword)
{
	db = dbName;
    user = dbUser;
    password = dbPassword;
    host = "localhost";
	codec = QTextCodec::codecForName("CP1251");
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool U_Db::conection_to_db(bool exclusive, bool quiet)
{
    QSqlDatabase pdb = QSqlDatabase::addDatabase("QODBC");
	QString excStr;
	if (exclusive)
		excStr='1';
	else
		excStr='0';
    pdb.setDatabaseName ("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ="+db+";Exclusive="+excStr);
    pdb.setUserName(user);
    pdb.setHostName(host);
    pdb.setPassword(password);
    if (!pdb.open()) {
        if (!quiet)
			QMessageBox::critical(0, codec->toUnicode("������� ������� �� ���� �����"), 
								codec->toUnicode("������� �������, ����������� ���������:\n")
								+ pdb.lastError().text() 
								+ codec->toUnicode("\n\n�������� �� ������ ��'�������� �� ���� �����.\n\
								\n������ �������:\
								\n 1. ��������� ��������� ����'����.\
								\n 2. �� ����������� ����'���� �� ������� ������������� �������� �����.\
								\n 3. ������ ���������� (����) ������� ����� ODBC.\
								\n 4. ��������� � �������.\n\
								\n������ - ! 1 ! �����, ��� ������� ��������� ����'���� � ��������� ���� �� �������������.\
								\n������ - !2-3! ��������� �� ������� � �������� �������� ��������.\
								\n������ - ! 4 ! �������� �� ��'������� ��������� ������ ������ ����'����� �� ����������.\
								\n                 � �� ��������� ��� ���������.\
								\n������ - ��������� ���������� � ������� �� ��������.\
								\n���� ����������� ��� �� �������, �� ����������� �� ������������.\n\
								\n                            ������� �� ����������."));
        return false;
    }
    return true;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
