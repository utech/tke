//------------------------------------------------------------
//
// Declaration of UForm_fillSubsDbfSpojyvannyaCO class.
// Created by Roman Mulyk 31.05.2010
//
//------------------------------------------------------------

#include "UForm_fillSubsDbfSpojyvannyaCO.h"
#include <QtDebug>
#include <QMessageBox>
#include <USqlAccessible>
#include <QFileDialog>
#include <UStandardDelegate>
#include <QTableWidgetItem>
#include <QSettings>
#include <math.h>

UForm_fillSubsDbfSpojyvannyaCO::UForm_fillSubsDbfSpojyvannyaCO( QWidget * parent) 
	: QWidget(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	QString lastFilePath = settings->value("Subs_dbf/LastDbfFileDir").toString();
	delete settings;
	if (QFile::exists(lastFilePath))
		ui.lineEdit_filePath->setText(lastFilePath);
	
	ui.tableWidget->setColumnWidth(1, 80);
	ui.tableWidget->setColumnWidth(2, 80);
	ui.tableWidget->setColumnWidth(3, 150);
	ui.tableWidget->setColumnWidth(4, 150);
	ui.tableWidget->setColumnWidth(5, 100);
	
	ui.tableWidget->setItemDelegateForColumn(1, new UDateTimeDelegate(ui.tableWidget,"MM.yyyy"));
	ui.tableWidget->setItemDelegateForColumn(3, new UDoubleSpinBoxDelegate(0,1000000000,6,ui.tableWidget));
	ui.tableWidget->setItemDelegateForColumn(4, new UDoubleSpinBoxDelegate(0,1000000000,tochnistDiuchogoTaryfu(),ui.tableWidget));
	
	ui.tableWidget->hideColumn(0);
	
	connect(ui.toolButton_browse, SIGNAL(clicked()), this, SLOT(toolButton_browse_clicked()));
	connect(ui.pushButton_fillFile, SIGNAL(clicked()), this, SLOT(pushButton_fillFile_clicked()));
	connect(ui.tableWidget->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(tableWidget_sectionClicked(int)));
	
}

UForm_fillSubsDbfSpojyvannyaCO::~UForm_fillSubsDbfSpojyvannyaCO()
{
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	if (QFile::exists(ui.lineEdit_filePath->text())) {
		QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
		settings->beginGroup("Subs_dbf");
		settings->setValue("LastDbfFileDir", ui.lineEdit_filePath->text());
		settings->endGroup();
		delete settings;
	}
}

void UForm_fillSubsDbfSpojyvannyaCO::toolButton_browse_clicked()
{
	QString dbfPath = QFileDialog::getOpenFileName( this, "Пошук dbf-файлу від Відділу субсидії", 
							ui.lineEdit_filePath->text(), "DBF-файли (*.dbf)");
	if (dbfPath.isEmpty())
		return;
	if (QFile::exists(dbfPath))
		ui.lineEdit_filePath->setText(dbfPath);
}

