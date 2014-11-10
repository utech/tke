//--------------------------------------------------------------------------------------------------------
//
// ���� ���������� ����� ����� ���������� ������� ������ �� 
// ���� �������� ������
// ��������� ������������ ������ "�������" 3.08.2007�.
// � ���� ���������������� ���������� �������� QT4.x
//
//--------------------------------------------------------------------------------------------------------
#ifndef UREPLACESTRINGPARSER_H
#define UREPLACESTRINGPARSER_H
// QT ��������� �������
#include <QChar>
#include <QString>
#include <QHash>
#include <QObject>

#include <QDebug>

//����� �������

//�����������

// ���������

// �����
//--------------------------------------------------------------------------------------------------------
class UReplaceStringParser: public QObject
{
	Q_OBJECT
	
	public:
		
		UReplaceStringParser(QObject *parent = 0);
		~UReplaceStringParser();
		
		void setTagsStringsHash(QHash<QString, QString> hash); 	//���� ������ ����� ������ � ������ QHash<QString, QString>
																//����� ������� �� ����� ����, ����� - ������ ��� ����� ����
		void setBeginTagChar(QChar ch);	//���� �������� �������, ���� ��������������� ���
		void setEndTagChar(QChar ch);	//���� �������� �������, ���� ����������� ���
		void setInputString(QString str);	//���������� ������ ������ � ������ ��� �����
		void setDelWrongTag(bool b);	//��������, ��� ����� �� ��, �� �������� � ������� ������ ���, ���� ������� � ������� ���� ��� �����
										//���� true - �� �������� (delWrongTag), ���� false - ��������.
		
		QString curOutputString();	//������� ������� ������
		QString curInputString();	//������� ������ ������
		bool delWrongTagValue();	//�������� ����� delWrongTag
		
		bool convertString();	// ������� ������������ ������ ������ � ������ ����, ���������� ���������� � ������� ���������, �� ���������
								// ��������� setBeginTagChar(QChar ch) � setEndTagChar(QChar ch), � ������������� � QHash<QString, QString>, ��� ��������
								// �������� setTagsStringsHash(QHash<QString, QString> hash), �� ������, �� ����������� � ������������ ���� �� ����� �������.
								// ������� ������ ����� ��������� �������� curOutputString().
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
