//------------------------------------------------------------
//
// ������� ������� �������� DbUpdater
// Created by Roman Mulyk 16.01.2009
//
//------------------------------------------------------------
#include <QApplication>
#include <QSplashScreen>
#include <QTextStream>
#include <QFile>
#include <QMessageBox>

#include "../tkeOrganizacii/udebug_form.h"
#include "../tkeAbonent/umath.h"
#include "../tkeAbonent/db_own.h"
#include "tke_form_updateDataBase.h"

UDebugForm * debugFormPointer;
//-----------------------------------------------------------------------------
void tkeMessageOutput(QtMsgType type, const char *msg);
//---------- main function ------------------------------------------------
int main(int argc, char *argv[])
{
	//��������� ��������  dbUpdater.exe newDbVersion -fon
	bool GuiInterface=false;
	int newProgVersion;
	if (argc != 1){
		if (argc < 2)
			return 2; // ������ ������� ���������
		newProgVersion = QString(argv[1]).toInt();
		if (newProgVersion < 5)
			return 3; // ������� ������ �������� - ����� ���� ���� ���� �����
		if (argc>2 && argv[2] == "-fon")
			GuiInterface = false;
	}
	
	QApplication app(argc, argv);
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("cp1251"));
	
	UDebugForm *debugForm = new UDebugForm();
	debugFormPointer = debugForm;
	qInstallMsgHandler(tkeMessageOutput);
	
#ifdef TKE_DEMO
		setTkeDbfSettings();

		QFile *aboutFile = new QFile("aboutDemo.html");
		aboutFile->open(QFile::ReadOnly);
		QTextStream *aboutText = new QTextStream(aboutFile);
		QMessageBox::about (0,"��� ��������",aboutText->readAll());
		delete aboutFile;
		delete aboutText;
#endif
	
	//  set conection to ODBC
	U_Db *db = new U_Db();
	QMessageBox * mBox = new QMessageBox(QMessageBox::Critical, "!!! ������� ��'������� �� �� !!!",
								"�������� �� ������ ��'�������� �� ���� ����� �� \"�������� ��������\".\n"
								"��������� ������� ��������, �� �������������� ���� �����,\n"
								"��� ������� ������ �� ��. \n"
								"��� ���������� ��'�������� �� ��� �������� ������ \"ϲ�'��������\"");
	QPushButton *connectButton = mBox->addButton("ϲ�'��������", QMessageBox::ActionRole);
	QPushButton *cancelButton = mBox->addButton("����� � ��������", QMessageBox::ActionRole);
	while (!db->conection_to_db(true, true)){
		mBox->exec();
		if (mBox->clickedButton() == cancelButton){
			delete mBox;
			return 1;
		}
	}
	delete mBox;
	delete db;
	
	if (GuiInterface){ //������ � ���������� �����
		tke_form_updateDataBase *mainWin = new tke_form_updateDataBase();
		mainWin->show();
		return app.exec();
	}
	else{ //������ � �������� �����
		
	}

	return 0;
}
//-----------------------------------------------------------------------------

void tkeMessageOutput(QtMsgType type, const char *msg) //������� ������� �������� ������ ������� � �����������
{
	QString messageSeparator("-------");
	switch (type) {
     case QtDebugMsg:
		//debugFormPointer->addDebugString("");
        debugFormPointer->addDebugString(msg);
        break;
     case QtWarningMsg:
		//debugFormPointer->addWarningString(messageSeparator);
        debugFormPointer->addWarningString(msg);
        break;
     case QtCriticalMsg:
		debugFormPointer->addCriticalString(messageSeparator + "\nQtCriticalMsg:");
        debugFormPointer->addCriticalString(msg);
        break;
     case QtFatalMsg:
		debugFormPointer->addCriticalString(messageSeparator + "\nQtFatalMsg:");
        debugFormPointer->addCriticalString(msg);
        abort();
    }
}
//------------------------------------------------------------

