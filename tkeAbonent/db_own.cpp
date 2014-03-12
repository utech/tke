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
			QMessageBox::critical(0, codec->toUnicode("Помилка доступу до бази даних"), 
								codec->toUnicode("Причина помилки, згенерована драйвером:\n")
								+ pdb.lastError().text() 
								+ codec->toUnicode("\n\nПрограма не змогла під'єднатись до бази даних.\n\
								\nМожливі причини:\
								\n 1. Вимкнений серверний комп'ютер.\
								\n 2. На сереверному комп'ютері не встигли завантажитись драйвери мережі.\
								\n 3. Погано налаштовані (збиті) джерела даних ODBC.\
								\n 4. Неполадки з мережею.\n\
								\nПорада - ! 1 ! Смішно, але ввімкніть серверний комп'ютер і зачекайте поки він завантажиться.\
								\nПорада - !2-3! Зачекайте пів хвилини і повторно запустіть програму.\
								\nПорада - ! 4 ! Перевірте чи під'єднаний мережевий кабель Вашого комп'ютера до комутатора.\
								\n                 і чи ввімкнений сам комутатор.\
								\nПорада - Пошукайте інформацію в справці до програми.\
								\nЯкщо вичерпались ідеї по ремонту, то звертайтесь до адміністратора.\n\
								\n                            Вибачте за незручності."));
        return false;
    }
    return true;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
