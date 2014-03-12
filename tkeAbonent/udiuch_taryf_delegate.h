/****************************************************************************
**
** Created: Roman Mulyk 17.08.2006
**
****************************************************************************/

#ifndef UDIUCH_TARYF_DELEGATE_H
#define UDIUCH_TARYF_DELEGATE_H

#include <QItemDelegate>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QString>
#include <QModelIndex>
#include <QVariant>

//----------interfaces-------------
class UDiuch_taryf_Delegate : public QItemDelegate
{	
   Q_OBJECT
public:
    UDiuch_taryf_Delegate( QObject *parent = 0, unsigned char days_in_month = 31 );
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
		unsigned char days;
};      
//-----------------------------------
#endif
