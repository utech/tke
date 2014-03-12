/****************************************************************************
**
** Created: Roman Mulyk 20.01.2007
**
****************************************************************************/

#include "uform_kvytancii_doc_delegate.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UFormKvytanciiDocDelegate::UFormKvytanciiDocDelegate(QObject *parent)
    : QItemDelegate(parent)
{
    durationColumn_1 = 1;
	durationColumn_2 = 2;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UFormKvytanciiDocDelegate::paint(QPainter *painter,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
	QStyleOptionViewItem myOption = option;
    if ( index.column() == durationColumn_2 ){
		QDate date( index.model()->data(index, Qt::DisplayRole).toDate() );
        QString text = QString("%1.%2.%3")
                       .arg(QVariant(date.day()).toString())
                       .arg(QVariant(date.month()).toString(), 2, QChar('0'))
					   .arg(QVariant(date.year()).toString(), 4, QChar('-'));
        myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
        drawDisplay(painter, myOption, myOption.rect, text);
        drawFocus(painter, myOption, myOption.rect);
	}
	else if ( index.column() == durationColumn_1 ){
		myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
        QItemDelegate::paint(painter, myOption, index);
    }
	else{
        QItemDelegate::paint(painter, option, index);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

