/****************************************************************************
**
** Created: Roman Mulyk 17.08.2006
**
****************************************************************************/

#include "ukorektura_s_delegate.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UKorektura_s_Delegate::UKorektura_s_Delegate(QObject *parent)
    : QSqlRelationalDelegate(parent)
{
    durationColumn_1 = 2;
	durationColumn_2 = 1;
	durationColumn_4 = 3;
	durationColumn_3 = 7;
	durationColumn_5 = 4;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UKorektura_s_Delegate::paint(QPainter *painter,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    QString text;
	if (index.column() == durationColumn_1 || index.column() == durationColumn_4 || index.column() == durationColumn_5) {
        double number = index.model()->data(index, Qt::DisplayRole).toDouble();
        text = uMToStr2(number);
        QStyleOptionViewItem myOption = option;
        myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
        drawDisplay(painter, myOption, myOption.rect, text);
        drawFocus(painter, myOption, myOption.rect);
    } 
	else if (index.column() == durationColumn_3){
		int num = index.model()->data(index, Qt::DisplayRole).toInt();
		if (num>2)
			num=1;
		if (num==0)
			text = "חאד.";
		else if (num==1)
			text = "ן³כüדא-1";
		else
			text = "ן³כüדא-2";
		QStyleOptionViewItem myOption = option;
        myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
        drawDisplay(painter, myOption, myOption.rect, text);
        drawFocus(painter, myOption, myOption.rect);
	}
	else{
        QItemDelegate::paint(painter, option, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QWidget *UKorektura_s_Delegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{	
    if (index.column() == durationColumn_1 || index.column() == durationColumn_4 || index.column() == durationColumn_5) {
        QDoubleSpinBox *doublespinbox = new QDoubleSpinBox(parent);
        doublespinbox->setMaximum ( 999999999 );
		doublespinbox->setMinimum ( -999999999 );
        connect(doublespinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return doublespinbox;
    }
    else if (index.column() == durationColumn_2) {
        QSpinBox *spinbox = new QSpinBox(parent);
        spinbox->setMaximum ( 999999 );
		spinbox->setMinimum ( 1 );
        connect(spinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return spinbox;
    }
	else if (index.column() == durationColumn_3){
		QComboBox *combo = new QComboBox(parent);
		combo->addItem("חאד.");
		combo->addItem("ן³כüדא-1");
		combo->addItem("ן³כüדא-2");
		return combo;
	}
	else {
        return QSqlRelationalDelegate::createEditor(parent, option, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UKorektura_s_Delegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
    if (index.column() == durationColumn_1 || index.column() == durationColumn_4 || index.column() == durationColumn_5) {
        double number = index.model()->data(index, Qt::DisplayRole).toDouble();
        QDoubleSpinBox *doublespinbox = qobject_cast<QDoubleSpinBox *>(editor);
        doublespinbox->setValue(number);
		doublespinbox->selectAll();
    }
    else if (index.column() == durationColumn_2) {
        int number = index.model()->data(index, Qt::DisplayRole).toInt();
        QSpinBox *spinbox = qobject_cast<QSpinBox *>(editor);
        spinbox->setValue(number);
		spinbox->selectAll();
    }
	else if (index.column() == durationColumn_3){
		int number = index.model()->data(index, Qt::DisplayRole).toInt();
		if (number>2)
			number=1;
        QComboBox *combo = qobject_cast<QComboBox *>(editor);
		combo->setCurrentIndex(number);
	}
	else {
        QSqlRelationalDelegate::setEditorData(editor, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UKorektura_s_Delegate::setModelData(QWidget *editor,
                                 QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
    if (index.column() == durationColumn_1 || index.column() == durationColumn_4 || index.column() == durationColumn_5) {
        QDoubleSpinBox *doublespinbox = qobject_cast<QDoubleSpinBox *>(editor);
		double number = doublespinbox->value();
        model->setData(index, number);
    }
	else if (index.column() == durationColumn_3){
		QComboBox *combo = qobject_cast<QComboBox *>(editor);
		model->setData(index, combo->currentIndex());
	}
	else {
        QSqlRelationalDelegate::setModelData(editor, model, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UKorektura_s_Delegate::commitAndCloseEditor()
{
    QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

