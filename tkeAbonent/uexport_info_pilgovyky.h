/****************************************************************************
**
** Створено: Роман Мулик 22.11.2006
**
****************************************************************************/

#ifndef UEXPORT_INFO_PILGOVYKY_H
#define UEXPORT_INFO_PILGOVYKY_H

//--- includes ---
#include<QSqlQuery>
#include<QSqlError>
#include<QString>
#include <QTextCodec>
#include <QVariant>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QCoreApplication>

#include <xbase64.h>

#include "tke_math.h"
//--- structures ---

//----------interfaces-------------
class UExportInfoPilgovyky
{

	public:
		//--- constructors/destrucror ---   
      UExportInfoPilgovyky( QString dirName="", short year=2006, short month=11, int _zFormType=0 );
	  virtual ~UExportInfoPilgovyky();
	  
	  //Функції створення стрічок для запису
	  QString zagolovok();
	  QString zagolovok_year(short year);
	  QString zagolovok_month(short month);
	  QString zagolovok_edrpou();
	  QString zagolovok_width_of_record();  //Довжина запису
	  QString identyf(QString id);
	  QString pib(QString prizv, QString imya, QString batk);
	  QString rahunok(int rah);
	  QString year_narah(short year);
	  QString month_narah(short month);
	  QString kod_pilgy(short pkod); // Код пільги
	  QString pilga_for(short p_for); // Пільга для кількості людей
	  QString vids_pilgy(short vids); // Відсоток пільги
	  QString pilga_narah(double narah); //Нарахування пільги
	  QString spojyvannya(double ploshcha); //Споживання (ТКЕ - площа)
	  char * strToxBaseChar(QString str);
	  QString name_of_file();
	  bool save_to_file();
	  bool save_to_file_v2007();
	  bool checkDirCompatibility();
	  
		//--- functions ---

    private:
        //variables
		short yy; // рік
		short mm; // місяць
		QString dir_name;
		int zFormType;
		
		QTextCodec *codec;
		QTextCodec *funcCodec;
		int rec_width;
		//--- functions ---

};
//-----------------------------------
#endif
