//--------------------utech--------------------utech--------------------utech--------------------
//
// Файл виконання класу головного вікна програми автозавантаження
//
// Створений програмістами Компанії "АбонПро" 9.04.2008р.
// У класі використовуються компоненти бібліотеки QT4.x
//
//--------------------utech--------------------utech--------------------utech--------------------

#include "UForm_tkeDisk.h"

//--------------------utech--------------------utech--------------------utech--------------------
UForm_tkeDisk::UForm_tkeDisk(QWidget *parent)
        : QWidget(parent)
{
    ui.setupUi(this);
	
	QSettings *settings = new QSettings("./Autorun.inf", QSettings::IniFormat);
	verString			= settings->value("program/Version").toString();
	demo        = settings->value("program/Demo").toBool();
	delete settings;
	
	setMinimumSize(size());
	setMaximumSize(size());
	
	ui.treeWidget->expandAll();
	
	if (demo){
		ui.toolButton_instFull->setEnabled(false);
		ui.label_full->setEnabled(false);
	}
	
	timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showInstallPage()));
    timer->start(10000); 
	
	connect(ui.toolButton_instDemo, SIGNAL(clicked()), this, SLOT(toolButton_instDemo_clicked()));
	connect(ui.toolButton_instFull, SIGNAL(clicked()), this, SLOT(toolButton_instFull_clicked()));
}
//--------------------utech--------------------utech--------------------utech--------------------
void UForm_tkeDisk::toolButton_instDemo_clicked()
{
	QProcess::startDetached("TKEnergoMenegerDEMO_v"+verString+".exe");
}
//--------------------utech--------------------utech--------------------utech--------------------
void UForm_tkeDisk::toolButton_instFull_clicked()
{
	QProcess::startDetached("TKEnergoMenegerFULL_v"+verString+".exe");
}
//--------------------utech--------------------utech--------------------utech--------------------
void UForm_tkeDisk::showInstallPage()
{
	timer->stop();
	delete timer;
	ui.toolBox->setCurrentIndex(1);
}
//--------------------utech--------------------utech--------------------utech--------------------
UForm_tkeDisk::~UForm_tkeDisk()
{

}
//--------------------utech--------------------utech--------------------utech--------------------
