//------------------------------------------------------------
//
// Declaration of tkeFormPrintPoshtaDlyaPilgovykiv class.
// Created by Roman Mulyk 2.04.2008
//
//------------------------------------------------------------

#ifndef TKE_FORM_PRINT_POSHTA_DLYA_PILGOVYKIV_H
#define TKE_FORM_PRINT_POSHTA_DLYA_PILGOVYKIV_H

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
#include <QCheckBox>

#include "ui_tke_form_print_poshtu_dlya_pilgovykiv.h"
#include "tke_math.h"
#include "../src_other/replaceStrings/UReplaceStringsParser.h"

//------------------------------------------------------------
class tkeFormPrintPoshtaDlyaPilgovykiv : public QDialog
{
	Q_OBJECT
public:
	tkeFormPrintPoshtaDlyaPilgovykiv( QWidget * parent = 0);
	~tkeFormPrintPoshtaDlyaPilgovykiv();
	
public slots:
	void selectAllBorjnyk();
	void deSelectAllBorjnyk();
	void printMessages();
	void printMessagesOn();
	void populateTableWidjetPilgovyky();
	
private:
	Ui::tke_form_print_poshtu_dlya_pilgovykiv ui;

	QTextCodec *codec;
	QPrinter *prnt;
};
//------------------------------------------------------------
#endif




