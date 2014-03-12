//------------------------------------------------------------
//
// Implamantation of tke_Dialog_narah class.
// Created by Roman Mulyk 12.09.2006
//
//------------------------------------------------------------

#include "tke_dialog_login.h"
#include <QSettings>
#include <QDir>

//------------------------------------------------------------
tke_Dialog_login::tke_Dialog_login(QWidget *parent)
        : QDialog(parent)
{
    ui.setupUi(this);
		//init
	codec = QTextCodec::codecForName("CP1251");
	
	setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint );
	
	bool lastLoginFound = false;
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QSettings settings("./tke.ini", QSettings::IniFormat);
	int lastLoginId = settings.value("RegisterDialog/LastUserId").toInt();
	loginQuery.exec("SELECT id, login FROM users ORDER BY login");
	QListWidgetItem *item;
	for (int ind=0; loginQuery.next(); ++ind){
		item = new QListWidgetItem();
		item->setText(loginQuery.value(1).toString());
		item->setData(Qt::UserRole, loginQuery.value(0).toInt());
		ui.listWidget_login->addItem(item);
		if (loginQuery.value(0).toInt() == lastLoginId) {
			ui.listWidget_login->setCurrentRow( ind );
			lastLoginFound = true;
		}
	}
	if (!lastLoginFound)
		ui.listWidget_login->setCurrentRow( 0 );
	
	//connectors
	connect(ui.lineEdit_password, SIGNAL(textEdited( const QString &)), this, SLOT(passwordEdited( const QString &))); // passwordEdited
	connect(ui.okButton_login, SIGNAL(clicked()), this, SLOT(okButton_login_clicked())); // try login
	connect(ui.pushButton_change_password, SIGNAL(clicked()), this, SLOT(pushButton_change_password_clicked())); // try change password
}
//------------------------------------------------------------
int tke_Dialog_login::dialogResult()
{
	return dRes;
}
//------------------------------------------------------------
int tke_Dialog_login::userId()
{
	return user_Id;
}
//------------------------------------------------------------
QString tke_Dialog_login::userLogin()
{
	return ui.listWidget_login->currentItem()->text();
}
//----------------slots--------------------------------------
void tke_Dialog_login::passwordEdited( const QString & pswrdString)
{
	if (pswrdString.size() == 0){
		ui.okButton_login->setEnabled(false);
		ui.pushButton_change_password->setEnabled(false);
	}
	else{
		ui.okButton_login->setEnabled(true);
		ui.pushButton_change_password->setEnabled(true);
		ui.okButton_login->setFocusPolicy(Qt::StrongFocus);
	}
}
//------------------------------------------------------------
void tke_Dialog_login::okButton_login_clicked()
{
	if ( validPassword() ){
		dRes = group_sys_id;
		//Збереження поточного користувача, як основного
		QDir::setCurrent(QCoreApplication::applicationDirPath());
		QSettings settings("./tke.ini", QSettings::IniFormat);
		settings.beginGroup("RegisterDialog");
		settings.setValue("LastUserId", user_Id);
		settings.endGroup();
		
		accept();
	}
	else{
		QMessageBox::critical(
                this,
                codec->toUnicode("!!! Невірний пароль !!!"),
                codec->toUnicode("Ви ввели невірний пароль?"),
                QMessageBox::Ok,
                QMessageBox::NoButton,
				QMessageBox::NoButton);
		ui.lineEdit_password->clear();
	}
}
//------------------------------------------------------------
void tke_Dialog_login::pushButton_change_password_clicked()
{
	if ( validPassword() ){
		dRes = group_sys_id+100;
		accept();
	}
	else{
		QMessageBox::critical(
                this,
                codec->toUnicode("!!! Невірний пароль !!!"),
                codec->toUnicode("Ви ввели невірний пароль?"),
                QMessageBox::Ok,
                QMessageBox::NoButton,
				QMessageBox::NoButton);
		ui.lineEdit_password->clear();
	}
}
//------------------------------------------------------------
bool tke_Dialog_login::validPassword() //protected
{
	user_Id = ui.listWidget_login->item(ui.listWidget_login->currentRow())->data(Qt::UserRole).toInt();
	loginQuery.exec("SELECT password, system_id FROM users WHERE id="+QVariant(user_Id).toString());
	if (loginQuery.seek(0)){
		if (ui.lineEdit_password->text() == loginQuery.value(0).toString()){
			group_sys_id = loginQuery.value(1).toInt();
			return true;
		}
		else
			return false;
	}
	else
		return false;
}
//------------------------------------------------------------
tke_Dialog_login::~tke_Dialog_login()
{

}
//------------------------------------------------------------
