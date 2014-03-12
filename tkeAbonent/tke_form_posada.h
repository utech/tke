#ifndef POSADA_H
#define POSADA_H
//
#include "ui_tke_form_posada.h"
//
#include <QWidget>
#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QLineEdit>

#include "../tkeOrganizacii/udiff_func.h"

class QSqlTableModel;
class QSqlQueryModel;
class LineEditDelegate;

class PosadaForm : public QWidget, public Ui::posadovciForm
{
Q_OBJECT
public:
	PosadaForm( QWidget * parent = 0, Qt::WFlags f = 0 );
	~PosadaForm(){};
private:
	QTextCodec *codec;
	int organiz_id;
	int posadaCount;
	QSqlTableModel *tableModel;
	
	void update();
private slots:
	void saveRecord();
	void itemIsEdit();
};
//=========================

class LineEditDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    LineEditDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const;
private slots:
	void emitEndEdit(){emit endEdit();};
	
signals:
	void endEdit();
};
#endif




