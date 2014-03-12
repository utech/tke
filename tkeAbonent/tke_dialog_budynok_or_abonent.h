//------------------------------------------------------------
//
// Implamantation of tke_Dialog_narah class.
// Created by Roman Mulyk 10.11.2010
//
//------------------------------------------------------------

#ifndef TKE_DIALOG_BUDYNOK_OR_ABONENT_H
#define TKE_DIALOG_BUDYNOK_OR_ABONENT_H

#include <QDialog>
#include <QtDebug>
#include <QSqlQuery>

#include "ui_tke_dialog_budynok_or_abonent.h"

class tke_dialog_budynok_or_abonent : public QDialog
{
  Q_OBJECT

  public:
	tke_dialog_budynok_or_abonent(QString title="", QDate dt = QDate::currentDate(), bool clearBudItem=false, QWidget *parent = 0);
	
	int budId();
	int rahunok();
	QDate date();
		
	~tke_dialog_budynok_or_abonent();
  public slots:
	void filterTypeChanged();
	void populatePib();
  private:
	Ui::tke_dialog_budynok_or_abonent ui;
	
};

#endif
