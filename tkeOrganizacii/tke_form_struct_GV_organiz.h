//------------------------------------------------------------
//
// Declaration of tke_form_struct_GV_organiz class.
// Created by Roman Mulyk 16.06.2008
//
//------------------------------------------------------------

#ifndef tke_form_struct_GV_organiz_H
#define tke_form_struct_GV_organiz_H

#include <QWidget>
#include <QSpinBox>
#include <QMessageBox>
#include <QtSql>
#include <QVariant>
#include <QSettings>

#include "../tkeOrganizacii/udiff_func.h"

#include "ui_tke_form_struct_GV_organiz.h"
//------------------------------------------------------------
class tke_form_struct_GV_organiz : public QWidget
{
  Q_OBJECT
  public:
	tke_form_struct_GV_organiz( QWidget * parent = 0);
	~tke_form_struct_GV_organiz();

  private slots:
	void populateKotelni();
	void populateLichZagaln();
	void populateLichPidrozdily();
	void updateKotelni(int row, int col);
	void updateLichZagaln(int row, int col);
	void updateLichPidrozdiliv(int row, int col);
	void toolButton_reload_clicked();
	void toolButton_addKotelni_clicked();
	void toolButton_delKotelni_clicked();
	void toolButton_addZagLich_clicked();
	void toolButton_delZagLich_clicked();
	void toolButton_addPidrozdLich_clicked();
	void toolButton_delPidrozdLich_clicked();
  signals:
	void requestRahunok(int);
	
  private:
	Ui::tke_Form_struct_GV_organiz ui;
	
	QList<QTableWidgetItem *> itemsList;
	QSqlQuery adQuery;
	int nextId;
	
	int kotelnyaId;
	int budLichId;
	
	QAbstractItemModel *zagLichModel;
	QAbstractItemModel *pidrozdLichModel;
};
//------------------------------------------------------------
#endif




