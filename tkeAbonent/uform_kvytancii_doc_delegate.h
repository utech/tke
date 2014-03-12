/****************************************************************************
**
** Created: Roman Mulyk 20.01.2007
**
****************************************************************************/

#ifndef UKVYT_DELEGATE_H
#define UKVYT_DELEGATE_H

#include <QItemDelegate>
#include <QLineEdit>
#include <QDate>
#include <QString>
#include <QModelIndex>
#include <QVariant>

#include "tke_math.h"
//----------interfaces-------------
class UFormKvytanciiDocDelegate : public QItemDelegate
{	
   Q_OBJECT
public:
		UFormKvytanciiDocDelegate( QObject *parent = 0 );
		void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
private slots:

private:
		int durationColumn_1;
		int durationColumn_2;

};      
//-----------------------------------
#endif
