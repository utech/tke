//------------------------------------------------------------
//
// Implamantation of tke_Dialog_narah class.
// Created by Roman Mulyk 12.09.2006
//
//------------------------------------------------------------

#include "tke_dialog_filtr_borjnykiv.h"

//------------------------------------------------------------
tke_Dialog_filtr_borjnykiv::tke_Dialog_filtr_borjnykiv(QString lblBorg, QString windowTitle, QWidget *parent)
        : QDialog(parent)
{
    ui.setupUi(this);
    ui.label_borg->setText(lblBorg);
    setWindowTitle(windowTitle);
    QSqlQuery *query = new QSqlQuery();
	
	query->exec("SELECT id, Vulycia & ' ' & Bud_num FROM budynky ORDER BY Vulycia & ' ' & Bud_num");
	ui.comboBox_budynok->clear();
	budList.clear();
	budList << 0;
	ui.comboBox_budynok->addItem("Усі будинки");
	while(query->next()){
		ui.comboBox_budynok->addItem(query->value(1).toString());
		budList << query->value(0).toInt();
	}
	setWindowFlags ( windowFlags() | Qt::WindowStaysOnTopHint );
	QDate date;

	date = QDate::currentDate();
	ui.dateEdit_poch->setMaximumDate(date);
	ui.dateEdit_kinc->setDate(date);
	ui.dateEdit_kinc->setMaximumDate(date);
	
	date.setYMD(2003, 1, 1);
	ui.dateEdit_poch->setMinimumDate(date);
	ui.dateEdit_poch->setDate(date);
	ui.dateEdit_kinc->setMinimumDate(date);
	
	delete query;
}
//------------------------------------------------------------
int tke_Dialog_filtr_borjnykiv::get_Bud_id()
{
	return budList.at( ui.comboBox_budynok->currentIndex() );
}
//------------------------------------------------------------
QDate tke_Dialog_filtr_borjnykiv::getPochDate()
{
	return ui.dateEdit_poch->date();
}
//------------------------------------------------------------
QDate tke_Dialog_filtr_borjnykiv::getKincDate()
{
	return ui.dateEdit_kinc->date();
}
//------------------------------------------------------------
void tke_Dialog_filtr_borjnykiv::setPochDate(QDate date)
{
	ui.dateEdit_poch->setDate(date);
	ui.dateEdit_poch->setMinimumDate(date);
	ui.dateEdit_kinc->setMinimumDate(date);
}
//------------------------------------------------------------
void tke_Dialog_filtr_borjnykiv::setKincDate(QDate date)
{
	ui.dateEdit_kinc->setDate(date);
	ui.dateEdit_poch->setMaximumDate(date);
	ui.dateEdit_kinc->setMaximumDate(date);
}
//------------------------------------------------------------
double tke_Dialog_filtr_borjnykiv::minBorg()
{
	return ui.doubleSpinBox_borg->value();
}
//------------------------------------------------------------
unsigned char tke_Dialog_filtr_borjnykiv::abonType()
{
	return ui.comboBox_printAbonentsTypes->currentIndex();
}
//------------------------------------------------------------
void tke_Dialog_filtr_borjnykiv::setAbonType(unsigned char type)
{
	ui.comboBox_printAbonentsTypes->setCurrentIndex(type);
}
//------------------------------------------------------------
tke_Dialog_filtr_borjnykiv::~tke_Dialog_filtr_borjnykiv()
{

}
//------------------------------------------------------------
