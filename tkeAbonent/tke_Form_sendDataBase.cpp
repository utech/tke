//------------------------------------------------------------
//
// Implamantation of tke_Form_sendDataBase class.
// Created by Roman Mulyk 19.06.2007
//
//------------------------------------------------------------

#include "tke_Form_sendDataBase.h"

#include "../tkeOrganizacii/udiff_func.h"

//------------------------------------------------------------
tke_Form_sendDataBase::tke_Form_sendDataBase( QWidget * parent) 
	: QWidget(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint); 
	
	ui.progressBar->setMinimum(0);
	ui.progressBar->setValue(0);
	
	file = NULL;
	ftp = NULL;
	
	connect(ui.pushButton_sendDatabase, SIGNAL(clicked()), this, SLOT(pushButton_sendDatabase_clicked()));
	connect(ui.pushButton_closeDatabase, SIGNAL(clicked()), this, SLOT(carefullClose()));
	
	show();
	pushButton_sendDatabase_clicked();
}
//------------------------------------------------------------
void tke_Form_sendDataBase::pushButton_sendDatabase_clicked()
{
	int orgId = organizID();
	ui.label_curOperation->setText("������� ���� �����");
	QString fileToSendName = saveDataBase(true);
	ui.pushButton_sendDatabase->setEnabled(false);
	QFileInfo fInfo(fileToSendName);
	
	file = new QFile(fileToSendName, this);
	if (!file->open(QIODevice::ReadOnly)){
		QMessageBox::critical(0, "������� ���������� �����", "�� ���� ������� ���� "+file->fileName());
		ui.pushButton_sendDatabase->setEnabled(true);
		return;
	}
	
	ftp = new QFtp(this);
	ftp->connectToHost("ftp.sambir.org");
	ftp->login("tke_utech_clients@sambir.in.ua","2y37aGhQVymvH8EtClWP");
	
	ftp->cd(QString::number(orgId).rightJustified (2, '0',false));
	ftp->put(file, fInfo.fileName());
	ftp->close();
	
	connect(ftp, SIGNAL(dataTransferProgress(qint64, qint64)), this, SLOT(progressUploading(qint64, qint64)));
	connect(ftp, SIGNAL(commandStarted(int)), this, SLOT(currentOperation(int)));
	connect(ftp, SIGNAL(done(bool)), this, SLOT(uploadingFinished(bool)));
}
//------------------------------------------------------------
void tke_Form_sendDataBase::progressUploading(qint64 done, qint64 total)
{
	ui.progressBar->setMaximum(total);
	ui.progressBar->setValue(done);
	ui.label_uploadInfo->setText("�������� "+QString::number(qRound(done/1000))+" kB �� "+QString::number(qRound(total/1000))+" kB");
}
//------------------------------------------------------------
void tke_Form_sendDataBase::uploadingFinished(bool error)
{
	if (!error)
		QMessageBox::information(0, "���������", "������ ��������� ����������� �����");
	else
		QMessageBox::critical(0, "������� �����������", "ϳ� ��� ����������� ����� ������� �������.\n��������� �������� ���� �� ���.");
	
	file->close();
	//delete file;
	//delete ftp;
	//file = NULL;
	//ftp = NULL;
	
	ui.pushButton_sendDatabase->setEnabled(true);
	if (!error){
		ui.label_curOperation->setText("����������� ������ ��������");
		close();
	}
	else
		ui.label_curOperation->setText("������� �����������");
}
//------------------------------------------------------------
void tke_Form_sendDataBase::currentOperation(int operId)
{
	switch (operId){
		case 1: ui.label_curOperation->setText("ϳ�'������� �� �������"); break;
		case 2: ui.label_curOperation->setText("��������� �� ������"); break;
		case 3: ui.label_curOperation->setText("���� �������� ����������"); break;
		case 4: ui.label_curOperation->setText("����������� ����� ���� �����"); break;
		case 5: ui.label_curOperation->setText("�������� �'�������"); break;
		default: ui.label_curOperation->setText("- - -"); break;
	}
}
//------------------------------------------------------------
void tke_Form_sendDataBase::carefullClose()
{
	if (ftp != NULL)
		ftp->abort();
	close();
}
//------------------------------------------------------------
tke_Form_sendDataBase::~tke_Form_sendDataBase()
{
	//if (file != NULL)
	//	delete file;
	//if (ftp != NULL)
	//	delete ftp;
}
//------------------------------------------------------------
