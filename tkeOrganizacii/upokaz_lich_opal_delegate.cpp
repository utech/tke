/****************************************************************************
**
** Created: Roman Mulyk 27.02.2007
**
****************************************************************************/

#include "upokaz_lich_opal_delegate.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UPokazLichOpalDelegate::UPokazLichOpalDelegate(QObject *parent)
    : QItemDelegate(parent)
{
	durationColumn_2 = 1;
	durationColumn_3 = 2;
	durationColumn_4 = 3;
	durationColumn_5 = 5;
	durationColumn_6 = 6;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QWidget *UPokazLichOpalDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
	//lichylnyk part 
	if ( (index.column() == durationColumn_2) || (index.column() == durationColumn_3) || (index.column() == durationColumn_4) || (index.column() == durationColumn_5) || (index.column() == durationColumn_6)) {
        QDoubleSpinBox *doubleSpinbox = new QDoubleSpinBox(parent);
        doubleSpinbox->setMaximum ( 1000000000 );
		doubleSpinbox->setMinimum ( 0 );
		doubleSpinbox->setDecimals(4);
        connect(doubleSpinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return doubleSpinbox;
    }
	else {
        return QItemDelegate::createEditor(parent, option, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UPokazLichOpalDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
	//lichylnyk part 
	if ( (index.column() == durationColumn_2) || (index.column() == durationColumn_3) || (index.column() == durationColumn_4) || (index.column() == durationColumn_5) || (index.column() == durationColumn_6) ) {
        double lich = index.model()->data(index, Qt::DisplayRole).toDouble();
        QDoubleSpinBox *doubleSpinbox = qobject_cast<QDoubleSpinBox *>(editor);
        doubleSpinbox->setValue(lich);
		doubleSpinbox->selectAll();
    }
	else {
        QItemDelegate::setEditorData(editor, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UPokazLichOpalDelegate::setModelData(QWidget *editor,
                                 QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
	//lichylnyk part 
	if ( (index.column() == durationColumn_2) || (index.column() == durationColumn_3) || (index.column() == durationColumn_4) || (index.column() == durationColumn_5) || (index.column() == durationColumn_6) ) {
        QDoubleSpinBox *doubleSpinbox = qobject_cast<QDoubleSpinBox *>(editor);
        double lich = doubleSpinbox->value();
        model->setData(index, lich);
    }
	else {
        QItemDelegate::setModelData(editor, model, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UPokazLichOpalDelegate::commitAndCloseEditor()
{
    QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

