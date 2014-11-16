//------------------------------------------------------------
//
// Implamantation of tke_form_fill_subs_narah class.
// Created by Roman Mulyk 14.11.2014
//
//------------------------------------------------------------

#ifndef TKE_FORM_FILL_SUBS_NARAH_H
#define TKE_FORM_FILL_SUBS_NARAH_H

#include <QWidget>
#include <QtSql>
#include <QMessageBox>
#include <QtDebug>

#include "../tkeOrganizacii/udiff_func.h"

#include "ui_tke_form_fill_subs_narah.h"

class UFillDbfStatistics
{
public:
    UFillDbfStatistics();
    ~UFillDbfStatistics();

    bool IsClear();

    unsigned int m_iNotOPSucceedCount;
    unsigned int m_iOPSucceedCount;
    unsigned int m_iErrorsCount;
    unsigned int m_iWarningsCount;
};

class tke_form_fill_subs_narah : public QWidget
{
	Q_OBJECT
  public:
	tke_form_fill_subs_narah( QWidget * parent = 0);
	~tke_form_fill_subs_narah();

    typedef enum FillState
    {
        InitialState = 0,
        GotPathsState,
        FilledDbfState,
        ReviewedFilledDbfState,
        SentViaEmailState
    } FillState;

  public slots:
    void find_original_dbf_file_clicked();
    void find_processed_dbf_file_clicked();
    void fill_dbf_file_clicked();
    void review_dbf_file_clicked();
    void send_via_email_clicked();

  private:
    void setProcessedDbfFileName(const QString& strDbfName);
    void fill_narah_subs_request(const QString& strDbfFileName);
    void populateFillStatistics();

    void updateState(FillState eState);

  private:
	Ui::tke_form_fill_subs_narah ui;

    FillState m_iState;
    UFillDbfStatistics m_fillStatistics;
};

#endif
