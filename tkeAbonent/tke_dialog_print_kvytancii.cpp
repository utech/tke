//------------------------------------------------------------
//
// Implamantation of tke_Dialog_narah class.
// Created by Roman Mulyk 12.09.2006
//
//------------------------------------------------------------

#include "tke_dialog_print_kvytancii.h"

//------------------------------------------------------------
tke_Dialog_print_kvytancii::tke_Dialog_print_kvytancii(QWidget *parent, QString title, short year, short month, bool clearItem)
        : QDialog(parent)
{
    ui.setupUi(this);
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT id, Vulycia & ' ' & Bud_num FROM budynky ORDER BY Vulycia & ' ' & Bud_num");
	if (clearItem)
		ui.comboBox_budynok->addItem("Усі будинки", 0);
	while(query->next())
		ui.comboBox_budynok->addItem(query->value(1).toString(), query->value(0).toInt());
	ui.comboBox_budynok->setCurrentIndex(0);
	setWindowTitle(title);
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint );
	ui.spinBox_year->setValue(year);
	ui.spinBox_month->setValue(month);
	
	delete query;
}
//------------------------------------------------------------
int tke_Dialog_print_kvytancii::get_Bud_id()
{
	return ui.comboBox_budynok->itemData(ui.comboBox_budynok->currentIndex()).toInt();
}
//------------------------------------------------------------
int tke_Dialog_print_kvytancii::get_Year()
{
	return ui.spinBox_year->value();
}
//------------------------------------------------------------
int tke_Dialog_print_kvytancii::get_Month()
{
	return ui.spinBox_month->value();
}
//------------------------------------------------------------
void tke_Dialog_print_kvytancii::setBudHidden(bool hide)
{
	ui.groupBox_budynok->setHidden(hide);
	adjustSize();
}
//------------------------------------------------------------
void tke_Dialog_print_kvytancii::setDateHidden(bool hide)
{
	ui.groupBox_date->setHidden(hide);
	adjustSize();
}
//------------------------------------------------------------
tke_Dialog_print_kvytancii::~tke_Dialog_print_kvytancii()
{

}
//------------------------------------------------------------
