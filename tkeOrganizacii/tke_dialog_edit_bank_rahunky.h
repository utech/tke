//------------------------------------------------------------
//
// Implamantation of tke_MainWindow class.
// Created by Roman Mulyk 16.11.2014
//
//------------------------------------------------------------

#ifndef TKE_DIALOG_EDIT_BANK_RAHUNKY_H
#define TKE_DIALOG_EDIT_BANK_RAHUNKY_H
// QT standart includes
#include <QDialog>
#include <QTSql>
#include <QHeaderView>
#include <QMessageBox>

#include "ui_tke_dialog_edit_bank_rahunky.h"
#include <QtDebug>

// Classes
class tke_dialog_edit_bank_rahunky : public QDialog
{
    Q_OBJECT

public:
    tke_dialog_edit_bank_rahunky(QWidget *parent = 0);
    ~tke_dialog_edit_bank_rahunky();
public slots:
    void tableWidget_itemChanged(QTableWidgetItem* item);

private:
    void populateTable();

private:
    Ui::tke_dialog_edit_bank_rahunky ui;

};
#endif
