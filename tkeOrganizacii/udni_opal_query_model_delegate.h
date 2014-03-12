/****************************************************************************
**
** Created: Roman Mulyk 18.03.2007
**
****************************************************************************/

#ifndef UDNI_OPAL_QUERY_MODEL_DELEGATE_H
#define UDNI_OPAL_QUERY_MODEL_DELEGATE_H

#include <QSqlRelationalDelegate>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QString>
#include <QModelIndex>
#include <QVariant>
#include <QDate>


//----------interfaces-------------
class UDniOpalQueryModelDelegate : public QSqlRelationalDelegate
{	
   Q_OBJECT
public:
		UDniOpalQueryModelDelegate( QObject *parent = 0);
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
private slots:
        //void commitAndCloseEditor();
private:
		int durationColumn_1;
		int durationColumn_2;
		int durationColumn_3;
		
		unsigned char days_in_month[12];
};      
//-----------------------------------
#endif
