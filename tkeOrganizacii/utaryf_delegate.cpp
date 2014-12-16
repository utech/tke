/****************************************************************************
**
** Created: Roman Mulyk 27.02.2007
**
****************************************************************************/

#include "utaryf_delegate.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UTaryf_Delegate::UTaryf_Delegate(QObject *parent)
    : QSqlRelationalDelegate(parent)
{
    durationColumn_1 = 1;
	durationColumn_2 = 2;
	durationColumn_3 = 3;
	durationColumn_4 = 4;
	durationColumn_5 = 5;
	durationColumn_6 = 6;
	durationColumn_7 = 7;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UTaryf_Delegate::paint(QPainter *painter,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    if ( (index.column() == durationColumn_3) 
		|| (index.column() == durationColumn_4) 
		|| (index.column() == durationColumn_5) 
		|| (index.column() == durationColumn_6)
		|| (index.column() == durationColumn_7)){
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
QWidget *UTaryf_Delegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{	QSpinBox *spinbox;
    //year_part
    if (index.column() == durationColumn_1) {
        spinbox = new QSpinBox(parent);
        spinbox->setMaximum ( 2050 );
		spinbox->setMinimum ( 2006 );
        connect(spinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return spinbox;
    }
	//month_part
	else if (index.column() == durationColumn_2) {
        spinbox = new QSpinBox(parent);
        spinbox->setMaximum ( 12 );
		spinbox->setMinimum ( 1 );
        connect(spinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return spinbox;
    }
	//taryfy part -- dlya dvoh vydiv cin
	else if ( (index.column() == durationColumn_3) 
				|| (index.column() == durationColumn_4) 
				|| (index.column() == durationColumn_5) 
				|| (index.column() == durationColumn_6)
				|| (index.column() == durationColumn_7)) {
        QDoubleSpinBox *double_spinbox = new QDoubleSpinBox(parent);
        double_spinbox->setMaximum ( 9999999 );
		double_spinbox->setMinimum ( 0 );
        connect(double_spinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return double_spinbox;
    }
	else {
        return QSqlRelationalDelegate::createEditor(parent, option, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UTaryf_Delegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
	QSpinBox *spinbox;
	//year_part
    if (index.column() == durationColumn_1) {
        short year = index.model()->data(index, Qt::DisplayRole).toInt();
        spinbox = qobject_cast<QSpinBox *>(editor);
        spinbox->setValue(year);
		spinbox->selectAll();
    }
	//month_part
	else if (index.column() == durationColumn_2) {
        unsigned char month = index.model()->data(index, Qt::DisplayRole).toInt();
        spinbox = qobject_cast<QSpinBox *>(editor);
        spinbox->setValue(month);
		spinbox->selectAll();
    }
	//taryfy part -- dlya dvoh vydiv cin
	else if ((index.column() == durationColumn_3) 
				|| (index.column() == durationColumn_4) 
				|| (index.column() == durationColumn_5) 
				|| (index.column() == durationColumn_6)
				|| (index.column() == durationColumn_7)) {
        double value = index.model()->data(index, Qt::DisplayRole).toDouble();
        QDoubleSpinBox *double_spinbox = qobject_cast<QDoubleSpinBox *>(editor);
        double_spinbox->setValue(value);
		double_spinbox->selectAll();
    }
	else {
        QSqlRelationalDelegate::setEditorData(editor, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UTaryf_Delegate::setModelData(QWidget *editor,
                                 QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
	QSpinBox *spinbox;
	//year_part
    if (index.column() == durationColumn_1) {
        spinbox = qobject_cast<QSpinBox *>(editor);
        short year = spinbox->value();
        model->setData(index, year);
    }
	//month_part
	else if (index.column() == durationColumn_2) {
        spinbox = qobject_cast<QSpinBox *>(editor);
        unsigned char month = spinbox->value();
        model->setData(index, month);
    }
	//taryfy part -- dlya dvoh vydiv cin
	else if ((index.column() == durationColumn_3) 
			|| (index.column() == durationColumn_4) 
			|| (index.column() == durationColumn_5) 
			|| (index.column() == durationColumn_6)
			|| (index.column() == durationColumn_7)) {
        QDoubleSpinBox *double_spinbox = qobject_cast<QDoubleSpinBox *>(editor);
		double value= double_spinbox->value();
        model->setData(index, value);
    }
	else {
        QSqlRelationalDelegate::setModelData(editor, model, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UTaryf_Delegate::commitAndCloseEditor()
{
    QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

