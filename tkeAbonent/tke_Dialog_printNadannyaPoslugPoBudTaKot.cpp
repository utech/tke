//------------------------------------------------------------
//
// Implamantation of tke_Dialog_printNadannyaPoslugPoBudTaKot class.
// Created by Roman Mulyk 12.09.2006
//
//------------------------------------------------------------

#include "tke_Dialog_printNadannyaPoslugPoBudTaKot.h"
#include <QSqlQuery>

//------------------------------------------------------------
tke_Dialog_printNadannyaPoslugPoBudTaKot::tke_Dialog_printNadannyaPoslugPoBudTaKot(QWidget *parent, QString title)
        : QDialog(parent)
{
    ui.setupUi(this);
	
	setWindowTitle(title);
	
	QSqlQuery query;
	query.exec("SELECT id, Vulycia, Nomer, Kot_num FROM kotelni ORDER BY Vulycia, Nomer");
	ui.comboBox_chooseKot->clear();
	ui.comboBox_chooseKot->addItem("Усі котельні", 0);
	while (query.next())
		ui.comboBox_chooseKot->addItem(query.value(1).toString()+" "+query.value(2).toString()+" №"+query.value(3).toString(), query.value(0));
	
	populateBudynkyComboBox();
	
	connect(ui.comboBox_chooseKot, SIGNAL(currentIndexChanged(int)), this, SLOT(populateBudynkyComboBox()));
	connect(ui.pushButton_accept, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui.pushButton_reject, SIGNAL(clicked()), this, SLOT(reject()));
}
//------------------------------------------------------------
void tke_Dialog_printNadannyaPoslugPoBudTaKot::populateBudynkyComboBox()
{
	int kId = kotelnyaId();
	
	QSqlQuery query;
	query.exec("SELECT id, Vulycia, Bud_num FROM budynky "+(kId!=0?QString("WHERE Kotelnya_ID=")+QString::number(kId):QString(""))+" ORDER BY Vulycia, Bud_num");
	ui.comboBox_chooseBud->clear();
	ui.comboBox_chooseBud->addItem((kId==0?"Усі будинки підприємства":"Усі будинки котельні"), 0);
	while (query.next())
		ui.comboBox_chooseBud->addItem(query.value(1).toString()+" "+query.value(2).toString(), query.value(0));
}
//------------------------------------------------------------
int tke_Dialog_printNadannyaPoslugPoBudTaKot::kotelnyaId()
{
	return ui.comboBox_chooseKot->itemData(ui.comboBox_chooseKot->currentIndex(), Qt::UserRole).toInt();
}
//------------------------------------------------------------
int tke_Dialog_printNadannyaPoslugPoBudTaKot::budynokId()
{
	return ui.comboBox_chooseBud->itemData(ui.comboBox_chooseBud->currentIndex(), Qt::UserRole).toInt();
}
//------------------------------------------------------------
tke_Dialog_printNadannyaPoslugPoBudTaKot::~tke_Dialog_printNadannyaPoslugPoBudTaKot()
{

}
//------------------------------------------------------------
