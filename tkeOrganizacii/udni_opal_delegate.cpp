/****************************************************************************
**
** Created: Roman Mulyk 18.03.2007
**
****************************************************************************/

#include "udni_opal_delegate.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UDniOpalDelegate::UDniOpalDelegate(QObject *parent)
    : QSqlRelationalDelegate(parent)
{
	durationColumn_1 = 3;
	durationColumn_2 = 4;
	durationColumn_3 = 5;
	
	//initialize days_in_month array
	days_in_month[0]=31;
	days_in_month[1]=28;
	days_in_month[2]=31;
	days_in_month[3]=30;
	days_in_month[4]=31;
	days_in_month[5]=30;
	days_in_month[6]=31;
	days_in_month[7]=31;
	days_in_month[8]=30;
	days_in_month[9]=31;
	days_in_month[10]=30;
	days_in_month[11]=31;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QWidget *UDniOpalDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
	if ( (index.column() == durationColumn_1) || (index.column() == durationColumn_2) ) {
        QSpinBox *Spinbox = new QSpinBox(parent);
        Spinbox->setMaximum ( days_in_month[index.model()->index(index.row(), 2).data().toDate().month()-1] );
		Spinbox->setMinimum ( 1 );
        connect(Spinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return Spinbox;
    }
	else {
        return QSqlRelationalDelegate::createEditor(parent, option, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UDniOpalDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
	if ( (index.column() == durationColumn_1) || (index.column() == durationColumn_2) ) {
        int day = index.model()->data(index, Qt::DisplayRole).toInt();
        QSpinBox *spinBox = qobject_cast<QSpinBox *>(editor);
        spinBox->setValue(day);
		spinBox->selectAll();
    }
	else {
        QSqlRelationalDelegate::setEditorData(editor, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UDniOpalDelegate::setModelData(QWidget *editor,
                                 QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
	if ( (index.column() == durationColumn_1) || (index.column() == durationColumn_2) ) {
        QSpinBox *spinBox = qobject_cast<QSpinBox *>(editor);
        int day = spinBox->value();
        model->setData(index, day);
    }
	else {
        QSqlRelationalDelegate::setModelData(editor, model, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UDniOpalDelegate::commitAndCloseEditor()
{
    QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