void UForm_fillSubsDbfSpojyvannyaCO::pushButton_fillFile_clicked()
{
	if	( QMessageBox::question(
                this,
                "Заповнення dbf-файлу",
                "Після заповнення обраного Вами dbf-файлу поля P2 і N2 у ньому можуть змінитись\nПідтверджуєте заповнення файлу?",
                "Так",
				"Ні-ні-ні",
                QString(), 0, 1)
		)
		return;
	
	disconnect(ui.tableWidget, SIGNAL(itemChanged(QTableWidgetItem *)), 
					this, SLOT(tableWidget_itemChanged(QTableWidgetItem *)));
	ui.tableWidget->setSortingEnabled(false);
	
	QSqlQuery query;
	
	xbXBase x;
	xbDbf dbfFile(&x);
	
	QString dbfFileName = ui.lineEdit_filePath->text();
	dbfFile.SetVersion( 3 );
	dbfFile.OpenDatabase(dbfFileName.toLocal8Bit().data());
	int rowCount = dbfFile.NoOfRecords();
	
	ui.tableWidget->setRowCount(rowCount);
	
	QTableWidgetItem *item;
	int row;
	for (row=0; dbfFile.GetNextRecord() == XB_NO_ERROR; row++) {
		QDate cDate;
		int rahunok;
		
		//Iter позиція стрічки
		item = new QTableWidgetItem();
		item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
		item->setData(Qt::EditRole, row+1);
		ui.tableWidget->setItem(row,0,item);
		
		//Результат
		item = new QTableWidgetItem();
		item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
		item->setBackground(QColor("#ffe7e0"));//червоний
		item->setFlags(item->flags() ^ Qt::ItemIsEditable);
		ui.tableWidget->setItem(row,5,item);
		
		//Дата
		item = new QTableWidgetItem();
		item->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
		cDate = QDate::fromString(dbfFile.GetStringField("DAT_BEG"),"yyyyMMdd");
		item->setBackground(QColor("#ffe7e0"));//червоний
		item->setFlags(item->flags() ^ Qt::ItemIsEditable);
		item->setData(Qt::EditRole, cDate);
		ui.tableWidget->setItem(row,1,item);
		
		//Рахунок
		item = new QTableWidgetItem();
		item->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
		rahunok = QString(dbfFile.GetField("OWN_NUM")).toInt();
		item->setBackground(QColor("#ffe7e0"));//червоний
		item->setFlags(item->flags() ^ Qt::ItemIsEditable);
		item->setData(Qt::EditRole, rahunok);
		ui.tableWidget->setItem(row,2,item);
		
		//Використання ГКал
		item = new QTableWidgetItem();
		item->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
		item->setBackground(QColor("#e5ffc4"));//салатовий
		QSqlQuery lichQuery;
		lichQuery.exec("SELECT Lichylnyk FROM (SELECT Budynok_ID FROM abonent \
											WHERE Rahunok="+sqlStr(rahunok)+")  AS abonent \
						LEFT JOIN budynky ON budynky.id = abonent.Budynok_ID");
		lichQuery.seek(0);
		if (lichQuery.value(0).toInt() != 0){ //Є лічильник
			lichQuery.exec("SELECT diuch_taryf.Lich_poch, diuch_taryf.Lich_kin, abonent.Ploshcha, \
									budynky.Koef_pereved, abonent.Budynok_ID \
							FROM( (SELECT * FROM abonent \
									WHERE Rahunok="+sqlStr(rahunok)+")  AS abonent \
							LEFT JOIN (SELECT * FROM diuch_taryf \
									WHERE year="+sqlStr(cDate.year())+" \
										and month="+sqlStr(cDate.month())+") AS diuch_taryf \
									ON diuch_taryf.Budynok_ID = abonent.Budynok_ID) \
							LEFT JOIN budynky ON budynky.id = abonent.Budynok_ID");
			lichQuery.next();
			double pokaznLich = lichQuery.value(1).toDouble() - lichQuery.value(0).toDouble();
			double plosAbon = lichQuery.value(2).toDouble();
			double koef = lichQuery.value(3).toDouble();
			lichQuery.exec("SELECT opal_ploshcha_bud.ZagPloshcha, opal_st_ploshcha_bud.ZagStPloshcha \
									FROM (budynky \
									LEFT JOIN (SELECT Budynok_ID, sum(Ploshcha) AS ZagPloshcha \
												FROM abonent WHERE Opal=true GROUP BY Budynok_ID) \
												AS opal_ploshcha_bud ON budynky.id = opal_ploshcha_bud.Budynok_ID) \
									LEFT JOIN (SELECT abonent.Budynok_ID, sum(pok.Plos) AS ZagStPloshcha FROM (SELECT Rahunok_ID, Plos \
												FROM pokaznOpalStoyak \
												WHERE Year="+sqlStr(cDate.year())+" \
													and Month="+sqlStr(cDate.month())+") AS pok \
												LEFT JOIN abonent ON abonent.Rahunok=pok.Rahunok_ID \
												GROUP BY abonent.Budynok_ID) AS opal_st_ploshcha_bud ON budynky.id = opal_st_ploshcha_bud.Budynok_ID \
									WHERE budynky.id="+lichQuery.value(4).toString());
			lichQuery.next();
			double zagOpalPlos = lichQuery.value(0).toDouble() + lichQuery.value(1).toDouble();
			if (zagOpalPlos > 0.009){
				if (koef > 0.00009){
					double pokaznAbon = pokaznLich * plosAbon / (koef * zagOpalPlos);
					item->setData(Qt::EditRole, pokaznAbon);
					dbfFile.PutDoubleField("N2", pokaznAbon);
					insertResult(row, "Викон.");
				}
				else {//невірний коефіцієнт лічильника
					item->setData(Qt::EditRole, 0);
					insertResult(row, "невірний коеф.перевед.лічильн.");
				}
				
			}
			else {//Нульова опалювальна площа в будинку
				item->setData(Qt::EditRole, 0);
				insertResult(row, "Нульова опал.площа буд.");
			}
		}
		else { //немає ліильника
			item->setData(Qt::EditRole, 0);
			insertResult(row, "Немає лічильника");
		}
		ui.tableWidget->setItem(row,3,item);
		
		//Тариф за метр квадр
		query.exec("SELECT diuch_taryf.Taryf \
					FROM abonent \
					LEFT JOIN diuch_taryf \
						ON diuch_taryf.Budynok_ID=abonent.Budynok_id \
					WHERE diuch_taryf.Year="+sqlStr(cDate.year())+" \
						and diuch_taryf.Month="+sqlStr(cDate.month())+" \
						and abonent.Rahunok="+sqlStr(rahunok));
		double taryf = 0;
		if (query.next())
			taryf = query.value(0).toDouble();
		item = new QTableWidgetItem();
		item->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
		item->setBackground(QColor("#e5ffc4"));//салатовий
		taryf = qRound(taryf * pow(10, tochnistDiuchogoTaryfu())) / pow(10, tochnistDiuchogoTaryfu());
		item->setData(Qt::EditRole, taryf);
		dbfFile.PutDoubleField("P2", taryf);
		ui.tableWidget->setItem(row,4,item);
		
		dbfFile.PutRecord();
	}
	ui.tableWidget->resizeRowsToContents();
	
	//dbfFile.CloseDatabase();
	connect(ui.tableWidget, SIGNAL(itemChanged(QTableWidgetItem *)), 
					this, SLOT(tableWidget_itemChanged(QTableWidgetItem *)));
}

