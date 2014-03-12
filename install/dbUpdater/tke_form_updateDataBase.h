//------------------------------------------------------------
//
// Declaration of tke_form_updateDataBase class.
// Created by Roman Mulyk 16.01.2009
//
//------------------------------------------------------------

#ifndef TKE_FORM_UPDATEDATABASE_H
#define TKE_FORM_UPDATEDATABASE_H

#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>

#include "ui_tke_form_updateDataBase.h"
//------------------------------------------------------------
class tke_form_updateDataBase : public QWidget
{
Q_OBJECT
public:
	tke_form_updateDataBase( QWidget * parent = 0);
	~tke_form_updateDataBase();
public slots:
	
private:
	Ui::tke_form_updateDataBase ui;
signals:
	
};
//------------------------------------------------------------
#endif




