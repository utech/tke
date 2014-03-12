/****************************************************************************
**
// Головна функція проекту для підприємства теплових мереж (ТКЕнерго менеджер :: підприємства).
// Created by Roman Mulyk 15.01.2007
**
****************************************************************************/
#include <QApplication>
#include <QSplashScreen>
#include <QTextStream>
#include <QFile>
#include <QMessageBox>

#include "../../tkeAbonent/db_own.h"
#include "../../tkeAbonent/UForm_fillSubsDbfSpojyvannyaCO.h"

//---------- main function ------------------------------------------------
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("cp1251"));

	//  set conection to ODBC
	U_Db *db = new U_Db();
	if (db->conection_to_db()){
		UPrintDocs * printDocs = new UPrintDocs();
		printDocs->print_vidomistSpojyvannyaCOZaRik(2010, 10, 0);
		delete printDocs;
		
		QApplication::setQuitOnLastWindowClosed(true);
		
		return app.exec();
	}
	else 
		return 1;
}
//-----------------------------------------------------------------------------
