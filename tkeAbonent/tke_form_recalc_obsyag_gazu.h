//------------------------------------------------------------
//
// Implamantation of tke_Dialog_narah class.
// Created by Roman Mulyk 29.10.2010
//
//------------------------------------------------------------

#ifndef TKE_FORM_RECALC_OBSYAG_GAZU_H
#define TKE_FORM_RECALC_OBSYAG_GAZU_H

#include <QWidget>
#include <QDate>
#include <QSqlQuery>
#include <UTableWidgetSqlExt>

#include "ui_tke_form_recalc_obsyag_gazu.h"

class tke_form_recalc_obsyag_gazu : public QWidget
{
	Q_OBJECT

  public:
	tke_form_recalc_obsyag_gazu(QDate dt = QDate::currentDate(), QWidget *parent = 0);
		
	~tke_form_recalc_obsyag_gazu();
	
	void initBudPokaznTable();
  public slots:
	void pushButton_createMonthRozrah_clicked();
	void pushButton_deleteMonthRozrah_clicked();
	void pushButton_calcValuesRozrahunok_clicked();
	void pushButton_printAkt_clicked();
	void pushButton_printRozrahunok_clicked();
	void pushButton_copyValuesFromPrevMonth_clicked();
	void pushButton_deleteButPokaznInRozrahunok_clicked();
	void pushButton_calcValuesBud_clicked();
	
	void populateBudPokaznTable();
	void calcSumPocazn();
	
	void tableWidget_budPokazn_itemChanged(QTableWidgetItem * item);
  signals:
	
  private:
	Ui::tke_form_recalc_obsyag_gazu ui;
		
	QDate date;
	UTableWidgetSqlExt *twExt;
};

#endif
