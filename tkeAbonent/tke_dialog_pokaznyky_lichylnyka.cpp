//------------------------------------------------------------
//
// Implamantation of tke_Dialog_pokaznyky_lichylnyka.
// Created by Roman Mulyk 18.06.2007
//
//------------------------------------------------------------

#include "tke_dialog_pokaznyky_lichylnyka.h"

//------------------------------------------------------------
tke_Dialog_pokaznyky_lichylnyka::tke_Dialog_pokaznyky_lichylnyka(QWidget *parent)
        : QDialog(parent)
{
    ui.setupUi(this);
		//init
	setMaximumWidth(width());
	setMinimumWidth(width());
	setMaximumHeight(height());
	setMinimumHeight(height());
	ui.doubleSpinBox_poch_lich->selectAll();
	//connectors
	connect(ui.doubleSpinBox_poch_lich, SIGNAL(valueChanged(double)), this, SLOT(valueEdited())); // Показник лічильника редаговано
	connect(ui.doubleSpinBox_kinc_lich, SIGNAL(valueChanged(double)), this, SLOT(valueEdited())); // Показник лічильника редаговано
	connect(ui.pushButton_accept, SIGNAL(clicked ()), this, SLOT(accept())); // Користувач прийняв значення
	connect(ui.pushButton_reject, SIGNAL(clicked ()), this, SLOT(reject())); // Користувач відхилив значення
}
//------------------------------------------------------------
double tke_Dialog_pokaznyky_lichylnyka::pochZnachLich()
{
	return ui.doubleSpinBox_poch_lich->value();
}
//------------------------------------------------------------
double tke_Dialog_pokaznyky_lichylnyka::kincZnachLich()
{
	return ui.doubleSpinBox_kinc_lich->value();
}
//-------------------slots-----------------------------------
void tke_Dialog_pokaznyky_lichylnyka::setPochZnachLich(double znachennya)
{
	ui.doubleSpinBox_poch_lich->setValue(znachennya);
}
//------------------------------------------------------------
void tke_Dialog_pokaznyky_lichylnyka::setKincZnachLich(double znachennya)
{
	ui.doubleSpinBox_kinc_lich->setValue(znachennya);
}
//------------------------------------------------------------
void tke_Dialog_pokaznyky_lichylnyka::valueEdited()
{
	if (ui.doubleSpinBox_poch_lich->value() <= ui.doubleSpinBox_kinc_lich->value())
		ui.pushButton_accept->setEnabled(true);
	else
		ui.pushButton_accept->setEnabled(false);
}
//------------------------------------------------------------
tke_Dialog_pokaznyky_lichylnyka::~tke_Dialog_pokaznyky_lichylnyka()
{

}
//------------------------------------------------------------
