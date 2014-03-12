/****************************************************************************
**

**
****************************************************************************/
#include <QtGui>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSettings>
#include <QSpacerItem>
#include <QSqlQuery>

#include "settings.h"
#include "../../tkeAbonent/usettings.h"
//------------------------------------------------------------
SettingsWizard::SettingsWizard(QWidget *parent)
    : QtWizard(parent)
{
	codec = QTextCodec::codecForName("cp1251");
	setWindowTitle(codec->toUnicode("������������ ��������"));
	setWindowIcon(QIcon(QString::fromUtf8(":/pictures/pictures/cube.png")));
	addPage(new OdbcPage);
    addPage(new RegisterIdPage);
    addPage(new EndPage);
	
	setButtonText(QtWizard::BackButton,codec->toUnicode("�����"));
	setButtonText(QtWizard::NextButton,codec->toUnicode("������"));
	setButtonText(QtWizard::FinishButton,codec->toUnicode("ϳ���������"));
	setButtonText(QtWizard::CancelButton,codec->toUnicode("³������"));
	setButtonText(QtWizard::HelpButton,codec->toUnicode("��������"));
	
    setOption(HaveHelpButton, true);
	setOption(QtWizard::NoBackButtonOnStartPage,true);
	
    setPixmap(QtWizard::LogoPixmap, QPixmap(":/pictures/pictures/utech_120_80.png"));

    connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));
}
//------------------------------------------------------------
void SettingsWizard::accept()
{
	button(QtWizard::FinishButton)->setEnabled(false);
	button(QtWizard::BackButton)->setEnabled(false);
	setButtonText(QtWizard::CancelButton,codec->toUnicode("�����"));
	
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	settings->beginGroup("dataBase");
	settings->setValue("path",field("tke_dbField").toString());
	settings->setValue("user",field("tke_dbUserNameField").toString());
	settings->setValue("password",encrypt(field("tke_dbPasswordField").toString()));
	settings->setValue("Organiz_id",field("labelId").toInt());// ������������ ������������� ���������� � �����
	delete settings;
	
	setTkeDbfSettings();
	
	QMessageBox::information(this, codec->toUnicode("�����!!!"),
				codec->toUnicode("�������� �� ������������ ����� \n"
                   " ���� ������ ��������."),
				QMessageBox::Ok );		
}
//------------------------------------------------------------
void SettingsWizard::showHelp()
{
    QString message;

    switch (currentId()) {
    case 1:
        message = codec->toUnicode("�� ������� ���������� ��� �������� ������ �� ��� �����, \n"
                     "�� ������ ������ �� ������ ��� �������� ������ ����� � ������. ");
        break;
    case 2:
        message = codec->toUnicode("�� ������� ���������� ��� ���������� �������������� ����������,\n"
					"�� ��������� ��� �������� ������ ��������. ������� ���� ���������� �� ������\n"
					"� �������� ������ \"������\".");
        break;
    case 3:
        message = codec->toUnicode("�� ��� ������� ������������� ������� ���� ��������. �� ������ ������ ��, \n"
					"���������� ������ \"�����\", ��� ����������, ���������� ������ \"ϳ���������\".");
        break;
    default:
        message = codec->toUnicode("�� ��������� ��������, ��� ���������� ��� ���������� ��������� ��������.");
    }

    QMessageBox::information(this, codec->toUnicode("��������"), message);
}
//------------------------------------------------------------
OdbcPage::OdbcPage(QWidget *parent)
    : QtWizardPage(parent)
{
    QTextCodec* codec = QTextCodec::codecForName("cp1251");
	setTitle(codec->toUnicode("������������ ���� ����� ��������."));
    setSubTitle(codec->toUnicode("������� ����� �� ��������� ��� �����"
											" ��� ������ �� ������ !!!"));
	
	QVBoxLayout *mainLayout = new QVBoxLayout;
	
    QHBoxLayout *hLay = new QHBoxLayout;
	tke_dbLbl = new QLabel(codec->toUnicode("���� �� ���� ����� \"�������� ��������\""));
	tke_dbLineEdit = new QLineEdit;
	tke_dbTool = new QToolButton;
	tke_dbTool->setText("...");
	hLay->addWidget(tke_dbLineEdit);
	hLay->addWidget(tke_dbTool);
    mainLayout->addWidget(tke_dbLbl);
    mainLayout->addLayout(hLay);

	tke_dbLbl = new QLabel(codec->toUnicode("��'� �����������"));
	tke_dbUserNameLineEdit = new QLineEdit;
    mainLayout->addWidget(tke_dbLbl);
	mainLayout->addWidget(tke_dbUserNameLineEdit);
	
	tke_dbLbl = new QLabel(codec->toUnicode("������"));
	tke_dbPasswordLineEdit = new QLineEdit;
	tke_dbPasswordLineEdit->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    mainLayout->addWidget(tke_dbLbl);
	mainLayout->addWidget(tke_dbPasswordLineEdit);
	
	registerField("tke_dbField",tke_dbLineEdit);
	setField("tke_dbField",tke_dbLineEdit->text());
	registerField("tke_dbUserNameField",tke_dbUserNameLineEdit);
	setField("tke_dbUserNameField",tke_dbUserNameLineEdit->text());
	registerField("tke_dbPasswordField",tke_dbPasswordLineEdit);
	setField("tke_dbPasswordField",tke_dbPasswordLineEdit->text());
	
	setLayout(mainLayout);
		
	connect(tke_dbTool,SIGNAL(clicked()),this,SLOT(showDialog()));
}
//------------------------------------------------------------
void OdbcPage::initializePage()
{
	if (tke_dbLineEdit->text().capacity() == 0){ //�������������� ��������� � ������
		QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
		tke_dbLineEdit->setText(settings->value("dataBase/path").toString());
		tke_dbUserNameLineEdit->setText(settings->value("dataBase/user").toString());
		tke_dbPasswordLineEdit->setText(decrypt(settings->value("dataBase/password").toString()));
		delete settings;
	}
}
//------------------------------------------------------------
void OdbcPage::showDialog()
{
    QString fileName = QFileDialog::getOpenFileName(this,"����� ����� ���� ����� �� \"�������� ��������\"",
                                                tke_dbLineEdit->text(),
                                                "���� ����� Microsoft Access (*.mdb)");
	tke_dbLineEdit->setText(fileName);
}
//------------------------------------------------------------
RegisterIdPage::RegisterIdPage(QWidget *parent)
    : QtWizardPage(parent)
{
    codec = QTextCodec::codecForName("cp1251");
	setTitle(codec->toUnicode("��������� ����������"));
    setSubTitle(codec->toUnicode("����-�����, ������ ���� ���������� �� ���������������� ������."));
		
	pidprListWidget = new QListWidget;
	
	labelId = new QLabel;
	pidprLbl = new QLabel(codec->toUnicode("����� ���������� : �� �������"));	
	
	QVBoxLayout* vBox = new QVBoxLayout;
	vBox->addWidget(pidprListWidget);
	vBox->addWidget(pidprLbl);
	
	registerField("pidprLbl",pidprLbl,"text");
	registerField("labelId",labelId,"text");		
    setLayout(vBox);
}
//------------------------------------------------------------
void RegisterIdPage::initializePage()
{
	disconnect(pidprListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(set(int)));
	
	U_Db *db = new U_Db(field("tke_dbField").toString(), field("tke_dbUserNameField").toString(), field("tke_dbPasswordField").toString());
	intOrgIdList.clear();
	pidprListWidget->clear();
	if (db->conection_to_db()){
		QSqlQuery *query = new QSqlQuery(QSqlDatabase::database());
		query->exec("SELECT * FROM this_organiz ORDER BY this_organiz_name");
		while (query->next()){
			pidprListWidget->addItem(query->value(1).toString());
			intOrgIdList << query->value(0).toInt();
		}
		delete query;
	}
	delete db;
	
	QSettings *setting = new QSettings("./tke.ini", QSettings::IniFormat); //³��������� ��������� ����������
	int curOrg = setting->value("dataBase/Organiz_id").toInt();
	delete setting;
	
	if (curOrg < 1){
		pidprListWidget->setCurrentRow(0);
	}
	else{
		pidprListWidget->setCurrentRow(intOrgIdList.indexOf(curOrg));
	}
	setField("labelId", intOrgIdList.at(pidprListWidget->currentRow()));
	
	connect(pidprListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(set(int)));
}
//------------------------------------------------------------
void RegisterIdPage::set(int row)
{
	setField("labelId", intOrgIdList.at(row));
	pidprLbl->setText(QString(codec->toUnicode("����� ���������� : %1 ")).arg(pidprListWidget->currentItem()->text()));
}
//------------------------------------------------------------
EndPage::EndPage(QWidget *parent)
    : QtWizardPage(parent)
{
    codec = QTextCodec::codecForName("cp1251"); 
	setTitle(codec->toUnicode("ϳ����������� �����������"));
    setSubTitle(codec->toUnicode("����� ����� ����� ������� ������� ����� ODBC/DBF"
					" ��� ���������� ��������� �� �������."));
		
    namePidprLabel = new QLabel;
    tke_dbLabel = new QLabel;
	
	QVBoxLayout* vBox = new QVBoxLayout;
	
	vBox->addWidget(namePidprLabel);
	vBox->addWidget(tke_dbLabel);
		
	setLayout(vBox);
}
//------------------------------------------------------------
void EndPage::initializePage()
{
	namePidprLabel->setText(field("pidprLbl").toString());
	tke_dbLabel->setText(codec->toUnicode("���� ����� �� \"�������� ��������\" - \"")+field("tke_dbField").toString()+'\"');
}
//------------------------------------------------------------
