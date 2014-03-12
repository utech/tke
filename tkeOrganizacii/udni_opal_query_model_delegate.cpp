/****************************************************************************
**
** Created: Roman Mulyk 18.03.2007
**
****************************************************************************/

#include "udni_opal_query_model_delegate.h"

#include <QPainter>
#include <QPalette>


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UDniOpalQueryModelDelegate::UDniOpalQueryModelDelegate(QObject *parent)
    : QSqlRelationalDelegate(parent)
{
	durationColumn_1 = 3;
	durationColumn_2 = 4;
	durationColumn_3 = 5;
	
	//initialize days_in_month array
	days_in_month[0]=31;
	days_in_month[1]=28;
	days_in_month[2]=31;
	days_in_month[3]=30;
	days_in_month[4]=31;
	days_in_month[5]=30;
	days_in_month[6]=31;
	days_in_month[7]=31;
	days_in_month[8]=30;
	days_in_month[9]=31;
	days_in_month[10]=30;
	days_in_month[11]=31;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UDniOpalQueryModelDelegate::paint(QPainter *painter,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
	//QStyleOptionViewItem myOption = option;
    //if ( index.column() == durationColumn_2 ){

    //    myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
    //    drawDisplay(painter, myOption, myOption.rect, text);
    //    drawFocus(painter, myOption, myOption.rect);
	//}
	//else if ( index.column() == durationColumn_1 ){
	//	myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
    //    QItemDelegate::paint(painter, myOption, index);
    //}
	//else{
		//painter->setBackground(QBrush(Qt::green));
	//	option.palette.setColor(9, Qt::green);
        QItemDelegate::paint(painter, option, index);
    //}
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
