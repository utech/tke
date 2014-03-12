//------------------------------------------------------------
//
// Implamantation of tke_form_updateDataBase class.
// Created by Roman Mulyk 16.01.2009
//
//------------------------------------------------------------

#include <QMessageBox>
#include <QTextCodec>
#include <QtSql>
#include <QVariant>
#include <QSettings>

#include "tke_form_updateDataBase.h"
//------------------------------------------------------------
tke_form_updateDataBase::tke_form_updateDataBase( QWidget * parent) 
	: QWidget(parent)
{
	ui.setupUi(this);
	
	//connect(ui.tableWidget_abonents, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(pushButton_openRahunok_clicked()));
	//connect(ui.pushButton_closeForm, SIGNAL(clicked()), this, SLOT(close()));
}
//------------------------------------------------------------

//------------------------------------------------------------
tke_form_updateDataBase::~tke_form_updateDataBase()
{
	
}
//------------------------------------------------------------
