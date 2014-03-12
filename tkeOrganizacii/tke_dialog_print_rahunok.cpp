//------------------------------------------------------------
//
// Implamantation of tkeDialogPrintRahunok class.
// Created by Roman Mulyk 26.10.2007
//
//------------------------------------------------------------

#include "tke_dialog_print_rahunok.h"

//------------------------------------------------------------
tkeDialogPrintRahunok::tkeDialogPrintRahunok(short year, short month,QWidget *parent)
        : QDialog(parent)
{
	ui.setupUi(this);
	
	ui.spinBox_month->setValue(month);
	ui.spinBox_year->setValue(year);
	
	ui.doubleSpinBox_suma->setEnabled ( false );
	ui.spinBox_month->setEnabled ( true );
	ui.spinBox_year->setEnabled ( true );
	
	//Connections
	connect(ui.pushButton_accept, SIGNAL(clicked ()), this, SLOT(accept()));
	connect(ui.pushButton_reject, SIGNAL(clicked ()), this, SLOT(reject()));
	connect(ui.comboBox_rahunok_type, SIGNAL(currentIndexChanged(int)), this, SLOT(rahTypeChanged(int)));
}
//----Slots-------------------------------------------------
void tkeDialogPrintRahunok::rahTypeChanged(int index)
{
	if (index==1){
		ui.doubleSpinBox_suma->setEnabled ( true );
		ui.spinBox_month->setEnabled ( false );
		ui.spinBox_year->setEnabled ( false );
	}
	else{
		ui.doubleSpinBox_suma->setEnabled ( false );
		ui.spinBox_month->setEnabled ( true );
		ui.spinBox_year->setEnabled ( true );
	}
}
//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
//----functions----------------------------------------------
double tkeDialogPrintRahunok::suma()
{
	return ui.doubleSpinBox_suma->value();
}
//------------------------------------------------------------
int tkeDialogPrintRahunok::rahunokType()
{
	return ui.comboBox_rahunok_type->currentIndex();
}
//------------------------------------------------------------
int tkeDialogPrintRahunok::year()
{
	return ui.spinBox_year->value();
}
//------------------------------------------------------------
int tkeDialogPrintRahunok::month()
{
	return ui.spinBox_month->value();
}
//------------------------------------------------------------
//------------------------------------------------------------
tkeDialogPrintRahunok::~tkeDialogPrintRahunok()
{

}
//------------------------------------------------------------
