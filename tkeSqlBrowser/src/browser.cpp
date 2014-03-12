#include "browser.h"
#include <QSqlDatabase>
#include <QStringList>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QTextEdit>
#include <QTextBlock>
#include <QSqlError>
#include <QStatusBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QTextCodec>
#include <QMimeData>
#include <QClipboard>
#include <QFile>
#include <QDebug>
#include <USqlAccessible>
//
Browser::Browser( QWidget * parent, Qt::WFlags f, QSqlDatabase db) 
	: QMainWindow(parent, f)
{	
	dataBase = db;
	
	codec = QTextCodec::codecForName("CP1251");
	
    MainWindow::setupUi(this);
    redagWidget = new QWidget();
    Form::setupUi(redagWidget);
        
    initTemplateModel();
    
    highlighter = new Highlighter(SqlQueryTextEdit, dataBase.tables());
    highlighter1 = new Highlighter(textEdit, dataBase.tables());
    
	dataInTablesModel = new QSqlQueryModel(this);
	
	initTreeWidget();
	
	groupBox_shablony->hide();
	
	tableView->addAction(actionCopy);
	tableView->addAction(actionSelectAll);
	tableView->setContextMenuPolicy(Qt::ActionsContextMenu);
	
	connect(templatePushButton,SIGNAL(clicked()),this,SLOT(operateTamplateFrame()));
	connect(NewPushButton,SIGNAL(clicked()),this,SLOT(newSqlTemplate()));
	connect(acceptPushButton,SIGNAL(clicked()),this,SLOT(acceptTemplate()));
	connect(editPushButton,SIGNAL(clicked()),this,SLOT(editTemplate()));
	connect(removePushButton,SIGNAL(clicked()),this,SLOT(removeTemplate()));
	connect(treeWidget,SIGNAL(itemPressed(QTreeWidgetItem*,int)),this,SLOT(setDataInView(QTreeWidgetItem*,int)));	
	connect(CancelPushButton,SIGNAL(clicked()),redagWidget,SLOT(hide()));
	connect(runPushButton,SIGNAL(clicked()),this,SLOT(setData()));
	connect(clearPushButton,SIGNAL(clicked()),textEdit,SLOT(clear()));
	connect(insertPushButton,SIGNAL(clicked()),this,SLOT(insertTemplate()));
	connect(SqlQueryTextEdit,SIGNAL(textChanged()),this,SLOT(setPbtnActive()));
	connect(OpusTextEdit,SIGNAL(textChanged()),this,SLOT(setPbtnActive()));
	
	connect(actionCopy, SIGNAL(activated()), this, SLOT(copyToClipBoard()));
	connect(actionSelectAll, SIGNAL(activated()), tableView, SLOT(selectAll()));
}

void Browser::setDataInView(QTreeWidgetItem* item, int column)
{
	dataInTablesModel->setQuery(QString("SELECT * FROM %1;").arg(item->text(column)), dataBase);
    tableView->setModel(dataInTablesModel); 
}

void Browser::initTemplateModel()
{
	edit = false;
	templateModel = new QSqlQueryModel(this);
	templateModel->setHeaderData(1,Qt::Horizontal,codec->toUnicode("SQL �����"));
	templateModel->setHeaderData(2,Qt::Horizontal,codec->toUnicode("���� SQL ������"));
	refresh();
}

void Browser::setData()   
{
	QTextCodec* codec = QTextCodec::codecForName("cp1251");
	QString sqlText;
	sqlText.append(textEdit->toPlainText());
	if(!isRoot)
	{
		if((sqlText.contains("insert",Qt::CaseInsensitive))||(sqlText.contains("delete",Qt::CaseInsensitive))||(sqlText.contains("update",Qt::CaseInsensitive)))
		{
			QMessageBox::warning(this, codec->toUnicode("����������"),
                   codec->toUnicode("�� �� ������ ���������� �������� ���� .\n"
                      "UPDATE, DELETE ��� INSERT !!! "),QMessageBox::Ok);
             return;
		}
    }
	QStringList qList = sqlText.split(';',QString::SkipEmptyParts);
	foreach(QString str, qList)
		dataInTablesModel->setQuery(str,dataBase);
	tableView->setModel(dataInTablesModel);
	tableView->resizeRowsToContents();
	Ui_MainWindow::statusBar->showMessage(dataInTablesModel->lastError().text());
}

void Browser::initTreeWidget()
{
	QStringList nameTables;
    nameTables = dataBase.tables();// ����� nameTables ������ ��
	QStringList::const_iterator it;                    // ������� ���� ����� db
	
	QTreeWidgetItem *topLevelItem[nameTables.count()];// ����� ����� ��������� ���� ������ QTreeWidgetItem ��������� ���� � ������� �������
	int i = 0;
	for(it = nameTables.constBegin(); it != nameTables.constEnd(); ++it)// ����� ���� �������� treeWidget ������� ������� �� ������� ���� �����
	{
		topLevelItem[i] = new QTreeWidgetItem(treeWidget,QStringList(QString("%1").arg(*it)));// ���������� ����� ��������� topLevelItem ������ ������� � ������� �������
		dataInTablesModel->setQuery(QString("SELECT * FROM %1;").arg(*it),dataBase);// ���������� ������ ������ � ���� ������� ������� ������ ��� ���� ��� ��� topLevelItem ����� ������ ����� ���� ������� ������� 
		int i1 = 0;
	    int col = dataInTablesModel->columnCount();
	    QTreeWidgetItem *lowLevelItem[col];// ��������� ����� ��������� ���� topLevelItem
	    while(i1<col)// ����� ���� ������� ���� topLevelItem � �������� �� ������� � ������ ���� ������� �������
	    {
			QString name_s = dataInTablesModel->record(0).field(i1).name();// ��������� ����� ������� �������
			QString type_s = dataInTablesModel->record(0).field(i1).value().typeName();// ��������� ��� ������� �������
			lowLevelItem[i1] = new QTreeWidgetItem(topLevelItem[i],QStringList(QString("%1 : type %2").arg(name_s).arg(type_s)));
	    	i1++;
		}
		treeWidget->insertTopLevelItem(i,topLevelItem[i]);   
		i++;
		dataInTablesModel->clear();// ���� ��� �������� ������ �������
	}
}

