//------------------------------------------------------------
//
// Declaration of dbUpdaterFunctions
// Created by Roman Mulyk 16.01.2009
//
//------------------------------------------------------------

#ifndef DBUPDATERFUNCTIONS_H
#define DBUPDATERFUNCTIONS_H

#include <QtSQL>
#include <QMessageBox>

#include "../../tkeAbonent/db_own.h"
#include "../../UTkeGlobal.h"

//------------------------------------------------------------
class UDbUpdater : public QObject
{
  public:
	UDbUpdater();
	~UDbUpdater();
	bool checkVersions();
	int updateVersions();
  private:
	int versionId;
	int curVersion;
	bool updateVarnins;
	U_Db *dbConnector;
}; 
//------------------------------------------------------------
#endif
