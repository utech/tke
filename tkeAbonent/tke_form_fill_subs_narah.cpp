//------------------------------------------------------------
//
// Implamantation of tke_form_fill_subs_narah class.
// Created by Roman Mulyk 14.11.2014
//
//------------------------------------------------------------

#include <USqlAccessible>
#include <xbase64.h>

#include "tke_form_fill_subs_narah.h"

//------------------------------------------------------------
UFillDbfStatistics::UFillDbfStatistics()
    : m_iNotOPSucceedCount(0),
      m_iOPSucceedCount(0),
      m_iErrorsCount(0),
      m_iWarningsCount(0)
{
}
//------------------------------------------------------------
UFillDbfStatistics::~UFillDbfStatistics()
{
}
//------------------------------------------------------------
bool UFillDbfStatistics::IsClear()
{
    return !m_iNotOPSucceedCount && !m_iOPSucceedCount
            && !m_iErrorsCount && !m_iWarningsCount;
}
//------------------------------------------------------------
tke_form_fill_subs_narah::tke_form_fill_subs_narah( QWidget * parent)
    : QWidget(parent),
      m_iState(InitialState)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	
    updateState(InitialState);

    connect(ui.pushButton_close,SIGNAL(clicked()),this,SLOT(close()));

    connect(ui.pushButton_find_original_dbf_file,SIGNAL(clicked()),this,SLOT(find_original_dbf_file_clicked()));
    connect(ui.pushButton_find_processed_dbf_file,SIGNAL(clicked()),this,SLOT(find_processed_dbf_file_clicked()));

    connect(ui.pushButton_fill_dbf_file,SIGNAL(clicked()),this,SLOT(fill_dbf_file_clicked()));
    connect(ui.pushButton_review_dbf_file,SIGNAL(clicked()),this,SLOT(review_dbf_file_clicked()));
    connect(ui.pushButton_send_via_email,SIGNAL(clicked()),this,SLOT(send_via_email_clicked()));
}
//------------------------------------------------------------
tke_form_fill_subs_narah::~tke_form_fill_subs_narah()
{
	
}
//------------------------------------------------------------
void tke_form_fill_subs_narah::find_original_dbf_file_clicked()
{
    QString strOrigDbfFileName = QFileDialog::getOpenFileName(this,
                                                          "DBF ���� - ����� ����� �����䳿",
                                                          "",
                                                          "*.dbf (dbf �����)");

    if (strOrigDbfFileName.isEmpty())
        return;

    m_fillStatistics = UFillDbfStatistics();
    populateFillStatistics();

    ui.lineEdit_original_dbf_file_path->setText(strOrigDbfFileName);

    setProcessedDbfFileName(strOrigDbfFileName);

    updateState(GotPathsState);
}
//------------------------------------------------------------
void tke_form_fill_subs_narah::find_processed_dbf_file_clicked()
{
    QString strProcDbfFileName = QFileDialog::getSaveFileName(this,
                                                          "���������� DBF ���� �� ����� ����� �����䳿",
                                                          "",
                                                          "*.dbf (dbf �����)");

    if (strProcDbfFileName.isEmpty())
        return;

    m_fillStatistics = UFillDbfStatistics();
    populateFillStatistics();

    setProcessedDbfFileName(strProcDbfFileName);

    if (ui.lineEdit_original_dbf_file_path->text().isEmpty())
        updateState(InitialState);
    else
        updateState(GotPathsState);
}
//------------------------------------------------------------
void tke_form_fill_subs_narah::setProcessedDbfFileName(const QString& strDbfName)
{
    QFileInfo fileInfo(strDbfName);
    fileInfo.setFile(fileInfo.path(), fileInfo.baseName() + "__������." + fileInfo.completeSuffix());

    if (fileInfo.exists())
    {
        QMessageBox::warning(this,
                             "������������",
                             "���������� dbf ���� ���� ��������� ������ ��������� �����,\n"
                             "���� ���� �������� ���� �������� ����������.\n\n"
                             "����� ����������� �����: " +fileInfo.fileName()+"\n\n"
                             "������: �� ������ ������������� �������� ���� ��� ���� ����������, ��� � ������� ������� ��� ���� ����� �� �������.");
    }

    ui.lineEdit_processed_dbf_file_path->setText(fileInfo.absoluteFilePath());
}
//------------------------------------------------------------
void tke_form_fill_subs_narah::fill_dbf_file_clicked()
{
    QString strOrigFileName = ui.lineEdit_original_dbf_file_path->text();
    QString strProcFileName = ui.lineEdit_processed_dbf_file_path->text();

    m_fillStatistics = UFillDbfStatistics();

    while (QFile::exists(strProcFileName))
    {
        if (!QFile::remove(strProcFileName))
        {
            QMessageBox::StandardButton btnClk = QMessageBox::information(this,
                                                                          "������� ������� �� �����",
                                                                          "����, ������� ��� ������ ����������� ����� �������� ����� ���������.\n"
                                                                          "��������, ����-�����, �� �������� �� �������������� ����\n"
                                                                          +strProcFileName,
                                                                          QMessageBox::Ok,
                                                                          QMessageBox::Cancel);
            if (btnClk == QMessageBox::Cancel)
                return;
        }
    }

    if (!QFile::copy(strOrigFileName, strProcFileName))
    {
        QMessageBox::critical(this, "������� ���������.",
                              "�� ������� ��������� ���� ��� ����������. ��������� �� ��� ��� ��������� � ������ ��������.");
        return;
    }

    fill_narah_subs_request(strProcFileName);
    populateFillStatistics();
    updateState(FilledDbfState);
}
//------------------------------------------------------------
void tke_form_fill_subs_narah::review_dbf_file_clicked()
{
    QUrl url("file:///"+ui.lineEdit_processed_dbf_file_path->text().replace('\\', '/'));

    QDesktopServices::openUrl(url);

    updateState(ReviewedFilledDbfState);
}
//------------------------------------------------------------
void tke_form_fill_subs_narah::send_via_email_clicked()
{
    QMessageBox::information(this, "�� ��������������", "�� ��������������.\n\n���� ����� ���������� �������� ��� ���, ��������� �� ������ ��������.");
}
//------------------------------------------------------------
int uMonthsDiff(QDate& dt1, QDate dt2)
{
    QDate dtLocal1(dt1.year(), dt1.month(), 1);
    QDate dtLocal2(dt2.year(), dt2.month(), 1);

    if (dt1 > dt2)
    {
        dtLocal1 = QDate(dt2.year(), dt2.month(), 1);
        dtLocal2 = QDate(dt1.year(), dt1.month(), 1);
    }

    int iResDiff = 0;

    while (dtLocal1 < dtLocal2)
    {
        dtLocal1 = dtLocal1.addMonths(1);
        ++iResDiff;
    }

    return iResDiff;
}
//------------------------------------------------------------
void tke_form_fill_subs_narah::fill_narah_subs_request(const QString& strDbfFileName)
{
    QSqlQuery query;

    xbXBase x;
    xbDbf dbfFile(&x);

    dbfFile.SetVersion( 3 );
    dbfFile.OpenDatabase(strDbfFileName.toLocal8Bit().data());
    //int rowCount = dbfFile.NoOfRecords();

    int row;
    for (row=0; dbfFile.GetNextRecord() == XB_NO_ERROR; row++) {

        int iRahunok = QString(dbfFile.GetField("OWN_NUM")).toInt();
        QDate dtBegDate = QDate::fromString(dbfFile.GetField("DAT_BEG"), "yyyyMMdd");
        QDate dtEndDate = QDate::fromString(dbfFile.GetField("DAT_END"), "yyyyMMdd");

        QDate dtBegDateOp = QDate::fromString(dbfFile.GetField("DAT_BEG_OP"), "yyyyMMdd");
        QDate dtEndDateOp = QDate::fromString(dbfFile.GetField("DAT_END_OP"), "yyyyMMdd");

        if (dtBegDate.isValid() && dtEndDate.isValid())
        {
            query.exec("SELECT sum(Narah) \
                       FROM narahuvannya \
                       WHERE  Rahunok = "+sqlStr(iRahunok)+" \
                           AND DateSerial(year, month, 1) \
                               BETWEEN DateSerial("+sqlStr(dtBegDate.year())+", "+sqlStr(dtBegDate.month())+", 1) \
                               AND DateSerial("+sqlStr(dtEndDate.year())+", "+sqlStr(dtEndDate.month())+", 1)");

            if (query.next())
            {
                double dNarahSum = query.value(0).toDouble();
                int iMonths = uMonthsDiff(dtBegDate, dtEndDate) + 1;

                double dResNarah = uDToM(dNarahSum/iMonths);
                if (dbfFile.PutDoubleField( "N5", dResNarah) == XB_NO_ERROR)
                    ++m_fillStatistics.m_iNotOPSucceedCount;
                else
                    ++m_fillStatistics.m_iErrorsCount;
            }
            else
            {
                ++m_fillStatistics.m_iErrorsCount;
                qDebug() << "�������: ������� ��������� ���������� ��� ����������� � ���� ����� ��� ��������������� ������";
            }
        }
        else if (dtBegDateOp.isValid() && dtEndDateOp.isValid())
        {
            query.exec("SELECT sum(Narah) \
                       FROM narahuvannya \
                       WHERE  Rahunok = "+sqlStr(iRahunok)+" \
                           AND DateSerial(year, month, 1) \
                               BETWEEN DateSerial("+sqlStr(dtBegDateOp.year())+", "+sqlStr(dtBegDateOp.month())+", 1) \
                               AND DateSerial("+sqlStr(dtEndDateOp.year())+", "+sqlStr(dtEndDateOp.month())+", 1)");

            if (query.next())
            {
                double dNarahSum = query.value(0).toDouble();
                int iMonths = uMonthsDiff(dtBegDateOp, dtEndDateOp) + 1;

                double dResNarah = uDToM(dNarahSum/iMonths);
                if (dbfFile.PutDoubleField( "P5", dResNarah) == XB_NO_ERROR)
                    ++m_fillStatistics.m_iOPSucceedCount;
                else
                    ++m_fillStatistics.m_iErrorsCount;
            }
            else
            {
                ++m_fillStatistics.m_iErrorsCount;
                qDebug() << "�������: ������� ��������� ���������� ��� ����������� � ���� ����� ��� ��������������� ������";
            }
        }
        else
        {
            // ��� ������� �� ������� �������� ���� � ��� �������������, � ��� ��������������� ������
            ++m_fillStatistics.m_iWarningsCount;
        }

       if (dbfFile.PutRecord() != XB_NO_ERROR)
           ++m_fillStatistics.m_iErrorsCount;
    }

    if (!m_fillStatistics.m_iErrorsCount)
    {
        QMessageBox::information(this, "���������� ���������", "���������� ����� ��������� ������");
    }
    else
    {
        QMessageBox::critical(this, "�������", "���������� ����� ��������� � ���������");
    }
}
//------------------------------------------------------------
void tke_form_fill_subs_narah::populateFillStatistics()
{
    QString strResHtml;

    if (!m_fillStatistics.IsClear())
    {
        strResHtml = QString("�-��� ���������� �� �������������� ����� (N5): <b>%1</b>;<br>"
                "�-��� ���������� �� ������������ ����� (P5): <b>%2</b>;<br>"
                "�-��� ����������� (��� ����./������ ������ ��� ������ ����): <b>%3</b>;<br>"
                "�-��� �������: <b>%4</b>.")
                .arg(m_fillStatistics.m_iNotOPSucceedCount)
                .arg(m_fillStatistics.m_iOPSucceedCount)
                .arg(m_fillStatistics.m_iWarningsCount)
                .arg(m_fillStatistics.m_iErrorsCount);
    }

    ui.label_fill_dbf_result->setText(strResHtml);
}
//------------------------------------------------------------
void tke_form_fill_subs_narah::updateState(FillState eState)
{
    ui.pushButton_fill_dbf_file->setEnabled(false);
    ui.pushButton_review_dbf_file->setEnabled(false);
    ui.pushButton_send_via_email->setEnabled(false);

    ui.lineEdit_original_dbf_file_path->setEnabled(true);
    ui.lineEdit_processed_dbf_file_path->setEnabled(true);

    switch (eState)
    {
    case SentViaEmailState:
    case ReviewedFilledDbfState:
    case FilledDbfState:
        ui.pushButton_review_dbf_file->setEnabled(true);
        ui.pushButton_send_via_email->setEnabled(true);

        ui.lineEdit_original_dbf_file_path->setEnabled(false);
        ui.lineEdit_processed_dbf_file_path->setEnabled(false);
    case GotPathsState:
        ui.pushButton_fill_dbf_file->setEnabled(true);
    }
}
//------------------------------------------------------------
