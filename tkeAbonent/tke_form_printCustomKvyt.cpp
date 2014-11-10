//--------------------utech--------------------utech--------------------utech--------------------
//
// Файл виконання класу вікна налаштування програми
//
// Створений програмістами Компанії "АбонПро" 13.09.2010р.
// У класі використовуються компоненти бібліотеки QT4.x
//
//--------------------utech--------------------utech--------------------utech--------------------
#include "tke_form_printCustomKvyt.H"

#include <QtDebug>
#include <UStandardDelegate>
#include <USqlAccessible>
#include <UMath>

#include "uprint_docs.h"

#define RAHUNOK_COL 0
#define PIB_COL 1
#define ADRESA_COL 2
#define KOTELNYA_COL 3
#define STOYAK_COL 4
#define SUMA_COL 5
#define ISPRINTED_COL 6

#define PRINTED_GB_COLOR "#d6f7a8"
#define NOT_PRINTED_GB_COLOR "#ffebe3"

#define MAX_ABON_COUNT_TO_PRINT 250

//--------------------utech--------------------utech--------------------utech--------------------
tke_form_printCustomKvyt::tke_form_printCustomKvyt(QDate cDate, QWidget *parent)
        : QWidget(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	
	ui.tableWidget_abonenty->addAction(ui.action_printSelectedKvyt);
	ui.tableWidget_abonenty->addAction(ui.action_setSelectedAsPrinted);
	ui.tableWidget_abonenty->addAction(ui.action_setSelectedAsNotPrinted);
	ui.tableWidget_abonenty->setContextMenuPolicy(Qt::ActionsContextMenu);
	
	initDovidnyky();
	
	ui.radioButton_stanFilterAll->setChecked(true);
	ui.dateEdit_cDate->setDate(cDate);
	ui.tableWidget_abonenty->setColumnWidth(RAHUNOK_COL, 50);
	ui.tableWidget_abonenty->setColumnWidth(PIB_COL, 130);
	ui.tableWidget_abonenty->setColumnWidth(ADRESA_COL, 230);
	ui.tableWidget_abonenty->setColumnWidth(KOTELNYA_COL, 180);
	ui.tableWidget_abonenty->setColumnWidth(STOYAK_COL, 50);
	ui.tableWidget_abonenty->setColumnWidth(SUMA_COL, 60);
	ui.tableWidget_abonenty->setColumnWidth(ISPRINTED_COL, 80);
	
	ui.tableWidget_abonenty->setItemDelegateForColumn(STOYAK_COL, new UCheckBoxDelegate(ui.tableWidget_abonenty, "Ст.", "-",""));
	ui.tableWidget_abonenty->setItemDelegateForColumn(ISPRINTED_COL, new UCheckBoxDelegate(ui.tableWidget_abonenty, "Надрук.", "-",""));
	
	markBudynkyFilter();
	tableWidget_abonenty_itemSelectionChanged();
	
	connect(ui.checkBox_stoyakFilter, SIGNAL(clicked(bool)), this, SLOT(markStoyakFilter()));
	connect(ui.groupBox_kotelniFilter, SIGNAL(clicked(bool)), this, SLOT(markKotelniFilter()));
	connect(ui.groupBox_filterBudynkiv, SIGNAL(clicked(bool)), this, SLOT(markBudynkyFilter()));
	
	connect(ui.comboBox_budynkyFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(populateAbonentyTable()));
	connect(ui.comboBox_kotelniFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(populateAbonentyTable()));
	
	connect(ui.radioButton_stanFilterAll, SIGNAL(toggled(bool)), this, SLOT(populateAbonentyTable()));
	connect(ui.radioButton_stanFilterNotPrinted, SIGNAL(toggled(bool)), this, SLOT(populateAbonentyTable()));
	connect(ui.radioButton_stanFilterPrinted, SIGNAL(toggled(bool)), this, SLOT(populateAbonentyTable()));
	connect(ui.lineEdit_adrFilter, SIGNAL(textChanged(const QString &)), this, SLOT(populateAbonentyTable()));
	connect(ui.dateEdit_cDate, SIGNAL(dateChanged(const QDate &)), this, SLOT(populateAbonentyTable()));
	
	connect(ui.toolButton_clearFilter, SIGNAL(clicked()), this, SLOT(clearPryvatBudFilter()));
	connect(ui.toolButton_clearPrintedStates, SIGNAL(clicked()), this, SLOT(clearPrintedState()));
	connect(ui.toolButton_print, SIGNAL(clicked()), this, SLOT(printSelected()));
	
	connect(ui.action_printSelectedKvyt, SIGNAL(activated()), this, SLOT(printSelected()()));
	connect(ui.action_setSelectedAsPrinted, SIGNAL(activated()), this, SLOT(action_setSelectedAsPrinted_activated()));
	connect(ui.action_setSelectedAsNotPrinted, SIGNAL(activated()), this, SLOT(action_setSelectedAsNotPrinted_activated()));
	
	connect(ui.tableWidget_abonenty->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(setTableWidgetSorting(int)));
	connect(ui.tableWidget_abonenty, SIGNAL(itemSelectionChanged()), this, SLOT(tableWidget_abonenty_itemSelectionChanged()));
}
//--------------------utech--------------------utech--------------------utech--------------------
void tke_form_printCustomKvyt::initDovidnyky()
{
	ui.comboBox_budynkyFilter->blockSignals(true);
	ui.comboBox_kotelniFilter->blockSignals(true);
	
	QSqlQuery query;
	UIntStringList intStrList;
	
	ui.comboBox_budynkyFilter->clear();
	query.exec("SELECT id, Vulycia, Bud_num FROM budynky ORDER BY Vulycia, Bud_num");
	while (query.next())
		ui.comboBox_budynkyFilter->addItem(query.value(1).toString()+" "+query.value(2).toString(), query.value(0));
	
	ui.comboBox_kotelniFilter->clear();
	intStrList.clear();
	intStrList.insert(0,"");
	query.exec("SELECT id, Vulycia, Nomer FROM kotelni ORDER BY Vulycia, Nomer");
	while (query.next()){
		ui.comboBox_kotelniFilter->addItem(query.value(1).toString()+" "+query.value(2).toString(), query.value(0));
		intStrList.insert(query.value(0).toInt(),query.value(1).toString()+" "+query.value(2).toString());
	}
	ui.tableWidget_abonenty->setItemDelegateForColumn(KOTELNYA_COL, 
								new UIntStringListComboDelegate(ui.tableWidget_abonenty, intStrList));
	
	ui.comboBox_budynkyFilter->blockSignals(false);
	ui.comboBox_kotelniFilter->blockSignals(false);
}
//--------------------utech--------------------utech--------------------utech--------------------
void tke_form_printCustomKvyt::populateAbonentyTable()
{
	QSqlQuery query;
	QTableWidgetItem *item;
	QString filterStr;
	
	if (ui.checkBox_stoyakFilter->isChecked())
		filterStr += " and abonent.NarahStoyak=true";
	else if (ui.groupBox_kotelniFilter->isChecked()){
		filterStr += " and kotelni.id="+sqlStr(ui.comboBox_kotelniFilter
						->itemData(ui.comboBox_kotelniFilter->currentIndex(), Qt::UserRole).toInt());
	}
	else if (ui.groupBox_filterBudynkiv->isChecked()){
		filterStr += " and budynky.id="+sqlStr(ui.comboBox_budynkyFilter
						->itemData(ui.comboBox_budynkyFilter->currentIndex(), Qt::UserRole).toInt());
	}
	
	if (ui.lineEdit_adrFilter->text().size())
		filterStr += " and budynky.Vulycia & ' ' & budynky.Bud_num LIKE '"+ui.lineEdit_adrFilter->text()+"%' ";
	if (ui.radioButton_stanFilterNotPrinted->isChecked())
		filterStr += " and sl_printedKvyt.id is null";
	else if (ui.radioButton_stanFilterPrinted->isChecked())
		filterStr += " and sl_printedKvyt.id > 0";
	
	query.exec("SELECT count(*) \
				FROM ((abonent \
				LEFT JOIN (SELECT id, Rahunok FROM sl_printedKvyt \
							WHERE year(CDate)="+sqlStr(ui.dateEdit_cDate->date().year())+" \
								and month(CDate)="+sqlStr(ui.dateEdit_cDate->date().month())+") AS sl_printedKvyt \
					ON abonent.Rahunok=sl_printedKvyt.Rahunok ) \
				LEFT JOIN budynky ON budynky.id=abonent.Budynok_ID) \
				LEFT JOIN kotelni ON kotelni.id=budynky.Kotelnya_ID \
				WHERE 1 "+filterStr);
	query.next();
	int rowCount = query.value(0).toInt();
	
	query.exec("SELECT abonent.Rahunok, abonent.Prizv, abonent.Imya, abonent.Batk, \
						budynky.Vulycia, budynky.Bud_num, abonent.Kvartyra, abonent.SufiksKvart, kotelni.id, abonent.NarahStoyak, \
						sl_printedKvyt.id \
				FROM ((abonent \
				LEFT JOIN (SELECT id, Rahunok FROM sl_printedKvyt \
							WHERE year(CDate)="+sqlStr(ui.dateEdit_cDate->date().year())+" \
								and month(CDate)="+sqlStr(ui.dateEdit_cDate->date().month())+") AS sl_printedKvyt \
					ON abonent.Rahunok=sl_printedKvyt.Rahunok ) \
				LEFT JOIN budynky ON budynky.id=abonent.Budynok_ID) \
				LEFT JOIN kotelni ON kotelni.id=budynky.Kotelnya_ID \
				WHERE 1 "+filterStr);
	ui.tableWidget_abonenty->setRowCount(rowCount);
	QColor rowBgColor;
	for (int row=0; row<rowCount && query.next(); row++){
		if (query.value(10).toBool())
			rowBgColor.setNamedColor(PRINTED_GB_COLOR);
		else
			rowBgColor.setNamedColor(NOT_PRINTED_GB_COLOR);
		
		
		//RAHUNOK_COL
		item = new QTableWidgetItem();
		item->setBackground(QBrush(rowBgColor));
		item->setData(Qt::EditRole, query.value(0).toInt());
		item->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
		ui.tableWidget_abonenty->setItem(row, RAHUNOK_COL, item);
		
		//PIB_COL
		item = new QTableWidgetItem();
		item->setBackground(QBrush(rowBgColor));
		item->setData(Qt::EditRole, uPrizvIB(query.value(1).toString(),query.value(2).toString(),query.value(3).toString()));
		item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
		ui.tableWidget_abonenty->setItem(row, PIB_COL, item);
		
		//ADRESA_COL
		item = new QTableWidgetItem();
		item->setBackground(QBrush(rowBgColor));
		item->setData(Qt::EditRole, query.value(4).toString()+" "+query.value(5).toString()+", кв."
									+query.value(6).toString()+query.value(7).toString());
		item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
		ui.tableWidget_abonenty->setItem(row, ADRESA_COL, item);
		
		//KOTELNYA_COL
		item = new QTableWidgetItem();
		item->setBackground(QBrush(rowBgColor));
		item->setData(Qt::EditRole, query.value(8));
		item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
		ui.tableWidget_abonenty->setItem(row, KOTELNYA_COL, item);
		
		//STOYAK_COL
		item = new QTableWidgetItem();
		item->setBackground(QBrush(rowBgColor));
		item->setData(Qt::EditRole, query.value(9).toBool());
		item->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
		ui.tableWidget_abonenty->setItem(row, STOYAK_COL, item);
		
		//SUMA_COL
		item = new QTableWidgetItem();
		item->setBackground(QBrush(rowBgColor));
		item->setData(Qt::EditRole, "-");
		item->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
		ui.tableWidget_abonenty->setItem(row, SUMA_COL, item);
		
		//ISPRINTED_COL
		item = new QTableWidgetItem();
		item->setBackground(QBrush(rowBgColor));
		item->setData(Qt::EditRole, query.value(10).toBool());
		item->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
		ui.tableWidget_abonenty->setItem(row, ISPRINTED_COL, item);
	}
	
	ui.tableWidget_abonenty->resizeRowsToContents();
	twSortOrderAsc = true;
}
//--------------------utech--------------------utech--------------------utech--------------------
void tke_form_printCustomKvyt::clearPryvatBudFilter()
{
	ui.lineEdit_adrFilter->clear();
	populateAbonentyTable();
}
//--------------------utech--------------------utech--------------------utech--------------------
void tke_form_printCustomKvyt::clearPrintedState()
{
	if ( !ui.tableWidget_abonenty->selectionModel()->hasSelection()) {
		QMessageBox::information(0, "Порада", "Для того, щоб позначити квитанції ненадруковними,\nпозначте необхідних абонентів");
		return;
	}
	QList<int>  rahList;
	QModelIndexList selList = ui.tableWidget_abonenty->selectionModel()->selectedRows(RAHUNOK_COL);
	bool doneOk = true;
	foreach (QModelIndex ind, selList)
		doneOk = doneOk && tke_form_printCustomKvyt::setKvytNotPrinted(ind.data().toInt(), ui.dateEdit_cDate->date());
	
	populateAbonentyTable();
	
	if (doneOk)
		QMessageBox::information(0, "Завершено виконання", "Обрані квитанції успішно позначено ненадрукованими");
	else
		QMessageBox::critical(0, "Помилка", "Усі або кілька обраних квитанцій не позначено ненадрукованими");
}
//--------------------utech--------------------utech--------------------utech--------------------
void tke_form_printCustomKvyt::printSelected()
{
	if ( !ui.tableWidget_abonenty->selectionModel()->hasSelection()) {
		QMessageBox::information(0, "Порада", "Для того, щоб друкувати квитанції,\nпозначте необхідних абонентів");
		return;
	}
	
	QModelIndexList selList = ui.tableWidget_abonenty->selectionModel()->selectedRows(RAHUNOK_COL);
	if (selList.size() > MAX_ABON_COUNT_TO_PRINT){
		QMessageBox::information(0, "Перевищення кількості", "Ви обрали для друку квитанцій "+QString::number(selList.size())+" абоентів,\n"
								"а максимально можлива кількість - "+QString::number(MAX_ABON_COUNT_TO_PRINT)+".\n"
								"Порада! При можливості, друкуйте квитанції групами в кількосі до "+QString::number(MAX_ABON_COUNT_TO_PRINT)+" абонентів.");
		return;
	}
	QList<int>  rahList;
	foreach (QModelIndex ind, selList)
		rahList << ind.data().toInt();
	
	UPrintDocs *printDocs = new UPrintDocs();
	printDocs->print_kvytancii(ui.dateEdit_cDate->date().month(), ui.dateEdit_cDate->date().year(), false, 0, rahList);
	connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
	delete printDocs;
	
	foreach (QModelIndex ind, selList)
		tke_form_printCustomKvyt::setKvytPrinted(ind.data().toInt(), ui.dateEdit_cDate->date());
	
	populateAbonentyTable();
}
//--------------------utech--------------------utech--------------------utech--------------------
void tke_form_printCustomKvyt::setTableWidgetSorting(int clickedColumn)
{
	Qt::SortOrder order;
	if (twSortOrderAsc)
		order = Qt::AscendingOrder;
	else
		order = Qt::DescendingOrder;
	twSortOrderAsc = (!twSortOrderAsc);
	ui.tableWidget_abonenty->sortItems(clickedColumn, order);
}
//--------------------utech--------------------utech--------------------utech--------------------
void tke_form_printCustomKvyt::markStoyakFilter()
{
	ui.checkBox_stoyakFilter->blockSignals(true);
	ui.groupBox_kotelniFilter->blockSignals(true);
	ui.groupBox_filterBudynkiv->blockSignals(true);
	
	ui.checkBox_stoyakFilter->setChecked(true);
	ui.groupBox_kotelniFilter->setChecked(false);
	ui.groupBox_filterBudynkiv->setChecked(false);
	
	ui.checkBox_stoyakFilter->blockSignals(false);
	ui.groupBox_kotelniFilter->blockSignals(false);
	ui.groupBox_filterBudynkiv->blockSignals(false);
	
	populateAbonentyTable();
}
//--------------------utech--------------------utech--------------------utech--------------------
void tke_form_printCustomKvyt::markKotelniFilter()
{
	ui.checkBox_stoyakFilter->blockSignals(true);
	ui.groupBox_kotelniFilter->blockSignals(true);
	ui.groupBox_filterBudynkiv->blockSignals(true);
	
	ui.checkBox_stoyakFilter->setChecked(false);
	ui.groupBox_kotelniFilter->setChecked(true);
	ui.groupBox_filterBudynkiv->setChecked(false);
	
	ui.checkBox_stoyakFilter->blockSignals(false);
	ui.groupBox_kotelniFilter->blockSignals(false);
	ui.groupBox_filterBudynkiv->blockSignals(false);
	
	populateAbonentyTable();
}
//--------------------utech--------------------utech--------------------utech--------------------
void tke_form_printCustomKvyt::markBudynkyFilter()
{
	ui.checkBox_stoyakFilter->blockSignals(true);
	ui.groupBox_kotelniFilter->blockSignals(true);
	ui.groupBox_filterBudynkiv->blockSignals(true);
	
	ui.checkBox_stoyakFilter->setChecked(false);
	ui.groupBox_kotelniFilter->setChecked(false);
	ui.groupBox_filterBudynkiv->setChecked(true);
	
	ui.checkBox_stoyakFilter->blockSignals(false);
	ui.groupBox_kotelniFilter->blockSignals(false);
	ui.groupBox_filterBudynkiv->blockSignals(false);
	
	populateAbonentyTable();
}
//--------------------utech--------------------utech--------------------utech--------------------
void tke_form_printCustomKvyt::action_setSelectedAsPrinted_activated()
{
	QModelIndexList selList = ui.tableWidget_abonenty->selectionModel()->selectedRows(RAHUNOK_COL);
	foreach (QModelIndex ind, selList)
		tke_form_printCustomKvyt::setKvytPrinted(ind.data().toInt(), ui.dateEdit_cDate->date());
	
	populateAbonentyTable();
}
//--------------------utech--------------------utech--------------------utech--------------------
void tke_form_printCustomKvyt::action_setSelectedAsNotPrinted_activated()
{
	QModelIndexList selList = ui.tableWidget_abonenty->selectionModel()->selectedRows(RAHUNOK_COL);
	foreach (QModelIndex ind, selList)
		tke_form_printCustomKvyt::setKvytNotPrinted(ind.data().toInt(), ui.dateEdit_cDate->date());
	
	populateAbonentyTable();
}
//--------------------utech--------------------utech--------------------utech--------------------
bool tke_form_printCustomKvyt::setKvytPrinted(int rah, QDate date)
{
	QSqlQuery query;
	query.exec("SELECT true FROM sl_printedKvyt \
				WHERE Rahunok="+sqlStr(rah)+" \
					and year(CDate)="+sqlStr(date.year())+" \
					and month(CDate)="+sqlStr(date.month()));
	if (query.next())
		return true;
	return query.exec("INSERT INTO sl_printedKvyt (CDate, Rahunok) \
				VALUES ("+sqlStr(date)+", "+sqlStr(rah)+")");
}
//--------------------utech--------------------utech--------------------utech--------------------
bool tke_form_printCustomKvyt::setKvytNotPrinted(int rah, QDate date)
{
	QSqlQuery query;
	query.exec("SELECT true FROM sl_printedKvyt \
				WHERE Rahunok="+sqlStr(rah)+" \
					and year(CDate)="+sqlStr(date.year())+" \
					and month(CDate)="+sqlStr(date.month()));
	if (!query.next())
		return true;
	return query.exec("DELETE FROM sl_printedKvyt \
						WHERE Rahunok="+sqlStr(rah)+" \
							and year(CDate)="+sqlStr(date.year())+" \
							and month(CDate)="+sqlStr(date.month()));
}
//--------------------utech--------------------utech--------------------utech--------------------
void tke_form_printCustomKvyt::tableWidget_abonenty_itemSelectionChanged()
{
	int selRowsCount = ui.tableWidget_abonenty->selectionModel()->selectedRows().count();
	if (selRowsCount <= MAX_ABON_COUNT_TO_PRINT)
		ui.label_selectedRowsCount->setText("Позначено абонентів: "+QString::number(selRowsCount)+".");
	else
		ui.label_selectedRowsCount->setText("<span style=\" color:#aa0000;\">!!! Позначено абонентів: "+QString::number(selRowsCount)+".</span>");
}
//--------------------utech--------------------utech--------------------utech--------------------
tke_form_printCustomKvyt::~tke_form_printCustomKvyt()
{
	emit mainWindowClosed();
}
//--------------------utech--------------------utech--------------------utech--------------------
