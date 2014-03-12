//------------------------------------------------------------
//
// Implamantation of tke_Dialog_narah class.
// Created by Roman Mulyk 12.09.2006
//
//------------------------------------------------------------

#ifndef TKE_DIALOG_PRINT_KVYTANCII_H
#define TKE_DIALOG_PRINT_KVYTANCII_H

#include <QDialog>
#include <QtDebug>
#include <QSqlQuery>

#include "ui_tke_dialog_print_kvytancii.h"

class tke_Dialog_print_kvytancii : public QDialog
{
  Q_OBJECT

  public:
	tke_Dialog_print_kvytancii(QWidget *parent = 0, QString title="", short year=2006, short month=10, bool clearItem=false);
		
	int get_Bud_id();
	int get_Year();
	int get_Month();
	
	void setBudHidden(bool hide = true);
	void setDateHidden(bool hide = true);
		
	~tke_Dialog_print_kvytancii();
  public slots:

  private:
	Ui::tke_Dialog_print_kvytancii ui;
};

#endif
