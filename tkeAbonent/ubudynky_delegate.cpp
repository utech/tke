/****************************************************************************
**
** Created: Roman Mulyk 17.08.2006
**
****************************************************************************/

#include "ubudynky_delegate.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UBudynky_Delegate::UBudynky_Delegate(QObject *parent)
    : QSqlRelationalDelegate(parent)
{
    durationColumn_1 = 4;
	durationColumn_2 = 5;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*void UBudynky_Delegate::paint(QPainter *painter,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    if (index.column() == durationColumn) {
        int number = index.model()->data(index, Qt::DisplayRole).toInt();
        QString text = QString("%1,%2")
                       .arg((int number), 1, 10, QChar('0'))
                       .arg(secs % 60, 2, 10, QChar('0'));
        QStyleOptionViewItem myOption = option;
        myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
        drawDisplay(painter, myOption, myOption.rect, text);
        drawFocus(painter, myOption, myOption.rect);
    } else{
        QItemDelegate::paint(painter, option, index);
    }
}*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QWidget *UBudynky_Delegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{	
	QDoubleSpinBox *doublespinbox;
    if (index.column() == durationColumn_1) {
        doublespinbox = new QDoubleSpinBox(parent);
		doublespinbox->setDecimals(4);
        doublespinbox->setMaximum ( 100 );
		doublespinbox->setMinimum ( 0.5 );
        connect(doublespinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return doublespinbox;
    }
	else if (index.column() == durationColumn_2) {
        doublespinbox = new QDoubleSpinBox(parent);
        doublespinbox->setMaximum ( 99999 );
		doublespinbox->setMinimum ( 1 );
        connect(doublespinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return doublespinbox;
    }
	else {
        return QSqlRelationalDelegate::createEditor(parent, option, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UBudynky_Delegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
    if ((index.column() == durationColumn_1) || (index.column() == durationColumn_2)) {
        double number = index.model()->data(index, Qt::DisplayRole).toDouble();
        QDoubleSpinBox *doublespinbox = qobject_cast<QDoubleSpinBox *>(editor);
        doublespinbox->setValue(number);
		doublespinbox->selectAll();
    } else {
        QSqlRelationalDelegate::setEditorData(editor, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UBudynky_Delegate::setModelData(QWidget *editor,
                                 QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
    if ((index.column() == durationColumn_1) || (index.column() == durationColumn_2)) {
        QDoubleSpinBox *doublespinbox = qobject_cast<QDoubleSpinBox *>(editor);
		double number = doublespinbox->value();
        model->setData(index, number);
    } else {
        QSqlRelationalDelegate::setModelData(editor, model, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UBudynky_Delegate::commitAndCloseEditor()
{
    QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

