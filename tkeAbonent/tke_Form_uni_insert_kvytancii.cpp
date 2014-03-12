//------------------------------------------------------------
//
// Implamantation of tke_Form_uni_insert_kvytancii class.
// Created by Roman Mulyk 30.06.2008
//
//------------------------------------------------------------

#include "UTkeGlobal.h"
#include "tke_Form_uni_insert_kvytancii.h"

//------------------------------------------------------------
tke_Form_uni_insert_kvytancii::tke_Form_uni_insert_kvytancii(short year, short month,  QWidget * parent) 
	: QWidget(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	
	dbfScript = new UDbScriptAccess(this, ui.tableWidget_kvytancii);
	
	ui.tableWidget_kvytancii->setColumnHidden(0, true);
	ui.tableWidget_kvytancii->setItemDelegateForColumn(1, new USpinBoxDelegate(-999999999, 999999999, ui.tableWidget_kvytancii));
	ui.tableWidget_kvytancii->setItemDelegateForColumn(2, new UDateEditDelegate(QDate(2000,1,1), QDate(2100,12,12), ui.tableWidget_kvytancii));
	ui.tableWidget_kvytancii->setItemDelegateForColumn(3, new UDoubleSpinBoxDelegate(-999999999, 999999999, 2, ui.tableWidget_kvytancii));
	ui.tableWidget_kvytancii->setItemDelegateForColumn(5, new USpinBoxDelegate(0, 999999999, ui.tableWidget_kvytancii));
	ui.tableWidget_kvytancii->resizeColumnsToContents();

	tableWidget_kvytancii_cellChanged(0, 0);
	
	curYear = year;
	curMonth = month;
	curOplataDate = QDate::currentDate();
	curKasaString = "";
	
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
	QModelIndex ind = dirModel->index(settings->value("ImportUni/DefaultKvytPath").toString());
	delete settings;
	ui.treeView_fileMeneger->expand(ind);
	ui.treeView_fileMeneger->scrollTo(ind);
	ui.treeView_fileMeneger->setCurrentIndex(ind);
	
	connect(ui.comboBox_vybirBanku, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBox_vybirBanku_currentIndexChanged(int)));
	connect(ui.treeView_fileMeneger, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(treeView_fileMeneger_doubleClicked(const QModelIndex &)));
	
	connect(ui.toolButton_addKvyt, SIGNAL(clicked()), this, SLOT(toolButton_addKvyt_clicked()));
	connect(ui.toolButton_dellKvyt, SIGNAL(clicked()), this, SLOT(toolButton_dellKvyt_clicked()));
	connect(ui.toolButton_insertKvytToDB, SIGNAL(clicked()), this, SLOT(toolButton_insertKvytToDB_clicked()));
	connect(ui.toolButton_setPackNumbers, SIGNAL(clicked()), this, SLOT(setPackNumbers()));
	connect(ui.toolButton_deleteNulRahKvyt, SIGNAL(clicked()), this, SLOT(deleteNulRahKvyt()));
	
	connect(ui.tableWidget_kvytancii, SIGNAL(cellChanged(int,int)), this, SLOT(tableWidget_kvytancii_cellChanged(int,int)));
	
	connect(ui.toolButton_print, SIGNAL(clicked()), this, SLOT(toolButton_print_clicked()));
	connect(ui.toolButton_printOn, SIGNAL(clicked()), this, SLOT(toolButton_printOn_clicked()));
	connect(ui.tabWidget_kvyt, SIGNAL(currentChanged(int)), this, SLOT(tabWidget_kvyt_currentChanged(int)));
	
	connect(ui.toolButton_reloadScript, SIGNAL(clicked()), this, SLOT(toolButton_reloadScript_clicked()));
	
	connect(ui.toolButton_reloadDirTree, SIGNAL(clicked()), this, SLOT(toolButton_reloadDirTree_clicked()));
}
//------------------------------------------------------------
void tke_Form_uni_insert_kvytancii::populateBanksComboBox()
{
	ui.comboBox_vybirBanku->blockSignals(true);
	
	ui.comboBox_vybirBanku->clear();
	onePackInFileList.clear();
	functionNamesList.clear();
	
	QString filesFilter;
	
	QScriptValue sVal;
	sVal = dbfScript->engine.globalObject().property(BANSK_SCR_STR);
	QScriptValueIterator itBank(sVal);
	sVal = dbfScript->engine.globalObject().property(FILETYPES_SCR_STR);
	QScriptValueIterator itFilter(sVal);
	sVal = dbfScript->engine.globalObject().property(ONEPACKINFILE_SCR_STR);
	QScriptValueIterator itOnePackInFile(sVal);
	sVal = dbfScript->engine.globalObject().property(FUNCTIONNAMES_SCR_STR);
	QScriptValueIterator itFunctionNames(sVal);
	while (itBank.hasNext()) {
		itBank.next();
		if (itFilter.hasNext()){
			itFilter.next();
			filesFilter = itFilter.value().toString();
		}
		else{
			filesFilter = "";
		}
		ui.comboBox_vybirBanku->addItem(itBank.value().toString(), filesFilter);
		if (itOnePackInFile.hasNext()){
			itOnePackInFile.next();
			onePackInFileList.append(itOnePackInFile.value().toBoolean());
		}
		else{
			onePackInFileList.append(false);
		}
		if (itFunctionNames.hasNext()){
			itFunctionNames.next();
			functionNamesList.append(itFunctionNames.value().toString());
		}
		else{
			onePackInFileList.append("");
		}
	}
	
	comboBox_vybirBanku_currentIndexChanged(ui.comboBox_vybirBanku->currentIndex());
	
	ui.comboBox_vybirBanku->blockSignals(false);
}
//------------------------------------------------------------
void tke_Form_uni_insert_kvytancii::comboBox_vybirBanku_currentIndexChanged(int index)
{
	dirModel->setNameFilters(QStringList() << ui.comboBox_vybirBanku->itemData(index).toString());
	
	ui.tableWidget_kvytancii->setRowCount(0);
	
	bool onePackKvyt = onePackInFileList.value(index);
	ui.tableWidget_kvytancii->setColumnHidden(5, onePackKvyt);
	ui.label_packNumber->setVisible(onePackKvyt);
	ui.spinBox_packNumber->setVisible(onePackKvyt);
	ui.toolButton_setPackNumbers->setVisible(!onePackKvyt);
	
	tableWidget_kvytancii_cellChanged(0, 3);
}
//------------------------------------------------------------
void tke_Form_uni_insert_kvytancii::treeView_fileMeneger_doubleClicked(const QModelIndex & index)
{
	ui.tableWidget_kvytancii->blockSignals(true);
	
	if (dirModel->isDir(index))
		return;

	QScriptValue scrVal = dbfScript->engine.globalObject().property(functionNamesList.value(ui.comboBox_vybirBanku->currentIndex()));
	if (scrVal.isFunction()){
		ui.tabWidget_kvyt->setCurrentIndex(0); 
		QScriptValue result = scrVal.call(scrVal, QScriptValueList() << QScriptValue(&dbfScript->engine, dirModel->filePath(index)) );
		if (dbfScript->engine.hasUncaughtException()){
			int line = dbfScript->engine.uncaughtExceptionLineNumber();
			qDebug() << "������� ��������� �������. ������ " << line << ":" << result.toString();
		}
	}
	else
		qDebug() << "<<<--- �������� �� ���� ��������� �� ��������� ������ ���������� ���������, ������� �� ���� ������ ������� - " 
				<< functionNamesList.value(ui.comboBox_vybirBanku->currentIndex()) << "\n������: �������� ����������� ������� ���������� ���������";
	
	if (onePackInFileList.value(ui.comboBox_vybirBanku->currentIndex()))
		ui.spinBox_packNumber->setValue(nextPackNumber());
	else{
		if	( !QMessageBox::question(
                this,
                "!!! �������������� ������ ������ !!!",
                "�� ������ �������� �������������� ������ ������?",
                "���",
				"ͳ",
                QString(), 1, 1)
		)
			setPackNumbers();
	}
	tableWidget_kvytancii_cellChanged(0, 3);
	
	ui.tableWidget_kvytancii->blockSignals(false);
}
//------------------------------------------------------------
void tke_Form_uni_insert_kvytancii::toolButton_addKvyt_clicked()
{
	if	( QMessageBox::question(
                this,
                "!!! ��������� ������ !!!",
                "�� ����� ������ ������ ������?",
                "���",
				"ͳ-�-�",
                QString(), 0, 1)
		)
		return;
	int curRow = ui.tableWidget_kvytancii->currentRow() + 1;
	ui.tableWidget_kvytancii->insertRow(curRow);
	ui.tableWidget_kvytancii->resizeRowToContents(curRow);
	
	dbfScript->populateTable(curRow, 0, "0", "#008000");
	dbfScript->populateTable(curRow, 1, "1", "#008000");
	dbfScript->populateTable(curRow, 2, curOplataDate.toString("dd.MM.yyyy"), "#008000");
	dbfScript->populateTable(curRow, 3, "0,00", "#008000");
	dbfScript->populateTable(curRow, 4, curKasaString, "#008000");
	dbfScript->populateTable(curRow, 5, "0", "#008000");
	
	tableWidget_kvytancii_cellChanged(0, 3);
	
	ui.tableWidget_kvytancii->setCurrentCell(curRow, 1);
	ui.tableWidget_kvytancii->editItem(ui.tableWidget_kvytancii->item(curRow, 1));
}
//------------------------------------------------------------
void tke_Form_uni_insert_kvytancii::toolButton_dellKvyt_clicked()
{
	if (ui.tableWidget_kvytancii->rowCount() < 1)
		return;
	
	if	( QMessageBox::question(
                this,
                "!!! ��������� ������ !!!",
                "�� ����� ������ �������� ������?",
                "���",
				"ͳ-�-�",
                QString(), 0, 1)
		)
		return;
	
	ui.tableWidget_kvytancii->removeRow(ui.tableWidget_kvytancii->currentRow());
	tableWidget_kvytancii_cellChanged(0, 3);
}
//------------------------------------------------------------
void tke_Form_uni_insert_kvytancii::toolButton_insertKvytToDB_clicked()
{
	rowCount = ui.tableWidget_kvytancii->rowCount();
	if (rowCount < 1)
		return;
	
	if	( QMessageBox::question(
                this,
                "!!! ����� �� ���� ����� !!!",
                "�� ����� ������ �������� ��������� �� ���� �����?",
                "���",
				"ͳ-�-�",
                QString(), 0, 1)
		)
		return;
	
	UPostupForm *postup_form = new UPostupForm(0, rowCount );
	postup_form->show();
	
	QSqlQuery *query = new QSqlQuery();
	bool doneOk = true;
	QString packStr="";
	
	qDebug() << ">>>----- �������� ��������� �� ���� ����� ----->>>";
	
	bool needPackNumFromCombo = onePackInFileList.value(ui.comboBox_vybirBanku->currentIndex());
	if (needPackNumFromCombo)
		packStr = ui.spinBox_packNumber->text();
	
	for (int row=0; row<ui.tableWidget_kvytancii->rowCount(); ++row){
		if (!needPackNumFromCombo)
			packStr = ui.tableWidget_kvytancii->item(row,5)->text();
		if (!query->exec("INSERT INTO kvytancii (Rahunok_id, Suma, Plata_date, Perevid_date, Pachka, NomerKasy, Bank_scriptID) VALUES ( "+
												ui.tableWidget_kvytancii->item(row,1)->text()+", "
												+ui.tableWidget_kvytancii->item(row,3)->text().replace(",",".")+", '"
												+ui.tableWidget_kvytancii->item(row,2)->text()+"', '"
												+ui.tableWidget_kvytancii->item(row,2)->text()+"', "
												+packStr+", '"
												+ui.tableWidget_kvytancii->item(row,4)->text()+"', "
												+QString::number(ui.comboBox_vybirBanku->currentIndex()+1)+")")){
			doneOk = false;
			qDebug() << "     ->> �� ������� ��������� �������� - " << ui.tableWidget_kvytancii->item(row,1)->text()
					<< ", �� " << ui.tableWidget_kvytancii->item(row,3)->text() << ", ����� - " 
					<< ui.tableWidget_kvytancii->item(row,4)->text() << "\n �������: " << query->lastError().text()
					<< "\n����� - " << query->lastQuery();
		}
		else{
			ui.tableWidget_kvytancii->removeRow(row);
			--row;
		}
		postup_form->inc_pos();
	}
	delete query;
	delete postup_form;
	
	if (doneOk){
		qDebug() << "     ->> �������� ��������� ������� ��� �������";
		QMessageBox::information(this,
				"!!! �������� ��������� !!!",
				"��������� ������ �������.",
				QMessageBox::Ok,
				QMessageBox::NoButton);
	}
	else{
		QMessageBox * mBox = new QMessageBox(QMessageBox::Critical, "!!! ������� �������� ��������� !!!",
									"�������� �� ������ ������ ����, ������� ��� �� ���������.\n"
									"�������� ��������� ���������� � �������"
									"�����, ������ ���������� ��������� �� ������ ���������� � ��� ��������� ����������."
									"\n��� ������� ���� ��������� 'OPEN'.");
		mBox->addButton("��", QMessageBox::ActionRole);
		QPushButton *debWindowButton = mBox->addButton("³��� ����������", QMessageBox::ActionRole);
		mBox->exec();
		if (mBox->clickedButton() == debWindowButton)
			emit requestDebugWindow();
		delete mBox;
	}
	qDebug() << "<<<----- �������� �������� ��������� �� ���� ����� -----<<<";
	tableWidget_kvytancii_cellChanged(0, 3);
}
//------------------------------------------------------------
void tke_Form_uni_insert_kvytancii::tableWidget_kvytancii_cellChanged(int row, int col)
{
	rowCount = ui.tableWidget_kvytancii->rowCount();
	if (rowCount < 1){
		ui.label_KvytSum->setText("�������� ����: 0,00 ���.");
		ui.toolButton_dellKvyt->setEnabled(false);
		ui.toolButton_insertKvytToDB->setEnabled(false);
		return;
	}
	ui.toolButton_dellKvyt->setEnabled(true);
	ui.toolButton_insertKvytToDB->setEnabled(true);
	
	if (col == 3){
		double sum=0;
		QTableWidgetItem *item;
		for (int row=0; row<rowCount; ++row){
			item = ui.tableWidget_kvytancii->item(row,3);
			if (item)
				sum += item->text().toDouble();
		}
		ui.label_KvytSum->setText("�������� ����: "+uMToStr2(sum)+" ���.");
	}
	else if (col == 2){
		QDate xDate = ui.tableWidget_kvytancii->item(row,col)->data(2).toDate();
		if (xDate.isValid()){
			curOplataDate = xDate;
		}
	}
	else if (col == 4)
		curKasaString = ui.tableWidget_kvytancii->item(row,col)->data(2).toString();
}
//------------------------------------------------------------
void tke_Form_uni_insert_kvytancii::toolButton_print_clicked()
{
	ui.textEdit_packInfo->document()->print(&prnt);
}
//------------------------------------------------------------
void tke_Form_uni_insert_kvytancii::toolButton_printOn_clicked()
{
	QPrintDialog *dlg = new QPrintDialog(&prnt, this);
	if (dlg->exec() != QDialog::Accepted)
		return;
	ui.textEdit_packInfo->document()->print(&prnt);
}
//------------------------------------------------------------
void tke_Form_uni_insert_kvytancii::tabWidget_kvyt_currentChanged(int index)
{
	if (index !=1)
		return;
	QMap<int, double> sumMap;
	QMap<int, QString> dateMap;
	QMap<int, int> kvytCountMap;
	QTableWidgetItem *item;
	double undefPackSum=0, defPackSum=0;
	int undefCount=0, defCount=0;
	int row;
	
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat, textCharFormat_bold;
	textCharFormat.setFontPointSize( 10 );
	textCharFormat_bold.font().setBold(true);
	
	ui.textEdit_packInfo->document()->clear();
	QTextCursor cursor(ui.textEdit_packInfo->document()), cellCursor;
	rowCount = ui.tableWidget_kvytancii->rowCount();
	if (rowCount==0){
		cursor.insertText("� ������� ��������� �� ������� ����� ���������", textCharFormat);
		return;
	}
	else{
		int pack=0;
		for (row=0; row<rowCount; ++row){
			item = ui.tableWidget_kvytancii->item(row,5);
			if (item){
				pack = item->data(2).toInt();
				item = ui.tableWidget_kvytancii->item(row,3);
				if (item){
					sumMap[pack] = sumMap[pack] + item->data(2).toDouble();
					kvytCountMap[pack] = kvytCountMap[pack] + 1;
				}
				else{
					++undefCount;
				}
				item = ui.tableWidget_kvytancii->item(row,2);
				if (item)
					dateMap[pack] = QDate::fromString(item->text(),"dd.MM.yyyy").toString("dd.MM.yyyy");
			}
			else{
				item = ui.tableWidget_kvytancii->item(row,3);
				if (item)
					undefPackSum += item->data(2).toDouble();
				++undefCount;
			}
		}
	}
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	cursor.insertText("�������� �������� �� \"�������� ��������\" "+QString(TkeProgramVersion())+" "+QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 13 );
	cursor.insertText("²��̲���\n������ ���������", textCharFormat);
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(" ", textCharFormat);
		//��������� � ������������ �������
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
	constraints << QTextLength(QTextLength::FixedLength, 20);
	constraints << QTextLength(QTextLength::FixedLength, 100);
	constraints << QTextLength(QTextLength::FixedLength, 100);
	constraints << QTextLength(QTextLength::FixedLength, 100);
	constraints << QTextLength(QTextLength::FixedLength, 100);
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(2);
	QTextTable *table = cursor.insertTable(sumMap.size()+3, 5, tableFormat);
		//���������� ����� �������
	cell = table->cellAt(0, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("� �/�",textCharFormat);
	
	cell = table->cellAt(0, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("���� ������",textCharFormat);
	
	cell = table->cellAt(0, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("����� ������",textCharFormat);
	
	cell = table->cellAt(0, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("����, ���.",textCharFormat);
	
	cell = table->cellAt(0, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("ʳ������",textCharFormat);
	
	//���������� ������
	row = 1;
	QMapIterator<int, double> iterMap(sumMap);
	while (iterMap.hasNext()) {
		iterMap.next();
		
		//� �/�
		cell = table->cellAt(row, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QString::number(row),textCharFormat);
		
		//���� ������
		cell = table->cellAt(row, 1);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(dateMap[iterMap.key()],textCharFormat);
		
		//����� ������
		cell = table->cellAt(row, 2);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QString::number(iterMap.key()),textCharFormat);
		
		//����, ���.
		cell = table->cellAt(row, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		double d = iterMap.value();
		defPackSum += d;
		cellCursor.insertText(uMToStr2(d),textCharFormat);
		
		//ʳ������
		cell = table->cellAt(row, 4);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		int c = kvytCountMap[iterMap.key()];
		defCount +=c;
		cellCursor.insertText(QString::number(c),textCharFormat);
		
		++row;
	}
		//��������� ���� ��������� ��� ������
	//� �/�
	cell = table->cellAt(row, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(row),textCharFormat);
	
	//���� ������
	cell = table->cellAt(row, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("-",textCharFormat);
	
	//����� ������
	cell = table->cellAt(row, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("-",textCharFormat);
	
	//����, ���.
	cell = table->cellAt(row, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(undefPackSum),textCharFormat);
	
	cell = table->cellAt(row, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(undefCount),textCharFormat);
	
	++row;
	//��������� ����
	table->mergeCells ( row, 0, 1, 2 );
	
	//� ������
	cell = table->cellAt(row, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("������, � ������.",textCharFormat);
	
	//ʳ������ ������
	cell = table->cellAt(row, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(sumMap.size()+1),textCharFormat);
	
	//����, ���.
	cell = table->cellAt(row, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(undefPackSum+defPackSum),textCharFormat);
	
	cell = table->cellAt(row, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(undefCount+defCount),textCharFormat);
}
//------------------------------------------------------------
void tke_Form_uni_insert_kvytancii::toolButton_reloadScript_clicked()
{
	dbfScript->setScriptEngine("./tke.ini", "Script/KvytScriptFilePath");
	populateBanksComboBox();
	comboBox_vybirBanku_currentIndexChanged(0);
	setWindowTitle("�������� ���������. ������ -  "+dbfScript->readScriptOrgName());
}
//------------------------------------------------------------
void tke_Form_uni_insert_kvytancii::toolButton_reloadDirTree_clicked()
{
	dirModel->refresh();
}
//------------------------------------------------------------
void tke_Form_uni_insert_kvytancii::setPackNumbers()
{
	rowCount = ui.tableWidget_kvytancii->rowCount();
	if (rowCount < 1)
		return;
	QMap<QString,int> dHash;
	QHash<int,int> yHash;
	QTableWidgetItem *item;
	QSqlQuery query;
	QDate dt, prevDt(1980, 1, 1);
	int year, prevYear=0, pack=0, row;
	
	for (row=0; row<rowCount; ++row){
		item = ui.tableWidget_kvytancii->item(row,2);
		if (item){
			//dt = QDate::fromString(item->text(),"dd.MM.yyyy");
			dt = item->data(Qt::EditRole).toDate();
			year = dt.year();
			if (year != prevYear){
				query.exec("SELECT max(Pachka) FROM kvytancii WHERE year(Plata_date)="+QString::number(year));
				query.seek(0);
				yHash[year] = query.value(0).toInt();
				prevYear = year;
			}
			if (dt != prevDt){
				dHash.insert(dt.toString("yyyy-MM-dd"),0);
			}
		}
	}
	
	QMapIterator<QString, int> iterHash(dHash);
	while (iterHash.hasNext()) {
		iterHash.next();
		dt = QDate::fromString(iterHash.key(),"yyyy-MM-dd");
		yHash[dt.year()] = pack= yHash[dt.year()]+1;
		dHash[dt.toString("yyyy-MM-dd")] = pack;
	}
	for (row=0; row<rowCount; ++row){
		item = ui.tableWidget_kvytancii->item(row,2);
		if (item){
			dt = item->data(Qt::EditRole).toDate();
			item = ui.tableWidget_kvytancii->item(row,5);
			if (!item){
				item = new QTableWidgetItem();
				ui.tableWidget_kvytancii->setItem(row,5, item);
			}
			item->setText(QString::number(dHash[dt.toString("yyyy-MM-dd")]));
		}
	}
}
//------------------------------------------------------------
void tke_Form_uni_insert_kvytancii::deleteNulRahKvyt()
{
	if	( QMessageBox::question(
                this,
                "!!! ��������� ������ !!!",
                "�� ����� ������ �������� ������ � ��������� ���������?",
                "���",
				"ͳ-�-�",
                QString(), 0, 1)
		)
		return;
	
	int rowCount = ui.tableWidget_kvytancii->rowCount();
	for (int row=0; row<rowCount; row++){
		if (ui.tableWidget_kvytancii->item(row, 1)->data(Qt::EditRole).toInt() == 0){
			ui.tableWidget_kvytancii->removeRow(row);
			rowCount--;
			row--;
		}
	}
}
//------------------------------------------------------------
int tke_Form_uni_insert_kvytancii::nextPackNumber()
{
	rowCount = ui.tableWidget_kvytancii->rowCount();
	QString str;
	if (rowCount < 1)
		return -1;
	int row, count=0, year;
	QHash<int,int> dHash;
	QTableWidgetItem *item;
	for (row=0; row<rowCount; ++row){
		item = ui.tableWidget_kvytancii->item(row,2);
		if (item){
			year = item->data(Qt::EditRole).toDate().year();
			dHash[year] = dHash[year] +1;
		}
	}
	QHashIterator<int,int> iter(dHash);
	while (iter.hasNext()) {
		iter.next();
		if (!(iter.value()<count)){
			year = iter.key();
			count = iter.value();
		}
	} 
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT max(Pachka) FROM kvytancii WHERE year(Plata_date)="+QString::number(year));
	if (query->seek(0))
		row = query->value(0).toInt()+1;
	else
		row = -1;
	delete query;
	return row;
}
//------------------------------------------------------------
tke_Form_uni_insert_kvytancii::~tke_Form_uni_insert_kvytancii()
{
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	settings->beginGroup("ImportUni");
	settings->setValue("DefaultKvytPath", QFileInfo(dirModel->filePath(ui.treeView_fileMeneger->currentIndex())).absolutePath());
	delete settings;
}
//------------------------------------------------------------
