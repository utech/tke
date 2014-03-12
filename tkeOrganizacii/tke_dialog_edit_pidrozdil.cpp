//------------------------------------------------------------
//
// Implamantation of tke_MainWindow class.
// Created by Roman Mulyk 25.02.2007
//
//------------------------------------------------------------

#include "tke_dialog_edit_pidrozdil.h"

//------------------------------------------------------------
tkeDialogEditPidrozdil::tkeDialogEditPidrozdil(QWidget *parent, int pidrozdilId)
        : QDialog(parent)
{
	ui.setupUi(this);
	
	codec = QTextCodec::codecForName("CP1251");
	idPidrozdil = pidrozdilId;
	
	QSqlQuery * query = new QSqlQuery();
	QSqlRecord * record = new QSqlRecord;
	
	//Заповнення комбо котелень
	query->exec("SELECT * FROM kotelni ORDER BY Vulycia");
	kotelniList.clear();
	kotelniList << 0;
	ui.comboBox_Kotelnya->addItem("");
	while (query->next()){
		*record = query->record();
		kotelniList << record->value("id").toInt();
		ui.comboBox_Kotelnya->addItem(record->value("Nas_punkt").toString()
										+ codec->toUnicode(" вул.")
										+ record->value("Vulycia").toString() + " "
										+ record->value("Nomer").toString() + codec->toUnicode(", Котельня ")
										+ record->value("Kot_num").toString());
	}
	
	query->exec("SELECT * FROM pidrozdily WHERE id=" + QVariant(pidrozdilId).toString());
	query->seek(0);
	
	*record = query->record();
	
	ui.lineEdit_name->setText(record->value("Name").toString());

	if (record->value("Opal").toInt() != 0)
		ui.checkBox_Opal->setChecked(true);
	else
		ui.checkBox_Opal->setChecked(false);
	ui.comboBox_typ_narah_CO_bez_lich->setCurrentIndex(record->value("TypNarahCoBezLich").toInt());
	if (record->value("Gar_voda").toInt() != 0)
		ui.checkBox_Gar_voda->setChecked(true);
	else
		ui.checkBox_Gar_voda->setChecked(false);
	ui.doubleSpinBox_Ploshcha->setValue(record->value("Ploshcha").toDouble());
	ui.doubleSpinBox_obem->setValue(record->value("Obem").toDouble());
	ui.doubleSpinBox_tepl_harakter->setValue(record->value("Tepl_harakt").toDouble());
	ui.doubleSpinBox_t_vnutr->setValue(record->value("T_vnutr").toDouble());
	ui.doubleSpinBox_Teplove_navantaj->setValue(record->value("Tepl_navantaj").toDouble());
	if (record->value("Lich").toInt() != 0)
		ui.checkBox_Lichyknyk_e->setChecked(true);
	else
		ui.checkBox_Lichyknyk_e->setChecked(false);
	ui.comboBox_Kotelnya->setCurrentIndex(kotelniList.indexOf(record->value("Kotelnya_ID").toInt()));
	ui.doubleSpinBox_koef_lich->setValue(record->value("Koef_lich").toDouble());
	
	delete query;
	delete record;
	
	connect(ui.pushButton_cancel, SIGNAL(clicked ()), this, SLOT(reject()));
	connect(ui.pushButton_Ok, SIGNAL(clicked ()), this, SLOT(acceptAndSave()));
	
	//Конектори зміни записів
	connect(ui.lineEdit_name, SIGNAL(textEdited (const QString &)), this, SLOT(recordChanges()));
	connect(ui.checkBox_Opal, SIGNAL(stateChanged ( int )), this, SLOT(recordChanges()));
	connect(ui.comboBox_typ_narah_CO_bez_lich, SIGNAL(currentIndexChanged(int)), this, SLOT(recordChanges()));
	connect(ui.checkBox_Gar_voda, SIGNAL(stateChanged ( int )), this, SLOT(recordChanges()));
	connect(ui.doubleSpinBox_Ploshcha, SIGNAL(valueChanged (double)), this, SLOT(recordChanges()));
	connect(ui.doubleSpinBox_obem, SIGNAL(valueChanged (double)), this, SLOT(recordChanges()));
	connect(ui.doubleSpinBox_tepl_harakter, SIGNAL(valueChanged (double)), this, SLOT(recordChanges()));
	connect(ui.doubleSpinBox_t_vnutr, SIGNAL(valueChanged (double)), this, SLOT(recordChanges()));
	connect(ui.doubleSpinBox_Teplove_navantaj, SIGNAL(valueChanged (double)), this, SLOT(recordChanges()));
	connect(ui.checkBox_Lichyknyk_e, SIGNAL(stateChanged ( int )), this, SLOT(recordChanges()));
	connect(ui.comboBox_Kotelnya, SIGNAL(currentIndexChanged(int)), this, SLOT(recordChanges()));
	connect(ui.doubleSpinBox_koef_lich, SIGNAL(valueChanged (double)), this, SLOT(recordChanges()));
}
//----Slots-------------------------------------------------
void tkeDialogEditPidrozdil::recordChanges()
{
	ui.pushButton_Ok->setEnabled( true );
}
//------------------------------------------------------------
void tkeDialogEditPidrozdil::acceptAndSave()
{
	QSqlTableModel * modelUpdatePidrozdily = new QSqlTableModel();
	QSqlRecord * xRecord = new QSqlRecord();
	modelUpdatePidrozdily->setTable("pidrozdily");
	modelUpdatePidrozdily->setFilter("id=" + QVariant(idPidrozdil).toString());
	modelUpdatePidrozdily->select();
	if (modelUpdatePidrozdily->rowCount() == 1){
		*xRecord = modelUpdatePidrozdily->record(0);
		xRecord->setValue("Name", ui.lineEdit_name->text());
		if (ui.checkBox_Opal->isChecked())
			xRecord->setValue("Opal", 1);
		else
			xRecord->setValue("Opal", 0);
		xRecord->setValue("TypNarahCoBezLich", ui.comboBox_typ_narah_CO_bez_lich->currentIndex());
		if (ui.checkBox_Gar_voda->isChecked())
			xRecord->setValue("Gar_voda", 1);
		else
			xRecord->setValue("Gar_voda", 0);
		xRecord->setValue("Ploshcha", ui.doubleSpinBox_Ploshcha->value());
		xRecord->setValue("Obem", ui.doubleSpinBox_obem->value());
		xRecord->setValue("Tepl_harakt", ui.doubleSpinBox_tepl_harakter->value());
		xRecord->setValue("T_vnutr", ui.doubleSpinBox_t_vnutr->value());
		xRecord->setValue("Tepl_navantaj", ui.doubleSpinBox_Teplove_navantaj->value());
		if (ui.checkBox_Lichyknyk_e->isChecked())
			xRecord->setValue("Lich", 1);
		else
			xRecord->setValue("Lich", 0);
		xRecord->setValue("Kotelnya_ID", kotelniList.at(ui.comboBox_Kotelnya->currentIndex()));
		xRecord->setValue("Koef_lich", ui.doubleSpinBox_koef_lich->value());
	
		modelUpdatePidrozdily->setRecord(0, *xRecord);
		modelUpdatePidrozdily->submitAll();
	}
	delete modelUpdatePidrozdily;
	delete xRecord;
	
	accept();
}
//------------------------------------------------------------
tkeDialogEditPidrozdil::~tkeDialogEditPidrozdil()
{

}
//------------------------------------------------------------
