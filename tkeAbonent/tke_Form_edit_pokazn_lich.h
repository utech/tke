//------------------------------------------------------------
//
// Implamantation of tke_Form_edit_pokazn_lich class.
// Created by Roman Mulyk 10.11.2008
//
//------------------------------------------------------------

#ifndef TKE_FORM_EDIT_POKAZN_LICH_H
#define TKE_FORM_EDIT_POKAZN_LICH_H

#include <QWidget>
#include <QtSql>
#include <QMessageBox>
#include <QtDebug>

#include "tke_dialog_narah.h"
#include "../tkeOrganizacii/udiff_func.h"

#include "ui_tke_Form_edit_pokazn_lich.h"

class tke_Form_edit_pokazn_lich : public QWidget
{
	Q_OBJECT
  public:
	tke_Form_edit_pokazn_lich( QWidget * parent = 0, short year=2008, short month=10);
	~tke_Form_edit_pokazn_lich();
	
	void populateRow(int row, QSqlRecord record);
  public slots:
	void loadPokazn();
	void loadPokaznFromPrevMonth();
	void saveItemValue(QTableWidgetItem *item);
	void tableWidgetSelectionChanged();
  private:
	Ui::tke_Form_edit_pokazn_lich ui;
  signals:
	void needRahunok(int rahunok);
};

#endif
