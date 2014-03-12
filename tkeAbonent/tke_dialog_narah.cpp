//------------------------------------------------------------
//
// Implamantation of tke_Dialog_narah class.
// Created by Roman Mulyk 12.09.2006
//
//------------------------------------------------------------

#include "tke_dialog_narah.h"

//------------------------------------------------------------
tke_Dialog_narah::tke_Dialog_narah(QWidget *parent, QString title, short year, short month)
        : QDialog(parent)
{
    ui.setupUi(this);

	date = QDate::currentDate();
	if (date.month() == 12)
		ui.spinBox_year->setMaximum(date.year()+1);
	else
		ui.spinBox_year->setMaximum(date.year());
	ui.groupBox->parentWidget()->setWindowTitle( title);
	ui.groupBox->parentWidget()->setWindowFlags ( ui.groupBox->parentWidget()->windowFlags() | Qt::WindowStaysOnTopHint );
	ui.spinBox_year->setValue(year);
	ui.spinBox_month->setValue(month);
}
//------------------------------------------------------------
int tke_Dialog_narah::get_Month()
{
	return ui.spinBox_month->value();
}
//------------------------------------------------------------
int tke_Dialog_narah::get_Year()
{
	return ui.spinBox_year->value();
}
//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
tke_Dialog_narah::~tke_Dialog_narah()
{

}
//------------------------------------------------------------
