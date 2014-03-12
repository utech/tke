//------------------------------------------------------------
//
// Declaration of tke_form_struct_GV class.
// Created by Roman Mulyk 16.06.2008
//
//------------------------------------------------------------

#ifndef TKE_FORM_STRUCT_GV_H
#define TKE_FORM_STRUCT_GV_H

#include <QWidget>
#include <QSpinBox>
#include <QMessageBox>
#include <QtSql>
#include <QVariant>
#include <QSettings>

#include "../tkeOrganizacii/udiff_func.h"

#include "ui_tke_form_struct_GV.h"
//------------------------------------------------------------
class tke_form_struct_GV : public QWidget
{
  Q_OBJECT
  public:
	tke_form_struct_GV( QWidget * parent = 0);
	~tke_form_struct_GV();

  private slots:
	void populateKotelni();
	void populateBudynky();
	void populateLichBudynkiv();
	void populateLichAbonentiv();
	void updateKotelni(int row, int col);
	void updateBudynky(int row, int col);
	void updateLichBudynkiv(int row, int col);
	void updateLichAbonentiv(int row, int col);
	void toolButton_reload_clicked();
	void toolButton_addKotelni_clicked();
	void toolButton_delKotelni_clicked();
	void toolButton_addBudynky_clicked();
	void toolButton_delBudynky_clicked();
	void toolButton_addBudLich_clicked();
	void toolButton_delBudLich_clicked();
	void toolButton_addAbonLich_clicked();
	void toolButton_delAbonLich_clicked();
  signals:
	void requestRahunok(int);
	
  private:
	Ui::tke_Form_struct_GV ui;
	
	QList<QTableWidgetItem *> itemsList;
	QSqlQuery adQuery;
	int nextId;
	
	int kotelnyaId;
	int budynokId;
	int budLichId;
	
	QAbstractItemModel *budLichModel;
	QAbstractItemModel *abonLichModel;
};
//------------------------------------------------------------
#endif




