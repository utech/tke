/****************************************************************************
**
** Created: Roman Mulyk 01.04.2007
**
****************************************************************************/

#include "uoplata_delegate.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UOplata_Delegate::UOplata_Delegate(QObject *parent)
    : QSqlRelationalDelegate(parent)
{
    durationColumn_1 = 1;
	durationColumn_2 = 2;
	durationColumn_3 = 3;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UOplata_Delegate::paint(QPainter *painter,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    if ( index.column() == durationColumn_3 ){
        double number = index.model()->data(index, Qt::DisplayRole).toDouble();
        QString text = uMToStr2(number);
        QStyleOptionViewItem myOption = option;
        myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
        drawDisplay(painter, myOption, myOption.rect, text);
        drawFocus(painter, myOption, myOption.rect);
    } 
	else if (index.column() == durationColumn_2){
		QDate date( index.model()->data(index, Qt::DisplayRole).toDate() );
        QString text = QString("%1.%2.%3")
                       .arg(QVariant(date.day()).toString())
                       .arg(QVariant(date.month()).toString(), 2, QChar('0'))
					   .arg(QVariant(date.year()).toString(), 4, QChar('-'));
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
QWidget *UOplata_Delegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
	QSpinBox *spinbox;
    if (index.column() == durationColumn_3) {
        QDoubleSpinBox *double_spinbox = new QDoubleSpinBox(parent);
        double_spinbox->setMaximum ( 10000000 );
		double_spinbox->setMinimum ( -10000000 );
        connect(double_spinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return double_spinbox;
    } 
	else if (index.column() == durationColumn_2){
        QDateEdit * dateEdit = new QDateEdit(parent);
		QDate date;
		date.setYMD( 2100, 12, 30 );
        dateEdit->setMaximumDate( date );
		date.setYMD( 2004, 1, 1 );
		dateEdit->setMinimumDate( date );
		dateEdit->setDisplayFormat( "d.MM.yyyy" );
        connect(dateEdit, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return dateEdit;
	}
	else if (index.column() == durationColumn_1){
        spinbox = new QSpinBox(parent);
        spinbox->setMaximum ( 100000 );
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
void UOplata_Delegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
    if (index.column() == durationColumn_3) {
        double value = index.model()->data(index, Qt::DisplayRole).toDouble();
        QDoubleSpinBox *double_spinbox = qobject_cast<QDoubleSpinBox *>(editor);
        double_spinbox->setValue(value);
		double_spinbox->selectAll();
    }
	else if (index.column() == durationColumn_2){
        QDateEdit * dateEdit = qobject_cast<QDateEdit *>(editor);
        dateEdit->setDate( index.model()->data(index, Qt::DisplayRole).toDate() );
		dateEdit->setCurrentSection( QDateTimeEdit::DaySection );
		dateEdit->stepBy( 0 );
	}
	else if (index.column() == durationColumn_1){
		int value = index.model()->data(index, Qt::DisplayRole).toInt();
        QSpinBox *spinbox = qobject_cast<QSpinBox *>(editor);
        spinbox->setValue(value);
		spinbox->selectAll();
	}
	else {
        QSqlRelationalDelegate::setEditorData(editor, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UOplata_Delegate::setModelData(QWidget *editor,
                                 QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
    if (index.column() == durationColumn_3){
        QDoubleSpinBox *double_spinbox = qobject_cast<QDoubleSpinBox *>(editor);
		double value= double_spinbox->value();
        model->setData(index, value);
    }
	else if (index.column() == durationColumn_2){
        QDateEdit * dateEdit = qobject_cast<QDateEdit *>(editor);
        model->setData(index, dateEdit->date());
	}
	else if (index.column() == durationColumn_1)
	{
        QSpinBox *spinbox = qobject_cast<QSpinBox *>(editor);
		int value = spinbox->value();
        model->setData(index, value);
	}
	else {
        QSqlRelationalDelegate::setModelData(editor, model, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UOplata_Delegate::commitAndCloseEditor()
{
    QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

