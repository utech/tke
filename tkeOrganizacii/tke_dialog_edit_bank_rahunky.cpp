//------------------------------------------------------------
//
// Implamantation of tke_MainWindow class.
// Created by Roman Mulyk 16.11.2014
//
//------------------------------------------------------------

#include "tke_dialog_edit_bank_rahunky.h"

#include <USqlAccessible>

//------------------------------------------------------------
tke_dialog_edit_bank_rahunky::tke_dialog_edit_bank_rahunky(QWidget *parent)
        : QDialog(parent)
{
	ui.setupUi(this);
	
    ui.tableWidget->setColumnHidden(0, true);
    ui.tableWidget->setColumnWidth(1, 120); // TypeName
    ui.tableWidget->setColumnWidth(2, 300); // BankName
    ui.tableWidget->setColumnWidth(3, 100); // Rahunok
    ui.tableWidget->setColumnWidth(4, 90); // Mfo

    populateTable();

    connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(close()));
}
//------------------------------------------------------------
tke_dialog_edit_bank_rahunky::~tke_dialog_edit_bank_rahunky()
{

}
//------------------------------------------------------------
void tke_dialog_edit_bank_rahunky::populateTable()
{
    disconnect(ui.tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(tableWidget_itemChanged(QTableWidgetItem*)));

    QSqlQuery q;
    q.exec("SELECT count(*) FROM bankRahunky");
    q.next();
    int iRowCount = q.value(0).toInt();
    ui.tableWidget->setRowCount(iRowCount);

    q.exec("SELECT RahTypeId, TypeName, BankName, Rahunok, Mfo FROM bankRahunky ORDER by id");

    for (int i = 0; q.next() && i < iRowCount; i++)
    {
        QTableWidgetItem* item;

        // RahTypeId
        item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole, q.value(0));
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui.tableWidget->setItem(i, 0, item);

        // TypeName
        item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole, q.value(1));
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui.tableWidget->setItem(i, 1, item);

        // BankName
        item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole, q.value(2));
        ui.tableWidget->setItem(i, 2, item);

        // Rahunok
        item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole, q.value(3));
        ui.tableWidget->setItem(i, 3, item);

        // Mfo
        item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole, q.value(4));
        ui.tableWidget->setItem(i, 4, item);
    }

    connect(ui.tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(tableWidget_itemChanged(QTableWidgetItem*)));
}
//------------------------------------------------------------
void tke_dialog_edit_bank_rahunky::tableWidget_itemChanged(QTableWidgetItem* item)
{
    disconnect(ui.tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(tableWidget_itemChanged(QTableWidgetItem*)));

    QSqlQuery q;
    int iRahTypeID = ui.tableWidget->item(item->row(), 0)->data(Qt::DisplayRole).toInt();

    QString strColumnName;

    switch (item->column())
    {
    case 2:
        strColumnName = "BankName";
        break;
    case 3:
        strColumnName = "Rahunok";
        break;
    case 4:
        strColumnName = "Mfo";
        break;
    }

    q.exec(QString("UPDATE bankRahunky SET %1 = %2 WHERE RahTypeId = %3")
           .arg(strColumnName)
           .arg(sqlStr(item->data(Qt::DisplayRole).toString()))
           .arg(iRahTypeID));

    q.exec(QString("SELECT %1 FROM bankRahunky WHERE RahTypeId = %2")
           .arg(strColumnName)
           .arg(iRahTypeID));
    q.next();
    item->setData(Qt::DisplayRole, q.value(0));

    connect(ui.tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(tableWidget_itemChanged(QTableWidgetItem*)));
}
//------------------------------------------------------------