void UForm_fillSubsDbfSpojyvannyaCO::tableWidget_itemChanged(QTableWidgetItem * item)
{
	disconnect(ui.tableWidget, SIGNAL(itemChanged(QTableWidgetItem *)), 
					this, SLOT(tableWidget_itemChanged(QTableWidgetItem *)));
	
	int row = item->row();
	int col = item->column();
	int iter = ui.tableWidget->item(row,0)->data(Qt::EditRole).toInt();
	
	xbXBase x;
	xbDbf dbfFile(&x);
	
	QString dbfFileName = ui.lineEdit_filePath->text();
	dbfFile.SetVersion( 3 );
	if (dbfFile.OpenDatabase(dbfFileName.toLocal8Bit().data()) != XB_NO_ERROR)
		qDebug() << "Помилка відкривання dbf файлу";
	dbfFile.GetRecord(iter);
	if (col == 3) // Запис використання Гкал в N2
		dbfFile.PutDoubleField("N2", item->data(Qt::EditRole).toDouble());
	if (col == 4) // Запис тарифу в P2
		dbfFile.PutDoubleField("P2", item->data(Qt::EditRole).toDouble());
	if (dbfFile.PutRecord() != XB_NO_ERROR)
		QMessageBox::information(0, "Помилка збереження", "Програма не змогла зберегти зміни до файлу.\nЗакрийте усі програми, що можуть використовувати dbf-файл,\nз яким Ви зааз працюєте.");
	
	dbfFile.GetRecord(iter);
	if (col == 3) // Запис використання Гкал в N2
		item->setData(Qt::EditRole, dbfFile.GetDoubleField("N2")) ;
	if (col == 4) // Запис тарифу в P2
		item->setData(Qt::EditRole, dbfFile.GetDoubleField("P2")) ;
	
	connect(ui.tableWidget, SIGNAL(itemChanged(QTableWidgetItem *)), 
					this, SLOT(tableWidget_itemChanged(QTableWidgetItem *)));
}

void UForm_fillSubsDbfSpojyvannyaCO::insertResult(int row, QString text)
{
	QString str = ui.tableWidget->item(row, 5)->data(Qt::EditRole).toString();
	if (!str.isEmpty())
		str += "\n";
	str += text;
	ui.tableWidget->item(row,5)->setData(Qt::EditRole, str);
}

inline short UForm_fillSubsDbfSpojyvannyaCO::tochnistDiuchogoTaryfu()
{
	QSqlQuery query;
	query.exec("SELECT value FROM slujb_znach WHERE id=8");
	query.seek(0);
	int iVal = query.value(0).toDouble();
	if (iVal < 2) iVal = 2;
	if (iVal > 6) iVal = 6;
	return iVal;
}

void UForm_fillSubsDbfSpojyvannyaCO::tableWidget_sectionClicked(int section)
{
	ui.tableWidget->setSortingEnabled(true);
}
