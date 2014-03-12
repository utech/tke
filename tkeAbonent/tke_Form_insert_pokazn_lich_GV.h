//------------------------------------------------------------
//
// Declaration of tke_Form_insert_pokazn_lich_GV class.
// Created by Roman Mulyk 16.06.2008
//
//------------------------------------------------------------

#ifndef TKE_FORM_INSERT_POKAZN_LICH_GV_H
#define TKE_FORM_INSERT_POKAZN_LICH_GV_H

#include <QWidget>
#include <QSpinBox>
#include <QMessageBox>
#include <QtSql>
#include <QVariant>
#include <QSettings>

#include "../tkeOrganizacii/udiff_func.h"
#include "tke_math.h"
#include <UStandardDelegate>
#include <UPostupForm>

#include "ui_tke_Form_insert_pokazn_lich_GV.h"
//------------------------------------------------------------
class tke_Form_insert_pokazn_lich_GV : public QWidget
{
  Q_OBJECT
  public:
	tke_Form_insert_pokazn_lich_GV(  int curYear=2008, int curMonth=10, QWidget * parent = 0);
	~tke_Form_insert_pokazn_lich_GV();
	
	void calcPokaznLich(int row);
  public slots:
	void populateLichBudynkiv();
	void populateLichAbonentiv();
	void updateLichBudynkiv(int row, int col);
	void updateLichAbonentiv(int row, int col);
	void toolButton_reload_clicked();
	void toolButton_calcOsobaTaryf_clicked();
	void dateChanged();
	double rozrahVytratyTeplaVDenBezLich(int row);
	void showPidsumkyBud();
	void showPidsumkyAbon();
	void curAbonentChanged();
	void findAbonentPokazn(int Rahunok);
	void toolButton_findRahunok_clicked();
  signals:
	void requestRahunok(int);
	
  private:
	Ui::tke_Form_insert_pokazn_lich_GV ui;
	
	QList<QTableWidgetItem *> itemsList;
	QSqlQuery adQuery;
	int nextId;
	QDate curDate;
	QDate prevDate;
	QTableWidgetItem *item;
	
	int budLichId;
};
//------------------------------------------------------------
class UFactSpojyvDelegate : public QItemDelegate
{
  public:
	UFactSpojyvDelegate(QObject *parent = 0) : QItemDelegate(parent){};
	void paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const;
};
#endif




