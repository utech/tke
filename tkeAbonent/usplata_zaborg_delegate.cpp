/****************************************************************************
**
** Created: Roman Mulyk 17.08.2006
**
****************************************************************************/

#include "usplata_zaborg_delegate.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USplata_zaborg_Delegate::USplata_zaborg_Delegate(QObject *parent)
    : QSqlRelationalDelegate(parent)
{
    durationColumn_1 = 2;
	durationColumn_2 = 3;
	durationColumn_3 = 4;
	durationColumn_4 = 5;
	durationColumn_5 = 6;
	durationColumn_6 = 7;
	durationColumn_7 = 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void USplata_zaborg_Delegate::paint(QPainter *painter,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    if ( index.column() == durationColumn_2 ){
        double number = index.model()->data(index, Qt::DisplayRole).toDouble();
        QString text = uMToStr2(number);
        QStyleOptionViewItem myOption = option;
        myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
        drawDisplay(painter, myOption, myOption.rect, text);
        drawFocus(painter, myOption, myOption.rect);
    } else{
        QItemDelegate::paint(painter, option, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QWidget *USplata_zaborg_Delegate::createEditor(QWidget *parent,
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
	else if (index.column() == durationColumn_2){
        QDoubleSpinBox *double_spinbox = new QDoubleSpinBox(parent);
        double_spinbox->setMaximum ( 99999 );
		double_spinbox->setMinimum ( 1 );
        connect(double_spinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return double_spinbox;
    }
	else if ((index.column() == durationColumn_3) || (index.column() == durationColumn_5)) {
        spinbox = new QSpinBox(parent);
        spinbox->setMaximum ( 12 );
		spinbox->setMinimum ( 1 );
        connect(spinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return spinbox;
    }
	else if ((index.column() == durationColumn_4) || (index.column() == durationColumn_6)) {
        spinbox = new QSpinBox(parent);
        spinbox->setMaximum ( 2100 );
		spinbox->setMinimum ( 2004 );
        connect(spinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return spinbox;
    }
    else if (index.column() == durationColumn_7) {
        QSpinBox *spinbox = new QSpinBox(parent);
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
void USplata_zaborg_Delegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
	QSpinBox *spinbox;
	int value;
    if ((index.column() == durationColumn_1)
		|| (index.column() == durationColumn_3)
		|| (index.column() == durationColumn_4)
		|| (index.column() == durationColumn_5)
		|| (index.column() == durationColumn_6)
		|| (index.column() == durationColumn_7)) {
			value = index.model()->data(index, Qt::DisplayRole).toInt();
			spinbox = qobject_cast<QSpinBox *>(editor);
			spinbox->setValue(value);
			spinbox->selectAll();
    }
	else if (index.column() == durationColumn_2) {
        double sum = index.model()->data(index, Qt::DisplayRole).toDouble();
        QDoubleSpinBox *double_spinbox = qobject_cast<QDoubleSpinBox *>(editor);
        double_spinbox->setValue(sum);
		double_spinbox->selectAll();
    }
	else {
        QSqlRelationalDelegate::setEditorData(editor, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void USplata_zaborg_Delegate::setModelData(QWidget *editor,
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
	else if (index.column() == durationColumn_2) {
        QDoubleSpinBox *double_spinbox = qobject_cast<QDoubleSpinBox *>(editor);
		double sum = double_spinbox->value();
        model->setData(index, sum);
    }
	else if ((index.column() == durationColumn_3) || (index.column() == durationColumn_5)) {
        spinbox = qobject_cast<QSpinBox *>(editor);
		value= spinbox->value();
        model->setData(index, value);
    }
	else if ((index.column() == durationColumn_4) || (index.column() == durationColumn_6)) {
        spinbox = qobject_cast<QSpinBox *>(editor);
		value= spinbox->value();
        model->setData(index, value);
    }
	else {
        QSqlRelationalDelegate::setModelData(editor, model, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void USplata_zaborg_Delegate::commitAndCloseEditor()
{
    QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

