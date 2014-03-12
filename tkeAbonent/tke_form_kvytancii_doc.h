//------------------------------------------------------------
//
// Implamantation of tke_Dialog_narah class.
// Created by Roman Mulyk 20.01.2007
//
//------------------------------------------------------------

#ifndef TKE_FORM_KVYTANCII_DOC_H
#define TKE_FORM_KVYTANCII_DOC_H
// QT standart includes
#include <QDate>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QTextCodec>
#include <QPrinter>
#include <QPrintDialog>
//own includes
#include "ui_tke_form_kvytancii_doc.h"
#include "tke_math.h"

// Srtuctures

// Classes
class tke_form_kvytancii_doc : public QWidget
{
        Q_OBJECT

    public:
        tke_form_kvytancii_doc(QWidget *parent = 0);
		
		int get_Month();
		int get_Year();
		
		~tke_form_kvytancii_doc();
    public slots:
		void dateFiltrChanged();
		void showHtml(const QModelIndex &index);
		void printDoc();
		void printDocOn();

    private:
		Ui::tke_form_kvytancii_doc_widget ui;

		QDate date;
		QTextCodec *codec;
		QSqlTableModel *tableModel;
		QPrinter *prnt;
};

#endif
