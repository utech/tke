//------------------------------------------------------------
//
// Implamantation of tke_Dialog_narah class.
// Created by Roman Mulyk 26.11.2006
//
//------------------------------------------------------------

#ifndef TKE_FORM_INSERT_KVYT_H
#define TKE_FORM_INSERT_KVYT_H
// QT standart includes
#include <QDialog>
#include <QDate>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTextCodec>
#include <QTextCursor>
#include <QSqlTableModel>
#include <QSqlRecord>
//own includes
#include "ui_tke_form_insert_kvyt.h"
#include "tke_math.h"

// Srtuctures

// Classes
class tke_Form_insert_kvyt : public QWidget
{
        Q_OBJECT

    public:
        tke_Form_insert_kvyt(QWidget *parent = 0, short year=2006, short month=10);
		
		int get_Month();
		int get_Year();
		
		~tke_Form_insert_kvyt();
    public slots:
		void read_data();
		void save_data();
		void narah_suma_spinbox_acepted();
		void new_pack();
		void show_sum();

	signals:
        void u_abonent_rahunok_changes(int rahunok);
		
    private:
		Ui::Form_insert_kvyt ui;
		
		QDate date;
		QDate x_date;
		QTextCodec *codec;
		QSqlQuery *query;
		QSqlQuery *delQuery;
		QSqlTableModel tableModel;
		short zapys; //Нумерація запису
		QTextBlockFormat blockFormat;
		QTextCharFormat textCharFormat, textCharFormat_bold, textCharFormat_red, textCharFormat_green, temp_textCharFormat;
		QTextCursor cursor;
		bool q_ok;
		double pachka_sum;
};

#endif
