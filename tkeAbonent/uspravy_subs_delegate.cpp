/****************************************************************************
**
** Created: Roman Mulyk 17.08.2006
**
****************************************************************************/

#include "uspravy_subs_delegate.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USpravy_subs_Delegate::USpravy_subs_Delegate(QObject *parent)
    : QSqlRelationalDelegate(parent)
{
    durationColumn_1 = 2;
	durationColumn_2 = 3;
	durationColumn_3 = 4;
	durationColumn_4 = 5;
	durationColumn_5 = 6;
	durationColumn_6 = 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QWidget *USpravy_subs_Delegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
	QSpinBox *spinbox;
    if (index.column() == durationColumn_1) {
        spinbox = new QSpinBox(parent);
        spinbox->setMaximum ( 999999 );
		spinbox->setMinimum ( 1 );
        connect(spinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return spinbox;
    }
	else if ((index.column() == durationColumn_2) || (index.column() == durationColumn_4)) {
        spinbox = new QSpinBox(parent);
        spinbox->setMaximum ( 12 );
		spinbox->setMinimum ( 1 );
        connect(spinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return spinbox;
    }
	else if ((index.column() == durationColumn_3) || (index.column() == durationColumn_5)) {
        spinbox = new QSpinBox(parent);
        spinbox->setMaximum ( 2100 );
		spinbox->setMinimum ( 2004 );
        connect(spinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return spinbox;
    }
    else if (index.column() == durationColumn_6) {
        spinbox = new QSpinBox(parent);
        spinbox->setMaximum ( 999999 );
		spinbox->setMinimum ( 1 );
        connect(spinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return spinbox;
    }
	else{
        return QSqlRelationalDelegate::createEditor(parent, option, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void USpravy_subs_Delegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
	QSpinBox *spinbox;
	int value;
    if (   (index.column() == durationColumn_1)
		|| (index.column() == durationColumn_2) 
		|| (index.column() == durationColumn_4)
		|| (index.column() == durationColumn_3) 
		|| (index.column() == durationColumn_5)
		|| (index.column() == durationColumn_6)) {
			value = index.model()->data(index, Qt::DisplayRole).toInt();
			spinbox = qobject_cast<QSpinBox *>(editor);
			spinbox->setValue(value);
			spinbox->selectAll();
    }
	else {
        QSqlRelationalDelegate::setEditorData(editor, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void USpravy_subs_Delegate::setModelData(QWidget *editor,
                                 QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
	QSpinBox *spinbox;
	int value;
    if ((index.column() == durationColumn_1)) {
        spinbox = qobject_cast<QSpinBox *>(editor);
		value= spinbox->value();
        model->setData(index, value);
    }
	else if ((index.column() == durationColumn_2) || (index.column() == durationColumn_4)) {
        spinbox = qobject_cast<QSpinBox *>(editor);
		value= spinbox->value();
        model->setData(index, value);
    }
	else if ((index.column() == durationColumn_3) || (index.column() == durationColumn_5)) {
        spinbox = qobject_cast<QSpinBox *>(editor);
		value= spinbox->value();
        model->setData(index, value);
    }
	else {
        QSqlRelationalDelegate::setModelData(editor, model, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void USpravy_subs_Delegate::commitAndCloseEditor()
{
    QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

