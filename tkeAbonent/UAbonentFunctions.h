//------------------------------------------------------------
//
// Implamantation of tke_Form_edit_pokazn_lich class.
// Created by Roman Mulyk 18.12.2008
//
//------------------------------------------------------------

#ifndef UABONENTFUNCTIONS_H
#define UABONENTFUNCTIONS_H

#include <QString>

bool znyattya_narahuv_vid_vidkluchenyh(int rahunok, QDate begDate, QDate endDate);
void export_vidomist_narahuvannya_po_lichylnyku(short year, short month, QString fileToSave);

class UxBaseDbfInfo
{
  public:
	UxBaseDbfInfo(const char* codecName);
	char* strToxBaseChar(QString str);
	char* strToxBaseDosChar(QString str);
  private:
	QTextCodec *funcCodec;
};

#endif
