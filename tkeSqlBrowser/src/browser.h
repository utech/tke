#ifndef BROWSER_H
#define BROWSER_H

#include <QSqlQueryModel>
#include <QString>
#include <QTextCodec>

#include "ui_redag.h"
#include "highlighter.h"
#include "ui_mainwindow.h"
#include "../../tkeOrganizacii/udiff_func.h"
//
class QTreeWidgetItem;


class Browser : public QMainWindow, public Ui::MainWindow, public Ui::Form
{
Q_OBJECT
public:
	Browser( QWidget * parent = 0, Qt::WFlags f = 0, QSqlDatabase db = QSqlDatabase::database());
	~Browser();
	void setRoot(bool);
private:
    void initTemplateModel();
	void initTreeWidget();
	void refresh();
	
	QSqlQueryModel* dataInTablesModel;
	QSqlQueryModel* templateModel;
	QWidget* redagWidget;
	bool edit;
	Highlighter* highlighter;
	Highlighter* highlighter1;
	bool isRoot;
	QTextCodec *codec;
	QSqlDatabase dataBase;
	int template_id;
	
private slots:
	void setDataInView(QTreeWidgetItem* item, int column);
	void setData();
	void newSqlTemplate();
	void acceptTemplate();
	void editTemplate();
	void removeTemplate();
	void insertTemplate();
	void setPbtnActive();
	void operateTamplateFrame();
	void copyToClipBoard();
};
#endif
