/****************************************************************************
**
** Created: Roman Mulyk 2.01.2007
**
** � ����� ��������� ������� ������������ ������ ����� ��� �������� ��������
****************************************************************************/

#ifndef USETTINGS_H
#define USETTINGS_H

#include <QString>
#include <QVariant>
#include <QSettings>
#include <QTextCodec>

// Srtuctures


//Function declatarion
	bool	setTkeDbfSettings();
	QString	readAbonentODBCPath();
	QString	readOrganizODBCPath();
	bool	setAbonentODBCPath();
	bool	setOrganizODBCPath();

#endif
