//------------------------------------------------------------
//
// Implamantation of tke_Dialog_narah class.
// Created by Roman Mulyk 12.09.2006
//
//------------------------------------------------------------

#include "tke_dialog_change_password.h"

//------------------------------------------------------------
tke_Dialog_change_password::tke_Dialog_change_password(QWidget *parent, QString title, int user_id)
        : QDialog(parent)
{
    ui.setupUi(this);
	
	codec = QTextCodec::codecForName("CP1251");
	
	ui.lineEdit_new_password_2->parentWidget()->setWindowTitle(ui.lineEdit_new_password_2->parentWidget()->windowTitle() + title);
	ui.lineEdit_new_password_2->parentWidget()->setWindowFlags ( ui.lineEdit_new_password_2->parentWidget()->windowFlags() | Qt::WindowStaysOnTopHint );
	
	usId = user_id;
		//Connectors
	connect(ui.okButton_login, SIGNAL(clicked()), this, SLOT(okButton_login_clicked()));
}
//------------------------------------------------------------
bool tke_Dialog_change_password::new_password_is_valid()
{
	if (ui.lineEdit_new_password_1->text() == ui.lineEdit_new_password_2->text())
		return true;
	else
		return false;
}
//------------------------------------------------------------
QString tke_Dialog_change_password::newPassword()
{
	return ui.lineEdit_new_password_2->text();
}
//---------------------slots---------------------------------
void tke_Dialog_change_password::okButton_login_clicked()
{
	if ( new_password_is_valid() ){
		query.exec("UPDATE users SET password='"+ui.lineEdit_new_password_2->text()+"' WHERE id="+QVariant(usId).toString());
		accept();
	}
	else{
		QMessageBox::critical(
                this,
                codec->toUnicode("!!! Невідповідне підтвердження пароля !!!"),
                codec->toUnicode("Помилка підтвердження пароля.\nСпробуйте ще раз."),
                QMessageBox::Ok,
                QMessageBox::NoButton,
				QMessageBox::NoButton);
		ui.lineEdit_new_password_1->clear();
		ui.lineEdit_new_password_2->clear();
	}
}
//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
tke_Dialog_change_password::~tke_Dialog_change_password()
{

}
//------------------------------------------------------------
