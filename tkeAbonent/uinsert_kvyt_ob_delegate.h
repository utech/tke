/****************************************************************************
**
** Created: Roman Mulyk 17.08.2006
**
****************************************************************************/

#ifndef UINSERT_KVYT_OB_DELEGATE_H
#define UINSERT_KVYT_OB_DELEGATE_H

#include <QItemDelegate>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QString>
#include <QModelIndex>
#include <QVariant>
#include <QDateEdit>
#include <QDate>

//----------interfaces-------------
class UInsert_kvyt_ob_Delegate : public QItemDelegate
{	
   Q_OBJECT
public:
		UInsert_kvyt_ob_Delegate( QObject *parent = 0);
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
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
};      
//-----------------------------------
#endif
