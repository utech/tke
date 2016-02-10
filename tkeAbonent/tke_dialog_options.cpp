//------------------------------------------------------------
//
// Implamantation of tke_dialog_options class.
// Created by Roman Mulyk 26.06.2008
//
//------------------------------------------------------------

#include <QFileDialog>
#include <USqlAccessible>
#include "../tkeOrganizacii/udiff_func.h"
#include "tke_dialog_options.h"

//------------------------------------------------------------
tke_dialog_options::tke_dialog_options(QWidget *parent)
        : QDialog(parent)
{
    ui.setupUi(this);
	buttonGroup = new QButtonGroup();
	buttonGroup->addButton(ui.toolButton_tkeAbonentOptions,0);
	buttonGroup->addButton(ui.toolButton_printKvytanciiOptions,1);
	buttonGroup->addButton(ui.toolButton_tkeOrganizaciiOptions,2);
	buttonGroup->addButton(ui.toolButton_importData,3);
	buttonGroup->addButton(ui.toolButton_dataBase,4);
	buttonGroup->addButton(ui.toolButton_exportPilg,5);
	
	ui.toolButton_tkeAbonentOptions->setChecked(true);
	ui.tabWidget_Options->setCurrentIndex(0);
	
	ui.lineEdit_infoText3Str->setMaxLength(75);
	ui.lineEdit_infoText4Str->setMaxLength(75);
	
    // Обов'язкова плата по субсидії
    ui.comboBox_obovPlataSubs->addItem("Не використовувати обов'язкової оплати по субсидії"); // 0
    ui.comboBox_obovPlataSubs->addItem("Використовувати автоматичний розрахунок субсидії"); // 1
    ui.comboBox_obovPlataSubs->addItem("Друкувати в оплаті значення Об.пл. по субс."); // 2

	populateOptions();
		//Connectors
	connect(ui.pushButton_accept, SIGNAL(clicked()), this, SLOT(pushButton_accept_clicked()));
	connect(ui.pushButton_cancel, SIGNAL(clicked()), this, SLOT(pushButton_cancel_clicked()));
	connect(ui.pushButton_zastosuvaty, SIGNAL(clicked()), this, SLOT(pushButton_zastosuvaty_clicked()));
	connect(ui.toolButton_pochatkoviZnachennya, SIGNAL(clicked()), this, SLOT(toolButton_pochatkoviZnachennya_clicked()));
	connect(buttonGroup, SIGNAL(buttonClicked(int)), ui.tabWidget_Options, SLOT(setCurrentIndex(int)));
	connect(ui.tabWidget_Options, SIGNAL(currentChanged(int)), this, SLOT(tabWidget_Options_currentChanged(int)));
	connect(ui.toolButton_oldValues, SIGNAL(clicked()), this, SLOT(toolButton_oldValues_clicked()));
	connect(ui.toolButton_findKvytScript, SIGNAL(clicked()), this, SLOT(toolButton_findKvytScript_clicked()));
	connect(ui.toolButton_findSubsScript, SIGNAL(clicked()), this, SLOT(toolButton_findSubsScript_clicked()));
	connect(ui.toolButton_arjDatabasePathFind, SIGNAL(clicked()), this, SLOT(toolButton_arjDatabasePathFind_clicked()));
}
//------------------------------------------------------------
void tke_dialog_options::connectEditres()
{
	connect(ui.checkBox_showPilgsDateLimits, SIGNAL(clicked()), this, SLOT(optionsEdited()));
	connect(ui.checkBox_showNarahCO, SIGNAL(clicked()), this, SLOT(optionsEdited()));
	connect(ui.checkBox_showNarahGV, SIGNAL(clicked()), this, SLOT(optionsEdited()));
	connect(ui.checkBox_showNarahST, SIGNAL(clicked()), this, SLOT(optionsEdited()));
	connect(ui.checkBox_showPerevedZaborg, SIGNAL(clicked()), this, SLOT(optionsEdited()));
	connect(ui.checkBox_showBeznadDebitZaborg, SIGNAL(clicked()), this, SLOT(optionsEdited()));
	connect(ui.checkBox_addBeznadiynaDebZaborgVGolovnKvyt, SIGNAL(clicked()), this, SLOT(optionsEdited()));
	connect(ui.spinBox_recomendedSplataDate, SIGNAL(valueChanged(int)), this, SLOT(optionsEdited()));
	connect(ui.lineEdit_infoText2Str, SIGNAL(textEdited(const QString &)), this, SLOT(optionsEdited()));
	connect(ui.lineEdit_infoText3Str, SIGNAL(textEdited(const QString &)), this, SLOT(optionsEdited()));
	connect(ui.lineEdit_infoText4Str, SIGNAL(textEdited(const QString &)), this, SLOT(optionsEdited()));
	connect(ui.lineEdit_importKvyt, SIGNAL(textChanged(const QString &)), this, SLOT(optionsEdited()));
	connect(ui.lineEdit_importSubs, SIGNAL(textChanged(const QString &)), this, SLOT(optionsEdited()));
	connect(ui.comboBox_organiz_rahunok_type, SIGNAL(currentIndexChanged(int)), this, SLOT(optionsEdited()));
	connect(ui.lineEdit_arjDatabasePath, SIGNAL(textChanged(const QString &)), this, SLOT(optionsEdited()));
	connect(ui.checkBox_printPokaznBudLichCO, SIGNAL(clicked()), this, SLOT(optionsEdited()));
	connect(ui.checkBox_printPokaznykOfType, SIGNAL(clicked()), this, SLOT(optionsEdited()));
	connect(ui.checkBox_zvitPilg_printPidpysyForAllKategor, SIGNAL(clicked()), this, SLOT(optionsEdited()));
	connect(ui.checkBox_saveInformationOnNarahCO, SIGNAL(clicked()), this, SLOT(optionsEdited()));
	connect(ui.spinBox_tochnistDiuchTaryf, SIGNAL(valueChanged(int)), this, SLOT(optionsEdited()));
	connect(ui.checkBox_printZeroNegativeKvyt, SIGNAL(clicked()), this, SLOT(optionsEdited()));
    connect(ui.comboBox_obovPlataSubs, SIGNAL(currentIndexChanged(int)), this, SLOT(optionsEdited()));
    connect(ui.checkBox_checkObovyazkPlataPoSubsAndNarah, SIGNAL(clicked()), this, SLOT(optionsEdited()));
	connect(ui.checkBox_printBankWiringInstructions, SIGNAL(clicked()), this, SLOT(optionsEdited()));
	connect(ui.plainTextEdit_bankWiringInstructions, SIGNAL(textChanged()), this, SLOT(optionsEdited()));
	connect(ui.spinBox_numberToAddToRahunok, SIGNAL(valueChanged(int)), this, SLOT(optionsEdited()));
	connect(ui.lineEdit_postDovidnykEMail, SIGNAL(textChanged(const QString &)), this, SLOT(optionsEdited()));
	connect(ui.lineEdit_kodPilgPoCO, SIGNAL(textEdited(const QString &)), this, SLOT(optionsEdited()));
	connect(ui.lineEdit_kodPilgPoGV, SIGNAL(textEdited(const QString &)), this, SLOT(optionsEdited()));
	connect(ui.lineEdit_opRahunok, SIGNAL(textEdited(const QString &)), this, SLOT(optionsEdited()));
	connect(ui.lineEdit_opCO, SIGNAL(textEdited(const QString &)), this, SLOT(optionsEdited()));
	connect(ui.lineEdit_opGV, SIGNAL(textEdited(const QString &)), this, SLOT(optionsEdited()));
	connect(ui.radioButton_CO_exportPilgUsingDiuchTaryf, SIGNAL(toggled(bool)), this, SLOT(optionsEdited()));
	connect(ui.radioButton_CO_exportPilgUsingGKal, SIGNAL(toggled(bool)), this, SLOT(optionsEdited()));
	connect(ui.radioButton_pilgyAllInOneFile, SIGNAL(toggled(bool)), this, SLOT(optionsEdited()));
	connect(ui.radioButton_pilgyAllInOneFileWithRozd, SIGNAL(toggled(bool)), this, SLOT(optionsEdited()));
	connect(ui.radioButton_PilgsInDifferentFiles, SIGNAL(toggled(bool)), this, SLOT(optionsEdited()));
}
//------------------------------------------------------------
void tke_dialog_options::disconnectEditres()
{
	disconnect(ui.checkBox_showPilgsDateLimits, SIGNAL(clicked()), this, SLOT(optionsEdited()));
	disconnect(ui.checkBox_showNarahCO, SIGNAL(clicked()), this, SLOT(optionsEdited()));
	disconnect(ui.checkBox_showNarahGV, SIGNAL(clicked()), this, SLOT(optionsEdited()));
	disconnect(ui.checkBox_showNarahST, SIGNAL(clicked()), this, SLOT(optionsEdited()));
	disconnect(ui.checkBox_showPerevedZaborg, SIGNAL(clicked()), this, SLOT(optionsEdited()));
	disconnect(ui.checkBox_showBeznadDebitZaborg, SIGNAL(clicked()), this, SLOT(optionsEdited()));
	disconnect(ui.checkBox_addBeznadiynaDebZaborgVGolovnKvyt, SIGNAL(clicked()), this, SLOT(optionsEdited()));
	disconnect(ui.spinBox_recomendedSplataDate, SIGNAL(valueChanged(int)), this, SLOT(optionsEdited()));
	disconnect(ui.lineEdit_infoText2Str, SIGNAL(textEdited(const QString &)), this, SLOT(optionsEdited()));
	disconnect(ui.lineEdit_infoText3Str, SIGNAL(textEdited(const QString &)), this, SLOT(optionsEdited()));
	disconnect(ui.lineEdit_infoText4Str, SIGNAL(textEdited(const QString &)), this, SLOT(optionsEdited()));
	disconnect(ui.lineEdit_importKvyt, SIGNAL(textChanged(const QString &)), this, SLOT(optionsEdited()));
	disconnect(ui.lineEdit_importSubs, SIGNAL(textChanged(const QString &)), this, SLOT(optionsEdited()));
	disconnect(ui.lineEdit_arjDatabasePath, SIGNAL(textChanged(const QString &)), this, SLOT(optionsEdited()));
	disconnect(ui.checkBox_printPokaznBudLichCO, SIGNAL(clicked()), this, SLOT(optionsEdited()));
	disconnect(ui.checkBox_printPokaznykOfType, SIGNAL(clicked()), this, SLOT(optionsEdited()));
	disconnect(ui.checkBox_zvitPilg_printPidpysyForAllKategor, SIGNAL(clicked()), this, SLOT(optionsEdited()));
	disconnect(ui.checkBox_saveInformationOnNarahCO, SIGNAL(clicked()), this, SLOT(optionsEdited()));
	disconnect(ui.spinBox_tochnistDiuchTaryf, SIGNAL(valueChanged(int)), this, SLOT(optionsEdited()));
	disconnect(ui.checkBox_printZeroNegativeKvyt, SIGNAL(clicked()), this, SLOT(optionsEdited()));
    disconnect(ui.comboBox_obovPlataSubs, SIGNAL(currentIndexChanged(int)), this, SLOT(optionsEdited()));
    disconnect(ui.checkBox_checkObovyazkPlataPoSubsAndNarah, SIGNAL(clicked()), this, SLOT(optionsEdited()));
    disconnect(ui.checkBox_printBankWiringInstructions, SIGNAL(clicked()), this, SLOT(optionsEdited()));
	disconnect(ui.plainTextEdit_bankWiringInstructions, SIGNAL(textChanged()), this, SLOT(optionsEdited()));
	disconnect(ui.spinBox_numberToAddToRahunok, SIGNAL(valueChanged(int)), this, SLOT(optionsEdited()));
	disconnect(ui.lineEdit_postDovidnykEMail, SIGNAL(textChanged(const QString &)), this, SLOT(optionsEdited()));
	disconnect(ui.lineEdit_kodPilgPoCO, SIGNAL(textEdited(const QString &)), this, SLOT(optionsEdited()));
	disconnect(ui.lineEdit_kodPilgPoGV, SIGNAL(textEdited(const QString &)), this, SLOT(optionsEdited()));
	disconnect(ui.lineEdit_opRahunok, SIGNAL(textEdited(const QString &)), this, SLOT(optionsEdited()));
	disconnect(ui.lineEdit_opCO, SIGNAL(textEdited(const QString &)), this, SLOT(optionsEdited()));
	disconnect(ui.lineEdit_opGV, SIGNAL(textEdited(const QString &)), this, SLOT(optionsEdited()));
	disconnect(ui.radioButton_CO_exportPilgUsingDiuchTaryf, SIGNAL(toggled(bool)), this, SLOT(optionsEdited()));
	disconnect(ui.radioButton_CO_exportPilgUsingGKal, SIGNAL(toggled(bool)), this, SLOT(optionsEdited()));
	disconnect(ui.radioButton_pilgyAllInOneFile, SIGNAL(toggled(bool)), this, SLOT(optionsEdited()));
	disconnect(ui.radioButton_pilgyAllInOneFileWithRozd, SIGNAL(toggled(bool)), this, SLOT(optionsEdited()));
	disconnect(ui.radioButton_PilgsInDifferentFiles, SIGNAL(toggled(bool)), this, SLOT(optionsEdited()));
}
//------------------------------------------------------------
void tke_dialog_options::populateOptions()
{
	disconnectEditres();
	
	QSqlQuery *query = new QSqlQuery();
	int iVal;
	
	ui.pushButton_zastosuvaty->setEnabled(false);
	ui.toolButton_oldValues->setEnabled(false);
	
	//Налаштування GUI з іні-файлу
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	// Розділ ПК ТКЕнерго менеджер::абонент
	ui.checkBox_showPilgsDateLimits->setChecked(settings->value("GUI_View/PilgsDatesLimitsVisible").toBool());
	ui.checkBox_showNarahCO->setChecked(settings->value("GUI_View/ShowNarahCO").toBool());
	ui.checkBox_showNarahGV->setChecked(settings->value("GUI_View/ShowNarahGV").toBool());
	ui.checkBox_showNarahST->setChecked(settings->value("GUI_View/ShowNarahST").toBool());
	ui.checkBox_showPerevedZaborg->setChecked(settings->value("GUI_View/ShowPerevedZaborg").toBool());
	ui.checkBox_showBeznadDebitZaborg->setChecked(settings->value("GUI_View/ShowBeznadDebitZaborg").toBool());
	ui.checkBox_saveInformationOnNarahCO->setChecked(settings->value("NarahOptions/SaveInformationOnNarahCO").toBool());
	query->exec("SELECT value FROM slujb_znach WHERE id=8");
	query->seek(0);
	iVal = query->value(0).toDouble();
	if (iVal < 2) iVal = 2;
	if (iVal > 6) iVal = 6;
	ui.spinBox_tochnistDiuchTaryf->setValue(iVal);
	
	query->exec("SELECT strTemplate FROM slujb_znach WHERE id=7");
	query->seek(0);
	ui.checkBox_zvitPilg_printPidpysyForAllKategor->setChecked(query->value(0).toBool());
	
	query->exec("SELECT [value] FROM slujb_znach WHERE id=11");
	query->seek(0);
	ui.spinBox_numberToAddToRahunok->setValue(query->value(0).toDouble());
	
	query->exec("SELECT strTemplate FROM slujb_znach WHERE id=12");
	query->seek(0);
	ui.lineEdit_postDovidnykEMail->setText(query->value(0).toString());
	
	query->exec("SELECT strTemplate FROM slujb_znach WHERE id=13");
	query->seek(0);
	ui.lineEdit_kodPilgPoCO->setText(query->value(0).toString());
	
	query->exec("SELECT strTemplate FROM slujb_znach WHERE id=14");
	query->seek(0);
	ui.lineEdit_kodPilgPoGV->setText(query->value(0).toString());
	
	
	// Розділ ПК ТКЕнерго менеджер::організації
	ui.comboBox_organiz_rahunok_type->setCurrentIndex(settings->value("PrintDocs/OrganizRahunokType").toInt()-1);
	
	//Розділ друк квитанцій
	query->exec("SELECT [value] FROM slujb_znach WHERE id=4");
	if (query->seek(0))
		ui.spinBox_recomendedSplataDate->setValue(query->value(0).toDouble());
	
	query->exec("SELECT strTemplate FROM slujb_znach WHERE id=2");
	if (query->seek(0))
		ui.lineEdit_infoText2Str->setText(query->value(0).toString());
	else
		ui.lineEdit_infoText2Str->setText("");
	
	query->exec("SELECT strTemplate FROM slujb_znach WHERE id=3");
	if (query->seek(0))
		ui.lineEdit_infoText3Str->setText(query->value(0).toString());
	else
		ui.lineEdit_infoText3Str->setText("");
	
	query->exec("SELECT strTemplate FROM slujb_znach WHERE id=23");
	if (query->seek(0))
		ui.lineEdit_infoText4Str->setText(query->value(0).toString());
	else
		ui.lineEdit_infoText4Str->setText("");
	
	query->exec("SELECT strTemplate FROM slujb_znach WHERE id=6");
	query->seek(0);
	ui.checkBox_printPokaznBudLichCO->setChecked(query->value(0).toBool());
	
	query->exec("SELECT strTemplate FROM slujb_znach WHERE id=20");
	query->seek(0);
	ui.checkBox_printPokaznykOfType->setChecked(query->value(0).toBool());
	
	query->exec("SELECT strTemplate FROM slujb_znach WHERE id=9");
	query->seek(0);
	ui.checkBox_printZeroNegativeKvyt->setChecked(query->value(0).toBool());
	
    query->exec("SELECT strTemplate FROM slujb_znach WHERE id=10");
    query->seek(0);
    int comboBoxIndex = query->value(0).toInt();
    ui.comboBox_obovPlataSubs->setCurrentIndex(comboBoxIndex > ui.comboBox_obovPlataSubs->count() - 1 ? 0 : comboBoxIndex);

    query->exec("SELECT strTemplate FROM slujb_znach WHERE id=22");
	query->seek(0);
    ui.checkBox_checkObovyazkPlataPoSubsAndNarah->setChecked(query->value(0).toBool());
	
	query->exec("SELECT strTemplate FROM slujb_znach WHERE id=18");
	query->seek(0);
	ui.checkBox_addBeznadiynaDebZaborgVGolovnKvyt->setChecked(query->value(0).toBool());
	
	query->exec("SELECT strTemplate FROM slujb_znach WHERE id=24");
	query->seek(0);
	ui.checkBox_printBankWiringInstructions->setChecked(query->value(0).toBool());
	
	query->exec("SELECT strTemplate FROM slujb_znach WHERE id=25");
	query->seek(0);
	ui.plainTextEdit_bankWiringInstructions->setPlainText(query->value(0).toString());
	
	//Розділ шляхів до файлів скриптів
	ui.lineEdit_importKvyt->setText(settings->value("Script/KvytScriptFilePath").toString());
	ui.lineEdit_importSubs->setText(settings->value("Script/SubsScriptFilePath").toString());
	
	query->exec("SELECT strTemplate FROM slujb_znach WHERE id=15");
	if (query->seek(0))
		ui.lineEdit_opRahunok->setText(query->value(0).toString());
	else
		ui.lineEdit_opRahunok->setText("");
		
	query->exec("SELECT strTemplate FROM slujb_znach WHERE id=16");
	if (query->seek(0))
		ui.lineEdit_opCO->setText(query->value(0).toString());
	else
		ui.lineEdit_opCO->setText("");
	
	query->exec("SELECT strTemplate FROM slujb_znach WHERE id=17");
	if (query->seek(0))
		ui.lineEdit_opGV->setText(query->value(0).toString());
	else
		ui.lineEdit_opGV->setText("");
	
	//Розділ бази даних
	ui.lineEdit_arjDatabasePath->setText(settings->value("dataBase/savePath").toString().replace("/","\\"));
	
	//Розділ формування звіту по пільгах
	query->exec("SELECT [value] FROM slujb_znach WHERE id=19");
	query->next();
	switch (qRound(query->value(0).toDouble())){
		case 0:
			ui.radioButton_CO_exportPilgUsingDiuchTaryf->setChecked(true);
			break;
		case 1:
			ui.radioButton_CO_exportPilgUsingGKal->setChecked(true);
			break;
		default:
			ui.radioButton_CO_exportPilgUsingDiuchTaryf->setChecked(true);
			query->exec("UPDATE slujb_znach SET [value]=0 WHERE id=19");
	}
	
	//Розділ формування звіту по пільгах
	query->exec("SELECT [value] FROM slujb_znach WHERE id=21");
	query->next();
	switch (qRound(query->value(0).toDouble())){
		case 0:
			ui.radioButton_pilgyAllInOneFile->setChecked(true);
			break;
		case 1:
			ui.radioButton_pilgyAllInOneFileWithRozd->setChecked(true);
			break;
		case 2:
			ui.radioButton_PilgsInDifferentFiles->setChecked(true);
			break;
		default:
			ui.radioButton_pilgyAllInOneFile->setChecked(true);
			query->exec("UPDATE slujb_znach SET [value]=0 WHERE id=21");
	}
	
	//query->exec("SELECT strTemplate FROM slujb_znach WHERE id=21");
	//query->seek(0);
	//ui.checkBox_exportPilgMarkingWithEmptyStrings->setChecked(query->value(0).toBool());
	
	delete query;
	delete settings;
	
	connectEditres();
}
//------------------------------------------------------------
void tke_dialog_options::saveOptions()
{
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	QSqlQuery *query = new QSqlQuery();
	int iVal;
	
	// Розділ ПК ТКЕнерго менеджер::абонент
	settings->beginGroup("GUI_View");
	settings->setValue("PilgsDatesLimitsVisible", ui.checkBox_showPilgsDateLimits->isChecked());
	settings->setValue("ShowNarahCO", ui.checkBox_showNarahCO->isChecked());
	settings->setValue("ShowNarahGV", ui.checkBox_showNarahGV->isChecked());
	settings->setValue("ShowNarahST", ui.checkBox_showNarahST->isChecked());
	settings->setValue("ShowPerevedZaborg", ui.checkBox_showPerevedZaborg->isChecked());
	settings->setValue("ShowBeznadDebitZaborg", ui.checkBox_showBeznadDebitZaborg->isChecked());
	settings->endGroup();
	settings->beginGroup("NarahOptions");
	settings->setValue("SaveInformationOnNarahCO", ui.checkBox_saveInformationOnNarahCO->isChecked());
	settings->endGroup();
	query->exec("UPDATE slujb_znach SET [value]='"+sqlStr(ui.spinBox_tochnistDiuchTaryf->value())+"' WHERE id=8");
	query->exec("UPDATE slujb_znach SET strTemplate='"+sqlStr(ui.checkBox_zvitPilg_printPidpysyForAllKategor->isChecked())+"' WHERE id=7");
	
	query->exec("UPDATE slujb_znach SET [value]='"+sqlStr(ui.spinBox_numberToAddToRahunok->value())+"' WHERE id=11");
	query->exec("UPDATE slujb_znach SET strTemplate="+sqlStr(ui.lineEdit_postDovidnykEMail->text())+" WHERE id=12");
	
	query->exec("UPDATE slujb_znach SET strTemplate="+sqlStr(ui.lineEdit_kodPilgPoCO->text())+" WHERE id=13");
	query->exec("UPDATE slujb_znach SET strTemplate="+sqlStr(ui.lineEdit_kodPilgPoGV->text())+" WHERE id=14");
	
	// Розділ ПК ТКЕнерго менеджер::організації
	settings->beginGroup("PrintDocs");
	settings->setValue("OrganizRahunokType", ui.comboBox_organiz_rahunok_type->currentIndex()+1);
	settings->endGroup();
	
	//Розділ шляхів до файлів скриптів
	settings->beginGroup("Script");
	settings->setValue("KvytScriptFilePath", ui.lineEdit_importKvyt->text());
	settings->setValue("SubsScriptFilePath", ui.lineEdit_importSubs->text());
	settings->endGroup();
	query->exec("UPDATE slujb_znach SET strTemplate="+sqlStr(ui.lineEdit_opRahunok->text())+" WHERE id=15");
	query->exec("UPDATE slujb_znach SET strTemplate="+sqlStr(ui.lineEdit_opCO->text())+" WHERE id=16");
	query->exec("UPDATE slujb_znach SET strTemplate="+sqlStr(ui.lineEdit_opGV->text())+" WHERE id=17");
	
	
	//Розділ друк квитанцій
	query->exec("UPDATE slujb_znach SET [value]="+ui.spinBox_recomendedSplataDate->text()+" WHERE id=4");
	query->exec("UPDATE slujb_znach SET strTemplate='"+ui.lineEdit_infoText2Str->text()+"' WHERE id=2");
	query->exec("UPDATE slujb_znach SET strTemplate='"+ui.lineEdit_infoText3Str->text()+"' WHERE id=3");
	query->exec("UPDATE slujb_znach SET strTemplate='"+ui.lineEdit_infoText4Str->text()+"' WHERE id=23");
	query->exec("UPDATE slujb_znach SET strTemplate='"+sqlStr(ui.checkBox_printPokaznBudLichCO->isChecked())+"' WHERE id=6");
	query->exec("UPDATE slujb_znach SET strTemplate='"+sqlStr(ui.checkBox_printPokaznykOfType->isChecked())+"' WHERE id=20");
	query->exec("UPDATE slujb_znach SET strTemplate='"+sqlStr(ui.checkBox_printZeroNegativeKvyt->isChecked())+"' WHERE id=9");
    query->exec("UPDATE slujb_znach SET strTemplate='"+sqlStr(ui.comboBox_obovPlataSubs->currentIndex())+"' WHERE id=10");
    query->exec("UPDATE slujb_znach SET strTemplate='"+sqlStr(ui.checkBox_checkObovyazkPlataPoSubsAndNarah->isChecked())+"' WHERE id=22");
	query->exec("UPDATE slujb_znach SET strTemplate='"+sqlStr(ui.checkBox_addBeznadiynaDebZaborgVGolovnKvyt->isChecked())+"' WHERE id=18");
	query->exec("UPDATE slujb_znach SET strTemplate='"+sqlStr(ui.checkBox_printBankWiringInstructions->isChecked())+"' WHERE id=24");
	query->exec("UPDATE slujb_znach SET strTemplate="+sqlStr(ui.plainTextEdit_bankWiringInstructions->toPlainText().replace('\'',"\'\'"))+" WHERE id=25");
	
	//Розділ бази даних
	settings->beginGroup("dataBase");
	settings->setValue("savePath", ui.lineEdit_arjDatabasePath->text().replace("\\","/"));
	settings->endGroup();
	
	//Розділ формування звіту по пільгах
	if (ui.radioButton_CO_exportPilgUsingDiuchTaryf->isChecked())
		iVal = 0;
	else if (ui.radioButton_CO_exportPilgUsingGKal->isChecked())
		iVal = 1;
	else
		iVal = 0;
	query->exec("UPDATE slujb_znach SET [value]="+sqlStr(iVal)+" WHERE id=19");
	
	if (ui.radioButton_pilgyAllInOneFile->isChecked())
		iVal = 0;
	else if (ui.radioButton_pilgyAllInOneFileWithRozd->isChecked())
		iVal = 1;
	else if (ui.radioButton_PilgsInDifferentFiles->isChecked())
		iVal = 2;
	else
		iVal = 0;
	query->exec("UPDATE slujb_znach SET [value]="+sqlStr(iVal)+" WHERE id=21");
	
	//query->exec("UPDATE slujb_znach SET strTemplate='"+sqlStr(ui.checkBox_exportPilgMarkingWithEmptyStrings->isChecked())+"' WHERE id=21");
	
	
	delete query;
	delete settings;
	
	ui.toolButton_oldValues->setEnabled(false);
	ui.pushButton_zastosuvaty->setEnabled(false);
	
	populateOptions();
}
//------------------------------------------------------------
void tke_dialog_options::pushButton_accept_clicked()
{
	saveOptions();
	emit requestReloadIniSettings();
	accept();
}
//------------------------------------------------------------
void tke_dialog_options::pushButton_cancel_clicked()
{
	reject();
}
//------------------------------------------------------------
void tke_dialog_options::pushButton_zastosuvaty_clicked()
{
	saveOptions();
	emit requestReloadIniSettings();
}
//------------------------------------------------------------
void tke_dialog_options::optionsEdited()
{
	ui.toolButton_pochatkoviZnachennya->setEnabled(true);
	ui.toolButton_oldValues->setEnabled(true);
	ui.pushButton_zastosuvaty->setEnabled(true);
}
//------------------------------------------------------------
void tke_dialog_options::toolButton_pochatkoviZnachennya_clicked()
{
	disconnectEditres();
	
	ui.lineEdit_infoText2Str->setText("м.Ваше Місто, вул.Ваша Вулиця ##. Телефон для довідок Х-ХХ-ХХ");
	ui.lineEdit_infoText3Str->setText("Абоненти, які отримують субсидію, оплачують згідно нарах. відділу субсидії.");
	ui.lineEdit_infoText4Str->setText("Абоненти, які отримують субсидію, оплачують згідно нарах. відділу субсидії.");
	
	connectEditres();
}
//------------------------------------------------------------
void tke_dialog_options::tabWidget_Options_currentChanged(int index)
{
	disconnect(buttonGroup, SIGNAL(buttonClicked(int)), ui.tabWidget_Options, SLOT(setCurrentIndex(int)));
	
	buttonGroup->button(index)->setChecked(true);
	
	connect(buttonGroup, SIGNAL(buttonClicked(int)), ui.tabWidget_Options, SLOT(setCurrentIndex(int)));
}
//------------------------------------------------------------
void tke_dialog_options::toolButton_oldValues_clicked()
{
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT value FROM slujb_znach WHERE id=4");
	if (query->seek(0))
		ui.spinBox_recomendedSplataDate->setValue(query->value(0).toInt());
	
	query->exec("SELECT strTemplate FROM slujb_znach WHERE id=2");
	if (query->seek(0))
		ui.lineEdit_infoText2Str->setText(query->value(0).toString());
	else
		ui.lineEdit_infoText2Str->setText("");
	
	query->exec("SELECT strTemplate FROM slujb_znach WHERE id=3");
	if (query->seek(0))
		ui.lineEdit_infoText3Str->setText(query->value(0).toString());
	else
		ui.lineEdit_infoText3Str->setText("");
	
	query->exec("SELECT strTemplate FROM slujb_znach WHERE id=23");
	if (query->seek(0))
		ui.lineEdit_infoText4Str->setText(query->value(0).toString());
	else
		ui.lineEdit_infoText4Str->setText("");
	
	delete query;
	
	ui.toolButton_oldValues->setEnabled(false);
}
//------------------------------------------------------------
void tke_dialog_options::toolButton_findKvytScript_clicked()
{
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QString curPath = ui.lineEdit_importKvyt->text();
	if (curPath.size() == 0 || !QFile::exists(curPath))
		curPath = "../script/";
	QString fileName = QFileDialog::getOpenFileName(this,
				"Пошук файла скрипта втягування квитанцій", curPath, "Файли скриптів (*.js *.tis)");
	if (fileName.size() == 0)
		return;
	QDir dir(QCoreApplication::applicationDirPath());
	ui.lineEdit_importKvyt->setText(dir.relativeFilePath(fileName));
}
//------------------------------------------------------------
void tke_dialog_options::toolButton_findSubsScript_clicked()
{
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QString curPath = ui.lineEdit_importSubs->text();
	if (curPath.size() == 0 || !QFile::exists(curPath))
		curPath = "../script/";
	QString fileName = QFileDialog::getOpenFileName(this,
				"Пошук файла скрипта втягування субсидій", curPath, "Файли скриптів (*.js *.tis)");
	if (fileName.size() == 0)
		return;
	QDir dir(QCoreApplication::applicationDirPath());
	ui.lineEdit_importSubs->setText(dir.relativeFilePath(fileName));
}
//------------------------------------------------------------
void tke_dialog_options::toolButton_arjDatabasePathFind_clicked()
{
	QString path = QFileDialog::getExistingDirectory(this, "Папка з архівами бази даних ТКЕнерго менеджера",
                                                 ui.lineEdit_arjDatabasePath->text().replace("/","\\"));
	if (path.size()>0)
		ui.lineEdit_arjDatabasePath->setText(path.replace("/","\\"));
}
//------------------------------------------------------------
tke_dialog_options::~tke_dialog_options()
{

}
//------------------------------------------------------------
