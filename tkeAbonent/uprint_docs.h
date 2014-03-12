/****************************************************************************
**
** Created: Roman Mulyk 04.01.2007
**
** В модулі реалізовані функції створення документів для друку з виводом форм
****************************************************************************/

#ifndef UPRINT_DOCS_H
#define UPRINT_DOCS_H

#include <QString>
#include <QVariant>
#include <QTextCodec>
#include <QTextLength>
#include <QTextTable>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDate>
#include <QTextTableCell>
#include <QTextCursor>
#include <QMessageBox>
#include <QSqlTableModel>
#include <UPostupForm>
#include <UPrintForm>
#include "tke_math.h"
#include "../tkeOrganizacii/udiff_func.h"

class UPrintDocs
{
    public:
        UPrintDocs();
		virtual ~UPrintDocs();
		
		UPrintForm *printform;
		
			//функції друку
		void print_kvytancii(int prev_month, int prev_year, bool one_kvyt, int rah_bud, QList<int> rahList = QList<int>()); // one_kvyt (true - в rah_bud даємо рахунок абонента (друкує одну квитанцію)
																						 //		(false - в rah_bud даємо id будинку (друкує багато квитанцію)
		void print_vidomist_pilg(int cur_year, int cur_month);
		void print_vidomist_pilg_kategor(int cur_year, int cur_month);
		void print_zvit_pilg_kategor(int cur_year, int cur_month);
		void print_vidomist_narah(int cur_year, int cur_month);
		void print_dovidka_oderj_subs(int cur_year, int cur_month);
		void print_forma_akt_vytrat_teploenergii();
		void print_diuch_taryf(int cur_year, int cur_month);
		void print_vidomist_borjnykiv(QDate pochDate, QDate kincDate, double minBorg, int BudId, unsigned char abonType);
		void print_vidomist_borjnykiv_riznyc(QDate pochDate, QDate kincDate, double minBorg, int BudId); //Різницева схема
		void print_vidomist_wrong_subs(int cur_year, int cur_month);
		void print_vidomist_narah_opl_za_CO_dlya_viddilu_subs(int cur_year, int cur_month);
		void print_dovidnyk_abonentiv();
		void print_dovidnykAbonentivVRozriziNadannyaPoslug(int budId);
		void print_vidomist_podachi_documentiv_na_pilgy();
		void print_zvit_forma_n_2_pilga_2007(int year, int month);
		void print_vidnoshennya_pro_vidshkoduvannya_pilg_CO(int year, int month);
		void print_vidomist_pererahunku_pilg_CO(int year, int month);
		void print_vidnoshennya_pro_vidshkoduvannya_pilg_GV(int year, int month);
		void print_vidomist_pererahunku_pilg_GV(int year, int month);
		void print_dovidka_u_viddil_subs(int rahunok, int year, int month, double suma=0);
		void print_dovidka_u_viddil_subs_2(int rahunok, int year, int month, double suma=0);
		void print_zagalna_vidomist_narahuvannya_i_postuplennya(int year, int month);
		void print_vidomost_narahuvannya_dlya_abon_z_lich(short year, short month);
		void print_vidomistOtrymannyaPoslugPoBudynkah(short year, short month);
		void print_vidomistOtrymannyaPoslugPoKotelnyah(short year, short month);
		void print_vidomistSpojyvannyaCOZaRik(short year, short month, int budId, int rah=0);
		void print_vidomistSpojyvannyaGVZaRik(short year, short month, int budId, int rah=0);
		void print_vidomistSamovilnoVidkluchenyh(short year, short month);
		void print_vidomistDebetoveKredytoveSaldo(short year, short month);
		void print_kvytanciiDlyaBeznadiynoiZaborg(int prev_month, int prev_year, bool one_kvyt, int rah_bud);
		void print_vidomistNadannyaPoslugPoBudTaKot(int kId, int bId);
		
		void print_rozrahunok_pererahuvannya_obsyagu_tepl_energ(int rozrahId);
		void print_printVidomistBorjnykivOtrymSubs(QDate dt);
		void print_printVidomistZaborgPoPeriodah(QDate dt);
		QString _zaborgNaselQuery(const QDate & begDate,const QDate & curDate);
		QString _zaborgOrgQuery(const QDate & begDate,const QDate & curDate);
		__inline__ double np(double val);
    public slots:

	signals:
		
    private:
		QTextCodec *codec;
		QSqlTableModel tableModel;
		QSqlRecord record;
		QDate date;
		QStringList monthlist;
};
//--------------------utech--------------------utech--------------------utech--------------------
#endif
