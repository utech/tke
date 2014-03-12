//------------------------------------------------------------
//
// Implamantation of tke_Dialog_saveDataBase class.
// Created by Roman Mulyk 17.04.2008
//
//------------------------------------------------------------

#include "tke_Dialog_saveDataBase.h"
#include "../tkeOrganizacii/udiff_func.h"

//------------------------------------------------------------
tke_Dialog_saveDataBase::tke_Dialog_saveDataBase(QWidget *parent)
        : QDialog(parent)
{
    ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	
	QApplication::setQuitOnLastWindowClosed(true);
	
	iter = 6;
	showTimeInfo();
	
	timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTimeInfo()));
    timer->start(1000); 
	
	connect(ui.pushButton_accept, SIGNAL(clicked ()), this, SLOT(dAccept()));
	connect(ui.pushButton_reject, SIGNAL(clicked ()), this, SLOT(dReject()));
}
//------------------------------------------------------------
void tke_Dialog_saveDataBase::showTimeInfo()
{
	iter--;
	if (iter==0)
		reject();
	ui.label_timer->setText("Автозакриття через -- "+ QString::number(iter) +" -- секунд.");
}
//------------------------------------------------------------
void tke_Dialog_saveDataBase::dAccept()
{
	timer->stop();
	delete timer;
	accept();
	saveDataBase();
}
//------------------------------------------------------------
void tke_Dialog_saveDataBase::dReject()
{
	timer->stop();
	delete timer;
	reject();
}
tke_Dialog_saveDataBase::~tke_Dialog_saveDataBase()
{
	//QCoreApplication::quit();
}
//------------------------------------------------------------
