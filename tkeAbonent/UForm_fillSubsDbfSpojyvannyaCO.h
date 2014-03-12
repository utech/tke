//------------------------------------------------------------
//
// Implamantation of UForm_fillSubsDbfSpojyvannyaCO class.
// Created by Roman Mulyk 31.05.2010
//
//------------------------------------------------------------

#ifndef UFORM_FILLSUBSDBFSPOJEVANNYACO_H
#define UFORM_FILLSUBSDBFSPOJEVANNYACO_H

#include "ui_UForm_fillSubsDbfSpojyvannyaCO.h"

#include <QWidget>
#include <QTableWidgetItem>
#include <xbase64.h>

class UForm_fillSubsDbfSpojyvannyaCO : public QWidget
{
Q_OBJECT
  public:
	UForm_fillSubsDbfSpojyvannyaCO(QWidget * parent = 0);
	~UForm_fillSubsDbfSpojyvannyaCO();
	void insertResult(int row, QString text);
	short tochnistDiuchogoTaryfu();
	
  public slots:
	void toolButton_browse_clicked();
	void pushButton_fillFile_clicked();
	void tableWidget_itemChanged(QTableWidgetItem * item);
	void tableWidget_sectionClicked(int section);
  private:
	Ui::UForm_fillSubsDbfSpojyvannyaCO ui;
};

#endif




