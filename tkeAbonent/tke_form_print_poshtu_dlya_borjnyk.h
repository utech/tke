//------------------------------------------------------------
//
// Declaration of tkeFormPrintPoshtaDlyaBorjnykiv class.
// Created by Roman Mulyk 4.08.2007
//
//------------------------------------------------------------

#ifndef TKE_FORM_PRINT_POSHTA_DLYA_BORJNYKIV_H
#define TKE_FORM_PRINT_POSHTA_DLYA_BORJNYKIV_H

#include <QDialog>
#include <QItemDelegate>
#include <QModelIndex>
#include <QSize>
#include <QLineEdit>
#include <QSpinBox>
#include <QTableWidget>
#include <QTextCodec>
#include <QSqlQuery>
#include <QPrinter>
#include <QPrintDialog>
#include <QFile>
#include <QTextDocument>
#include <QMessageBox>
#include <QByteArray>

#include "ui_tke_form_print_poshtu_dlya_borjnyk.h"
#include "tke_math.h"
#include "../src_other/replaceStrings/UReplaceStringsParser.h"

//------------------------------------------------------------
class tkeFormPrintPoshtaDlyaBorjnykiv : public QDialog
{
	Q_OBJECT
public:
	tkeFormPrintPoshtaDlyaBorjnykiv( QWidget * parent = 0);
	~tkeFormPrintPoshtaDlyaBorjnykiv();
private slots:
	void populateTableWidjetBorjnyky();
	
public slots:
	void selectAllBorjnyk();
	void deSelectAllBorjnyk();
	void printMessages();
	void printMessagesOn();
	
private:
	Ui::tke_form_print_poshtu_dlya_borjnyk ui;

	QTextCodec *codec;
	QPrinter *prnt;
};
//------------------------------------------------------------
#endif




