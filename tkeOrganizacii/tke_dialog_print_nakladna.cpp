//------------------------------------------------------------
//
// Implamantation of tke_dialog_print_nakladna class.
// Created by Roman Mulyk 26.10.2007
//
//------------------------------------------------------------

#include "tke_dialog_print_nakladna.h"

//------------------------------------------------------------
tke_dialog_print_nakladna::tke_dialog_print_nakladna(short year, short month,QWidget *parent)
        : QDialog(parent)
{
	ui.setupUi(this);
	
	ui.spinBox_month->setValue(month);
	ui.spinBox_year->setValue(year);
	
	calcSum();
	
	//Connections
	connect(ui.pushButton_accept, SIGNAL(clicked ()), this, SLOT(accept()));
	connect(ui.pushButton_reject, SIGNAL(clicked ()), this, SLOT(reject()));
	connect(ui.doubleSpinBox_sumaCO, SIGNAL(valueChanged(double)), this, SLOT(calcSum()));
	connect(ui.doubleSpinBox_sumaGV, SIGNAL(valueChanged(double)), this, SLOT(calcSum()));
	
	connect(ui.spinBox_year, SIGNAL(valueChanged(int)), this, SLOT(readCoGvSum()));
	connect(ui.spinBox_month, SIGNAL(valueChanged(int)), this, SLOT(readCoGvSum()));
}
//----Slots-------------------------------------------------
double tke_dialog_print_nakladna::sumaCO()
{
	return ui.doubleSpinBox_sumaCO->value();
}
//------------------------------------------------------------
double tke_dialog_print_nakladna::sumaGV()
{
	return ui.doubleSpinBox_sumaGV->value();
}
//------------------------------------------------------------
int tke_dialog_print_nakladna::year()
{
	return ui.spinBox_year->value();
}
//------------------------------------------------------------
int tke_dialog_print_nakladna::month()
{
	return ui.spinBox_month->value();
}
//------------------------------------------------------------
void tke_dialog_print_nakladna::calcSum()
{
	double sum = ui.doubleSpinBox_sumaCO->value()+ui.doubleSpinBox_sumaGV->value();
	ui.label_sumInfo->setText("Заг.сума: "+QString::number(sum,'f',2)+"грн.");
	if (sum > 0.009)
		ui.pushButton_accept->setEnabled(true);
	else
		ui.pushButton_accept->setEnabled(false);
}
//------------------------------------------------------------
void tke_dialog_print_nakladna::readCoGvSum()
{
	//QSqlQuery query;
	//query.exec("SELECT * FROM narahuvannya WHERE");
}
//------------------------------------------------------------
tke_dialog_print_nakladna::~tke_dialog_print_nakladna()
{

}
//------------------------------------------------------------
