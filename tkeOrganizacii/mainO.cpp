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

#include "../tkeAbonent/db_own.h"
#include "tke_mainwindow_org.h"
#include <UDebugForm>
#include "../install/dbUpdater/UDbUpdaterFunctions.h"

//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QSplashScreen *splash = new QSplashScreen(QPixmap(":/pictures/pictures/organizLogo.png"));
    splash->show();
	splash->showMessage("");
	
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("cp1251"));
	
	//Читання параметрів
	bool exclusiveDBAccess=false;
	if ((argc > 1) && ((QString(argv[1])=="-e") || (QString(argv[1])=="e")) )
		exclusiveDBAccess = true;
	
	UDebug::createDebugForm();
	
#ifdef TKE_DEMO
		QFile *aboutFile = new QFile("aboutDemo.html");
		aboutFile->open(QFile::ReadOnly);
		QTextStream *aboutText = new QTextStream(aboutFile);
		QMessageBox::about (0,"Про програму",aboutText->readAll());
		delete aboutFile;
		delete aboutText;
#endif
	
	//  set conection to ODBC
	U_Db *db = new U_Db();
	if (db->conection_to_db(exclusiveDBAccess)){
		UDbUpdater *upd = new UDbUpdater();
		if (!upd->checkVersions()){
			return 0;
		}
		delete upd;
		delete db;
	
		tke_MainWindow_org *mainWin = new tke_MainWindow_org();
		mainWin->showMaximized();
		splash->finish(mainWin);
		delete splash;
		QApplication::connect(mainWin, SIGNAL(requestDebugWindow()), UDebug::debugFormPointer, SLOT(showDebug()));
		QApplication::connect(mainWin, SIGNAL(mainWindowClosed()), UDebug::debugFormPointer, SLOT(close()));
		QApplication::connect(mainWin, SIGNAL(closeLoginSignal()), UDebug::debugFormPointer, SLOT(close()));
		
		QApplication::setQuitOnLastWindowClosed(false);
		
		return app.exec();
	}
	else {
		delete splash;
		delete db;
	}
	return 1;
}
//-----------------------------------------------------------------------------
