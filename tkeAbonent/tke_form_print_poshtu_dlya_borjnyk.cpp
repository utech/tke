//------------------------------------------------------------
//
// Implamantation of tkeFormPrintPoshtaDlyaBorjnykiv class.
// Created by Roman Mulyk 4.08.2007
//
//------------------------------------------------------------
#include <QApplication>
#include <UPostupForm>
#include <USqlAccessible>
#include "tke_form_print_poshtu_dlya_borjnyk.h"

//------------------------------------------------------------
tkeFormPrintPoshtaDlyaBorjnykiv::tkeFormPrintPoshtaDlyaBorjnykiv( QWidget * parent) 
	: QDialog(parent)
{
	ui.setupUi(this);
	codec = QTextCodec::codecForName("cp1251");

	prnt = new QPrinter();
	
	connect(ui.pushButton_acceptMaxBorg, SIGNAL(clicked()), this, SLOT(populateTableWidjetBorjnyky()));
	connect(ui.checkBox_BorjnykOpal, SIGNAL(stateChanged(int)), this, SLOT(populateTableWidjetBorjnyky()));
	connect(ui.pushButton_selectAll, SIGNAL(clicked()), this, SLOT(selectAllBorjnyk()));
	connect(ui.pushButton_deselectAll, SIGNAL(clicked()), this, SLOT(deSelectAllBorjnyk()));
	connect(ui.pushButton_printSelected, SIGNAL(clicked()), this, SLOT(printMessages()));
	connect(ui.pushButton_printSelectedOnPrinter, SIGNAL(clicked()), this, SLOT(printMessagesOn()));
}
//------------------------------------------------------------
void tkeFormPrintPoshtaDlyaBorjnykiv::printMessages()
{
	QString *fileName = new QString(QCoreApplication::applicationDirPath ()+"/templates/borjnykyMessage_1.html");
	QString pageHTML;
	QFile *file = new QFile(*fileName);
	if (file->open(QFile::ReadOnly)) {
		QHash<QString, QString> hash;
		UReplaceStringParser *parser = new UReplaceStringParser;
		parser->setBeginTagChar('[');
		parser->setEndTagChar(']');
		
		QByteArray *data = new QByteArray();
		*data = file->readAll();
		QTextCodec *c = Qt::codecForHtml(*data);
		parser->setInputString(c->toUnicode(*data));

		file->close();
		
		QTextDocument *doc = new QTextDocument();
		//doc->setPageSize(QSizeF (doc->pageSize().width(), doc->pageSize().height()+20));
		prnt->setPageMargins ( 12, 10, 12, 8, QPrinter::Millimeter );
		int iter=0;
		for (int i=0; i<ui.tableWidgetBorjnyky->rowCount(); i++){
			if ((dynamic_cast<QCheckBox*>(ui.tableWidgetBorjnyky->cellWidget(i, 4)))->isChecked()){
				hash.clear();
				hash.insert(QString("Rahunok"), ui.tableWidgetBorjnyky->item(i,0)->text());
				hash.insert(QString("PIB"), ui.tableWidgetBorjnyky->item(i,1)->text());
				hash.insert(QString("Adresa"), ui.tableWidgetBorjnyky->item(i,2)->text());
				hash.insert(QString("Borg"), ui.tableWidgetBorjnyky->item(i,3)->text());
				parser->setTagsStringsHash(hash);
				parser->convertString();
				
				iter++;
				if (iter!=1){
					pageHTML.append("<center>---------------------------------------------------------------------------------------------------------</center>");
					pageHTML.append("");
				}
				pageHTML.append(parser->curOutputString());
				
				if (iter==5){
					iter = 0;
					doc->setHtml(pageHTML);
					doc->print(prnt);
					pageHTML.clear();
				}
			}
		}
		if (pageHTML.size() > 0){
			doc->setHtml(pageHTML);
			doc->print(prnt);
		}
		pageHTML.clear();
		delete doc;
		delete data;
	}
	else{
		QMessageBox::critical(
				this,
				codec->toUnicode("!!! Не знайдено зразка пошти !!!"),
				codec->toUnicode("Не знайдено файла ")+*fileName+".",
				QMessageBox::Ok,
				QMessageBox::NoButton,
				QMessageBox::NoButton);
	}
	delete file;
	delete fileName;
}
//------------------------------------------------------------
void tkeFormPrintPoshtaDlyaBorjnykiv::printMessagesOn()
{
	QPrintDialog *dlg = new QPrintDialog(prnt, this);
	if (dlg->exec() == QDialog::Accepted)
		printMessages();
	delete dlg;
}
//------------------------------------------------------------
void tkeFormPrintPoshtaDlyaBorjnykiv::populateTableWidjetBorjnyky()
{
	QSqlQuery *query = new QSqlQuery;
	QSqlQuery *abonentsQuery = new QSqlQuery;
	UPostupForm *pForm = new UPostupForm(0, 0);
	pForm->show();
	
	int month, year;
	query->exec("SELECT max(Year) FROM narahuvannya");
	query->seek(0);
	year = query->value(0).toInt();
	
	query->exec("SELECT max(Month) FROM narahuvannya WHERE year=" + QVariant(year).toString());
	query->seek(0);
	month = query->value(0).toInt();

	query->exec("SELECT count(*) FROM (SELECT abonBorg.Rahunok, abonent.Prizv, abonent.Imya, abonent.Batk, \
						abonent.Adresa, abonBorg.Borg-(SELECT IIf(sum(subs.Suma) is null,0,sum(subs.Suma))+\
						IIf(sum(subs.Suma_d) is null,0,sum(subs.Suma_d)) \
						FROM subs \
						WHERE subs.Rahunok_ID=abonBorg.Rahunok \
							and subs.Year="+QVariant(year).toString()+" \
							and subs.month="+QVariant(month).toString()+")-\
						(SELECT IIf(sum(Suma) is null,0,sum(Suma)) \
						FROM kvytancii \
						WHERE kvytancii.Rahunok_ID=abonBorg.Rahunok \
							and year(kvytancii.Plata_Date)="+QVariant(year).toString()+" \
							and month(kvytancii.Plata_Date)="+QVariant(month).toString()+")- \
						 \
						(SELECT IIf(sum(Suma) is null,0,sum(Suma)) AS perevSuma \
						FROM narahuvannyaPereved \
						WHERE year(CDate)="+sqlStr(year)+" \
							and month(CDate)="+sqlStr(month)+" \
							and Rahunok_ID=abonBorg.Rahunok) + \
						 \
						(SELECT sum(iif(Vh_saldo is null, 0, Vh_saldo) + \
								iif(ZaborgAdd is null, 0, ZaborgAdd) - \
								iif(OplataSuma is null, 0, OplataSuma)) \
						FROM debitorskaZaborg \
						WHERE year(CDate)="+QVariant(year).toString()+" \
							and month(CDate)="+QVariant(month).toString()+" \
							and Rahunok=abonBorg.Rahunok) \
						 \
							AS Borg \
						FROM [SELECT narahuvannya.Rahunok, narahuvannya.Vh_saldo+\
								narahuvannya.Narah AS Borg \
							FROM narahuvannya \
							WHERE narahuvannya.year="+QVariant(year).toString()+" \
								and narahuvannya.month="+QVariant(month).toString()+" \
								and narahuvannya.Status=0 \
								and narahuvannya.Vh_saldo+narahuvannya.Narah>"+uMToStr2Krapka(ui.doubleSpinBox_minBorg->value())+"]. AS abonBorg \
						LEFT JOIN (SELECT abonent.Rahunok, abonent.Prizv, abonent.Imya, abonent.Batk, \
							budynky.Vulycia & ' ' & budynky.Bud_num & ' "+codec->toUnicode("кв.")+"' & abonent.Kvartyra AS Adresa, \
							abonent.opal \
						FROM abonent \
						LEFT JOIN budynky ON abonent.Budynok_ID=budynky.id) AS abonent \
						ON abonent.Rahunok = abonBorg.Rahunok \
						WHERE opal="+QVariant(ui.checkBox_BorjnykOpal->isChecked()).toString()+" ) \
				WHERE Borg>"+uMToStr2Krapka(ui.doubleSpinBox_minBorg->value()));
	qDebug() << query->lastQuery();
	query->seek(0);
	ui.tableWidgetBorjnyky->setRowCount(query->value(0).toInt());
	pForm->setNumPos(query->value(0).toInt());
	
	query->exec("SELECT * FROM (SELECT abonBorg.Rahunok, abonent.Prizv, abonent.Imya, abonent.Batk, \
						abonent.Adresa, abonBorg.Borg-(SELECT IIf(sum(subs.Suma) is null,0,sum(subs.Suma))+\
						IIf(sum(subs.Suma_d) is null,0,sum(subs.Suma_d)) \
						FROM subs \
						WHERE subs.Rahunok_ID=abonBorg.Rahunok \
							and subs.Year="+QVariant(year).toString()+" \
							and subs.month="+QVariant(month).toString()+")-\
						(SELECT IIf(sum(Suma) is null,0,sum(Suma)) \
						FROM kvytancii \
						WHERE kvytancii.Rahunok_ID=abonBorg.Rahunok \
							and year(kvytancii.Plata_Date)="+QVariant(year).toString()+" \
							and month(kvytancii.Plata_Date)="+QVariant(month).toString()+") - \
						 \
						(SELECT IIf(sum(Suma) is null,0,sum(Suma)) AS perevSuma \
						FROM narahuvannyaPereved \
						WHERE year(CDate)="+sqlStr(year)+" \
							and month(CDate)="+sqlStr(month)+" \
							and Rahunok_ID=abonBorg.Rahunok) + \
						 \
						(SELECT sum(iif(Vh_saldo is null, 0, Vh_saldo) + \
								iif(ZaborgAdd is null, 0, ZaborgAdd) - \
								iif(OplataSuma is null, 0, OplataSuma)) \
						FROM debitorskaZaborg \
						WHERE year(CDate)="+QVariant(year).toString()+" \
							and month(CDate)="+QVariant(month).toString()+" \
							and Rahunok=abonBorg.Rahunok) \
						 \
							AS Borg \
						FROM [SELECT narahuvannya.Rahunok, narahuvannya.Vh_saldo+\
								narahuvannya.Narah AS Borg \
							FROM narahuvannya \
							WHERE narahuvannya.year="+QVariant(year).toString()+" \
								and narahuvannya.month="+QVariant(month).toString()+" \
								and narahuvannya.Status=0 \
								and narahuvannya.Vh_saldo+narahuvannya.Narah>"+uMToStr2Krapka(ui.doubleSpinBox_minBorg->value())+"]. AS abonBorg \
						LEFT JOIN (SELECT abonent.Rahunok, abonent.Prizv, abonent.Imya, abonent.Batk, \
							budynky.Vulycia & ' ' & budynky.Bud_num & ' "+codec->toUnicode("кв.")+"' & abonent.Kvartyra AS Adresa, \
							abonent.opal \
						FROM abonent \
						LEFT JOIN budynky ON abonent.Budynok_ID=budynky.id) AS abonent \
						ON abonent.Rahunok = abonBorg.Rahunok \
						WHERE opal="+QVariant(ui.checkBox_BorjnykOpal->isChecked()).toString()+" ORDER BY abonBorg.Rahunok) \
				WHERE Borg>"+uMToStr2Krapka(ui.doubleSpinBox_minBorg->value()));
	
	int row=0;
	while (query->next()){
		QString Imya, Batk, PIB;
		Imya = query->value(2).toString();
		Batk = query->value(3).toString();
		PIB = query->value(1).toString();
		if (Imya.size() > 0){
			PIB += QString(" ") + Imya.at(0) + ".";
		}
		if (Batk.size() > 0){
			PIB += Batk.at(0) + QString(".");
		}
		ui.tableWidgetBorjnyky->setItem(row, 0, new QTableWidgetItem(query->value(0).toString()));
		ui.tableWidgetBorjnyky->item(row,0)->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
		ui.tableWidgetBorjnyky->setItem(row, 1, new QTableWidgetItem(PIB));
		ui.tableWidgetBorjnyky->item(row,1)->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
		ui.tableWidgetBorjnyky->setItem(row, 2, new QTableWidgetItem(query->value(4).toString()));
		ui.tableWidgetBorjnyky->item(row,2)->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
		ui.tableWidgetBorjnyky->setItem(row, 3, new QTableWidgetItem(uMToStr2(query->value(5).toDouble())));
		ui.tableWidgetBorjnyky->item(row,3)->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
		ui.tableWidgetBorjnyky->setCellWidget(row,4,new QCheckBox(codec->toUnicode("друкувати")));
		row++;
		pForm->incPos();
	}
	ui.tableWidgetBorjnyky->resizeColumnsToContents();
	ui.tableWidgetBorjnyky->resizeRowsToContents();
	
	delete abonentsQuery;
	delete query;
	delete pForm;
}
//------------------------------------------------------------
void tkeFormPrintPoshtaDlyaBorjnykiv::selectAllBorjnyk()
{
	for (int i=0; i<ui.tableWidgetBorjnyky->rowCount(); i++)
		(dynamic_cast<QCheckBox*>(ui.tableWidgetBorjnyky->cellWidget(i, 4)))->setChecked(true);
}
//------------------------------------------------------------
void tkeFormPrintPoshtaDlyaBorjnykiv::deSelectAllBorjnyk()
{
	for (int i=0; i<ui.tableWidgetBorjnyky->rowCount(); i++)
		(dynamic_cast<QCheckBox*>(ui.tableWidgetBorjnyky->cellWidget(i, 4)))->setChecked(false);
}
//------------------------------------------------------------
tkeFormPrintPoshtaDlyaBorjnykiv::~tkeFormPrintPoshtaDlyaBorjnykiv()
{

}
//------------------------------------------------------------
