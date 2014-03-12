/****************************************************************************
**
** Created: Roman Mulyk 24.09.2006
**
****************************************************************************/

#include <QtDebug>
#include "tke_math.h"
#include <USqlAccessible>

//------------------------------------------------------------
double f_narah(UNarah_data *d)
{
	double s;
	s = d->t*d->s;
	s = d_to_m(s); //Не змінювати. Заокруглює до копійок
	return s;
}
//------------------------------------------------------------
double	s_pilga(const UNarah_data *d)
{
	double s_pilgy;
	if (d->sp == 0)//Обрахунок площі з санітарних норм
		s_pilgy = d->pd * d->pd1 + d->pds;
	else //Вибір сталої площі на конкретну категорію пільги
		s_pilgy = d->sp * d->pd;
	if (s_pilgy > d->s)
		s_pilgy = d->s;
	return s_pilgy;
}
//------------------------------------------------------------
double	f_pilga(UNarah_data *d)
{
	double s;
	d->pilgPlos = s_pilga(d);
	s = d->t * d->p * (d->pilgPlos)/100.0;
		//Врахування випадку нарахування за неповний місяць
	if (d->needDateLimits){
		int begDay, endDay;
		if ((d->pilgaBeginDate.year() == d->nDate.year()) && (d->pilgaBeginDate.month() == d->nDate.month()))
			begDay = d->pilgaBeginDate.day();
		else
			begDay = 1;
		if ((d->pilgaEndDate.year() == d->nDate.year()) && (d->pilgaEndDate.month() == d->nDate.month()))
			endDay = d->pilgaEndDate.day();
		else
			endDay = d->nDate.daysInMonth();
		
		//s = s * (endDay - begDay + 1) / d->nDate.daysInMonth(); //розраховується весь місяць
		if (begDay < d->pochDayPoslugCO)
			begDay = d->pochDayPoslugCO;
		if (endDay > d->kincDayPoslugCO)
			endDay = d->kincDayPoslugCO;
		s = s * (endDay - begDay + 1) / (d->kincDayPoslugCO - d->pochDayPoslugCO + 1);
		
		d->begPilgDayCO = begDay;
		d->endPilgDayCO = endDay;
	}
	s = d_to_m(s); //Не змінювати. Заокруглює до копійок
	return s;
}
//------------------------------------------------------------
double	f_pilga_2(UNarah_data *d)
{
	double s,s_pilgy, s_pilgy_1;
	
	//Обрахунок площі пільги в залежності від кількості кімнат для другої пільги
	s_pilgy = d->pd_2 * d->pd1;
	if (s_pilgy > d->s)
		s_pilgy = d->s;
	//Врахування категорії пільги, коли площа пільги є ненульовою наприклад для вчителів
	if (d->sp_2 != 0)
		s_pilgy = d->sp_2 * d->pd_2;
	//Врахування того, що площа на другу пільгу не може бути більшою ніж (загальна площа - площа першої пільги)
	s_pilgy_1 = s_pilga(d);
	if (d->s - s_pilgy_1 < s_pilgy)
		s_pilgy = d->s - s_pilgy_1;
	d->pilgPlos = s_pilgy;
	s = d->t * d->p_2 * (s_pilgy)/100.0;
		//Врахування випадку нарахування за неповний місяць
	if (d->needDateLimits){
		int begDay, endDay;
		if ((d->pilgaBeginDate_2.year() == d->nDate.year()) && (d->pilgaBeginDate_2.month() == d->nDate.month()))
			begDay = d->pilgaBeginDate_2.day();
		else
			begDay = 1;
		if ((d->pilgaEndDate_2.year() == d->nDate.year()) && (d->pilgaEndDate_2.month() == d->nDate.month()))
			endDay = d->pilgaEndDate_2.day();
		else
			endDay = d->nDate.daysInMonth();
			
		//s = s * (endDay - begDay + 1) / d->nDate.daysInMonth();
		if (begDay < d->pochDayPoslugCO)
			begDay = d->pochDayPoslugCO;
		if (endDay > d->kincDayPoslugCO)
			endDay = d->kincDayPoslugCO;
		s = s * (endDay - begDay + 1) / (d->kincDayPoslugCO - d->pochDayPoslugCO + 1);
		
		d->begPilgDayCO_2 = begDay;
		d->endPilgDayCO_2 = endDay;
	}
	s = d_to_m(s); //Не змінювати. Заокруглює до копійок
	return s;
}
//------------------------------------------------------------
double f_narah_korektura_ploshchi(UNarah_data *d)
{
	double s;
	s = d->t * d->ks * d->kp/100.0;
	s = d_to_m(s); //Не змінювати. Заокруглює до копійок
	return s;
}
//------------------------------------------------------------
double	s_pilga_korektura_ploshchi(const UNarah_data *d)
{
	double s_pilgy;
	if (d->sp == 0)//Обрахунок площі з санітарних норм
		s_pilgy = d->pd * d->pd1 + d->pds;
	else //Вибір сталої площі на конкретну категорію пільги
		s_pilgy = d->sp;
	if (s_pilgy > d->ks)
		s_pilgy = d->ks;
	return s_pilgy;
}
//------------------------------------------------------------
double	f_pilga_korektura_ploshchi(UNarah_data *d)
{
	double s;
	d->pilgPlos = s_pilga_korektura_ploshchi(d) * d->kp / 100.0;
	s = d->t * d->p * d->pilgPlos / 100.0;
		//Врахування випадку нарахування за неповний місяць
	if (d->needDateLimits){
		int begDay, endDay;
		if ((d->pilgaBeginDate.year() == d->nDate.year()) && (d->pilgaBeginDate.month() == d->nDate.month()))
			begDay = d->pilgaBeginDate.day();
		else
			begDay = 1;
		if ((d->pilgaEndDate.year() == d->nDate.year()) && (d->pilgaEndDate.month() == d->nDate.month()))
			endDay = d->pilgaEndDate.day();
		else
			endDay = d->nDate.daysInMonth();
		s = s * (endDay - begDay + 1) / d->nDate.daysInMonth();
		d->begPilgDayCO = begDay;
		d->endPilgDayCO = endDay;
	}
	s = d_to_m(s); //Не змінювати. Заокруглює до копійок
	return s;
}
//------------------------------------------------------------
double	f_pilga_2_korektura_ploshchi(UNarah_data *d)
{
	double s,s_pilgy, s_pilgy_1;
	if (d->sp_2 == 0)
		s_pilgy = d->pd_2 * d->pd1;
	else //Врахування категорії пільги, коли площа пільги є ненульовою наприклад для вчителів
		s_pilgy = d->sp_2;
	if (s_pilgy > d->ks)
		s_pilgy = d->ks;
	//Врахування того, що площа на другу пільгу не може бути більшою ніж (загальна площа - площа першої пільги)
	s_pilgy_1 = s_pilga_korektura_ploshchi(d);
	if (d->ks - s_pilgy_1 < s_pilgy)
		s_pilgy = d->ks - s_pilgy_1;
	if (s_pilgy < 0)
		s_pilgy = 0;
	d->pilgPlos = s_pilgy * d->kp / 100.0;
	s = d->t * d->p_2 * d->pilgPlos/100.0;
		//Врахування випадку нарахування за неповний місяць
	if (d->needDateLimits){
		int begDay, endDay;
		if ((d->pilgaBeginDate_2.year() == d->nDate.year()) && (d->pilgaBeginDate_2.month() == d->nDate.month()))
			begDay = d->pilgaBeginDate_2.day();
		else
			begDay = 1;
		if ((d->pilgaEndDate_2.year() == d->nDate.year()) && (d->pilgaEndDate_2.month() == d->nDate.month()))
			endDay = d->pilgaEndDate_2.day();
		else
			endDay = d->nDate.daysInMonth();
		s = s * (endDay - begDay + 1) / d->nDate.daysInMonth();
		d->begPilgDayCO_2 = begDay;
		d->endPilgDayCO_2 = endDay;
	}
	s = d_to_m(s); //Не змінювати. Заокруглює до копійок
	return s;
}
//------------------------------------------------------------
double	f_narah_GV(UNarah_data *d)
{
	d->vykorGV = f_read_vykor_GV(d, true);
	double narah;
	if (d->isLichGV)
		narah = d->vykorGV * d->tVGV;
	else{
		if (d->kilkistDnivNoGVAbon == 0){
			narah = d->ludy * d->tOsGV;
			d->vykorGV = 0;
		}
		else{
			short daysInMonth = d->nDate.daysInMonth();
			if (d->kilkistDnivNoGVAbon > daysInMonth)
				d->kilkistDnivNoGVAbon = daysInMonth;
			narah = d->ludy * d->tOsGV * (daysInMonth - d->kilkistDnivNoGVAbon) / daysInMonth;
			d->vykorGV = 0;
			d->kilkistDnivNoLichAbonGV = 0;
		}
	}
	if (d->kilkistDnivNoLichAbonGV != 0){
		narah += d->ludy * d->tOsGV * d->kilkistDnivNoLichAbonGV / d->nDate.daysInMonth();
		d->vykorGV = 0;
	}
	//narah += d->dodSumaOnKuby;
	if (narah < 0)
		narah = 0;
	//qDebug() << " -- рах: " << d->rah << ", викор ГВ:" << d->vykorGV;
	narah = d_to_m(narah);
	return narah;
}
//------------------------------------------------------------
double	f_read_vykor_GV(UNarah_data *d, bool isNarah)
{
	QSqlQuery query, prevPokaznQuery;
	QDate prevDate=d->nDate.addMonths(-1);
	bool neadNarahNoLich=true;
	double vykor, vykorSum=0;
	d->isLichGV = false;
	//d->dodSumaOnKuby = 0;
	d->kilkistDnivNoLichAbonGV = 0;
	d->kilkistDnivNoGVAbon = 0;
	query.exec("SELECT pokaznLichAbonCur.KilkistDnivBezGV, pokaznLichAbonCur.Pokazn AS PokaznCur, \
						pokaznLichAbonCur.Korektura, pokaznLichAbonCur.KilkistDnivBezLichylnyka, \
						pokaznLichBudCur.VykorVodyOsobaInMonth,  lichylnykyAbon.id, pokaznLichAbonCur.id, \
						pokaznLichAbonCur.FactVykor, pokaznLichBudCur.KilkistDnivBezLichylnyka, \
						pokaznLichBudCur.KilkistDnivBezGV, lichylnykyAbon.IsLichylnyk \
				FROM (lichylnykyAbon \
				LEFT JOIN (SELECT * FROM pokaznLichAbon \
							WHERE year(pokaznDate)="+QString::number(d->nDate.year())+" \
								and month(pokaznDate)="+QString::number(d->nDate.month())+") \
					AS pokaznLichAbonCur \
					ON pokaznLichAbonCur.AbonLich_id=lichylnykyAbon.id) \
				LEFT JOIN (SELECT * FROM pokaznLichBud \
							WHERE year(pokaznDate)="+QString::number(d->nDate.year())+" \
								and month(pokaznDate)="+QString::number(d->nDate.month())+") \
					AS pokaznLichBudCur \
					ON pokaznLichBudCur.BudLich_id=lichylnykyAbon.BudLich_id \
				WHERE lichylnykyAbon.Rahunok="+QString::number(d->rah));
	//qDebug() << "----------------\n" << query.lastQuery() << "----------------\n";
	while (query.next()){
		if (query.value(6).toInt()>0 && query.value(10).toBool()){
			neadNarahNoLich=false;
			prevPokaznQuery.exec("SELECT Pokazn FROM pokaznLichAbon \
									WHERE ((year(pokaznDate)="+QString::number(d->nDate.year())+" and month(pokaznDate)<"+QString::number(d->nDate.month())+") \
											or year(pokaznDate)<"+QString::number(d->nDate.year())+") \
											and AbonLich_ID="+query.value(5).toString()+" \
									ORDER BY pokaznDate DESC");
			double prevPokaznOffDate=0;
			if (prevPokaznQuery.seek(0)){
				prevPokaznOffDate = prevPokaznQuery.value(0).toDouble();
			}
			vykor = query.value(1).toDouble() - query.value(2).toDouble() - prevPokaznOffDate;
			if (vykor>0)
				vykorSum += vykor;
			vykor = query.value(7).toDouble();
			if (vykor > 0)
				vykorSum += vykor;
			if (query.value(3).toInt()>0){
				vykor = query.value(4).toDouble() * query.value(3).toInt() * d->ludy / d->nDate.daysInMonth();
				if (vykor > 0)
					vykorSum += vykor;
			}
			d->isLichGV = true;
		}
		else if (query.value(5).toInt()>0 && query.value(10).toBool()){ //немає показника, є абон лічильник
			neadNarahNoLich=false;
			d->isLichGV = true;
			//qDebug() << "Попередження: не внесено показник лічильника для абонента №" << d->rah;
		}
	}
	
	if (neadNarahNoLich){
		query.exec("SELECT pokaznLichBudCur.VykorVodyOsobaInMonth, lichBud.IsLichylnyk, \
					pokaznLichBudCur.KilkistDnivBezLichylnyka, pokaznLichBudCur.KilkistDnivBezGV \
					FROM ((SELECT * FROM abonent WHERE abonent.Rahunok="+QString::number(d->rah)+") AS abon \
					LEFT JOIN (SELECT * FROM lichylnykyBudynkovi \
								WHERE (SELECT max(Kvartyra) FROM abonent \
											WHERE Rahunok="+QString::number(d->rah)+") BETWEEN lichylnykyBudynkovi.StartKvNumber \
																AND lichylnykyBudynkovi.EndKvNumber)  \
						AS lichBud \
						ON lichBud.Budynok_id=abon.Budynok_id) \
					LEFT JOIN (SELECT * FROM pokaznLichBud \
								WHERE year(pokaznDate)="+QString::number(d->nDate.year())+" \
									and month(pokaznDate)="+QString::number(d->nDate.month())+") \
						AS pokaznLichBudCur \
						ON pokaznLichBudCur.BudLich_id=lichBud.id");
		//qDebug() << "\n----------------\n" << query.lastQuery() << "\n----------------\n";
		if (query.seek(0)){
			if (query.value(1).toBool()){
				vykor = query.value(0).toDouble() * d->ludy;
				d->isLichGV = true;
			}
			else
				vykor = 0;
			vykorSum += vykor;
		}
	}
	
	// Врахування днів без лічильника або без ГВ
	if (isNarah){
		query.exec("SELECT lichBud.IsLichylnyk, pokaznLichBudCur.KilkistDnivBezLichylnyka, pokaznLichBudCur.KilkistDnivBezGV, \
						pokaznLichBudCur.VykorVodyOsobaInMonth, \
						lichylnykyAbon.IsLichylnyk, pokaznLichAbonCur.KilkistDnivBezLichylnyka, pokaznLichAbonCur.KilkistDnivBezGV \
					FROM ((((SELECT * FROM abonent WHERE Rahunok="+QString::number(d->rah)+") AS abon \
					LEFT JOIN (SELECT * FROM lichylnykyBudynkovi WHERE (SELECT max(Kvartyra) \
									FROM abonent WHERE Rahunok="+QString::number(d->rah)+") \
										BETWEEN lichylnykyBudynkovi.StartKvNumber \
											AND lichylnykyBudynkovi.EndKvNumber)  AS lichBud \
						ON lichBud.Budynok_id=abon.Budynok_id) \
					LEFT JOIN (SELECT * FROM pokaznLichBud \
								WHERE year(pokaznDate)="+QString::number(d->nDate.year())+" \
									and month(pokaznDate)="+QString::number(d->nDate.month())+") AS pokaznLichBudCur \
						ON pokaznLichBudCur.BudLich_id=lichBud.id) \
					LEFT JOIN (lichylnykyAbon) ON lichylnykyAbon.Rahunok=abon.Rahunok) \
					LEFT JOIN (SELECT * FROM pokaznLichAbon \
								WHERE year(pokaznDate)="+QString::number(d->nDate.year())+" \
									and month(pokaznDate)="+QString::number(d->nDate.month())+") AS pokaznLichAbonCur \
						ON pokaznLichAbonCur.AbonLich_id=lichylnykyAbon.id");
		//qDebug() << "----------------\n" << query.lastQuery() << "----------------\n";
		while (query.next()){
			if ((query.value(1).toInt()>0) || (query.value(2).toInt()>0) || (query.value(5).toInt()>0) || (query.value(6).toInt()>0)){
				int numDaysNoLichBud = query.value(1).toInt();
				int numDaysNoGVBud = query.value(2).toInt();
				int numDaysNoLichAbon = query.value(5).toInt();
				int numDaysNoGVAbon = query.value(6).toInt();
				//double vykorGVOsobaInMonth = query.value(3).toDouble();
				int sumaryNoGVDays = 0;
				if (numDaysNoGVAbon > numDaysNoGVBud)
					sumaryNoGVDays = numDaysNoGVAbon;
				else
					sumaryNoGVDays = numDaysNoGVBud;
				if (query.value(4).toBool()){ //Є абонентський лічильник
					if ((numDaysNoLichAbon - sumaryNoGVDays) > 0)
						d->kilkistDnivNoLichAbonGV += numDaysNoLichAbon - sumaryNoGVDays;
				}
				else{
					if (query.value(0).toBool()){ // Є будинковий лічильник
						if ((numDaysNoLichBud - sumaryNoGVDays) > 0)
							d->kilkistDnivNoLichAbonGV += numDaysNoLichBud - sumaryNoGVDays;
						//d->dodSumaOnKuby += vykorGVOsobaInMonth * d->ludy * d->tVGV * d->kilkistDnivNoLichAbonGV / d->nDate.daysInMonth();
						//d->dodSumaOnKuby += d->ludy * d->tOsGV * d->kilkistDnivNoLichAbonGV / d->nDate.daysInMonth();
						//d->kilkistDnivNoLichAbonGV = 0;
					}
					else{ //немає будинкового лічильника
						d->kilkistDnivNoLichAbonGV -= sumaryNoGVDays;
					}
				}
				d->kilkistDnivNoGVAbon += sumaryNoGVDays;
			}
		}
	}
	
	/*
		qDebug() << "d->kilkistDnivNoLichAbonGV: " << d->kilkistDnivNoLichAbonGV;
		qDebug() << "d->kilkistDnivNoGVAbon: " << d->kilkistDnivNoGVAbon;
		qDebug() << "d->dodSumaOnKuby: " << d->dodSumaOnKuby;
		qDebug() << "d->isLichGV: " << d->isLichGV;
		qDebug() << "neadNarahNoLich: " << neadNarahNoLich;
		qDebug() << "vykorSum: " << vykorSum;
	*/
	
	//Коригування кількості днів без абонентського лічильника до кількості днів у місяці
	if (d->kilkistDnivNoLichAbonGV > d->nDate.daysInMonth())
		d->kilkistDnivNoLichAbonGV = d->nDate.daysInMonth();
	if (d->kilkistDnivNoGVAbon > d->nDate.daysInMonth())
		d->kilkistDnivNoGVAbon = d->nDate.daysInMonth();
	
	return vykorSum;
}
//------------------------------------------------------------
double v_pilga_GV(const UNarah_data *d)
{
	double vykorPilgaOneOs=0;
	if (d->vykorGV/d->ludy > d->normaGV)
		vykorPilgaOneOs = d->normaGV;
	else
		vykorPilgaOneOs = d_to_m(d->vykorGV/d->ludy);
	double vykorForPilga = d_to_m(vykorPilgaOneOs * d->pd);
	return vykorForPilga;
}
//------------------------------------------------------------
double v_pilga_GV_2(const UNarah_data *d)
{
	double vykorPilgaOneOs=0;
	if (d->vykorGV/d->ludy > d->normaGV)
		vykorPilgaOneOs = d->normaGV;
	else
		vykorPilgaOneOs = d_to_m(d->vykorGV/d->ludy);
	double vykorForPilga = d_to_m(vykorPilgaOneOs * d->pd_2);
	return vykorForPilga;
}
//------------------------------------------------------------
double f_pilga_GV(UNarah_data *d)
{
	double s;
	if (d->isLichGV){
		d->pilgObem = v_pilga_GV(d);
		s = d->tVGV * d->p * d->pilgObem/100.0;
	}
	else{
		d->pilgObem = 0;
		if (d->kilkistDnivNoGVAbon == 0)
			s = d->tOsGV * d->p * d->pd/100.0;
		else{
			short daysInMonth = d->nDate.daysInMonth();
			s = d->tOsGV * d->p * d->pd * (daysInMonth - d->kilkistDnivNoGVAbon)/(100.0 * daysInMonth);
		}
	}
	if (d->kilkistDnivNoLichAbonGV != 0){
		d->pilgObem = 0;
		//d->isLichGV = false;
		s += d->tOsGV * d->p * d->pd * d->kilkistDnivNoLichAbonGV / (100.0 * d->nDate.daysInMonth());
	}
	
	//narah += d->ludy * d->tOsGV * d->kilkistDnivNoLichAbonGV / d->nDate.daysInMonth();
	
		//Врахування випадку нарахування пільги за неповний місяць  (обмежений термін дії пільги)
	if (d->needDateLimits){
		int begDay, endDay;
		if ((d->pilgaBeginDate.year() == d->nDate.year()) && (d->pilgaBeginDate.month() == d->nDate.month()))
			begDay = d->pilgaBeginDate.day();
		else
			begDay = 1;
		if ((d->pilgaEndDate.year() == d->nDate.year()) && (d->pilgaEndDate.month() == d->nDate.month()))
			endDay = d->pilgaEndDate.day();
		else
			endDay = d->nDate.daysInMonth();
		s = s * (endDay - begDay + 1) / d->nDate.daysInMonth();
		d->begPilgDayGV = begDay;
		d->endPilgDayGV = endDay;
	}
	
		//Врахування випадку нарахування за неповний місяць (послуги надавались не увесь місяць)
	if ( !d->isLichGV && (d->kilkistDnivNoGVAbon != 0) ){
		int riznycia_dniv = d->nDate.daysInMonth() - (d->endPilgDayGV - d->begPilgDayGV + 1);
		if ( riznycia_dniv < d->kilkistDnivNoGVAbon ){ //кількість днів у які не було подання послуг більша за інтервал часу у який не надавалась пільга
			d->begPilgDayGV += d->kilkistDnivNoGVAbon - riznycia_dniv;
			if (d->begPilgDayGV > d->endPilgDayGV){
				d->begPilgDayGV = d->endPilgDayGV;
				s = 0;
			}
		}
	}
	
	s = d_to_m(s); //Не змінювати. Заокруглює до копійок
	return s;
}
//------------------------------------------------------------
double f_pilga_GV_2(UNarah_data *d)
{
	double s;
	if (d->isLichGV){
		d->pilgObem = v_pilga_GV_2(d);
		s = d->tVGV * d->p_2 * d->pilgObem/100.0;
	}
	else{
		d->pilgObem = 0;
		if (d->kilkistDnivNoGVAbon == 0)
			s = d->tOsGV * d->p_2 * d->pd_2/100.0;
		else{
			short daysInMonth = d->nDate.daysInMonth();
			s = d->tOsGV * d->p_2 * d->pd_2 * (daysInMonth - d->kilkistDnivNoGVAbon)/(100.0 * daysInMonth);
		}
	}
	
	if (d->kilkistDnivNoLichAbonGV != 0){
		d->pilgObem = 0;
		//d->isLichGV = false;
		s += d->tOsGV * d->p_2 * d->pd_2 * d->kilkistDnivNoLichAbonGV / (100.0 * d->nDate.daysInMonth());
	}
	
		//Врахування випадку нарахування пільги за неповний місяць  (обмежений термін дії пільги)
	if (d->needDateLimits){
		int begDay, endDay;
		if ((d->pilgaBeginDate_2.year() == d->nDate.year()) && (d->pilgaBeginDate_2.month() == d->nDate.month()))
			begDay = d->pilgaBeginDate_2.day();
		else
			begDay = 1;
		if ((d->pilgaEndDate_2.year() == d->nDate.year()) && (d->pilgaEndDate_2.month() == d->nDate.month()))
			endDay = d->pilgaEndDate_2.day();
		else
			endDay = d->nDate.daysInMonth();
		s = s * (endDay - begDay + 1) / d->nDate.daysInMonth();
		d->begPilgDayGV_2 = begDay;
		d->endPilgDayGV_2 = endDay;
	}
	
		//Врахування випадку нарахування за неповний місяць (послуги надавались не увесь місяць)
	if ( !d->isLichGV && (d->kilkistDnivNoGVAbon != 0) ){
		int riznycia_dniv = d->nDate.daysInMonth() - (d->endPilgDayGV_2 - d->begPilgDayGV_2 +1 );
		if ( riznycia_dniv < d->kilkistDnivNoGVAbon ){ //кількість днів у які не було подання послуг більша за інтервал часу у який не надавалась пільга
			d->begPilgDayGV_2 += d->kilkistDnivNoGVAbon - riznycia_dniv;
			if (d->begPilgDayGV_2 > d->endPilgDayGV_2){
				d->begPilgDayGV_2 = d->endPilgDayGV_2;
				s = 0;
			}
		}
	}
	
	s = d_to_m(s); //Не змінювати. Заокруглює до копійок
	return s;
}
//------------------------------------------------------------
double f_narah_St(UNarah_data *d)
{
	double narah_St = d->gkalSt * d->vartGkalSt + d->kvadrSt * d->vartKvadrSt;
	return d_to_m(narah_St);
}
//------------------------------------------------------------
double vhSaldoInMonth(int rahunok, short year, short month, bool useCustomVhSaldo, double customVhSaldo)
{
	// обрахунок вхідного сільдо
	short prev_year, prev_month;
	prev_year = year;
	prev_month = month;
	prev_month --;
	if (prev_month == 0){
		prev_month = 12;
		prev_year --;
	}
	double prev_vh_saldo, prev_narah, prev_narah_GV, prev_narah_St, prev_subs, prev_oplach, vh_saldo;
	QSqlQuery vh_saldo_query;
	if (!useCustomVhSaldo){
		vh_saldo_query.exec("SELECT Vh_saldo FROM narahuvannya \
							WHERE (Rahunok="+ QVariant(rahunok).toString() +") \
								and (Year="+ QVariant(prev_year).toString() +") \
								and (Month="+ QVariant(prev_month).toString() +") \
								and Status=0");
		if (vh_saldo_query.seek(0))
			prev_vh_saldo = vh_saldo_query.value(0).toDouble();
		else
			prev_vh_saldo = 0;
	}
	else
		prev_vh_saldo = customVhSaldo;
	
	vh_saldo_query.exec("SELECT sum(Narah), sum(Narah_GV), sum(Narah_St) FROM narahuvannya \
						WHERE (Rahunok="+ QVariant(rahunok).toString() +") \
							and (Year="+ QVariant(prev_year).toString() +") \
							and (Month="+ QVariant(prev_month).toString() +")");
	if (vh_saldo_query.seek(0)){
		prev_narah = vh_saldo_query.value(0).toDouble();
		prev_narah_GV = vh_saldo_query.value(1).toDouble();
		prev_narah_St = vh_saldo_query.value(2).toDouble();
	}
	else{
		prev_narah = 0;
		prev_narah_GV = 0;
		prev_narah_St = 0;
	}
	
	vh_saldo_query.exec("SELECT sum(Suma), sum(Suma_d), sum(Suma_GV), sum(Suma_d_GV) FROM subs \
						WHERE (Rahunok_ID="+ QVariant(rahunok).toString() +") \
							and (Year="+ QVariant(prev_year).toString() +") \
							and (Month="+ QVariant(prev_month).toString() +")");
	if (vh_saldo_query.next()){
		prev_subs = vh_saldo_query.value(0).toDouble() + vh_saldo_query.value(2).toDouble();
		prev_subs += vh_saldo_query.value(1).toDouble() + vh_saldo_query.value(3).toDouble();
	}
	else
		prev_subs = 0;
	
	vh_saldo_query.exec("SELECT SUM(Suma) FROM (SELECT * FROM kvytancii \
						WHERE (Rahunok_id="+QVariant(rahunok).toString()+") \
							and (year(Plata_date)="+ QVariant(prev_year).toString() +") \
							and (month(Plata_date)="+ QVariant(prev_month).toString() +")) \
						GROUP BY Rahunok_ID");
			
	if (vh_saldo_query.next())
		prev_oplach = vh_saldo_query.value(0).toDouble();
	else
		prev_oplach = 0;
	
	double prev_pereved = 0;
	vh_saldo_query.exec("SELECT sum(Suma) FROM narahuvannyaPereved \
						WHERE Rahunok_id="+sqlStr(rahunok)+" \
							and year(CDate)="+sqlStr(prev_year)+" \
							and month(CDate)="+sqlStr(prev_month));
	if (vh_saldo_query.next())
		prev_pereved = uDToM(vh_saldo_query.value(0).toDouble());
	
	vh_saldo = prev_vh_saldo + prev_narah + prev_narah_GV + prev_narah_St - prev_subs - prev_oplach + prev_pereved;
	return d_to_m(vh_saldo);
}
//------------------------------------------------------------
double vhSaldoInMonth(int rahunok, short year, short month)
{
	return vhSaldoInMonth(rahunok, year, month, false);
}
//------------------------------------------------------------
bool inMDateInterval(QDate curDate, QDate pochDate, QDate kincDate)
{
	pochDate.setDate(pochDate.year(), pochDate.month(), 1);
	kincDate.setDate(kincDate.year(), kincDate.month(), kincDate.daysInMonth());
	return (pochDate<=curDate) && (curDate<=kincDate);
}
//------------------------------------------------------------
double normaSpojyvGVNaOsobu(int year, int month)
{
	QSqlQuery query("SELECT normaKubivGVNaOsobu FROM normat_taryf \
								WHERE (year="+QVariant(year).toString()+" and month<="+QVariant(month).toString()+") \
										or (year<"+QVariant(year).toString()+") \
								ORDER BY year DESC, month DESC");
	query.seek(0);
	return query.value(0).toDouble();
}
//------------------------------------------------------------
UNasTaryfInfo nasTaryf(short year, short month)
{
	UNasTaryfInfo taryfInfo;
	QSqlQuery query("SELECT vart_g_kal, normat_taryf_kvadr, vart_kuba_GV, normaKubivGVNaOsobu, vart_na_lud_in_month_GV FROM normat_taryf \
				WHERE year<"+QString::number(year)+" \
					or (year="+QString::number(year)+" and month<="+QString::number(month)+") \
				ORDER BY year DESC, month DESC");
	if (query.seek(0)){
		taryfInfo.vart_g_kal = query.value(0).toDouble();
		taryfInfo.normat_taryf_kvadr = query.value(1).toDouble();
		taryfInfo.vart_kuba_GV = query.value(2).toDouble();
		taryfInfo.normaKubivGVNaOsobu = query.value(3).toDouble();
		taryfInfo.vart_na_lud_in_month_GV = query.value(4).toDouble();
		taryfInfo.valid = true;
	}
	return taryfInfo;
}
//------------------------------------------------------------
double diuchTaryfInMonth(int bud_id, short year, short month)
{
	QSqlQuery query;
	query.exec("SELECT taryf FROM diuch_taryf \
				WHERE Budynok_id="+QString::number(bud_id)+" and Year="+QString::number(year)+" and month="+QString::number(month));
	query.next();
	return uDToM(query.value(0).toDouble());
}
//------------------------------------------------------------
UAbonObPlSubs oplataZgidnoNarahSubs(int rah, short year, short month)
{
#define SUBS_YEAR_BEG_MONTH 10
	QSqlQuery query;
	QDate begDate, endDate(year, month, 1), xDate;
	endDate.setYMD(endDate.year(), endDate.month(), endDate.daysInMonth());
	
	if (endDate.month() >= SUBS_YEAR_BEG_MONTH)
		begDate.setYMD(endDate.year(), SUBS_YEAR_BEG_MONTH, 1);
	else
		begDate.setYMD(endDate.year()-1, SUBS_YEAR_BEG_MONTH, 1);
	
	query.exec("SELECT count(*) FROM plataZgidnoSubs \
				WHERE Rahunok_id="+sqlStr(rah)+" \
					and (cdate(Year&'-'&Month&'-'&'1') BETWEEN \
						cdate("+sqlStr(begDate)+") \
						AND cdate("+sqlStr(endDate)+"))");
	query.next();
	if (query.value(0).toInt() == 0)
		return UAbonObPlSubs();
	
	//читання інформації про необхідність слідкування, щоб Об.пл. по субс. була не більша ніж нарахування
	query.exec("SELECT strTemplate FROM slujb_znach WHERE id=22");
	query.next();
	bool needCheckObPl = query.value(0).toBool();
	
	double vhSaldo = vhSaldoInMonth(rah, begDate.year(), SUBS_YEAR_BEG_MONTH);
	double obPlCoSuma, obPlGvSuma, narahCo, narahGv, narahSt, oplata, kpsCO, kpsGV, perevedZaborg;
	bool needOpInMonth;
	for (xDate=begDate; xDate<endDate; xDate = xDate.addMonths(1)){
		query.exec("SELECT count(*) FROM plataZgidnoSubs \
					WHERE Rahunok_id="+sqlStr(rah)+" \
						and Year="+sqlStr(xDate.year())+" \
						and Month="+sqlStr(xDate.month()));
		query.next();
		query.value(0).toInt() ? needOpInMonth=true : needOpInMonth=false;
		if (needOpInMonth){ // розрахунок вхідного сальдо згідно обов'язкової плати у місяці
			query.exec("SELECT Plata_CO, Plata_GV FROM plataZgidnoSubs \
						WHERE Rahunok_id="+sqlStr(rah)+" \
							and Year="+sqlStr(xDate.year())+" \
							and Month="+sqlStr(xDate.month()));
			query.next();
			obPlCoSuma = query.value(0).toDouble();
			obPlGvSuma = query.value(1).toDouble();
			
			query.exec("SELECT sum(Narah), sum(Narah_GV), sum(Narah_St) FROM narahuvannya \
						WHERE (Rahunok="+sqlStr(rah)+") \
							and (Year="+sqlStr(xDate.year())+") \
							and (Month="+sqlStr(xDate.month())+")");
			query.next();
			narahCo = query.value(0).toDouble();
			narahGv = query.value(1).toDouble();
			narahSt = query.value(2).toDouble();
			
			query.exec("SELECT SUM(Suma) FROM (SELECT * FROM kvytancii \
						WHERE (Rahunok_id="+sqlStr(rah)+") \
							and (year(Plata_date)="+sqlStr(xDate.year())+") \
							and (month(Plata_date)="+sqlStr(xDate.month())+")) \
						GROUP BY Rahunok_ID");
			query.next();
			oplata = query.value(0).toDouble();
			
			query.exec("SELECT sum(Suma) FROM narahuvannyaPereved \
						WHERE Rahunok_id="+sqlStr(rah)+" \
							and year(CDate)="+sqlStr(xDate.year())+" \
							and month(CDate)="+sqlStr(xDate.month()));
			query.next();
			perevedZaborg = query.value(0).toDouble();
			
			query.exec("SELECT sum(suma), sum(suma_GV) FROM korektura_s \
									WHERE year="+sqlStr(year)+" \
									and month="+sqlStr(month)+" \
									and id="+sqlStr(rah));
			query.next();
			kpsCO = query.value(0).toDouble();
			kpsGV = query.value(1).toDouble();
			
			if (needCheckObPl){ // слідкування, щоб Об.пл. по субс. була не більша ніж нарахування
				if (obPlCoSuma > (narahCo - kpsCO) )
					obPlCoSuma = narahCo - kpsCO;
				if (obPlGvSuma > (narahGv - kpsGV) )
					obPlGvSuma = narahGv - kpsGV;
			}
			
			//виключити від'ємність обов'язкової плати
			if (obPlCoSuma < 0) 
				obPlCoSuma = 0;
			if (obPlGvSuma < 0)
				obPlGvSuma = 0;
			
			vhSaldo = vhSaldo + obPlCoSuma + obPlGvSuma + narahSt - oplata + kpsCO + kpsGV + perevedZaborg;
		}
		else{ // розрахунок вхідного сальдо за стандартним аглоритмом з вказаним вхідним сальдо
			QDate dt = xDate.addMonths(1);
			vhSaldo = vhSaldoInMonth(rah, dt.year(), dt.month(), true, vhSaldo);
		}
	}
	
	return UAbonObPlSubs(vhSaldo);
}
//------------------------------------------------------------
