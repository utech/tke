/****************************************************************************
**
** Created: Roman Mulyk 24.09.2006
**
**
****************************************************************************/

#ifndef TKE_MATH_H
#define TKE_MATH_H

#include <QString>
#include <QVariant>
#include <QDate>
#include <QSqlQuery>

#include <UMath>

// Srtuctures
struct UNarah_data
{
	int				rah;	//рахунок
	short			bud;	//id будинку
	double 			s;		//площа квартири
	double			zs;		//зайва площа
	unsigned char	pd;		//пільга для (кількості чоловік)
	unsigned char	p;		//відсоток пільги
	unsigned char	pd_2;	//пільга для (кількості чоловік)_2
	unsigned char	p_2;	//відсоток пільги_2
	double			sp;		//площа на пільгу
	double			sp_2;		//площа на пільгу
	double			pds;	//площа на сім'ю
	double			pd1;	//площа на одного
	double			ks;		//коректура по площі
	int				kp;		//відсоток по коректурі
	double  		t; 		//тариф для будинку
	short			kimn;	//кількість кімнат
	short			ludy;	//кількість проживаючих
	QDate			nDate;	//дата нарахування
	double			normaGV;//нормативна кількість гарячої води,яку використовує одна особа за місяць
	double			vykorGV;//загальне використання води абонентом
	double			gkalSt; //Кількість гігакалорій опалення по стояках
	double			kvadrSt;//Квадрати для опалення стояками
	double			vartGkalSt;//Вартість гігакалорії опалення стояків
	double			vartKvadrSt;//Вартість квадрата опалення стояків
	double			pilgPlos;//Пільгова площа
	double			pilgObem;//Пільговий об'єм гарячої води
	double			tVGV;//Тариф куба ГВ
	double			tOsGV; //Тариф на особу в місяць
	bool 			isLichGV; //Є лічильник по ГВ, по якому внесено показник
	double			dodSumaOnKuby; //Кількість днів без лічильника ГВ
	short			kilkistDnivNoLichAbonGV; //Кількість днів без абонентського лічильника ГВ
	short			kilkistDnivNoGVAbon;
	bool			needDateLimits; //Вимога використання обмежень по даті для пільги
	QDate			pilgaBeginDate; //Дата початку надання пільги
	QDate			pilgaEndDate; //Дата закінчення надання пільги
	QDate			pilgaBeginDate_2; //Дата початку надання пільги
	QDate			pilgaEndDate_2; //Дата закінчення надання пільги
	short			begPilgDayCO; //Початковий день надання пільги по ЦО
	short			endPilgDayCO; //Кінцевий день надання пільги по ЦО
	short			begPilgDayGV; //Початковий день надання пільги по ГВ
	short			endPilgDayGV; //Кінцевий день надання пільги по ГВ
	short			begPilgDayCO_2; //Початковий день надання пільги по ЦО
	short			endPilgDayCO_2; //Кінцевий день надання пільги по ЦО
	short			begPilgDayGV_2; //Початковий день надання пільги по ГВ
	short			endPilgDayGV_2; //Кінцевий день надання пільги по ГВ
	short			pochDayPoslugCO; //Початковий день надання послуг ГВ (тільки ініціалізується для контретного місяця)
	short			kincDayPoslugCO; //Кінцевий день надання послуг ГВ (тільки ініціалізується для контретного місяця)
	short			pochDayPoslugGV; //Початковий день надання послуг ГВ (тільки ініціалізується для контретного місяця)
	short			kincDayPoslugGV; //Кінцевий день надання послуг ГВ (тільки ініціалізується для контретного місяця)
};

struct UNasTaryfInfo
{
	UNasTaryfInfo(){
		vart_g_kal=0; normat_taryf_kvadr=0; vart_kuba_GV=0; normaKubivGVNaOsobu=0; valid=false;
	};
	double vart_g_kal;
	double normat_taryf_kvadr;
	double vart_kuba_GV;
	double vart_na_lud_in_month_GV;
	double normaKubivGVNaOsobu;
	bool valid;
};

struct UOrgTaryfInfo
{
	UOrgTaryfInfo(){
		vart_g_kal = 0;
		vart_g_kal_gar_vody = 0;
		vart_kvadr_CO = 0;
		vart_kuba_GV = 0;
		valid=false;
	};
	UOrgTaryfInfo(int year, int month){
		valid = populate(year, month);
	};
	bool populate(int year, int month){
		bool doneOk=true;
		QSqlQuery query("SELECT vart_g_kal, vart_g_kal_gar_vody, vart_kvadr_CO, vart_kuba_GV FROM normat_taryf_organiz \
								WHERE (year="+QVariant(year).toString()+" and month<="+QVariant(month).toString()+") \
										or (year<"+QVariant(year).toString()+") \
								ORDER BY year DESC, month DESC");
		if (query.seek(0)){
			vart_g_kal = query.value(0).toDouble();
			vart_g_kal_gar_vody = query.value(1).toDouble();
			vart_kvadr_CO = query.value(2).toDouble();
			vart_kuba_GV = query.value(3).toDouble();
			doneOk=true;
		}
		else{
			vart_g_kal = 0;
			vart_g_kal_gar_vody = 0;
			vart_kvadr_CO = 0;
			vart_kuba_GV = 0;
			doneOk=false;
		}
		return doneOk;
	};
  public:
	double vart_g_kal;
	double vart_g_kal_gar_vody;
	double vart_kvadr_CO;
	double vart_kuba_GV;
  private:
	bool valid;
};

struct UAbonObPlSubs
{
	bool need;
	double suma;
	UAbonObPlSubs():need(false),suma(0){};
	UAbonObPlSubs(double opSum):need(true),suma(opSum){};
};

//Function declatarions
	double	f_narah(UNarah_data *d);
	double	f_pilga(UNarah_data *d);
	double	f_pilga_2(UNarah_data *d);
	double	f_narah_korektura_ploshchi(UNarah_data *d);
	double	s_pilga_korektura_ploshchi(const UNarah_data *d);
	double	f_pilga_korektura_ploshchi(UNarah_data *d);
	double	f_pilga_2_korektura_ploshchi(UNarah_data *d);
	double	f_narah_GV(UNarah_data *d);
	double	f_read_vykor_GV(UNarah_data *d, bool isNarah=false);
	double	f_pilga_GV(UNarah_data *d);
	double	f_pilga_GV_2(UNarah_data *d);
	double	f_narah_St(UNarah_data *d);
	double	vhSaldoInMonth(int rahunok, short year, short month, bool useCustomVhSaldo, double customVhSaldo=0);
	double	vhSaldoInMonth(int rahunok, short year, short month);
	bool inMDateInterval(QDate curDate, QDate pochDate, QDate kincDate);
	
	double normaSpojyvGVNaOsobu(int year, int month);
	UNasTaryfInfo nasTaryf(short year, short month);
	UOrgTaryfInfo orgTaryf(short year, short month);
	double diuchTaryfInMonth(int bud_id, short year, short month);
	UAbonObPlSubs oplataZgidnoNarahSubs(int rah, short year, short month);
#endif
