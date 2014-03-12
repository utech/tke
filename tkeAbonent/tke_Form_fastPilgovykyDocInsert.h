//------------------------------------------------------------
//
// Implamantation of tke_Form_fastPilgovykyDocInsert class.
// Created by Roman Mulyk 26.01.2009
//
//------------------------------------------------------------

#ifndef TKE_FORM_FASTPILGOVYKYDOCINSERT_H
#define TKE_FORM_FASTPILGOVYKYDOCINSERT_H

#include <QWidget>
#include <QtSql>
#include <QMessageBox>
#include <QtDebug>

#include "../tkeOrganizacii/udiff_func.h"

#include "ui_tke_Form_fastPilgovykyDocInsert.h"

class tke_Form_fastPilgovykyDocInsert : public QWidget
{
	Q_OBJECT
  public:
	tke_Form_fastPilgovykyDocInsert( QWidget * parent = 0);
	~tke_Form_fastPilgovykyDocInsert();
  public slots:
	void populatePilgovykyTable();
	void saveItem(QTableWidgetItem *item);
  private:
	Ui::tke_Form_fastPilgovykyDocInsert ui;
  signals:
	void needRahunok(int rahunok);
};

#endif
