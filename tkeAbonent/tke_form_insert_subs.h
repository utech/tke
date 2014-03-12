//------------------------------------------------------------
//
// Implamantation of tke_Dialog_narah class.
// Created by Roman Mulyk 24.11.2006
//
//------------------------------------------------------------

#ifndef TKE_FORM_INSERT_SUBS_H
#define TKE_FORM_INSERT_SUBS_H

#include <QDialog>
#include <QDate>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTextCodec>
#include <QTextCursor>
#include <QSqlTableModel>
#include <QSqlRecord>

#include "ui_tke_form_insert_subs.h"
#include "tke_math.h"

// Srtuctures

// Classes
class tke_Form_insert_subs : public QWidget
{
        Q_OBJECT

    public:
        tke_Form_insert_subs(QWidget *parent = 0, short year=2006, short month=10, bool donarah=true);
		
		int get_Month();
		int get_Year();
		
		~tke_Form_insert_subs();
		//friend class tke_MainWindow;
    public slots:
		void read_data();
		void save_data();
		void clear_subs_form();
		void show_subs_sum();

	signals:
        void u_abonent_rahunok_changes(int rahunok);
		
    private:
		Ui::Form_insert_subs ui;
		
		QDate date;
		QTextCodec *codec;
		QSqlQuery *query;
		QSqlTableModel tableModel;
		short operaciya; // 0-створено запис, 1-змінено запис, 2-видалено запис
		short zapys; //Нумерація запису
		QTextBlockFormat blockFormat;
		QTextCharFormat textCharFormat, textCharFormat_bold, textCharFormat_red, textCharFormat_green, temp_textCharFormat;
		QTextCursor cursor;
		bool q_ok;
		bool isDonarah;
		double subs_donarah, sum_donarah;
		QString subsCol;
		//QString subsVydText;
};

#endif
