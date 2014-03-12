/****************************************************************************
**
** Created: Roman Mulyk 17.08.2006
**
****************************************************************************/

#include "ukorektura_delegate.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UKorektura_Delegate::UKorektura_Delegate(QObject *parent)
    : QSqlRelationalDelegate(parent)
{
    durationColumn_1 = 2;
	durationColumn_2 = 3;
	durationColumn_3 = 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UKorektura_Delegate::paint(QPainter *painter,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    if ((index.column() == durationColumn_1) || (index.column() == durationColumn_2)) {
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
QWidget *UKorektura_Delegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{	
    if (index.column() == durationColumn_1) {
        QDoubleSpinBox *doublespinbox = new QDoubleSpinBox(parent);
        doublespinbox->setMaximum ( 200 );
		doublespinbox->setMinimum ( 1 );
        connect(doublespinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return doublespinbox;
    }
	else if (index.column() == durationColumn_2) {
        QDoubleSpinBox *spinbox = new QDoubleSpinBox(parent);
        spinbox->setMaximum ( 100 );
		spinbox->setMinimum ( 1 );
        connect(spinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return spinbox;
    }
    else if (index.column() == durationColumn_3) {
        QSpinBox *spinbox = new QSpinBox(parent);
        spinbox->setMaximum ( 999999 );
		spinbox->setMinimum ( 1 );
        connect(spinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return spinbox;
    }
	else {
        return QSqlRelationalDelegate::createEditor(parent, option, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UKorektura_Delegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
    if ((index.column() == durationColumn_1) || (index.column() == durationColumn_2)) {
        double number = index.model()->data(index, Qt::DisplayRole).toDouble();
        QDoubleSpinBox *doublespinbox = qobject_cast<QDoubleSpinBox *>(editor);
        doublespinbox->setValue(number);
		doublespinbox->selectAll();
    } 
	else if ((index.column() == durationColumn_3)) {
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
void UKorektura_Delegate::setModelData(QWidget *editor,
                                 QAbstractItemModel *model,
                                 const QModelIndex &index) const
{

    QSqlRelationalDelegate::setModelData(editor, model, index);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UKorektura_Delegate::commitAndCloseEditor()
{
    QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

