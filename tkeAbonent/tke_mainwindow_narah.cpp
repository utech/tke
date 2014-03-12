//------------------------------------------------------------
//
// Implamantation of tke_MainWindow class.
// Created by Roman Mulyk 3.10.2007
//
//------------------------------------------------------------

#include "tke_mainwindow.h"
#include "tke_dialog_print_kvytancii.h"
#include <USqlAccessible>

//------------------------------------------------------------
void tke_MainWindow::action_narah_activated()
{
	short month, year;
	tke_Dialog_print_kvytancii *dialog_narah = new tke_Dialog_print_kvytancii(this, "Нарахування за ЦО", currentSysDate.year(), currentSysDate.month(), true);
	if (dialog_narah->exec() == QDialog::Accepted){
		int budynokToNarah = dialog_narah->get_Bud_id();
		QString abonentsFilterString;
		if (budynokToNarah == 0)
			abonentsFilterString = "";
		else
			abonentsFilterString = " WHERE Budynok_ID="+QString::number(budynokToNarah);
		month = dialog_narah->get_Month();
		year = dialog_narah->get_Year();
		QDate narahDate(year, month, 1);
		delete dialog_narah;
		
		//Відкриття з'єднання для тестування нарахування
		QDir::setCurrent(QCoreApplication::applicationDirPath());
		QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
		bool needWriteTestInfo=settings->value("NarahOptions/SaveInformationOnNarahCO").toBool();
		//bool needWriteTestInfo=false;
		delete settings;
		
		QSqlQuery *testNarahQuery=NULL;
		if (needWriteTestInfo){
				QDir::setCurrent(QCoreApplication::applicationDirPath());
				QSqlDatabase testDb = QSqlDatabase::addDatabase("QSQLITE", "testNarahConnection" );
				testDb.setDatabaseName("tke.testDb");
				if (testDb.open()){
					needWriteTestInfo = true;
					testNarahQuery = new QSqlQuery(testDb);
					testNarahQuery->exec("DROP TABLE IF EXISTS narahCOTest");
					testNarahQuery->exec(" \
CREATE TABLE narahCOTest( \
	id INTEGER PRIMARY KEY AUTOINCREMENT, \
	Year INTEGER, \
	Month INTEGER, \
	Rahunok INTEGER, \
	Budynok_ID INTEGER, \
	PlosKvart REAL, \
	ZaivaPlosKvart REAL, \
	PilgaDlya INTEGER, \
	VidsotkyPilgy REAL, \
	PlosPilgy REAL, \
	PilgaDlya_2 INTEGER, \
	VidsotkyPilgy_2 REAL, \
	PlosPilgy_2 REAL, \
	Kimnaty INTEGER, \
	Taryf REAL, \
	Vh_saldo REAL, \
	Narah REAL, \
	Pilga_sum REAL, \
	Pilga_ID INTEGER, \
	Pilga_sum_2 REAL, \
	Pilga_ID_2 INTEGER, \
	KorecturaPloshchi REAL, \
	KorecturaSumy REAL, \
	needDateLimits INTEGER)");
				}
				else
					qDebug() << "Помилка. Не вдалося під'єднатися до бази даних tke.testDb";
		}
		
		//Провірка умов необхідних для нарахування
			//провірка діючого тарифу
		narah_temp_query->exec("SELECT count(*) FROM diuch_taryf \
									WHERE (year="+ QVariant(year).toString() +") \
									and (month="+ QVariant(month).toString() +")");
		narah_temp_query->seek(0);
		int rowCount = narah_temp_query->value(0).toInt();
		narah_temp_query->exec("SELECT count(*) FROM budynky");
		narah_temp_query->seek(0);
		if (narah_temp_query->value(0).toInt() > rowCount){
			QMessageBox::critical(this,
				"!!! Помилка нарахування !!!",
				"Програма не може виконати нарахування, \nоскільки не внесено діючі тарифи за "+
									QString::number(month)+"."+QString::number(year)+" у таблиці \"Діючий тариф\".\n" +
								"Порада - заповніть за даний місяць діючі тарифи в таблиці \"Діючий тариф\"",
				QMessageBox::Ok,
				QMessageBox::NoButton);
			return;
		}
		//Кінець провірки умов необхідних для нарахування
		narah_select_query->clear();
		narah_select_query->exec("SELECT count(*) FROM abonent"+abonentsFilterString);
		narah_select_query->next();
		UPostupForm *postup_form = new UPostupForm(0, narah_select_query->value(0).toInt() );
		postup_form->show();
		//Читання тарифів
		narah_temp_query->clear();
		narah_temp_query->exec("SELECT Budynok_ID, taryf FROM diuch_taryf \
									WHERE (year="+ QVariant(year).toString() +") \
									and (month="+ QVariant(month).toString() +")");
		taryf_ID.clear();
		taryf.clear();
		taryf_ID << 0;
		taryf << 0;
		while (narah_temp_query->next()){
			taryf_ID << narah_temp_query->value(0).toInt();
			taryf << narah_temp_query->value(1).toDouble();
		}
		//Читання пільг
		narah_temp_query->exec("SELECT id, Vidsotok, Ploshsca FROM pilgy");
		pilga_ID.clear();
		pilga_vids.clear();
		pilga_ploshcha.clear();
		pilga_ID << 0;
		pilga_vids << 0;
		pilga_ploshcha << 0;
		while (narah_temp_query->next()){
			pilga_ID << narah_temp_query->value(0).toInt();
			pilga_vids << narah_temp_query->value(1).toDouble();
			pilga_ploshcha << narah_temp_query->value(2).toDouble();
		}
		
		//Читання початкових і кінцевих днів послуг опалення і ГВ
		short pochDayCO, kincDayCO, pochDayGV, kincDayGV;
		narah_temp_query->exec("SELECT pochDayCO, kincDayCO, pochDayGV, KincDayGV FROM grafikPoslug \
								WHERE year="+QString::number(year)+" and month="+QString::number(month));
		if (narah_temp_query->seek(0)){
			pochDayCO = narah_temp_query->value(0).toInt();
			kincDayCO = narah_temp_query->value(1).toInt();
			pochDayGV = narah_temp_query->value(2).toInt();
			kincDayGV = narah_temp_query->value(3).toInt();
		}
		else{
			pochDayCO = 0;
			kincDayCO = 0;
			pochDayGV = 0;
			kincDayGV = 0;
		}
		
		narah_select_query->clear();
		narah_select_query->exec("SELECT Rahunok, Budynok_ID, Ploshcha, \
									Zaiva_ploshcha, Pilga_for, Pilga_ID, \
									kimn, Pilga_for_2, Pilga_ID_2, opal, \
									UsePilgyDateLimits, PochPilgaDate, KincPilgaDate, \
									PochPilga_2Date, KincPilga_2Date \
								FROM abonent "+abonentsFilterString);
		d.nDate = narahDate;
		while (narah_select_query->next()){
			int tVal;
			
			d.rah	= narah_select_query->value(0).toInt();
			d.bud 	= narah_select_query->value(1).toInt();
			d.s	 	= narah_select_query->value(2).toDouble();
			d.zs 	= narah_select_query->value(3).toDouble();
			d.pd 	= narah_select_query->value(4).toInt();
			tVal = pilga_ID.indexOf(narah_select_query->value(5).toInt());
			if (tVal<0)
				tVal = 0;
			d.p 	= pilga_vids[tVal];
			d.sp	= pilga_ploshcha[tVal];
			d.kimn	= narah_select_query->value(6).toInt();
			d.pd_2 	= narah_select_query->value(7).toInt();
			tVal = pilga_ID.indexOf(narah_select_query->value(8).toInt());
			if (tVal<0)
				tVal = 0;
			d.p_2 	= pilga_vids[tVal];
			d.sp_2	= pilga_ploshcha[tVal];
			if (taryf_ID.indexOf(d.bud) > 0)
				d.t		= taryf[taryf_ID.indexOf(d.bud)];
			else
				d.t		= 0;
			
			d.needDateLimits = narah_select_query->value(10).toBool();
			d.pilgaBeginDate = narah_select_query->value(11).toDate();
			d.pilgaEndDate = narah_select_query->value(12).toDate();
			d.pilgaBeginDate_2 = narah_select_query->value(13).toDate();
			d.pilgaEndDate_2 = narah_select_query->value(14).toDate();
			d.begPilgDayCO = pochDayCO;
			d.endPilgDayCO = kincDayCO;
			d.begPilgDayCO_2 = pochDayCO;
			d.endPilgDayCO_2 = kincDayCO;
			d.pochDayPoslugCO = pochDayCO;
			d.kincDayPoslugCO = kincDayCO;

			
			double narah, pilga_narah, pilga_narah_2, pilgPlos, pilgPlos_2;
			QDate 	pochPilgaDate = narah_select_query->value(11).toDate(),
					kincPilgaDate = narah_select_query->value(12).toDate(),
					pochPilga_2Date = narah_select_query->value(13).toDate(),
					kincPilga_2Date = narah_select_query->value(14).toDate();
			if (narah_select_query->value(9).toBool()){
				narah = f_narah(&d);
				if ( (d.p!=0) && (d.pd>0) && ((!d.needDateLimits) || (d.needDateLimits && inMDateInterval(narahDate, pochPilgaDate, kincPilgaDate)))){
					pilga_narah = f_pilga(&d);
					pilgPlos = d.pilgPlos;
					if ( (d.p_2!=0) && (d.pd_2>0) && ( (!d.needDateLimits) || (d.needDateLimits && inMDateInterval(narahDate, pochPilga_2Date, kincPilga_2Date)) )){
						pilga_narah_2 = f_pilga_2(&d);
						pilgPlos_2 = d.pilgPlos;
					}
					else{
						pilga_narah_2 = 0;
						pilgPlos_2 = 0;
					}
				}
				else{
					pilga_narah = 0;
					pilga_narah_2 = 0;
					pilgPlos = 0;
					pilgPlos_2 = 0;
				}
			}
			else{
				narah = 0;
				pilga_narah = 0;
				pilga_narah_2 = 0;
				pilgPlos = 0;
				pilgPlos_2 = 0;
			}
			
			//обрахунок коректури по площі (проводиться повторне нарахування з площею з коректури)
			narah_temp_query->exec("SELECT ploshcha, vidsotok FROM korektura \
									WHERE (year="+ QVariant(year).toString() +") \
									and (month="+ QVariant(month).toString() +") \
									and (id="+ narah_select_query->value(0).toString() +")");
			double korektura_ploshchi;
			double narah_korektura, pilga_narah_korektura, pilga_narah_2_korektura;
			if (narah_temp_query->next()){
				d.ks = narah_temp_query->value(0).toDouble();
				d.kp = narah_temp_query->value(1).toInt();
				//d.s  = d.ks * d.kp / 100;
				if (narah_select_query->value(9).toBool()){
					narah_korektura = f_narah_korektura_ploshchi(&d);
					if ( (d.p!=0) && ( (!d.needDateLimits) || (d.needDateLimits && inMDateInterval(narahDate, pochPilgaDate, kincPilgaDate)) ) ){
						pilga_narah_korektura = f_pilga_korektura_ploshchi(&d);
						pilgPlos -= d.pilgPlos;
						if ((d.p_2!=0)  && ( (!d.needDateLimits) || (d.needDateLimits && inMDateInterval(narahDate, pochPilga_2Date, kincPilga_2Date)) )){
							pilga_narah_2_korektura = f_pilga_2_korektura_ploshchi(&d);
							pilgPlos_2 -= d.pilgPlos;
						}
						else
							pilga_narah_2_korektura = 0;
					}
					else{
						pilga_narah_korektura = 0;
						pilga_narah_2_korektura = 0;
					}
				}
				else{
					narah_korektura = 0;
					pilga_narah_korektura = 0;
					pilga_narah_2_korektura = 0;
				}
			}
			else{
				narah_korektura = 0;
				pilga_narah_korektura = 0;
				pilga_narah_2_korektura = 0;
			}
			
			//читання коректури по сумі
			narah_temp_query->exec("SELECT suma, pryznach FROM korektura_s \
									WHERE (year="+ QVariant(year).toString() +") \
									and (month="+ QVariant(month).toString() +") \
									and (id="+ narah_select_query->value(0).toString() +")");
			narah_temp_query->seek(-1);
			double korektura_sumy=0;
			short pryznachennya_korektury_sumy=0;
			while (narah_temp_query->next()){
				korektura_sumy += narah_temp_query->value(0).toDouble();
				pryznachennya_korektury_sumy = narah_temp_query->value(0).toInt();
			}
			//Нарахування не може бути меншим за нуль
			narah -= narah_korektura;
			pilga_narah -= pilga_narah_korektura;
			pilga_narah_2 -= pilga_narah_2_korektura;
			korektura_ploshchi = narah_korektura - pilga_narah_korektura - pilga_narah_2_korektura;
			narah = narah - pilga_narah - pilga_narah_2; //Вже врахована коректура по прлощі
			if (narah < 0)
				narah = 0;
			narah = narah + korektura_sumy;
			//Врахування коректури по сумі в пільзі, якщо потрібно
			if (pryznachennya_korektury_sumy==1)
				pilga_narah += korektura_sumy;
			else if (pryznachennya_korektury_sumy==2)
				pilga_narah_2 += korektura_sumy;
			
			double vh_saldo = vhSaldoInMonth(d.rah, year, month);
			
			//Запис сальдо і нарахування до бази даних
			narah_temp_query->exec("SELECT id FROM narahuvannya \
									WHERE (Rahunok=" + 
										QVariant(d.rah).toString() + ") and (year=" + QVariant(year).toString() + 
										") and (month=" + QVariant(month).toString() + ") and (Status=0)");
			if (narah_temp_query->seek(0)){
				narah_temp_query->exec("UPDATE narahuvannya SET \
											Narah="+sqlStr(narah)+" ,\
											Vh_saldo="+sqlStr(vh_saldo)+" ,\
											Pilga_sum="+sqlStr(pilga_narah)+" ,\
											Pilga_ID="+narah_select_query->value(5).toString()+" ,\
											Pilga_for="+sqlStr(d.pd)+" ,\
											Pilga_plos="+sqlStr(pilgPlos)+" ,\
											PochDayCO="+sqlStr(d.begPilgDayCO)+" ,\
											KincDayCO="+sqlStr(d.endPilgDayCO)+" \
										WHERE id="+narah_temp_query->value(0).toString());
			}
			else{
				narah_temp_query->exec("INSERT INTO narahuvannya (id, Rahunok, Year, Month, Narah, Narah_GV, \
													Vh_saldo, Pilga_sum, Pilga_ID, Pilga_for, Pilga_plos, \
													Status, PochDayCO, KincDayCO) \
										VALUES \
												("+sqlStr(nextLichylnyk("narahuvannya"))+", \
												"+sqlStr(d.rah)+", "+sqlStr(year)+", "+sqlStr(month)+", "+sqlStr(narah)+", \
												0, "+sqlStr(vh_saldo)+", "+sqlStr(pilga_narah)+", \
												"+narah_select_query->value(5).toString()+", "+sqlStr(d.pd)+", "+sqlStr(pilgPlos,false)+", \
												0, "+sqlStr(d.begPilgDayCO)+", "+sqlStr(d.endPilgDayCO)+")");
			}
			//Запис другої пільги
			if (narah_select_query->value(8).toInt() != 0){
				narah_temp_query->exec("SELECT id FROM narahuvannya \
										WHERE (Rahunok=" + 
											QVariant(d.rah).toString() + ") and (year=" + QVariant(year).toString() + 
											") and (month=" + QVariant(month).toString() + ") and (Status=1)");
				if (narah_temp_query->seek(0)){
					narah_temp_query->exec("UPDATE narahuvannya SET \
												Pilga_sum="+sqlStr(pilga_narah_2)+" ,\
												Pilga_ID="+narah_select_query->value(8).toString()+" ,\
												Pilga_for="+sqlStr(d.pd_2)+" ,\
												Pilga_plos="+sqlStr(pilgPlos_2,false)+" ,\
												PochDayCO="+sqlStr(d.begPilgDayCO_2)+" ,\
												KincDayCO="+sqlStr(d.endPilgDayCO_2)+" \
											WHERE id="+narah_temp_query->value(0).toString());
				}
				else{
				narah_temp_query->exec("INSERT INTO narahuvannya (id, Rahunok, Year, Month, Narah, Narah_GV, \
													Vh_saldo, Pilga_sum, Pilga_ID, Pilga_for, Pilga_plos, \
													Status, PochDayCO, KincDayCO) \
										VALUES \
												("+sqlStr(nextLichylnyk("narahuvannya"))+", \
												"+sqlStr(d.rah)+", "+sqlStr(year)+", "+sqlStr(month)+", 0, \
												0, 0, "+sqlStr(pilga_narah_2)+", \
												"+narah_select_query->value(8).toString()+", "+sqlStr(d.pd_2)+", "+sqlStr(pilgPlos_2,false)+", \
												1, "+sqlStr(d.begPilgDayCO_2)+", "+sqlStr(d.endPilgDayCO_2)+")");
				}
			}
			//Запис суми коректури по площі
			if (korektura_ploshchi != 0){
				narah_temp_query->exec("UPDATE korektura SET suma="+sqlStr(korektura_ploshchi)+" \
										WHERE (id="+sqlStr(d.rah)+") \
											and (year="+sqlStr(year)+") \
											and (month="+sqlStr(month)+")");
			}
			postup_form->inc_pos();
			
			postup_form->update();
			postup_form->repaint();
			
			//Запис при необхідності інформації про нарахування в тестову базу даних tke.testDb
			if (needWriteTestInfo){
				testNarahQuery->exec("INSERT INTO narahCOTest (Year, Month, Rahunok, Budynok_ID, PlosKvart, \
													ZaivaPlosKvart, PilgaDlya, VidsotkyPilgy, PlosPilgy, PilgaDlya_2, \
													VidsotkyPilgy_2, PlosPilgy_2, Kimnaty, Taryf, Vh_saldo, \
													Narah, Pilga_sum, Pilga_ID, Pilga_sum_2, Pilga_ID_2, \
													KorecturaPloshchi, KorecturaSumy, needDateLimits) \
										VALUES ("+sqlStr(year)+","+sqlStr(month)+","+sqlStr(d.rah)+","+sqlStr(d.bud)+","+sqlStr(d.s)+", \
												"+sqlStr(d.zs)+","+sqlStr(d.pd)+","+sqlStr(d.p)+","+sqlStr(d.sp)+","+sqlStr(d.pd_2)+", \
												"+sqlStr(d.p_2)+","+sqlStr(d.sp_2)+","+sqlStr(d.kimn)+","+sqlStr(d.t)+","+sqlStr(vh_saldo)+",\
												"+sqlStr(narah)+","+sqlStr(pilga_narah)+","+sqlStr(narah_select_query->value(5).toInt())+","+sqlStr(pilga_narah_2)+","+sqlStr(narah_select_query->value(8).toInt())+",\
												"+sqlStr(korektura_ploshchi)+","+sqlStr(korektura_sumy)+","+sqlStr(QVariant(d.needDateLimits).toString())+")");
			}
		}
		if (needWriteTestInfo)
			delete testNarahQuery;
		postup_form->close();
		delete postup_form;
		populate_operacii_table();
	}
}
//------------------------------------------------------------
void tke_MainWindow::action_narah_GV_activated()
{
	short month, year;
	tke_Dialog_print_kvytancii *dialog_narah = new tke_Dialog_print_kvytancii(this, "Нарахування за ГВ", currentSysDate.year(), currentSysDate.month(), true);
	if (dialog_narah->exec() == QDialog::Accepted){
		qDebug() << " -->> Нарахування за ГВ --- ";
		int budynokToNarah = dialog_narah->get_Bud_id();
		QString abonentsFilterString;
		if (budynokToNarah == 0)
			abonentsFilterString = "";
		else
			abonentsFilterString = " WHERE Budynok_ID="+QString::number(budynokToNarah);
		month = dialog_narah->get_Month();
		year = dialog_narah->get_Year();
		QDate narahDate(year, month, 11);
		delete dialog_narah;
				narah_select_query->clear();
		narah_select_query->exec("SELECT count(*) FROM abonent"+abonentsFilterString);
		narah_select_query->next();
		UPostupForm *postup_form = new UPostupForm(0, narah_select_query->value(0).toInt() );
		postup_form->show();
		//Читання тарифів
		UNasTaryfInfo taryfInfo = nasTaryf(year, month);
		d.tVGV = taryfInfo.vart_kuba_GV;
		d.tOsGV = taryfInfo.vart_na_lud_in_month_GV;
		
		//Читання пільг
		narah_temp_query->exec("SELECT id, Vidsotok, Ploshsca FROM pilgy");
		pilga_ID.clear();
		pilga_vids.clear();
		pilga_ploshcha.clear();
		narah_temp_query->seek(-1);
		pilga_ID << 0;
		pilga_vids << 0;
		pilga_ploshcha << 0;
		while (narah_temp_query->next()){
			pilga_ID << narah_temp_query->value(0).toInt();
			pilga_vids << narah_temp_query->value(1).toDouble();
			pilga_ploshcha << narah_temp_query->value(2).toDouble();
		}
		//Читання норми використання води на особу
		d.normaGV = normaSpojyvGVNaOsobu(year,month);
		
		//Читання початкових і кінцевих днів послуг опалення і ГВ
		short pochDayCO, kincDayCO, pochDayGV, kincDayGV;
		narah_temp_query->exec("SELECT pochDayCO, kincDayCO, pochDayGV, KincDayGV FROM grafikPoslug \
								WHERE year="+QString::number(year)+" and month="+QString::number(month));
		if (narah_temp_query->seek(0)){
			pochDayCO = narah_temp_query->value(0).toInt();
			kincDayCO = narah_temp_query->value(1).toInt();
			pochDayGV = narah_temp_query->value(2).toInt();
			kincDayGV = narah_temp_query->value(3).toInt();
		}
		else{
			pochDayCO = 0;
			kincDayCO = 0;
			pochDayGV = 0;
			kincDayGV = 0;
		}
		
		narah_select_query->clear();
		narah_select_query->exec("SELECT Rahunok, Budynok_ID, Ploshcha, \
										Zaiva_ploshcha, Pilga_for, Pilga_ID, \
										kimn, Pilga_for_2, Pilga_ID_2, opal, \
										UsePilgyDateLimits, PochPilgaDate, KincPilgaDate, \
										PochPilga_2Date, KincPilga_2Date, ludy, GarVoda \
									FROM abonent "+abonentsFilterString+" \
									ORDER BY Rahunok");
		d.nDate = narahDate;
		while (narah_select_query->next()){
			int tVal;
			d.rah	= narah_select_query->value(0).toInt();
			d.bud 	= narah_select_query->value(1).toInt();
			d.s	 	= narah_select_query->value(2).toDouble();
			d.zs 	= narah_select_query->value(3).toDouble();
			d.pd 	= narah_select_query->value(4).toInt();
			tVal = pilga_ID.indexOf(narah_select_query->value(5).toInt());
			if (tVal<0)
				tVal = 0;
			d.p 	= pilga_vids[tVal];
			d.sp	= pilga_ploshcha[tVal];
			d.kimn	= narah_select_query->value(6).toInt();
			d.pd_2 	= narah_select_query->value(7).toInt();
			tVal = pilga_ID.indexOf(narah_select_query->value(8).toInt());
			if (tVal<0)
				tVal = 0;
			d.p_2 	= pilga_vids[tVal];
			d.sp_2	= pilga_ploshcha[tVal];
			d.needDateLimits = narah_select_query->value(10).toBool();
			d.pilgaBeginDate = narah_select_query->value(11).toDate();
			d.pilgaEndDate = narah_select_query->value(12).toDate();
			d.pilgaBeginDate_2 = narah_select_query->value(13).toDate();
			d.pilgaEndDate_2 = narah_select_query->value(14).toDate();
			d.begPilgDayGV = pochDayGV;
			d.endPilgDayGV = kincDayGV;
			d.begPilgDayGV_2 = pochDayGV;
			d.endPilgDayGV_2 = kincDayGV;
			d.pochDayPoslugGV = pochDayGV;
			d.kincDayPoslugGV = kincDayGV;
			//d.t		= taryf;
			
			d.ludy	= narah_select_query->value(15).toInt();
			d.vykorGV= 0;
			
			double narah_GV=0, pilga_narah=0, pilga_narah_2=0, pilgObem=0, pilgObem_2=0;
			QDate 	pochPilgaDate = narah_select_query->value(11).toDate(),
					kincPilgaDate = narah_select_query->value(12).toDate(),
					pochPilga_2Date = narah_select_query->value(13).toDate(),
					kincPilga_2Date = narah_select_query->value(14).toDate();
			//Нарахування за ГВ і першу і другу пільгу
			if (narah_select_query->value(16).toBool()){
				narah_GV = f_narah_GV(&d);
				//qDebug() << "rah: "<< d.rah << ", pilgaSum:" << narah_GV;
				if ( (d.p!=0) && (d.pd>0) && ( (!d.needDateLimits) || (d.needDateLimits && inMDateInterval(narahDate, pochPilgaDate, kincPilgaDate)) ) && (narah_GV>0.009)){
					pilga_narah = f_pilga_GV(&d);
					pilgObem = d.pilgObem;
					//qDebug() << "rah: " << d.rah << ", pilgObem" << pilgObem;
					//qDebug() << "rah: " << d.rah << ", pilga_narah" << pilga_narah;
					if ( (d.p_2!=0) && (d.pd_2>0) && ( (!d.needDateLimits) || (d.needDateLimits && inMDateInterval(narahDate, pochPilga_2Date, kincPilga_2Date)) )){
						pilga_narah_2 = f_pilga_GV_2(&d);
						pilgObem_2 = d.pilgObem;
						//qDebug() << "rah: " << d.rah << ", pilgObem_2" << pilgObem_2;
					}
					else{
						pilga_narah_2 = 0;
						pilgObem_2 = 0;
					}
				}
				else{
					pilga_narah = 0;
					pilga_narah_2 = 0;
					pilgObem = 0;
					pilgObem_2 = 0;
					//d.isLichGV = false;
				}
			}
			else{
				narah_GV = 0;
				pilga_narah = 0;
				pilga_narah_2 = 0;
				pilgObem = 0;
				pilgObem_2 = 0;
				d.isLichGV = false;
			}
			//Читання коректури по сумі
			narah_temp_query->exec("SELECT suma_GV, pryznach FROM korektura_s \
									WHERE (year="+ QVariant(year).toString() +") \
										and (month="+ QVariant(month).toString() +") \
										and (id="+ narah_select_query->value(0).toString() +")");
			double korektura_sumy=0;
			short pryznachennya_korektury_sumy=0;
			while (narah_temp_query->next()){
				korektura_sumy += narah_temp_query->value(0).toDouble();
				pryznachennya_korektury_sumy = narah_temp_query->value(0).toInt();
			}
			
			//Нарахування не може бути меншим за нуль
			narah_GV = narah_GV - pilga_narah - pilga_narah_2;
			if (narah_GV < 0)
				narah_GV = 0;
			narah_GV = narah_GV + korektura_sumy;
			
			double vh_saldo = vhSaldoInMonth(d.rah, year, month);
			//Запис сальдо і нарахування до бази даних
			//qDebug()	<< "рах: " << d.rah << ", нарГВ: " << narah_GV << "пільгаГВ1: " << pilga_narah << "пільгаГВ2: " << pilga_narah_2 
			//			<< ", викорГВ: " << d.vykorGV << ", тариф: " << d.t << ", проживають: " << d.ludy << ", відсПільги: " << d.p << ";";
			narah_temp_query->exec("SELECT id FROM narahuvannya \
									WHERE (Rahunok=" + 
										QVariant(d.rah).toString() + ") and (year=" + QVariant(year).toString() + 
										") and (month=" + QVariant(month).toString() + ") and (Status=0)");
			if (narah_temp_query->seek(0)){
				narah_temp_query->exec("UPDATE narahuvannya SET \
											Narah_GV="+sqlStr(narah_GV)+" ,\
											Vh_saldo="+sqlStr(vh_saldo)+" ,\
											Pilga_sum_GV="+sqlStr(pilga_narah)+" ,\
											Pilga_ID_GV="+narah_select_query->value(5).toString()+" ,\
											Pilga_for_GV="+sqlStr(d.pd)+" ,\
											Obsyag_GV="+sqlStr(pilgObem)+" ,\
											Is_Lich_GV="+sqlStr(d.isLichGV)+" ,\
											PochDayGV="+sqlStr(d.begPilgDayGV)+" ,\
											KincDayGV="+sqlStr(d.endPilgDayGV)+" \
										WHERE id="+narah_temp_query->value(0).toString());
			}
			else{
				narah_temp_query->exec("INSERT INTO narahuvannya (id, Rahunok, Year, Month, Narah, Narah_GV, \
													Vh_saldo, Pilga_sum_GV, Pilga_ID_GV, Pilga_for_GV, Obsyag_GV, \
													Status, PochDayGV, KincDayGV, Is_Lich_GV) \
										VALUES \
												("+sqlStr(nextLichylnyk("narahuvannya"))+", \
												"+sqlStr(d.rah)+", "+sqlStr(year)+", "+sqlStr(month)+", 0, \
												"+sqlStr(narah_GV)+", "+sqlStr(vh_saldo)+", "+sqlStr(pilga_narah)+", \
												"+narah_select_query->value(5).toString()+", "+sqlStr(d.pd)+", "+sqlStr(pilgObem,false)+", \
												0, "+sqlStr(d.begPilgDayGV)+", "+sqlStr(d.endPilgDayGV)+", "+sqlStr(d.isLichGV)+")");
			}
			//Запис другої пільги
			if (narah_select_query->value(8).toInt() != 0){
				narah_temp_query->exec("SELECT id FROM narahuvannya \
										WHERE (Rahunok=" + 
											QVariant(d.rah).toString() + ") and (year=" + QVariant(year).toString() + 
											") and (month=" + QVariant(month).toString() + ") and (Status=1)");
				if (narah_temp_query->seek(0)){
					narah_temp_query->exec("UPDATE narahuvannya SET \
												Pilga_sum_GV="+sqlStr(pilga_narah_2)+" ,\
												Pilga_ID_GV="+narah_select_query->value(8).toString()+" ,\
												Pilga_for_GV="+sqlStr(d.pd_2)+" ,\
												Obsyag_GV="+sqlStr(pilgObem_2,false)+" ,\
												PochDayGV="+sqlStr(d.begPilgDayGV_2)+" ,\
												KincDayGV="+sqlStr(d.endPilgDayGV_2)+" \
											WHERE id="+narah_temp_query->value(0).toString());
				}
				else{
				narah_temp_query->exec("INSERT INTO narahuvannya (id, Rahunok, Year, Month, Narah, Narah_GV, \
													Vh_saldo, Pilga_sum_GV, Pilga_ID_GV, Pilga_for_GV, Obsyag_GV, \
													Status, PochDayGV, KincDayGV) \
										VALUES \
												("+sqlStr(nextLichylnyk("narahuvannya"))+", \
												"+sqlStr(d.rah)+", "+sqlStr(year)+", "+sqlStr(month)+", 0, \
												0, 0, "+sqlStr(pilga_narah_2)+", \
												"+narah_select_query->value(8).toString()+", "+sqlStr(d.pd_2)+", "+sqlStr(pilgObem_2,false)+", \
												1, "+sqlStr(d.begPilgDayGV_2)+", "+sqlStr(d.endPilgDayGV_2)+")");
				}
			}
			
			//Запис використання ГВ
			narah_temp_query->exec("SELECT id FROM spojyvannyaGV WHERE Rahunok_ID="+sqlStr(d.rah)+" \
										and year="+sqlStr(year)+" \
										and month="+sqlStr(month));
			if (narah_temp_query->seek(0)){
				narah_temp_query->exec("UPDATE spojyvannyaGV SET \
											IsGarVoda="+sqlStr(narah_select_query->value(16).toBool())+", \
											IsLichGV="+sqlStr(d.isLichGV)+", \
											VykorVody="+sqlStr(d.vykorGV,false)+", \
											Ludy="+sqlStr(d.ludy)+" \
										WHERE id="+sqlStr(narah_temp_query->value(0).toInt()));
			}
			else{
				narah_temp_query->exec("INSERT INTO spojyvannyaGV (id, Rahunok_ID, Year, Month, IsGarVoda, IsLichGV, VykorVody, Ludy) \
										VALUES ("+sqlStr(nextLichylnyk("spojyvannyaGV"))+", \
										"+sqlStr(d.rah)+", \
										"+sqlStr(year)+", \
										"+sqlStr(month)+", \
										"+sqlStr(narah_select_query->value(16).toBool())+", \
										"+sqlStr(d.isLichGV)+", \
										"+sqlStr(d.vykorGV,false)+", \
										"+sqlStr(d.ludy)+")");
			}
			postup_form->inc_pos();
			
			postup_form->update();
			postup_form->repaint();
		}
		postup_form->close();
		delete postup_form;
		qDebug() << " -->> Закінчено нарахування за ГВ --- ";
		populate_operacii_table();
	}
}
//------------------------------------------------------------
void tke_MainWindow::action_narah_ST_activated()
{
	short month, year;
	tke_Dialog_print_kvytancii *dialog_narah = new tke_Dialog_print_kvytancii(this, "Нарахування по СТ", currentSysDate.year(), currentSysDate.month(), true);
	if (dialog_narah->exec() == QDialog::Accepted){
		qDebug() << " -->> Нарахування за СТ --- ";
		int budynokToNarah = dialog_narah->get_Bud_id();
		QString abonentsFilterString;
		if (budynokToNarah == 0)
			abonentsFilterString = "";
		else
			abonentsFilterString = " WHERE Budynok_ID="+QString::number(budynokToNarah);
		month = dialog_narah->get_Month();
		year = dialog_narah->get_Year();
		QDate narahDate(year, month, 1);
		delete dialog_narah;
		
		//Провірка умов необхідних для нарахування
			//провірка діючого тарифу
		narah_temp_query->exec("SELECT count(*) FROM diuch_taryf \
									WHERE (year="+ QVariant(year).toString() +") \
									and (month="+ QVariant(month).toString() +")");
		narah_temp_query->seek(0);
		int rowCount = narah_temp_query->value(0).toInt();
		narah_temp_query->exec("SELECT count(*) FROM budynky");
		narah_temp_query->seek(0);
		if (narah_temp_query->value(0).toInt() > rowCount){
			QMessageBox::critical(this,
				"!!! Помилка нарахування !!!",
				"Програма не може виконати нарахування, \nоскільки не внесено діючі тарифи за "+
									QString::number(month)+"."+QString::number(year)+" у таблиці \"Діючий тариф\".\n" +
								"Порада - заповніть за даний місяць діючі тарифи в таблиці \"Діючий тариф\"",
				QMessageBox::Ok,
				QMessageBox::NoButton);
			return;
		}
		//Кінець провірки умов необхідних для нарахування
		
		narah_select_query->exec("SELECT count(*) FROM abonent"+abonentsFilterString);
		narah_select_query->next();
		UPostupForm *postup_form = new UPostupForm(0, narah_select_query->value(0).toInt() );
		postup_form->show();
		//Читання тарифів
		narah_temp_query->clear();
		narah_temp_query->exec("SELECT Budynok_ID, taryf FROM diuch_taryf \
									WHERE (year="+ QVariant(year).toString() +") \
									and (month="+ QVariant(month).toString() +")");
		taryf_ID.clear();
		taryf.clear();
		narah_temp_query->seek(-1);
		taryf_ID << 0;
		taryf << 0;
		while (narah_temp_query->next()){
			taryf_ID << narah_temp_query->value(0).toInt();
			taryf << narah_temp_query->value(1).toDouble();
		}
		
		narah_temp_query->exec("SELECT vart_g_kal, normat_taryf_kvadr FROM normat_taryf ORDER BY year DESC, month DESC");
		narah_temp_query->seek(0);
		d.vartGkalSt=narah_temp_query->value(0).toDouble();
		//d.vartKvadrSt=narah_temp_query->value(1).toDouble();
		
		narah_select_query->clear();
		narah_select_query->exec("SELECT Rahunok, Budynok_ID, Ploshcha, \
									kimn, opal, GarVoda, NarahStoyak, ludy \
									FROM abonent "+abonentsFilterString+" \
									ORDER BY Rahunok");
		while (narah_select_query->next()){
			d.rah	= narah_select_query->value(0).toInt();
			d.bud 	= narah_select_query->value(1).toInt();
			d.s	 	= narah_select_query->value(2).toDouble();
			d.kimn	= narah_select_query->value(3).toInt();
			d.ludy	= narah_select_query->value(7).toInt();
			d.nDate.setDate(year, month, 1);
			
			if (taryf_ID.indexOf(d.bud) > 0)
				d.vartKvadrSt	= taryf[taryf_ID.indexOf(d.bud)];
			else
				d.vartKvadrSt	= 0;
			narah_temp_query->exec("SELECT sum(Plos), sum(GKal) \
									FROM pokaznOpalStoyak \
									WHERE (Rahunok_ID="+QString::number(d.rah)+") \
										AND (Year="+QString::number(year)+") \
										AND (Month="+QString::number(month)+")");
			if (narah_temp_query->seek(0)){
				d.gkalSt = narah_temp_query->value(1).toDouble();
				d.kvadrSt = narah_temp_query->value(0).toDouble();
			}
			else{
				d.gkalSt = 0;
				d.kvadrSt = 0;
			}
			
			double narah_St;
			//Нарахування по стояках
			if (narah_select_query->value(6).toBool()){
				narah_St = f_narah_St(&d);
				qDebug() << "d.gkalSt: " << d.gkalSt << ", d.kvadrSt: " << d.kvadrSt << ", d.vartGkalSt: " << d.vartGkalSt 
					<< ", d.vartKvadrSt: " << d.vartKvadrSt << ", narah_St: " << narah_St;
			}
			else
				narah_St = 0;
			//Читання коректури по сумі
			double korektura_sumy=0;
			narah_temp_query->exec("SELECT sum(suma_ST) FROM korektura_s \
									WHERE (year="+ QVariant(year).toString() +") \
										and (month="+ QVariant(month).toString() +") \
										and (id="+ narah_select_query->value(0).toString() +")");
			if (narah_temp_query->seek(0))
				korektura_sumy=narah_temp_query->value(0).toDouble();
			
			//Нарахування не може бути меншим за нуль
			if (narah_St < 0)
				narah_St = 0;
			narah_St = narah_St + korektura_sumy;
			
			double vh_saldo = vhSaldoInMonth(d.rah, year, month);
			
			//Запис сальдо і нарахування до бази даних
			narah_temp_query->exec("SELECT id FROM narahuvannya \
									WHERE (Rahunok=" + 
										QVariant(d.rah).toString() + ") and (year=" + QVariant(year).toString() + 
										") and (month=" + QVariant(month).toString() + ") and (Status=0)");
			if (narah_temp_query->seek(0)){
				narah_temp_query->exec("UPDATE narahuvannya SET \
											Narah_St="+sqlStr(narah_St)+" ,\
											Vh_saldo="+sqlStr(vh_saldo)+" \
										WHERE id="+narah_temp_query->value(0).toString());
			}
			else{
				narah_temp_query->exec("INSERT INTO narahuvannya (id, Rahunok, Year, Month, Narah, Narah_GV, Narah_St \
													Vh_saldo, Status) \
										VALUES \
												("+sqlStr(nextLichylnyk("narahuvannya"))+", \
												"+sqlStr(d.rah)+", "+sqlStr(year)+", "+sqlStr(month)+", 0, 0, \
												"+sqlStr(narah_St)+", "+sqlStr(vh_saldo)+", 0)");
			}
			postup_form->inc_pos();
			
			postup_form->update();
			postup_form->repaint();
		}
		postup_form->close();
		delete postup_form;
		qDebug() << " -->> Закінчено нарахування за СТ --- ";
		populate_operacii_table();
	}
}
//------------------------------------------------------------
