/****************************************************************************
**
// Implamantation of tke_MainWindow class.
// Created by Roman Mulyk 08.08.2006
**
****************************************************************************/
#include <QApplication>
#include <QSplashScreen>
#include <QTextStream>
#include <QFile>
#include <QMessageBox>

#include "tke_mainwindow.h"
#include "tke_dialog_narah.h"
#include <UDebugForm>
#include "usettings.h"
#include "../install/dbUpdater/UDbUpdaterFunctions.h"

#include <QtDebug>
//---------- main function ------------------------------------------------
int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("cp1251"));
    QSplashScreen *splash = new QSplashScreen(QPixmap(":/pictures/pictures/abonentLogo.png"));
    splash->show();
	splash->showMessage("");
	
	//Читання параметрів
	bool exclusiveDBAccess=false;
	if ((argc > 1) && ((QString(argv[1])=="-e") || (QString(argv[1])=="e")) )
		exclusiveDBAccess = true;
	
	UDebug::createDebugForm();
	
#ifdef TKE_DEMO
		setTkeDbfSettings();

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
		
		tke_MainWindow *mainWin = new tke_MainWindow();
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
