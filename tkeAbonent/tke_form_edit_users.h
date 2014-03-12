//------------------------------------------------------------
//
// Declaration of tkeFormEditUsers class.
// Created by Roman Mulyk 19.06.2007
//
//------------------------------------------------------------

#ifndef TKE_FORM_EDIT_USERS
#define TKE_FORM_EDIT_USERS

#include <QWidget>
#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QLineEdit>
#include <QSpinBox>
#include <QSqlTableModel>

#include "ui_tke_form_edit_users.h"
//------------------------------------------------------------
class tkeFormEditUsers : public QDialog
{
Q_OBJECT
public:
	tkeFormEditUsers( QWidget * parent = 0);
	~tkeFormEditUsers();
private:
	Ui::tke_Dialog_edit_users ui;

	QTextCodec *codec;
	QSqlTableModel *tableModel;
private slots:
	void pushButton_add_user_clicked();
	void pushButton_dell_user_clicked();
};
//------------------------------------------------------------
class tkeFormEditUsersDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    tkeFormEditUsersDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;
private:
	int durationColumn_1;
	
	
signals:
	void endEdit();
};
//------------------------------------------------------------
#endif




