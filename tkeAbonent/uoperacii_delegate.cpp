/****************************************************************************
**
** Created: Roman Mulyk 17.08.2006
**
****************************************************************************/

#include "uoperacii_delegate.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UOperacii_Delegate::UOperacii_Delegate(QObject *parent)
    : QItemDelegate(parent)
{
    //durationColumn_1 = 4;
	//durationColumn_2 = 5;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*void UOperacii_Delegate::paint(QPainter *painter,
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
QWidget *UOperacii_Delegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{	
	QDoubleSpinBox *doublespinbox = new QDoubleSpinBox(parent);
    doublespinbox->setMaximum ( 999999999 );
	doublespinbox->setMinimum ( -999999999 );
    connect(doublespinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
    return doublespinbox;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UOperacii_Delegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
    double number = index.model()->data(index, Qt::DisplayRole).toDouble();
    QDoubleSpinBox *doublespinbox = qobject_cast<QDoubleSpinBox *>(editor);
    doublespinbox->setValue(number);
	doublespinbox->selectAll();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*void UOperacii_Delegate::setModelData(QWidget *editor,
                                 QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
    if ((index.column() == durationColumn_1) || (index.column() == durationColumn_2)) {
        QDoubleSpinBox *doublespinbox = qobject_cast<QDoubleSpinBox *>(editor);
		double number = doublespinbox->value();
        model->setData(index, number);
    } else {
        QItemDelegate::setModelData(editor, model, index);
    }
}*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UOperacii_Delegate::commitAndCloseEditor()
{
    QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

