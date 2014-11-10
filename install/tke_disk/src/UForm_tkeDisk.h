//--------------------utech--------------------utech--------------------utech--------------------
//
// Файл декларації класу головного вікна програми автозавантаження
//
// Створений програмістами Компанії "АбонПро" 9.04.2008р.
// У класі використовуються компоненти бібліотеки QT4.x
//
//--------------------utech--------------------utech--------------------utech--------------------

#ifndef TKE_DISK_FORM_H
#define TKE_DISK_FORM_H

#include <QWidget>
#include <QProcess>
#include <QSettings>
#include <QTimer>

#include "ui_UForm_tkeDisk.h"

//--------------------utech--------------------utech--------------------utech--------------------
class UForm_tkeDisk : public QWidget
{
        Q_OBJECT

    public:
        UForm_tkeDisk(QWidget *parent = 0);

		~UForm_tkeDisk();
		//friend class tke_MainWindow;
    public slots:
		void toolButton_instDemo_clicked();
		void toolButton_instFull_clicked();
		void showInstallPage();
    private:
		Ui::UForm_tkeDisk ui;
		
		QString verString;
		bool demo;
		QTimer *timer;
};
//--------------------utech--------------------utech--------------------utech--------------------
#endif
