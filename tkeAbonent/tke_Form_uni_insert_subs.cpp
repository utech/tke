//------------------------------------------------------------
//
// Implamantation of tke_Form_uni_insert_subs class.
// Created by Roman Mulyk 08.07.2008
//
//------------------------------------------------------------

#include "UTkeGlobal.h"
#include "tke_Form_uni_insert_subs.h"

//------------------------------------------------------------
tke_Form_uni_insert_subs::tke_Form_uni_insert_subs(short year, short month,  QWidget * parent) 
	: QWidget(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	
	dbfScript = new UDbScriptAccess(this, ui.tableWidget_subsydii);
	
	ui.tableWidget_subsydii->setColumnHidden(0, true);
	ui.tableWidget_subsydii->setItemDelegateForColumn(1, new USpinBoxDelegate(-999999999, 999999999, ui.tableWidget_subsydii));
	ui.tableWidget_subsydii->setItemDelegateForColumn(2, new UDoubleSpinBoxDelegate(-999999999, 999999999, 2, ui.tableWidget_subsydii));
	ui.tableWidget_subsydii->setItemDelegateForColumn(3, new UDoubleSpinBoxDelegate(-999999999, 999999999, 2, ui.tableWidget_subsydii));
	ui.tableWidget_subsydii->setItemDelegateForColumn(4, new UDoubleSpinBoxDelegate(-999999999, 999999999, 2, ui.tableWidget_subsydii));
	ui.tableWidget_subsydii->setItemDelegateForColumn(5, new UDoubleSpinBoxDelegate(-999999999, 999999999, 2, ui.tableWidget_subsydii));
	ui.tableWidget_subsydii->resizeColumnsToContents();

	tableWidget_subsydii_cellChanged(0, 0);
	
	ui.dateEdit_date->setDate(QDate(year, month, 1));
	
	dirModel = new QDirModel();
    dirModel->setSorting(QDir::DirsFirst | QDir::IgnoreCase | QDir::Name);
	dirModel->setFilter(QDir::AllDirs | QDir::Drives | QDir::Files | QDir::NoDotAndDotDot);
	ui.treeView_fileMeneger->setModel(dirModel);
	ui.treeView_fileMeneger->header()->resizeSection(0, 300 );
	ui.treeView_fileMeneger->header()->resizeSection(1, 70 );
	ui.treeView_fileMeneger->header()->setSectionHidden(2, true );
	ui.treeView_fileMeneger->header()->setStretchLastSection(true);
    ui.treeView_fileMeneger->header()->setSortIndicator(0, Qt::AscendingOrder);
    ui.treeView_fileMeneger->header()->setSortIndicatorShown(true);
    ui.treeView_fileMeneger->header()->setClickable(true);
	
	toolButton_reloadScript_clicked();
	
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	QModelIndex ind = dirModel->index(settings->value("ImportUni/DefaultSubsPath").toString());
	delete settings;
	
	ui.treeView_fileMeneger->expand(ind);
	ui.treeView_fileMeneger->scrollTo(ind);
	ui.treeView_fileMeneger->setCurrentIndex(ind);
	
	connect(ui.treeView_fileMeneger, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(treeView_fileMeneger_doubleClicked(const QModelIndex &)));
	
	connect(ui.toolButton_addSubs, SIGNAL(clicked()), this, SLOT(toolButton_addSubs_clicked()));
	connect(ui.toolButton_dellSubs, SIGNAL(clicked()), this, SLOT(toolButton_dellSubs_clicked()));
	connect(ui.toolButton_insertSubsToDB, SIGNAL(clicked()), this, SLOT(toolButton_insertSubsToDB_clicked()));
	
	connect(ui.tableWidget_subsydii, SIGNAL(cellChanged(int,int)), this, SLOT(tableWidget_subsydii_cellChanged(int,int)));
	
	connect(ui.toolButton_reloadScript, SIGNAL(clicked()), this, SLOT(toolButton_reloadScript_clicked()));
	
	connect(ui.toolButton_reloadDirTree, SIGNAL(clicked()), this, SLOT(toolButton_reloadDirTree_clicked()));
}
//------------------------------------------------------------
void tke_Form_uni_insert_subs::treeView_fileMeneger_doubleClicked(const QModelIndex & index)
{
	ui.tableWidget_subsydii->blockSignals(true);
	
	if (dirModel->isDir(index))
		return;

	QScriptValue scrVal = dbfScript->engine.globalObject().property(subsInsertFunctionName);
	if (scrVal.isFunction()){
		QScriptValue result = scrVal.call(scrVal, QScriptValueList() << QScriptValue(&dbfScript->engine, dirModel->filePath(index)) );
		if (dbfScript->engine.hasUncaughtException()){
			int line = dbfScript->engine.uncaughtExceptionLineNumber();
			qDebug() << "Помилка виконання скрипта. Стрічка " << line << ":" << result.toString();
		}
	}
	else
		qDebug() << "<<<--- Програма не може запустити на виконання скрипт втягування субсидії, оскільки не може знайти функцію - " 
				<< subsInsertFunctionName << "\nПорада: Перевірте правильність скрипта втягування субсидій";
	tableWidget_subsydii_cellChanged(0, 2);
	
	ui.tableWidget_subsydii->blockSignals(false);
}
//------------------------------------------------------------
void tke_Form_uni_insert_subs::toolButton_addSubs_clicked()
{
	/*if	( QMessageBox::question(
                this,
                "!!! Додавання стрічки !!!",
                "Ви дійсно бажаєте додати стрічку?",
                "Так",
				"Ні-ні-ні",
                QString(), 0, 1)
		)
		return;*/
	
	int curRow = ui.tableWidget_subsydii->currentRow() + 1;
	ui.tableWidget_subsydii->insertRow(curRow);
	ui.tableWidget_subsydii->resizeRowToContents(curRow);
	
	dbfScript->populateTable(curRow, 0, "0", "#008000");
	dbfScript->populateTable(curRow, 1, "1", "#008000");
	dbfScript->populateTable(curRow, 2, "0,00", "#008000");
	dbfScript->populateTable(curRow, 3, "0,00", "#008000");
	dbfScript->populateTable(curRow, 4, "0,00", "#008000");
	dbfScript->populateTable(curRow, 5, "0,00", "#008000");
	
	tableWidget_subsydii_cellChanged(0, 2);
	
	ui.tableWidget_subsydii->setCurrentCell(curRow, 1);
	ui.tableWidget_subsydii->editItem(ui.tableWidget_subsydii->item(curRow, 1));
}
//------------------------------------------------------------
void tke_Form_uni_insert_subs::toolButton_dellSubs_clicked()
{
	if (ui.tableWidget_subsydii->rowCount() < 1)
		return;
	
	if	( QMessageBox::question(
                this,
                "!!! Видалення стрічки !!!",
                "Ви дійсно бажаєте видалити стрічку?",
                "Так",
				"Ні-ні-ні",
                QString(), 0, 1)
		)
		return;
	
	ui.tableWidget_subsydii->removeRow(ui.tableWidget_subsydii->currentRow());
	tableWidget_subsydii_cellChanged(0, 2);
}
//------------------------------------------------------------
void tke_Form_uni_insert_subs::toolButton_insertSubsToDB_clicked()
{
	rowCount = ui.tableWidget_subsydii->rowCount();
	if (rowCount < 1)
		return;
	
	if	( QMessageBox::question(
                this,
                "!!! Запис до бази даних !!!",
                "Ви дійсно бажаєте записати субсидії до бази даних?",
                "Так",
				"Ні-ні-ні",
                QString(), 0, 1)
		)
		return;
	
	UPostupForm *postup_form = new UPostupForm(0, rowCount );
	postup_form->show();
	
	QSqlQuery *query = new QSqlQuery();
	bool doneOk = true;
	QString packStr="";
	
	qDebug() << ">>>----- Внесення субсидій до бази даних ----->>>";
	
	for (int row=0; row<ui.tableWidget_subsydii->rowCount(); ++row){
		if (!query->exec("INSERT INTO subs (Rahunok_ID, Year, Month, Suma, Suma_d, Suma_GV, Suma_d_GV) VALUES ( "+
												ui.tableWidget_subsydii->item(row,1)->text()+", "
												+QString::number(ui.dateEdit_date->date().year())+", "
												+QString::number(ui.dateEdit_date->date().month())+", "
												+ui.tableWidget_subsydii->item(row,2)->text().replace(",",".")+", "
												+ui.tableWidget_subsydii->item(row,3)->text().replace(",",".")+", "
												+ui.tableWidget_subsydii->item(row,4)->text().replace(",",".")+", "
												+ui.tableWidget_subsydii->item(row,5)->text().replace(",",".")+")")){
			doneOk = false;
			qDebug() << "     ->> Не внесено субсидію абонента - " << ui.tableWidget_subsydii->item(row,1)->text()
					<< "\n Причина: " << query->lastError().text()
					<< "\nЗапит - " << query->lastQuery();
		}
		else{
			ui.tableWidget_subsydii->removeRow(row);
			--row;
		}
		postup_form->inc_pos();
	}
	delete query;
	delete postup_form;
	
	if (doneOk){
		qDebug() << "     ->> Внесення субсидій пройшло без помилок";
		QMessageBox::information(this,
				"!!! Внесення субсидій !!!",
				"Субсидії успішно внесено.",
				QMessageBox::Ok,
				QMessageBox::NoButton);
	}
	else{
		QMessageBox * mBox = new QMessageBox(QMessageBox::Critical, "!!! Невдале Внесення субсидій !!!",
									"Програма не змогла внести одну, декілька або усі субсидії.\n"
									"Невнесені субсидії залишились у таблиці"
									"Також, список невнесених субсидій Ви можете проглянути у вікні системних повідомлень."
									"\nЩоб відкрити його натисність 'OPEN'.");
		mBox->addButton("ОК", QMessageBox::ActionRole);
		QPushButton *debWindowButton = mBox->addButton("Вікно повідомлень", QMessageBox::ActionRole);
		mBox->exec();
		if (mBox->clickedButton() == debWindowButton)
			emit requestDebugWindow();
		delete mBox;
	}
	qDebug() << "<<<----- Закінчено Внесення субсидій до бази даних -----<<<";
	tableWidget_subsydii_cellChanged(0, 2);
}
//------------------------------------------------------------
void tke_Form_uni_insert_subs::tableWidget_subsydii_cellChanged(int row, int col)
{
	rowCount = ui.tableWidget_subsydii->rowCount();
	if (rowCount < 1){
		ui.label_SubsSum->setText("<b>ЦО</b>: Cума субсидії: Нарах. - 0,00 грн., Донарах. - 0,00 грн.");
		ui.label_SubsSum_GV->setText("<b>ГВ</b>: Cума субсидії: Нарах. - 0,00 грн., Донарах. - 0,00 грн.");
		ui.toolButton_dellSubs->setEnabled(false);
		ui.toolButton_insertSubsToDB->setEnabled(false);
		return;
	}
	ui.toolButton_dellSubs->setEnabled(true);
	ui.toolButton_insertSubsToDB->setEnabled(true);
	
	if (col==2 || col==3 || col==4 || col==5){
		double sum=0, sum_d=0, sum_GV=0, sum_d_GV=0;
		QTableWidgetItem *item;
		for (int row=0; row<rowCount; ++row){
			item = ui.tableWidget_subsydii->item(row,2);
			if (item)
				sum += item->text().toDouble();
			item = ui.tableWidget_subsydii->item(row,3);
			if (item)
				sum_d += item->text().toDouble();
			item = ui.tableWidget_subsydii->item(row,4);
			if (item)
				sum_GV += item->text().toDouble();
			item = ui.tableWidget_subsydii->item(row,5);
			if (item)
				sum_d_GV += item->text().toDouble();
		}
		ui.label_SubsSum->setText("<b>ЦО</b>: Cума субсидії: Нарах. - "+uMToStr2(sum)+" грн., Донарах. - "+uMToStr2(sum_d)+" грн.");
		ui.label_SubsSum_GV->setText("<b>ГВ</b>: Cума субсидії: Нарах. - "+uMToStr2(sum_GV)+" грн., Донарах. - "+uMToStr2(sum_d_GV)+" грн.");
	}
}
//------------------------------------------------------------
void tke_Form_uni_insert_subs::toolButton_reloadScript_clicked()
{
	dbfScript->setScriptEngine("./tke.ini", "Script/SubsScriptFilePath");
	QScriptValue scrVal = dbfScript->engine.globalObject().property(FILETYPES_SCR_STR);
	if (scrVal.isString())
		dirModel->setNameFilters(QStringList() << scrVal.toString());
	else
		dirModel->setNameFilters(QStringList() << "*");
	scrVal = dbfScript->engine.globalObject().property(FUNCTIONNAMES_SCR_STR);
	if (scrVal.isString())
		subsInsertFunctionName =  scrVal.toString();
	else
		subsInsertFunctionName = "";
	setWindowTitle("Імпортер нарах. субсидій. Скрипт -  "+dbfScript->readScriptOrgName());
	
	tableWidget_subsydii_cellChanged(0, 2);
}
//------------------------------------------------------------
void tke_Form_uni_insert_subs::toolButton_reloadDirTree_clicked()
{
	dirModel->refresh();
}
//------------------------------------------------------------
tke_Form_uni_insert_subs::~tke_Form_uni_insert_subs()
{
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	settings->beginGroup("ImportUni");
	settings->setValue("DefaultSubsPath", QFileInfo(dirModel->filePath(ui.treeView_fileMeneger->currentIndex())).absolutePath());
	delete settings;
}
//------------------------------------------------------------
