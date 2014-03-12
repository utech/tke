//------------------------------------------------------------
//
// Implamantation of tkeFormDaysOpalSimple class.
// Created by Roman Mulyk 16.11.2007
//
//------------------------------------------------------------

#include "tke_form_days_opal_simple.h"
#include <USqlAccessible>

//------------------------------------------------------------
tkeFormDaysOpalSimple::tkeFormDaysOpalSimple(QWidget *parent, int pidrozdilId, int year, int month, QString pidrozdilName)
        : QWidget(parent)
{
	ui.setupUi(this);
	ui.pushButton_org_prev->setVisible(false);
	ui.pushButton_org_next->setVisible(false);
	ui.lineEdit_Pidrozdil_Name->setText(pidrozdilName);
	setAttribute(Qt::WA_DeleteOnClose);
	
	//initialize days_in_month array
	days_in_month[0]=31;
	days_in_month[1]=28;
	days_in_month[2]=31;
	days_in_month[3]=30;
	days_in_month[4]=31;
	days_in_month[5]=30;
	days_in_month[6]=31;
	days_in_month[7]=31;
	days_in_month[8]=30;
	days_in_month[9]=31;
	days_in_month[10]=30;
	days_in_month[11]=31;
	
	codec = QTextCodec::codecForName("CP1251");
	dniOpalEdited = false;
	pidrId = pidrozdilId;
	curYear = year;
	curMonth = month;
	
	//default values
	d_OpalLich_Nolich = 1;
	
			//set table kotelni
	tableModel = new QSqlRelationalTableModel(this);
	tableModel->setTable("grafik_opal");
	tableModel->setEditStrategy(QSqlTableModel::OnRowChange);
	tableModel->setRelation(5, QSqlRelation("int_values", "id", "opal_days_values"));
	tableModel->setHeaderData(3, Qt::Horizontal, codec->toUnicode("Поч.день") );
	tableModel->setHeaderData(4, Qt::Horizontal, codec->toUnicode("Кінц.день") );
	tableModel->setHeaderData(5, Qt::Horizontal, codec->toUnicode("Опалення") );
	tableModel->select();
	ui.tableView_dni_opal->setModel(tableModel);
	ui.tableView_dni_opal->setColumnHidden(0, true);
	ui.tableView_dni_opal->setColumnHidden(1, true);
	ui.tableView_dni_opal->setColumnHidden(2, true);
	ui.tableView_dni_opal->setItemDelegate(new UDniOpalDelegate(ui.tableView_dni_opal));
	ui.tableView_dni_opal->resizeColumnsToContents();
	ui.tableView_dni_opal->resizeRowsToContents();
	ui.tableView_dni_opal->setAlternatingRowColors(true);
	//Контекстне меню
	ui.tableView_dni_opal->addAction(ui.actionAddString);
	ui.tableView_dni_opal->addAction(ui.actionDelString);
	ui.tableView_dni_opal->setContextMenuPolicy(Qt::ActionsContextMenu);
		// end of set table kotelni
	
	populateTableDniOpal();
	
	//connectors
	connect(ui.pushButton_days_opal_lich, SIGNAL(clicked ()), this, SLOT(pushButton_days_opal_lich_clicked()));
	
	connect(ui.actionAddString, SIGNAL(activated()), this, SLOT(actionAddString_activated()));
	connect(ui.actionDelString, SIGNAL(activated()), this, SLOT(actionDelString_activated()));
	connect(tableModel, SIGNAL(primeInsert(int, QSqlRecord &)), this, SLOT(tableModel_primeInsert(int, QSqlRecord &)));
	connect(ui.pushButton_close_form, SIGNAL(clicked ()), this, SLOT(close()));//close form
}
//----Slots-------------------------------------------------
void tkeFormDaysOpalSimple::populateTableDniOpal()
{
	//Заповнення таблиці днів опалення
	tableModel->setFilter("Pidrozdil="+QVariant(pidrId).toString()+" \
						and year(Opal_date)="+QVariant(curYear).toString()+" \
						and month(Opal_date)="+QVariant(curMonth).toString());
						
	//Керування кнопкою створення стрічки
	if (tableModel->rowCount()==0)
		ui.pushButton_days_opal_lich->setEnabled(true);
	else
		ui.pushButton_days_opal_lich->setEnabled(false);
}
//------------------------------------------------------------
void tkeFormDaysOpalSimple::pushButton_days_opal_lich_clicked()
{
	QSqlRecord record = tableModel->record(0);
	if (tableModel->rowCount()==0){
			tableModel->insertRows(0, 1);
			record.setValue("id", nextLichylnyk("grafik_opal"));
			record.setValue("Pidrozdil", pidrId);
			record.setValue("Opal_date", QDate(curYear, curMonth, 1));
			record.setValue("Day_poch_opal", 1);
			record.setValue("Day_kinc_opal", days_in_month[curMonth-1]);
			record.setValue("Opal_lich", 1);
			tableModel->setRecord(0, record);
			tableModel->submitAll();
	}
	else{
	
	}
	ui.pushButton_days_opal_lich->setEnabled(false);
}
//------------------------------------------------------------
void tkeFormDaysOpalSimple::actionAddString_activated()
{
	disconnect(ui.tableView_dni_opal->selectionModel(), SIGNAL(currentChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(tableModel_selectionModel_currentChanged()));
	int row = tableModel->rowCount();
	tableModel->insertRow(row);
	
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT max(Day_kinc_opal) FROM grafik_opal WHERE Pidrozdil="+QVariant(pidrId).toString()+
						" and year(Opal_date)="+QVariant(curYear).toString()+
						" and month(Opal_date)="+QVariant(curMonth).toString());
	query->seek(0);
	int maxPrevDay = query->value(0).toInt();
	
	delete query;
	
	QSqlRecord record = tableModel->record(row);
	record.setValue("Pidrozdil", pidrId);
	record.setValue("Opal_date", QDate(curYear, curMonth, 1));
	record.setValue("Day_poch_opal", QVariant(maxPrevDay+1));
	record.setValue("Day_kinc_opal", QVariant(31));
	record.setValue("Opal_lich", QVariant(d_OpalLich_Nolich));
	tableModel->setRecord(row, record);
	
	dniOpalEdited = true; //Обов'язковий, використовується для здійснення після редагування SubmitAll()
	
	QModelIndex index = tableModel->index(row, 3);
	ui.tableView_dni_opal->setCurrentIndex(index);
	ui.tableView_dni_opal->edit(index);
	connect(ui.tableView_dni_opal->selectionModel(), SIGNAL(currentChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(tableModel_selectionModel_currentChanged()));
}
//------------------------------------------------------------
void tkeFormDaysOpalSimple::actionDelString_activated()
{
	if	( ! QMessageBox::question(
                this,
                codec->toUnicode("!!! Видалення стрічки !!!"),
                codec->toUnicode("Ви дійсно бажаєте видалити сирічку?"),
                codec->toUnicode("Так"),
				codec->toUnicode("Ні-ні-ні"),
                QString(), 0, 1)
		){
		tableModel->removeRow(ui.tableView_dni_opal->currentIndex().row());
		//Керування кнопкою створення стрічки
		if (tableModel->rowCount()==0)
			ui.pushButton_days_opal_lich->setEnabled(true);
		else
			ui.pushButton_days_opal_lich->setEnabled(false);
	}
}
//------------------------------------------------------------
void tkeFormDaysOpalSimple::tableModel_primeInsert(int row, QSqlRecord & sqlRecord)
{
	sqlRecord.setValue("id", nextLichylnyk("grafik_opal"));
	dniOpalEdited = false;
}
//------------------------------------------------------------
void tkeFormDaysOpalSimple::tableModel_selectionModel_currentChanged()
{
	
	if (dniOpalEdited){
		tableModel->submitAll();
		dniOpalEdited = false;
	}
}
//------------------------------------------------------------
//------------------------------------------------------------
//------functions--------------------------------------------
QSqlRelationalTableModel* tkeFormDaysOpalSimple::dniOpalTableModel()
{
	return tableModel;
}
//------------------------------------------------------------
//------------------------------------------------------------
tkeFormDaysOpalSimple::~tkeFormDaysOpalSimple()
{
	delete tableModel;
}
//------------------------------------------------------------
