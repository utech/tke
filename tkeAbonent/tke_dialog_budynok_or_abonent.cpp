//------------------------------------------------------------
//
// Implamantation of tke_Dialog_narah class.
// Created by Roman Mulyk 10.11.2010
//
//------------------------------------------------------------

#include "tke_dialog_budynok_or_abonent.h"

//------------------------------------------------------------
tke_dialog_budynok_or_abonent::tke_dialog_budynok_or_abonent(QString title, QDate dt, bool clearBudItem, QWidget *parent)
        : QDialog(parent)
{
    ui.setupUi(this);
	QSqlQuery query;
	query.exec("SELECT id, Vulycia & ' ' & Bud_num FROM budynky ORDER BY Vulycia & ' ' & Bud_num");
	if (clearBudItem)
		ui.comboBox_budynok->addItem("Усі будинки", 0);
	while(query.next())
		ui.comboBox_budynok->addItem(query.value(1).toString(), query.value(0).toInt());
	ui.comboBox_budynok->setCurrentIndex(0);
	setWindowTitle(title);
	ui.dateEdit_dateFilter->setDate(dt);
	ui.spinBox_rahunok->setValue(1);
	
	filterTypeChanged();
	populatePib();
	connect(ui.spinBox_rahunok, SIGNAL(valueChanged(int)), this, SLOT(populatePib()));
}
//------------------------------------------------------------
int tke_dialog_budynok_or_abonent::budId()
{
	if (ui.groupBox_chooseBudynok->isChecked())
		return ui.comboBox_budynok->itemData(ui.comboBox_budynok->currentIndex()).toInt();
	return 0;
}
//------------------------------------------------------------
int tke_dialog_budynok_or_abonent::rahunok()
{
	if (ui.groupBox_chooseRahunok->isChecked())
		return ui.spinBox_rahunok->value();
	return 0;
}
//------------------------------------------------------------
QDate tke_dialog_budynok_or_abonent::date()
{
	return ui.dateEdit_dateFilter->date();
}
//------------------------------------------------------------
void tke_dialog_budynok_or_abonent::filterTypeChanged()
{
	QGroupBox *bg = qobject_cast<QGroupBox *>(sender());
	
	disconnect(ui.groupBox_chooseBudynok, SIGNAL(clicked(bool)), this, SLOT(filterTypeChanged()));
	disconnect(ui.groupBox_chooseRahunok, SIGNAL(clicked(bool)), this, SLOT(filterTypeChanged()));
	
	bool isAbonFilter = (bg==ui.groupBox_chooseRahunok ? true : false);
	ui.groupBox_chooseBudynok->setChecked(!isAbonFilter);
	ui.groupBox_chooseRahunok->setChecked(isAbonFilter);
	
	connect(ui.groupBox_chooseBudynok, SIGNAL(clicked(bool)), this, SLOT(filterTypeChanged()));
	connect(ui.groupBox_chooseRahunok, SIGNAL(clicked(bool)), this, SLOT(filterTypeChanged()));
}
//------------------------------------------------------------
void tke_dialog_budynok_or_abonent::populatePib()
{
	QSqlQuery query;
	query.exec("SELECT Prizv, Imya, Batk FROM abonent WHERE Rahunok="+QString::number(ui.spinBox_rahunok->value()));
	if (query.next())
		ui.label_pib->setText("<span style=\" color:#005500;\">ПІБ: "+query.value(0).toString()+" "+
								query.value(1).toString()+" "+query.value(2).toString()+"</span>");
	else
		ui.label_pib->setText("<span style=\" color:#aa0000;\">Невірний рахунок</span>");
}
//------------------------------------------------------------
tke_dialog_budynok_or_abonent::~tke_dialog_budynok_or_abonent()
{
	
}
//------------------------------------------------------------
