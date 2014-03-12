/****************************************************************************
**
** Created: Roman Mulyk 18.09.2006
**
****************************************************************************/

#include "uinsert_kvyt_ob_delegate.h"
#include "tke_math.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UInsert_kvyt_ob_Delegate::UInsert_kvyt_ob_Delegate(QObject *parent)
    : QItemDelegate(parent)
{
    durationColumn_1 = 0;
	durationColumn_2 = 100;
	durationColumn_3 = 1;
	durationColumn_4 = 2;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UInsert_kvyt_ob_Delegate::paint(QPainter *painter,
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
QWidget *UInsert_kvyt_ob_Delegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{	
	QSpinBox *spinbox;
    //Рахунок
    if (index.column() == durationColumn_1) {
        spinbox = new QSpinBox(parent);
        spinbox->setMaximum ( 999999 );
		spinbox->setMinimum ( 0 );
        connect(spinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return spinbox;
    }
	//Дата оплати
	else if ( (index.column() == durationColumn_2)) {
        QDateEdit *dateEdit = new QDateEdit(parent);
		QDate date = QDate::currentDate();
		dateEdit->setMaximumDate( date );
		if (date.month() == 1)
			date.setYMD(date.year()-1, 12, 1);
		dateEdit->setMinimumDate( date );
		dateEdit->setDisplayFormat( "d.MM.yyyy" );
        connect(dateEdit, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return dateEdit;
    }
	//Сума оплати
	else if ( index.column() == durationColumn_3 ) {
        QDoubleSpinBox *double_spinbox = new QDoubleSpinBox(parent);
        double_spinbox->setMaximum ( 999999999 );
		double_spinbox->setMinimum ( 0 );
        connect(double_spinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return double_spinbox;
    }
    //Касовий апарат
    else if (index.column() == durationColumn_4) {
        spinbox = new QSpinBox(parent);
        spinbox->setMaximum ( 1000000000 );
		spinbox->setMinimum ( 0 );
        connect(spinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return spinbox;
    }
	else {
        return QItemDelegate::createEditor(parent, option, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UInsert_kvyt_ob_Delegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
    //Рахунок
    if (index.column() == durationColumn_1) {
		int value = index.model()->data(index, Qt::DisplayRole).toInt();
        QSpinBox *spinbox = qobject_cast<QSpinBox *>(editor);
        spinbox->setValue(value);
		spinbox->selectAll();
    }
	//Дата оплати
	else if ( (index.column() == durationColumn_2)) {
		QDateEdit * dateEdit = qobject_cast<QDateEdit *>(editor);
        dateEdit->setDate( index.model()->data(index, Qt::DisplayRole).toDate() );
		dateEdit->setCurrentSection( QDateTimeEdit::DaySection );
		dateEdit->stepBy( 0 );
    }
	//Сума оплати
	else if ( index.column() == durationColumn_3 ) {
        double value = index.model()->data(index, Qt::DisplayRole).toDouble();
        QDoubleSpinBox *double_spinbox = qobject_cast<QDoubleSpinBox *>(editor);
        double_spinbox->setValue(value);
		double_spinbox->selectAll();
    }
    //Касовий апарат
    else if (index.column() == durationColumn_4) {
		int value = index.model()->data(index, Qt::DisplayRole).toInt();
        QSpinBox *spinbox = qobject_cast<QSpinBox *>(editor);
        spinbox->setValue(value);
		spinbox->selectAll();
    }
	else {
        QItemDelegate::setEditorData(editor, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UInsert_kvyt_ob_Delegate::setModelData(QWidget *editor,
                                 QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
       //Рахунок
    if (index.column() == durationColumn_1) {
        QSpinBox *spinbox = qobject_cast<QSpinBox *>(editor);
		int value= spinbox->value();
        model->setData(index, value);
    }
	//Дата оплати
	else if ( (index.column() == durationColumn_2)) {
        QDateEdit * dateEdit = qobject_cast<QDateEdit *>(editor);
        model->setData(index, dateEdit->date());
    }
	//Сума оплати
	else if ( index.column() == durationColumn_3 ) {
        QDoubleSpinBox *double_spinbox = qobject_cast<QDoubleSpinBox *>(editor);
		double value= double_spinbox->value();
        model->setData(index, value);
    }
    //Касовий апарат
    else if (index.column() == durationColumn_4) {
        QSpinBox *spinbox = qobject_cast<QSpinBox *>(editor);
		int value= spinbox->value();
        model->setData(index, value);
    }
	else {
        QItemDelegate::setModelData(editor, model, index);
    }
	
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UInsert_kvyt_ob_Delegate::commitAndCloseEditor()
{
    QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

