/****************************************************************************
**
** Created: Roman Mulyk 04.01.2007
**
** В модулі реалізовані функції створення документів для друку з виводом форм
****************************************************************************/

#ifndef UPRINT_DOCS_ORGANIZ_H
#define UPRINT_DOCS_ORGANIZ_H

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
#include "udiff_func.h"
#include "../tkeAbonent/tke_math.h"
#include <UNumberToStringConverter>


// Srtuctures

//Class
class UPrintDocsOrganiz
{
    public:
        UPrintDocsOrganiz();
		virtual ~UPrintDocsOrganiz();
		
		UPrintForm *printform;
		
			//функції друку
		void print_vid_narah_za_gar_vodu(int cur_year, int cur_month);
		void print_vidomist_narahuvannya_i_postuplennya_dohodiv(int cur_year, int cur_month);
		void print_nakopychuvalna_vidomist_narahuvannya_i_postuplennya_dohodiv(int cur_year, int cur_month);
		void print_vidomist_oplaty_za_CO(int cur_year, int cur_month);
		void print_vidomist_realizacii(int cur_year, int cur_month);
		void print_rahunok_1(int cur_year, int cur_month, int ugodaNum, int isAvans, double avansSum);
		void print_rahunok_2(int cur_year, int cur_month, int ugodaNum, int isAvans, double avansSum);
        void print_act_vykon_robit(QDate dt, int ugodaNum);
		void print_vidomist_borjnykiv(QDate pochDate, QDate kincDate, int finansFromId, bool needMinSum, double minSum);
		void print_podatkova_nakladna(int cur_year, int cur_month, int ugodaNum, double sumCO, double sumGV);
		
    public slots:

	signals:
		
    private:
		QTextCodec *codec;
		QSqlTableModel tableModel;
		QSqlRecord record;
		QDate date;
		QStringList monthlist;
        QStringList monthListRod;
		
//Function declatarion
	
};

#endif
