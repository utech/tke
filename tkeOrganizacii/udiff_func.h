/****************************************************************************
**
** Created: Roman Mulyk 4.03.2006
**
****************************************************************************/

#ifndef UDIFF_FUNC_H
#define UDIFF_FUNC_H

#include <QString>
#include <QSqlQuery>
#include <QVariant>
#include <QSettings>
#include <QtGui>

#include "../tkeAbonent/tke_math.h"

#define KVYT_ARJ_DIR "kvyt_ob"
#define TEMP_SQLITE_DB_NAME "tmpDB.tkeSqLite"

static const short Tcp=-19; 

//----------interfaces-------------
QString TkeProgramVersion();
int TkeDbVersion();
QString PrizvIB(int posadaId);
QString OrganizName();
QString OrganizCity();
QString OrganizEDRPOU();
QString OrganizAdresa();
QString OrganizTel();
QString OrganizRozrahRahunok();
QString OrganizBank();
QString OrganizEMail();
QString DocsPosadaPidpysText(int posadaId);
double  OrganizPdv();
int 	organizID(); //Ідентифікатор організації, який доступний в усій програмі
short	tochnistDiuchogoTaryfu();
QString encrypt(QString str);
QString decrypt(QString str);
QString saveDataBase(bool wait=false);
void castControls(QObject *obj, bool isAct);
//-----------------------------------
#endif
