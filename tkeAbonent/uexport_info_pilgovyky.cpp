/****************************************************************************
**
** Створено: Роман Мулик 22.11.2006
**
****************************************************************************/

#include "uexport_info_pilgovyky.h"

#include <UPostupForm>
#include <USqlAccessible>
#include <QTime>
#include "../tkeOrganizacii/udiff_func.h"

//~~~~~~~~constructor1~~~~~~~~~~~~~~~~~~~~~~~~
UExportInfoPilgovyky::UExportInfoPilgovyky( QString dirName, short year, short month, int _zFormType )
{
	yy = year;
	mm = month;
	dir_name = dirName;
	zFormType = _zFormType;
	
	codec = QTextCodec::codecForName("CP1251");
	funcCodec = QTextCodec::codecForName("IBM 866");
	//funcCodec = QTextCodec::codecForName("Windows-1251");
	rec_width = 111;
}
//------------------------------------------------------------
QString UExportInfoPilgovyky::zagolovok()
{
	return "IMPORT_LGT";
}
//------------------------------------------------------------
QString UExportInfoPilgovyky::zagolovok_year(short year)
{
	QString str;
	if (year<100)
		str += "20";
	if (year<10)
		str += "0";
	str+=QVariant(year).toString();
	return str;
}
//------------------------------------------------------------
QString UExportInfoPilgovyky::zagolovok_month(short month)
{
	QString str;
	if (month<10)
		str += "0";
	str+=QVariant(month).toString();
	return str;
}
//------------------------------------------------------------
QString UExportInfoPilgovyky::zagolovok_edrpou()
{
	return OrganizEDRPOU();
}
//------------------------------------------------------------
QString UExportInfoPilgovyky::zagolovok_width_of_record()
{
	QString str;
	str=QVariant(rec_width).toString();
	str = str.rightJustified ( 3, '0', true );
	return str;
}
//------------------------------------------------------------
QString UExportInfoPilgovyky::identyf(QString id)
{
	id = id.rightJustified ( 10, '0', true );
	return id;
}
//------------------------------------------------------------
QString UExportInfoPilgovyky::pib(QString prizv, QString imya, QString batk)
{
	QString str;
	str = prizv + " " + imya + " " + batk;
	str = str.leftJustified ( 50, ' ', true );
	return str;
}
//------------------------------------------------------------
QString UExportInfoPilgovyky::rahunok(int rah)
{
	QString str;
	str = QVariant(rah).toString();
	str = str.rightJustified ( 15, '0', true );
	return str;
}
//------------------------------------------------------------
QString UExportInfoPilgovyky::year_narah(short year)
{
	QString str;
	if (year<100)
		str += "20";
	if (year<10)
		str += "0";
	str+=QVariant(year).toString();
	return str;
}
//------------------------------------------------------------
QString UExportInfoPilgovyky::month_narah(short month)
{
	QString str;
	if (month<10)
		str += "0";
	str+=QVariant(month).toString();
	return str;
}
//------------------------------------------------------------
QString UExportInfoPilgovyky::kod_pilgy(short pkod)
{
	QString str;
	str = QVariant(pkod).toString();
	str = str.rightJustified ( 3, '0', true );
	return str;
}
//------------------------------------------------------------
QString UExportInfoPilgovyky::pilga_for(short p_for)
{
	QString str;
	str = QVariant(p_for).toString();
	str = str.rightJustified ( 2, '0', true );
	return str;
}
//------------------------------------------------------------
QString UExportInfoPilgovyky::vids_pilgy(short vids)
{
	QString str;
	str = QVariant(vids).toString();
	str = str.rightJustified ( 3, '0', true );
	return str;
}
//------------------------------------------------------------
QString UExportInfoPilgovyky::pilga_narah(double narah)
{
	QString str;
	str = uMToStr2Krapka(narah);
	str = str.rightJustified ( 6, '0', true );
	return str;
}
//------------------------------------------------------------
QString UExportInfoPilgovyky::spojyvannya(double ploshcha)
{
	QString temp_str;
	QString str;
	temp_str = m_to_str(ploshcha);
	temp_str = temp_str.rightJustified ( 4, '0', true );
	str = temp_str + ".";
	short i;
	i = ploshcha;
	i = (ploshcha - i)*100;
	temp_str = m_to_str(i);
	temp_str = temp_str.leftJustified ( 4, '0', true );
	str += temp_str;
	return str;
}
//------------------------------------------------------------
char * UExportInfoPilgovyky::strToxBaseChar(QString str)
{
	return funcCodec->fromUnicode(str).data();
}
//------------------------------------------------------------
QString UExportInfoPilgovyky::name_of_file()
{
	QString temp_str;
	QString str;
	temp_str = QVariant(yy-2000).toString();
	temp_str = temp_str.rightJustified ( 2, '0', true );
	str = temp_str;
	temp_str = QVariant(mm).toString();
	temp_str = temp_str.rightJustified ( 2, '0', true );
	str += temp_str;
	temp_str = zagolovok_edrpou();
	str += temp_str;
	//str += ".dbf";
	return str;
}
//------------------------------------------------------------
bool UExportInfoPilgovyky::save_to_file()
{
	//Обрахунок кількості записів
	QSqlDatabase db = QSqlDatabase::addDatabase("QODBC", "zvit_pilg");
	QString dirString = QCoreApplication::applicationDirPath();
	QFile::remove(dirString + "\\" + "temp_dbf" + "\\" + "z_pilg.dbf");
    db.setDatabaseName("DRIVER={Driver do Microsoft dBase (*.dbf)};FIL={dBase III};DBQ="+QCoreApplication::applicationDirPath().replace("\\","/")+"/Temp_dbf");
    if (!db.open()) {
        QMessageBox::critical(0, QObject::tr("Помилка підключення до таблиці пільг"),
                              db.lastError().text());
        return false;
    }

	QSqlQuery * query = new QSqlQuery();
	QSqlQuery * saveQuery = new QSqlQuery(db);
	QSqlQuery * pilgovykyQuery = new QSqlQuery();
	
	QString str;
	int num_rec;
	int num_rec_GV;
	
	//чищення таблиці
	if (saveQuery->exec("CREATE TABLE z_pilg(\
						CDPR varchar(12),\
						IDCODE varchar(10),\
						FIO varchar(50),\
						RS varchar(15),\
						YEARIN varchar(4),\
						MONTHIN varchar(2),\
						LGCODE varchar(3),\
						YEAR varchar(4),\
						MONTH varchar(2),\
						LGKOL varchar(2),\
						LGPRC varchar(3),\
						SUMMFACT varchar(7),\
						FACT  varchar(13)\
					)"))
		qDebug() << "Створено нову таблицю для електронного звіту по пільгіх";
	else
		qDebug() << "Використовується стара таблиця для електронного звіту по пільгах";
	saveQuery->exec("DELETE FROM z_pilg");
	//Кількість записів по ЦО
	query->exec("SELECT Count(*) AS Num_rec \
				FROM abonent INNER JOIN narahuvannya ON abonent.Rahunok = narahuvannya.Rahunok \
				WHERE (((narahuvannya.Year)="+QVariant(yy).toString()+") \
				AND ((narahuvannya.Month)="+QVariant(mm).toString()+") \
				AND ((narahuvannya.Pilga_ID)<>0) \
				AND ((narahuvannya.Pilga_sum)<>0))"); 
	if (query->seek(0))
		num_rec = query->value(0).toInt();
	else
		return false;
	
	//Кількість записів по ГВ
	query->exec("SELECT Count(*) AS Num_rec \
				FROM abonent INNER JOIN narahuvannya ON abonent.Rahunok = narahuvannya.Rahunok \
				WHERE (((narahuvannya.Year)="+QVariant(yy).toString()+") \
				AND ((narahuvannya.Month)="+QVariant(mm).toString()+") \
				AND ((narahuvannya.Pilga_ID_GV)<>0) \
				AND ((narahuvannya.Pilga_sum_GV)<>0))"); 
	if (query->seek(0))
		num_rec_GV = query->value(0).toInt();
	else
		return false;
	
	
	QString file_path;
	file_path = dir_name + "\\" + name_of_file();

	int num_saved_records=0;
	UPostupForm *postup = new UPostupForm(0, num_rec+num_rec_GV);
	postup->show();
	// Записи з інформацією про пільговиків для ЦО
	query->exec("SELECT abonent.Identyf, abonent.Prizv, abonent.Imya, abonent.Batk, abonent.Rahunok, \
					abonent.Pilga_for, abonent.Pilga_for_2, \
					narahuvannya.Pilga_sum, pilgy.Vidsotok, abonent.ploshcha, \
					narahuvannya.Status, narahuvannya.Pilga_ID\
					FROM ((narahuvannya INNER JOIN abonent ON abonent.Rahunok = narahuvannya.Rahunok) \
					INNER JOIN pilgy ON pilgy.id = narahuvannya.Pilga_ID) \
					WHERE (((narahuvannya.Year)="+QVariant(yy).toString()+") \
					AND ((narahuvannya.Month)="+QVariant(mm).toString()+") \
					AND ((narahuvannya.Pilga_ID)<>0) \
					AND ((narahuvannya.Pilga_sum)<>0)) \
					ORDER BY abonent.Rahunok");
	query->seek(-1);
	QString edrpouString = OrganizEDRPOU();
	while (query->next()){
		saveQuery->prepare("INSERT INTO z_pilg (CDPR, IDCODE, FIO, RS, YEARIN, MONTHIN, LGCODE, YEAR, MONTH, LGKOL, LGPRC, SUMMFACT, FACT) "
			"VALUES (:CDPR, :IDCODE, :FIO, :RS, :YEARIN, :MONTHIN, :LGCODE, :YEAR, :MONTH, :LGKOL, :LGPRC, :SUMMFACT, :FACT)");
		saveQuery->bindValue(":CDPR", QVariant(edrpouString));
		pilgovykyQuery->exec("SELECT Prizv, Imya, Batk, Ident FROM pilgovyky WHERE Rahunok="+query->value(4).toString()+" and Pilga_ID="+query->value(11).toString());
		if (pilgovykyQuery->seek(0)){
			saveQuery->bindValue(":IDCODE", pilgovykyQuery->value(3));
			QString pibStr = pilgovykyQuery->value(0).toString() + " " + pilgovykyQuery->value(1).toString() + " " + pilgovykyQuery->value(2).toString();
			saveQuery->bindValue(":FIO", pibStr.replace('і','i').replace('І','I'));
		}
		else{
			saveQuery->bindValue(":IDCODE", query->value(0));
			saveQuery->bindValue(":FIO", query->value(1).toString() + " " + query->value(2).toString() + " " + query->value(3).toString());
		}
		saveQuery->bindValue(":RS", query->value(4).toInt());
		saveQuery->bindValue(":YEARIN", yy);
		saveQuery->bindValue(":MONTHIN", mm);
		saveQuery->bindValue(":LGCODE", 505);
		saveQuery->bindValue(":YEAR", yy);
		saveQuery->bindValue(":MONTH", mm);
		if (query->value(10).toInt() == 1)
			saveQuery->bindValue(":LGKOL", query->value(6).toInt());
		else
			saveQuery->bindValue(":LGKOL", query->value(5).toInt());
		saveQuery->bindValue(":LGPRC", query->value(8).toDouble());
		saveQuery->bindValue(":SUMMFACT", query->value(7).toDouble());
		saveQuery->bindValue(":FACT", uMToStr2(query->value(9).toDouble()));
		//saveQuery->bindValue(":FACT", 1);
		
		saveQuery->exec();
		postup->inc_pos();
		num_saved_records++;
	}
	
	// Записи з інформацією про пільговиків для ГВ
	query->exec("SELECT abonent.Identyf, abonent.Prizv, abonent.Imya, abonent.Batk, abonent.Rahunok, \
						abonent.Pilga_for, abonent.Pilga_for_2, \
						narahuvannya.Pilga_sum_GV, pilgy.Vidsotok, abonent.ploshcha, \
						narahuvannya.Status, narahuvannya.Pilga_ID_GV, abonent.ludy\
				FROM ((narahuvannya INNER JOIN abonent ON abonent.Rahunok = narahuvannya.Rahunok) \
				INNER JOIN pilgy ON pilgy.id = narahuvannya.Pilga_ID_GV) \
				WHERE (((narahuvannya.Year)="+QVariant(yy).toString()+") \
					AND ((narahuvannya.Month)="+QVariant(mm).toString()+") \
					AND ((narahuvannya.Pilga_ID_GV)<>0) \
					AND ((narahuvannya.Pilga_sum_GV)<>0)) \
				ORDER BY abonent.Rahunok");
	query->seek(-1);
	while (query->next()){
		saveQuery->prepare("INSERT INTO z_pilg (CDPR, IDCODE, FIO, RS, YEARIN, MONTHIN, LGCODE, YEAR, MONTH, LGKOL, LGPRC, SUMMFACT, FACT) "
			"VALUES (:CDPR, :IDCODE, :FIO, :RS, :YEARIN, :MONTHIN, :LGCODE, :YEAR, :MONTH, :LGKOL, :LGPRC, :SUMMFACT, :FACT)");
		saveQuery->bindValue(":CDPR", QVariant(edrpouString));
		pilgovykyQuery->exec("SELECT Prizv, Imya, Batk, Ident FROM pilgovyky WHERE Rahunok="+query->value(4).toString()+" and Pilga_ID="+query->value(11).toString());
		if (pilgovykyQuery->seek(0)){
			saveQuery->bindValue(":IDCODE", pilgovykyQuery->value(3));
			QString pibStr = pilgovykyQuery->value(0).toString() + " " + pilgovykyQuery->value(1).toString() + " " + pilgovykyQuery->value(2).toString();
			saveQuery->bindValue(":FIO", pibStr.replace('і','i').replace('І','I'));
		}
		else{
			saveQuery->bindValue(":IDCODE", query->value(0));
			saveQuery->bindValue(":FIO", query->value(1).toString() + " " + query->value(2).toString() + " " + query->value(3).toString());
		}
		saveQuery->bindValue(":RS", query->value(4).toInt());
		saveQuery->bindValue(":YEARIN", yy);
		saveQuery->bindValue(":MONTHIN", mm);
		saveQuery->bindValue(":LGCODE", 501);
		saveQuery->bindValue(":YEAR", yy);
		saveQuery->bindValue(":MONTH", mm);
		if (query->value(10).toInt() == 1)
			saveQuery->bindValue(":LGKOL", query->value(6).toInt());
		else
			saveQuery->bindValue(":LGKOL", query->value(5).toInt());
		saveQuery->bindValue(":LGPRC", query->value(8).toDouble());
		saveQuery->bindValue(":SUMMFACT", query->value(7).toDouble());
		saveQuery->bindValue(":FACT", uMToStr2(query->value(12).toDouble()));
		//saveQuery->bindValue(":FACT", 1);
		
		saveQuery->exec();
		postup->inc_pos();
		num_saved_records++;
	}
	
	delete postup;
	
	delete saveQuery;
	delete query;
	delete pilgovykyQuery;
	QSqlDatabase::removeDatabase( "zvit_pilg" );
	
	//Копіювання dbf в вихідне місце
	
	QFile file_temp(dirString + "\\" + "temp_dbf" + "\\" + "z_pilg.dbf");
	QFile file_out(file_path);
	if (file_out.exists()){
		file_out.remove();
	}
	if (!file_temp.copy(file_path)){
		QMessageBox::critical(
					0,
					codec->toUnicode("!!! Запис файла не здійснено !!!"),
					codec->toUnicode("Не можу скопіювати файл з .\\temp_dbf\\ до папки ") + file_path + ".",
					QMessageBox::Ok,
					QMessageBox::NoButton);
		return false;
	}
	
	if (num_saved_records == num_rec + num_rec_GV){
		QMessageBox::information(
					0,
					"!!! Успішно закінчено запис у файл інформації про пільговиків !!!",
					"Успішно зроблено:\n     --"+QString::number(num_rec)+" записів по ЦО;\n     --"+QString::number(num_rec_GV)+" записів по ГВ."
					"\n\nФайл звіту: \""+file_path+"\"",
					QMessageBox::Ok,
					QMessageBox::NoButton);
		return true;
	}
	else
		QMessageBox::critical(
					0,
					"!!! Запис у файл інформації про пільговиків закінчено з помилками !!!",
					"В нарахуванні "+QString::number(num_rec)+" пільговиків ЦО і "+QString::number(num_rec_GV)+" пільговиків ГВ,\n" +
					"а записано "+ QVariant(num_saved_records).toString() +" пільговиків.\n" +
					"Знайдіть у таблиці пільговиків дубляжі (по рахунку і пільзі) і видаліть їх.",
					QMessageBox::Ok,
					QMessageBox::NoButton);
		return false;
}
//------------------------------------------------------------
//При  zFormType==2 записуються усі без винятку категорії пільг (< 200 і < 300 і >=30)
//

