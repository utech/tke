#include <USqlAccessible>

#include "tke_form_posada.h"
#include <QMessageBox>
#include <QTextCodec>
#include <QtSql>
#include <QVariant>
#include <QSettings>

//
PosadaForm::PosadaForm( QWidget * parent, Qt::WFlags f) 
	: QWidget(parent, f)
{
	LineEditDelegate* delegate = new LineEditDelegate;
	setupUi(this);
	savePbtn->setEnabled(false);
	tableWidget_posadovci->setItemDelegate(delegate);
	tableWidget_posadovci->setColumnHidden(0,true);
	
	codec = QTextCodec::codecForName("cp1251");

	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	organiz_id = settings->value("dataBase/Organiz_id").toInt();
	delete settings;
    
    
	update();	
	connect(savePbtn,SIGNAL(clicked()),this,SLOT(saveRecord()));
	connect(cancelPbtn,SIGNAL(clicked()),this,SLOT(close()));
	connect(delegate,SIGNAL(endEdit()),this,SLOT(itemIsEdit()));
}
//==================
void PosadaForm::itemIsEdit()
{
	savePbtn->setEnabled(true);
}
//===================
void PosadaForm::saveRecord()
{
	tableModel = new QSqlTableModel();
	tableModel->setTable("this_posadovci");
	for (int i=0; i<tableWidget_posadovci->rowCount(); i++){
		tableModel->setFilter("This_organiz_id="+QVariant(organiz_id).toString()+" \
								and Posada_id="+tableWidget_posadovci->item(i,0)->text());
		tableModel->select();
		QSqlRecord record;
		if (tableModel->rowCount() == 0){
			tableModel->insertRows(0, 1);
			record = tableModel->record(0);
			record.setValue("id", nextLichylnyk("this_posadovci"));
			record.setValue("This_organiz_id", organiz_id);
			record.setValue("Posada_id", tableWidget_posadovci->item(i,0)->text());
			record.setValue("Prizv", tableWidget_posadovci->item(i,2)->text());
			record.setValue("Imya", tableWidget_posadovci->item(i,3)->text());
			record.setValue("Batk", tableWidget_posadovci->item(i,4)->text());
			record.setValue("Docs_text", tableWidget_posadovci->item(i,5)->text());
			tableModel->setRecord(0, record);
		}
		else{
			record = tableModel->record(0);
			record.setValue("Prizv", tableWidget_posadovci->item(i,2)->text());
			record.setValue("Imya", tableWidget_posadovci->item(i,3)->text());
			record.setValue("Batk", tableWidget_posadovci->item(i,4)->text());
			record.setValue("Docs_text", tableWidget_posadovci->item(i,5)->text());
			tableModel->setRecord(0, record);
		}
		tableModel->submitAll();
	}
	
	update();
	
	savePbtn->setEnabled(false);
	delete tableModel;
}
//
//==========================
void PosadaForm::update()
{
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT count(*) FROM this_posada");
	query->seek(0);
	posadaCount = query->value(0).toInt();
	tableWidget_posadovci->setRowCount(posadaCount);
	
	query->exec("SELECT this_posada.id, this_posada.Posada_name, \
						this_posadovci.Prizv, this_posadovci.Imya, \
						this_posadovci.Batk, this_posadovci.Docs_text \
				FROM this_posada \
					LEFT JOIN (SELECT * FROM this_posadovci \
								WHERE this_posadovci.This_organiz_id="+QVariant(organiz_id).toString()+") \
								AS this_posadovci \
					ON this_posada.id=this_posadovci.Posada_id \
				ORDER BY this_posada.id");
	int i=0;
	while(query->next()){
		tableWidget_posadovci->setItem(i, 0, new QTableWidgetItem(query->value(0).toString()));

		tableWidget_posadovci->setItem(i, 1, new QTableWidgetItem(query->value(1).toString()));
		tableWidget_posadovci->item(i,1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		tableWidget_posadovci->item(i,1)->setFlags( 0 );
		tableWidget_posadovci->setItem(i, 2, new QTableWidgetItem(query->value(2).toString()));
		tableWidget_posadovci->item(i,2)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		tableWidget_posadovci->setItem(i, 3, new QTableWidgetItem(query->value(3).toString()));
		tableWidget_posadovci->item(i,3)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		tableWidget_posadovci->setItem(i, 4, new QTableWidgetItem(query->value(4).toString()));
		tableWidget_posadovci->item(i,4)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		tableWidget_posadovci->setItem(i, 5, new QTableWidgetItem(query->value(5).toString()));
		tableWidget_posadovci->item(i,5)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		
		i++;
	}
	
	delete query;
}
//=============
LineEditDelegate::LineEditDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

QWidget *LineEditDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex &/* index */) const
{
    QLineEdit *editor = new QLineEdit(parent);
    editor->installEventFilter(const_cast<LineEditDelegate*>(this));
	connect(editor,SIGNAL(editingFinished()),this,SLOT(emitEndEdit()));
    return editor;
}

void LineEditDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::DisplayRole).toString();
    
    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    lineEdit->setText(value);
}

void LineEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
   	QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    QString string = lineEdit->text();
	if ((index.column()!=5) && (string.capacity()!=0)){
		string = string.toLower();
		string.replace(0,1,QString(string.at(0).toUpper()));
	}
    model->setData(index, string);
}

void LineEditDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
