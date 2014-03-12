/****************************************************************************
**
** created by Volodya Paslavskij
**
****************************************************************************/
#ifndef SETTINGS_WIZARD_H
#define SETTINGS_WIZARD_H

#include <qtwizard.h>
#include <QString>

#include "../../tkeAbonent/db_own.h"
#include "../../tkeOrganizacii/udiff_func.h"

//------------------------------------------------------------
class QCheckBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QToolButton;
class QSqlTableModel;
class QListWidget;
class QListWidgetItem;
class QSettings;
//------------------------------------------------------------
class SettingsWizard : public QtWizard
{
    Q_OBJECT

public:
    SettingsWizard(QWidget *parent = 0);
    
private slots:
    void showHelp();
	void accept();
	
private:
	QTextCodec* codec;
};
//------------------------------------------------------------
class OdbcPage : public QtWizardPage
{
    Q_OBJECT

public:
    OdbcPage(QWidget *parent = 0);

	void initializePage();

private slots:
	void showDialog();
	
private:
    QLabel *topLabel;
    QLineEdit* tke_dbLineEdit;
    QLineEdit* tke_dbUserNameLineEdit;
	QLineEdit* tke_dbPasswordLineEdit;
	QToolButton* tke_dbTool;
	QLabel* tke_dbLbl;
};
//------------------------------------------------------------
class RegisterIdPage : public QtWizardPage
{
    Q_OBJECT

public:
    RegisterIdPage(QWidget *parent = 0);

	void initializePage();
	
public slots:
	void set(int row);
	
private:
    QListWidget *pidprListWidget;
    QLabel *pidprLbl;
	QLabel* labelId;
    QString namePidpr;
	QList<int> intOrgIdList;
	
	QTextCodec* codec;
};
//------------------------------------------------------------
class EndPage : public QtWizardPage
{
    Q_OBJECT

public:
    EndPage(QWidget *parent = 0);

protected:
	void initializePage();

private:
    QLabel *namePidprLabel;
    QLabel *tke_dbLabel;
	
	QTextCodec* codec;
};
//------------------------------------------------------------
#endif
