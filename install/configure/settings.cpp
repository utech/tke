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
	setWindowTitle(codec->toUnicode("Налаштування програми"));
	setWindowIcon(QIcon(QString::fromUtf8(":/pictures/pictures/cube.png")));
	addPage(new OdbcPage);
    addPage(new RegisterIdPage);
    addPage(new EndPage);
	
	setButtonText(QtWizard::BackButton,codec->toUnicode("Назад"));
	setButtonText(QtWizard::NextButton,codec->toUnicode("Вперед"));
	setButtonText(QtWizard::FinishButton,codec->toUnicode("Підтвердити"));
	setButtonText(QtWizard::CancelButton,codec->toUnicode("Відмінити"));
	setButtonText(QtWizard::HelpButton,codec->toUnicode("Допомога"));
	
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
	setButtonText(QtWizard::CancelButton,codec->toUnicode("Вийти"));
	
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	settings->beginGroup("dataBase");
	settings->setValue("path",field("tke_dbField").toString());
	settings->setValue("user",field("tke_dbUserNameField").toString());
	settings->setValue("password",encrypt(field("tke_dbPasswordField").toString()));
	settings->setValue("Organiz_id",field("labelId").toInt());// запамятовуємо ідентифікатор підприємства в реєстрі
	delete settings;
	
	setTkeDbfSettings();
	
	QMessageBox::information(this, codec->toUnicode("Увага!!!"),
				codec->toUnicode("Операція по настроюванню даних \n"
                   " була успішно виконана."),
				QMessageBox::Ok );		
}
//------------------------------------------------------------
void SettingsWizard::showHelp()
{
    QString message;

    switch (currentId()) {
    case 1:
        message = codec->toUnicode("Ця сторінка призначена для введення шляхів до баз даних, \n"
                     "Ви можете ввести їх вручну або вибравши потрібні файли з діалогу. ");
        break;
    case 2:
        message = codec->toUnicode("Ця сторінка призначена для збереження ідентифікатора підприємства,\n"
					"що необхідно для коректної роботи програми. Виберіть Ваше підприємство із списку\n"
					"і натисніть кнопку \"Вперед\".");
        break;
    case 3:
        message = codec->toUnicode("На цій сторінці відображаються зроблені Вами настойки. Ви можете змінити їх, \n"
					"натиснувши кнопку \"Назад\", або підтвердити, натиснувши кнопку \"Підтвердити\".");
        break;
    default:
        message = codec->toUnicode("Це невеличка допомога, яка призначена для полегшення настройки програми.");
    }

    QMessageBox::information(this, codec->toUnicode("Допомога"), message);
}
//------------------------------------------------------------
OdbcPage::OdbcPage(QWidget *parent)
    : QtWizardPage(parent)
{
    QTextCodec* codec = QTextCodec::codecForName("cp1251");
	setTitle(codec->toUnicode("Налаштування бази даних програми."));
    setSubTitle(codec->toUnicode("Виберіть шляхи до відповідних баз даних"
											" або введіть їх вручну !!!"));
	
	QVBoxLayout *mainLayout = new QVBoxLayout;
	
    QHBoxLayout *hLay = new QHBoxLayout;
	tke_dbLbl = new QLabel(codec->toUnicode("Шлях до бази даних \"ТКЕнерго менеджер\""));
	tke_dbLineEdit = new QLineEdit;
	tke_dbTool = new QToolButton;
	tke_dbTool->setText("...");
	hLay->addWidget(tke_dbLineEdit);
	hLay->addWidget(tke_dbTool);
    mainLayout->addWidget(tke_dbLbl);
    mainLayout->addLayout(hLay);

	tke_dbLbl = new QLabel(codec->toUnicode("Ім'я користувача"));
	tke_dbUserNameLineEdit = new QLineEdit;
    mainLayout->addWidget(tke_dbLbl);
	mainLayout->addWidget(tke_dbUserNameLineEdit);
	
	tke_dbLbl = new QLabel(codec->toUnicode("Пароль"));
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
	if (tke_dbLineEdit->text().capacity() == 0){ //автозаповнення значенням з реєстру
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
    QString fileName = QFileDialog::getOpenFileName(this,"Пошук файлу бази даних ПК \"ТКЕнерго менеджер\"",
                                                tke_dbLineEdit->text(),
                                                "Бази даних Microsoft Access (*.mdb)");
	tke_dbLineEdit->setText(fileName);
}
//------------------------------------------------------------
RegisterIdPage::RegisterIdPage(QWidget *parent)
    : QtWizardPage(parent)
{
    codec = QTextCodec::codecForName("cp1251");
	setTitle(codec->toUnicode("Реєстрація підприємства"));
    setSubTitle(codec->toUnicode("Будь-ласка, оберіть ваше підприємство із нижчеприведеного списку."));
		
	pidprListWidget = new QListWidget;
	
	labelId = new QLabel;
	pidprLbl = new QLabel(codec->toUnicode("Назва підприємства : не вибрано"));	
	
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
	
	QSettings *setting = new QSettings("./tke.ini", QSettings::IniFormat); //Відзначення поточного підприємства
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
	pidprLbl->setText(QString(codec->toUnicode("Назва підприємства : %1 ")).arg(pidprListWidget->currentItem()->text()));
}
//------------------------------------------------------------
EndPage::EndPage(QWidget *parent)
    : QtWizardPage(parent)
{
    codec = QTextCodec::codecForName("cp1251"); 
	setTitle(codec->toUnicode("Підтвердження настроювань"));
    setSubTitle(codec->toUnicode("Даний візард також налаштує джерела даних ODBC/DBF"
					" для втягування квитанцій та субсидій."));
		
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
	tke_dbLabel->setText(codec->toUnicode("База даних ПК \"ТКЕнерго менеджер\" - \"")+field("tke_dbField").toString()+'\"');
}
//------------------------------------------------------------
