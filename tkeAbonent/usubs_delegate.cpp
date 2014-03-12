/****************************************************************************
**
** Created: Roman Mulyk 17.08.2006
**
****************************************************************************/

#include "usubs_delegate.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USubs_Delegate::USubs_Delegate(QObject *parent)
    : QSqlRelationalDelegate(parent)
{
    durationColumn_1 = 4;
	durationColumn_2 = 1;
	durationColumn_3 = 5;
	durationColumn_4 = 6;
	durationColumn_5 = 7;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void USubs_Delegate::paint(QPainter *painter,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    if ((index.column() == durationColumn_1) || (index.column() == durationColumn_3) || (index.column() == durationColumn_4) || (index.column() == durationColumn_5) ){
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
QWidget *USubs_Delegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    if ((index.column() == durationColumn_1) || (index.column() == durationColumn_3) || (index.column() == durationColumn_4) || (index.column() == durationColumn_5)) {
        QDoubleSpinBox *double_spinbox = new QDoubleSpinBox(parent);
        double_spinbox->setMaximum ( 999999999 );
		double_spinbox->setMinimum ( -999999999 );
        connect(double_spinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return double_spinbox;
    }
    else if (index.column() == durationColumn_2) {
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
void USubs_Delegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
    if ((index.column() == durationColumn_1) || (index.column() == durationColumn_3) || (index.column() == durationColumn_4) || (index.column() == durationColumn_5)) {
        double value = index.model()->data(index, Qt::DisplayRole).toDouble();
        QDoubleSpinBox *double_spinbox = qobject_cast<QDoubleSpinBox *>(editor);
        double_spinbox->setValue(value);
		double_spinbox->selectAll();
    }
	else if (index.column() == durationColumn_2) {
        int number = index.model()->data(index, Qt::DisplayRole).toInt();
        QSpinBox *spinbox = qobject_cast<QSpinBox *>(editor);
        spinbox->setValue(number);
		spinbox->selectAll();
    }
	else {
        QSqlRelationalDelegate::setEditorData(editor, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void USubs_Delegate::setModelData(QWidget *editor,
                                 QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
    if ((index.column() == durationColumn_1) || (index.column() == durationColumn_3) || (index.column() == durationColumn_4) || (index.column() == durationColumn_5)) {
        QDoubleSpinBox *double_spinbox = qobject_cast<QDoubleSpinBox *>(editor);
		double value= double_spinbox->value();
        model->setData(index, value);
    } else {
        QSqlRelationalDelegate::setModelData(editor, model, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void USubs_Delegate::commitAndCloseEditor()
{
    QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

