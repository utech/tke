//------------------------------------------------------------
//
// Implamantation of tke_Dialog_narah class.
// Created by Roman Mulyk 29.10.2010
//
//------------------------------------------------------------

#include "tke_form_recalc_obsyag_gazu.h"

#include <UMath>
#include <USqlAccessible>
#include <UStandardDelegate>

#include "uprint_docs.h"

#define BUDTABLE_ID_____COL 0 //Ідентифікатор
#define BUDTABLE_BUDID__COL 1 //Ідентифікатор будинку
#define BUDTABLE_USEBUD_COL 2 //Чи використовувати у розрахунках будинок
#define BUDTABLE_NAVANT_COL 3 //Навантаження
#define BUDTABLE_DNOPAL_COL 4 //Дні опалення
#define BUDTABLE_POKIDN_COL 5 //Початок/кінець місяця надання послуг
#define BUDTABLE_SERTEM_COL 6 //Середня температура
#define BUDTABLE_OBSTEN_COL 7 //Обсяг відпущеної теплової енергії

//------------------------------------------------------------
tke_form_recalc_obsyag_gazu::tke_form_recalc_obsyag_gazu(QDate dt, QWidget *parent)
        : QWidget(parent)
{
    ui.setupUi(this);

	date = dt;
	
	twExt = new UTableWidgetSqlExt(ui.tableWidget_rozrahInMonth, "gas_monthRozrah");
	twExt->setSortingEnabled(true);
	
	ui.tableWidget_rozrahInMonth->setColumnCount(11);
	ui.tableWidget_rozrahInMonth->setHorizontalHeaderLabels(QStringList() << "id"
															<< "Місяць"
															<< "Номер док."
															<< "Обсяг\nгазу"
															<< "Середн.\nККД"
															<< "Теплотворна\nздатн."
															<< "Обсяг\nвиробл ТЕ"
															<< "Коеф. втрат\nв мережі"
															<< "Обсяг\nвідпущ. ТЕ"
															<< "Перерах.\nгазу"
															<< "Примітка");
	
	//Запповнення назв полів
	twExt->colNamesList << "id"
						<< "CDate"
						<< "NumInPeriod"
						<< "ObsyagPostavl"
						<< "SerKKD"
						<< "TeplotvZdatn"
						<< "ObsyagVyroblTeploenerg"
						<< "KoefVtrat"
						<< "ObsyagVidpushchTeploenerg"
						<< "PererahObsyagGazu"
						<< "Prymitka";
	
	//Задання делегатів
	twExt->setItemDelegateForColumn("CDate", new UDateTimeDelegate(twExt, "MM.yyyy"));
	twExt->setItemDelegateForColumn("NumInPeriod", new USpinBoxDelegate(0,999, twExt));
	twExt->setItemDelegateForColumn("ObsyagPostavl", new UDoubleSpinBoxDelegate(0,999999999,3, twExt));
	twExt->setItemDelegateForColumn("SerKKD", new UDoubleSpinBoxDelegate(0,999999999,4, twExt));
	twExt->setItemDelegateForColumn("TeplotvZdatn", new UDoubleSpinBoxDelegate(0,999999999,2, twExt));
	twExt->setItemDelegateForColumn("ObsyagVyroblTeploenerg", new UDoubleSpinBoxDelegate(0,999999999,3, twExt));
	twExt->setItemDelegateForColumn("KoefVtrat", new UDoubleSpinBoxDelegate(0,100,1, twExt));
	twExt->setItemDelegateForColumn("ObsyagVidpushchTeploenerg", new UDoubleSpinBoxDelegate(0,999999999,3, twExt));
	twExt->setItemDelegateForColumn("PererahObsyagGazu", new UDoubleSpinBoxDelegate(0,999999999,3, twExt));
	
	twExt->setColumnHidden(0);
	twExt->setColumnWidth("CDate", 80);
	twExt->setColumnWidth("NumInPeriod", 80);
	twExt->setColumnWidth("ObsyagPostavl", 90);
	twExt->setColumnWidth("SerKKD", 80);
	twExt->setColumnWidth("TeplotvZdatn", 80);
	twExt->setColumnWidth("ObsyagVyroblTeploenerg", 90);
	twExt->setColumnWidth("KoefVtrat", 90);
	twExt->setColumnWidth("ObsyagVidpushchTeploenerg", 90);
	twExt->setColumnWidth("PererahObsyagGazu", 90);
	twExt->setColumnWidth("Prymitka", 150);
	
		
	//Вирівнювання в полі
	twExt->addColAlignment("CDate", Qt::AlignVCenter | Qt::AlignRight);
	twExt->addColAlignment("NumInPeriod", Qt::AlignVCenter | Qt::AlignRight);
	twExt->addColAlignment("ObsyagPostavl", Qt::AlignVCenter | Qt::AlignRight);
	twExt->addColAlignment("SerKKD", Qt::AlignVCenter | Qt::AlignRight);
	twExt->addColAlignment("TeplotvZdatn", Qt::AlignVCenter | Qt::AlignRight);
	twExt->addColAlignment("ObsyagVyroblTeploenerg", Qt::AlignVCenter | Qt::AlignRight);
	twExt->addColAlignment("KoefVtrat", Qt::AlignVCenter | Qt::AlignRight);
	twExt->addColAlignment("ObsyagVidpushchTeploenerg", Qt::AlignVCenter | Qt::AlignRight);
	twExt->addColAlignment("PererahObsyagGazu", Qt::AlignVCenter | Qt::AlignRight);
	twExt->addColAlignment("Prymitka", Qt::AlignVCenter | Qt::AlignLeft);
	
	twExt->setSqlOrderBy("CDate, NumInPeriod");
	
	//Заповнення таблиці
	twExt->populateTable();
	
	initBudPokaznTable();
	populateBudPokaznTable();
	connect(ui.tableWidget_rozrahInMonth->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &,const QModelIndex &)), 
				this, SLOT(populateBudPokaznTable()));
	
	connect(ui.pushButton_createMonthRozrah, SIGNAL(clicked()), this, SLOT(pushButton_createMonthRozrah_clicked()));
	connect(ui.pushButton_deleteMonthRozrah, SIGNAL(clicked()), this, SLOT(pushButton_deleteMonthRozrah_clicked()));
	connect(ui.pushButton_calcValuesRozrahunok, SIGNAL(clicked()), this, SLOT(pushButton_calcValuesRozrahunok_clicked()));
	connect(ui.pushButton_printAkt, SIGNAL(clicked()), this, SLOT(pushButton_printAkt_clicked()));
	connect(ui.pushButton_printRozrahunok, SIGNAL(clicked()), this, SLOT(pushButton_printRozrahunok_clicked()));
	connect(ui.pushButton_copyValuesFromPrevMonth, SIGNAL(clicked()), this, SLOT(pushButton_copyValuesFromPrevMonth_clicked()));
	connect(ui.pushButton_deleteButPokaznInRozrahunok, SIGNAL(clicked()), this, SLOT(pushButton_deleteButPokaznInRozrahunok_clicked()));
	connect(ui.pushButton_calcValuesBud, SIGNAL(clicked()), this, SLOT(pushButton_calcValuesBud_clicked()));
}
//------------------------------------------------------------
void tke_form_recalc_obsyag_gazu::initBudPokaznTable()
{
	ui.tableWidget_budPokazn->setColumnCount(8);
	ui.tableWidget_budPokazn->setColumnHidden(0, true);
	ui.tableWidget_budPokazn->setColumnHidden(1, true);
	ui.tableWidget_budPokazn->setHorizontalHeaderLabels(QStringList() << "id"
															<< "Bud_id."
															<< "Врахув."
															<< "Навантаж."
															<< "Дні опал."
															<< "Поч./Кін міс."
															<< "Сер. t, С"
															<< "Обсяг\nпередан. ТЕ");
	
	ui.tableWidget_budPokazn->setItemDelegateForColumn(BUDTABLE_USEBUD_COL, 
								new UCheckBoxDelegate(ui.tableWidget_rozrahInMonth, "Викор.", "-", "Використ.?"));
	ui.tableWidget_budPokazn->setItemDelegateForColumn(BUDTABLE_NAVANT_COL, 
								new UDoubleSpinBoxDelegate(0, 999999999, 8, ui.tableWidget_rozrahInMonth));
	ui.tableWidget_budPokazn->setItemDelegateForColumn(BUDTABLE_DNOPAL_COL, 
								new USpinBoxDelegate(0, 100, ui.tableWidget_rozrahInMonth));
	ui.tableWidget_budPokazn->setItemDelegateForColumn(BUDTABLE_SERTEM_COL, 
								new UDoubleSpinBoxDelegate(-40, 30, 2, ui.tableWidget_rozrahInMonth));
	ui.tableWidget_budPokazn->setItemDelegateForColumn(BUDTABLE_OBSTEN_COL, 
								new UDoubleSpinBoxDelegate(0, 999999999, 3, ui.tableWidget_rozrahInMonth));
}
//------------------------------------------------------------
void tke_form_recalc_obsyag_gazu::populateBudPokaznTable()
{
	disconnect(ui.tableWidget_budPokazn, SIGNAL(itemChanged(QTableWidgetItem *)), 
					this, SLOT(tableWidget_budPokazn_itemChanged(QTableWidgetItem *)));
	
	QSqlQuery query;
	QTableWidgetItem *item;
	query.exec("SELECT count(*) FROM budynky");
	query.next();
	int rowCount = query.value(0).toInt();
	ui.tableWidget_budPokazn->setRowCount(rowCount);
	
	query.exec("SELECT budynky.Vulycia, budynky.Bud_Num, budynky.id, gas_BudPokazn.id, gas_BudPokazn.UseBud, \
						gas_BudPokazn.Navantajennya, gas_BudPokazn.DniOpal, gas_BudPokazn.OpalPochKincMonth, \
						gas_BudPokazn.SerTemperatura, gas_BudPokazn.ObsyagVidpushchTE \
				FROM budynky \
				LEFT JOIN (SELECT * FROM gas_BudPokazn \
							WHERE Rozrah_id="+sqlStr(twExt->currentItemData("id").toInt())+") \
					AS gas_BudPokazn ON budynky.id=gas_BudPokazn.Budynok_id \
				ORDER BY budynky.Vulycia, budynky.Bud_Num");
	
	for (int row=0; row<rowCount && query.next(); row++){
		//Адреса на хедері
		item = new QTableWidgetItem();
		item->setData(Qt::EditRole, query.value(0).toString()+" "+query.value(1).toString());
		item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
		ui.tableWidget_budPokazn->setVerticalHeaderItem(row, item);
		
		//Ідентифікатор місячного розрахунку
		item = new QTableWidgetItem();
		item->setData(Qt::EditRole, query.value(3));
		item->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
		ui.tableWidget_budPokazn->setItem(row, BUDTABLE_ID_____COL, item);
		
		//Ідентифікатор будинка
		item = new QTableWidgetItem();
		item->setData(Qt::EditRole, query.value(2));
		item->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
		ui.tableWidget_budPokazn->setItem(row, BUDTABLE_BUDID__COL, item);
		
		//Чи використовувати у розрахунках будинок
		item = new QTableWidgetItem();
		item->setData(Qt::EditRole, query.value(4));
		item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
		ui.tableWidget_budPokazn->setItem(row, BUDTABLE_USEBUD_COL, item);
		
		//Навантаження
		item = new QTableWidgetItem();
		item->setData(Qt::EditRole, query.value(5));
		item->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
		ui.tableWidget_budPokazn->setItem(row, BUDTABLE_NAVANT_COL, item);
		
		//Дні опалення
		item = new QTableWidgetItem();
		item->setData(Qt::EditRole, query.value(6));
		item->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
		ui.tableWidget_budPokazn->setItem(row, BUDTABLE_DNOPAL_COL, item);
		
		//Початок/кінець місяця надання послуг
		item = new QTableWidgetItem();
		item->setData(Qt::EditRole, query.value(7));
		item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
		ui.tableWidget_budPokazn->setItem(row, BUDTABLE_POKIDN_COL, item);
		
		//Середня температура
		item = new QTableWidgetItem();
		item->setData(Qt::EditRole, query.value(8));
		item->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
		ui.tableWidget_budPokazn->setItem(row, BUDTABLE_SERTEM_COL, item);
		
		//Обсяг відпущеної теплової енергії
		item = new QTableWidgetItem();
		item->setData(Qt::EditRole, query.value(9));
		item->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
		ui.tableWidget_budPokazn->setItem(row, BUDTABLE_OBSTEN_COL, item);
	}
	
	ui.tableWidget_budPokazn->resizeRowsToContents();
	
	connect(ui.tableWidget_budPokazn, SIGNAL(itemChanged(QTableWidgetItem *)), 
					this, SLOT(tableWidget_budPokazn_itemChanged(QTableWidgetItem *)));
	
	calcSumPocazn();
}
//------------------------------------------------------------
void tke_form_recalc_obsyag_gazu::calcSumPocazn()
{
	int rowCount = ui.tableWidget_budPokazn->rowCount();
	double navantajSum = 0, obsyagSum = 0;
	for (int row = 0; row < rowCount; ++row) {
		navantajSum += ui.tableWidget_budPokazn->item(row, BUDTABLE_NAVANT_COL)->data(Qt::EditRole).toDouble();
		obsyagSum += ui.tableWidget_budPokazn->item(row, BUDTABLE_OBSTEN_COL)->data(Qt::EditRole).toDouble();
	}
	ui.label_sum->setText("Навантаження: "+QString::number(navantajSum,'f',3)+"; Обсяг: "+QString::number(obsyagSum,'f',3));
}
//------------------------------------------------------------
void tke_form_recalc_obsyag_gazu::tableWidget_budPokazn_itemChanged(QTableWidgetItem * item)
{
	disconnect(ui.tableWidget_budPokazn, SIGNAL(itemChanged(QTableWidgetItem *)), 
					this, SLOT(tableWidget_budPokazn_itemChanged(QTableWidgetItem *)));
	
	int row = item->row();
	int id = 0;
	int budId = ui.tableWidget_budPokazn->item(row, BUDTABLE_BUDID__COL)->data(Qt::EditRole).toInt();
	int rozrahId = twExt->currentItemData("id").toInt();
	QSqlQuery query;
	if ( !ui.tableWidget_budPokazn->item(row, BUDTABLE_ID_____COL)->data(Qt::EditRole).toInt() ) { //перевірка необхідності створення стічки
		id = nextLichylnyk("gas_BudPokazn");
		if (query.exec("INSERT INTO gas_BudPokazn (id, Rozrah_id, Budynok_id) \
						VALUES ("+sqlStr(id)+", "+sqlStr(rozrahId)+", "+sqlStr(budId)+")"))
			ui.tableWidget_budPokazn->item(row, BUDTABLE_ID_____COL)->setData(Qt::EditRole, id);
	}
	
	id = ui.tableWidget_budPokazn->item(row, BUDTABLE_ID_____COL)->data(Qt::EditRole).toInt();
	
	if (id) {
		query.exec("UPDATE gas_BudPokazn SET \
							UseBud="+sqlStr(ui.tableWidget_budPokazn->item(row, BUDTABLE_USEBUD_COL)->data(Qt::EditRole).toBool(), false)+", \
							Navantajennya="+sqlStr(ui.tableWidget_budPokazn->item(row, BUDTABLE_NAVANT_COL)->data(Qt::EditRole).toDouble(), false)+", \
							DniOpal="+sqlStr(ui.tableWidget_budPokazn->item(row, BUDTABLE_DNOPAL_COL)->data(Qt::EditRole).toInt(), false)+", \
							OpalPochKincMonth="+sqlStr(ui.tableWidget_budPokazn->item(row, BUDTABLE_POKIDN_COL)->data(Qt::EditRole).toInt(), false)+", \
							SerTemperatura="+sqlStr(ui.tableWidget_budPokazn->item(row, BUDTABLE_SERTEM_COL)->data(Qt::EditRole).toDouble(), false)+", \
							ObsyagVidpushchTE="+sqlStr(ui.tableWidget_budPokazn->item(row, BUDTABLE_OBSTEN_COL)->data(Qt::EditRole).toDouble(), false)+" \
					WHERE id="+sqlStr(id));
		qDebug() << query.lastQuery();
	}
	
	query.exec("SELECT '', '', '', '', UseBud, Navantajennya, DniOpal, OpalPochKincMonth, \
						SerTemperatura, ObsyagVidpushchTE \
				FROM gas_BudPokazn \
				WHERE id="+sqlStr(id));
	query.next();
	
	//Чи використовувати у розрахунках будинок
	item = new QTableWidgetItem();
	item->setData(Qt::EditRole, query.value(4));
	item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	ui.tableWidget_budPokazn->setItem(row, BUDTABLE_USEBUD_COL, item);
	
	//Навантаження
	item = new QTableWidgetItem();
	item->setData(Qt::EditRole, query.value(5));
	item->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
	ui.tableWidget_budPokazn->setItem(row, BUDTABLE_NAVANT_COL, item);
	
	//Дні опалення
	item = new QTableWidgetItem();
	item->setData(Qt::EditRole, query.value(6));
	item->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
	ui.tableWidget_budPokazn->setItem(row, BUDTABLE_DNOPAL_COL, item);
	
	//Початок/кінець місяця надання послуг
	item = new QTableWidgetItem();
	item->setData(Qt::EditRole, query.value(7));
	item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	ui.tableWidget_budPokazn->setItem(row, BUDTABLE_POKIDN_COL, item);
	
	//Середня температура
	item = new QTableWidgetItem();
	item->setData(Qt::EditRole, query.value(8));
	item->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
	ui.tableWidget_budPokazn->setItem(row, BUDTABLE_SERTEM_COL, item);
	
	//Обсяг відпущеної теплової енергії
	item = new QTableWidgetItem();
	item->setData(Qt::EditRole, query.value(9));
	item->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
	ui.tableWidget_budPokazn->setItem(row, BUDTABLE_OBSTEN_COL, item);
	
	connect(ui.tableWidget_budPokazn, SIGNAL(itemChanged(QTableWidgetItem *)), 
					this, SLOT(tableWidget_budPokazn_itemChanged(QTableWidgetItem *)));
	
	calcSumPocazn();
}
//------------------------------------------------------------
void tke_form_recalc_obsyag_gazu::pushButton_createMonthRozrah_clicked()
{
	if (!twExt->addRow())
		QMessageBox::critical(0,"Помилка створення запису","Запис не створено");
	calcSumPocazn();
}
//------------------------------------------------------------
void tke_form_recalc_obsyag_gazu::pushButton_deleteMonthRozrah_clicked()
{
	if	( QMessageBox::question(
                this,
                "!!! Видалення стрічки !!!",
                "Ви дійсно бажаєте видалити стрічку?",
                "Так",
				"Ні-ні-ні",
                QString(), 0, 1)
		)
		return;
	if (!twExt->deleteRow())
		QMessageBox::critical(0,"Помикла видалення запису","Запис не видалено");
	calcSumPocazn();
}
//------------------------------------------------------------
void tke_form_recalc_obsyag_gazu::pushButton_calcValuesRozrahunok_clicked()
{
	double obsyagVyroblTeplEnerg = twExt->currentItemData("ObsyagPostavl").toDouble()
									* twExt->currentItemData("SerKKD").toDouble()
									* twExt->currentItemData("TeplotvZdatn").toDouble() 
									/ 1000;
	
	twExt->setCurrentItemData("ObsyagVyroblTeploenerg", uDToM(obsyagVyroblTeplEnerg));
	
	
	QSqlQuery query;
	query.exec("SELECT sum(ObsyagVidpushchTE) FROM gas_BudPokazn \
				WHERE UseBud=true and Rozrah_id="+sqlStr(twExt->currentItemData("id").toInt()));
	query.next();
	double obsyagVidpushchEnerg = query.value(0).toDouble() * (1 + twExt->currentItemData("KoefVtrat").toDouble() / 100.0);
	twExt->setCurrentItemData("ObsyagVidpushchTeploenerg", uDToM(obsyagVidpushchEnerg));
	
	
	if (twExt->currentItemData("ObsyagVyroblTeploenerg").toDouble() < 0.01) {
		QMessageBox::critical(0,"Невірне значення виробленої теплової енергії","Невірне значення виробленої теплової енергії");
		return;
	}
	
	double pererahObsyagGazu = qRound(twExt->currentItemData("ObsyagPostavl").toDouble()
								* twExt->currentItemData("ObsyagVidpushchTeploenerg").toDouble()
								/ twExt->currentItemData("ObsyagVyroblTeploenerg").toDouble() * 1000);
	twExt->setCurrentItemData("PererahObsyagGazu", uDToM(pererahObsyagGazu));
	
	calcSumPocazn();
}
//------------------------------------------------------------
void tke_form_recalc_obsyag_gazu::pushButton_printAkt_clicked()
{
	
}
//------------------------------------------------------------
void tke_form_recalc_obsyag_gazu::pushButton_printRozrahunok_clicked()
{
	UPrintDocs * printDocs = new UPrintDocs();
	printDocs->print_rozrahunok_pererahuvannya_obsyagu_tepl_energ(twExt->currentItemData("id").toInt());
	connect(this, SIGNAL(closeLoginSignal()), printDocs->printform, SLOT(close()));
	connect(this, SIGNAL(mainWindowClosed()), printDocs->printform, SLOT(close()));
	delete printDocs;
}
//------------------------------------------------------------
void tke_form_recalc_obsyag_gazu::pushButton_copyValuesFromPrevMonth_clicked()
{
	
}
//------------------------------------------------------------
void tke_form_recalc_obsyag_gazu::pushButton_deleteButPokaznInRozrahunok_clicked()
{
	
}
//------------------------------------------------------------
void tke_form_recalc_obsyag_gazu::pushButton_calcValuesBud_clicked()
{
	int rowCount = ui.tableWidget_budPokazn->rowCount();
	
	for (int row=0; row<rowCount; row++) {
		double navantaj = ui.tableWidget_budPokazn->item(row, BUDTABLE_NAVANT_COL)->data(Qt::EditRole).toDouble();
		double serTemperat = ui.tableWidget_budPokazn->item(row, BUDTABLE_SERTEM_COL)->data(Qt::EditRole).toDouble();
		int dniOpal = ui.tableWidget_budPokazn->item(row, BUDTABLE_DNOPAL_COL)->data(Qt::EditRole).toInt();
		
		double odsyag = qRound(navantaj * (18 - serTemperat) / 37 * 24 * dniOpal * 1000) / 1000.0;
		ui.tableWidget_budPokazn->item(row, BUDTABLE_OBSTEN_COL)->setData(Qt::EditRole, odsyag);
	}
	
	QMessageBox::information(0, "Розрахунок завершено", "Успішно завершено розрахунок обсягу теплової енергії.");
}
//------------------------------------------------------------
tke_form_recalc_obsyag_gazu::~tke_form_recalc_obsyag_gazu()
{
	
}
//------------------------------------------------------------
