/****************************************************************************
**
** Created: Roman Mulyk 17.08.2006
**
****************************************************************************/

#ifndef USPRAVY_SUBS_DELEGATE_H
#define USPRAVY_SUBS_DELEGATE_H

#include <QSqlRelationalDelegate>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QString>
#include <QModelIndex>
#include <QVariant>

//----------interfaces-------------
class USpravy_subs_Delegate : public QSqlRelationalDelegate
{	
   Q_OBJECT
public:
    USpravy_subs_Delegate( QObject *parent = 0 );
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
