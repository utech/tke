//------------------------------------------------------------
//
// Implamantation of tkeDialogFiltrBorjnykivOrganiz class.
// Created by Roman Mulyk 13.11.2007
//
//------------------------------------------------------------

#include "tke_dialog_filtr_borjnykiv_organiz.h"

//------------------------------------------------------------
tkeDialogFiltrBorjnykivOrganiz::tkeDialogFiltrBorjnykivOrganiz(QWidget *parent)
        : QDialog(parent)
{
	ui.setupUi(this);
	
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT id, Finans_name FROM finans_from ORDER BY Finans_name");
	int iter=0;
	while (query->next()){
		finasHash.insert(iter, query->value(0).toInt());
		ui.comboBox_finans_from->addItem(query->value(1).toString());
		iter ++;
	}
	delete query;
	//Connections
	connect(ui.pushButton_accept, SIGNAL(clicked ()), this, SLOT(tryAccept()));
	connect(ui.pushButton_reject, SIGNAL(clicked ()), this, SLOT(reject()));
}
//----Slots-------------------------------------------------
void tkeDialogFiltrBorjnykivOrganiz::tryAccept()
{
	if (ui.dateEditPochDate->date() > ui.dateEditKincDate->date()){
		QMessageBox::critical(
				this,
				"!!! Помилка !!!",
				"Початкова дата повинна бути меншою (рівною) від кінцевої.",
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
void tkeDialogFiltrBorjnykivOrganiz::setPochDate(QDate date)
{
	ui.dateEditPochDate->setDate(date);
}
//------------------------------------------------------------
void tkeDialogFiltrBorjnykivOrganiz::setKincDate(QDate date)
{
	ui.dateEditKincDate->setDate(date);
}
//------------------------------------------------------------
QDate tkeDialogFiltrBorjnykivOrganiz::pochDate()
{
	return ui.dateEditPochDate->date();
}
//------------------------------------------------------------
QDate tkeDialogFiltrBorjnykivOrganiz::kincDate()
{
	return ui.dateEditKincDate->date();
}
//------------------------------------------------------------
int tkeDialogFiltrBorjnykivOrganiz::curFinansFromId()
{
	return finasHash.value(ui.comboBox_finans_from->currentIndex());
}
//------------------------------------------------------------
bool tkeDialogFiltrBorjnykivOrganiz::neadMinSum()
{
	return ui.groupBox_borgSum->isChecked();
}
//------------------------------------------------------------
double tkeDialogFiltrBorjnykivOrganiz::minSum()
{
	return ui.doubleSpinBox_borgSum->value();
}
//------------------------------------------------------------
void tkeDialogFiltrBorjnykivOrganiz::setMinDate(QDate date)
{
	ui.dateEditPochDate->setMaximumDate(date);
	ui.dateEditKincDate->setMaximumDate(date);
}
//------------------------------------------------------------
void tkeDialogFiltrBorjnykivOrganiz::setMaxDate(QDate date)
{
	ui.dateEditPochDate->setMinimumDate(date);
	ui.dateEditKincDate->setMinimumDate(date);
}
//------------------------------------------------------------
tkeDialogFiltrBorjnykivOrganiz::~tkeDialogFiltrBorjnykivOrganiz()
{

}
//------------------------------------------------------------
