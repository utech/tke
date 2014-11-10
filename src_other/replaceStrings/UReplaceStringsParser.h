//--------------------------------------------------------------------------------------------------------
//
// Файл декларації класу заміни визначених напепед стрічок на 
// певні необхідні стрічки
// Створений програмістами Компанії "АбонПро" 3.08.2007р.
// У класі використовуються компоненти бібліотеки QT4.x
//
//--------------------------------------------------------------------------------------------------------
#ifndef UREPLACESTRINGPARSER_H
#define UREPLACESTRINGPARSER_H
// QT стандартні інклуди
#include <QChar>
#include <QString>
#include <QHash>
#include <QObject>

#include <QDebug>

//власні інклуди

//Перерахунки

// Структури

// Класи
//--------------------------------------------------------------------------------------------------------
class UReplaceStringParser: public QObject
{
	Q_OBJECT
	
	public:
		
		UReplaceStringParser(QObject *parent = 0);
		~UReplaceStringParser();
		
		void setTagsStringsHash(QHash<QString, QString> hash); 	//Задає взаємно замінні стрічки в форматі QHash<QString, QString>
																//перша колонка дає назву тега, друга - стрічку для заміни тега
		void setBeginTagChar(QChar ch);	//Задає значення символа, яким відкриватиметься тег
		void setEndTagChar(QChar ch);	//Задає значення символа, яким закривається тег
		void setInputString(QString str);	//Встановлює вхідну стрічку з тегами для заміни
		void setDelWrongTag(bool b);	//Значення, яке вказує на те, чи подавати у вихідну стрічку тег, який відсутній у переліку тегів для заміни
										//Якщо true - не подавати (delWrongTag), якщо false - подавати.
		
		QString curOutputString();	//Поточна вихідна стрічка
		QString curInputString();	//Поточна вхідна стрічка
		bool delWrongTagValue();	//Значення змінної delWrongTag
		
		bool convertString();	// Функція перетворення вхідної стрічки з заміною тегів, позначених початковим і кінцевим символами, які задаються
								// функціями setBeginTagChar(QChar ch) і setEndTagChar(QChar ch), і перерахованих у QHash<QString, QString>, яка задається
								// функцією setTagsStringsHash(QHash<QString, QString> hash), на стрічки, які знаходяться у попередньому хеші на другій позиції.
								// Вихідну стрічку можна прочитати функцією curOutputString().
	signals:

	private slots:
		
		
	private:
		QChar beginTagChar, endTagChar;
		QString inputString, outString, tagString, simplifiedTagString;
		QHash<QString, QString> tagsHash;
		bool delWrongTag;
		bool wrongTag;
};
//--------------------------------------------------------------------------------------------------------

#endif
