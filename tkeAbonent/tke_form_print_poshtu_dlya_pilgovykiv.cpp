//------------------------------------------------------------
//
// Implamantation of tableWidgetPilgovyky class.
// Created by Roman Mulyk 4.08.2007
//
//------------------------------------------------------------
#include <QApplication>
#include "tke_form_print_poshtu_dlya_pilgovykiv.h"

//------------------------------------------------------------
tkeFormPrintPoshtaDlyaPilgovykiv::tkeFormPrintPoshtaDlyaPilgovykiv( QWidget * parent) 
	: QDialog(parent)
{
	ui.setupUi(this);
	codec = QTextCodec::codecForName("cp1251");

	prnt = new QPrinter();
	
	connect(ui.pushButton_reload, SIGNAL(clicked()), this, SLOT(populateTableWidjetPilgovyky()));
	connect(ui.pushButton_selectAll, SIGNAL(clicked()), this, SLOT(selectAllBorjnyk()));
	connect(ui.pushButton_deselectAll, SIGNAL(clicked()), this, SLOT(deSelectAllBorjnyk()));
	connect(ui.pushButton_printSelected, SIGNAL(clicked()), this, SLOT(printMessages()));
	connect(ui.pushButton_printSelectedOnPrinter, SIGNAL(clicked()), this, SLOT(printMessagesOn()));
}
//------------------------------------------------------------
void tkeFormPrintPoshtaDlyaPilgovykiv::printMessages()
{
	QString *fileName = new QString(QCoreApplication::applicationDirPath ()+"/templates/pilgovykyMessage_1.html");
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
		int iter=0;
		for (int i=0; i<ui.tableWidgetPilgovyky->rowCount(); i++){
			if ((dynamic_cast<QCheckBox*>(ui.tableWidgetPilgovyky->cellWidget(i, 5)))->isChecked()){
				hash.clear();
				hash.insert(QString("Rahunok"), ui.tableWidgetPilgovyky->item(i,0)->text());
				hash.insert(QString("PIB"), ui.tableWidgetPilgovyky->item(i,1)->text());
				hash.insert(QString("Adresa"), ui.tableWidgetPilgovyky->item(i,2)->text());
				parser->setTagsStringsHash(hash);
				parser->convertString();
				
				iter++;
				if (iter!=1)
					pageHTML.append("\n\n\n\n");
				pageHTML.append(parser->curOutputString());
				
				if (iter==4){
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
void tkeFormPrintPoshtaDlyaPilgovykiv::printMessagesOn()
{
	QPrintDialog *dlg = new QPrintDialog(prnt, this);
	if (dlg->exec() == QDialog::Accepted)
			printMessages();
	delete dlg;
}
//------------------------------------------------------------
void tkeFormPrintPoshtaDlyaPilgovykiv::populateTableWidjetPilgovyky()
{
	ui.tableWidgetPilgovyky->setSortingEnabled(false);
	
	QSqlQuery *query = new QSqlQuery;
	QHash<int, QString> pilgyHash;
	query->exec("SELECT id, Kategoriya FROM pilgy");
	while (query->next()){
		pilgyHash.insert(query->value(0).toInt(), query->value(1).toString());
	}
	
	query->exec("SELECT count(*) \
				FROM abonent \
				LEFT JOIN budynky \
					ON budynky.id=abonent.Budynok_ID \
				WHERE abonent.opal=true and abonent.Pilga_ID>0");
	query->seek(0);
	int rowCount = query->value(0).toInt();
	QString str, tStr;
	ui.tableWidgetPilgovyky->setRowCount(rowCount);
	query->exec("SELECT abonent.Rahunok, abonent.Prizv, abonent.Imya, abonent.Batk, budynky.Vulycia, \
						budynky.Bud_num, abonent.Kvartyra, abonent.Pilga_ID, abonent.Pilga_for, \
						abonent.Pilga_ID_2, abonent.Pilga_for_2, abonent.Pilgy_zdani  \
				FROM abonent \
				LEFT JOIN budynky \
					ON budynky.id=abonent.Budynok_ID \
				WHERE abonent.opal=true and abonent.Pilga_ID>0");
	for (int row=0; (row<rowCount) && query->next(); row++){
		QTableWidgetItem *item = new QTableWidgetItem(query->value(0).toString());
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		ui.tableWidgetPilgovyky->setItem(row, 0, item);
		str = query->value(1).toString()+" ";
		tStr = query->value(2).toString();
		if (tStr.size()>0)
			str += tStr.at(0)+QString(".");
		tStr = query->value(3).toString();
		if (tStr.size()>0)
			str += tStr.at(0)+QString(".");
		item = new QTableWidgetItem(str);
		item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ui.tableWidgetPilgovyky->setItem(row, 1, item);
		item = new QTableWidgetItem(query->value(4).toString()+" "+query->value(5).toString()+"/"+query->value(6).toString());
		item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ui.tableWidgetPilgovyky->setItem(row, 2, item);
		str=pilgyHash.value(query->value(7).toInt())+" на "+query->value(8).toString();
		if (query->value(9).toInt() > 0)
			str += "; 2)" + pilgyHash.value(query->value(9).toInt())+" на "+query->value(10).toString();
		item = new QTableWidgetItem(str);
		item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ui.tableWidgetPilgovyky->setItem(row, 3, item);
		QCheckBox *chBox = new QCheckBox("друкувати");
		if (query->value(11).toBool()){
			item = new QTableWidgetItem("Здано");
			item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
			ui.tableWidgetPilgovyky->setItem(row, 4, item);
			chBox->setChecked(false);
		}
		else{
			chBox->setChecked(true);
		}
		ui.tableWidgetPilgovyky->setCellWidget(row,5,chBox);
	}
	
	ui.tableWidgetPilgovyky->resizeColumnsToContents();
	ui.tableWidgetPilgovyky->resizeRowsToContents();
	
	delete query;
	
	ui.tableWidgetPilgovyky->setSortingEnabled(true);
}
//------------------------------------------------------------
void tkeFormPrintPoshtaDlyaPilgovykiv::selectAllBorjnyk()
{
	for (int i=0; i<ui.tableWidgetPilgovyky->rowCount(); i++)
		(dynamic_cast<QCheckBox*>(ui.tableWidgetPilgovyky->cellWidget(i, 5)))->setChecked(true);
}
//------------------------------------------------------------
void tkeFormPrintPoshtaDlyaPilgovykiv::deSelectAllBorjnyk()
{
	for (int i=0; i<ui.tableWidgetPilgovyky->rowCount(); i++)
		(dynamic_cast<QCheckBox*>(ui.tableWidgetPilgovyky->cellWidget(i, 5)))->setChecked(false);
}
//------------------------------------------------------------
tkeFormPrintPoshtaDlyaPilgovykiv::~tkeFormPrintPoshtaDlyaPilgovykiv()
{

}
//------------------------------------------------------------
