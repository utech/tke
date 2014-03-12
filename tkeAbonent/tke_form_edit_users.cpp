//------------------------------------------------------------
//
// Implamantation of tkeFormEditUsers class.
// Created by Roman Mulyk 19.06.2007
//
//------------------------------------------------------------

#include <QMessageBox>
#include <QTextCodec>
#include <QtSql>
#include <QVariant>
#include <QSettings>

#include "tke_form_edit_users.h"
//------------------------------------------------------------
tkeFormEditUsers::tkeFormEditUsers( QWidget * parent) 
	: QDialog(parent)
{
	ui.setupUi(this);
	codec = QTextCodec::codecForName("cp1251");
	
	setMaximumWidth(width());
	setMinimumWidth(width());
	setMaximumHeight(height());
	setMinimumHeight(height());
	
	tableModel = new QSqlTableModel(this);
	ui.tableView_sys_users->setAlternatingRowColors(true);
	tableModel->setTable("users");
	tableModel->setHeaderData(1, Qt::Horizontal, codec->toUnicode("Ім'я користувача") );
	tableModel->setHeaderData(2, Qt::Horizontal, codec->toUnicode("Пароль") );
	tableModel->setHeaderData(3, Qt::Horizontal, codec->toUnicode("Системний_ІД") );

	tableModel->select();
	tableModel->setEditStrategy(QSqlTableModel::OnRowChange);
			
	ui.tableView_sys_users->setModel(tableModel);
	ui.tableView_sys_users->setColumnHidden(0, true);
	ui.tableView_sys_users->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.tableView_sys_users->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableView_sys_users->setItemDelegate(new tkeFormEditUsersDelegate(ui.tableView_sys_users));
	ui.tableView_sys_users->resizeColumnsToContents();
	
	connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui.pushButton_add_user, SIGNAL(clicked()), this, SLOT(pushButton_add_user_clicked()));
	connect(ui.pushButton_dell_user, SIGNAL(clicked()), this, SLOT(pushButton_dell_user_clicked()));
}
//------------------------------------------------------------
void tkeFormEditUsers::pushButton_add_user_clicked()
{
	int row = tableModel->rowCount();
	tableModel->insertRow(row);
	
	QModelIndex index = tableModel->index(row, 1);
	ui.tableView_sys_users->setCurrentIndex(index);
	ui.tableView_sys_users->edit(index);
}
//------------------------------------------------------------
void tkeFormEditUsers::pushButton_dell_user_clicked()
{
if	( ! QMessageBox::question(
                this,
                codec->toUnicode("!!! Видалення користувача !!!"),
                codec->toUnicode("Ви дійсно бажаєте видалити користувача?"),
                codec->toUnicode("Так"),
				codec->toUnicode("Ні-ні-ні"),
                QString(), 0, 1)
		)
		tableModel->removeRow(ui.tableView_sys_users->currentIndex().row());
}
//------------------------------------------------------------
tkeFormEditUsers::~tkeFormEditUsers()
{
	delete tableModel;
}
//------------------------------------------------------------
tkeFormEditUsersDelegate::tkeFormEditUsersDelegate(QObject *parent)
    : QItemDelegate(parent)
{
	durationColumn_1 = 3;
}
//------------------------------------------------------------
QWidget *tkeFormEditUsersDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem & option,
    const QModelIndex & index) const
{
    	QSpinBox *spinbox;
    if (index.column() == durationColumn_1) {
        spinbox = new QSpinBox(parent);
        spinbox->setMaximum ( 100 );
		spinbox->setMinimum ( 1 );
        connect(spinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return spinbox;
    }
	else {
        return QItemDelegate::createEditor(parent, option, index);
    }
}
//------------------------------------------------------------
void tkeFormEditUsersDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
	if (index.column() == durationColumn_1){
        int number = index.model()->data(index, Qt::DisplayRole).toInt();
        QSpinBox *spinbox = qobject_cast<QSpinBox *>(editor);
        spinbox->setValue(number);
		spinbox->selectAll();
    } else {
        QItemDelegate::setEditorData(editor, index);
    }
}
//------------------------------------------------------------
void tkeFormEditUsersDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
   	if (index.column() == durationColumn_1){
        QSpinBox *spinbox = qobject_cast<QSpinBox *>(editor);
		int number = spinbox->value();
        model->setData(index, number);
    } else {
        QItemDelegate::setModelData(editor, model, index);
    }
}
//------------------------------------------------------------
