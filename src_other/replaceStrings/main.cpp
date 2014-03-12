#include "UReplaceStringsParser.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>

int main()
{
	qDebug() << "Programm works" << endl;
	QFile indata("input.html");
	if (indata.open(QFile::ReadOnly)) {
		QHash<QString, QString> strHash;
		strHash.insert( "Prizv_I_B", "PIB" );
		strHash.insert( "pidpys", "pidpys" );
		
		QTextStream in(&indata);
		QString inString = in.readAll();
		UReplaceStringParser strParser;
		strParser.setInputString(inString);
		strParser.setTagsStringsHash(strHash);
		if (strParser.convertString())
			qDebug() << "Done Ok." << endl;
		else
			qDebug() << "Done whith warning." << endl;
		QFile outdata("output.html");
		if (outdata.open(QFile::WriteOnly)) {
			QTextStream out(&outdata);
			out	<< strParser.curOutputString();
			outdata.close();
		}
	}
	return 0;
}