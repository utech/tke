/****************************************************************************
**
** Created: Roman Mulyk 06.10.2006
**
****************************************************************************/

#include "upilgovyky_delegate.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UPilgovyky_Delegate::UPilgovyky_Delegate(QObject *parent)
    : QSqlRelationalDelegate(parent)
{
    durationColumn_1 = 5;
	durationColumn_2 = 2;
	durationColumn_3 = 3;
	durationColumn_4 = 4;
	durationColumn_5 = 1;
	durationColumn_6 = 9;
	
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*void UNormat_taryf_Delegate::paint(QPainter *painter,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    if ( (index.column() == durationColumn_3) || (index.column() == durationColumn_4) ){
        double number = index.model()->data(index, Qt::DisplayRole).toDouble();
        QString text = uMToStr2(number);
        QStyleOptionViewItem myOption = option;
        myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
        drawDisplay(painter, myOption, myOption.rect, text);
        drawFocus(painter, myOption, myOption.rect);
    } else{
        QItemDelegate::paint(painter, option, index);
    }
}*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QWidget *UPilgovyky_Delegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    //ідентифікаційний код
    if (index.column() == durationColumn_1) {
        QLineEdit *lineEdit = new QLineEdit(parent);
        lineEdit->setInputMask( "9999999999" );
        connect(lineEdit, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return lineEdit;
    }
    else if (index.column() == durationColumn_5) {
        QSpinBox *spinbox = new QSpinBox(parent);
        spinbox->setMaximum ( 999999 );
		spinbox->setMinimum ( 1 );
        connect(spinbox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return spinbox;
    }
    else if (index.column() == durationColumn_6) {
        QSpinBox *spinbox = new QSpinBox(parent);
        spinbox->setMaximum ( 25 );
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
void UPilgovyky_Delegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
	if ( (index.column() == durationColumn_1)
		 || (index.column() == durationColumn_2)
		 || (index.column() == durationColumn_3)
		 || (index.column() == durationColumn_4)){
		QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
		if (lineEdit){
			lineEdit->setText(index.model()->data(index, Qt::DisplayRole).toString());
			lineEdit->selectAll();
		}
	}
	else if ((index.column() == durationColumn_5) || (index.column() == durationColumn_6)) {
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
void UPilgovyky_Delegate::setModelData(QWidget *editor,
                                 QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
        QSqlRelationalDelegate::setModelData(editor, model, index);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UPilgovyky_Delegate::commitAndCloseEditor()
{
    QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

