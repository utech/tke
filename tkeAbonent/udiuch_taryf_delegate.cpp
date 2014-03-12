/****************************************************************************
**
** Created: Roman Mulyk 18.09.2006
**
****************************************************************************/

#include "udiuch_taryf_delegate.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UDiuch_taryf_Delegate::UDiuch_taryf_Delegate(QObject *parent, unsigned char days_in_month)
    : QItemDelegate(parent)
{
    durationColumn_1 = 2;
	durationColumn_2 = 3;
	durationColumn_3 = 4;
	durationColumn_4 = 5;
	durationColumn_5 = 7;
	days = days_in_month;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QWidget *UDiuch_taryf_Delegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{	QSpinBox *spinbox;
    //not lichylnyk (days part) 
    if (index.column() == durationColumn_1) {
        spinbox = new QSpinBox(parent);
        spinbox->setMaximum ( days );
		spinbox->setMinimum ( 0 );
        connect(spinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return spinbox;
    }
	//lichylnyk part 
	else if ( (index.column() == durationColumn_2) || (index.column() == durationColumn_3) || (index.column() == durationColumn_5)) {
        QDoubleSpinBox *doubleSpinbox = new QDoubleSpinBox(parent);
        doubleSpinbox->setMaximum ( 999999999 );
		doubleSpinbox->setMinimum ( 0 );
		doubleSpinbox->setDecimals ( 5 );
        connect(doubleSpinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return doubleSpinbox;
    }
	//taryf part
	else if ( index.column() == durationColumn_4 ) {
        QDoubleSpinBox *double_spinbox = new QDoubleSpinBox(parent);
        double_spinbox->setMaximum ( 99999 );
		double_spinbox->setMinimum ( 0 );
        connect(double_spinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return double_spinbox;
    }
	else {
        return QItemDelegate::createEditor(parent, option, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UDiuch_taryf_Delegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
	QSpinBox *spinbox;
	//not lichylnyk (days part) 
    if (index.column() == durationColumn_1) {
        short num_days = index.model()->data(index, Qt::DisplayRole).toInt();
        spinbox = qobject_cast<QSpinBox *>(editor);
        spinbox->setValue(num_days);
		spinbox->selectAll();
    }
	//lichylnyk part 
	else if ( (index.column() == durationColumn_2) || (index.column() == durationColumn_3) || (index.column() == durationColumn_5)) {
        double lich = index.model()->data(index, Qt::DisplayRole).toDouble();
        QDoubleSpinBox *doubleSpinbox = qobject_cast<QDoubleSpinBox *>(editor);
        doubleSpinbox->setValue(lich);
		doubleSpinbox->selectAll();
    }
	//taryf part
	else if (index.column() == durationColumn_4) {
        double value = index.model()->data(index, Qt::DisplayRole).toDouble();
        QDoubleSpinBox *double_spinbox = qobject_cast<QDoubleSpinBox *>(editor);
        double_spinbox->setValue(value);
		double_spinbox->selectAll();
    }
	else {
        QItemDelegate::setEditorData(editor, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UDiuch_taryf_Delegate::setModelData(QWidget *editor,
                                 QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
	QSpinBox *spinbox;
	//not lichylnyk (days part) 
    if (index.column() == durationColumn_1) {
        spinbox = qobject_cast<QSpinBox *>(editor);
        unsigned char days = spinbox->value();
        model->setData(index, days);
    }
	//lichylnyk part 
	else if ( (index.column() == durationColumn_2) || (index.column() == durationColumn_3) || (index.column() == durationColumn_5)) {
        QDoubleSpinBox *doubleSpinbox = qobject_cast<QDoubleSpinBox *>(editor);
        double lich = doubleSpinbox->value();
        model->setData(index, lich);
    }
	//taryf part
	else if (index.column() == durationColumn_4) {
        QDoubleSpinBox *double_spinbox = qobject_cast<QDoubleSpinBox *>(editor);
		double value= double_spinbox->value();
        model->setData(index, value);
    }
	else {
        QItemDelegate::setModelData(editor, model, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UDiuch_taryf_Delegate::commitAndCloseEditor()
{
    QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

