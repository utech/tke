/****************************************************************************
**
** Created: Roman Mulyk 27.02.2007
**
****************************************************************************/

#ifndef UPOKAZ_LICH_OPAL_DELEGATE_H
#define UPOKAZ_LICH_OPAL_DELEGATE_H

#include <QItemDelegate>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QString>
#include <QModelIndex>
#include <QVariant>

//----------interfaces-------------
class UPokazLichOpalDelegate : public QItemDelegate
{	
   Q_OBJECT
public:
    UPokazLichOpalDelegate( QObject *parent = 0);
        //void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        void setEditorData(QWidget *editor, const QModelIndex &index) const;
        void setModelData(QWidget *editor, QAbstractItemModel *model,  const QModelIndex &index) const;
private slots:
        void commitAndCloseEditor();
private:
		int durationColumn_2;
		int durationColumn_3;
		int durationColumn_4;
		int durationColumn_5;
		int durationColumn_6;
};      
//-----------------------------------
#endif