void Browser::newSqlTemplate()
{
	SqlQueryTextEdit->setPlainText(textEdit->toPlainText());
	redagWidget->show();
	acceptPushButton->setEnabled(false);
}

void Browser::acceptTemplate()
{
	
	if(!edit)
		templateModel->setQuery(QString("INSERT INTO table_template (id, SQLQuery, Opus) VALUES('%1','%2','%3');").arg(nextLichylnyk("table_template")).arg(SqlQueryTextEdit->toPlainText()).arg(OpusTextEdit->toPlainText()),dataBase);
	else
	{
		QSqlQuery *query = new QSqlQuery(dataBase);
		query->exec("UPDATE table_template SET SQLQuery = '"+SqlQueryTextEdit->toPlainText()+"', \
								Opus = '"+OpusTextEdit->toPlainText()+"' \
								WHERE id = "+ QVariant(template_id).toString());
				   qDebug() << template_id;
		delete query;
	}
	refresh();
}
void Browser::removeTemplate()
{
	templateModel->setQuery(QString("DELETE FROM table_template WHERE id="+
									templateModel->record(SqlTemplateTableView->currentIndex().row()).value("id").toString()));
	refresh();
	
}

void Browser::editTemplate()
{
	template_id = templateModel->record(SqlTemplateTableView->currentIndex().row()).value("id").toInt();
	
	SqlQueryTextEdit->setText(templateModel->record(SqlTemplateTableView->currentIndex().row()).value("SQLQuery").toString());
	OpusTextEdit->setText(templateModel->record(SqlTemplateTableView->currentIndex().row()).value("Opus").toString());
	redagWidget->show();
	edit = true;
}

void Browser::insertTemplate()
{
	textEdit->setText(templateModel->record(SqlTemplateTableView->currentIndex().row()).value("SQLQuery").toString());
}

void Browser::setPbtnActive()
{
	acceptPushButton->setEnabled(true);
}

Browser::~Browser()
{
	delete redagWidget;
}

void Browser::refresh()
{
	templateModel->setQuery("SELECT * FROM table_template;",dataBase);
	SqlTemplateTableView->setModel(templateModel);
	SqlTemplateTableView->setColumnHidden(0, true);
	SqlTemplateTableView->resizeColumnsToContents();
}

void Browser::setRoot(bool r)
{
	isRoot = r;
}

void Browser::operateTamplateFrame()
{
	if (groupBox_shablony->isVisible()){
		templatePushButton->setText(codec->toUnicode("������ ->>"));
		groupBox_shablony->hide();
	}
	else{
		templatePushButton->setText(codec->toUnicode("<<- ������"));
		groupBox_shablony->show();
	}
}

void Browser::copyToClipBoard()
{
	int k, n, x, y, i; 
	QModelIndexList selIndList = tableView->selectionModel()->selectedIndexes();
	if (selIndList.size() == 0)
		return;
	QModelIndex modelIndex = selIndList[0];
	//���������� �������� ����������, �� ����������  k...n, �� �������� x....y
	k = modelIndex.column();
	n = k;
	x = modelIndex.row();
	y = x;
	for (int iter=1; iter<selIndList.size(); iter++){
		modelIndex = selIndList[iter];
		i = modelIndex.column();
		if (i<k)
			k=i;
		else if (i>n)
			n=i;
		i = modelIndex.row();
		if (i<x)
			x=i;
		else if (i>y)
			y=i;	
	}
	const QAbstractItemModel *model = modelIndex.model();
	QString htmlStr="<table border=\"1\" cellspacing=\"0\" cellpadding=\"3\">";
	htmlStr += "<tr>";
	for (int col=k; col<=n; col++){
		htmlStr += "<th>";
		htmlStr += model->headerData( col, Qt::Horizontal, Qt::DisplayRole ).toString();
		htmlStr += "</th>";
	}
	htmlStr += "</tr>";
	for (int row=x; row<=y; row++){
		htmlStr += "<tr>";
		for (int col=k; col<=n; col++){
			htmlStr += "<td>";
			htmlStr += model->data(model->index(row,col)).toString();
			htmlStr += "</td>";
		}
		htmlStr += "</tr>";
	}
	htmlStr += "</table>";
	
	qDebug() <<  "htmlstr - " <<htmlStr;
	QMimeData *mimeData = new QMimeData();
	mimeData->setHtml(htmlStr);
	//mimeData->setData(QLatin1String("text/html"), htmlStr.toUtf8());
	//mimeData->setText(htmlStr);
	QApplication::clipboard()->setMimeData(mimeData);
}
