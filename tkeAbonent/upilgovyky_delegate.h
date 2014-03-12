/****************************************************************************
**
** Created: Roman Mulyk 06.10.2006
**
****************************************************************************/

#ifndef UPILGOVYKY_DELEGATE_H
#define UPILGOVYKY_DELEGATE_H

#include <QSqlRelationalDelegate>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QString>
#include <QModelIndex>
#include <QVariant>

#include "tke_math.h"

//----------interfaces-------------
class UPilgovyky_Delegate : public QSqlRelationalDelegate
{	
   Q_OBJECT
public:
    UPilgovyky_Delegate( QObject *parent = 0 );
        //void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        void setEditorData(QWidget *editor, const QModelIndex &index) const;
        void setModelData(QWidget *editor, QAbstractItemModel *model,  const QModelIndex &index) const;
private slots:
        void commitAndCloseEditor();
private:
		int durationColumn_1;
		int durationColumn_2;
		int durationColumn_3;
		int durationColumn_4;
		int durationColumn_5;
		int durationColumn_6;
};      
//-----------------------------------
#endif
