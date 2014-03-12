//------------------------------------------------------------
//
// Declaration of tke_form_extendedSearch class.
// Created by Roman Mulyk 14.06.2008
//
//------------------------------------------------------------

#ifndef TKE_FORM_EXTENDEDSEARCH
#define TKE_FORM_EXTENDEDSEARCH

#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QSqlQueryModel>

#include "ui_tke_form_extendedSearch.h"
//------------------------------------------------------------
class tke_form_extendedSearch : public QWidget
{
Q_OBJECT
public:
	tke_form_extendedSearch( QWidget * parent = 0);
	~tke_form_extendedSearch();
private:
	Ui::tke_Form_extendedSearch ui;
private slots:
	void searchAbonents();
	void pushButton_openRahunok_clicked();
signals:
	void requestRahunok(int);
};
//------------------------------------------------------------
#endif




