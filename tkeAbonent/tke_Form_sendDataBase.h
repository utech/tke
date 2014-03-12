//------------------------------------------------------------
//
// Declaration of tke_Form_sendDataBase class.
// Created by Roman Mulyk 04.08.2009
//
//------------------------------------------------------------

#ifndef TKE_FORM_SENDDATABASE_H
#define TKE_FORM_SENDDATABASE_H

#include <QWidget>
#include <QFile>
#include <QFtp>

#include "ui_tke_Form_sendDataBase.h"
//------------------------------------------------------------
class tke_Form_sendDataBase : public QWidget
{
	Q_OBJECT
  public:
	tke_Form_sendDataBase( QWidget * parent = 0);
	virtual ~tke_Form_sendDataBase();
  private slots:
	void pushButton_sendDatabase_clicked();
	void progressUploading(qint64 done, qint64 total);
	void uploadingFinished(bool error);
	void currentOperation(int operId);
	void carefullClose();
  private:
	Ui::tke_Form_sendDataBase ui;
	QFile *file;
	QFtp *ftp;
};
//------------------------------------------------------------
#endif
