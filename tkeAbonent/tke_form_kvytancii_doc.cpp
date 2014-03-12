//------------------------------------------------------------
//
// Implamantation of tke_Dialog_narah class.
// Created by Roman Mulyk 20.01.2007
//
//------------------------------------------------------------

#include "tke_form_kvytancii_doc.h"
#include "uform_kvytancii_doc_delegate.h"

//------------------------------------------------------------
tke_form_kvytancii_doc::tke_form_kvytancii_doc(QWidget *parent)
        : QWidget(parent)
{
    ui.setupUi(this);

	date = QDate::currentDate();
	codec = QTextCodec::codecForName("CP1251");
	prnt = new QPrinter();
	
	ui.spinBox_month->setValue(date.month());
	ui.spinBox_year->setValue(date.year());
	
	//Налаштування моделі таблиці
	tableModel = new QSqlTableModel(this);
	tableModel->setTable("kvytancii_html");
	tableModel->setHeaderData(1, Qt::Horizontal, codec->toUnicode("Пакет") );
	tableModel->setHeaderData(2, Qt::Horizontal, codec->toUnicode("Дата") );
	dateFiltrChanged();
	
	ui.tableView->setModel(tableModel);
	ui.tableView->setColumnHidden(0, true);
	ui.tableView->setColumnHidden(3, true);
	ui.tableView->setItemDelegate(new UFormKvytanciiDocDelegate(ui.tableView));
	ui.tableView->resizeColumnsToContents();
	ui.tableView->setColumnWidth( 2, 65 );
	
		//connectors 
			//фільтр
	connect(ui.groupBox_date_filtr, SIGNAL(toggled(bool)), this, SLOT(dateFiltrChanged())); //Активовано groupBox_date_filtr --> зміна фільтру
	connect(ui.spinBox_month, SIGNAL(valueChanged(int)), this, SLOT(dateFiltrChanged())); //Змінено значення місяця фільтру --> зміна фільтру
	connect(ui.spinBox_year, SIGNAL(valueChanged(int)), this, SLOT(dateFiltrChanged())); //Змінено значення року фільтру --> зміна фільтру
			//загальні
	connect(ui.tableView->selectionModel(), SIGNAL(currentRowChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(showHtml(const QModelIndex &))); //Вибрано стрічку --> показати відомість
	
	connect(ui.pushButton_print, SIGNAL(clicked()), this, SLOT(printDoc())); //друк
	connect(ui.pushButton_print_on, SIGNAL(clicked()), this, SLOT(printDocOn())); //друк
	connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(close())); //вихід
}
//------------------------------------------------------------
int tke_form_kvytancii_doc::get_Month()
{
	return ui.spinBox_month->value();
}
//------------------------------------------------------------
int tke_form_kvytancii_doc::get_Year()
{
	return ui.spinBox_year->value();
}
//------------------------------------------------------------
void tke_form_kvytancii_doc::dateFiltrChanged()
{
	//Налаштування максимумів для combo місяців
	if (ui.spinBox_year->value() == date.year())
		ui.spinBox_month->setMaximum(date.month());
	else
		ui.spinBox_month->setMaximum(12);
	if (ui.spinBox_year->value() == date.year())
		ui.spinBox_month->setMaximum(date.month());
	else
		ui.spinBox_month->setMaximum(12);
	//Зміна фільтру
	QString filtrString;
	if (ui.groupBox_date_filtr->isChecked()){
		filtrString = "year(Packet_date)=" + QVariant(ui.spinBox_year->value()).toString() +
						" and month(Packet_date)=" + QVariant(ui.spinBox_month->value()).toString();	
	}
	else
		filtrString = "";
	tableModel->setFilter(filtrString);
	tableModel->select();
	
	ui.textEdit->clear();
}
//------------------------------------------------------------
void tke_form_kvytancii_doc::showHtml(const QModelIndex &index)
{
	QModelIndex index_1 = tableModel->index(index.row(), 3);
	ui.textEdit->document()->setHtml( tableModel->data(index_1).toString() );
}
//------------------------------------------------------------
void tke_form_kvytancii_doc::printDoc()
{
	ui.textEdit->document()->print(prnt);
}
//------------------------------------------------------------
void tke_form_kvytancii_doc::printDocOn()
{
	QPrintDialog *dlg = new QPrintDialog(prnt, this);
	if (dlg->exec() != QDialog::Accepted)
		return;
	ui.textEdit->document()->print(prnt);
}
//------------------------------------------------------------
tke_form_kvytancii_doc::~tke_form_kvytancii_doc()
{
	delete tableModel;
	delete prnt;
}
//------------------------------------------------------------
