//------------------------------------------------------------
//
// Implamantation of tkeDialogDateInterval class.
// Created by Roman Mulyk 21.11.2007
//
//------------------------------------------------------------

#include "tke_dialog_date_interval.h"

//------------------------------------------------------------
tkeDialogDateInterval::tkeDialogDateInterval(QWidget *parent)
        : QDialog(parent)
{
	ui.setupUi(this);

	//Connections
	connect(ui.pushButton_accept, SIGNAL(clicked ()), this, SLOT(tryAccept()));
	connect(ui.pushButton_reject, SIGNAL(clicked ()), this, SLOT(reject()));
}
//----Slots-------------------------------------------------
void tkeDialogDateInterval::tryAccept()
{
	if (ui.dateEditPochDate->date() >= ui.dateEditKincDate->date()){
		QMessageBox::critical(
				this,
				"!!! Помилка !!!",
				"Початкова дата повинна бути меншою від кінцевої.",
				QMessageBox::Ok,
				QMessageBox::NoButton,
				QMessageBox::NoButton);
	}
	else{
		accept();
	}
}
//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
//----functions----------------------------------------------
void tkeDialogDateInterval::setPochDate(QDate date)
{
	ui.dateEditPochDate->setDate(date);
}
//------------------------------------------------------------
void tkeDialogDateInterval::setKincDate(QDate date)
{
	ui.dateEditKincDate->setDate(date);
}
//------------------------------------------------------------
QDate tkeDialogDateInterval::pochDate()
{
	return ui.dateEditPochDate->date();
}
//------------------------------------------------------------
QDate tkeDialogDateInterval::kincDate()
{
	return ui.dateEditKincDate->date();
}
//------------------------------------------------------------
void tkeDialogDateInterval::setMinDate(QDate date)
{
	ui.dateEditPochDate->setMaximumDate(date);
	ui.dateEditKincDate->setMaximumDate(date);
}
//------------------------------------------------------------
void tkeDialogDateInterval::setMaxDate(QDate date)
{
	ui.dateEditPochDate->setMinimumDate(date);
	ui.dateEditKincDate->setMinimumDate(date);
}
//------------------------------------------------------------
tkeDialogDateInterval::~tkeDialogDateInterval()
{

}
//------------------------------------------------------------
