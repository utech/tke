/****************************************************************************
**
** Created: Roman Mulyk 17.08.2006
**
****************************************************************************/

#ifndef UOPERACII_DELEGATE_H
#define UOPERACII_DELEGATE_H

#include <QSqlRelationalDelegate>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QString>
#include <QModelIndex>
#include <QVariant>

//----------interfaces-------------
class UOperacii_Delegate : public QItemDelegate
{	
   Q_OBJECT
public:
    UOperacii_Delegate( QObject *parent = 0 );
//        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        void setEditorData(QWidget *editor, const QModelIndex &index) const;
        //void setModelData(QWidget *editor, QAbstractItemModel *model,  const QModelIndex &index) const;
private slots:
        void commitAndCloseEditor();
private:
		//int durationColumn_1;
		//int durationColumn_2;
};      
//-----------------------------------
#endif
