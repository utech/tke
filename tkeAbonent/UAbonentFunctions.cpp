//------------------------------------------------------------
//
// Implamantation of tke_Form_edit_pokazn_lich class.
// Created by Roman Mulyk 18.12.2008
//
//------------------------------------------------------------

#include <QtDebug>
#include <QtSql>
#include <QMessageBox>
#include <USqlAccessible>
#include <UPostupForm>
#include "tke_math.h"

#include "UAbonentFunctions.h"
#include <xbase64.h>

//------------------------------------------------------------
bool znyattya_narahuv_vid_vidkluchenyh(int rahunok, QDate begDate, QDate endDate)
{
	bool doneOk = true;
	begDate = QDate(begDate.year(), begDate.month(),1);
	endDate = QDate(endDate.year(), endDate.month(),endDate.daysInMonth());
	short year = endDate.addMonths(1).year();
	short month = endDate.addMonths(1).month();
	double narahCOSum=0, narahGVSum=0, narahSTSum=0;
	QSqlQuery query, insQuery;
	
	query.exec("SELECT count(*) FROM narahuvannya \
				WHERE Rahunok="+sqlStr(rahunok)+" \
					AND cdate(year&'-'&month&'-01') \
						BETWEEN cdate("+sqlStr(begDate.toString("yyyy-MM-dd"))+") \
							AND cdate("+sqlStr(endDate.toString("yyyy-MM-dd"))+")");
	query.seek(0);
	UPostupForm *postup = new UPostupForm(0, query.value(0).toInt());
	postup->show();
	query.exec("SELECT * FROM narahuvannya \
				WHERE Rahunok="+sqlStr(rahunok)+" \
					AND cdate(year&'-'&month&'-01') \
						BETWEEN cdate("+sqlStr(begDate.toString("yyyy-MM-dd"))+") \
							AND cdate("+sqlStr(endDate.toString("yyyy-MM-dd"))+") \
				ORDER BY cdate(year&'-'&month&'-01'), status");
	//Обробка нарахувань місяців занулення і запис необхідних віднімаючих стрічок в останній місяць
	while (query.next()){
		QSqlRecord rec = query.record();
		short prevYear = rec.value("Year").toInt();
		short prevMonth = rec.value("Month").toInt();
		narahCOSum += rec.value("Narah").toDouble();
		narahGVSum += rec.value("Narah_GV").toDouble();
		narahSTSum += rec.value("Narah_St").toDouble();
		if (rec.value("Pilga_sum").toDouble()!=0 || rec.value("Pilga_sum_GV").toDouble()!=0){ //Запис пільгових стрічок у разі необхідності, але з нульовими нарахуваннями
			short statusOpt = rec.value("Status").toInt();
			if (statusOpt == 0)
				statusOpt = 11;
			else if (statusOpt == 1)
				statusOpt = 12;
			if (!insQuery.exec("INSERT INTO narahuvannya (id, Rahunok, Year, Month, \
													Vh_saldo, \
													Narah, Pilga_sum, Pilga_ID, Pilga_for, Pilga_plos, \
													Status, \
													Narah_GV, Pilga_sum_GV, Pilga_ID_GV, Pilga_for_GV, Obsyag_GV, Is_Lich_GV, \
													Narah_St, \
													Year_pilg_pererah, Month_pilg_pererah, \
													PochDayCO, KincDayCO, \
													PochDayGV, KincDayGV) \
								VALUES ("+sqlStr(nextLichylnyk("narahuvannya"))+", "+sqlStr(rahunok)+", "+sqlStr(year)+", "+sqlStr(month)+", \
													"+sqlStr(rec.value("Vh_saldo").toDouble())+", \
													0, "+sqlStr(rec.value("Pilga_sum").toDouble())+", "+sqlStr(rec.value("Pilga_ID").toDouble())+", "+sqlStr(rec.value("Pilga_for").toDouble())+", "+sqlStr(rec.value("Pilga_plos").toDouble())+", \
													"+sqlStr(statusOpt)+", \
													0, "+sqlStr(rec.value("Pilga_sum_GV").toDouble())+", "+sqlStr(rec.value("Pilga_ID_GV").toDouble())+", "+sqlStr(rec.value("Pilga_for_GV").toDouble())+", "+sqlStr(rec.value("Obsyag_GV").toDouble())+", "+sqlStr(rec.value("Is_Lich_GV").toDouble())+", \
													0, \
													"+sqlStr(prevYear)+", "+sqlStr(prevMonth)+", \
													"+sqlStr(rec.value("PochDayCO").toDouble())+", "+sqlStr(rec.value("KincDayCO").toDouble())+", \
													"+sqlStr(rec.value("PochDayGV").toDouble())+", "+sqlStr(rec.value("KincDayGV").toDouble())+")")){
				doneOk = false;
				qDebug() << "Помилка ВНЕСЕННЯ запису пільги до таблиці narahuvannya. \nЗапит: " << insQuery.lastQuery() << "\n Причина: " << insQuery.lastError().text();
			}
		}
		postup->inc_pos();
	}
	// Зміна/запис арахування в останньому місяці
	if ( narahCOSum>0.009 || narahCOSum<-0.009 || narahGVSum>0.009 || narahGVSum<-0.009 || narahSTSum>0.009 || narahSTSum<-0.09 ){
		//Запис в таблицю narahuvannya
		query.exec("SELECT id, Narah, Narah_GV, Narah_St \
					FROM narahuvannya \
					WHERE rahunok="+sqlStr(rahunok)+" and \
						year="+sqlStr(year)+" \
						and month="+sqlStr(month)+" \
						and status=0");
		if (query.seek(0)){
			if (!insQuery.exec("UPDATE narahuvannya SET \
								Narah="+sqlStr(query.value(1).toDouble() - narahCOSum)+", \
								Narah_GV="+sqlStr(query.value(2).toDouble() - narahGVSum)+", \
								Narah_St="+sqlStr(query.value(3).toDouble() - narahSTSum)+" \
							WHERE id="+query.value(0).toString())){
				doneOk = false;
				qDebug() << "Помилка ЗМІНИ запису нарахування в таблиці narahuvannya. \nЗапит: " << insQuery.lastQuery() << "\n Причина: " << insQuery.lastError().text();
			}
		}
		else{
			insQuery.exec("SELECT pochDayCO, kincDayCO, pochDayGV, KincDayGV \
							FROM grafikPoslug \
							WHERE year="+QString::number(year)+" \
								and month="+QString::number(month));
			insQuery.seek(0);
			if (!insQuery.exec("INSERT INTO narahuvannya (id, Rahunok, Year, Month, Narah, Narah_GV, Narah_ST, \
													Vh_saldo, Status, PochDayCO, KincDayCO, PochDayGV, KincDayGV) \
								VALUES ("+sqlStr(nextLichylnyk("narahuvannya"))+", "+sqlStr(rahunok)+", "+sqlStr(year)+", "+sqlStr(month)+", "+sqlStr(-narahCOSum)+", "+sqlStr(-narahGVSum)+", "+sqlStr(-narahSTSum)+", \
													"+sqlStr(vhSaldoInMonth(rahunok,year,month))+", 0, "+insQuery.value(0).toString()+", "+insQuery.value(1).toString()+", "+insQuery.value(2).toString()+", "+insQuery.value(3).toString()+")")){
				doneOk = false;
				qDebug() << "Помилка ВНЕСЕННЯ запису нархування до таблиці narahuvannya. \nЗапит: " << insQuery.lastQuery() << "\n Причина: " << insQuery.lastError().text();
			}
		}
		//Запис в таблицю korektura_s
		if (!insQuery.exec("INSERT INTO korektura_s (increment, id, \
										suma, suma_GV, suma_ST, \
										month, year, pryznach) \
								VALUES ("+sqlStr(nextLichylnyk("korektura_s", "increment"))+", "+sqlStr(rahunok)+", \
										"+sqlStr(-narahCOSum)+", "+sqlStr(-narahGVSum)+", "+sqlStr(-narahSTSum)+", \
										"+sqlStr(month)+", "+sqlStr(year)+", 0) ")){
			doneOk = false;
			qDebug() << "Помилка запису коректур по сумі до таблиці korektura_s. \nЗапит: " << insQuery.lastQuery() << "\n Причина: " << insQuery.lastError().text();
		}
	}
	delete postup;
	return doneOk;
}
//------------------------------------------------------------
void export_vidomist_narahuvannya_po_lichylnyku(short year, short month, QString fileToSave)
{
	
	QSqlQuery query;
	UPostupForm *postup_form = new UPostupForm(0, 0);
	postup_form->showLable(true);
	postup_form->setPostupLabel("Створення відомості");
	postup_form->show();
	
	query.exec("SELECT count(*) FROM (SELECT abonent.rahunok, sum(narahuvannya.Narah) AS narahCO, \
				sum(narahuvannya.Narah_GV) AS NarahGV \
				FROM (abonent \
				LEFT JOIN budynky ON budynky.id=abonent.Budynok_id) \
				LEFT JOIN (SELECT * FROM narahuvannya \
				WHERE year="+sqlStr(year)+" and month="+sqlStr(month)+") AS narahuvannya ON narahuvannya.Rahunok=abonent.Rahunok \
				WHERE (EXISTS (SELECT lichylnykyAbon.Rahunok FROM lichylnykyAbon WHERE \
					lichylnykyAbon.rahunok=abonent.Rahunok and lichylnykyAbon.isLichylnyk=true)) or (budynky.Lichylnyk=1 \
					and abonent.Opal=true) or (EXISTS (SELECT lichylnykyBudynkovi.id FROM lichylnykyBudynkovi WHERE \
					lichylnykyBudynkovi.Budynok_ID=abonent.Budynok_ID and lichylnykyBudynkovi.isLichylnyk=true)) \
				GROUP BY abonent.rahunok) \
				WHERE narahCO>0.009 or narahGV>0.009");
	
	query.seek(0);
	int rowCount = query.value(0).toInt();
	postup_form->setNumPos(rowCount);
	
	UxBaseDbfInfo c("IBM 866");
	xbXBase x;
	xbDbf dbfFile( &x );
	xbSchema zvitRecordShema[] =
	{
		{ "YEAR",		XB_NUMERIC_FLD,	4, 0 },
		{ "MONTH",		XB_NUMERIC_FLD,	2, 0 },
		{ "RAHUNOK",	XB_NUMERIC_FLD,	6, 0 },
		{ "PIB",		XB_CHAR_FLD,	40, 0 },
		{ "ADRESA",		XB_CHAR_FLD,	50, 0 },
		{ "PLOS",		XB_NUMERIC_FLD,	12, 6 },
		{ "VYKOR_GV",	XB_NUMERIC_FLD,	12, 6 },
		{ "SUMA_CO",	XB_NUMERIC_FLD,	7, 2 },
		{ "SUMA_GV",	XB_NUMERIC_FLD,	7, 2 },
		{ "SUMA_ZAG",	XB_NUMERIC_FLD, 7, 2 },
		{ "",0,0,0 }
	};
	
	dbfFile.SetVersion( 3 );
	if( dbfFile.CreateDatabase(fileToSave.replace("/","\\").toAscii().data(),zvitRecordShema,XB_OVERLAY) != XB_NO_ERROR) {
		qDebug() << "Не можу створити *.dbf файл: " << fileToSave;
		delete postup_form;
		QMessageBox::critical(0, "Експорт відомості нарахування по лічильниках", 
		"Помилка створення dbf файлу з назвою:\n"+fileToSave+"."
		);
		return;
	}
	
	
	query.exec("SELECT * FROM (SELECT abonent.rahunok, abonent.Prizv, abonent.Imya, abonent.Batk, budynky.Vulycia, \
					budynky.Bud_num, abonent.Kvartyra, sum(narahuvannya.Narah) AS narahCO, \
					sum(narahuvannya.Narah_GV) AS NarahGV, abonent.ludy, abonent.Ploshcha \
				FROM (abonent \
				LEFT JOIN budynky ON budynky.id=abonent.Budynok_id) \
				LEFT JOIN (SELECT * FROM narahuvannya \
				WHERE year="+sqlStr(year)+" and month="+sqlStr(month)+") AS narahuvannya ON narahuvannya.Rahunok=abonent.Rahunok \
				WHERE (EXISTS (SELECT lichylnykyAbon.Rahunok FROM lichylnykyAbon WHERE \
					lichylnykyAbon.rahunok=abonent.Rahunok and lichylnykyAbon.isLichylnyk=true)) or (budynky.Lichylnyk=1 \
					and abonent.Opal=true) or (EXISTS (SELECT lichylnykyBudynkovi.id FROM lichylnykyBudynkovi WHERE \
					lichylnykyBudynkovi.Budynok_ID=abonent.Budynok_ID and lichylnykyBudynkovi.isLichylnyk=true))\
				GROUP BY abonent.rahunok, abonent.Prizv, abonent.Imya, abonent.Batk, budynky.Vulycia, \
					budynky.Bud_num, abonent.Kvartyra, abonent.ludy, abonent.Ploshcha \
				ORDER BY abonent.rahunok) \
				WHERE narahCO>0.009 or narahGV>0.009");
	
	int iter=0;
	QString adrStr;
	UNarah_data *d = new UNarah_data;
	d->nDate = QDate(year, month, 1);
	d->normaGV = normaSpojyvGVNaOsobu(year,month);
	double sSum=0, kubSum=0, opalSum=0, gvSum=0, val;
	while (query.next()){
		dbfFile.BlankRecord();
		
		//Рік
		dbfFile.PutLongField( "YEAR", year);
		
		//Місяць
		dbfFile.PutLongField( "MONTH", month);
		
		//"рахунок"
		dbfFile.PutLongField( "RAHUNOK", query.value(0).toInt());
		
		//"ПІБ"
		dbfFile.PutField( "PIB", c.strToxBaseDosChar((query.value(1).toString() + " " + query.value(2).toString() + " " + query.value(3).toString())));
		
		//"адреса"
		adrStr = query.value(4).toString() + " " + query.value(5).toString();
		if (query.value(6).toInt()>0)
			adrStr += ", кв." + query.value(6).toString();
		dbfFile.PutField( "ADRESA", c.strToxBaseDosChar(adrStr));
		
		//"ЦО, м.кв."
		val = query.value(10).toDouble();
		sSum += val;
		dbfFile.PutDoubleField( "PLOS", val);
		
		//"м.куб"
		d->rah = query.value(0).toInt();
		d->ludy = query.value(9).toInt();
		val = f_read_vykor_GV(d);
		kubSum += val;
		dbfFile.PutDoubleField( "VYKOR_GV", val);
		
		//"Сума ЦО грн"
		val = d_to_m(query.value(7).toDouble());
		opalSum += val;
		dbfFile.PutDoubleField( "SUMA_CO", val);

		//"Сума ГВ, грн"
		val = d_to_m(query.value(8).toDouble());
		gvSum += val;
		dbfFile.PutDoubleField( "SUMA_GV", val);
	
		//"Сума, грн"
		dbfFile.PutDoubleField( "SUMA_ZAG", d_to_m(query.value(7).toDouble() + query.value(8).toDouble()));
		
		++iter;
		postup_form->inc_pos();
		
		dbfFile.AppendRecord();
	}
	delete d;
	delete postup_form;
	
	dbfFile.CloseDatabase();
	
	QMessageBox::information(0, "Експорт відомості нарахування по лічильниках", 
		"Завершено експорт відомості нарахування по лічильниках в dbf-файл за "+QString::number(month)+"."+QString::number(year)+".\n"+
		QString("Підсумкова інформація:\n")+
		"  кількість абонентів: "+QString::number(iter)+QString(";\n")+
		"  загальна опалювальна площа: "+QString::number(sSum, 'f', 5)+QString(";\n")+
		"  загальне фактичне використання: "+QString::number(kubSum, 'f', 5)+QString(";\n")+
		"  сума за ЦО: "+QString::number(opalSum, 'f', 2)+QString(";\n")+
		"  сума за ГВ: "+QString::number(gvSum, 'f', 2)+QString(";\n")+
		"  всього нарахування ЦО+ГВ: "+QString::number(opalSum+gvSum, 'f', 2)+"."
		);
}
//------------------------------------------------------------
UxBaseDbfInfo::UxBaseDbfInfo(const char* codecName)
{
	funcCodec = QTextCodec::codecForName(codecName);
}
//------------------------------------------------------------
char * UxBaseDbfInfo::strToxBaseChar(QString str)
{
	return funcCodec->fromUnicode(str).data();
}
//------------------------------------------------------------
char * UxBaseDbfInfo::strToxBaseDosChar(QString str)
{
	return funcCodec->fromUnicode(str.replace('і','i').replace('І','I').replace('ї','i').replace('Ї','I')).data();
}
//------------------------------------------------------------
