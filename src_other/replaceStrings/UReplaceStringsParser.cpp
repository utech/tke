//--------------------------------------------------------------------------------------------------------
//
// Файл визначення класу заміни визначених напепед стрічок на 
// певні необхідні стрічки
// Створений програмістами Компанії "Утех" 3.08.2007р.
// У класі використовуються компоненти бібліотеки QT4.x
//
//--------------------------------------------------------------------------------------------------------
#include "UReplaceStringsParser.h"
//--------------------------------------------------------------------------------------------------------
UReplaceStringParser::UReplaceStringParser(QObject *parent)
	:QObject(parent) 
{
	beginTagChar = '[';
	endTagChar = ']';
}		
//--------------------------------------------------------------------------------------------------------
void UReplaceStringParser::setTagsStringsHash(QHash<QString, QString> hash)
{
	tagsHash = hash;
}
//--------------------------------------------------------------------------------------------------------
void UReplaceStringParser::setBeginTagChar(QChar ch)
{
	beginTagChar = ch;
}
//--------------------------------------------------------------------------------------------------------
void UReplaceStringParser::setEndTagChar(QChar ch)
{
	endTagChar = ch;
}
//--------------------------------------------------------------------------------------------------------
void UReplaceStringParser::setInputString(QString str)
{
	UReplaceStringParser::inputString = str;
}
//--------------------------------------------------------------------------------------------------------
void UReplaceStringParser::setDelWrongTag(bool b)
{
	delWrongTag = b;
}
//--------------------------------------------------------------------------------------------------------
QString UReplaceStringParser::curOutputString()
{
	return outString;
}
//--------------------------------------------------------------------------------------------------------
QString UReplaceStringParser::curInputString()
{
	return inputString;
}
//--------------------------------------------------------------------------------------------------------
bool UReplaceStringParser::convertString()
{
	wrongTag = false;
	int iter_main=0;
	bool tag=false;
	outString.clear();
	while (iter_main<inputString.size()){
		QChar ch=inputString.at(iter_main);
		if ( ch == beginTagChar){
			if (tag){
				outString.append(beginTagChar);
				outString.append(tagString);
			}
			tagString.clear();
			tag = true;
		}
		else if ( ch == endTagChar){
			tag = false;
			simplifiedTagString = tagString.simplified();
			if (tagsHash.contains(simplifiedTagString)){
				outString.append(tagsHash.value(simplifiedTagString));
			}
			else{
				wrongTag = true;
				if (!delWrongTag){
					outString.append(beginTagChar);
					outString.append(tagString);
					outString.append(endTagChar);
				}
			}
		}
		else if (tag){
			tagString.append(ch);
		}
		else{
			outString.append(ch);
		}
		iter_main++;
	}
	if (wrongTag){
		//qDebug() << "wrongTag - true" << endl;
		return false;
	}
	else{
		//qDebug() << "wrongTag - false" << endl;
		return true;
	}
}
//--------------------------------------------------------------------------------------------------------
UReplaceStringParser::~UReplaceStringParser()
{
	
}
//--------------------------------------------------------------------------------------------------------
