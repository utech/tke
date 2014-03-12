//------------------------------------------------------------
//
// Implamantation of tke_Dialog_narah class.
// Created by Roman Mulyk 26.11.2006
//
//------------------------------------------------------------

#include <QSqlDriver>
#include <QDebug>
#include <USqlAccessible>
#include "tke_form_insert_kvyt_dbf_ob.h"
#include "tke_math.h"
#include "uinsert_kvyt_ob_delegate.h"
#include "../tkeOrganizacii/udiff_func.h"

//------------------------------------------------------------
tke_Form_insert_kvyt_dbf_ob::tke_Form_insert_kvyt_dbf_ob(QWidget *parent, QString dir_name)
        : QWidget(parent)
{
    ui.setupUi(this);
	
	codec = QTextCodec::codecForName("CP1251");
	dirName = dir_name;
	appPath = QCoreApplication::applicationDirPath();
	QDir dir(dirName);
	filesList = dir.entryList( QStringList("*.ARJ"), QDir::Files, QDir::Name );
	QFile arjFile;
	
	prnt = new QPrinter();
	
	arjModel = new QStandardItemModel(filesList.count(), 2);
	
	bool done_ok = true;
	QString tempFileName;
	for (int row = 0; row < filesList.count(); row++) {
		fileName = filesList.at(row);
		tempFileName = appPath + "/" + KVYT_ARJ_DIR + "/" + fileName;
		if (QFile::exists(tempFileName))
			QFile::remove(tempFileName);
		if (QFile::copy(dirName + "/" + fileName, tempFileName)){
			index = arjModel->index(row, 0);
			arjModel->setData(index, QVariant(QString(fileName.at(6)) + fileName.at(7) + "." +
										QString(fileName.at(4)) + fileName.at(5) + "." +
										QString(fileName.at(2)) + fileName.at(3)));
			index = arjModel->index(row, 1);
			arjModel->setData(index, QVariant(fileName));
		}
		else{
			done_ok = false;
			errorList << fileName;
		}
	}
	if (!done_ok){
		for (int i=0; i<errorList.count(); i++){
			fileName += "    " + errorList.at(i) + '\n';
		}
		QMessageBox::critical(
            this,
            codec->toUnicode("!!! Помилка копіювання файлів !!!"),
            codec->toUnicode("Не вдалося скопіювати файли\n")+ fileName +"\nЗ скопійованими \
								файлами, якщо такі є, Ви можете працювати.\
								Про всяк випадок спробуйте ще раз вказати папку з архівами dbf-ок",
            QMessageBox::Ok,
            QMessageBox::NoButton,
			QMessageBox::NoButton);
	}
	
	ui.listView->setModel( arjModel );
	ui.listView->setModelColumn(0);
	ui.listView->setAlternatingRowColors(true);
	
	ui.tableWidget->setItemDelegate(new UInsert_kvyt_ob_Delegate(ui.tableWidget));
	
	//Контекстне меню
	ui.tableWidget->addAction(ui.action_add_row);
	ui.tableWidget->addAction(ui.action_del_row);
	ui.tableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);

	
		//connectors 
	connect(ui.tableWidget, SIGNAL(cellChanged ( int , int )), this, SLOT(show_sum())); //зміна даних в таблиці
	connect(ui.tabWidget, SIGNAL(currentChanged(int)), this, SLOT(changedTab(int))); //зміна поточної закладки
	connect(ui.pushButton_insert, SIGNAL(clicked()), this, SLOT(insert_kvyt_into_db())); //внести правлені квитанції до бази даних
	connect(ui.listView->selectionModel(), SIGNAL(currentRowChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(open_input_dbf(const QModelIndex &))); //позначено запис --> читання даних з DBF
	connect(ui.action_add_row, SIGNAL(activated()), this, SLOT(action_add_row_activated())); // Додавання стрічки квитанції в пакет
	connect(ui.action_del_row, SIGNAL(activated()), this, SLOT(action_del_row_activated())); // Видалення стрічки квитанції з пакету

	connect(ui.pushButton_print, SIGNAL(clicked()), this, SLOT(printDoc())); //друк
	connect(ui.pushButton_print_on, SIGNAL(clicked()), this, SLOT(printDocOn())); //друк
}
//------------------------------------------------------------
void tke_Form_insert_kvyt_dbf_ob::show_sum()
{
	sumaOplaty=0;
	for (int i=0; i<ui.tableWidget->rowCount(); i++){
		sumaOplaty += ui.tableWidget->item(i, 1)->data(Qt::DisplayRole).toDouble();
	}
	ui.label_show_sum->setText(codec->toUnicode("Загальна сума: ") + uMToStr2(sumaOplaty) + codec->toUnicode("грн."));
}
//------------------------------------------------------------
void tke_Form_insert_kvyt_dbf_ob::open_input_dbf(const QModelIndex & index)
{
	disconnect(ui.tableWidget, SIGNAL(cellChanged ( int , int )), this, SLOT(show_sum())); //зміна даних в таблиці
	
	ui.pushButton_insert->setEnabled(true);
	ui.textEdit->document()->clear();
	disconnect(ui.tabWidget, SIGNAL(currentChanged(int)), this, SLOT(changedTab(int))); ////зміна поточної закладки
	ui.tabWidget->setCurrentIndex(0);
	connect(ui.tabWidget, SIGNAL(currentChanged(int)), this, SLOT(changedTab(int))); ////зміна поточної закладки
	
	QModelIndex index_1 = arjModel->index(index.row(), 1);
	fileName = arjModel->data(index_1).toString();
	curDateString = arjModel->data(index).toString();
	curDateStringExt = curDateString.section('.', 0, 1) + ".20" + curDateString.section('.', 2, 2);
	curIndex = index;
	QDir::setCurrent(appPath+"/"+KVYT_ARJ_DIR);
	//int res = QProcess::execute( "..\\arj -y e " + fileName + " *.dbf" );
	int res = QProcess::execute("..\\7z e -tARJ "+fileName+" *.dbf -y");
	
	//Під'єднання до бази даних
	if (res == 0){
		QSqlDatabase db = QSqlDatabase::addDatabase("QODBC", "insert_kvyt_dbf_data");
		db.setDatabaseName("tke_kvytancii_dbf_ob.dsn");
    		if (db.open()){
				QSqlQuery * kvytQuery = new QSqlQuery(db);
				kvytQuery->exec("SELECT count(*) FROM " + fileName.remove(".ARJ", Qt::CaseInsensitive));
				kvytQuery->seek(0);
				int numRec = kvytQuery->value(0).toInt();
				ui.tableWidget->setColumnCount( 3 );
				ui.tableWidget->setRowCount( numRec );
				
				QStringList hLabelList;
				hLabelList	<< codec->toUnicode("Рахунок")
							<< codec->toUnicode("Сума")
							<< codec->toUnicode("Кас.ап.");
				ui.tableWidget->setHorizontalHeaderLabels ( hLabelList );
				hLabelList.clear();
				
				kvytQuery->exec("SELECT * FROM " + fileName.remove(".ARJ", Qt::CaseInsensitive) + " ORDER BY NRA");
				kvytQuery->seek(-1);
				int row=0, rahunok;
				QColor rowColor;
				bool wrongRahunok = false;
				while (kvytQuery->next()){
					rahunok = kvytQuery->value(5).toDouble() + 0.2 - 1000000;
					if (rahunok<=0){
						rahunok = 0;
						rowColor = Qt::magenta;
						wrongRahunok=true;
					}
					else{
						rowColor = Qt::white;
					}
					ui.tableWidget->setItem(row, 0, new QTableWidgetItem(QVariant(rahunok).toString()));
					ui.tableWidget->item(row,0)->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter);
					ui.tableWidget->item(row,0)->setBackgroundColor(rowColor);
					ui.tableWidget->setItem(row, 1, new QTableWidgetItem(uMToStr2(kvytQuery->value(6).toDouble())));
					ui.tableWidget->item(row,1)->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter);
					ui.tableWidget->item(row,1)->setBackgroundColor(rowColor);
					ui.tableWidget->setItem(row, 2, new QTableWidgetItem(QVariant(kvytQuery->value(1).toInt()).toString()));
					ui.tableWidget->item(row,2)->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter);
					ui.tableWidget->item(row,2)->setBackgroundColor(rowColor);
					ui.tableWidget->item(row,2)->setFlags( 0 );
					row++;
				}
				ui.tableWidget->setCurrentCell( 0, 0 );
				if (wrongRahunok){
					QMessageBox::critical(
						this,
						codec->toUnicode("!!! Помилка в рахунку абонента !!!"),
						codec->toUnicode("Один або декілька рахунків менші за 1.\n"),
						QMessageBox::Ok,
						QMessageBox::NoButton,
						QMessageBox::NoButton);
				}	
				delete kvytQuery;
			}
			else{
				QMessageBox::critical(0, QObject::tr("Помилка підключення до таблиці квитанцій"),
         	                     codec->toUnicode("Причина помилки, згенерована драйвером:\n") +
								 db.lastError().text());
				ui.tableWidget->clear();
			}
			QSqlDatabase::removeDatabase( "insert_kvyt_dbf_data" );
	}
	else{
	QMessageBox::critical(
            this,
            codec->toUnicode("!!! Помилка розархівування !!!"),
            codec->toUnicode("Програмі не вдалося розархівувати пакет з dbf-кою квитанцій.\n") +
			codec->toUnicode("   ПОРАДА  - Закрийте відкритий в MS Excel або іншій програмі\n")+
			codec->toUnicode("           dbf-файл квитанцій вибраної вами дати.\n"),
            QMessageBox::Ok,
            QMessageBox::NoButton,
			QMessageBox::NoButton);
	ui.tableWidget->clear();
	}
	show_sum();
	connect(ui.tableWidget, SIGNAL(cellChanged ( int , int )), this, SLOT(show_sum())); //зміна даних в таблиці
}
//------------------------------------------------------------
void tke_Form_insert_kvyt_dbf_ob::insert_kvyt_into_db()
{
	//bool done_ok=true;
	int numPaketu;

	QSqlQuery *packQuery = new QSqlQuery();
	int kvytYear = ("20" + curDateString.section('.', 2, 2)).toInt();
	packQuery->exec("SELECT max(Pachka) FROM kvytancii WHERE year(Plata_date)="+QVariant(kvytYear).toString());
	packQuery->seek(0);
	numPaketu = packQuery->value(0).toInt() + 1;
	if	( ! QMessageBox::question(
                this,
                codec->toUnicode("!!! Вибір пакету квитанцій !!!"),
                codec->toUnicode("Присвоїти даному пакету номер - ") + QVariant(numPaketu).toString() + "?\n"+
				codec->toUnicode("Дата оплати в пакеті - ") + curDateStringExt + codec->toUnicode("р.,\n") +
				codec->toUnicode("Сума оплати в пакеті - ") + uMToStr2(sumaOplaty) + codec->toUnicode("грн.")
				,
                codec->toUnicode("Так"),
				codec->toUnicode("Ні-ні-ні"),
                QString(), 0, 1)){
		query = new QSqlQuery;
		bool q_ok=true;
		QSqlDatabase::database().transaction();
		
		for (int i=0; i<ui.tableWidget->rowCount(); i++){
			query->prepare("INSERT INTO kvytancii (id, Rahunok_ID, Suma, Plata_date, Perevid_date, Pachka) "
              "VALUES (:id, :Rahunok_ID, :Suma, :Plata_date, :Perevid_date, :Pachka)");
			query->bindValue(":id", nextLichylnyk("kvytancii"));
			query->bindValue(":Rahunok_ID", ui.tableWidget->item(i,0)->text());
			query->bindValue(":Suma", ui.tableWidget->item(i,1)->text().toDouble());
			query->bindValue(":Plata_date", curDateStringExt);
			query->bindValue(":Perevid_date", curDateStringExt);
			query->bindValue(":Pachka", numPaketu);
			q_ok = q_ok && query->exec();
		}
		if (q_ok){
			q_ok = q_ok && QSqlDatabase::database().commit();
		}
		else{
			QSqlDatabase::database().rollback();
		}
		if ( !q_ok){
			QMessageBox::critical(
				this,
				codec->toUnicode("!!! Помилка запису пакета квитанцій. !!!"),
				codec->toUnicode("Програмі не вдалося записати пакет квитанцій до бази даних повністю або частково.\n") +
				codec->toUnicode("   ПОРАДА  - Провірте, які квитанції було внесено, решту довнесіть вручну.\n")+
				codec->toUnicode("   ПОРАДА  - Якщо не внесено жодної квитанції, спробуйте ще раз.\n"),
				QMessageBox::Ok,
				QMessageBox::NoButton,
				QMessageBox::NoButton);	
		}
		else{
			createPrintDock(numPaketu);
			bool q_ok = query->exec("INSERT INTO kvytancii_html (id, Packet, Packet_date, Packet_html) \
						VALUES ("+QVariant(nextLichylnyk("kvytancii_html")).toString()+", \
						"+QVariant(numPaketu).toString()+", \
						'"+curDateStringExt+"', '\
						"+ui.textEdit->document()->toHtml("KOI8-U")+"')");
			qDebug() << query->lastError().text();
			int doPrint;
			if ( !q_ok ){
				doPrint = QMessageBox::warning(
					this,
					codec->toUnicode("!!! Помилка запису друкованого документа до бази даних. !!!"),
					codec->toUnicode("Програма успішно внесла квитанції пакету  - ") + QVariant(numPaketu).toString() + ".\n"+
					codec->toUnicode("Але програмі не вдалося записати друкований документ до бази даних.\n"),
					"OK",
					codec->toUnicode("Друк відомості"), 0, 0, 1);	
			}
			else{
				doPrint = QMessageBox::information(this,
					codec->toUnicode("!!! Закінчено внесення квитанцій !!!"),
					codec->toUnicode("Успішно закінчено внесення квитанцій пакету - ") + QVariant(numPaketu).toString() + '.',
					"OK",
					codec->toUnicode("Друк відомості"), 0, 0, 1);
			}
			if (doPrint == 1){
				printDoc();
			}
			arjModel->removeRow(curIndex.row());
		}
		delete query;
	}
	delete packQuery;
}
//------------------------------------------------------------
void tke_Form_insert_kvyt_dbf_ob::createPrintDock(int packet)
{
		//Заголовок документа
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat;
	//Створення документа тарифів для всіх будинків за X місяць, Y рік
	ui.textEdit->document()->clear();
	QTextCursor cursor(ui.textEdit->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 10 );
	cursor.insertText(codec->toUnicode("Реєстр квитанцій оплати в Ощадбанку за ") + curDateStringExt + codec->toUnicode("р. "), textCharFormat);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 9 );
	cursor.insertText(codec->toUnicode("   Загальна сума - ") + uMToStr2(sumaOplaty) + codec->toUnicode("грн.\n"), textCharFormat);
	cursor.insertText(codec->toUnicode("   Номер пакету -  ") + QVariant(packet).toString() + ".", textCharFormat);
	
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
	constraints << QTextLength(QTextLength::FixedLength, 20);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(2);
	QTextTable *table = cursor.insertTable(ui.tableWidget->rowCount()+2, ui.tableWidget->columnCount()+1, tableFormat);
	
	//Заповнення шапки таблиці
		//№ п/п
	cell = table->cellAt(0, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("№п/п"));
	
	for (int col=0; col<ui.tableWidget->columnCount(); col++){
		cell = table->cellAt(0, col+1);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(ui.tableWidget->horizontalHeaderItem(col)->text());
	}
	
	//Заповнення даних таблиці
	for (int row=0; row<ui.tableWidget->rowCount(); row++){
	
		cell = table->cellAt(row+1, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QVariant(row+1).toString());
		
		for (int col=0; col<ui.tableWidget->columnCount(); col++){
			cell = table->cellAt(row+1, col+1);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(ui.tableWidget->item(row, col)->text());
		}
	}
	//Сумарне значення
	table->mergeCells ( ui.tableWidget->rowCount()+1, 0, 1, ui.tableWidget->columnCount() );
	
	cell = table->cellAt(ui.tableWidget->rowCount()+1, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Всього"));
	
	cell = table->cellAt(ui.tableWidget->rowCount()+1, ui.tableWidget->columnCount());
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sumaOplaty));
}
//------------------------------------------------------------
void tke_Form_insert_kvyt_dbf_ob::changedTab(int curTab)
{
	if (curTab==1){
		QSqlQuery *packQuery = new QSqlQuery();
		int kvytYear = ("20" + curDateString.section('.', 2, 2)).toInt();
		packQuery->exec("SELECT max(Pachka) FROM kvytancii WHERE year(Plata_date)="+QVariant(kvytYear).toString());
		packQuery->seek(0);
		createPrintDock(packQuery->value(0).toInt() + 1);
		delete packQuery;
	}
}
//------------------------------------------------------------
void tke_Form_insert_kvyt_dbf_ob::action_add_row_activated()
{
	int row = ui.tableWidget->rowCount();
	ui.tableWidget->insertRow( row );
	ui.tableWidget->setItem(row, 1, new QTableWidgetItem(uMToStr2(100)));
	ui.tableWidget->item(row,1)->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget->item(row,1)->setBackgroundColor(Qt::green);
	ui.tableWidget->setItem(row, 0, new QTableWidgetItem("1"));
	ui.tableWidget->item(row,0)->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget->item(row,0)->setBackgroundColor(Qt::green);
	ui.tableWidget->setItem(row, 2, new QTableWidgetItem(codec->toUnicode("Вручну")));
	ui.tableWidget->item(row,2)->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter);
	ui.tableWidget->item(row,2)->setBackgroundColor(Qt::green);
	ui.tableWidget->item(row,2)->setFlags( 0 );
	ui.tableWidget->setCurrentCell( row, 0 );
	ui.tableWidget->editItem( ui.tableWidget->item(row, 0) );
}
//------------------------------------------------------------
void tke_Form_insert_kvyt_dbf_ob::action_del_row_activated()
{
	ui.tableWidget->removeRow( ui.tableWidget->currentRow());
	show_sum();
}
//------------------------------------------------------------
void tke_Form_insert_kvyt_dbf_ob::printDoc()
{
	ui.textEdit->document()->print(prnt);
}
//------------------------------------------------------------
void tke_Form_insert_kvyt_dbf_ob::printDocOn()
{
	QPrintDialog *dlg = new QPrintDialog(prnt, this);
	if (dlg->exec() != QDialog::Accepted)
		return;
	ui.textEdit->document()->print(prnt);
	delete dlg;
}
//------------------------------------------------------------
tke_Form_insert_kvyt_dbf_ob::~tke_Form_insert_kvyt_dbf_ob()
{
	delete arjModel;
	delete prnt;
}
//------------------------------------------------------------