bool UExportInfoPilgovyky::save_to_file_v2007()
{
	if (!checkDirCompatibility())
		return false;
	qDebug() << "---- Розпочато створення файлу пільг. Час: " << QTime::currentTime().toString("hh.mm.ss");
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	bool exportCOPilgy = settings->value("GUI_View/ShowNarahCO").toBool();
	bool exportGVPilgy = settings->value("GUI_View/ShowNarahGV").toBool();
	delete settings;
	
	QSqlQuery query, pilgovykyQuery;
	QString str;
	int num_rec;
	int num_rec_GV;
	
	//Читання способу записування обсягів споживання, тарифу та суми пільги
	query.exec("SELECT [value] FROM slujb_znach WHERE id=19");
	query.next();
	char vykorystType = qRound(query.value(0).toDouble());
	if (vykorystType > 1)
		vykorystType = 0;
	
	//Читання тарифів
	UNasTaryfInfo taryfInfo = nasTaryf(yy, mm);
	QHash<short, double> taryfBudCO;
	query.exec("SELECT Budynok_ID, Taryf FROM diuch_taryf WHERE year="+QString::number(yy)+" and month="+QString::number(mm));
	while (query.next()){
		taryfBudCO.insert(query.value(0).toInt(), query.value(1).toDouble());
	}
	QHash<short, int>  pilgyVids;
	query.exec("SELECT id, Vidsotok FROM pilgy");
	while (query.next()){
		pilgyVids.insert(query.value(0).toInt(), query.value(1).toDouble());
	}
	
	//Читання будинків з лічильників
	query.exec("SELECT id FROM Budynky WHERE Lichylnyk=1");
	QList<int> lichylnykyBudCO;
	while (query.next())
		lichylnykyBudCO.append(query.value(0).toInt());
	
	//Читання часових меж надання послуг
	query.exec("SELECT PochDayCO, KincDayCO, PochDayGV, KincDayGV \
				FROM grafikPoslug \
				WHERE Year="+QString::number(yy)+" and month="+QString::number(mm));
	query.next();
	const char defaultBegPoslugDateCO = query.value(0).toInt();
	const char defaultEndPoslugDateCO = query.value(1).toInt();
	const char defaultBegPoslugDateGV = query.value(2).toInt();
	const char defaultEndPoslugDateGV = query.value(3).toInt();
	
	QString pib;
	QString edrpou = zagolovok_edrpou();
	short wellDoneCO=0, errDoneCO=0;
	short wellDoneGV=0, errDoneGV=0;
	double sumPilgCO=0, sumPilgGV=0, pilgVal;
	
	short cur_wellDoneCO=0, cur_errDoneCO=0;
	short cur_wellDoneGV=0, cur_errDoneGV=0;
	double cur_sumPilgCO=0, cur_sumPilgGV=0;
	
	const short daysInMonth = QDate(yy,mm,1).daysInMonth();
	
	query.exec("SELECT strTemplate FROM slujb_znach WHERE id=13");
	query.seek(0);
	QString coPilKod = query.value(0).toString();
	query.exec("SELECT strTemplate FROM slujb_znach WHERE id=14");
	query.seek(0);
	QString gvPilKod = query.value(0).toString();
	
	UPostupForm *postup_form = new UPostupForm(0, 0 );
	postup_form->showLable(true);
	postup_form->setPostupLabel("Підготовка файлу dbf");
	postup_form->show();
	
	xbXBase x;
	xbDbf dbfFile( &x );
	xbSchema zvitRecordShema[] =
	{
		{ "CDPR",		XB_NUMERIC_FLD,12, 0 },
		{ "IDCODE",		XB_CHAR_FLD,   10, 0 },
		{ "FIO",		XB_CHAR_FLD,   50, 0 },
		{ "PPOS",		XB_CHAR_FLD,   15, 0 },
		{ "RS",			XB_CHAR_FLD,   25, 0 },
		{ "YEARIN",		XB_NUMERIC_FLD, 4, 0 },
		{ "MONTHIN",	XB_NUMERIC_FLD, 2, 0 },
		{ "LGCODE",		XB_NUMERIC_FLD, 5, 0 },
		{ "DATA1",		XB_DATE_FLD,    8, 0 },
		{ "DATA2",		XB_DATE_FLD,    8, 0 },
		{ "LGKOL",		XB_NUMERIC_FLD, 2, 0 },
		{ "LGKAT",		XB_CHAR_FLD,    3, 0 }, //По документації потрібно 2
		{ "LGPRC",		XB_NUMERIC_FLD, 3, 0 },
		{ "SUMM",		XB_NUMERIC_FLD, 8, 2 },
		{ "FACT",		XB_NUMERIC_FLD,19, 6 }, //Було 19\6
		{ "TARIF",		XB_NUMERIC_FLD,14, 7 }, //Було 14\3
		{ "FLAG",		XB_NUMERIC_FLD, 1, 0 },
		{ "",0,0,0 }
	};
	
	QString dbfFileName = dir_name.replace("/","\\")+"\\"+name_of_file();
	dbfFile.SetVersion( 3 );
	
	QString fileNameSuf, pIdFilterString, pIdFilterString_GV;
	QSqlQuery pilgaIdQuery;
	QFile reportFile;
	bool createDbfFirstTime=true;
	
	if (zFormType == 2){ //створення файлу звіту процесу експорту пільг
		reportFile.setFileName(dir_name.replace("/","\\")+"\\_Підсумок.html");
		reportFile.open(QIODevice::WriteOnly | QIODevice::Text);
		reportFile.write("<html>\
							<title>Інформація про експорт пільг</title><body leftmargin=5 topmargin=5 marginwidth=5 marginheight=5 style=\"font: normal normal 12px Arial, Tahoma, Verdana, Helvetica, sans-serif\"> \
								<center>ІНФОРМАЦІЯ ПРО ЕКСПОРТ НАРАХУВАНЬ ПІЛЬГ У dbf-ФАЙЛИ</center><br>");
		reportFile.write(("Підприємство: "+OrganizName()+"<br>").toLocal8Bit());
		reportFile.write(("Рік нарахування: "+QString::number(yy)+"<br>").toLocal8Bit());
		reportFile.write(("Місяць нарахування: "+QString::number(mm)+"<br>").toLocal8Bit());
		reportFile.write(("Час експорту: "+QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm")+"<br><br>").toLocal8Bit());
		reportFile.write("<table border=1 cellpadding=1 cellspacing=0 style=\"font: normal normal 12px Arial, Tahoma, Verdana, Helvetica, sans-serif\">\
									<tr align=center><td>Пільга</td><td>ІД пільги</td><td>К-сть записів ЦО</td><td>Сума пільги ЦО</td><td>К-сть записів ГВ</td><td>Сума пільги ГВ</td></tr>");
	}
	if (zFormType == 1 || zFormType == 2){ ////задання запиту для отримання переліку категорій пільг для експорту
		pilgaIdQuery.exec("SELECT pilgnar.Pilga_Id, pilgy.Kategoriya FROM \
							( \
							SELECT * FROM \
							( \
								SELECT Pilga_ID FROM narahuvannya \
								WHERE year="+QString::number(yy)+" \
									and month="+QString::number(mm)+" and (Pilga_sum<>0) \
								GROUP BY Pilga_ID) \
								UNION \
								(SELECT Pilga_ID_GV FROM narahuvannya \
								WHERE year="+QString::number(yy)+" \
									and month="+QString::number(mm)+" and (Pilga_sum_GV<>0) \
								GROUP BY Pilga_ID_GV ) \
							) AS pilgnar \
							LEFT JOIN pilgy ON pilgnar.Pilga_ID=pilgy.id \
							GROUP BY pilgnar.Pilga_Id, pilgy.Kategoriya \
							ORDER BY pilgnar.Pilga_Id");
		if (!pilgaIdQuery.next())
			goto END_EXPORT_PILG;
	}
	
NEXT_PILGA_ID: //наступний ідентифікатор пільги

	if (zFormType == 1 || zFormType == 2){ //задання необхідних  змінних
		if (zFormType == 2)
			fileNameSuf = "_"+pilgaIdQuery.value(0).toString().rightJustified(3,'0');
		pIdFilterString = " and Pilga_id="+sqlStr(pilgaIdQuery.value(0).toInt());
		pIdFilterString_GV = " and Pilga_id_GV="+sqlStr(pilgaIdQuery.value(0).toInt());
		
		cur_wellDoneCO=0;
		cur_errDoneCO=0;
		cur_wellDoneGV=0;
		cur_errDoneGV=0;
		cur_sumPilgCO=0;
		cur_sumPilgGV=0;
	}
	
	if (zFormType == 0 || (createDbfFirstTime && zFormType == 1) || zFormType == 2 ){
		createDbfFirstTime = false;
		dbfFile.CloseDatabase();
		if( dbfFile.CreateDatabase((dbfFileName+fileNameSuf).toAscii().data(),zvitRecordShema,XB_OVERLAY) != XB_NO_ERROR) {
			qDebug() << "Не можу створити *.dbf файл: " << dbfFileName+fileNameSuf;
			
			return false;
		}
	}
	qDebug() << "  -- Розпочато запис пільг по ЦО";
	
	if (exportCOPilgy){
		query.exec("SELECT count(*) FROM t_abonent_inf_2 \
				INNER JOIN (SELECT Rahunok, sum(Pilga_sum) AS pilgaSum, Pilga_ID, Pilga_For, \
									Pilga_plos, Status, Year_pilg_pererah, Month_pilg_pererah, PochDayCO, KincDayCO \
							FROM narahuvannya \
							WHERE year="+QString::number(yy)+" \
									and month="+QString::number(mm)+" \
									and Pilga_sum<>0 \
									"+pIdFilterString+" \
							GROUP BY Rahunok, Pilga_ID, Pilga_For, Pilga_plos, Status, Year_pilg_pererah, \
									Month_pilg_pererah, PochDayCO, KincDayCO \
					) AS narahT \
					ON  t_abonent_inf_2.id = narahT.Rahunok");
		query.seek(0);
		postup_form->setPostupLabel("Експорт стрічок пільг по ЦО");
		postup_form->setNumPos(query.value(0).toInt());
		postup_form->setCurrentPos(0);
	
		//Запис пільг по центральному опаленню
		query.exec("SELECT narahT.Rahunok, t_abonent_inf_2.abon_NPR, t_abonent_inf_2.abon_ADR, \
						t_abonent_inf_2.identyf, narahT.pilgaSum, narahT.Pilga_ID, narahT.Pilga_for, \
						narahT.Pilga_plos, narahT.Status, narahT.Year_pilg_pererah, \
						narahT.Month_pilg_pererah, narahT.PochDayCo, narahT.KincDayCo, abonent.Budynok_ID \
				FROM (t_abonent_inf_2 \
				INNER JOIN (SELECT Rahunok, sum(Pilga_sum) AS pilgaSum, Pilga_ID, Pilga_For, \
									Pilga_plos, Status, Year_pilg_pererah, Month_pilg_pererah, PochDayCO, KincDayCO \
							FROM narahuvannya \
							WHERE year="+QString::number(yy)+" \
									and month="+QString::number(mm)+" \
									and Pilga_sum<>0 \
									"+pIdFilterString+" \
							GROUP BY Rahunok, Pilga_ID, Pilga_For, Pilga_plos, Status, Year_pilg_pererah, \
									Month_pilg_pererah, PochDayCO, KincDayCO \
					) AS narahT \
					ON  t_abonent_inf_2.id = narahT.Rahunok) \
				LEFT JOIN abonent ON abonent.Rahunok=narahT.Rahunok\
				ORDER BY narahT.Pilga_ID, t_abonent_inf_2.abon_NPR");
		while (query.next()){
		  if (query.value(5).toInt() < 300 || zFormType==2){
			dbfFile.BlankRecord();
			dbfFile.PutField( "CDPR", strToxBaseChar(edrpou));
			
			short pilgaNumber = query.value(8).toInt();
			if (pilgaNumber == 0)
				pilgaNumber = 1;
			else if (pilgaNumber == 1)
				pilgaNumber = 2;
			else if (pilgaNumber == 11)
				pilgaNumber = 1;
			else if (pilgaNumber == 12)
				pilgaNumber = 2;
			pilgovykyQuery.exec("SELECT Prizv, Imya, Batk, Ident, Grupa, PosvidSeriya, PosvidNomer FROM pilgovyky \
							WHERE Rahunok="+query.value(0).toString()+" \
								and Pilga_ID="+query.value(5).toString()+" \
								and (PilgaNumber="+sqlStr(pilgaNumber)+" or PilgaNumber=0 or PilgaNumber is null) \
							ORDER BY PilgaNumber DESC, Prizv, Imya, Batk");
			if (pilgovykyQuery.seek(0)){
				dbfFile.PutField( "IDCODE", strToxBaseChar(pilgovykyQuery.value(3).toString()) );
				pib = pilgovykyQuery.value(0).toString()+" "+pilgovykyQuery.value(1).toString()+" "+pilgovykyQuery.value(2).toString();
				if (pib.size()>50)
					pib.resize(50);
				dbfFile.PutField( "FIO", strToxBaseChar(pib.toUpper().replace("І","I")));
				pib = pilgovykyQuery.value(5).toString()+" "+pilgovykyQuery.value(6).toString();
				if (pib.size()>15)
					pib.resize(15);
					dbfFile.PutField( "PPOS", strToxBaseChar(pib.toUpper()) );
			}
			else{
				dbfFile.PutField( "IDCODE", strToxBaseChar(query.value(3).toString().toUpper()) );
				pib = query.value(1).toString();
				if (pib.size()>50)
					pib.resize(50);
				dbfFile.PutField( "FIO", strToxBaseChar(pib.toUpper().replace("І","I")));
				//dbfFile.PutField( "PPOS", "КВП 154" );
			}
			dbfFile.PutLongField( "RS", query.value(0).toInt() );
			dbfFile.PutField( "YEARIN", strToxBaseChar(year_narah(yy)) );
			dbfFile.PutField( "MONTHIN", strToxBaseChar(month_narah(mm)) );
			dbfFile.PutField( "LGCODE", coPilKod.toLocal8Bit().constData() ); //0505
		
			short pYear, pMonth, pBegDate, pEndDate;
			if (query.value(9).toInt()>2000 and query.value(10).toInt()>0 and query.value(10).toInt()<13){
				pYear = query.value(9).toInt();
				pMonth = query.value(10).toInt();
			}
			else{
				pYear = yy;
				pMonth = mm;
			}
			if (query.value(11).toInt()>0 and query.value(11).toInt()<=daysInMonth)
				pBegDate = query.value(11).toInt();
			else
				pBegDate = defaultBegPoslugDateCO;
			if (query.value(12).toInt()>0 and query.value(12).toInt()<=daysInMonth)
				pEndDate = query.value(12).toInt();
			else
				pEndDate = defaultEndPoslugDateCO;
			dbfFile.PutField( "DATA1", strToxBaseChar(QDate(pYear,pMonth,pBegDate).toString("yyyyMMdd")) );
			dbfFile.PutField( "DATA2", strToxBaseChar(QDate(pYear,pMonth,pEndDate).toString("yyyyMMdd")) );
			dbfFile.PutLongField( "LGKOL", query.value(6).toInt() );
			if (query.value(5).toInt() < 200 || zFormType==2)
				dbfFile.PutLongField( "LGKAT", query.value(5).toInt());
			
			int curPilgaVids = pilgyVids.value(query.value(5).toInt(),0);
			if (curPilgaVids > 0)
				dbfFile.PutLongField( "LGPRC", curPilgaVids );
			else{
				dbfFile.PutLongField( "LGPRC", 0 );
				qDebug() << "Для абонента з рах. " << query.value(0).toInt() << " внесена неіснуюча або нульова категорія пільги: " << query.value(5).toString() << ".";
			}
			pilgVal = d_to_m(query.value(4).toDouble());
			dbfFile.PutDoubleField( "SUMM", pilgVal );
			
			if (vykorystType == 0){ //схема використання діючого тарифу
				dbfFile.PutField( "FACT", uMToStr2Krapka(query.value(7).toDouble()).toLocal8Bit() );
				double curTaryf = taryfBudCO.value(query.value(13).toInt(),taryfInfo.normat_taryf_kvadr);
				if (curTaryf > 0.009)
					dbfFile.PutDoubleField( "TARIF", curTaryf);
				else
					dbfFile.PutDoubleField( "TARIF", 0);
			}
			else{ //схема використання нормативного тарифу і показників ГКал у випадку наявності лічильника
				if (lichylnykyBudCO.contains(query.value(13).toInt())){ //абонент живе у будинку з лічильником ЦО
					dbfFile.PutDoubleField( "FACT", pilgVal * 100 / (taryfInfo.vart_g_kal * curPilgaVids) );
					dbfFile.PutField( "TARIF", uMToStr2Krapka(taryfInfo.vart_g_kal).toLocal8Bit());
				}
				else{ //абонент живе у будинку у якому немає лічильника по ЦО
					dbfFile.PutField( "FACT", uMToStr2Krapka(query.value(7).toDouble()).toLocal8Bit());
					dbfFile.PutField( "TARIF", uMToStr2Krapka(taryfInfo.normat_taryf_kvadr).toLocal8Bit());
					/*dbfFile.PutDoubleField( "FACT", 0 );
					double curTaryf = taryfBudCO.value(query.value(13).toInt(),taryfInfo.normat_taryf_kvadr);
					if (curTaryf > 0.009)
						dbfFile.PutDoubleField( "TARIF", curTaryf);
					else
						dbfFile.PutDoubleField( "TARIF", 0);
					*/
					
					if ( d_to_m((pEndDate-pBegDate+1) * query.value(7).toDouble()
							* taryfInfo.normat_taryf_kvadr * curPilgaVids / (100 * daysInMonth)) != pilgVal){
						++errDoneCO;
						qDebug() << "Рахунок: " << query.value(0).toInt() << "; "
								"Код пільги: " << query.value(5).toInt() << "; "
								"Відсотки: " << curPilgaVids << "; "
								"Сума: " << pilgVal << "; "
								"Сума перевірки: " << d_to_m((pEndDate-pBegDate+1) * query.value(7).toDouble()
									* taryfInfo.normat_taryf_kvadr * curPilgaVids / (100 * daysInMonth)) << "; "
								"Пільгова площа (\"FACT\"): " << query.value(7).toDouble() << "; "
								"Тариф: " << taryfInfo.normat_taryf_kvadr << "; "
								"Поч.день.пільги: " << pBegDate << "; "
								"Кін.день.пільги: " << pEndDate << ";";
					}
				}
			}
			
			if (query.value(9).toInt() >2000 and query.value(10).toInt()>0 and query.value(10).toInt()<13)
				dbfFile.PutLongField( "FLAG", 1 );
			else
				dbfFile.PutLongField( "FLAG", 0 );
			if (dbfFile.AppendRecord() == XB_NO_ERROR){
				wellDoneCO++;
				sumPilgCO += pilgVal;
				
				cur_wellDoneCO++;
				cur_sumPilgCO += pilgVal;
			}
			else{
				errDoneCO++;
				cur_errDoneCO++;
			}
		  }
		  postup_form->inc_pos();
		}
	}
	
	qDebug() << "  -- Завершено запис пільг по ЦО";
	qDebug() << "  -- Розпочато запис пільг по ГВ";
	
	if (exportGVPilgy){
		query.exec("SELECT count(*) FROM t_abonent_inf_2 \
					INNER JOIN (SELECT Rahunok, sum(Pilga_sum_GV) AS pilgaSum, Pilga_ID_GV, Pilga_for_GV, \
									Obsyag_GV, Status, Year_pilg_pererah, Month_pilg_pererah, PochDayGV, KincDayGV \
							FROM narahuvannya \
							WHERE year="+QString::number(yy)+" \
									and month="+QString::number(mm)+" \
									and Pilga_sum_GV<>0 \
									"+pIdFilterString_GV+" \
							GROUP BY Rahunok, Pilga_ID_GV, Pilga_for_GV, Obsyag_GV, Status, Year_pilg_pererah, \
									Month_pilg_pererah, PochDayGV, KincDayGV \
					) AS narahT \
					ON  t_abonent_inf_2.id = narahT.Rahunok");
		qDebug() << query.lastQuery();
		query.seek(0);
		postup_form->setPostupLabel("Експорт стрічок пільг по ГАРЯЧІЙ ВОДІ");
		postup_form->setNumPos(query.value(0).toInt());
		postup_form->setCurrentPos(0);
		
		//Запис пільг по гарячій воді
		query.exec("SELECT narahT.Rahunok, t_abonent_inf_2.abon_NPR, t_abonent_inf_2.abon_ADR, \
						t_abonent_inf_2.identyf, narahT.pilgaSum, narahT.Pilga_ID_GV, narahT.Pilga_for_GV, \
						narahT.Obsyag_GV, narahT.Status, narahT.Year_pilg_pererah, \
						narahT.Month_pilg_pererah, narahT.PochDayGV, narahT.KincDayGV, abonent.Budynok_ID, narahT.Is_Lich_GV \
				FROM (t_abonent_inf_2 \
				INNER JOIN (SELECT Rahunok, sum(Pilga_sum_GV) AS pilgaSum, Pilga_ID_GV, Pilga_for_GV, \
									Obsyag_GV, Status, Year_pilg_pererah, Month_pilg_pererah, PochDayGV, KincDayGV, Is_Lich_GV \
							FROM narahuvannya \
							WHERE year="+QString::number(yy)+" \
									and month="+QString::number(mm)+" \
									and Pilga_sum_GV<>0 \
									"+pIdFilterString_GV+" \
							GROUP BY Rahunok, Pilga_ID_GV, Pilga_for_GV, Obsyag_GV, Status, Year_pilg_pererah, \
									Month_pilg_pererah, PochDayGV, KincDayGV, Is_Lich_GV \
					) AS narahT \
					ON  t_abonent_inf_2.id = narahT.Rahunok) \
				LEFT JOIN abonent ON abonent.Rahunok=narahT.Rahunok\
				ORDER BY narahT.Pilga_ID_GV, t_abonent_inf_2.abon_NPR");
		qDebug() << "--------------";
		qDebug() << query.lastQuery();
		while (query.next()){
		  if (query.value(5).toInt() < 300 || zFormType==2){
			dbfFile.BlankRecord();
			dbfFile.PutField( "CDPR", strToxBaseChar(edrpou));
			
			short pilgaNumber = query.value(8).toInt();
			if (pilgaNumber == 0)
				pilgaNumber = 1;
			else if (pilgaNumber == 1)
				pilgaNumber = 2;
			else if (pilgaNumber == 11)
				pilgaNumber = 1;
			else if (pilgaNumber == 12)
				pilgaNumber = 2;
			pilgovykyQuery.exec("SELECT Prizv, Imya, Batk, Ident, Grupa, PosvidSeriya, PosvidNomer FROM pilgovyky \
							WHERE Rahunok="+query.value(0).toString()+" \
								and Pilga_ID="+query.value(5).toString()+" \
								and (PilgaNumber="+sqlStr(pilgaNumber)+" or PilgaNumber=0 or PilgaNumber is null) \
							ORDER BY PilgaNumber DESC, Prizv, Imya, Batk");
			if (pilgovykyQuery.seek(0)){
				dbfFile.PutField( "IDCODE", strToxBaseChar(pilgovykyQuery.value(3).toString()) );
				pib = pilgovykyQuery.value(0).toString()+" "+pilgovykyQuery.value(1).toString()+" "+pilgovykyQuery.value(2).toString();
				if (pib.size()>50)
					pib.resize(50);
				dbfFile.PutField( "FIO", strToxBaseChar(pib.toUpper().replace("І","I")));
				pib = pilgovykyQuery.value(5).toString()+" "+pilgovykyQuery.value(6).toString();
				if (pib.size()>15)
					pib.resize(15);
					dbfFile.PutField( "PPOS", strToxBaseChar(pib.toUpper()) );
			}
			else{
				dbfFile.PutField( "IDCODE", strToxBaseChar(query.value(3).toString().toUpper()) );
				pib = query.value(1).toString();
				if (pib.size()>50)
					pib.resize(50);
				dbfFile.PutField( "FIO", strToxBaseChar(pib.toUpper().replace("І","I")));
				//dbfFile.PutField( "PPOS", "КВП 154" );
			}
			dbfFile.PutLongField( "RS", query.value(0).toInt() );
			dbfFile.PutField( "YEARIN", strToxBaseChar(year_narah(yy)) );
			dbfFile.PutField( "MONTHIN", strToxBaseChar(month_narah(mm)) );
			dbfFile.PutField( "LGCODE", gvPilKod.toLocal8Bit().constData() ); //0501
		
			short pYear, pMonth, pBegDate, pEndDate;
			if (query.value(9).toInt()>2000 and query.value(10).toInt()>0 and query.value(10).toInt()<13){
				pYear = query.value(9).toInt();
				pMonth = query.value(10).toInt();
			}
			else{
				pYear = yy;
				pMonth = mm;
			}
			if (query.value(11).toInt()>0 and query.value(11).toInt()<=daysInMonth)
				pBegDate = query.value(11).toInt();
			else
				pBegDate = defaultBegPoslugDateGV;
			if (query.value(12).toInt()>0 and query.value(12).toInt()<=daysInMonth)
				pEndDate = query.value(12).toInt();
			else
				pEndDate = defaultEndPoslugDateGV;
			dbfFile.PutField( "DATA1", strToxBaseChar(QDate(pYear,pMonth,pBegDate).toString("yyyyMMdd")) );
			dbfFile.PutField( "DATA2", strToxBaseChar(QDate(pYear,pMonth,pEndDate).toString("yyyyMMdd")) );
			dbfFile.PutLongField( "LGKOL", query.value(6).toInt() );
			if (query.value(5).toInt() < 200 || zFormType==2)
				dbfFile.PutLongField( "LGKAT", query.value(5).toInt() );
			int curPilgaVids = pilgyVids.value(query.value(5).toInt(),0);
			if (curPilgaVids > 0)
				dbfFile.PutLongField( "LGPRC", curPilgaVids );
			else{
				dbfFile.PutLongField( "LGPRC", 0 );
				qDebug() << "Для абонента з рах. " << query.value(0).toInt() << " внесена неіснуюча або нульова категорія пільги: " << query.value(5).toString() << ".";
			}
			pilgVal = query.value(4).toDouble();
			dbfFile.PutDoubleField( "SUMM", d_to_m(pilgVal) );
			dbfFile.PutDoubleField( "FACT", query.value(7).toDouble() );
			double curTaryf = 0;
			if (query.value(14).toBool())
				curTaryf = taryfInfo.vart_kuba_GV;
			else
				curTaryf = taryfInfo.vart_na_lud_in_month_GV;
			if (curTaryf > 0.009)
				dbfFile.PutDoubleField( "TARIF", curTaryf);
			else
				dbfFile.PutDoubleField( "TARIF", 0);
			if (query.value(9).toInt() >2000 and query.value(10).toInt()>0 and query.value(10).toInt()<13)
				dbfFile.PutLongField( "FLAG", 1 );
			else
				dbfFile.PutLongField( "FLAG", 0 );
			if (dbfFile.AppendRecord() == XB_NO_ERROR){
				wellDoneGV++;
				sumPilgGV += pilgVal;
				
				cur_wellDoneGV++;
				cur_sumPilgGV += pilgVal;
			}
			else
				cur_errDoneGV++;
		  }
		  postup_form->inc_pos();
		}
	}
	
	qDebug() << "  -- Завершено запис пільг по ГВ";
	
	if (zFormType == 2){
		reportFile.write(("\
						<tr align=left>\
							<td>"+pilgaIdQuery.value(1).toString()+"</td>\
							<td align=center>"+pilgaIdQuery.value(0).toString()+"</td>\
							<td align=right>"+QString::number(cur_wellDoneCO)+"</td>\
							<td align=right>"+uMToStr2(cur_sumPilgCO)+"</td>\
							<td align=right>"+QString::number(cur_wellDoneGV)+"</td>\
							<td align=right>"+uMToStr2(cur_sumPilgGV)+"</td>\
						</tr>").toLocal8Bit());
	}
	if (zFormType == 1 || zFormType == 2){
		if (pilgaIdQuery.next()){
			if (zFormType == 1){ //Запис пустої стрічки
				dbfFile.BlankRecord();
				dbfFile.AppendRecord();
			}
			goto NEXT_PILGA_ID;
		}
	}
	
END_EXPORT_PILG:
	
	if (zFormType == 2){
		reportFile.write(("\
						<tr align=center>\
							<td>-</td>\
							<td>-</td>\
							<td align=right>"+QString::number(wellDoneCO)+"</td>\
							<td align=right>"+uMToStr2(sumPilgCO)+"</td>\
							<td align=right>"+QString::number(wellDoneGV)+"</td>\
							<td align=right>"+uMToStr2(sumPilgGV)+"</td>\
						</tr>").toLocal8Bit());
		reportFile.write("</table></html>");
		reportFile.close();
		
		cur_wellDoneCO=0;
		cur_errDoneCO=0;
		cur_wellDoneGV=0;
		cur_errDoneGV=0;
		cur_sumPilgCO=0;
		cur_sumPilgGV=0;
	}
	
	delete postup_form;
	
	QString resStr;
	if (exportCOPilgy){
		resStr += "\n      ЦО:";
		resStr += "\n   Зроблено записів пільг по ЦО: "+QString::number(wellDoneCO)+".";
		resStr += "\n    Помилки записів пільг по ЦО: "+QString::number(errDoneCO)+".";
		resStr += "\n     Сума записаних пільг по ЦО: "+uMToStr2(sumPilgCO)+".";
	}
	if (exportGVPilgy){
		resStr += "\n      ГВ:";
		resStr += "\n   Зроблено записів пільг по ГВ: "+QString::number(wellDoneGV)+".";
		resStr += "\n    Помилки записів пільг по ГВ: "+QString::number(errDoneGV)+".";
		resStr += "\n     Сума записаних пільг по ГВ: "+uMToStr2(sumPilgGV)+".";
	}
	
	if (errDoneCO+errDoneGV == 0){
		QMessageBox::information(0,
				"!!! Успішний експорт пільг !!!",
				"Успішно завершено експорт пільг."+resStr,
				QMessageBox::Ok,
				QMessageBox::NoButton);
	}
	else{
		QMessageBox::critical(0,
				"!!! Експорт пільг !!!",
				"Експорт пільг завершено з помилками."+resStr,
				QMessageBox::Ok,
				QMessageBox::NoButton);
	}
	qDebug() << "---- Завершено запис файлу пільг";
	return true;
}
//------------------------------------------------------------
bool UExportInfoPilgovyky::checkDirCompatibility()
{
	return true;
}
//------------------------------------------------------------
UExportInfoPilgovyky::~UExportInfoPilgovyky()
{
	
}
//------------------------------------------------------------
