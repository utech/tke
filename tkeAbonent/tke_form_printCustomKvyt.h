//--------------------utech--------------------utech--------------------utech--------------------
//
// Файл декларації класу вікна реєстрації користувача
//
// Створений програмістами Компанії "Утех" 13.09.2010р.
// У класі використовуються компоненти бібліотеки QT4.x
//
//--------------------utech--------------------utech--------------------utech--------------------

#ifndef tke_form_printCustomKvyt_H
#define tke_form_printCustomKvyt_H

#include <QtGui>
#include <QSettings>
#include <QDir>
#include <QtSql>

#include "ui_tke_form_printCustomKvyt.h"
//--------------------utech--------------------utech--------------------utech--------------------
class tke_form_printCustomKvyt : public QWidget
{
  Q_OBJECT
  public:
	tke_form_printCustomKvyt(QDate cDate = QDate::currentDate(), QWidget *parent = 0);
	~tke_form_printCustomKvyt();
	
	void initDovidnyky();
	static bool setKvytPrinted(int rah, QDate date);
	static bool setKvytNotPrinted(int rah, QDate date);
  public slots:
	void populateAbonentyTable();
	void clearPryvatBudFilter();
	void clearPrintedState();
	void printSelected();
	void setTableWidgetSorting(int clickedColumn);
	
	void markStoyakFilter();
	void markKotelniFilter();
	void markBudynkyFilter();
	
	void action_setSelectedAsPrinted_activated();
	void action_setSelectedAsNotPrinted_activated();
	
	void tableWidget_abonenty_itemSelectionChanged();
  signals:
    void mainWindowClosed();
  private:
	Ui::tke_form_printCustomKvyt ui;
	
	bool twSortOrderAsc;
};
//--------------------utech--------------------utech--------------------utech--------------------
#endif
