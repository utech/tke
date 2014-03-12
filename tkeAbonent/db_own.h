/****************************************************************************
**
** Created: Roman Mulyk 29.07.2005
**
****************************************************************************/

#ifndef DB_OWN_H
#define DB_OWN_H

#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlError>
#include <QTextCodec>
#include <QSettings>
#include <QByteArray>

#include "../tkeOrganizacii/udiff_func.h"
//----------interfaces-------------
class U_Db
{	
    public:
       QString db;
       QString user;
       QString password;
       QString host;
	   QTextCodec *codec;
    public:  
	  U_Db();
      U_Db(QString dbName, QString dbUser, QString dbPassword); 
      bool conection_to_db(bool exclusive=false, bool quiet=false);
};      
//-----------------------------------
#endif
