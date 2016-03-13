/****************************************************************************
**
** Created: Roman Mulyk 04.01.2007
**
****************************************************************************/

#include "uprint_docs.h"
#include <QMessageBox>
#include <UPopulateTextTableCells>
#include <USqlAccessible>

//------------------------------------------------------------
UPrintDocs::UPrintDocs()
{
	date= QDate::currentDate();
	codec = QTextCodec::codecForName("CP1251");
	
	monthlist 	<< codec->toUnicode("�����")
				<< codec->toUnicode("�����")
				<< codec->toUnicode("��������")
				<< codec->toUnicode("������")
				<< codec->toUnicode("�������")
				<< codec->toUnicode("�������")
				<< codec->toUnicode("������")
				<< codec->toUnicode("�������")
				<< codec->toUnicode("��������")
				<< codec->toUnicode("�������")
				<< codec->toUnicode("��������")
				<< codec->toUnicode("�������");
}
//------------------------------------------------------------
void UPrintDocs::print_vidomist_pilg(int cur_year, int cur_month)
{
		int year = cur_year;
		int month = cur_month;
		printform = new UPrintForm(0, codec->toUnicode("������� ������� ����"));
		QTextBlockFormat blockFormat;
		QTextTableCell cell;
		QTextCharFormat textCharFormat;
		//��������� ��������� ������� ��� ��� ������� �� X �����, Y ��
		printform->document()->clear();
		QTextCursor cursor(printform->document()), cellCursor;
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.setBlockFormat( blockFormat );
		textCharFormat.setFontPointSize( 13 );
		cursor.insertText(codec->toUnicode("������� ²��̲��� ϲ������ʲ�"), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 8 );
		cursor.insertText(codec->toUnicode(" "), textCharFormat);
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 11 );
		cursor.insertText(codec->toUnicode("�� ") + monthlist.at(month-1) +
							codec->toUnicode(" ") + QVariant(year).toString() + codec->toUnicode("�."), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText(codec->toUnicode("�� ��������� ")+OrganizName()+"\n");
		textCharFormat.setFontPointSize( 8 );
		cursor.insertText(codec->toUnicode(" "), textCharFormat);
			//��������� � ������������ �������
		QTextTableFormat tableFormat;
		QVector<QTextLength> constraints;
		constraints << QTextLength(QTextLength::FixedLength, 20);
		constraints << QTextLength(QTextLength::FixedLength, 200);
		constraints << QTextLength(QTextLength::FixedLength, 100);
		constraints << QTextLength(QTextLength::FixedLength, 80);
		constraints << QTextLength(QTextLength::FixedLength, 80);
		tableFormat.setColumnWidthConstraints(constraints);
		tableFormat.setCellSpacing(0);
		tableFormat.setCellPadding(1);
		tableFormat.setBorder(0.5);
		QTextTable *table = cursor.insertTable(2, 5, tableFormat);
			//���������� ����� �������
		cell = table->cellAt(0, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("� �/�"));
	
		cell = table->cellAt(0, 1);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("�������� �����"));
	
		cell = table->cellAt(0, 2);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("³������ �����, %"));
		
		cell = table->cellAt(0, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("ϳ���� �� �����, �.��."));
		
		cell = table->cellAt(0, 4);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("���� �����, ���"));
			//������������ ������� ������
		QSqlQuery query, query2;
		query.exec("SELECT count(*) FROM pilgy");
		query.next();
		table->insertRows( 2, query.value(0).toInt() );	
		//��������� ����� �������
		UPostupForm *postup_form = new UPostupForm(0, query.value(0).toInt() );
		postup_form->show();
			//���������� ������ �����������
		query.exec("SELECT id, Kategoriya, Vidsotok, Ploshsca FROM pilgy ORDER BY Kategoriya");
		int iter=0;
		double zved_pilga=0;
		while (query.next()){
			// � �/�
			cell = table->cellAt(1+iter, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( QVariant(iter+1).toString() );
			// �������� �����
			cell = table->cellAt(1+iter, 1);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( query.value(1).toString() );
			// ³������ �����
			cell = table->cellAt(1+iter, 2);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( query.value(2).toString() );
			// ϳ���� �� �����
			cell = table->cellAt(1+iter, 3);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			if (query.value(3).toDouble() != 0)
			cellCursor.insertText( query.value(3).toString() );
			
			//��������� ������� ���� ����� ��� ����� � ��������
			query2.exec("SELECT sum(Pilga_sum) FROM narahuvannya \
							WHERE month="+QVariant(month).toString()+" \
							and year="+QVariant(year).toString()+" \
							and Pilga_ID="+query.value(0).toString());
			query2.next();
			double pilga = query2.value(0).toDouble();
			zved_pilga += pilga;
			// ���� �����
			cell = table->cellAt(1+iter, 4);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(pilga) );
			iter ++;
			postup_form->inc_pos();
		}
		// ������������ ������� � ����� �������� ������
		table->mergeCells ( 1+iter, 0, 1, 4 );
		//���� ������� ->������� �����
		cell = table->cellAt(1+iter, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( codec->toUnicode("�������� ����, ���.") );
		//���� �������� ������� �����
		cell = table->cellAt(1+iter, 4);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( uMToStr2(zved_pilga) );
		
		cursor.movePosition(QTextCursor::End);
		blockFormat.setAlignment( Qt::AlignLeft );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText("     "+DocsPosadaPidpysText(3)+"					"+PrizvIB(3));

			//ʳ���� ��������� ��������� ��� �����
		postup_form->close();
		printform->printer()->setDocName("³������ ��������� �� ��������� �� "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"�.");
		printform->show();
}
//------------------------------------------------------------
void UPrintDocs::print_vidomist_pilg_kategor(int cur_year, int cur_month)
{
			//����������� ����� �������
		QSqlQuery query, query_2;
		query.exec("SELECT count(*) FROM pilgy");
		query.next();
		UPostupForm *postup_form = new UPostupForm(0, query.value(0).toInt() );
		postup_form->show();
			//��������� �����
		int year = cur_year;
		int month = cur_month;
		printform = new UPrintForm(0, codec->toUnicode("³������ ��������� �� ���������"));
		
		//*****************************************
		//��������� ������� ������� �� ��������� ���� ������� �������� + ���� ����� � �������
		//*****************************************
		
		QTextBlockFormat blockFormat;
		QTextTableCell cell;
		QTextCharFormat textCharFormat;
		//��������� ��������� ������� ��� ��� ������� �� X �����, Y ��
		printform->document()->clear();
		QTextCursor cursor(printform->document()), cellCursor;
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.setBlockFormat( blockFormat );
		textCharFormat.setFontPointSize( 13 );
		cursor.insertText(codec->toUnicode("������� ²��̲��� ϲ������ʲ� �� ������в��"), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 8 );
		cursor.insertText(codec->toUnicode(" "), textCharFormat);
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 11 );
		cursor.insertText(codec->toUnicode("�� ") + monthlist.at(month-1) +
							codec->toUnicode(" ") + QVariant(year).toString() + codec->toUnicode("�."), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText(codec->toUnicode("�� ��������� ")+OrganizName()+"\n");

			//��������� ������� ������� �� ������� ��������� � ����� �������
		QTextTableFormat tableFormat;
		QVector<QTextLength> constraints;
		constraints << QTextLength(QTextLength::FixedLength, 20);
		constraints << QTextLength(QTextLength::FixedLength, 200);
		constraints << QTextLength(QTextLength::FixedLength, 100);
		constraints << QTextLength(QTextLength::FixedLength, 80);
		constraints << QTextLength(QTextLength::FixedLength, 80);
		tableFormat.setColumnWidthConstraints(constraints);
		tableFormat.setCellSpacing(0);
		tableFormat.setCellPadding(1);
		tableFormat.setBorder(0.5);
		QTextTable *table = cursor.insertTable(2, 4, tableFormat);
			//���������� ����� �������
		cell = table->cellAt(0, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("� �/�"));
	
		cell = table->cellAt(0, 1);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("�������� �����"));
	
		cell = table->cellAt(0, 2);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("ʳ������ ��������"));
		
		cell = table->cellAt(0, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("���� �� �������"));

			//������������ ������� ������
		query.exec("SELECT count(*) \
					FROM (SELECT Pilga_ID FROM narahuvannya \
					WHERE (Pilga_ID<>0) and (Pilga_sum<>0)\
					and (Year="+QVariant(year).toString()+") \
					and (Month="+QVariant(month).toString()+") GROUP BY Pilga_ID)");
		query.next();
		table->insertRows( 2, query.value(0).toInt() );

		query.exec("SELECT * FROM (SELECT id, Kategoriya, (SELECT count(*) \
					FROM narahuvannya \
					WHERE (year="+QVariant(year).toString()+") \
					and (month="+QVariant(month).toString()+") \
					and (Pilga_ID=pilgy.id) \
					and (Pilga_sum<>0)) AS Abon_count, \
					(SELECT sum(narahuvannya.Pilga_sum) \
					FROM narahuvannya \
					WHERE (year="+QVariant(year).toString()+") \
					and (month="+QVariant(month).toString()+") \
					and (Pilga_ID=pilgy.id)) AS Pilgy_Sum \
					FROM pilgy) WHERE Abon_count>0 ORDER BY Kategoriya");
		int iter = 0;
		double zved_pilga=0;
		while (query.next()){
			// � �/�
			cell = table->cellAt(1+iter, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( QVariant(iter+1).toString() );
			// �������� �����
			cell = table->cellAt(1+iter, 1);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( query.value(1).toString() );
			// ʳ������ ��������
			cell = table->cellAt(1+iter, 2);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( query.value(2).toString() );
			// ���� �� �������
			cell = table->cellAt(1+iter, 3);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			zved_pilga += query.value(3).toDouble();
			cellCursor.insertText( uMToStr2(query.value(3).toDouble()) );
			
			iter ++;
		}
		//������� ������ ������� (����)
		table->mergeCells ( 1+iter, 0, 1, 3 );
		//���� ������� ->������� �����
		cell = table->cellAt(1+iter, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( codec->toUnicode("�������� ����, ���.") );
		//���� �������� ������� �����
		cell = table->cellAt(1+iter, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( uMToStr2(zved_pilga) );
		
			//��������� ��������� ��� ������
		cursor.insertText(codec->toUnicode(" "), textCharFormat);
		cursor.movePosition(QTextCursor::End);
		blockFormat.setAlignment( Qt::AlignLeft );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText(codec->toUnicode("     ��������					")+PrizvIB(3));
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		
		//*****************************************
		//��������� ��������� � ����������-�����������
		//*****************************************
		
		query.first();
		do{
			//��������� ��������� ���������
			blockFormat.setAlignment( Qt::AlignHCenter );
			cursor.setBlockFormat( blockFormat );
			textCharFormat.setFontPointSize( 13 );
			cursor.insertText(codec->toUnicode("ϳ�������� ������� - ")+query.value(1).toString(), textCharFormat);
			blockFormat.setAlignment( Qt::AlignHCenter );
			cursor.insertBlock(blockFormat);
			textCharFormat.setFontPointSize( 8 );
			cursor.insertText(codec->toUnicode(" "), textCharFormat);
			cursor.insertBlock(blockFormat);
			textCharFormat.setFontPointSize( 11 );
			cursor.insertText(codec->toUnicode("�� ") + monthlist.at(month-1) +
							codec->toUnicode(" ") + QVariant(year).toString() + codec->toUnicode("�."), textCharFormat);
			blockFormat.setAlignment( Qt::AlignHCenter );
			cursor.insertBlock(blockFormat);
			textCharFormat.setFontPointSize( 8 );
		
		
			//��������� ������ �������
			constraints.clear();
			constraints << QTextLength(QTextLength::FixedLength, 20);
			constraints << QTextLength(QTextLength::FixedLength, 80);
			constraints << QTextLength(QTextLength::FixedLength, 250);
			constraints << QTextLength(QTextLength::FixedLength, 80);
			constraints << QTextLength(QTextLength::FixedLength, 80);
			tableFormat.setColumnWidthConstraints(constraints);
			tableFormat.setCellSpacing(0);
			tableFormat.setCellPadding(2);
			QTextTable *table_2 = cursor.insertTable(2, 5, tableFormat);
				//���������� ����� �������
			cell = table_2->cellAt(0, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(codec->toUnicode("� �/�"));
	
			cell = table_2->cellAt(0, 1);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(codec->toUnicode("�������"));
	
			cell = table_2->cellAt(0, 2);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(codec->toUnicode("� � �"));
		
			cell = table_2->cellAt(0, 3);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(codec->toUnicode("������� ������"));
		
			cell = table_2->cellAt(0, 4);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(codec->toUnicode("���� �����"));
			
				//������������ ������� ������
			table_2->insertRows( 2, query.value(2).toInt() );

			query_2.exec("SELECT narahuvannya.Rahunok, abonent.Prizv & ' ' & abonent.Imya & ' ' & abonent.Batk, \
								budynky.Vulycia & ' ' & budynky.Bud_num & ' ��.' & abonent.Kvartyra, narahuvannya.Pilga_sum \
						FROM (narahuvannya \
						LEFT JOIN abonent ON abonent.Rahunok=narahuvannya.Rahunok) \
						LEFT JOIN budynky ON abonent.Budynok_ID=budynky.id \
						WHERE (narahuvannya.Year="+QVariant(year).toString()+") \
							and (narahuvannya.Month="+QVariant(month).toString()+") \
							and  (narahuvannya.Pilga_ID="+query.value(0).toString()+") \
							and (narahuvannya.Pilga_sum<>0) \
						ORDER BY abonent.Prizv & ' ' & abonent.Imya & ' ' & abonent.Batk");
			iter=0;
			double pilgy_kateg_sum=0;
			QSqlQuery pilgovykyQuery;
			while (query_2.next()){
				// � �/�
				cell = table_2->cellAt(1+iter, 0);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignRight );
				cellCursor.setBlockFormat( blockFormat );
				cellCursor.insertText( QVariant(iter+1).toString() );
				// �������
				cell = table_2->cellAt(1+iter, 1);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				cellCursor.insertText( query_2.value(0).toString() );
				// ϲ� ���������, � � ��� ��������� - ��������
				cell = table_2->cellAt(1+iter, 2);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				pilgovykyQuery.exec("SELECT Prizv, Imya, Batk From pilgovyky WHERE Rahunok="+query_2.value(0).toString()+" ORDER BY Prizv, Imya, Batk");
				bool pilgovyky_e=false;
				while (pilgovykyQuery.next()){
					if (pilgovyky_e)
						cellCursor.insertText( "\n" );
					cellCursor.insertText( pilgovykyQuery.value(0).toString()+ " " 
											+pilgovykyQuery.value(1).toString()+ " " 
											+pilgovykyQuery.value(2).toString()  );
					pilgovyky_e=true;
				}
				if ( !pilgovyky_e )
					cellCursor.insertText( query_2.value(1).toString() );
				// ������
				cell = table_2->cellAt(1+iter, 3);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				cellCursor.insertText( query_2.value(2).toString() );
				// ���� �����
				cell = table_2->cellAt(1+iter, 4);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				pilgy_kateg_sum +=query_2.value(3).toDouble();
				cellCursor.insertText( uMToStr2(query_2.value(3).toDouble()) );
				
				iter ++;
			}
			//������� ������ ������� (����)
			table_2->mergeCells ( 1+iter, 0, 1, 4 );
			//���� ������� ->������� �����
			cell = table_2->cellAt(1+iter, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("�������� ����, ���.") );
			//���� �������� ������� �����
			cell = table_2->cellAt(1+iter, 4);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(pilgy_kateg_sum) );
			//��������� ��������� ��� ������
			cursor.insertText(codec->toUnicode(" "), textCharFormat);
			cursor.movePosition(QTextCursor::End);
			blockFormat.setAlignment( Qt::AlignLeft );
			cursor.insertBlock(blockFormat);
			cursor.insertBlock(blockFormat);
			cursor.insertBlock(blockFormat);
			cursor.insertText("     "+DocsPosadaPidpysText(3)+"					"+PrizvIB(3));
			cursor.insertBlock(blockFormat);
			cursor.insertBlock(blockFormat);
			
			postup_form->inc_pos();
		} while (query.next());
		
			//ʳ���� ��������� ��������� ��� �����
		postup_form->close();
		printform->printer()->setDocName("������� ������� ��������� �� ��������� �� "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"�.");
		printform->show();
}
//------------------------------------------------------------
void UPrintDocs::print_zvit_pilg_kategor(int cur_year, int cur_month)
{
			//����������� ����� �������
	QSqlQuery query, query_2, query_vidsotok, query_pilga_for;
	query.exec("SELECT count(*) FROM pilgy");
	query.next();
	UPostupForm *postup_form = new UPostupForm(0, query.value(0).toInt() );
	postup_form->show();
		//��������� �����
	int year = cur_year;
	int month = cur_month;
	printform = new UPrintForm(0, codec->toUnicode("³������ ��������� �� ���������"));
	int iter = 0;
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat;
	//��������� ��������� ������� ��� ��� ������� �� X �����, Y �� ����� �2
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 12 );
	cursor.insertText(codec->toUnicode("����� � 2-�����"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("����������� ������� ̳������ ������"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("�� 21.04.2003 � 100"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("�� ����������� � �������������"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 12 );
	cursor.insertText(OrganizName(), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("��� ������ ")+OrganizEDRPOU(), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 13 );
	cursor.insertText(codec->toUnicode("� � � � � � � � � �"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("������ �� ������������ ������, ���'������ � �������� ����"), textCharFormat);
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("�� ") + monthlist.at(month-1) +
						codec->toUnicode(" ") + QVariant(year).toString() + codec->toUnicode("�."), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
						
	//*****************************************
	//��������� ��������� � ����������-�����������
	//*****************************************
	query.exec("SELECT * FROM (SELECT id, Kategoriya, (SELECT count(*) \
				FROM narahuvannya \
				WHERE (year="+QVariant(year).toString()+") \
				and (month="+QVariant(month).toString()+") \
				and (Pilga_ID=pilgy.id) \
				and (Pilga_sum<>0)) AS Abon_count, \
				(SELECT sum(narahuvannya.Pilga_sum) \
				FROM narahuvannya \
				WHERE (year="+QVariant(year).toString()+") \
				and (month="+QVariant(month).toString()+") \
				and (Pilga_ID=pilgy.id)) AS Pilgy_Sum \
				FROM pilgy) WHERE Abon_count>0 ORDER BY Kategoriya");
	query.first();
	textCharFormat.setFontPointSize( 10 );
	do{
		query_vidsotok.exec("SELECT Vidsotok, Grupy FROM pilgy WHERE id="+query.value(0).toString());
		query_vidsotok.seek(0);
		int curPilgyGrupy = query_vidsotok.value(1).toInt();
		//��������� ��������� ���������
		blockFormat.setAlignment( Qt::AlignLeft );
		cursor.setBlockFormat( blockFormat );
		textCharFormat.setFontPointSize( 11 );
		cursor.insertText(codec->toUnicode("ϳ�������� ������� - ")+query.value(1).toString(), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 8 );
		cursor.insertText(codec->toUnicode(" "), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 8 );
	
	
		//��������� ������ �������
		QTextTableFormat tableFormat;
		QVector<QTextLength> constraints;
		constraints << QTextLength(QTextLength::FixedLength, 20);
		constraints << QTextLength(QTextLength::FixedLength, 250);
		constraints << QTextLength(QTextLength::FixedLength, 200);
		constraints << QTextLength(QTextLength::FixedLength, 80);
		constraints << QTextLength(QTextLength::FixedLength, 30);
		constraints << QTextLength(QTextLength::FixedLength, 30);
		constraints << QTextLength(QTextLength::FixedLength, 40);
		constraints << QTextLength(QTextLength::FixedLength, 60);
		tableFormat.setColumnWidthConstraints(constraints);
		tableFormat.setCellSpacing(0);
		tableFormat.setCellPadding(1);
		tableFormat.setBorder(0.5);
		QTextTable *table_2 = cursor.insertTable(2, 8, tableFormat);
			//���������� ����� �������
		cell = table_2->cellAt(0, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("� �/�"));
		cell = table_2->cellAt(0, 1);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("� � �"));
		
		cell = table_2->cellAt(0, 2);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("������"));
	
		cell = table_2->cellAt(0, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("�������."));
	
		cell = table_2->cellAt(0, 4);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("ʳ���. ���"));
		
		cell = table_2->cellAt(0, 5);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("����� �����"));
	
		cell = table_2->cellAt(0, 6);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("���.���������"));
		
		cell = table_2->cellAt(0, 7);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("���������������"));
			//������������ ������� ������
		table_2->insertRows( 2, query.value(2).toInt() );
		
		query_2.exec("SELECT narahuvannya.Rahunok, t_abonent_inf_2.abon_NPR, t_abonent_inf_2.abon_ADR, narahuvannya.Pilga_sum, t_abonent_inf_2.identyf, narahuvannya.Status \
						FROM t_abonent_inf_2, narahuvannya \
						WHERE (narahuvannya.Rahunok = t_abonent_inf_2.id) \
						and (narahuvannya.Year="+QVariant(year).toString()+") \
						and (narahuvannya.Month="+QVariant(month).toString()+")\
						and (narahuvannya.Pilga_ID="+query.value(0).toString()+") \
						and (narahuvannya.Pilga_sum<>0) ORDER BY t_abonent_inf_2.abon_NPR");
		iter=0;
		int curGrupaPilgovyka = 0;
		double grupPilgaSum[4];
		grupPilgaSum[0] = 0;
		grupPilgaSum[1] = 0;
		grupPilgaSum[2] = 0;
		grupPilgaSum[3] = 0;
		int grupPilgovykyCount[4];
		grupPilgovykyCount[0] = 0;
		grupPilgovykyCount[1] = 0;
		grupPilgovykyCount[2] = 0;
		grupPilgovykyCount[3] = 0;
		double pilgy_kateg_sum=0;
		QSqlQuery pilgovykyQuery;
		while (query_2.next()){
			// � �/�
			cell = table_2->cellAt(1+iter, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( QVariant(iter+1).toString() );
				// ϲ� ���������, � � ��� ��������� - ��������
			pilgovykyQuery.exec("SELECT Prizv, Imya, Batk, Ident, Grupa FROM pilgovyky \
								WHERE Rahunok="+query_2.value(0).toString()+" \
								and Pilga_ID="+query.value(0).toString()+" \
								ORDER BY Prizv, Imya, Batk");
			bool pilgovyky_e=false;
			while (pilgovykyQuery.next()){
				cell = table_2->cellAt(1+iter, 1);
				cellCursor = cell.lastCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				if (pilgovyky_e)
					cellCursor.insertText( "\n" );
				cellCursor.insertText( pilgovykyQuery.value(0).toString()+ " " 
										+pilgovykyQuery.value(1).toString()+ " " 
										+pilgovykyQuery.value(2).toString()  );
				// ���������������� ��� ��� �������� ���������
				cell = table_2->cellAt(1+iter, 3);
				cellCursor = cell.lastCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				if (pilgovyky_e)
					cellCursor.insertText( "\n" );
				cellCursor.insertText( pilgovykyQuery.value(3).toString() );
				
					//�����'���������� ����� ��������� ��� ���������� ������������ ����������� ���������� ��� �����
				curGrupaPilgovyka = pilgovykyQuery.value(4).toInt();
				
				pilgovyky_e=true;
			}
			if ( !pilgovyky_e ){
				cell = table_2->cellAt(1+iter, 1);
				cellCursor = cell.lastCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				cellCursor.insertText( query_2.value(1).toString() );
				// ���������������� ��� ��� ��������� ���������
				cell = table_2->cellAt(1+iter, 3);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				cellCursor.insertText( query_2.value(4).toString() );
			}
			// ������
			cell = table_2->cellAt(1+iter, 2);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( query_2.value(2).toString() );
			// ʳ������ ���, �� ���. �����
			query_pilga_for.exec("SELECT Pilga_for, Pilga_for_2 FROM abonent WHERE Rahunok="+query_2.value(0).toString());
			query_pilga_for.seek(0);
			cell = table_2->cellAt(1+iter, 4);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			if (query_2.value(5).toInt() == 0)
				cellCursor.insertText( query_pilga_for.value(0).toString() );
			else{
				cellCursor.insertText( query_pilga_for.value(1).toString() );
			}
			// ������ �����
			cell = table_2->cellAt(1+iter, 5);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( query_vidsotok.value(0).toString() );
			// �������
			cell = table_2->cellAt(1+iter, 6);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( query_2.value(0).toString() );
			// ���� �����
			cell = table_2->cellAt(1+iter, 7);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			pilgy_kateg_sum +=query_2.value(3).toDouble();
			cellCursor.insertText( uMToStr2(query_2.value(3).toDouble()) );
				//��� ����������� ���������� �� ������
			if (curPilgyGrupy > 0){
				if (curGrupaPilgovyka>3)
					curGrupaPilgovyka = 0;
				grupPilgaSum[curGrupaPilgovyka] += query_2.value(3).toDouble();
				grupPilgovykyCount[curGrupaPilgovyka] ++;
			}
			
			iter ++;
		}
		//������� ������ ������� (����)
		table_2->mergeCells ( 1+iter, 0, 1, 7 );
		//���� ������� ->������� �����
		cell = table_2->cellAt(1+iter, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( codec->toUnicode("�������� ����, ���.") );
		//���� �������� ������� �����
		cell = table_2->cellAt(1+iter, 7);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( uMToStr2(pilgy_kateg_sum) );
		//��� ����������� ���������� �� ������
		if (curPilgyGrupy > 0){
			table_2->insertRows( 2+iter, 4 );
			table_2->mergeCells ( 2+iter, 0, 1, 6 );
			table_2->mergeCells ( 3+iter, 0, 1, 6 );
			table_2->mergeCells ( 4+iter, 0, 1, 6 );
			table_2->mergeCells ( 5+iter, 0, 1, 6 );
			
			//���� ������� ->����� 1
			cell = table_2->cellAt(2+iter, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("����� 1") );
			//���� �������� ������� ��������� � ����
			cell = table_2->cellAt(2+iter, 6);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( QVariant(grupPilgovykyCount[1]).toString() + codec->toUnicode(" ���.") );
			//���� �������� ���� ���� ��� �������
			cell = table_2->cellAt(2+iter, 7);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(grupPilgaSum[1]) + codec->toUnicode(" ���.") );
			
			//���� ������� ->����� 2
			cell = table_2->cellAt(3+iter, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("����� 2") );
			//���� �������� ������� ��������� � ����
			cell = table_2->cellAt(3+iter, 6);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( QVariant(grupPilgovykyCount[2]).toString() + codec->toUnicode(" ���.") );
			//���� �������� ���� ���� ��� �������
			cell = table_2->cellAt(3+iter, 7);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(grupPilgaSum[2]) + codec->toUnicode(" ���.") );
			
			//���� ������� ->����� 3
			cell = table_2->cellAt(4+iter, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("����� 3") );
			//���� �������� ������� ��������� � ����
			cell = table_2->cellAt(4+iter, 6);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( QVariant(grupPilgovykyCount[3]).toString() + codec->toUnicode(" ���.") );
			//���� �������� ���� ���� ��� �������
			cell = table_2->cellAt(4+iter, 7);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(grupPilgaSum[3]) + codec->toUnicode(" ���.") );
			
			//���� ������� ->��� �����
			cell = table_2->cellAt(5+iter, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("��� �����") );
			//���� �������� ������� ��������� � ����
			cell = table_2->cellAt(5+iter, 6);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( QVariant(grupPilgovykyCount[0]).toString() + codec->toUnicode(" ���.") );
			//���� �������� ���� ���� ��� �������
			cell = table_2->cellAt(5+iter, 7);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(grupPilgaSum[0]) + codec->toUnicode(" ���.") );
		}
		
		cursor.movePosition(QTextCursor::End);
		blockFormat.setAlignment( Qt::AlignLeft );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		
		postup_form->inc_pos();
	} while (query.next());
	
	//��������� ��������� ��� ������
	textCharFormat.setFontPointSize( 12 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("	�������� ")+OrganizName()+"					"+PrizvIB(1));
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("    �.�."));
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("	���.���������")+"								 "+PrizvIB(2));
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("�������"));
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("� �������������� �������"));
		//ʳ���� ��������� ��������� ��� �����
	postup_form->close();
	printform->printer()->setOrientation( QPrinter::Landscape );
	printform->printer()->setDocName("����� �2-����� (2003�.) �� "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"�.");
	printform->show();	
}
//------------------------------------------------------------
void UPrintDocs::print_vidomist_narah(int cur_year, int cur_month)
{
		int year = cur_year;
		int month = cur_month;
		printform = new UPrintForm(0, codec->toUnicode("������� ������� ����������� ������ �� ��"));
		QTextBlockFormat blockFormat;
		QTextTableCell cell;
		QTextCharFormat textCharFormat;
		//��������� ��������� ����������� ������ �� �� �� X �����, Y ��
		printform->document()->clear();
		QTextCursor cursor(printform->document()), cellCursor;
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.setBlockFormat( blockFormat );
		textCharFormat.setFontPointSize( 13 );
		cursor.insertText(codec->toUnicode("������� ²��̲���"), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		cursor.insertText(codec->toUnicode("����������� ������ �� ���������� ��������"), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 8 );
		cursor.insertText(codec->toUnicode(" "), textCharFormat);
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 11 );
		cursor.insertText(codec->toUnicode("�� ") + monthlist.at(month-1) +
							codec->toUnicode(" ") + QVariant(year).toString() + codec->toUnicode("�."), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText(codec->toUnicode("�� ��������� ")+OrganizName()+"\n");
		textCharFormat.setFontPointSize( 8 );
		cursor.insertText(codec->toUnicode(" "), textCharFormat);
			//��������� � ������������ �������
		QTextTableFormat tableFormat;
		QVector<QTextLength> constraints;
		constraints << QTextLength(QTextLength::FixedLength, 20);
		constraints << QTextLength(QTextLength::FixedLength, 200);
		constraints << QTextLength(QTextLength::FixedLength, 150);
		tableFormat.setColumnWidthConstraints(constraints);
		tableFormat.setCellSpacing(0);
		tableFormat.setCellPadding(1);
		tableFormat.setBorder(0.5);
		QTextTable *table = cursor.insertTable(2, 3, tableFormat);
			//���������� ����� �������
		cell = table->cellAt(0, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("� �/�"));
	
		cell = table->cellAt(0, 1);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("�������� �����"));
		
		cell = table->cellAt(0, 2);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("�����������"));
			//������������ ������� ������
		QSqlQuery query, query2;
		query.exec("SELECT count(*) FROM (SELECT Pilga_ID FROM narahuvannya \
					WHERE (year="+QVariant(year).toString()+") \
					and (month="+QVariant(month).toString()+") GROUP BY Pilga_ID)");
		query.next();
		table->insertRows( 2, query.value(0).toInt()+1 );	
		//��������� ����� �������
		UPostupForm *postup_form = new UPostupForm(0, query.value(0).toInt() );
		postup_form->show();
		
		query.exec("SELECT Pilga_ID, SUM(Narah) \
					FROM (SELECT narahuvannya.Narah, narahuvannya.Pilga_ID FROM abonent, narahuvannya \
					WHERE (abonent.Rahunok=narahuvannya.Rahunok) \
					and (narahuvannya.year="+QVariant(year).toString()+") \
					and (narahuvannya.month="+QVariant(month).toString()+")) \
					GROUP BY Pilga_ID");
							
		int iter=0;
		double pilgy_kateg_sum=0;
		while (query.next()){
			// � �/�
			cell = table->cellAt(1+iter, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( QVariant(iter+1).toString() );
			// ��������
			query2.exec("SELECT Kategoriya FROM pilgy WHERE id=" + query.value(0).toString());
			query2.next();
			cell = table->cellAt(1+iter, 1);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			if (query.value(0).toInt() !=0 )
				cellCursor.insertText( query2.value(0).toString() );
			else
				cellCursor.insertText( codec->toUnicode("��� �����") );
			// ����������
			cell = table->cellAt(1+iter, 2);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(query.value(1).toDouble()) );
			pilgy_kateg_sum += query.value(1).toDouble();
			
			postup_form->inc_pos();
			iter ++;
		}
		//������� ������ ������� (����)
		table->mergeCells ( 1+iter, 0, 1, 2 );
		//���� ������� ->�������� ����
		cell = table->cellAt(1+iter, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( codec->toUnicode("�������� ����, ���.") );
		//���� �������� ������� ���� ����������� �� �����
		cell = table->cellAt(1+iter, 2);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( uMToStr2(pilgy_kateg_sum) );
		table->mergeCells ( 2+iter, 0, 1, 2 );
		//���� ������� ->�������� �����
		cell = table->cellAt(2+iter, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( codec->toUnicode("�������� ����� ��������, �.��.") );
		//���� �������� ������� ����� ��������
		query2.exec("SELECT SUM(Ploshcha) FROM abonent WHERE opal=true");
		query2.next();
		cell = table->cellAt(2+iter, 2);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( uMToStr2(query2.value(0).toDouble()) );
		//��������� ��������� ��� ������
		cursor.insertText(codec->toUnicode(" "), textCharFormat);
		cursor.movePosition(QTextCursor::End);
		blockFormat.setAlignment( Qt::AlignLeft );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText("     "+DocsPosadaPidpysText(3)+"					"+PrizvIB(3));
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		
		postup_form->close();
		printform->printer()->setDocName("³������ ����������� �� "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"�.");
		printform->show();
}
//------------------------------------------------------------
void UPrintDocs::print_dovidka_oderj_subs(int cur_year, int cur_month)
{
		int year = cur_year;
		int month = cur_month;
		printform = new UPrintForm(0, codec->toUnicode("������ ��� ���� ��������� �������"));
		QTextBlockFormat blockFormat;
		QTextTableCell cell;
		QTextCharFormat textCharFormat;
		//��������� ��������� ����������� ������ �� �� �� X �����, Y ��
		printform->document()->clear();
		QTextCursor cursor(printform->document()), cellCursor;
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.setBlockFormat( blockFormat );
		textCharFormat.setFontPointSize( 14 );
		cursor.insertText(codec->toUnicode("��²���"), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		cursor.insertText(codec->toUnicode("��� ���� ��������� �������"), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 8 );
		cursor.insertText(codec->toUnicode(" "), textCharFormat);
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 11 );
		cursor.insertText(codec->toUnicode("�� ") + monthlist.at(month-1) +
							codec->toUnicode(" ") + QVariant(year).toString() + codec->toUnicode("�."), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText(codec->toUnicode("�� ��������� ")+OrganizName()+"\n");
		textCharFormat.setFontPointSize( 8 );
		cursor.insertText(codec->toUnicode(" "), textCharFormat);
		
		// ��� ������
		QSqlQuery query;
		query.exec("SELECT SUM(Suma), SUM(Suma_d), SUM(Suma_GV), SUM(Suma_d_GV) FROM subs \
					WHERE (year="+QVariant(year).toString()+") \
					and (month="+QVariant(month).toString()+")");
		query.next();
		//���������� �����䳿 ��
		blockFormat.setAlignment( Qt::AlignLeft );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 12 );
		cursor.insertText(codec->toUnicode("    ���������� �����䳿	��		") + uMToStr2(query.value(0).toDouble()) 
												+ codec->toUnicode(" ���"), textCharFormat);
		//������������ �����䳿 ��
		blockFormat.setAlignment( Qt::AlignLeft );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 12 );
		cursor.insertText(codec->toUnicode("    ������������ �����䳿 ��			") + uMToStr2(query.value(1).toDouble()) 
												+ codec->toUnicode(" ���"), textCharFormat);
		//���������� �����䳿 ��
		blockFormat.setAlignment( Qt::AlignLeft );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 12 );
		cursor.insertText(codec->toUnicode("    ���������� �����䳿	��		") + uMToStr2(query.value(2).toDouble()) 
												+ codec->toUnicode(" ���"), textCharFormat);
		//������������ �����䳿 ��
		blockFormat.setAlignment( Qt::AlignLeft );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 12 );
		cursor.insertText(codec->toUnicode("    ������������ �����䳿 ��			") + uMToStr2(query.value(3).toDouble()) 
												+ codec->toUnicode(" ���"), textCharFormat);
		//������ �����䳿
		double suma = query.value(0).toDouble() + query.value(1).toDouble() + query.value(2).toDouble() + query.value(3).toDouble();
		blockFormat.setAlignment( Qt::AlignLeft );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 12 );
		cursor.insertText(codec->toUnicode("    ������ �� ����������			") + uMToStr2(suma) 
												+ codec->toUnicode(" ���"), textCharFormat);
		
		//��������� ��������� ��� ������
		cursor.insertText(codec->toUnicode(" "), textCharFormat);
		cursor.movePosition(QTextCursor::End);
		blockFormat.setAlignment( Qt::AlignLeft );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText("     "+DocsPosadaPidpysText(3)+"					"+PrizvIB(3));
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		
		printform->printer()->setDocName("������ ��� ���� �������� �����䳿 �� "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"�.");
		printform->show();
}
//------------------------------------------------------------
void UPrintDocs::print_forma_akt_vytrat_teploenergii()
{
	printform = new UPrintForm(0, codec->toUnicode("��� ��� ������� ���������㳿"));
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat;
	//��������� ��������� ������� ��� ��� ������� �� X �����, Y ��
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 13 );
	cursor.insertText(codec->toUnicode("� � �"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 8 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("��� ������� ���������㳿 �� ______________ ")+OrganizCity(), textCharFormat);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("					̳���� ") + monthlist.at(date.month()-1)
						+ codec->toUnicode(" ") + QVariant(date.year()).toString() + codec->toUnicode("�.\n"));
	textCharFormat.setFontPointSize( 8 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
		//��������� � ������������ �������
	QTextTableFormat tableFormat;
    QVector<QTextLength> constraints;
	constraints << QTextLength(QTextLength::FixedLength, 20);
    constraints << QTextLength(QTextLength::FixedLength, 160);
    constraints << QTextLength(QTextLength::FixedLength, 100);
    constraints << QTextLength(QTextLength::FixedLength, 100);
    constraints << QTextLength(QTextLength::FixedLength, 80);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 70);
    tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(1);
	tableFormat.setBorder(0.5);
    QTextTable *table = cursor.insertTable(5, 8, tableFormat);
	table->mergeCells ( 0, 2, 1, 3 );
	table->mergeCells ( 0, 0, 2, 1 );
	table->mergeCells ( 0, 1, 2, 1 );
	table->mergeCells ( 0, 5, 2, 1 );
	table->mergeCells ( 0, 6, 2, 1 );
	table->mergeCells ( 0, 7, 2, 1 );
		//���������� ����� �������
	cell = table->cellAt(0, 0);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("� �/�"));
	
	cell = table->cellAt(0, 1);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("����� ��'����"));
	
	cell = table->cellAt(0, 2);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("��������� ���������"));
	
	cell = table->cellAt(0, 5);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("�-��� �.�. � ����."));
	
	cell = table->cellAt(0, 6);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("�����"));
	
	cell = table->cellAt(0, 7);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("����, ���."));
	
	cell = table->cellAt(1, 2);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("��. �����"));
	
	cell = table->cellAt(1, 3);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("���. �����"));
	
	cell = table->cellAt(1, 4);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("������"));
	
	
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("     ³����������� \n     �� ����������"));
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("     �����������\n     ")+OrganizName());
	
	//ʳ���� ��������� ��������� ��� �����
	printform->printer()->setDocName("��� ������ ���������㳿 (�����)");
	printform->show();
}
//------------------------------------------------------------
void UPrintDocs::print_diuch_taryf(int cur_year, int cur_month)
{
	printform = new UPrintForm(0, codec->toUnicode("��� ��� ������� ���������㳿"));
	short diuchTaryfPrecision = tochnistDiuchogoTaryfu();
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat;
	//��������� ��������� ������� ��� ��� ������� �� X �����, Y ��
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 13 );
	cursor.insertText(codec->toUnicode("� � �"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 8 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("��� ������� ���������㳿 �� ______________ ")+OrganizCity(), textCharFormat);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("					̳���� ") + monthlist.at(cur_month-1)
						+ codec->toUnicode(" ") + QVariant(cur_year).toString() + codec->toUnicode("�.\n"));
	textCharFormat.setFontPointSize( 8 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
		//��������� � ������������ �������
	QTextTableFormat tableFormat;
    QVector<QTextLength> constraints;
	constraints << QTextLength(QTextLength::FixedLength, 20);
    constraints << QTextLength(QTextLength::FixedLength, 160);
    constraints << QTextLength(QTextLength::FixedLength, 100);
    constraints << QTextLength(QTextLength::FixedLength, 100);
    constraints << QTextLength(QTextLength::FixedLength, 80);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 70);
    tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(1);
	tableFormat.setBorder(0.5);
    QTextTable *table = cursor.insertTable(3, 8, tableFormat);
	table->mergeCells ( 0, 2, 1, 3 );
	table->mergeCells ( 0, 0, 2, 1 );
	table->mergeCells ( 0, 1, 2, 1 );
	table->mergeCells ( 0, 5, 2, 1 );
	table->mergeCells ( 0, 6, 2, 1 );
	table->mergeCells ( 0, 7, 2, 1 );
		//���������� ����� �������
	cell = table->cellAt(0, 0);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("� �/�"));
	
	cell = table->cellAt(0, 1);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("����� ��'����"));
	
	cell = table->cellAt(0, 2);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("��������� ���������"));
	
	cell = table->cellAt(0, 5);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("�-��� �.�. � ����."));
	
	cell = table->cellAt(0, 6);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("�����"));
	
	cell = table->cellAt(0, 7);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("����, ���."));
	
	cell = table->cellAt(1, 2);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("��. �����"));
	
	cell = table->cellAt(1, 3);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("���. �����"));
	
	cell = table->cellAt(1, 4);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("������"));
	
		//������������ ������� ������
	QSqlQuery query;
	query.exec("SELECT count(*) FROM budynky");
	//query.exec("SELECT count(*) FROM budynky WHERE Lichylnyk <> 0");
	query.next();
	table->insertRows( 3, query.value(0).toInt());
	
		//���������� ������� ������ ��� ������ ��� �������
	query.exec("SELECT budynky.Vulycia + ' ' + budynky.Bud_num AS adr, budynky.Koef_pereved, \
						diuch_taryf.Lich_poch, diuch_taryf.Lich_kin, \
						diuch_taryf.Lich_kin - diuch_taryf.Lich_poch AS rizn, diuch_taryf.Taryf, \
						t_bud.pl * diuch_taryf.Taryf AS Sum \
						FROM ((budynky \
						LEFT JOIN (SELECT * FROM diuch_taryf \
						WHERE (diuch_taryf.Year=" + QVariant(cur_year).toString() + ") \
						and (diuch_taryf.Month=" + QVariant(cur_month).toString() + ")) \
						AS diuch_taryf ON budynky.id = diuch_taryf.Budynok_ID) \
						LEFT JOIN (SELECT Budynok_ID, sum(Ploshcha) AS pl \
						FROM abonent WHERE (opal=true) \
						GROUP BY Budynok_ID) \
						AS t_bud ON budynky.id = t_bud.Budynok_ID) \
						ORDER BY budynky.Vulycia + ' ' + budynky.Bud_num");
						//WHERE (budynky.lichylnyk<>0) ORDER BY budynky.Vulycia + ' ' + budynky.Bud_num");
	int i=0;
	double gKalSum=0, narahSum=0;
	double pokaznLichRizn, koef, dVal;
	while (query.next()){
		// � �/�
		cell = table->cellAt(2+i, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( QVariant(i+1).toString() );
		// ����� ��'����
		cell = table->cellAt(2+i, 1);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( query.value(0).toString() );
		// �������� ��������� -> ����� �����
		cell = table->cellAt(2+i, 2);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( query.value(3).toString() );
		// �������� ��������� -> ������� �����
		cell = table->cellAt(2+i, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( query.value(2).toString() );
		// �������� ��������� -> ������
		cell = table->cellAt(2+i, 4);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		pokaznLichRizn = query.value(4).toDouble();
		cellCursor.insertText( QString::number(pokaznLichRizn) );
		
		// ���������� �����������
		cell = table->cellAt(2+i, 5);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		koef = query.value(1).toDouble();
		cellCursor.insertText( QString::number(koef) );
		
		if (koef > 0)
			gKalSum += pokaznLichRizn/koef;
		
		// �����
		cell = table->cellAt(2+i, 6);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( QString::number(query.value(5).toDouble(),'f', diuchTaryfPrecision).replace('.',',') );
		// ����
		cell = table->cellAt(2+i, 7);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		dVal = query.value(6).toDouble();
		narahSum += dVal;
		cellCursor.insertText( uMToStr2(dVal) );
		
		i++;
	}
		//�������
	
	table->mergeCells ( 2+i, 0, 1, 4 );
	// � �/�
	cell = table->cellAt(2+i, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText( "������" );
	// �������� ��������� -> ������
	cell = table->cellAt(2+i, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText( QString::number(gKalSum)+" ����" );
	// ���������� �����������
	cell = table->cellAt(2+i, 5);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText( "-" );
	// �����
	cell = table->cellAt(2+i, 6);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText( "-" );
	// ����
	cell = table->cellAt(2+i, 7);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText( uMToStr2(narahSum) );

	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("     ³�����������\n     �� ����������"));
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("     �����������\n     ")+OrganizName());
	
	//ʳ���� ��������� ��������� ��� �����
	printform->printer()->setDocName("��� ������ ���������㳿 �� "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"�.");
	printform->show();
}
//------------------------------------------------------------
void UPrintDocs::print_vidomist_borjnykiv(QDate pochDate, QDate kincDate, double minSum, int BudId, unsigned char abonType, bool isPereplata)
{
    pochDate.setYMD(pochDate.year(), pochDate.month(), 1);
    kincDate.setYMD(kincDate.year(), kincDate.month(), kincDate.daysInMonth());
    //��������� ���������
    printform = new UPrintForm(0,codec->toUnicode((isPereplata)?"³������ �� ��������":"³������ ��������"));
    QTextBlockFormat blockFormat;
    QTextTableCell cell;
    QTextCharFormat textCharFormat;
    printform->document()->clear();
    QTextCursor cursor(printform->document()), cellCursor;
    blockFormat.setAlignment( Qt::AlignHCenter );
    cursor.setBlockFormat( blockFormat );
    textCharFormat.setFontPointSize( 13 );
    cursor.insertText(codec->toUnicode("²��̲���"), textCharFormat);
    blockFormat.setAlignment( Qt::AlignHCenter );
    cursor.insertBlock(blockFormat);
    textCharFormat.setFontPointSize( 12 );
    if (abonType == 0)
        cursor.insertText("�Ѳ� ", textCharFormat);
    else if (abonType == 1)
        cursor.insertText("��'������� �� �� ", textCharFormat);
    else if (abonType == 2)
        cursor.insertText("��'������� �� �� ", textCharFormat);
    QString informStr;
    if (isPereplata)
        informStr = "��������, ��������� ���� �������� ";
    else
        informStr = "��������, ���� ���� �������� ";
        cursor.insertText( informStr +
                        uMToStr2(minSum) +
                        codec->toUnicode(" ���."), textCharFormat);
    blockFormat.setAlignment( Qt::AlignRight );
    cursor.insertBlock(blockFormat);
    textCharFormat.setFontPointSize( 9 );
    cursor.insertText(codec->toUnicode("����������� �������� ������: ") +
                        QVariant(pochDate.month()).toString() + "." +
                        QVariant(pochDate.year()).toString() + "." +
                        codec->toUnicode("�. - ") +
                        QVariant(kincDate.month()).toString() + "." +
                        QVariant(kincDate.year()).toString() + "." +
                        codec->toUnicode("�."), textCharFormat);
    cursor.insertBlock(blockFormat);
    blockFormat.setAlignment( Qt::AlignCenter );
    cursor.insertBlock(blockFormat);
    textCharFormat.setFontPointSize( 12 );
    cursor.insertText("������� ������� �� ��������", textCharFormat);
    cursor.insertBlock(blockFormat);
    cursor.insertBlock(blockFormat);
    textCharFormat.setFontPointSize( 9 );

    //������������ �������
    QTextTable *table = NULL;
    QTextTableFormat tableFormat;
    QVector<QTextLength> constraints;

    //������� ������� ����� �� ��������
    constraints.clear();
    constraints << QTextLength(QTextLength::FixedLength, 40);
    constraints << QTextLength(QTextLength::FixedLength, 150);
    constraints << QTextLength(QTextLength::FixedLength, 100);
    constraints << QTextLength(QTextLength::FixedLength, 100);
    tableFormat.setColumnWidthConstraints(constraints);
    tableFormat.setCellSpacing(0);
    tableFormat.setCellPadding(1);
    tableFormat.setBorder(0.5);
    tableFormat.setBorderBrush(QColor(Qt::black));
    QTextTable *sumaryTable = cursor.insertTable(2, 4, tableFormat);
    UPopulateTextTableCells tableCell(sumaryTable);
    tableCell.setAlignment(Qt::AlignCenter);
    tableCell.set(0,0,"� �/�");
    tableCell.set(0,1,"�������");
    tableCell.set(0,2,(isPereplata)?"�-��� ������������":"�-��� ��������");
    tableCell.set(0,3,(isPereplata)?"���� ���������, ���":"���� �����, ���.");

    if (isPereplata)
        minSum = minSum *-1;
    int sumaryTableRow = 1;
    int sumaryTableBorgBudCount = 0, sumaryTableBorgAllCount = 0;
    double sumaryTableBorgBudSum = 0, sumaryTableBorgAllSum = 0;

    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock(blockFormat);
    cursor.insertBlock(blockFormat);
    blockFormat.setAlignment( Qt::AlignCenter );
    cursor.insertBlock(blockFormat);
    textCharFormat.setFontPointSize( 12 );
    cursor.insertText((isPereplata)?"����˲� ������Ҳ� - ����������ʲ�":"����˲� ������Ҳ� - ������ʲ�", textCharFormat);
    blockFormat.setAlignment( Qt::AlignLeft );
    cursor.insertBlock(blockFormat);
    textCharFormat.setFontPointSize( 9 );


    //������� �������� �� ��������
    constraints.clear();
    constraints << QTextLength(QTextLength::FixedLength, 20);
    constraints << QTextLength(QTextLength::FixedLength, 50);
    constraints << QTextLength(QTextLength::FixedLength, 80);
    constraints << QTextLength(QTextLength::FixedLength, 120);
    constraints << QTextLength(QTextLength::FixedLength, 40);
    constraints << QTextLength(QTextLength::FixedLength, 55);
    constraints << QTextLength(QTextLength::FixedLength, 55);
    constraints << QTextLength(QTextLength::FixedLength, 55);
    constraints << QTextLength(QTextLength::FixedLength, 55);
    constraints << QTextLength(QTextLength::FixedLength, 55);
    constraints << QTextLength(QTextLength::FixedLength, 55);
    constraints << QTextLength(QTextLength::FixedLength, 55);
    tableFormat.setColumnWidthConstraints(constraints);
    tableFormat.setCellSpacing(0);
    tableFormat.setCellPadding(1);
    tableFormat.setBorder(0.5);
    tableFormat.setBorderBrush(QColor(Qt::black));

    //����� � �������� �������� �� ��������
    QSqlQuery *abonQuery = new QSqlQuery();
    QSqlQuery *budQuery = new QSqlQuery();
    QSqlQuery *pokaznQuery = new QSqlQuery();
    QString filtrStr;

    bool needWhere = true;
    if (BudId>0){
        filtrStr += (needWhere?" WHERE ":" and ") + QString("Budynok_ID=")+QVariant(BudId).toString();
        needWhere = false;
    }
    if (abonType>0){
        filtrStr += (needWhere?" WHERE ":" and ") + QString("Opal=")+(abonType==1?"true":"false");
        needWhere = false;
    }

    abonQuery->exec("SELECT count(*) FROM abonent" + filtrStr);
    abonQuery->seek(0);
    UPostupForm *postup_form = new UPostupForm(0, abonQuery->value(0).toInt());
    postup_form->show();

    abonQuery->exec("SELECT Rahunok, Budynok_ID FROM abonent INNER JOIN budynky ON budynky.id=abonent.budynok_ID"
                        +filtrStr+" ORDER BY Vulycia, Bud_num, Rahunok");
    int old_bud=0;
    int iterBorj=0;
    while (abonQuery->next()){
        double zagVh_Saldo, zagNarah, zagNarahGV, zagNarahST, zagSubs, zagOplata, zagVyh_Saldo, zagPereved, zagDebitorska;
        double potVh_Saldo, potNarah, potSubs, potOplata, potNarahGV, potNarahST, potPereved, potDebitorska;
        int Rahunok = abonQuery->value(0).toInt();
        //��������� �������� ���������
        pokaznQuery->exec("SELECT Vh_saldo FROM narahuvannya \
                            WHERE Rahunok=" +QVariant(Rahunok).toString()+
                                     " and month=" +QVariant(kincDate.month()).toString()+
                                     " and year=" +QVariant(kincDate.year()).toString()+
                                     " and Status=0");
        pokaznQuery->seek(0);
        potVh_Saldo = pokaznQuery->value(0).toDouble();

        pokaznQuery->exec("SELECT sum(Narah), sum(Narah_GV), sum(Narah_ST) FROM narahuvannya \
                            WHERE Rahunok=" +QVariant(Rahunok).toString()+
                                    " and month=" +QVariant(kincDate.month()).toString()+
                                    " and year=" +QVariant(kincDate.year()).toString());
        pokaznQuery->seek(0);
        potNarah = pokaznQuery->value(0).toDouble();
        potNarahGV = pokaznQuery->value(1).toDouble();
        potNarahST = pokaznQuery->value(2).toDouble();

        pokaznQuery->exec("SELECT sum(Suma), sum(Suma_d), sum(Suma_GV), sum(Suma_d_GV) FROM subs \
                            WHERE Rahunok_ID=" +QVariant(Rahunok).toString()+
                                " and month=" +QVariant(kincDate.month()).toString()+
                                " and year=" +QVariant(kincDate.year()).toString());
        pokaznQuery->seek(0);
        potSubs = pokaznQuery->value(0).toDouble() + pokaznQuery->value(1).toDouble() + pokaznQuery->value(2).toDouble() + pokaznQuery->value(3).toDouble();

        pokaznQuery->exec("SELECT sum(Suma) FROM kvytancii \
                            WHERE Rahunok_ID=" +QVariant(Rahunok).toString()+
                            " and year(Plata_date)=" +QVariant(kincDate.year()).toString()+
                            " and month(Plata_date)=" +QVariant(kincDate.month()).toString());
        pokaznQuery->seek(0);
        potOplata = pokaznQuery->value(0).toDouble();

        pokaznQuery->exec("SELECT sum(Suma) FROM narahuvannyaPereved \
                            WHERE Rahunok_id="+sqlStr(Rahunok)+" \
                            and year(CDate)="+sqlStr(kincDate.year())+" \
                            and month(CDate)="+sqlStr(kincDate.month()));
        pokaznQuery->next();
        potPereved = uDToM(pokaznQuery->value(0).toDouble());

        pokaznQuery->exec("SELECT sum(Vh_saldo), sum(ZaborgAdd), sum(OplataSuma) \
                            FROM debitorskaZaborg \
                            WHERE Rahunok="+sqlStr(Rahunok)+" \
                            and year(CDate)="+sqlStr(kincDate.year())+" \
                            and month(CDate)="+sqlStr(kincDate.month()));
        if (pokaznQuery->next())
            potDebitorska = uDToM(pokaznQuery->value(0).toDouble() + pokaznQuery->value(1).toDouble() - pokaznQuery->value(2).toDouble());
        else
            potDebitorska = 0;

    ///
        zagVyh_Saldo = potVh_Saldo + potNarah + potNarahGV + potNarahST - potSubs - potOplata + potPereved + potDebitorska;
    ///
        if (!isPereplata) goto l;
        if (zagVyh_Saldo <= minSum && zagVyh_Saldo < 0){
                goto m;
                l:
        if (zagVyh_Saldo >= minSum){
                m:
            iterBorj++;

            pokaznQuery->exec("SELECT Vh_saldo FROM narahuvannya \
                                WHERE Rahunok=" +QVariant(Rahunok).toString()+
                                " and month=" +QVariant(pochDate.month()).toString()+
                                " and year=" +QVariant(pochDate.year()).toString()+
                                " and Status=0");
            pokaznQuery->seek(0);
            zagVh_Saldo = pokaznQuery->value(0).toDouble();

            pokaznQuery->exec("SELECT sum(Narah), sum(Narah_GV), sum(Narah_ST) FROM narahuvannya \
                               WHERE Rahunok="+QVariant(Rahunok).toString()+" and \
                              (("+QVariant(pochDate.year()).toString()+"="+QVariant(kincDate.year()).toString()+" \
                               and year="+QVariant(kincDate.year()).toString()+" \
                               and month<="+QVariant(kincDate.month()).toString()+" \
                               and month>="+QVariant(pochDate.month()).toString()+") \
                               OR ("+QVariant(pochDate.year()).toString()+"<"+QVariant(kincDate.year()).toString()+" \
                               and ((year="+QVariant(pochDate.year()).toString()+" \
                               and month>="+QVariant(pochDate.month()).toString()+") \
                               or (year>"+QVariant(pochDate.year()).toString()+" and year<"+QVariant(kincDate.year()).toString()+") \
                               or (year="+QVariant(kincDate.year()).toString()+" and month<="+QVariant(kincDate.month()).toString()+"))))");
            pokaznQuery->seek(0);
            zagNarah = pokaznQuery->value(0).toDouble();
            zagNarahGV = pokaznQuery->value(1).toDouble();
            zagNarahST = pokaznQuery->value(2).toDouble();

            pokaznQuery->exec("SELECT sum(Suma), sum(Suma_d), sum(Suma_GV), sum(Suma_d_GV) FROM subs \
                               WHERE Rahunok="+QVariant(Rahunok).toString()+" and Status=0 and \
                              (("+QVariant(pochDate.year()).toString()+"="+QVariant(kincDate.year()).toString()+" \
                              and year="+QVariant(kincDate.year()).toString()+" \
                              and month<="+QVariant(kincDate.month()).toString()+" \
                              and month>="+QVariant(pochDate.month()).toString()+") \
                              OR ("+QVariant(pochDate.year()).toString()+"<"+QVariant(kincDate.year()).toString()+" \
                              and ((year="+QVariant(pochDate.year()).toString()+" \
                              and month>="+QVariant(pochDate.month()).toString()+") \
                              or (year>"+QVariant(pochDate.year()).toString()+" and year<"+QVariant(kincDate.year()).toString()+") \
                              or (year="+QVariant(kincDate.year()).toString()+" and month<="+QVariant(kincDate.month()).toString()+"))))");
            pokaznQuery->seek(0);
            zagSubs = pokaznQuery->value(0).toDouble() + pokaznQuery->value(1).toDouble() + pokaznQuery->value(2).toDouble() + pokaznQuery->value(3).toDouble();

            int opl_poch_month = pochDate.month();
            int opl_poch_year = pochDate.year();
            int opl_kinc_month = kincDate.month();
            int opl_kinc_year = kincDate.year();
            opl_poch_month --;
            if (opl_poch_month==0){
                opl_poch_month = 12;
                opl_poch_year --;
            }
            opl_kinc_month ++;
            if (opl_kinc_month==13){
                opl_kinc_month = 1;
                opl_kinc_year ++;
            }
            pokaznQuery->exec("SELECT sum(Suma) FROM kvytancii \
                               WHERE Rahunok_ID=" +QVariant(Rahunok).toString()+
                              " and Plata_date BETWEEN CDate('1."+QVariant(opl_poch_month).toString()+"."+QVariant(opl_poch_year).toString()+"') \
                                                AND CDATE('1."+QVariant(opl_kinc_month).toString()+"."+QVariant(opl_kinc_year).toString()+"')");
            pokaznQuery->seek(0);
            zagOplata = pokaznQuery->value(0).toDouble();

            pokaznQuery->exec("SELECT sum(Suma) FROM narahuvannyaPereved \
                               WHERE Rahunok_id="+sqlStr(Rahunok)+" \
                                and (CDate BETWEEN cdate("+sqlStr(pochDate)+") AND cdate("+sqlStr(kincDate)+"))");
            pokaznQuery->next();
            zagPereved = uDToM(pokaznQuery->value(0).toDouble());

            zagDebitorska = potDebitorska;


            //��������� ������ ��������� � ������� ��� ������ �������
            if (old_bud != abonQuery->value(1).toInt()){
                cursor.movePosition(QTextCursor::End);
                blockFormat.setAlignment( Qt::AlignLeft );
                cursor.insertBlock(blockFormat);
                textCharFormat.setFontPointSize( 9 );
                budQuery->exec("SELECT Vulycia & ' ' & Bud_num FROM budynky WHERE id="+abonQuery->value(1).toString());
                budQuery->seek(0);

                //��������� ����� ������� �������� ��� ��������� �������
                table = cursor.insertTable(2, 12, tableFormat);
                table->mergeCells ( 0, 0, 1, 12);

                //���������� ����� �������
                cell = table->cellAt(0, 0);
                cellCursor = cell.firstCursorPosition();
                blockFormat.setAlignment( Qt::AlignCenter );
                cellCursor.setBlockFormat( blockFormat );
                cellCursor.insertText(codec->toUnicode("������� ") + budQuery->value(0).toString(), textCharFormat);

                cell = table->cellAt(1, 0);
                cellCursor = cell.firstCursorPosition();
                blockFormat.setAlignment( Qt::AlignCenter );
                cellCursor.setBlockFormat( blockFormat );
                cellCursor.insertText(codec->toUnicode("� �/�"), textCharFormat);

                cell = table->cellAt(1, 1);
                cellCursor = cell.firstCursorPosition();
                blockFormat.setAlignment( Qt::AlignCenter );
                cellCursor.setBlockFormat( blockFormat );
                cellCursor.insertText(codec->toUnicode("�������"), textCharFormat);

                cell = table->cellAt(1, 2);
                cellCursor = cell.firstCursorPosition();
                blockFormat.setAlignment( Qt::AlignCenter );
                cellCursor.setBlockFormat( blockFormat );
                cellCursor.insertText(codec->toUnicode("� ���."), textCharFormat);

                cell = table->cellAt(1, 3);
                cellCursor = cell.firstCursorPosition();
                blockFormat.setAlignment( Qt::AlignCenter );
                cellCursor.setBlockFormat( blockFormat );
                cellCursor.insertText(codec->toUnicode("� � �"), textCharFormat);

                cell = table->cellAt(1, 4);
                cellCursor = cell.firstCursorPosition();
                blockFormat.setAlignment( Qt::AlignCenter );
                cellCursor.setBlockFormat( blockFormat );
                cellCursor.insertText(codec->toUnicode("�����."), textCharFormat);

                cell = table->cellAt(1, 5);
                cellCursor = cell.firstCursorPosition();
                blockFormat.setAlignment( Qt::AlignCenter );
                cellCursor.setBlockFormat( blockFormat );
                cellCursor.insertText(codec->toUnicode("��. ������"), textCharFormat);

                cell = table->cellAt(1, 6);
                cellCursor = cell.firstCursorPosition();
                blockFormat.setAlignment( Qt::AlignCenter );
                cellCursor.setBlockFormat( blockFormat );
                cellCursor.insertText(codec->toUnicode("�����."), textCharFormat);

                cell = table->cellAt(1, 7);
                cellCursor = cell.firstCursorPosition();
                blockFormat.setAlignment( Qt::AlignCenter );
                cellCursor.setBlockFormat( blockFormat );
                cellCursor.insertText(codec->toUnicode("����."), textCharFormat);

                cell = table->cellAt(1, 8);
                cellCursor = cell.firstCursorPosition();
                blockFormat.setAlignment( Qt::AlignCenter );
                cellCursor.setBlockFormat( blockFormat );
                cellCursor.insertText(codec->toUnicode("�����."), textCharFormat);

                cell = table->cellAt(1, 9);
                cellCursor = cell.firstCursorPosition();
                blockFormat.setAlignment( Qt::AlignCenter );
                cellCursor.setBlockFormat( blockFormat );
                cellCursor.insertText(codec->toUnicode("�����. ������. + ����."), textCharFormat);

                cell = table->cellAt(1, 10);
                cellCursor = cell.firstCursorPosition();
                blockFormat.setAlignment( Qt::AlignCenter );
                cellCursor.setBlockFormat( blockFormat );
                cellCursor.insertText(codec->toUnicode("���. ������"), textCharFormat);

                cell = table->cellAt(1, 11);
                cellCursor = cell.firstCursorPosition();
                blockFormat.setAlignment( Qt::AlignCenter );
                cellCursor.setBlockFormat( blockFormat );
                cellCursor.insertText(codec->toUnicode("���. �����."), textCharFormat);
                //ʳ���� ������ � ��������

            //����� ����� � ������� �������
                sumaryTable->insertRows(sumaryTableRow, 1);


                tableCell.setAlignment(Qt::AlignCenter);
                //"� �/�"
                tableCell.set(sumaryTableRow,0,sumaryTableRow);
                //"�������"
                tableCell.setAlignment(Qt::AlignLeft);
                tableCell.set(sumaryTableRow,1,budQuery->value(0).toString());
                if (old_bud > 0){
                    tableCell.setAlignment(Qt::AlignRight);
                    //"�-��� ��������"
                    tableCell.set(sumaryTableRow-1,2,sumaryTableBorgBudCount);
                    //"���� �����, ���."
                    tableCell.set(sumaryTableRow-1,3,(isPereplata) ? sumaryTableBorgBudSum*-1 : sumaryTableBorgBudSum);
                }
                sumaryTableBorgAllCount += sumaryTableBorgBudCount;
                sumaryTableBorgAllSum += sumaryTableBorgBudSum;
                sumaryTableRow++;
                sumaryTableBorgBudCount = 0;
                sumaryTableBorgBudSum = 0;
                //ʳ���� ������ ����� � ������� �������

                old_bud = abonQuery->value(1).toInt();
                iterBorj = 0;
            }

            table->insertRows( iterBorj+2, 1 );
            //�������� ����� � ������
            pokaznQuery->exec("SELECT Prizv, Imya, Batk, Kvartyra, tel FROM abonent \
                                WHERE Rahunok=" + QVariant(Rahunok).toString());
            pokaznQuery->seek(0);
                //� �/�"
            cell = table->cellAt(iterBorj+2, 0);
            cellCursor = cell.firstCursorPosition();
            blockFormat.setAlignment( Qt::AlignCenter );
            cellCursor.setBlockFormat( blockFormat );
            cellCursor.insertText(QVariant(iterBorj+1).toString(), textCharFormat);
                //�������
            cell = table->cellAt(iterBorj+2, 1);
            cellCursor = cell.firstCursorPosition();
            blockFormat.setAlignment( Qt::AlignCenter );
            cellCursor.setBlockFormat( blockFormat );
            cellCursor.insertText(QVariant(Rahunok).toString(), textCharFormat);
                //�������
            cell = table->cellAt(iterBorj+2, 2);
            cellCursor = cell.firstCursorPosition();
            blockFormat.setAlignment( Qt::AlignCenter );
            cellCursor.setBlockFormat( blockFormat );
            cellCursor.insertText(pokaznQuery->value(4).toString());
            //� � �
            cell = table->cellAt(iterBorj+2, 3);
            cellCursor = cell.firstCursorPosition();
            blockFormat.setAlignment( Qt::AlignCenter );
            cellCursor.setBlockFormat( blockFormat );
            QString str = pokaznQuery->value(0).toString() + " ";
            if (pokaznQuery->value(1).toString().size() > 0)
                str += pokaznQuery->value(1).toString().at(0) + QString(".");
            if (pokaznQuery->value(2).toString().size() > 0)
                str += pokaznQuery->value(2).toString().at(0) + QString(".");
            cellCursor.insertText( str, textCharFormat);
            //��������
            cell = table->cellAt(iterBorj+2, 4);
            cellCursor = cell.firstCursorPosition();
            blockFormat.setAlignment( Qt::AlignCenter );
            cellCursor.setBlockFormat( blockFormat );
            cellCursor.insertText(pokaznQuery->value(3).toString(), textCharFormat);
             //��.������
            cell = table->cellAt(iterBorj+2, 5);
            cellCursor = cell.firstCursorPosition();
            blockFormat.setAlignment( Qt::AlignCenter );
            cellCursor.setBlockFormat( blockFormat );
            cellCursor.insertText(uMToStr2(zagVh_Saldo), textCharFormat);
            //�������.
            cell = table->cellAt(iterBorj+2, 6);
            cellCursor = cell.firstCursorPosition();
            blockFormat.setAlignment( Qt::AlignCenter );
            cellCursor.setBlockFormat( blockFormat );
            cellCursor.insertText(uMToStr2(zagNarah + zagNarahGV + zagNarahST), textCharFormat);
              //����.
            cell = table->cellAt(iterBorj+2, 7);
            cellCursor = cell.firstCursorPosition();
            blockFormat.setAlignment( Qt::AlignCenter );
            cellCursor.setBlockFormat( blockFormat );
            cellCursor.insertText(uMToStr2(zagSubs), textCharFormat);
            //�����.
            cell = table->cellAt(iterBorj+2, 8);
            cellCursor = cell.firstCursorPosition();
            blockFormat.setAlignment( Qt::AlignCenter );
            cellCursor.setBlockFormat( blockFormat );
            cellCursor.insertText(uMToStr2(zagOplata), textCharFormat);
            //����������� �������������
            cell = table->cellAt(iterBorj+2, 9);
            cellCursor = cell.firstCursorPosition();
            blockFormat.setAlignment( Qt::AlignCenter );
            cellCursor.setBlockFormat( blockFormat );
            cellCursor.insertText(uMToStr2(zagPereved + zagDebitorska), textCharFormat);
            //���. ������
            cell = table->cellAt(iterBorj+2, 10);
            cellCursor = cell.firstCursorPosition();
            blockFormat.setAlignment( Qt::AlignCenter );
            cellCursor.setBlockFormat( blockFormat );
            cellCursor.insertText((isPereplata)? uMToStr2(zagVyh_Saldo*-1) : uMToStr2(zagVyh_Saldo), textCharFormat);
            //���.�����.
            cell = table->cellAt(iterBorj+2, 11);
            cellCursor = cell.firstCursorPosition();
            blockFormat.setAlignment( Qt::AlignCenter );
            cellCursor.setBlockFormat( blockFormat );
            cellCursor.insertText(uMToStr2(potNarah), textCharFormat);

            sumaryTableBorgBudCount++;
            sumaryTableBorgBudSum += zagVyh_Saldo;
        }
        }
        postup_form->inc_pos();
     }

     //����� ������� �������� �� ���� ����� ��� ���������� ������� �� ����� ������� ������� �������� �� ���� �����
    if (old_bud > 0){
         tableCell.setAlignment(Qt::AlignRight);
         //"�-��� ��������"
        tableCell.set(sumaryTableRow-1,2,sumaryTableBorgBudCount);
        //"���� �����, ���."
        tableCell.set(sumaryTableRow-1,3,(isPereplata) ? sumaryTableBorgBudSum*-1 : sumaryTableBorgBudSum);
    }
    sumaryTableBorgAllCount += sumaryTableBorgBudCount;
    sumaryTableBorgAllSum += sumaryTableBorgBudSum;
    sumaryTable->mergeCells(sumaryTableRow,0,1,2);
    tableCell.set(sumaryTableRow,0,"������");
    //"�-��� ��������"
    tableCell.set(sumaryTableRow,2,sumaryTableBorgAllCount);
    //"���� �����, ���."
    tableCell.set(sumaryTableRow,3,(isPereplata) ? sumaryTableBorgAllSum*-1 : sumaryTableBorgAllSum);


    delete abonQuery;
    delete budQuery;
    delete pokaznQuery;
    delete postup_form;
    printform->printer()->setDocName((isPereplata)? "³������ ���������" : "³������ ��������");
    printform->show();
}
//------------------------------------------------------------
void UPrintDocs::print_vidomist_borjnykiv_riznyc(QDate pochDate, QDate kincDate, double minBorg, int BudId)
{
	if (minBorg==0)
		minBorg = 0.1;
	//��������� ���������
	printform = new UPrintForm(0, codec->toUnicode("³������ �������� �� ��������� �����"));
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat;
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 13 );
	cursor.insertText(codec->toUnicode("²��̲��� �� ����²���� �����"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 12 );
	cursor.insertText(codec->toUnicode("��������, �������� ���� ���� �������� ") + 
						uMToStr2(minBorg) +
						codec->toUnicode(" ���."), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 9 );
	cursor.insertText(codec->toUnicode("����������� �������� ������: ") +
						QVariant(pochDate.month()).toString() + "." +
						QVariant(pochDate.year()).toString() + "." +
						codec->toUnicode("�. - ") +
						QVariant(kincDate.month()).toString() + "." +
						QVariant(kincDate.year()).toString() + "." +
						codec->toUnicode("�."), textCharFormat);
	cursor.insertBlock(blockFormat);
	
	//������������ �������
	QTextTable *table = NULL;
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
	constraints << QTextLength(QTextLength::FixedLength, 20);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 120);
	constraints << QTextLength(QTextLength::FixedLength, 30);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(1);
	tableFormat.setBorder(0.5);
	tableFormat.setBorderBrush(QColor(Qt::black));
	
	//����� � �������� �������� �� ��������
	QSqlQuery *abonQuery = new QSqlQuery();
	QSqlQuery *budQuery = new QSqlQuery();
	QSqlQuery *pokaznQuery = new QSqlQuery();
	QString filtrStr;
	if (BudId>0)
		filtrStr = " WHERE Budynok_ID="+QVariant(BudId).toString();
	else
		filtrStr = "";
	
	abonQuery->exec("SELECT count(*) FROM abonent" + filtrStr);
	abonQuery->seek(0);
	UPostupForm *postup_form = new UPostupForm(0, abonQuery->value(0).toInt());
	postup_form->show();
	
	abonQuery->exec("SELECT Rahunok, Budynok_ID FROM abonent INNER JOIN budynky ON budynky.id=abonent.budynok_ID"
						+filtrStr+" ORDER BY Vulycia, Bud_num, Rahunok");
	int old_bud=0;
	int iterBorj=0;
	int zagKilkBorjn=0;
	double zagSumnivnBorg = 0, zagVyhBorg = 0;
	while (abonQuery->next()){
		double zagNarah, zagVyh_Saldo;
		double potVh_Saldo, potNarah, potSubs, potOplata;
		int Rahunok = abonQuery->value(0).toInt();
		//��������� �������� ���������
		pokaznQuery->exec("SELECT Vh_saldo FROM narahuvannya \
							WHERE Rahunok=" +QVariant(Rahunok).toString()+
									" and month=" +QVariant(kincDate.month()).toString()+
									" and year=" +QVariant(kincDate.year()).toString()+
									" and Status=0");
		pokaznQuery->seek(0);
		potVh_Saldo = pokaznQuery->value(0).toDouble();
		
		pokaznQuery->exec("SELECT sum(Narah), sum(Narah_GV), sum(Narah_ST) FROM narahuvannya \
							WHERE Rahunok=" +QVariant(Rahunok).toString()+
									" and month=" +QVariant(kincDate.month()).toString()+
									" and year=" +QVariant(kincDate.year()).toString());
		pokaznQuery->seek(0);
		potNarah = pokaznQuery->value(0).toDouble() + pokaznQuery->value(1).toDouble() + pokaznQuery->value(2).toDouble();
		
		pokaznQuery->exec("SELECT sum(Suma), sum(Suma_d), sum(Suma_GV), sum(Suma_d_GV) FROM subs \
							WHERE Rahunok_ID=" +QVariant(Rahunok).toString()+
									" and month=" +QVariant(kincDate.month()).toString()+
									" and year=" +QVariant(kincDate.year()).toString());
		pokaznQuery->seek(0);
		potSubs = pokaznQuery->value(0).toDouble() + pokaznQuery->value(1).toDouble() + pokaznQuery->value(2).toDouble() + pokaznQuery->value(3).toDouble();
		
		pokaznQuery->exec("SELECT sum(Suma) FROM kvytancii \
							WHERE Rahunok_ID=" +QVariant(Rahunok).toString()+
							" and year(Plata_date)=" +QVariant(kincDate.year()).toString()+
							" and month(Plata_date)=" +QVariant(kincDate.month()).toString());
		pokaznQuery->seek(0);
		potOplata = pokaznQuery->value(0).toDouble();
		
		///
		zagVyh_Saldo = potVh_Saldo + potNarah - potSubs - potOplata;
		///
		if (zagVyh_Saldo > 0){	
			pokaznQuery->exec("SELECT sum(Narah) FROM narahuvannya \
							WHERE Rahunok="+QVariant(Rahunok).toString()+" and \
							(("+QVariant(pochDate.year()).toString()+"="+QVariant(kincDate.year()).toString()+" \
								and year="+QVariant(kincDate.year()).toString()+" \
								and month<="+QVariant(kincDate.month()).toString()+" \
								and month>="+QVariant(pochDate.month()).toString()+") \
							OR ("+QVariant(pochDate.year()).toString()+"<"+QVariant(kincDate.year()).toString()+" \
								and ((year="+QVariant(pochDate.year()).toString()+" \
									and month>="+QVariant(pochDate.month()).toString()+") \
									or (year>"+QVariant(pochDate.year()).toString()+" and year<"+QVariant(kincDate.year()).toString()+") \
									or (year="+QVariant(kincDate.year()).toString()+" and month<="+QVariant(kincDate.month()).toString()+"))))");
			pokaznQuery->seek(0);
			zagNarah = pokaznQuery->value(0).toDouble();

			int opl_poch_month = pochDate.month();
			int opl_poch_year = pochDate.year();
			int opl_kinc_month = kincDate.month();
			int opl_kinc_year = kincDate.year();
			opl_poch_month --;
			if (opl_poch_month==0){
				opl_poch_month = 12;
				opl_poch_year --;
			}
			opl_kinc_month ++;
			if (opl_kinc_month==13){
				opl_kinc_month = 1;
				opl_kinc_year ++;
			}
			if (zagVyh_Saldo - zagNarah >= minBorg){
				iterBorj++;
					//��������� ������ ��������� � ������� ��� ������ �������
				if (old_bud != abonQuery->value(1).toInt()){
					cursor.movePosition(QTextCursor::End);
					blockFormat.setAlignment( Qt::AlignLeft );
					cursor.insertBlock(blockFormat);
					textCharFormat.setFontPointSize( 9 );
					budQuery->exec("SELECT Vulycia & ' ' & Bud_num FROM budynky WHERE id="+abonQuery->value(1).toString());
					budQuery->seek(0);
					
					//��������� ����� ������� �������� ��� ��������� �������
					table = cursor.insertTable(2, 6, tableFormat);
					table->mergeCells ( 0, 0, 1, 6);
				
						//���������� ����� �������
					cell = table->cellAt(0, 0);
					cellCursor = cell.firstCursorPosition();
					blockFormat.setAlignment( Qt::AlignCenter );
					cellCursor.setBlockFormat( blockFormat );
					cellCursor.insertText(codec->toUnicode("������� ") + budQuery->value(0).toString(), textCharFormat);	
					
					cell = table->cellAt(1, 0);
					cellCursor = cell.firstCursorPosition();
					blockFormat.setAlignment( Qt::AlignCenter );
					cellCursor.setBlockFormat( blockFormat );
					cellCursor.insertText(codec->toUnicode("� �/�"), textCharFormat);
	
					cell = table->cellAt(1, 1);
					cellCursor = cell.firstCursorPosition();
					blockFormat.setAlignment( Qt::AlignCenter );
					cellCursor.setBlockFormat( blockFormat );
					cellCursor.insertText(codec->toUnicode("�������"), textCharFormat);
	
					cell = table->cellAt(1, 2);
					cellCursor = cell.firstCursorPosition();
					blockFormat.setAlignment( Qt::AlignCenter );
					cellCursor.setBlockFormat( blockFormat );
					cellCursor.insertText(codec->toUnicode("� � �"), textCharFormat);
	
					cell = table->cellAt(1, 3);
					cellCursor = cell.firstCursorPosition();
					blockFormat.setAlignment( Qt::AlignCenter );
					cellCursor.setBlockFormat( blockFormat );
					cellCursor.insertText(codec->toUnicode("�����."), textCharFormat);
	
					cell = table->cellAt(1, 4);
					cellCursor = cell.firstCursorPosition();
					blockFormat.setAlignment( Qt::AlignCenter );
					cellCursor.setBlockFormat( blockFormat );
					cellCursor.insertText(codec->toUnicode("������. ����"), textCharFormat);
	
					cell = table->cellAt(1, 5);
					cellCursor = cell.firstCursorPosition();
					blockFormat.setAlignment( Qt::AlignCenter );
					cellCursor.setBlockFormat( blockFormat );
					cellCursor.insertText(codec->toUnicode("���. ����"), textCharFormat);
					//ʳ���� ������ � ��������
			
					old_bud = abonQuery->value(1).toInt();
					iterBorj = 0;
				}
			
				table->insertRows( iterBorj+2, 1 );
				//�������� ����� � ������
				pokaznQuery->exec("SELECT Prizv, Imya, Batk, Kvartyra FROM abonent \
									WHERE Rahunok=" + QVariant(Rahunok).toString());
				pokaznQuery->seek(0);
					//� �/�"
				cell = table->cellAt(iterBorj+2, 0);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				cellCursor.insertText(QVariant(iterBorj+1).toString(), textCharFormat);
					//�������
				cell = table->cellAt(iterBorj+2, 1);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				cellCursor.insertText(QVariant(Rahunok).toString(), textCharFormat);
					//� � �
				cell = table->cellAt(iterBorj+2, 2);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				cellCursor.insertText(pokaznQuery->value(0).toString() + " " + pokaznQuery->value(1).toString().at(0) + "." +
										pokaznQuery->value(2).toString().at(0) + ".", textCharFormat);
				//��������
				cell = table->cellAt(iterBorj+2, 3);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				cellCursor.insertText(pokaznQuery->value(3).toString(), textCharFormat);
					//������. ����
				cell = table->cellAt(iterBorj+2, 4);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				cellCursor.insertText(uMToStr2(zagVyh_Saldo - zagNarah), textCharFormat);
					//�������� ����
				cell = table->cellAt(iterBorj+2, 5);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				cellCursor.insertText(uMToStr2(zagVyh_Saldo), textCharFormat);
				
				zagSumnivnBorg += zagVyh_Saldo - zagNarah;
				zagVyhBorg += zagVyh_Saldo;
				zagKilkBorjn ++;
			}
		postup_form->inc_pos();
		}
	}
	
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("     �������� ������� �������� - ") + QVariant(zagKilkBorjn).toString() + codec->toUnicode("."));
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("     �������� ���� ��������� ����� - ") + uMToStr2(zagSumnivnBorg) + codec->toUnicode("���."));
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("     �������� ���� ��������� ����� �� ����� ������ - ") + uMToStr2(zagVyhBorg) + codec->toUnicode("���."));
	
	delete abonQuery;
	delete budQuery;
	delete pokaznQuery;
	delete postup_form;
	printform->printer()->setDocName("³������ ��������");
	printform->show();
}
//------------------------------------------------------------
void UPrintDocs::print_vidomist_wrong_subs(int cur_year, int cur_month)
{
		int year = cur_year;
		int month = cur_month;
		printform = new UPrintForm(0, codec->toUnicode("³������ ��������� ����������� ������� ��"));
		QTextBlockFormat blockFormat;
		QTextTableCell cell;
		QTextCharFormat textCharFormat;
		//��������� ��������� ������� ��� ��� ������� �� X �����, Y ��
		printform->document()->clear();
		QTextCursor cursor(printform->document()), cellCursor;
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.setBlockFormat( blockFormat );
		textCharFormat.setFontPointSize( 13 );
		cursor.insertText(codec->toUnicode("²��̲���\n��������� ���������� �����䳿 ��"), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 8 );
		cursor.insertText(codec->toUnicode(" "), textCharFormat);
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 11 );
		cursor.insertText(codec->toUnicode("�� ") + monthlist.at(month-1) +
							codec->toUnicode(" ") + QVariant(year).toString() + codec->toUnicode("�."), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText(codec->toUnicode("�� ��������� ")+OrganizName()+"\n");
		textCharFormat.setFontPointSize( 8 );
		cursor.insertText(codec->toUnicode(" "), textCharFormat);
			//��������� � ������������ �������
		QTextTableFormat tableFormat;
		QVector<QTextLength> constraints;
		constraints << QTextLength(QTextLength::FixedLength, 20);
		constraints << QTextLength(QTextLength::FixedLength, 70);
		constraints << QTextLength(QTextLength::FixedLength, 70);
		constraints << QTextLength(QTextLength::FixedLength, 150);
		constraints << QTextLength(QTextLength::FixedLength, 125);
		constraints << QTextLength(QTextLength::FixedLength, 100);
		constraints << QTextLength(QTextLength::FixedLength, 100);
		tableFormat.setColumnWidthConstraints(constraints);
		tableFormat.setCellSpacing(0);
		tableFormat.setCellPadding(1);
		tableFormat.setBorder(0.5);
		QTextTable *table = cursor.insertTable(1, 7, tableFormat);
			//���������� ����� �������
		cell = table->cellAt(0, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("� �/�"));
	
		cell = table->cellAt(0, 1);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("�������"));
	
		cell = table->cellAt(0, 2);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("����� ����."));
		
		cell = table->cellAt(0, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("� � �"));
		
		cell = table->cellAt(0, 4);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("������"));
		
		cell = table->cellAt(0, 5);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("���� �����. �� ��, ���"));
		
		cell = table->cellAt(0, 6);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("���� ����� ����., ���"));
			//������������ ������� ������
		QSqlQuery query;
		QSqlQuery *budynokQuery = new QSqlQuery();
		
		query.exec("SELECT count(*) \
							FROM (SELECT Rahunok, sum(Narah) AS Suma_narah \
								FROM narahuvannya \
								WHERE year="+QVariant(year).toString()+" \
										and month="+QVariant(month).toString()+" GROUP BY Rahunok) AS cur_narahuvannya, \
						(SELECT Rahunok_ID, sum(Suma) AS Suma_subs FROM subs \
								WHERE year="+QVariant(year).toString()+" \
										and month="+QVariant(month).toString()+" GROUP BY Rahunok_ID) AS cur_subs \
								WHERE cur_subs.Rahunok_ID = cur_narahuvannya.Rahunok \
										and (cur_subs.Suma_subs-cur_narahuvannya.Suma_narah)>=0.01");
		query.seek(0);
		table->insertRows( 1, query.value(0).toInt() );	
		//��������� ����� �������
		UPostupForm *postup_form = new UPostupForm(0, query.value(0).toInt() );
		postup_form->show();
			//���������� ������ �����������
		query.exec("SELECT wrong_subs.Rahunok, spravy_subs.Nom_spravy, \
							abonent.Prizv & ' ' & abonent.Imya & ' ' & abonent.Batk, \
							wrong_subs.Suma_narah, wrong_subs.Suma_subs, abonent.Budynok_ID, abonent.Kvartyra\
					FROM ((SELECT cur_narahuvannya.Rahunok, cur_subs.Suma_subs, cur_narahuvannya.Suma_narah \
							FROM (SELECT Rahunok, sum(Narah) AS Suma_narah \
								FROM narahuvannya \
								WHERE year="+QVariant(year).toString()+" \
										and month="+QVariant(month).toString()+" GROUP BY Rahunok) AS cur_narahuvannya, \
						(SELECT Rahunok_ID, sum(Suma) AS Suma_subs FROM subs \
								WHERE year="+QVariant(year).toString()+" \
										and month="+QVariant(month).toString()+" GROUP BY Rahunok_ID) AS cur_subs \
								WHERE cur_subs.Rahunok_ID = cur_narahuvannya.Rahunok \
										and (cur_subs.Suma_subs-cur_narahuvannya.Suma_narah)>=0.01) AS wrong_subs \
					LEFT JOIN spravy_subs ON wrong_subs.Rahunok=spravy_subs.Rahunok_ID) \
					LEFT JOIN abonent ON wrong_subs.Rahunok=abonent.Rahunok");
		
		int iter=0;
		while (query.next()){
			// � �/�
			cell = table->cellAt(1+iter, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( QVariant(iter+1).toString() );
			// �������
			cell = table->cellAt(1+iter, 1);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( query.value(0).toString() );
			// ����� ����
			cell = table->cellAt(1+iter, 2);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( query.value(1).toString() );
			// � � �
			cell = table->cellAt(1+iter, 3);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( query.value(2).toString() );
			// ������
			cell = table->cellAt(1+iter, 4);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			budynokQuery->exec("SELECT Vulycia & ' ' & Bud_num FROM budynky WHERE id="+query.value(5).toString());
			budynokQuery->seek(0);
			cellCursor.insertText( budynokQuery->value(0).toString() + codec->toUnicode(", ��.") + query.value(6).toString() );
			// ���� �����. �� ��
			cell = table->cellAt(1+iter, 5);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(query.value(3).toDouble()) );
			// ���� ����� ����.
			cell = table->cellAt(1+iter, 6);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(query.value(4).toDouble()) );

			iter ++;
			postup_form->inc_pos();
		}
		cursor.movePosition(QTextCursor::End);
		blockFormat.setAlignment( Qt::AlignLeft );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText("     "+DocsPosadaPidpysText(3)+"					"+PrizvIB(3));
		
			//ʳ���� ��������� ��������� ��� �����
		delete budynokQuery;
		postup_form->close();
		printform->printer()->setDocName("³������ ��������� ����������� ������� �� "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"�.");
		printform->show();
}
//------------------------------------------------------------
void UPrintDocs::print_vidomist_narah_opl_za_CO_dlya_viddilu_subs(int cur_year, int cur_month)
{
	int year = cur_year;
	int month = cur_month;
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT count(*) FROM narahuvannya \
					WHERE Year="+QVariant(year).toString()+" \
						and Month="+QVariant(month).toString()+" \
						and Narah>0.01");
	query->seek(0);
	int numAbonOpal = query->value(0).toInt();
	UPostupForm *postup_form = new UPostupForm(0, numAbonOpal );
	postup_form->show();
	printform = new UPrintForm(0, codec->toUnicode("³������ �����. ���. �� �� ��� ³���� �������"));
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat;
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignTop);
	//��������� ��������� ������� ��� ��� ������� �� X �����, Y ��
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 13 );
	cursor.insertText(codec->toUnicode("²��̲���\n ����������� ������ �� �� ��� ³���� �������"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 8 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("�� ��������� ")+OrganizName()+"\n");
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("�� ") + monthlist.at(month-1) +
						codec->toUnicode(" ") + QVariant(year).toString() + codec->toUnicode("�."), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 10 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
		//��������� � ������������ �������
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
	constraints << QTextLength(QTextLength::FixedLength, 20);
	constraints << QTextLength(QTextLength::FixedLength, 20);
	constraints << QTextLength(QTextLength::FixedLength, 20);
	constraints << QTextLength(QTextLength::FixedLength, 150);
	constraints << QTextLength(QTextLength::FixedLength, 120);
	constraints << QTextLength(QTextLength::FixedLength, 90);
	constraints << QTextLength(QTextLength::FixedLength, 10);
	constraints << QTextLength(QTextLength::FixedLength, 10);
	constraints << QTextLength(QTextLength::FixedLength, 10);
	constraints << QTextLength(QTextLength::FixedLength, 30);
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(1);
	tableFormat.setBorder(0.5);
	QTextTable *table = cursor.insertTable(numAbonOpal+1, 10, tableFormat);
		//���������� ����� �������
	cell = table->cellAt(0, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("� �/�"), textCharFormat);

	cell = table->cellAt(0, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("���."), textCharFormat);

	cell = table->cellAt(0, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("����� ����."), textCharFormat);
	
	cell = table->cellAt(0, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("� � �"), textCharFormat);
	
	cell = table->cellAt(0, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("������"), textCharFormat);
	
	cell = table->cellAt(0, 5);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("ϳ����"), textCharFormat);
	
	cell = table->cellAt(0, 6);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("�����"), textCharFormat);
	
	cell = table->cellAt(0, 7);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("�-��� ����."), textCharFormat);
	
	cell = table->cellAt(0, 8);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("����� �����."), textCharFormat);
	
	cell = table->cellAt(0, 9);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("���� �����."), textCharFormat);
	
	query->exec("SELECT abonent.Rahunok, IIf(spravy_subs.Nom_spravy>0, spravy_subs.Nom_spravy, '-'), abonent.Prizv & ' ' & abonent.Imya & ' ' & abonent.Batk, \
					budynky.Vulycia & ' ' & budynky.Bud_num & ', "+codec->toUnicode("��")+".' & abonent.Kvartyra, \
					IIF(abonent.Pilga_ID>0, IIf(abonent.Pilga_ID_2>0, '1)' & pilgy.Vidsotok & '% "+codec->toUnicode("���")+" ' & abonent.Pilga_for & ' "+codec->toUnicode("���")+". 2)' & pilgy_2.Vidsotok & '% "+codec->toUnicode("���")+" ' & abonent.Pilga_for_2 & ' "+codec->toUnicode("���")+".'    ,\
						pilgy.Vidsotok & '% "+codec->toUnicode("���")+" ' & abonent.Pilga_for & ' "+codec->toUnicode("���")+".'), '-'), \
					abonent.Ploshcha, abonent.Ludy, abonent.Zaiva_ploshcha, narahuvannya.Narah \
				FROM ((((abonent \
				LEFT JOIN spravy_subs \
					ON spravy_subs.Rahunok_id=abonent.Rahunok) \
				LEFT JOIN budynky \
					ON budynky.id=abonent.Budynok_id) \
				LEFT JOIN pilgy \
					ON pilgy.id=abonent.Pilga_id) \
				LEFT JOIN (SELECT * FROM pilgy) AS pilgy_2 \
					ON pilgy_2.id=abonent.Pilga_id_2) \
				LEFT JOIN narahuvannya \
					ON narahuvannya.Rahunok=abonent.Rahunok \
				WHERE narahuvannya.year="+QVariant(year).toString()+" and narahuvannya.month="+QVariant(month).toString()+" and narahuvannya.Narah>0.01 \
				ORDER BY abonent.Rahunok");
	for (int iter=0; iter<numAbonOpal; iter++){
		query->next();
		
		cell = table->cellAt(iter+1, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QVariant(iter+1).toString(), textCharFormat);

		cell = table->cellAt(iter+1, 1);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query->value(0).toString(), textCharFormat);

		cell = table->cellAt(iter+1, 2);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query->value(1).toString(), textCharFormat);
	
		cell = table->cellAt(iter+1, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query->value(2).toString(), textCharFormat);
	
		cell = table->cellAt(iter+1, 4);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query->value(3).toString(), textCharFormat);
	
		cell = table->cellAt(iter+1, 5);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query->value(4).toString(), textCharFormat);
	
		cell = table->cellAt(iter+1, 6);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(query->value(5).toDouble()), textCharFormat);
	
		cell = table->cellAt(iter+1, 7);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query->value(6).toString(), textCharFormat);
	
		cell = table->cellAt(iter+1, 8);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(query->value(7).toDouble()), textCharFormat);
	
		cell = table->cellAt(iter+1, 9);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(query->value(8).toDouble()), textCharFormat);
		
		postup_form->inc_pos();
	}
		
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("     "+DocsPosadaPidpysText(3)+"					"+PrizvIB(3));
	
			//ʳ���� ��������� ��������� ��� �����
	printform->printer()->setDocName("³������ ����������� ��� ����� �����䳿 �� "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"�.");
	printform->show();
	delete query;
	delete postup_form;
}
//------------------------------------------------------------
void UPrintDocs::print_dovidnyk_abonentiv()
{
	QSqlQuery *query = new QSqlQuery();
	QSqlQuery *budynkyQuery = new QSqlQuery();
	query->exec("SELECT count(*) FROM abonent");
	query->seek(0);
	int numAbon = query->value(0).toInt();
	UPostupForm *postup_form = new UPostupForm(0,  numAbon);
	postup_form->show();
	printform = new UPrintForm(0, codec->toUnicode("������� ��������"));
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat, textCharFormat_italic;
	//��������� ��������� ������� ��� ��� ������� �� X �����, Y ��
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 13 );
	cursor.insertText(codec->toUnicode("��²���� ������Ҳ�\n")+OrganizName(), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 8 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("��������� �� \"�������� ��������\" ") + date.toString("dd.MM.yyyy") + codec->toUnicode("�."), textCharFormat);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("�������� � ��� ����� - ") + QVariant(numAbon).toString(), textCharFormat);
	
	textCharFormat.setFontPointSize( 9 );
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	textCharFormat_italic.setFontPointSize( 9 );
	textCharFormat_italic.setFontItalic( true );
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	//������� ����������� ����������� CO � ��
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	bool needCO = settings->value("GUI_View/ShowNarahCO").toBool();
	bool needGV = settings->value("GUI_View/ShowNarahGV").toBool();

	delete settings;
	
	int pidsCO[3] = {0,0,0};
	int pidsGV[3] = {0,0,0};
	double pidsPLOS[3] = {0,0,0};
	int pidsLUDY[3] = {0,0,0};
	
	//���������� �������� ����
	QHash<short, QString> hash;
	query->exec("SELECT id, Kategoriya FROM pilgy ORDER BY id");
	while (query->next()){
		hash.insert(query->value(0).toInt(), query->value(1).toString());
	}
	
	budynkyQuery->exec("SELECT id, Vulycia, Bud_num FROM budynky ORDER BY Vulycia, Bud_num");
	int abonIter=1;
	while (budynkyQuery->next()){
		double zagPloshcha=0, opalPloshcha=0, vidklPloshcha=0;
		int kilkOpal=0, kilkVidkl=0;
		int kilkGV=0, kilkVidklGV=0;
		int kilkLudOpal=0, kilkLudVidkl=0;
		
	
		cursor.movePosition(QTextCursor::End);
		blockFormat.setAlignment( Qt::AlignLeft );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		//��������� � ������������ �������
		QTextTableFormat tableFormat;
		QVector<QTextLength> constraints;
		constraints << QTextLength(QTextLength::FixedLength, 40);
		constraints << QTextLength(QTextLength::FixedLength, 40);
		constraints << QTextLength(QTextLength::FixedLength, 180);
		constraints << QTextLength(QTextLength::FixedLength, 40);
		constraints << QTextLength(QTextLength::FixedLength, 60);
		constraints << QTextLength(QTextLength::FixedLength, 40);
		constraints << QTextLength(QTextLength::FixedLength, 40);
		constraints << QTextLength(QTextLength::FixedLength, 120);
		constraints << QTextLength(QTextLength::FixedLength, 80);
		constraints << QTextLength(QTextLength::FixedLength, 20);
		constraints << QTextLength(QTextLength::FixedLength, 20);
		tableFormat.setColumnWidthConstraints(constraints);
		tableFormat.setCellSpacing(0);
		tableFormat.setCellPadding(1);
		tableFormat.setBorder(0.5);
		//tableFormat.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysBefore);
		//tableFormat.setProperty(QTextFormat::ListFormat, QTextFormat::BlockNonBreakableLines);
		query->exec("SELECT count(*) FROM abonent WHERE Budynok_ID="+budynkyQuery->value(0).toString());
		query->seek(0);
		int numColumns = 9;
		if (needCO)
			++numColumns;
		if (needGV)
			++numColumns;
		QTextTable *table = cursor.insertTable(query->value(0).toInt()+5, numColumns, tableFormat);
		table->mergeCells ( 0, 0, 1, numColumns );
				
				//���������� ����� �������
		//������ �������
		cell = table->cellAt(0, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("���. ") + budynkyQuery->value(1).toString()+" "+budynkyQuery->value(2).toString(), textCharFormat);
		
		//� �/�
		cell = table->cellAt(1, 0);
		cell.setFormat(textCharFormat_italic);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("� �/�"), textCharFormat_italic);
		
		//�������
		cell = table->cellAt(1, 1);
		cell.setFormat(textCharFormat_italic);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("���."), textCharFormat_italic);
		
		//ϲ�
		cell = table->cellAt(1, 2);
		cell.setFormat(textCharFormat_italic);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("� � �"), textCharFormat_italic);
		
		//��������
		cell = table->cellAt(1, 3);
		cell.setFormat(textCharFormat_italic);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("�����."), textCharFormat_italic);
		
		//�����
		cell = table->cellAt(1, 4);
		cell.setFormat(textCharFormat_italic);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("�����"), textCharFormat_italic);
		
		//ʳ������ �����
		cell = table->cellAt(1, 5);
		cell.setFormat(textCharFormat_italic);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("�.���."), textCharFormat_italic);
		
		//����� �����
		cell = table->cellAt(1, 6);
		cell.setFormat(textCharFormat_italic);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("����� �����"), textCharFormat_italic);
		
		//ϳ����
		cell = table->cellAt(1, 7);
		cell.setFormat(textCharFormat_italic);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("ϳ����"), textCharFormat_italic);
		
		//� ���.
		cell = table->cellAt(1, 8);
		cell.setFormat(textCharFormat_italic);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("� ��������"), textCharFormat_italic);
		
		int colNum=8;
		//��������
		if (needCO){
			++colNum;
			cell = table->cellAt(1, colNum);
			cell.setFormat(textCharFormat_italic);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(codec->toUnicode("��"), textCharFormat_italic);
		}
		
		//��������
		if (needGV){
			++colNum;
			cell = table->cellAt(1, colNum);
			cell.setFormat(textCharFormat_italic);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(codec->toUnicode("��"), textCharFormat_italic);
		}
		
		int budIter=2;
		query->exec("SELECT * FROM abonent WHERE Budynok_ID="+budynkyQuery->value(0).toString()+" ORDER BY Kvartyra");
		while (query->next()){
			//� �/�
			cell = table->cellAt(budIter, 0);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QVariant(abonIter).toString(), textCharFormat);
		
			//�������
			cell = table->cellAt(budIter, 1);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(query->value(1).toString(), textCharFormat);
		
			//ϲ�
			cell = table->cellAt(budIter, 2);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(query->value(2).toString()+" "+
								query->value(3).toString()+" "+
								query->value(4).toString(), textCharFormat);
		
			//��������
			cell = table->cellAt(budIter, 3);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(query->value(7).toString(), textCharFormat);
		
			//�����
			cell = table->cellAt(budIter, 4);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			double ploshcha = query->value(8).toDouble();
			cellCursor.insertText(uMToStr2(ploshcha), textCharFormat);
			zagPloshcha += ploshcha;
			if (query->value(17).toBool())
				opalPloshcha += ploshcha;
			else
				vidklPloshcha += ploshcha;
		
			//ʳ������ �����
			cell = table->cellAt(budIter, 5);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			short kilkLudey = query->value(18).toInt();
			cellCursor.insertText(QVariant(kilkLudey).toString(), textCharFormat);
			if (query->value(20).toBool())
				kilkLudOpal+=kilkLudey;
			else
				kilkLudVidkl+=kilkLudey;
		
			//����� �����
			cell = table->cellAt(budIter, 6);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(uMToStr2(query->value(9).toDouble()), textCharFormat);
			
			//ϳ����
			QString pilgyStr="-";
			if (query->value(10).toInt()>0){
				pilgyStr = hash.value(query->value(10).toInt()) 
							+ codec->toUnicode(" �� ")
							+ query->value(11).toString()
							+ codec->toUnicode("���.");
			}
			if (query->value(12).toInt()>0){
				pilgyStr += ", " + hash.value(query->value(12).toInt()) 
							+ codec->toUnicode(" �� ")
							+ query->value(13).toString()
							+ codec->toUnicode("���.");
			}
			cell = table->cellAt(budIter, 7);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(pilgyStr, textCharFormat);
		
			//� ���.
			cell = table->cellAt(budIter, 8);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(query->value(14).toString(), textCharFormat);
			
			colNum = 8;
			//��������
			if (needCO){
				++colNum;
				cell = table->cellAt(budIter, colNum);
				cell.setFormat(textCharFormat);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				if (query->value(17).toBool()){
					cellCursor.insertText("+", textCharFormat);
					kilkOpal++;
				}
				else{
					cellCursor.insertText("-", textCharFormat);
					kilkVidkl++;
				}
			}
			
			//��
			if (needGV){
				++colNum;
				cell = table->cellAt(budIter, colNum);
				cell.setFormat(textCharFormat);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				if (query->value(20).toBool()){
					cellCursor.insertText("+", textCharFormat);
					kilkGV++;
				}
				else{
					cellCursor.insertText("-", textCharFormat);
					kilkVidklGV++;
				}
			}
			
			budIter++;
			abonIter++; 
			postup_form->inc_pos();
		}
		table->mergeCells ( budIter, 0, 1, 3 );
		table->mergeCells ( budIter+1, 0, 1, 3 );
		table->mergeCells ( budIter+2, 0, 1, 3 );
		
			//ϳ������ �� ���������, �� �������� �� ��
		//����� �������
		cell = table->cellAt(budIter, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("������ �� ���������, ���� �� ��������� �������"), textCharFormat);
		
		//ʳ������ �������
		cell = table->cellAt(budIter, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("-", textCharFormat);
		
		//�����
		cell = table->cellAt(budIter, 4);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(vidklPloshcha), textCharFormat);
		
		//ʳ������ �����
		cell = table->cellAt(budIter, 5);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QVariant(kilkLudVidkl).toString(), textCharFormat);
		
		//����� �����
		cell = table->cellAt(budIter, 6);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("-", textCharFormat);
		
		//ϳ����
		cell = table->cellAt(budIter, 7);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("-", textCharFormat);
		
		//� ���.
		cell = table->cellAt(budIter, 8);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("-", textCharFormat);
		
		colNum = 8;
		//��������
		if (needCO){
			++colNum;
			cell = table->cellAt(budIter, colNum);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QVariant(kilkVidkl).toString(), textCharFormat);
		}
		
		//��
		if (needGV){
			++colNum;
			cell = table->cellAt(budIter, colNum);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QVariant(kilkVidklGV).toString(), textCharFormat);
		}
		
			//ϳ������ �� ��������� � ��
		//����� �������
		cell = table->cellAt(budIter+1, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("������ �� ���������, ���� ��������� �������"), textCharFormat);
		
		//ʳ������ �������
		cell = table->cellAt(budIter+1, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("-", textCharFormat);
		
		//�����
		cell = table->cellAt(budIter+1, 4);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(opalPloshcha), textCharFormat);
		
		//ʳ������ �����
		cell = table->cellAt(budIter+1, 5);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QVariant(kilkLudOpal).toString(), textCharFormat);
		
		//����� �����
		cell = table->cellAt(budIter+1, 6);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("-", textCharFormat);
		
		//ϳ����
		cell = table->cellAt(budIter+1, 7);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("-", textCharFormat);
		
		//� ���.
		cell = table->cellAt(budIter+1, 8);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("-", textCharFormat);
		
		colNum = 8;
		//��������
		if (needCO){
			++colNum;
			cell = table->cellAt(budIter+1, colNum);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QVariant(kilkOpal).toString(), textCharFormat);
		}
		
		//GV
		if (needGV){
			++colNum;
			cell = table->cellAt(budIter+1, colNum);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QVariant(kilkGV).toString(), textCharFormat);
		}
		
			//ϳ������ �� �������
		//����� �������
		cell = table->cellAt(budIter+2, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("������ �� �������"), textCharFormat);
		
		//ʳ������ �������
		cell = table->cellAt(budIter+2, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("-", textCharFormat);
		
		//�����
		cell = table->cellAt(budIter+2, 4);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(opalPloshcha+vidklPloshcha), textCharFormat);
		pidsPLOS[0] += opalPloshcha;
		pidsPLOS[1] += vidklPloshcha;
		pidsPLOS[2] += opalPloshcha + vidklPloshcha;
		
		//ʳ������ �����
		cell = table->cellAt(budIter+2, 5);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QVariant(kilkLudOpal+kilkLudVidkl).toString(), textCharFormat);
		pidsLUDY[0] += kilkLudOpal;
		pidsLUDY[1] += kilkLudVidkl;
		pidsLUDY[2] += kilkLudOpal + kilkLudVidkl;
		
		//����� �����
		cell = table->cellAt(budIter+2, 6);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("-", textCharFormat);
		
		//ϳ����
		cell = table->cellAt(budIter+2, 7);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("-", textCharFormat);
		
		//� ���.
		cell = table->cellAt(budIter+2, 8);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("-", textCharFormat);
		
		colNum=8;
		//��������
		if (needCO){
			++colNum;
			cell = table->cellAt(budIter+2, colNum);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QVariant(kilkOpal+kilkVidkl).toString(), textCharFormat);
			pidsCO[0] += kilkOpal;
			pidsCO[1] += kilkVidkl;
			pidsCO[2] += kilkOpal + kilkVidkl;
		}
		
		//GV
		if (needGV){
			++colNum;
			cell = table->cellAt(budIter+2, colNum);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QVariant(kilkGV+kilkVidklGV).toString(), textCharFormat);
			pidsGV[0] += kilkGV;
			pidsGV[1] += kilkVidklGV;
			pidsGV[2] += kilkGV + kilkVidklGV;
		}
	}
	
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	
	blockFormat.setPageBreakPolicy( QTextFormat::PageBreak_AlwaysBefore );
	cursor.insertBlock(blockFormat);
	cursor.insertText( "		ϲ������ �� ϲ���Ȫ�����", textCharFormat);
	blockFormat.setPageBreakPolicy( QTextFormat::PageBreak_Auto );
	cursor.insertBlock(blockFormat);
	if (needCO){
		cursor.insertBlock(blockFormat);
		cursor.insertText("	���������� ��������\n", textCharFormat);
		cursor.insertText("ʳ������ ��������, �� ��������� ������� �� - "+QString::number(pidsCO[0])+"\n", textCharFormat);
		cursor.insertText("ʳ������ ��������, �� �� ��������� ������� �� - "+QString::number(pidsCO[1])+"\n", textCharFormat);
		cursor.insertText("������ �������� - "+QString::number(pidsCO[2])+"\n", textCharFormat);
		
		cursor.insertText("����� ������� ��������, �� ��������� ������� �� - "+QString::number(pidsPLOS[0],'f',4)+"\n", textCharFormat);
		cursor.insertText("����� ������� ��������, �� �� ��������� ������� �� - "+QString::number(pidsPLOS[1],'f',4)+"\n", textCharFormat);
		cursor.insertText("�������� ����� ������� �������� �� ���������� - "+QString::number(pidsPLOS[2],'f',4)+"\n", textCharFormat);
	}
	
	if (needGV){
		cursor.insertBlock(blockFormat);
		cursor.insertText("	������ ����\n", textCharFormat);
		cursor.insertText("ʳ������ ��������, �� ��������� ������� �� - "+QString::number(pidsGV[0])+"\n", textCharFormat);
		cursor.insertText("ʳ������ ��������, �� �� ��������� ������� �� - "+QString::number(pidsGV[1])+"\n", textCharFormat);
		cursor.insertText("������ �������� - "+QString::number(pidsGV[2])+"\n", textCharFormat);
		
		cursor.insertText("ʳ������ ���������� �����, �� ��������� ������� �� - "+QString::number(pidsLUDY[0])+"\n", textCharFormat);
		cursor.insertText("ʳ������ ���������� �����, �� �� ��������� ������� �� - "+QString::number(pidsLUDY[1])+"\n", textCharFormat);
		cursor.insertText("�������� ������� ���������� ����� �� ���������� - "+QString::number(pidsLUDY[2])+"\n", textCharFormat);
	}
	
	//ʳ���� ��������� ��������� ��� �����
	printform->printer()->setDocName("��²���� ������Ҳ�");
	printform->show();
	delete query;
	delete budynkyQuery;
	delete postup_form;
}
//------------------------------------------------------------
void UPrintDocs::print_vidomist_podachi_documentiv_na_pilgy()
{
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT count(*) FROM abonent WHERE Opal=true and Pilga_ID>0 and Pilgy_zdani=false");
	query->seek(0);
	int numRows = query->value(0).toInt();
	UPostupForm *postup_form = new UPostupForm(0,  numRows);
	postup_form->show();
	printform = new UPrintForm(0, codec->toUnicode("³������ ������������� ������� ��������� �� �����"));
	
	QHash<int, QString> pilgyHash;
	query->exec("SELECT id, Kategoriya FROM pilgy");
	while (query->next()){
		pilgyHash.insert(query->value(0).toInt(), query->value(1).toString());
	}
	
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat, textCharFormat_italic;
	//��������� ��������� ������� ��� ��� ������� �� X �����, Y ��
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 13 );
	cursor.insertText(codec->toUnicode("²��̲���\n ������������� ������� ��������� �� �����")+OrganizName(), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 8 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("��������� �� \"�������� ��������\" ") + date.toString("dd.MM.yyyy") + codec->toUnicode("�."), textCharFormat);
	
	textCharFormat.setFontPointSize( 9 );
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	textCharFormat_italic.setFontPointSize( 9 );
	textCharFormat_italic.setFontItalic( true );
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	//��������� � ������������ �������
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
	constraints << QTextLength(QTextLength::FixedLength, 20);
	constraints << QTextLength(QTextLength::FixedLength, 70);
	constraints << QTextLength(QTextLength::FixedLength, 200);
	constraints << QTextLength(QTextLength::FixedLength, 200);
	constraints << QTextLength(QTextLength::FixedLength, 100);
	
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(1);
	tableFormat.setBorder(0.5);
	QTextTable *table = cursor.insertTable(numRows+1, 5, tableFormat);
		//���������� ����� �������
	cell = table->cellAt(0, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("� �/�"));
	
	cell = table->cellAt(0, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("�������"));
	
	cell = table->cellAt(0, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("� � �"));
	
	cell = table->cellAt(0, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("ϳ����"));
	
	cell = table->cellAt(0, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("���������"));

	
		//���������� ������
	query->exec("SELECT Rahunok, Prizv, Imya, Batk, \
						Pilga_ID, Pilga_for, Pilga_ID_2, Pilga_for_2, Pilgy_zdani \
				FROM abonent \
				WHERE Opal=true and Pilga_ID>0 and Pilgy_zdani=false \
				ORDER BY Pilga_ID, Rahunok");
	for (int row=0; (row<numRows) && query->next(); row++){
		QString str, tStr;
		
		//� �/�
		cell = table->cellAt(row+1, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QString::number(row+1));
		
		//�������
		cell = table->cellAt(row+1, 1);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query->value(0).toString());
		
		//� � �
		str = query->value(1).toString()+" ";
		tStr = query->value(2).toString();
		if (tStr.size()>0)
			str += tStr.at(0)+QString(".");
		tStr = query->value(3).toString();
		if (tStr.size()>0)
			str += tStr.at(0)+QString(".");
		cell = table->cellAt(row+1, 2);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(str);
		
		//ϳ����
		str=pilgyHash.value(query->value(4).toInt())+" �� "+query->value(5).toString();
		if (query->value(6).toInt() > 0)
			str += "; 2)" + pilgyHash.value(query->value(6).toInt())+" �� "+query->value(7).toString();
		cell = table->cellAt(row+1, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(str);
		
		//���������
		cell = table->cellAt(row+1, 4);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		if (query->value(8).toBool())
			cellCursor.insertText(codec->toUnicode("�����"));
		
		postup_form->inc_pos();
	}
		//ʳ���� ��������� ��������� ��� �����
	printform->printer()->setDocName("³������ ������� ��������� �� �����");
	printform->show();
	delete query;
	delete postup_form;
}
//------------------------------------------------------------
void UPrintDocs::print_zvit_forma_n_2_pilga_2007(int year, int month)
{
	QSqlQuery query, pilgyQuery, query_2;
	
	query.exec("SELECT strTemplate FROM slujb_znach WHERE id=13");
	query.seek(0);
	QString coPilKod = query.value(0).toString();
	query.exec("SELECT strTemplate FROM slujb_znach WHERE id=14");
	query.seek(0);
	QString gvPilKod = query.value(0).toString();
	
	query.exec("SELECT count(*) FROM pilgy");
	query.next();
	UPostupForm *postup_form = new UPostupForm(0, query.value(0).toInt() );
	postup_form->show();
	
	short diuchTaryfPrecision = tochnistDiuchogoTaryfu();
	
	//������� ����������� ����� �������� ��� ������ ���� ����� ������� �����
	bool needPrintPidpysyAfterAllKat;
	query.exec("SELECT strTemplate FROM slujb_znach WHERE id=7");
	query.seek(0);
	needPrintPidpysyAfterAllKat = query.value(0).toBool();
	
	//������� �������
	UNasTaryfInfo taryfInfo = nasTaryf(year, month);
	QHash<short, double> tarybBudCO;
	query.exec("SELECT Budynok_ID, Taryf FROM diuch_taryf WHERE year="+QString::number(year)+" and month="+QString::number(month));
	while (query.next()){
		tarybBudCO.insert(query.value(0).toInt(), query.value(1).toDouble());
	}
	
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat, textCharFormat_italic, textCharFormat_bold, cur_textCharFormat;
	
	textCharFormat_italic.setFontPointSize( 9 );
	textCharFormat_italic.setFontItalic( true );
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	QFont font = textCharFormat_bold.font();
	font.setBold(true);
	textCharFormat_bold.setFont(font);
	textCharFormat_italic.setFontPointSize( 9 );
	//��������� ��������� ������� ��� ��� ������� �� X �����, Y ��
	printform = new UPrintForm("����� � 2-����� �� "+monthlist.at(month-1)+" "+QString::number(year)+"�.", 10, 10, 10, 10);
	printform->document()->clear();
	printform->printer()->setOrientation( QPrinter::Landscape );
	printform->printer()->setDocName("�²� - ����� �2-����� �� "+monthlist.at(month-1)+" "+QString::number(year)+"�.");
	QTextCursor cursor(printform->document()), cellCursor;
		
	//��������� �������
	pilgyQuery.exec("SELECT id, Kategoriya, Vidsotok, Grupy FROM pilgy ORDER BY Kategoriya");
	int curPilgyGrupy;
	double curVidsotokPilgy;
	int curKategoriyaPilgy;
	int iter;
	int numRowsForPilgovyky;
	QSqlQuery pilgovykyQuery;
	int curGrupaPilgovyka;
	QTextTable *table=NULL;
	//��������� ������ �������
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
	constraints << QTextLength(QTextLength::FixedLength, 30);
	constraints << QTextLength(QTextLength::FixedLength, 200);
	constraints << QTextLength(QTextLength::FixedLength, 130);
	constraints << QTextLength(QTextLength::FixedLength, 80);
	constraints << QTextLength(QTextLength::FixedLength, 30);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 80);
	constraints << QTextLength(QTextLength::FixedLength, 80);
	constraints << QTextLength(QTextLength::FixedLength, 80);
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(1);
	tableFormat.setBorder(0.5);
	if (!needPrintPidpysyAfterAllKat){
		tableFormat.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysAfter);
	}
	bool firstHeader=true;
	while (pilgyQuery.next()){
		curKategoriyaPilgy = pilgyQuery.value(0).toInt();
		curPilgyGrupy = pilgyQuery.value(3).toInt();
		curVidsotokPilgy = pilgyQuery.value(2).toDouble();
		
		//������ ������� ������ ����������� ���� � ��������� ����� ��� ������� �����
		query_2.exec("SELECT count(*) FROM t_abonent_inf_2 \
					RIGHT JOIN ( \
							SELECT Rahunok, sum(Pilga_sum) AS pilgaCoSum, unPilgaFor, Pilga_plos, \
									sum(Pilga_sum_GV) AS pilgaGvSum, Obsyag_GV, Year_pilg_pererah, \
									Month_pilg_pererah, PochDayCO, KincDayCO, PochDayGV, KincDayGV, Status, Is_Lich_GV \
							FROM \
								(SELECT Rahunok, Pilga_sum, Pilga_for AS unPilgaFor, Pilga_plos, 0 AS Pilga_sum_GV, \
										Obsyag_GV, Year_pilg_pererah, Month_pilg_pererah, PochDayCO, KincDayCO, \
										PochDayGV, KincDayGV, Status, Is_Lich_GV \
								FROM narahuvannya \
								WHERE year="+QString::number(year)+" \
									and month="+QString::number(month)+" \
									and (Pilga_sum>0.009 or Pilga_sum<-0.009) \
									and Pilga_ID="+QString::number(curKategoriyaPilgy)+" \
								UNION ALL \
								SELECT Rahunok, 0 AS Pilga_sum, Pilga_for_GV AS unPilgaFor, Pilga_plos, Pilga_sum_GV, \
										Obsyag_GV, Year_pilg_pererah, Month_pilg_pererah, PochDayCO, KincDayCO, \
										PochDayGV, KincDayGV, Status, Is_Lich_GV \
								FROM narahuvannya \
								WHERE year="+QString::number(year)+" \
									and month="+QString::number(month)+" \
									and (Pilga_sum_GV>0.009 or Pilga_sum_GV<-0.009) \
									and Pilga_ID_GV="+QString::number(curKategoriyaPilgy)+") \
							GROUP BY Rahunok, unPilgaFor, Pilga_plos, Obsyag_GV, Year_pilg_pererah, Month_pilg_pererah, \
							PochDayCO, KincDayCO, PochDayGV, KincDayGV, Status, Is_Lich_GV \
					) AS unNarahGR \
					ON  t_abonent_inf_2.id = unNarahGR.Rahunok");
		query_2.seek(0);
		numRowsForPilgovyky = query_2.value(0).toInt();
		if (numRowsForPilgovyky == 0)
			goto nextKategoriya;
		
		if (needPrintPidpysyAfterAllKat || firstHeader){
			blockFormat.setAlignment( Qt::AlignRight );
			if (firstHeader){
				cursor.setBlockFormat( blockFormat );
			}
			else{
				cursor.insertBlock(blockFormat);
			}
			firstHeader = false;
			
			textCharFormat.setFontPointSize( 8 );
            cursor.insertText("���������� �� \"�������� ��������\" (������� ������� \"�������\". ���: www.abonpro.com). ��� ����������: "+QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy")+"�." , textCharFormat);
			cursor.insertBlock(blockFormat);
			cursor.insertBlock(blockFormat);
			//�����������
			textCharFormat.setFontPointSize( 9 );
			cursor.insertText("�����������", textCharFormat); 
			cursor.insertBlock(blockFormat);
			cursor.insertText("����� ̳��������� �����", textCharFormat);
			cursor.insertBlock(blockFormat);
			cursor.insertText("�� ��������� ������� ������ ", textCharFormat);
			cursor.insertBlock(blockFormat);
			cursor.insertText("04.10.2007  N 535", textCharFormat);
			cursor.insertBlock(blockFormat);
			cursor.insertBlock(blockFormat);
			cursor.insertText("����� N 2-�����", textCharFormat);
			blockFormat.setAlignment( Qt::AlignLeft );
			cursor.insertBlock(blockFormat);
			textCharFormat_italic.setFontPointSize( 7 );
			cursor.insertText("������������ ����������� (�����������), �� ���� ������� ", textCharFormat_italic);
			blockFormat.setAlignment( Qt::AlignCenter );
			cursor.insertBlock(blockFormat);
			textCharFormat_bold.setFontPointSize( 11 );
			cursor.insertText("ϳ���������: "+OrganizName(), textCharFormat_bold);
			blockFormat.setAlignment( Qt::AlignCenter );
			cursor.insertBlock(blockFormat);
			cursor.insertBlock(blockFormat);
			textCharFormat_bold.setFontPointSize( 12 );
			cursor.insertText("� � � � � � � � � �", textCharFormat_bold);
			cursor.insertBlock(blockFormat);
			cursor.insertText("������� �� ������������ ������, ���'������ � �������� ����", textCharFormat_bold);
			cursor.insertBlock(blockFormat);
			cursor.insertText("�� " + monthlist.at(month-1) + " " + QString::number(year), textCharFormat_bold);
		
			cursor.movePosition(QTextCursor::End);
			cursor.insertBlock(blockFormat);
			cursor.insertBlock(blockFormat);
		}
		
		textCharFormat.setFontPointSize( 10 );
		textCharFormat_bold.setFontPointSize( 8 );
		textCharFormat_italic.setFontPointSize( 10 );
		textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
		textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
		textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignMiddle);
		
		table = cursor.insertTable(4+numRowsForPilgovyky*3, 13, tableFormat);
			//���������� ����� �������
		table->mergeCells(0,0,1,13);
		cell = table->cellAt(0, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("ϳ�������� ������� - "+pilgyQuery.value(1).toString()+" ("+pilgyQuery.value(0).toString()+")", textCharFormat_bold);
		
		table->mergeCells(1,0,2,1);
		cell = table->cellAt(1, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("� �/�");
		
		table->mergeCells(1,1,1,6);
		cell = table->cellAt(1, 1);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("��� ��� ���������");
		
		table->mergeCells(1,7,2,1);
		cell = table->cellAt(1, 7);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("г�, �� ���� ���-������ ������-�����");
		
		table->mergeCells(1,8,2,1);
		cell = table->cellAt(1, 8);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("̳����, �� ���� ���-������ ������-�����");
		
		table->mergeCells(1,9,2,1);
		cell = table->cellAt(1, 9);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("�-��� ���, �� �� ���-������ ������-�����");
		
		table->mergeCells(1,10,1,2);
		cell = table->cellAt(1, 10);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("���� ������");
		
		table->mergeCells(1,12,2,1);
		cell = table->cellAt(1, 12);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("�����, ���");
		
		cell = table->cellAt(2, 1);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("�������, ��'�, �� �������");
		
		cell = table->cellAt(2, 2);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("������");
	
		cell = table->cellAt(2, 3);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("������-���������� �����");
	
		cell = table->cellAt(2, 4);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("ʳ���. ���");
		
		cell = table->cellAt(2, 5);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("��� ����-��� / ����� �����");
	
		cell = table->cellAt(2, 6);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("� ��������� �������");
		
		cell = table->cellAt(2, 10);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("������-������� �����-�����-�����\n"+coPilKod);
		
		cell = table->cellAt(2, 11);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("������-������� �����-����� ������ ����\n"+gvPilKod);
		
		query_2.exec("SELECT unNarahGR.Rahunok, t_abonent_inf_2.abon_NPR, t_abonent_inf_2.abon_ADR, t_abonent_inf_2.identyf, \
							unNarahGR.pilgaCoSum, unNarahGR.pilgaGvSum, unNarahGR.Status, unNarahGR.Year_pilg_pererah, \
							unNarahGR.Month_pilg_pererah, unNarahGR.unPilgaFor, unNarahGR.Pilga_plos, unNarahGR.Obsyag_GV, \
							unNarahGR.PochDayCO, unNarahGR.KincDayCO, unNarahGR.PochDayGV, unNarahGR.KincDayGV, \
							unNarahGR.Is_Lich_GV \
					FROM t_abonent_inf_2 \
					RIGHT JOIN ( \
							SELECT Rahunok, sum(Pilga_sum) AS pilgaCoSum, unPilgaFor, Pilga_plos, \
									sum(Pilga_sum_GV) AS pilgaGvSum, Obsyag_GV, Year_pilg_pererah, \
									Month_pilg_pererah, PochDayCO, KincDayCO, PochDayGV, KincDayGV, Status, Is_Lich_GV \
							FROM \
								(SELECT Rahunok, Pilga_sum, Pilga_for AS unPilgaFor, Pilga_plos, 0 AS Pilga_sum_GV, \
										Obsyag_GV, Year_pilg_pererah, Month_pilg_pererah, PochDayCO, KincDayCO, \
										PochDayGV, KincDayGV, Status, Is_Lich_GV \
								FROM narahuvannya \
								WHERE year="+QString::number(year)+" \
									and month="+QString::number(month)+" \
									and (Pilga_sum>0.009 or Pilga_sum<-0.009) \
									and Pilga_ID="+QString::number(curKategoriyaPilgy)+" \
								UNION ALL \
								SELECT Rahunok, 0 AS Pilga_sum, Pilga_for_GV AS unPilgaFor, Pilga_plos, Pilga_sum_GV, \
										Obsyag_GV, Year_pilg_pererah, Month_pilg_pererah, PochDayCO, KincDayCO, \
										PochDayGV, KincDayGV, Status, Is_Lich_GV \
								FROM narahuvannya \
								WHERE year="+QString::number(year)+" \
									and month="+QString::number(month)+" \
									and (Pilga_sum_GV>0.009 or Pilga_sum_GV<-0.009) \
									and Pilga_ID_GV="+QString::number(curKategoriyaPilgy)+") \
							GROUP BY Rahunok, unPilgaFor, Pilga_plos, Obsyag_GV, Year_pilg_pererah, Month_pilg_pererah, \
							PochDayCO, KincDayCO, PochDayGV, KincDayGV, Status, Is_Lich_GV \
					) AS unNarahGR \
					ON  t_abonent_inf_2.id = unNarahGR.Rahunok ORDER BY t_abonent_inf_2.abon_NPR");
		qDebug() << query_2.lastQuery();
		iter=0;
		curGrupaPilgovyka = 0;
		double grupPilgaSumCO[4];
		grupPilgaSumCO[0] = 0;
		grupPilgaSumCO[1] = 0;
		grupPilgaSumCO[2] = 0;
		grupPilgaSumCO[3] = 0;
		int grupPilgovykyCountCO[4];
		grupPilgovykyCountCO[0] = 0;
		grupPilgovykyCountCO[1] = 0;
		grupPilgovykyCountCO[2] = 0;
		grupPilgovykyCountCO[3] = 0;
		double grupPilgaSumGV[4];
		grupPilgaSumGV[0] = 0;
		grupPilgaSumGV[1] = 0;
		grupPilgaSumGV[2] = 0;
		grupPilgaSumGV[3] = 0;
		int grupPilgovykyCountGV[4];
		grupPilgovykyCountGV[0] = 0;
		grupPilgovykyCountGV[1] = 0;
		grupPilgovykyCountGV[2] = 0;
		grupPilgovykyCountGV[3] = 0;
		int pilgovykyKatCount;
		pilgovykyKatCount = 0;
		
		textCharFormat.setVerticalAlignment(QTextCharFormat::AlignTop);
		textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignTop);
		textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignTop);
		
		while (query_2.next()){
			if (query_2.value(7).toInt()<1900 || query_2.value(8).toInt()<1 || query_2.value(8).toInt()>12)
				cur_textCharFormat = textCharFormat;
			else
				cur_textCharFormat = textCharFormat_bold;
			// � �/�
			table->mergeCells(3+iter*3,0,3,1);
			cell = table->cellAt(3+iter*3, 0);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( QVariant(iter+1).toString() );
				// ϲ� ���������, � � ��� ��������� - ��������
			short pilgaNumber = query_2.value(6).toInt();
			if (pilgaNumber == 0)
				pilgaNumber = 1;
			else if (pilgaNumber == 1)
				pilgaNumber = 2;
			else if (pilgaNumber == 11)
				pilgaNumber = 1;
			else if (pilgaNumber == 12)
				pilgaNumber = 2;
			pilgovykyQuery.exec("SELECT Prizv, Imya, Batk, Ident, Grupa FROM pilgovyky \
								WHERE Rahunok="+query_2.value(0).toString()+" \
								and Pilga_ID="+QString::number(curKategoriyaPilgy)+" \
								and (PilgaNumber="+sqlStr(pilgaNumber)+" or PilgaNumber=0 or PilgaNumber is null) \
								ORDER BY PilgaNumber DESC, Prizv, Imya, Batk");
			bool pilgovyky_e=false;
			while (pilgovykyQuery.next()){
				table->mergeCells(3+iter*3,1,3,1);
				cell = table->cellAt(3+iter*3, 1);
				cell.setFormat(cur_textCharFormat);
				cellCursor = cell.lastCursorPosition();
				blockFormat.setAlignment( Qt::AlignLeft );
				cellCursor.setBlockFormat( blockFormat );
				if (pilgovyky_e)
					cellCursor.insertText( "\n" );
				cellCursor.insertText( pilgovykyQuery.value(0).toString()+ " " 
										+pilgovykyQuery.value(1).toString()+ " " 
										+pilgovykyQuery.value(2).toString()  );
				// ���������������� ��� ��� �������� ���������
				table->mergeCells(3+iter*3,3,3,1);
				cell = table->cellAt(3+iter*3, 3);
				cell.setFormat(cur_textCharFormat);
				cellCursor = cell.lastCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				if (pilgovyky_e)
					cellCursor.insertText( "\n" );
				cellCursor.insertText( pilgovykyQuery.value(3).toString() );
				
					//�����'���������� ����� ��������� ��� ���������� ������������ ����������� ���������� ��� �����
				curGrupaPilgovyka = pilgovykyQuery.value(4).toInt();
				
				pilgovyky_e=true;
			}
			if ( !pilgovyky_e ){
				table->mergeCells(3+iter*3,1,3,1);
				cell = table->cellAt(3+iter*3, 1);
				cell.setFormat(cur_textCharFormat);
				cellCursor = cell.lastCursorPosition();
				blockFormat.setAlignment( Qt::AlignLeft );
				cellCursor.setBlockFormat( blockFormat );
				cellCursor.insertText( query_2.value(1).toString() );
				// ���������������� ��� ��� ��������� ���������
				table->mergeCells(3+iter*3,3,3,1);
				cell = table->cellAt(3+iter*3, 3);
				cell.setFormat(cur_textCharFormat);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				cellCursor.insertText( query_2.value(3).toString() );
			}
			// ������
			table->mergeCells(3+iter*3,2,3,1);
			cell = table->cellAt(3+iter*3, 2);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( query_2.value(2).toString() );
			// ʳ������ ���, �� ���. �����
			//query.exec("SELECT Pilga_for, Pilga_for_2 FROM abonent WHERE Rahunok="+query_2.value(0).toString());
			//query.seek(0);
			table->mergeCells(3+iter*3,4,3,1);
			cell = table->cellAt(3+iter*3, 4);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			pilgovykyKatCount += query_2.value(9).toInt();
			cellCursor.insertText( query_2.value(9).toString() );

			// ������ �����
			table->mergeCells(3+1+iter*3,5,2,1);
			cell = table->cellAt(3+iter*3, 5);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( pilgyQuery.value(0).toString() );
			cell = table->cellAt(4+iter*3, 5);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( QString::number(pilgyQuery.value(2).toDouble())+"%" );
			
			// �������
			table->mergeCells(3+iter*3,6,3,1);
			cell = table->cellAt(3+iter*3, 6);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( query_2.value(0).toString() );
			
			// г�, �� ���� ��������� �����������
			table->mergeCells(3+iter*3,7,3,1);
			cell = table->cellAt(3+iter*3, 7);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			if (query_2.value(7).toInt() > 1900)
				cellCursor.insertText( query_2.value(7).toString() );
			else
				cellCursor.insertText( QString::number(year) );
			
			// ̳����, �� ���� ��������� �����������
			table->mergeCells(3+iter*3,8,3,1);
			cell = table->cellAt(3+iter*3, 8);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			if (query_2.value(8).toInt() > 0 && query_2.value(8).toInt() < 13)
				cellCursor.insertText( query_2.value(8).toString() );
			else
				cellCursor.insertText( QString::number(month) );
			
			// ʳ������ ���, �� �� ��������� �����������
			table->mergeCells(3+iter*3,9,3,1);
			cell = table->cellAt(3+iter*3, 9);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			int numPilgDays = QDate(year, month, 1).daysInMonth();
			if (d_to_m(query_2.value(4).toDouble()) != 0) //���� � ����������� ����� �� ��
				numPilgDays = abs(query_2.value(13).toInt() - query_2.value(12).toInt() + 1);
			else
				numPilgDays = abs(query_2.value(15).toInt() - query_2.value(14).toInt() + 1);
			//QDate pilgyDateVidr(year, month, 1);
			if (numPilgDays == QDate(year, month, 1).daysInMonth())
				cellCursor.insertText( "-" );
			else
				cellCursor.insertText( QString::number(numPilgDays) );
			
			double curNarahPilgaCO, curNarahPilgaGV;
			// ���� ����� �� ������������ ��������
				//�������� ������������
			cell = table->cellAt(3+iter*3, 10);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			double fuctPlos = query_2.value(10).toDouble();
			if (fuctPlos > 0.0000009)
				cellCursor.insertText( QString::number(fuctPlos).replace(".",",") );
			else
				cellCursor.insertText( "-" );
				//���� �����
			cell = table->cellAt(4+iter*3, 10);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			curNarahPilgaCO = d_to_m(query_2.value(4).toDouble());
			if (curNarahPilgaCO>0.00999 || curNarahPilgaCO<-0.00999){
				cellCursor.insertText( uMToStr2(curNarahPilgaCO) );
				if (curGrupaPilgovyka>3)
					curGrupaPilgovyka = 0;
				grupPilgaSumCO[curGrupaPilgovyka] += curNarahPilgaCO;
				grupPilgovykyCountCO[curGrupaPilgovyka] ++;
			}
			else
				cellCursor.insertText( "-" );
				//�����
			cell = table->cellAt(5+iter*3, 10);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			query.exec("SELECT Budynok_ID FROM abonent WHERE Rahunok="+query_2.value(0).toString());
			query.seek(0);
			double curTaryf = tarybBudCO.value(query.value(0).toInt(),taryfInfo.normat_taryf_kvadr);
			if (curTaryf>0.009 && curNarahPilgaCO>0.009)
				cellCursor.insertText(QString::number(curTaryf,'f',diuchTaryfPrecision).replace('.',','));
			else
				cellCursor.insertText( "-" );
			
			// ���� ����� �� ������� ���
				//�������� ������������
			cell = table->cellAt(3+iter*3, 11);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			double fuctObyem = query_2.value(11).toDouble();
			if (fuctObyem > 0.0000009)
				cellCursor.insertText( QString::number(fuctObyem).replace(".",",") );
			else
				cellCursor.insertText( "-" );
				//���� �����
			cell = table->cellAt(4+iter*3, 11);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			curNarahPilgaGV = query_2.value(5).toDouble();
			curNarahPilgaGV = d_to_m(curNarahPilgaGV);
			if (curNarahPilgaGV>0.00999 || curNarahPilgaGV<-0.00999){
				cellCursor.insertText( uMToStr2(curNarahPilgaGV) );
				if (curGrupaPilgovyka>3)
					curGrupaPilgovyka = 0;
				grupPilgaSumGV[curGrupaPilgovyka] += query_2.value(5).toDouble();
				grupPilgovykyCountGV[curGrupaPilgovyka] ++;
			}
			else
				cellCursor.insertText( "-" );
				//�����
			cell = table->cellAt(5+iter*3, 11);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			if (query_2.value(16).toBool())
				curTaryf = taryfInfo.vart_kuba_GV;
			else
				curTaryf = taryfInfo.vart_na_lud_in_month_GV;
			if (curTaryf>0.009 && curNarahPilgaGV>0.009)
				cellCursor.insertText(uMToStr2(curTaryf));
			else
				cellCursor.insertText( "-" );
			
			// �������� ����
			table->mergeCells(3+iter*3,12,3,1);
			cell = table->cellAt(3+iter*3, 12);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(curNarahPilgaGV+curNarahPilgaCO) );
			
			iter ++;
		}
		
		cur_textCharFormat = textCharFormat;
		iter = iter*3;
		
		//������� ������ ������� (����)
		table->mergeCells ( 3+iter, 0, 1, 4 );
		table->mergeCells ( 3+iter, 4, 1, 3 );
		table->mergeCells ( 3+iter, 7, 1, 3 );
		//������
		cell = table->cellAt(3+iter, 0);
		cell.setFormat(textCharFormat_bold);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( codec->toUnicode("������"));
		
		//ʳ������ ���������
		cell = table->cellAt(3+iter, 4);
		cell.setFormat(textCharFormat_bold);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( codec->toUnicode("ϳ��������: ") + QString::number(pilgovykyKatCount));
		
		//���� ������� ->������� �����
		cell = table->cellAt(3+iter, 7);
		cell.setFormat(textCharFormat_bold);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( codec->toUnicode("���. ����, ���.") );
		//���� �������� c���� ���� �� ��
		cell = table->cellAt(3+iter, 10);
		cell.setFormat(textCharFormat_bold);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( uMToStr2(grupPilgaSumCO[0]+grupPilgaSumCO[1]+grupPilgaSumCO[2]+grupPilgaSumCO[3]));
		//���� �������� c���� ���� �� ��
		cell = table->cellAt(3+iter, 11);
		cell.setFormat(textCharFormat_bold);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( uMToStr2(grupPilgaSumGV[0]+grupPilgaSumGV[1]+grupPilgaSumGV[2]+grupPilgaSumGV[3]));
		//���� �������� ������� �����
		cell = table->cellAt(3+iter, 12);
		cell.setFormat(textCharFormat_bold);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( uMToStr2(grupPilgaSumCO[0]+grupPilgaSumCO[1]+grupPilgaSumCO[2]+grupPilgaSumCO[3]
										 +grupPilgaSumGV[0]+grupPilgaSumGV[1]+grupPilgaSumGV[2]+grupPilgaSumGV[3]));
		//��� ����������� ���������� �� ������
		if (curPilgyGrupy > 0){
			table->insertRows( 4+iter, 4 );
			table->mergeCells ( 4+iter, 0, 1, 10 );
			table->mergeCells ( 5+iter, 0, 1, 10 );
			table->mergeCells ( 6+iter, 0, 1, 10 );
			table->mergeCells ( 7+iter, 0, 1, 10 );
			
			//���� ������� ->����� 1
			cell = table->cellAt(4+iter, 0);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText("����� 1");
			//���� �������� ������� ��������� ��
			cell = table->cellAt(4+iter, 10);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QString::number(grupPilgovykyCountCO[1])+" ���.\n"+ uMToStr2(grupPilgaSumCO[1])+"���." );
			//���� �������� ������� ��������� ��
			cell = table->cellAt(4+iter, 11);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QString::number(grupPilgovykyCountGV[1])+" ���.\n"+ uMToStr2(grupPilgaSumGV[1])+"���." );
			//���� ��������� �������� �� ����
			cell = table->cellAt(4+iter, 12);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QString::number(grupPilgovykyCountCO[1]+grupPilgovykyCountGV[1])+" ���.\n"
									+uMToStr2(grupPilgaSumCO[1]+grupPilgaSumGV[1])+"���." );
			
			//���� ������� ->����� 2
			cell = table->cellAt(5+iter, 0);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("����� 2") );
			//���� �������� ������� ��������� ��
			cell = table->cellAt(5+iter, 10);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QString::number(grupPilgovykyCountCO[2])+" ���.\n"+ uMToStr2(grupPilgaSumCO[2])+"���." );
			//���� �������� ������� ��������� ��
			cell = table->cellAt(5+iter, 11);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QString::number(grupPilgovykyCountGV[2])+" ���.\n"+ uMToStr2(grupPilgaSumGV[2])+"���." );
			//���� ��������� �������� �� ����
			cell = table->cellAt(5+iter, 12);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QString::number(grupPilgovykyCountCO[2]+grupPilgovykyCountGV[2])+" ���.\n"
									+uMToStr2(grupPilgaSumCO[2]+grupPilgaSumGV[2])+"���." );
			
			//���� ������� ->����� 3
			cell = table->cellAt(6+iter, 0);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("����� 3") );
			//���� �������� ������� ��������� ��
			cell = table->cellAt(6+iter, 10);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QString::number(grupPilgovykyCountCO[3])+" ���.\n"+ uMToStr2(grupPilgaSumCO[3])+"���." );
			//���� �������� ������� ��������� ��
			cell = table->cellAt(6+iter, 11);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QString::number(grupPilgovykyCountGV[3])+" ���.\n"+ uMToStr2(grupPilgaSumGV[3])+"���." );
			//���� ��������� �������� �� ����
			cell = table->cellAt(6+iter, 12);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QString::number(grupPilgovykyCountCO[3]+grupPilgovykyCountGV[3])+" ���.\n"
									+uMToStr2(grupPilgaSumCO[3]+grupPilgaSumGV[3])+"���." );
			
			//���� ������� ->��� �����
			cell = table->cellAt(7+iter, 0);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("��� �����") );
			//���� �������� ������� ��������� ��
			cell = table->cellAt(7+iter, 10);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QString::number(grupPilgovykyCountCO[0])+" ���.\n"+ uMToStr2(grupPilgaSumCO[0])+"���." );
			//���� �������� ������� ��������� ��
			cell = table->cellAt(7+iter, 11);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QString::number(grupPilgovykyCountGV[0])+" ���.\n"+ uMToStr2(grupPilgaSumGV[0])+"���." );
			//���� ��������� �������� �� ����
			cell = table->cellAt(7+iter, 12);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QString::number(grupPilgovykyCountCO[0]+grupPilgovykyCountGV[0])+" ���.\n"
									+uMToStr2(grupPilgaSumCO[0]+grupPilgaSumGV[0])+"���." );
			
		}
		
		
		cursor.movePosition(QTextCursor::End);
		if (needPrintPidpysyAfterAllKat){
			//��������� ��������� ��� ������
			textCharFormat.setFontPointSize( 12 );
			cursor.insertText(codec->toUnicode(" "), textCharFormat);
			blockFormat.setAlignment( Qt::AlignLeft );
			cursor.insertBlock(blockFormat);
			cursor.insertBlock(blockFormat);
			cursor.insertBlock(blockFormat);
			cursor.insertText(codec->toUnicode("	�������� ")+OrganizName()+"					"+PrizvIB(1));
			cursor.insertBlock(blockFormat);
			cursor.insertBlock(blockFormat);
			cursor.insertBlock(blockFormat);
			cursor.insertText(codec->toUnicode("    �.�."));
			cursor.insertBlock(blockFormat);
			cursor.insertBlock(blockFormat);
			blockFormat.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysAfter);
			cursor.insertBlock(blockFormat);
			cursor.insertText(codec->toUnicode("	���.���������")+"							 "+PrizvIB(2));
			blockFormat.setPageBreakPolicy(QTextFormat::PageBreak_Auto);
			
		}
		
nextKategoriya:
		postup_form->inc_pos();
	}
	
	if (table != NULL){
		QTextTableFormat lastTableFormat = table->format();
		lastTableFormat.setPageBreakPolicy(QTextFormat::PageBreak_Auto);
		table->setFormat(lastTableFormat);
	}
	
	//ϳ��� �������� ������ �� ������� ���� �������� �� ���� �������
	if (needPrintPidpysyAfterAllKat)
		cursor.setBlockFormat(blockFormat);
	
	if (!needPrintPidpysyAfterAllKat){
		//��������� ��������� ��� ������
		textCharFormat.setFontPointSize( 12 );
		cursor.insertText(codec->toUnicode(" "), textCharFormat);
		cursor.movePosition(QTextCursor::End);
		blockFormat.setAlignment( Qt::AlignLeft );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText(codec->toUnicode("	�������� ")+OrganizName()+"					"+PrizvIB(1));
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText(codec->toUnicode("    �.�."));
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText(codec->toUnicode("	���.���������")+"								 "+PrizvIB(2));
	}

		//ʳ���� ��������� ��������� ��� �����
	delete postup_form;
	printform->show();
}
//------------------------------------------------------------
void UPrintDocs::print_vidnoshennya_pro_vidshkoduvannya_pilg_CO(int year, int month)
{
	QSqlQuery query;
	int rowCount;
	query.exec("SELECT count(*) FROM pilgy");
	query.seek(0);
	rowCount = query.value(0).toInt();
	UPostupForm *postup_form = new UPostupForm(0, rowCount );
	postup_form->show();
	
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat, textCharFormat_italic, textCharFormat_bold, cur_textCharFormat;
	
	textCharFormat_italic.setFontPointSize( 9 );
	textCharFormat_italic.setFontItalic( true );
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	QFont font = textCharFormat_bold.font();
	font.setBold(true);
	textCharFormat_bold.setFont(font);
	textCharFormat_italic.setFontPointSize( 9 );
	//��������� ��������� ������� ��� ��� ������� �� X �����, Y ��
	printform = new UPrintForm("³�������� ��� ������������ ���� �� �� �� "+monthlist.at(month-1)+" "+QString::number(year)+"�.", 10, 10, 10, 10);
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 8 );
    cursor.insertText("���������� �� \"�������� ��������\" (������� ������� \"�������\". ���: www.abonpro.com). ��� ����������: "+QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy")+"�." , textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	//�����������
	textCharFormat.setFontPointSize( 12 );
	cursor.insertText("����������", textCharFormat); 
	cursor.insertBlock(blockFormat);
	cursor.insertText("��������� ����� �� �����������", textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("������� ���������", textCharFormat);
	cursor.insertBlock(blockFormat);
	blockFormat.setAlignment( Qt::AlignJustify );
	cursor.insertBlock(blockFormat);
	textCharFormat_bold.setFontPointSize( 12 );
	cursor.insertText("   "+OrganizName()+" ������� ��� ����������� ��������� ����� �� ������������ ����������� ���������, �� �������� ���������� ��� �������� �� " + monthlist.at(month-1) + " " + QString::number(year)+" ����.", textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	
	//��������� �������
	textCharFormat.setFontPointSize( 10 );
	textCharFormat_bold.setFontPointSize( 9 );
	textCharFormat_italic.setFontPointSize( 10 );
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
		
	//��������� ������ �������
	constraints << QTextLength(QTextLength::FixedLength, 30);
	constraints << QTextLength(QTextLength::FixedLength, 180);
	constraints << QTextLength(QTextLength::FixedLength, 45);
	constraints << QTextLength(QTextLength::FixedLength, 45);
	constraints << QTextLength(QTextLength::FixedLength, 55);
	constraints << QTextLength(QTextLength::FixedLength, 45);
	constraints << QTextLength(QTextLength::FixedLength, 65);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 65);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 65);
	
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(1);
	tableFormat.setBorder(0.5);
	tableFormat.setHeaderRowCount(2);
	QTextTable *table = cursor.insertTable(rowCount+3, 11, tableFormat);
	

	//���������� ����� �������
	table->mergeCells(0,0,2,1);
	table->mergeCells(0,1,2,1);
	table->mergeCells(0,2,2,1);
	table->mergeCells(0,3,2,1);
	table->mergeCells(0,4,2,1);
	table->mergeCells(0,5,2,1);
	table->mergeCells(0,6,2,1);
	
	table->mergeCells(0,7,1,4);
	
	cell = table->cellAt(0, 0);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("� �/�", textCharFormat_italic);
	
	cell = table->cellAt(0, 1);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("ϳ����", textCharFormat_italic);
	
	cell = table->cellAt(0, 2);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("�������� �����", textCharFormat_italic);
	
	cell = table->cellAt(0, 3);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("�-��� �����-����", textCharFormat_italic);
	
	cell = table->cellAt(0, 4);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("����� �����, �.��.", textCharFormat_italic);
	
	cell = table->cellAt(0, 5);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("�-��� ����� ���� �����-����", textCharFormat_italic);
	
	cell = table->cellAt(0, 6);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("���� �����. ����", textCharFormat_italic);
	
	cell = table->cellAt(0, 7);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("� ���� ����", textCharFormat_italic);
	
	cell = table->cellAt(1, 7);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("�-��� ������ ����", textCharFormat_italic);
	
	cell = table->cellAt(1, 8);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("���� ������ ����", textCharFormat_italic);
	
	cell = table->cellAt(1, 9);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("�-��� �������. ����", textCharFormat_italic);
	
	cell = table->cellAt(1, 10);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("���� �������. ����", textCharFormat_italic);
	
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignTop);
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignTop);
	textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignTop);
	
	query.exec("SELECT pilgy.id, pilgy.Kategoriya, narPilgCount.pilgCount, narPilg.forCount, \
						narPilg.pilgaSum, znPilg.pilgCount, znPilg.forCount, znPilg.pilgaSum, \
						donarahPilg.pilgCount, donarahPilg.forCount, donarahPilg.pilgaSum, \
						pilgy.Vyd, narPilg.pilgaPlos \
				FROM (((pilgy \
				LEFT JOIN (SELECT Pilga_ID, count(*) AS pilgCount, \
									sum(Pilga_for) AS forCount, sum(Pilga_sum) AS pilgaSum, \
									sum(Pilga_plos) AS pilgaPlos \
							FROM narahuvannya \
							WHERE ((Pilga_sum>0.009) or (Pilga_sum<-0.009)) \
								and Year="+QString::number(year)+" and Month="+QString::number(month)+" GROUP BY Pilga_ID) \
							AS narPilg ON narPilg.Pilga_ID=pilgy.id) \
				LEFT JOIN (SELECT Pilga_ID, count(*) AS pilgCount, sum(Pilga_for) AS forCount, \
								sum(Pilga_sum) AS pilgaSum \
							FROM narahuvannya \
							WHERE Pilga_sum<-0.009 \
								and Year="+QString::number(year)+" \
								and Month="+QString::number(month)+" \
								and Year_pilg_pererah>2000 and Month_pilg_pererah>0 \
								and Month_pilg_pererah<13 GROUP BY Pilga_ID) \
							AS znPilg ON znPilg.Pilga_ID=pilgy.id) \
				LEFT JOIN (SELECT Pilga_ID, count(*) AS pilgCount, sum(Pilga_for) AS forCount, \
								sum(Pilga_sum) AS pilgaSum \
							FROM narahuvannya \
							WHERE Pilga_sum>0.009 \
								and Year="+QString::number(year)+" \
								and Month="+QString::number(month)+" \
								and Year_pilg_pererah>2000 and Month_pilg_pererah>0 \
								and Month_pilg_pererah<13 GROUP BY Pilga_ID) \
							AS donarahPilg ON donarahPilg.Pilga_ID=pilgy.id) \
				LEFT JOIN (SELECT Pilga_ID, count(*) AS pilgCount \
							FROM narahuvannya \
							WHERE ((Pilga_sum>0.009) or (Pilga_sum<-0.009)) \
									and Year="+QString::number(year)+" \
									and Month="+QString::number(month)+" \
									and (Status=0 or Status=1) \
				GROUP BY Pilga_ID) AS narPilgCount \
				ON narPilgCount.Pilga_ID=pilgy.id \
				ORDER BY pilgy.Vyd, pilgy.Kategoriya");
	int iVal;
	double dVal;
	double pilgaSum=0, znyatoSum=0, donarahSum=0, pilgaPlosSum;
	int pilgovykyCount=0, utrymCount=0, znyatoCount=0, donarahCount=0;
	int iter=2;
	int iterForRows=1;
	//���� ������� ��� ����
	bool notFirstRowInTable=false;
	int oldVyd=0, pilgovykyCountVyd=0, utrymCountVyd=0, znyatoCountVyd=0, donarahCountVyd=0;
	double pilgaSumVyd=0, znyatoSumVyd=0, donarahSumVyd=0, pilgaPlosSumVyd=0;
	
	while (query.next()){
		if (oldVyd != query.value(11).toInt()){
			if (notFirstRowInTable){
				//ϳ�������� ������ �� ����
				//"� �/�"
				table->insertRows(table->rows(), 1);
				table->mergeCells(iter,0,1,3);
				cell = table->cellAt(iter, 0);
				cell.setFormat(textCharFormat);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignRight );
				cellCursor.setBlockFormat( blockFormat );
				cellCursor.insertText("������ �� ����", textCharFormat_bold);

					//"�-��� �����-����"
				cell = table->cellAt(iter, 3);
				cell.setFormat(textCharFormat);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				iVal = pilgovykyCountVyd;
				if (iVal>0)
					cellCursor.insertText( QString::number(iVal), textCharFormat_bold);
				else
					cellCursor.insertText( "-", textCharFormat_bold);
				
					//"����� �����
				cell = table->cellAt(iter, 4);
				cell.setFormat(textCharFormat);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				dVal = pilgaPlosSumVyd;
				if (dVal>0)
					cellCursor.insertText( QString::number(dVal,'f',1).replace('.',','), textCharFormat_bold);
				else
					cellCursor.insertText( "-", textCharFormat_bold);
				
					//"�-��� ����� ���� �����-����"
				cell = table->cellAt(iter, 5);
				cell.setFormat(textCharFormat);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				iVal = utrymCountVyd;
				if (iVal>0)
					cellCursor.insertText( QString::number(iVal), textCharFormat_bold);
				else
					cellCursor.insertText( "-", textCharFormat_bold);
	
					//"���� �����. ����"
				cell = table->cellAt(iter, 6);
				cell.setFormat(textCharFormat);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				dVal = pilgaSumVyd;
				if (dVal>0.009 || dVal<-0.009)
					cellCursor.insertText( uMToStr2(dVal), textCharFormat_bold);
				else
					cellCursor.insertText( "-", textCharFormat_bold);
	
					//"�-��� ������. ����"
				cell = table->cellAt(iter, 7);
				cell.setFormat(textCharFormat);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				iVal = znyatoCountVyd;
				if (iVal>0)
					cellCursor.insertText( QString::number(iVal), textCharFormat_bold);
				else
					cellCursor.insertText( "-", textCharFormat_bold);
	
					//"���� ������. ����"
				cell = table->cellAt(iter, 8);
				cell.setFormat(textCharFormat);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				dVal = znyatoSumVyd;
					if (dVal>0.009 || dVal<-0.009)
					cellCursor.insertText( uMToStr2(dVal), textCharFormat_bold);
				else
					cellCursor.insertText( "-", textCharFormat_bold);
	
				//"�-��� ������������� ����"
				cell = table->cellAt(iter, 9);
				cell.setFormat(textCharFormat);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				iVal = donarahCountVyd;
				if (iVal>0)
					cellCursor.insertText( QString::number(iVal), textCharFormat_bold);
				else
					cellCursor.insertText( "-", textCharFormat_bold);
	
					//"���� ������������� ����"
				cell = table->cellAt(iter, 10);
				cell.setFormat(textCharFormat);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				dVal = donarahSumVyd;
				if (dVal>0.009 || dVal<-0.009)
					cellCursor.insertText( uMToStr2(dVal), textCharFormat_bold);
				else
					cellCursor.insertText( "-", textCharFormat_bold);
				
				pilgovykyCountVyd=0;
				utrymCountVyd=0;
				znyatoCountVyd=0;
				donarahCountVyd=0;
				pilgaSumVyd=0;
				znyatoSumVyd=0;
				donarahSumVyd=0;
				pilgaPlosSum=0;
				pilgaPlosSumVyd=0;
				
				iter++;
			}
			oldVyd = query.value(11).toInt();
			notFirstRowInTable = true;
		}
		
		
		//"� �/�"
		cell = table->cellAt(iter, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QString::number(iterForRows), textCharFormat);
		iterForRows++;
		
		//"����� �����
		cell = table->cellAt(iter, 1);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(1).toString(), textCharFormat);
		
		//"�������� �����"
		cell = table->cellAt(iter, 2);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(0).toString(), textCharFormat);
	
		//"�-��� �����-����"
		cell = table->cellAt(iter, 3);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		iVal = query.value(2).toInt();
		pilgovykyCount += iVal;
		pilgovykyCountVyd += iVal;
		if (iVal>0)
			cellCursor.insertText( QString::number(iVal), textCharFormat);
		else
			cellCursor.insertText( "-", textCharFormat);
		
		//"����� ����
		cell = table->cellAt(iter, 4);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		dVal = uDToM(query.value(12).toDouble());
		pilgaPlosSum += dVal;
		pilgaPlosSumVyd += dVal;
		if (dVal>0.09)
			cellCursor.insertText( QString::number(dVal,'f',1).replace('.',','), textCharFormat);
		else
			cellCursor.insertText( "-", textCharFormat);
		
		//"�-��� ����� ���� �����-����"
		cell = table->cellAt(iter, 5);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		iVal = int(query.value(3).toDouble()+0.2) - query.value(2).toInt();
		utrymCount += iVal;
		utrymCountVyd += iVal;
		if (iVal>0)
			cellCursor.insertText( QString::number(iVal), textCharFormat);
		else
			cellCursor.insertText( "-", textCharFormat);
	
		//"���� �����. ����"
		cell = table->cellAt(iter, 6);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		dVal = query.value(4).toDouble();
		pilgaSum += dVal;
		pilgaSumVyd += dVal;
		if (dVal>0.009 || dVal<-0.009)
			cellCursor.insertText( uMToStr2(dVal), textCharFormat);
		else
			cellCursor.insertText( "-", textCharFormat);
	
		//"�-��� ������. ����"
		cell = table->cellAt(iter, 7);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		iVal = query.value(5).toInt();
		znyatoCount += iVal;
		znyatoCountVyd += iVal;
		if (iVal>0)
			cellCursor.insertText( QString::number(iVal), textCharFormat);
		else
			cellCursor.insertText( "-", textCharFormat);
	
		//"���� ������. ����"
		cell = table->cellAt(iter, 8);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		dVal = query.value(7).toDouble();
		znyatoSum += dVal;
		znyatoSumVyd += dVal;
		if (dVal>0.009 || dVal<-0.009)
			cellCursor.insertText( uMToStr2(dVal), textCharFormat);
		else
			cellCursor.insertText( "-", textCharFormat);
		
		//"�-��� ������������� ����"
		cell = table->cellAt(iter, 9);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		iVal = query.value(8).toInt();
		donarahCount += iVal;
		donarahCountVyd += iVal;
		if (iVal>0)
			cellCursor.insertText( QString::number(iVal), textCharFormat);
		else
			cellCursor.insertText( "-", textCharFormat);
	
		//"���� ������������� ����"
		cell = table->cellAt(iter, 10);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		dVal = query.value(10).toDouble();
		donarahSum += dVal;
		donarahSumVyd += dVal;
		if (dVal>0.009 || dVal<-0.009)
			cellCursor.insertText( uMToStr2(dVal), textCharFormat);
		else
			cellCursor.insertText( "-", textCharFormat);
		
		++iter;
		postup_form->inc_pos();
	}
	
	//ϳ�������� ������
		//"� �/�"
	table->mergeCells(iter,0,1,2);
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("������", textCharFormat_bold);

		//"�-��� �����-����"
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	iVal = pilgovykyCount;
	if (iVal>0)
		cellCursor.insertText( QString::number(iVal), textCharFormat_bold);
	else
		cellCursor.insertText( "-", textCharFormat_bold);
	
		//"����� ����
	cell = table->cellAt(iter, 4);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	dVal = pilgaPlosSumVyd;
	if (dVal>0)
		cellCursor.insertText( QString::number(dVal,'f',1).replace('.',','), textCharFormat_bold);
	else
		cellCursor.insertText( "-", textCharFormat_bold);
	
		//"�-��� ����� ���� �����-����"
	cell = table->cellAt(iter, 5);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	iVal = utrymCount;
	if (iVal>0)
		cellCursor.insertText( QString::number(iVal), textCharFormat_bold);
	else
		cellCursor.insertText( "-", textCharFormat_bold);
	
		//"���� �����. ����"
	cell = table->cellAt(iter, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	dVal = pilgaSum;
	if (dVal>0.009 || dVal<-0.009)
		cellCursor.insertText( uMToStr2(dVal), textCharFormat_bold);
	else
		cellCursor.insertText( "-", textCharFormat_bold);
	
		//"�-��� ������. ����"
	cell = table->cellAt(iter, 7);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	iVal = znyatoCount;
	if (iVal>0)
		cellCursor.insertText( QString::number(iVal), textCharFormat_bold);
	else
		cellCursor.insertText( "-", textCharFormat_bold);
	
		//"���� ������. ����"
	cell = table->cellAt(iter, 8);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	dVal = znyatoSum;
	if (dVal>0.009 || dVal<-0.009)
		cellCursor.insertText( uMToStr2(dVal), textCharFormat_bold);
	else
		cellCursor.insertText( "-", textCharFormat_bold);
	
	//"�-��� ������������� ����"
	cell = table->cellAt(iter, 9);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	iVal = donarahCount;
	if (iVal>0)
		cellCursor.insertText( QString::number(iVal), textCharFormat_bold);
	else
		cellCursor.insertText( "-", textCharFormat_bold);
	
		//"���� ������������� ����"
	cell = table->cellAt(iter, 10);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	dVal = donarahSum;
	if (dVal>0.009 || dVal<-0.009)
		cellCursor.insertText( uMToStr2(dVal), textCharFormat_bold);
	else
		cellCursor.insertText( "-", textCharFormat_bold);
	
	//��������� ��������� ��� ������
	textCharFormat.setFontPointSize( 12 );
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("	��������						"+PrizvIB(1));
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("    �.�.");
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("	���.���������					"+PrizvIB(2));

		//ʳ���� ��������� ��������� ��� �����
	delete postup_form;
	printform->printer()->setOrientation( QPrinter::Portrait );
	printform->printer()->setDocName("³�������� �� ������������ ���� �� �� �� "+monthlist.at(month-1)+" "+QString::number(year)+"�.");
	printform->show();
}
//------------------------------------------------------------
void UPrintDocs::print_vidomist_pererahunku_pilg_CO(int year, int month)
{
	QSqlQuery query, countQuery;
	QSqlQuery pilgovykyQuery;
	countQuery.exec("SELECT count(*) FROM narahuvannya \
				WHERE Pilga_sum<-0.009 \
					and Year="+QString::number(year)+" \
					and Month="+QString::number(month)+" \
					and Year_pilg_pererah>2000 and Month_pilg_pererah>0 and Month_pilg_pererah<13");
	countQuery.seek(0);
	int rowCount = countQuery.value(0).toInt();
	UPostupForm *postup_form = new UPostupForm(0, rowCount );
	postup_form->setPostupLabel("1. ���� ������ ����");
	postup_form->showLable(true);
	postup_form->show();
	
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat, textCharFormat_italic, textCharFormat_bold, cur_textCharFormat;
	
	textCharFormat_italic.setFontPointSize( 9 );
	textCharFormat_italic.setFontItalic( true );
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	QFont font = textCharFormat_bold.font();
	font.setBold(true);
	textCharFormat_bold.setFont(font);
	textCharFormat_italic.setFontPointSize( 9 );
	//��������� ��������� ������� ��� ��� ������� �� X �����, Y ��
	printform = new UPrintForm("³������ ����������� ���� �� �� �� "+monthlist.at(month-1)+" "+QString::number(year)+"�.", 10, 10, 10, 10);
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 8 );
    cursor.insertText("���������� �� \"�������� ��������\" (������� ������� \"�������\". ���: www.abonpro.com). ��� ����������: "+QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy")+"�." , textCharFormat);
	cursor.insertBlock(blockFormat);
	blockFormat.setAlignment( Qt::AlignCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 12 );
	textCharFormat_bold.setFontPointSize( 12 );
	cursor.insertText("²��̲���", textCharFormat_bold); 
	cursor.insertBlock(blockFormat);
	cursor.insertText("����������� ���� �� �� �� "+OrganizName(), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	cursor.insertText("�� " + monthlist.at(month-1) + " " + QString::number(year)+" ����.", textCharFormat);
	cursor.insertBlock(blockFormat);
	
	
	//���� �����
	//��������� �������
	textCharFormat.setFontPointSize( 11 );
	textCharFormat_bold.setFontPointSize( 11 );
	textCharFormat_italic.setFontPointSize( 11 );
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertText("1. ���� �����", textCharFormat_bold);
	cursor.insertBlock(blockFormat);
	
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
		
	//��������� ������ �������
	constraints << QTextLength(QTextLength::FixedLength, 30);
	constraints << QTextLength(QTextLength::FixedLength, 170);
	constraints << QTextLength(QTextLength::FixedLength, 170);
	constraints << QTextLength(QTextLength::FixedLength, 170);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 70);
	
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(1);
	tableFormat.setBorder(0.5);
	QTextTable *table = cursor.insertTable(rowCount+2, 7, tableFormat);
			//���������� ����� �������
	cell = table->cellAt(0, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("� �/�", textCharFormat_italic);
	
	cell = table->cellAt(0, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("�������� �����", textCharFormat_italic);
	
	cell = table->cellAt(0, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("� � �", textCharFormat_italic);
	
	cell = table->cellAt(0, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("������", textCharFormat_italic);
	
	cell = table->cellAt(0, 4);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("г�", textCharFormat_italic);
	
	cell = table->cellAt(0, 5);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("̳����", textCharFormat_italic);
	
	cell = table->cellAt(0, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("���� �����", textCharFormat_italic);
	
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignTop);
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignTop);
	textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignTop);
	
	int iter=1;
	query.exec("SELECT pilgy.Kategoriya, t_abonent_inf_2.abon_NPR, t_abonent_inf_2.abon_ADR, \
						narahuvannya.Year_pilg_pererah, narahuvannya.Month_pilg_pererah, narahuvannya.Pilga_sum, \
						narahuvannya.Rahunok, pilgy.id, narahuvannya.Status \
				FROM (narahuvannya \
				LEFT JOIN t_abonent_inf_2 ON t_abonent_inf_2.id=narahuvannya.Rahunok) \
				LEFT JOIN pilgy ON pilgy.id=narahuvannya.Pilga_id \
				WHERE Pilga_sum<-0.009 \
					and Year="+QString::number(year)+" \
					and Month="+QString::number(month)+" \
					and Year_pilg_pererah>2000 and Month_pilg_pererah>0 and Month_pilg_pererah<13 \
				ORDER BY pilgy.Kategoriya, t_abonent_inf_2.abon_NPR");
	double dVal;
	double pilgSum=0;
	while (query.next()){
		//"� �/�"
		cell = table->cellAt(iter, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QString::number(iter), textCharFormat);
		
		//"�������� �����"
		cell = table->cellAt(iter, 1);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(0).toString(), textCharFormat);
	
		// ϲ� ���������, � � ��� ��������� - ��������
		short pilgaNumber = query.value(8).toInt();
		if (pilgaNumber == 0)
			pilgaNumber = 1;
		else if (pilgaNumber == 1)
			pilgaNumber = 2;
		else if (pilgaNumber == 11)
			pilgaNumber = 1;
		else if (pilgaNumber == 12)
			pilgaNumber = 2;
		pilgovykyQuery.exec("SELECT Prizv, Imya, Batk, Ident, Grupa FROM pilgovyky \
							WHERE Rahunok="+query.value(6).toString()+" \
							and Pilga_ID="+query.value(7).toString()+" \
							and (PilgaNumber="+sqlStr(pilgaNumber)+" or PilgaNumber=0 or PilgaNumber is null) \
							ORDER BY PilgaNumber DESC, Prizv, Imya, Batk");
		bool pilgovyky_e=false;
		if (pilgovykyQuery.next()){
			cell = table->cellAt(iter, 2);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( pilgovykyQuery.value(0).toString()+ " " 
									+pilgovykyQuery.value(1).toString()+ " " 
									+pilgovykyQuery.value(2).toString()  );
			pilgovyky_e=true;
		}
		else{
			//"ϲ�"
			cell = table->cellAt(iter, 2);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(query.value(1).toString(), textCharFormat);
		}
	
		//������
		cell = table->cellAt(iter, 3);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(2).toString(), textCharFormat);
	
		//г�
		cell = table->cellAt(iter, 4);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(3).toString(), textCharFormat);
	
		//̳����
		cell = table->cellAt(iter, 5);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(4).toString(), textCharFormat);
	
		//"����
		cell = table->cellAt(iter, 6);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		dVal = query.value(5).toDouble();
		pilgSum += dVal;
		cellCursor.insertText( uMToStr2(dVal), textCharFormat);

		
		++iter;
		postup_form->inc_pos();
	}
	//ϳ�������� ������
	table->mergeCells(iter,0,1,6);
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText( "������", textCharFormat);
	
	cell = table->cellAt(iter, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText( uMToStr2(pilgSum), textCharFormat);
	
	//����������� �����
	countQuery.exec("SELECT count(*) FROM narahuvannya \
				WHERE Pilga_sum>0.009 \
					and Year="+QString::number(year)+" \
					and Month="+QString::number(month)+" \
					and Year_pilg_pererah>2000 and Month_pilg_pererah>0 and Month_pilg_pererah<13");
	countQuery.seek(0);
	rowCount = countQuery.value(0).toInt();
	postup_form->setCurrentPos(0);
	postup_form->setNumPos(rowCount);
	postup_form->setPostupLabel("2. ���� ������������� ����");
	
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("2. ����������� �����", textCharFormat_bold);
	cursor.insertBlock(blockFormat);
	//��������� �������
	table = cursor.insertTable(rowCount+2, 7, tableFormat);
			//���������� ����� �������
	cell = table->cellAt(0, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("� �/�", textCharFormat_italic);
	
	cell = table->cellAt(0, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("�������� �����", textCharFormat_italic);
	
	cell = table->cellAt(0, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("� � �", textCharFormat_italic);
	
	cell = table->cellAt(0, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("������", textCharFormat_italic);
	
	cell = table->cellAt(0, 4);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("г�", textCharFormat_italic);
	
	cell = table->cellAt(0, 5);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("̳����", textCharFormat_italic);
	
	cell = table->cellAt(0, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("���� �����", textCharFormat_italic);
	
	query.exec("SELECT pilgy.Kategoriya, t_abonent_inf_2.abon_NPR, t_abonent_inf_2.abon_ADR, \
						narahuvannya.Year_pilg_pererah, narahuvannya.Month_pilg_pererah, narahuvannya.Pilga_sum, \
						narahuvannya.Rahunok, pilgy.id, narahuvannya.Status \
				FROM (narahuvannya \
				LEFT JOIN t_abonent_inf_2 ON t_abonent_inf_2.id=narahuvannya.Rahunok) \
				LEFT JOIN pilgy ON pilgy.id=narahuvannya.Pilga_id \
				WHERE Pilga_sum>0.009 \
					and Year="+QString::number(year)+" \
					and Month="+QString::number(month)+" \
					and Year_pilg_pererah>2000 and Month_pilg_pererah>0 and Month_pilg_pererah<13 \
				ORDER BY pilgy.Kategoriya, t_abonent_inf_2.abon_NPR");
	pilgSum = 0;
	iter=1;
	while (query.next()){
		//"� �/�"
		cell = table->cellAt(iter, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QString::number(iter), textCharFormat);
		
		//"�������� �����"
		cell = table->cellAt(iter, 1);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(0).toString(), textCharFormat);
		
		
		// ϲ� ���������, � � ��� ��������� - ��������
		short pilgaNumber = query.value(8).toInt();
		if (pilgaNumber == 0)
			pilgaNumber = 1;
		else if (pilgaNumber == 1)
			pilgaNumber = 2;
		else if (pilgaNumber == 11)
			pilgaNumber = 1;
		else if (pilgaNumber == 12)
			pilgaNumber = 2;
		pilgovykyQuery.exec("SELECT Prizv, Imya, Batk, Ident, Grupa FROM pilgovyky \
							WHERE Rahunok="+query.value(6).toString()+" \
							and Pilga_ID="+query.value(7).toString()+" \
							and (PilgaNumber="+sqlStr(pilgaNumber)+" or PilgaNumber=0 or PilgaNumber is null) \
							ORDER BY PilgaNumber DESC, Prizv, Imya, Batk");
		bool pilgovyky_e=false;
		if (pilgovykyQuery.next()){
			cell = table->cellAt(iter, 2);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( pilgovykyQuery.value(0).toString()+ " " 
									+pilgovykyQuery.value(1).toString()+ " " 
									+pilgovykyQuery.value(2).toString()  );
			pilgovyky_e=true;
		}
		else{
			//"ϲ�"
			cell = table->cellAt(iter, 2);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(query.value(1).toString(), textCharFormat);
		}
	
		//������
		cell = table->cellAt(iter, 3);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(2).toString(), textCharFormat);
	
		//г�
		cell = table->cellAt(iter, 4);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(3).toString(), textCharFormat);
	
		//̳����
		cell = table->cellAt(iter, 5);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(4).toString(), textCharFormat);
	
		//"����
		cell = table->cellAt(iter, 6);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		dVal = query.value(5).toDouble();
		pilgSum += dVal;
		cellCursor.insertText( uMToStr2(dVal), textCharFormat);

		
		++iter;
		postup_form->inc_pos();
	}
	//ϳ�������� ������
	table->mergeCells(iter,0,1,6);
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText( "������", textCharFormat);
	
	cell = table->cellAt(iter, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText( uMToStr2(pilgSum), textCharFormat);
	
	//��������� ��������� ��� ������
	textCharFormat.setFontPointSize( 12 );
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("	��������						"+PrizvIB(1));
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("    �.�.");
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("	���.���������					"+PrizvIB(2));

		//ʳ���� ��������� ��������� ��� �����
	delete postup_form;
	printform->printer()->setOrientation( QPrinter::Portrait );
	printform->printer()->setDocName("³������ ����������� ���� �� �� �� "+monthlist.at(month-1)+" "+QString::number(year)+"�.");
	printform->show();
}
//------------------------------------------------------------
void UPrintDocs::print_vidnoshennya_pro_vidshkoduvannya_pilg_GV(int year, int month)
{
	QSqlQuery query;
	int rowCount;
	query.exec("SELECT count(*) FROM pilgy");
	query.seek(0);
	rowCount = query.value(0).toInt();
	UPostupForm *postup_form = new UPostupForm(0, rowCount );
	postup_form->show();
	
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat, textCharFormat_italic, textCharFormat_bold, cur_textCharFormat;
	
	textCharFormat_italic.setFontPointSize( 9 );
	textCharFormat_italic.setFontItalic( true );
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	QFont font = textCharFormat_bold.font();
	font.setBold(true);
	textCharFormat_bold.setFont(font);
	textCharFormat_italic.setFontPointSize( 9 );
	//��������� ��������� ������� ��� ��� ������� �� X �����, Y ��
	printform = new UPrintForm("³�������� ��� ������������ ���� �� �� �� "+monthlist.at(month-1)+" "+QString::number(year)+"�.", 10, 10, 10, 10);
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 8 );
    cursor.insertText("���������� �� \"�������� ��������\" (������� ������� \"�������\". ���: www.abonpro.com). ��� ����������: "+QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy")+"�." , textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	//�����������
	textCharFormat.setFontPointSize( 12 );
	cursor.insertText("����������", textCharFormat); 
	cursor.insertBlock(blockFormat);
	cursor.insertText("��������� ����� �� �����������", textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("������� ���������", textCharFormat);
	cursor.insertBlock(blockFormat);
	blockFormat.setAlignment( Qt::AlignJustify );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	textCharFormat_bold.setFontPointSize( 12 );
	cursor.insertText("   "+OrganizName()+" ������� ��� ����������� ��������� ����� �� ������������ ������� �����, �� �������� ���������� ��� �������� �� " + monthlist.at(month-1) + " " + QString::number(year)+" ����.", textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	
	//��������� �������
	textCharFormat.setFontPointSize( 11 );
	textCharFormat_bold.setFontPointSize( 10 );
	textCharFormat_italic.setFontPointSize( 11 );
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
		
	//��������� ������ �������
	constraints << QTextLength(QTextLength::FixedLength, 30);
	constraints << QTextLength(QTextLength::FixedLength, 250);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 70);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 70);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 70);
	
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(1);
	tableFormat.setBorder(0.5);
	tableFormat.setHeaderRowCount(2);
	QTextTable *table = cursor.insertTable(rowCount+2, 9, tableFormat);
			//���������� ����� �������
	cell = table->cellAt(0, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("� �/�", textCharFormat_italic);
	
	cell = table->cellAt(0, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("�������� �����", textCharFormat_italic);
	
	cell = table->cellAt(0, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("�-��� �����-����", textCharFormat_italic);
	
	cell = table->cellAt(0, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("�-��� ����� ���� �����-����", textCharFormat_italic);
	
	cell = table->cellAt(0, 4);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("���� �����. ����", textCharFormat_italic);
	
	cell = table->cellAt(0, 5);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("�-��� ������. ����", textCharFormat_italic);
	
	cell = table->cellAt(0, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("���� ������. ����", textCharFormat_italic);
	
	cell = table->cellAt(0, 7);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("�-��� �������. ����", textCharFormat_italic);
	
	cell = table->cellAt(0, 8);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("���� �������. ����", textCharFormat_italic);
	
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignTop);
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignTop);
	textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignTop);
	
	int iter=1;
	query.exec("SELECT pilgy.id, pilgy.Kategoriya, narPilgCount.pilgCount, narPilg.forCount, \
						narPilg.pilgaSum, znPilg.pilgCount, znPilg.forCount, znPilg.pilgaSum, \
						donarahPilg.pilgCount, donarahPilg.forCount, donarahPilg.pilgaSum \
				FROM (((pilgy \
				LEFT JOIN (SELECT Pilga_ID_GV, count(*) AS pilgCount, \
									sum(Pilga_for_GV) AS forCount, sum(Pilga_sum_GV) AS pilgaSum \
							FROM narahuvannya \
							WHERE ((Pilga_sum_GV>0.009) or (Pilga_sum_GV<-0.009)) \
								and Year="+QString::number(year)+" and Month="+QString::number(month)+" GROUP BY Pilga_ID_GV) \
							AS narPilg ON narPilg.Pilga_ID_GV=pilgy.id) \
				LEFT JOIN (SELECT Pilga_ID_GV, count(*) AS pilgCount, sum(Pilga_for_GV) AS forCount, \
								sum(Pilga_sum_GV) AS pilgaSum \
							FROM narahuvannya \
							WHERE Pilga_sum_GV<-0.009 \
								and Year="+QString::number(year)+" \
								and Month="+QString::number(month)+" \
								and Year_pilg_pererah>2000 and Month_pilg_pererah>0 \
								and Month_pilg_pererah<13 GROUP BY Pilga_ID_GV) \
							AS znPilg ON znPilg.Pilga_ID_GV=pilgy.id) \
				LEFT JOIN (SELECT Pilga_ID_GV, count(*) AS pilgCount, sum(Pilga_for_GV) AS forCount, \
								sum(Pilga_sum_GV) AS pilgaSum \
							FROM narahuvannya \
							WHERE Pilga_sum_GV>0.009 \
								and Year="+QString::number(year)+" \
								and Month="+QString::number(month)+" \
								and Year_pilg_pererah>2000 and Month_pilg_pererah>0 \
								and Month_pilg_pererah<13 GROUP BY Pilga_ID_GV) \
							AS donarahPilg ON donarahPilg.Pilga_ID_GV=pilgy.id) \
				LEFT JOIN (SELECT Pilga_ID_GV, count(*) AS pilgCount \
							FROM narahuvannya \
							WHERE ((Pilga_sum_GV>0.009) or (Pilga_sum_GV<-0.009)) \
									and Year="+QString::number(year)+" \
									and Month="+QString::number(month)+" \
									and (Status=0 or Status=1) \
							GROUP BY Pilga_ID_GV) \
							AS narPilgCount ON narPilgCount.Pilga_ID_GV=pilgy.id \
				ORDER BY pilgy.Kategoriya");
	int iVal;
	double dVal;
	double pilgaSum=0, znyatoSum=0, donarahSum=0;
	int pilgovykyCount=0, utrymCount=0, znyatoCount=0, donarahCount=0;
	while (query.next()){
		//"� �/�"
		cell = table->cellAt(iter, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QString::number(iter), textCharFormat);
		
		//"�������� �����"
		cell = table->cellAt(iter, 1);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(1).toString(), textCharFormat);
	
		//"�-��� �����-����"
		cell = table->cellAt(iter, 2);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		iVal = query.value(2).toInt();
		pilgovykyCount += iVal;
		if (iVal>0)
			cellCursor.insertText( QString::number(iVal), textCharFormat);
		else
			cellCursor.insertText( "-", textCharFormat);
	
		//"�-��� ����� ���� �����-����"
		cell = table->cellAt(iter, 3);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		iVal = int(query.value(3).toDouble()+0.2) - query.value(2).toInt();
		utrymCount += iVal;
		if (iVal>0)
			cellCursor.insertText( QString::number(iVal), textCharFormat);
		else
			cellCursor.insertText( "-", textCharFormat);
	
		//"���� �����. ����"
		cell = table->cellAt(iter, 4);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		dVal = query.value(4).toDouble();
		pilgaSum += dVal;
		if (dVal>0.009 || dVal<-0.009)
			cellCursor.insertText( uMToStr2(dVal), textCharFormat);
		else
			cellCursor.insertText( "-", textCharFormat);
	
		//"�-��� ������. ����"
		cell = table->cellAt(iter, 5);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		iVal = query.value(5).toInt();
		znyatoCount += iVal;
		if (iVal>0)
			cellCursor.insertText( QString::number(iVal), textCharFormat);
		else
			cellCursor.insertText( "-", textCharFormat);
	
		//"���� ������. ����"
		cell = table->cellAt(iter, 6);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		dVal = query.value(7).toDouble();
		znyatoSum += dVal;
		if (dVal>0.009 || dVal<-0.009)
			cellCursor.insertText( uMToStr2(dVal), textCharFormat);
		else
			cellCursor.insertText( "-", textCharFormat);
		
		//"�-��� ������������� ����"
		cell = table->cellAt(iter, 7);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		iVal = query.value(8).toInt();
		donarahCount += iVal;
		if (iVal>0)
			cellCursor.insertText( QString::number(iVal), textCharFormat);
		else
			cellCursor.insertText( "-", textCharFormat);
	
		//"���� ������������� ����"
		cell = table->cellAt(iter, 8);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		dVal = query.value(10).toDouble();
		donarahSum += dVal;
		if (dVal>0.009 || dVal<-0.009)
			cellCursor.insertText( uMToStr2(dVal), textCharFormat);
		else
			cellCursor.insertText( "-", textCharFormat);
		
		++iter;
		postup_form->inc_pos();
	}
	
	//ϳ�������� ������
		//"� �/�"
	table->mergeCells(iter,0,1,2);
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("������", textCharFormat);

		//"�-��� �����-����"
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	iVal = pilgovykyCount;
	if (iVal>0)
		cellCursor.insertText( QString::number(iVal), textCharFormat);
	else
		cellCursor.insertText( "-", textCharFormat);
	
		//"�-��� ����� ���� �����-����"
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	iVal = utrymCount;
	if (iVal>0)
		cellCursor.insertText( QString::number(iVal), textCharFormat);
	else
		cellCursor.insertText( "-", textCharFormat);
	
		//"���� �����. ����"
	cell = table->cellAt(iter, 4);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	dVal = pilgaSum;
	if (dVal>0.009 || dVal<-0.009)
		cellCursor.insertText( uMToStr2(dVal), textCharFormat);
	else
		cellCursor.insertText( "-", textCharFormat);
	
		//"�-��� ������. ����"
	cell = table->cellAt(iter, 5);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	iVal = znyatoCount;
	if (iVal>0)
		cellCursor.insertText( QString::number(iVal), textCharFormat);
	else
		cellCursor.insertText( "-", textCharFormat);
	
		//"���� ������. ����"
	cell = table->cellAt(iter, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	dVal = znyatoSum;
	if (dVal>0.009 || dVal<-0.009)
		cellCursor.insertText( uMToStr2(dVal), textCharFormat);
	else
		cellCursor.insertText( "-", textCharFormat);
	
	//"�-��� ������������� ����"
	cell = table->cellAt(iter, 7);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	iVal = donarahCount;
	if (iVal>0)
		cellCursor.insertText( QString::number(iVal), textCharFormat);
	else
		cellCursor.insertText( "-", textCharFormat);
	
		//"���� ������������� ����"
	cell = table->cellAt(iter, 8);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	dVal = donarahSum;
	if (dVal>0.009 || dVal<-0.009)
		cellCursor.insertText( uMToStr2(dVal), textCharFormat);
	else
		cellCursor.insertText( "-", textCharFormat);
	
	//��������� ��������� ��� ������
	textCharFormat.setFontPointSize( 12 );
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("	��������						"+PrizvIB(1));
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("    �.�.");
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("	���.���������					"+PrizvIB(2));

		//ʳ���� ��������� ��������� ��� �����
	delete postup_form;
	printform->printer()->setOrientation( QPrinter::Portrait );
	printform->printer()->setDocName("³�������� �� ������������ ���� �� �� �� "+monthlist.at(month-1)+" "+QString::number(year)+"�.");
	printform->show();
}
//------------------------------------------------------------
void UPrintDocs::print_vidomist_pererahunku_pilg_GV(int year, int month)
{
	QSqlQuery query, countQuery;
	QSqlQuery pilgovykyQuery;
	countQuery.exec("SELECT count(*) FROM narahuvannya \
				WHERE Pilga_sum_GV<-0.009 \
					and Year="+QString::number(year)+" \
					and Month="+QString::number(month)+" \
					and Year_pilg_pererah>2000 and Month_pilg_pererah>0 and Month_pilg_pererah<13");
	countQuery.seek(0);
	int rowCount = countQuery.value(0).toInt();
	UPostupForm *postup_form = new UPostupForm(0, rowCount );
	postup_form->setPostupLabel("1. ���� ������ ����");
	postup_form->showLable(true);
	postup_form->show();
	
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat, textCharFormat_italic, textCharFormat_bold, cur_textCharFormat;
	
	textCharFormat_italic.setFontPointSize( 9 );
	textCharFormat_italic.setFontItalic( true );
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	QFont font = textCharFormat_bold.font();
	font.setBold(true);
	textCharFormat_bold.setFont(font);
	textCharFormat_italic.setFontPointSize( 9 );
	//��������� ��������� ������� ��� ��� ������� �� X �����, Y ��
	printform = new UPrintForm("³������ ����������� ���� �� �� �� "+monthlist.at(month-1)+" "+QString::number(year)+"�.", 10, 10, 10, 10);
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 8 );
    cursor.insertText("���������� �� \"�������� ��������\" (������� ������� \"�������\". ���: www.abonpro.com). ��� ����������: "+QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy")+"�." , textCharFormat);
	cursor.insertBlock(blockFormat);
	blockFormat.setAlignment( Qt::AlignCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 12 );
	textCharFormat_bold.setFontPointSize( 12 );
	cursor.insertText("²��̲���", textCharFormat_bold); 
	cursor.insertBlock(blockFormat);
	cursor.insertText("����������� ���� �� �� �� "+OrganizName(), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	cursor.insertText("�� " + monthlist.at(month-1) + " " + QString::number(year)+" ����.", textCharFormat);
	cursor.insertBlock(blockFormat);
	
	
	//���� �����
	//��������� �������
	textCharFormat.setFontPointSize( 11 );
	textCharFormat_bold.setFontPointSize( 11 );
	textCharFormat_italic.setFontPointSize( 11 );
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertText("1. ���� �����", textCharFormat_bold);
	cursor.insertBlock(blockFormat);
	
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
		
	//��������� ������ �������
	constraints << QTextLength(QTextLength::FixedLength, 30);
	constraints << QTextLength(QTextLength::FixedLength, 170);
	constraints << QTextLength(QTextLength::FixedLength, 170);
	constraints << QTextLength(QTextLength::FixedLength, 170);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 70);
	
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(1);
	tableFormat.setBorder(0.5);
	QTextTable *table = cursor.insertTable(rowCount+2, 7, tableFormat);
			//���������� ����� �������
	cell = table->cellAt(0, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("� �/�", textCharFormat_italic);
	
	cell = table->cellAt(0, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("�������� �����", textCharFormat_italic);
	
	cell = table->cellAt(0, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("� � �", textCharFormat_italic);
	
	cell = table->cellAt(0, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("������", textCharFormat_italic);
	
	cell = table->cellAt(0, 4);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("г�", textCharFormat_italic);
	
	cell = table->cellAt(0, 5);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("̳����", textCharFormat_italic);
	
	cell = table->cellAt(0, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("���� �����", textCharFormat_italic);
	
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignTop);
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignTop);
	textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignTop);
	
	int iter=1;
	query.exec("SELECT pilgy.Kategoriya, t_abonent_inf_2.abon_NPR, t_abonent_inf_2.abon_ADR, \
						narahuvannya.Year_pilg_pererah, narahuvannya.Month_pilg_pererah, narahuvannya.Pilga_sum_GV, \
						narahuvannya.Rahunok, pilgy.id, narahuvannya.Status \
				FROM (narahuvannya \
				LEFT JOIN t_abonent_inf_2 ON t_abonent_inf_2.id=narahuvannya.Rahunok) \
				LEFT JOIN pilgy ON pilgy.id=narahuvannya.Pilga_id_GV \
				WHERE narahuvannya.Pilga_sum_GV<-0.009 \
					and narahuvannya.Year="+QString::number(year)+" \
					and narahuvannya.Month="+QString::number(month)+" \
					and narahuvannya.Year_pilg_pererah>2000 and narahuvannya.Month_pilg_pererah>0 and narahuvannya.Month_pilg_pererah<13 \
				ORDER BY pilgy.Kategoriya, t_abonent_inf_2.abon_NPR");
	double dVal;
	double pilgSum=0;
	while (query.next()){
		//"� �/�"
		cell = table->cellAt(iter, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QString::number(iter), textCharFormat);
		
		//"�������� �����"
		cell = table->cellAt(iter, 1);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(0).toString(), textCharFormat);
	
		// ϲ� ���������, � � ��� ��������� - ��������
		short pilgaNumber = query.value(8).toInt();
		if (pilgaNumber == 0)
			pilgaNumber = 1;
		else if (pilgaNumber == 1)
			pilgaNumber = 2;
		else if (pilgaNumber == 11)
			pilgaNumber = 1;
		else if (pilgaNumber == 12)
			pilgaNumber = 2;
		pilgovykyQuery.exec("SELECT Prizv, Imya, Batk, Ident, Grupa FROM pilgovyky \
							WHERE Rahunok="+query.value(6).toString()+" \
							and Pilga_ID="+query.value(7).toString()+" \
							and (PilgaNumber="+sqlStr(pilgaNumber)+" or PilgaNumber=0 or PilgaNumber is null) \
							ORDER BY PilgaNumber DESC, Prizv, Imya, Batk");
		bool pilgovyky_e=false;
		if (pilgovykyQuery.next()){
			cell = table->cellAt(iter, 2);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( pilgovykyQuery.value(0).toString()+ " " 
									+pilgovykyQuery.value(1).toString()+ " " 
									+pilgovykyQuery.value(2).toString()  );
			pilgovyky_e=true;
		}
		else{
			//"ϲ�"
			cell = table->cellAt(iter, 2);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(query.value(1).toString(), textCharFormat);
		}
	
		//������
		cell = table->cellAt(iter, 3);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(2).toString(), textCharFormat);
	
		//г�
		cell = table->cellAt(iter, 4);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(3).toString(), textCharFormat);
	
		//̳����
		cell = table->cellAt(iter, 5);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(4).toString(), textCharFormat);
	
		//"����
		cell = table->cellAt(iter, 6);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		dVal = query.value(5).toDouble();
		pilgSum += dVal;
		cellCursor.insertText( uMToStr2(dVal), textCharFormat);

		
		++iter;
		postup_form->inc_pos();
	}
	//ϳ�������� ������
	table->mergeCells(iter,0,1,6);
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText( "������", textCharFormat);
	
	cell = table->cellAt(iter, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText( uMToStr2(pilgSum), textCharFormat);
	
	//����������� �����
	countQuery.exec("SELECT count(*) FROM narahuvannya \
				WHERE Pilga_sum_GV>0.009 \
					and Year="+QString::number(year)+" \
					and Month="+QString::number(month)+" \
					and Year_pilg_pererah>2000 and Month_pilg_pererah>0 and Month_pilg_pererah<13");
	countQuery.seek(0);
	rowCount = countQuery.value(0).toInt();
	postup_form->setCurrentPos(0);
	postup_form->setNumPos(rowCount);
	postup_form->setPostupLabel("2. ���� ������������� ����");
	
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("2. ����������� �����", textCharFormat_bold);
	cursor.insertBlock(blockFormat);
	//��������� �������
	table = cursor.insertTable(rowCount+2, 7, tableFormat);
			//���������� ����� �������
	cell = table->cellAt(0, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("� �/�", textCharFormat_italic);
	
	cell = table->cellAt(0, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("�������� �����", textCharFormat_italic);
	
	cell = table->cellAt(0, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("� � �", textCharFormat_italic);
	
	cell = table->cellAt(0, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("������", textCharFormat_italic);
	
	cell = table->cellAt(0, 4);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("г�", textCharFormat_italic);
	
	cell = table->cellAt(0, 5);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("̳����", textCharFormat_italic);
	
	cell = table->cellAt(0, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("���� �����", textCharFormat_italic);
	
	query.exec("SELECT pilgy.Kategoriya, t_abonent_inf_2.abon_NPR, t_abonent_inf_2.abon_ADR, \
						narahuvannya.Year_pilg_pererah, narahuvannya.Month_pilg_pererah, narahuvannya.Pilga_sum_GV, \
						narahuvannya.Rahunok, pilgy.id, narahuvannya.Status \
				FROM (narahuvannya \
				LEFT JOIN t_abonent_inf_2 ON t_abonent_inf_2.id=narahuvannya.Rahunok) \
				LEFT JOIN pilgy ON pilgy.id=narahuvannya.Pilga_id_GV \
				WHERE narahuvannya.Pilga_sum_GV>0.009 \
					and narahuvannya.Year="+QString::number(year)+" \
					and narahuvannya.Month="+QString::number(month)+" \
					and narahuvannya.Year_pilg_pererah>2000 and narahuvannya.Month_pilg_pererah>0 and narahuvannya.Month_pilg_pererah<13 \
				ORDER BY pilgy.Kategoriya, t_abonent_inf_2.abon_NPR");
	pilgSum = 0;
	iter=1;
	while (query.next()){
		//"� �/�"
		cell = table->cellAt(iter, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QString::number(iter), textCharFormat);
		
		//"�������� �����"
		cell = table->cellAt(iter, 1);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(0).toString(), textCharFormat);
	
		// ϲ� ���������, � � ��� ��������� - ��������
		short pilgaNumber = query.value(8).toInt();
		if (pilgaNumber == 0)
			pilgaNumber = 1;
		else if (pilgaNumber == 1)
			pilgaNumber = 2;
		else if (pilgaNumber == 11)
			pilgaNumber = 1;
		else if (pilgaNumber == 12)
			pilgaNumber = 2;
		pilgovykyQuery.exec("SELECT Prizv, Imya, Batk, Ident, Grupa FROM pilgovyky \
							WHERE Rahunok="+query.value(6).toString()+" \
							and Pilga_ID="+query.value(7).toString()+" \
							and (PilgaNumber="+sqlStr(pilgaNumber)+" or PilgaNumber=0 or PilgaNumber is null) \
							ORDER BY PilgaNumber DESC, Prizv, Imya, Batk");
		bool pilgovyky_e=false;
		if (pilgovykyQuery.next()){
			cell = table->cellAt(iter, 2);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( pilgovykyQuery.value(0).toString()+ " " 
									+pilgovykyQuery.value(1).toString()+ " " 
									+pilgovykyQuery.value(2).toString()  );
			pilgovyky_e=true;
		}
		else{
			//"ϲ�"
			cell = table->cellAt(iter, 2);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(query.value(1).toString(), textCharFormat);
		}
	
		//������
		cell = table->cellAt(iter, 3);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(2).toString(), textCharFormat);
	
		//г�
		cell = table->cellAt(iter, 4);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(3).toString(), textCharFormat);
	
		//̳����
		cell = table->cellAt(iter, 5);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(4).toString(), textCharFormat);
	
		//"����
		cell = table->cellAt(iter, 6);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		dVal = query.value(5).toDouble();
		pilgSum += dVal;
		cellCursor.insertText( uMToStr2(dVal), textCharFormat);

		
		++iter;
		postup_form->inc_pos();
	}
	//ϳ�������� ������
	table->mergeCells(iter,0,1,6);
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText( "������", textCharFormat);
	
	cell = table->cellAt(iter, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText( uMToStr2(pilgSum), textCharFormat);
	
	//��������� ��������� ��� ������
	textCharFormat.setFontPointSize( 12 );
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("	��������						"+PrizvIB(1));
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("    �.�.");
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("	���.���������					"+PrizvIB(2));

		//ʳ���� ��������� ��������� ��� �����
	delete postup_form;
	printform->printer()->setOrientation( QPrinter::Portrait );
	printform->printer()->setDocName("³������ ����������� ���� �� �� �� "+monthlist.at(month-1)+" "+QString::number(year)+"�.");
	printform->show();
}
//------------------------------------------------------------
void UPrintDocs::print_dovidka_u_viddil_subs(int rahunok, int year, int month, double suma)
{
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat, textCharFormat_italic, textCharFormat_bold, cur_textCharFormat;
	
	textCharFormat_italic.setFontItalic( true );
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	QFont font = textCharFormat_bold.font();
	font.setBold(true);
	textCharFormat_bold.setFont(font);
	//��������� ��������� ������� ��� ��� ������� �� X �����, Y ��
	
	printform = new UPrintForm("������ � ���� �����䳿 �� "+monthlist.at(month-1)+" "+QString::number(year)+"�.", 5,5,5,5);
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 7 );
	
	//cursor.insertBlock(blockFormat);
    cursor.insertText("���������� �� \"�������� ��������\" (������� ������� \"�������\". ���: www.abonpro.com). ��� ����������: "+QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy")+"�.      _" , textCharFormat);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	QSqlQuery *query = new QSqlQuery;
	//�����������
	blockFormat.setAlignment( Qt::AlignCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat_bold.setFontPointSize( 12.5 );
	cursor.insertText("��²��� �__________    ������������ ������� � "+QString::number(rahunok), textCharFormat_bold);
	textCharFormat_bold.setFontPointSize( 11 );
	textCharFormat.setFontPointSize( 11 );
	textCharFormat_italic.setFontPointSize( 11 );
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	query->exec("SELECT abonent.prizv, abonent.imya, abonent.batk, budynky.vulycia, \
						budynky.Bud_num, abonent.kvartyra \
				FROM abonent \
				LEFT JOIN budynky \
					ON budynky.id=abonent.budynok_id \
				WHERE abonent.Rahunok="+QString::number(rahunok));
	query->seek(0);
	QString nprStr = query->value(0).toString(), tempStr, adrStr;
	tempStr = query->value(1).toString();
	if (tempStr.size()>0)
		nprStr += " "+tempStr;
	tempStr = query->value(2).toString();
	if (tempStr.size()>0)
		nprStr += " "+tempStr;
	adrStr = "���. "+query->value(3).toString()+" "+query->value(4).toString();
	if (query->value(5).toInt()>0);
		adrStr += " ��. "+ query->value(5).toString();
	cursor.insertText("     ������ �������� ", textCharFormat);
	cursor.insertText(nprStr+" "+", "+adrStr+".", textCharFormat_bold);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("     ��� ��, �� ���� ������� �� ", textCharFormat);
	cursor.insertText(monthlist.at(month-1)+" "+QString::number(year), textCharFormat_bold);
	cursor.insertText("�. ���������:", textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	query->exec("SELECT sum(Narah) \
				FROM narahuvannya \
				WHERE year="+QString::number(year)+" and month="+QString::number(month)+" and Rahunok="+QString::number(rahunok));
	query->seek(0);
	cursor.insertText("         1) �� ���������� �������� ������ ", textCharFormat);
	cursor.insertText(uMToStr2(query->value(0).toDouble()), textCharFormat_bold);
	cursor.insertText(" ���.", textCharFormat);
	cursor.insertBlock(blockFormat);
	if (suma==0)
		tempStr = "________________________________";
	else
		tempStr = uMToStr2(suma);
	cursor.insertText("         2) �� ���������� �������� � ����� �������� ����� "+tempStr+" ���.", textCharFormat);
	query->exec("SELECT sum(Narah_GV) \
				FROM narahuvannya \
				WHERE year="+QString::number(year)+" and month="+QString::number(month)+" and Rahunok="+QString::number(rahunok));
	query->seek(0);
	cursor.insertBlock(blockFormat);
	cursor.insertText("         3) �� ������ �������������� ", textCharFormat);
	cursor.insertText(uMToStr2(query->value(0).toDouble()), textCharFormat_bold);
	cursor.insertText(" ���.", textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	if (suma==0)
		tempStr = "__________________________________";
	else
		tempStr = uMToStr2(suma);
	cursor.insertText("     ����, ��� ������ ��������� �� ���������� �� ������� "+tempStr+" ���.", textCharFormat);
	delete query;
	//ϳ�����
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("	"+DocsPosadaPidpysText(1)+"					"+PrizvIB(1), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("	"+DocsPosadaPidpysText(3)+"				"+PrizvIB(3));
	
	printform->printer()->setDocName("��²��� � ���� �����䳿 �� "+monthlist.at(month-1)+" "+QString::number(year)+"�.");
	printform->show();
}
//------------------------------------------------------------
void UPrintDocs::print_dovidka_u_viddil_subs_2(int rahunok, int year, int month, double suma)
{
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat, textCharFormat_italic, textCharFormat_bold, cur_textCharFormat;
	
	textCharFormat_italic.setFontItalic( true );
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	QFont font = textCharFormat_bold.font();
	font.setBold(true);
	textCharFormat_bold.setFont(font);
	//��������� ��������� ������� ��� ��� ������� �� X �����, Y ��
	
	printform = new UPrintForm("������ � ���� �����䳿 �� "+monthlist.at(month-1)+" "+QString::number(year)+"�.", 5,5,5,5);
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 7 );
	
	//cursor.insertBlock(blockFormat);
    cursor.insertText("���������� �� \"�������� ��������\" (������� ������� \"�������\". ���: www.abonpro.com). ��� ����������: "+QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy")+"�.      _" , textCharFormat);
	cursor.insertBlock(blockFormat);
	blockFormat.setAlignment( Qt::AlignLeft );
	QSqlQuery *query = new QSqlQuery;
	QSqlQuery q;
	//�����������
	blockFormat.setAlignment( Qt::AlignCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 12.5 );
	textCharFormat_bold.setFontPointSize( 12.5 );
	cursor.insertText(OrganizName(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("��²��� �____", textCharFormat_bold);
	textCharFormat_bold.setFontPointSize( 11 );
	textCharFormat.setFontPointSize( 11 );
	textCharFormat_italic.setFontPointSize( 11 );
	blockFormat.setAlignment( Qt::AlignLeft );
	//cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	query->exec("SELECT abonent.prizv, abonent.imya, abonent.batk, budynky.vulycia, \
						budynky.Bud_num, abonent.kvartyra, abonent.Ploshcha, abonent.Ludy, abonent.Pilga_id, \
						abonent.Pilga_for, abonent.Pilga_id_2, abonent.Pilga_for_2, \
						abonent.budynok_id \
				FROM abonent \
				LEFT JOIN budynky ON budynky.id=abonent.budynok_id \
				WHERE abonent.Rahunok="+QString::number(rahunok));
	query->next();
	QString nprStr = query->value(0).toString(), tempStr, adrStr;
	int budId = query->value(12).toInt();
	tempStr = query->value(1).toString();
	if (tempStr.size()>0)
		nprStr += " "+tempStr;
	tempStr = query->value(2).toString();
	if (tempStr.size()>0)
		nprStr += " "+tempStr;
	adrStr = "���. "+query->value(3).toString()+" "+query->value(4).toString();
	if (query->value(5).toInt()>0);
		adrStr += " ��. "+ query->value(5).toString();
	cursor.insertText("      ������ �������� (��������) ", textCharFormat);
	cursor.insertText( nprStr , textCharFormat_bold);
	cursor.insertBlock(blockFormat);
	cursor.insertText("      �������� �����: ", textCharFormat);
	cursor.insertText(uMToStr2(query->value(6).toDouble()), textCharFormat_bold);
	cursor.insertText(" ��.�.", textCharFormat);
	cursor.insertText("      ���������� �� �������:  ", textCharFormat);
	cursor.insertText( adrStr , textCharFormat_bold);
	cursor.insertBlock(blockFormat);
	cursor.insertText("      �������� �������:  ", textCharFormat);
	cursor.insertText( QString::number(rahunok) + "     " , textCharFormat_bold);
	cursor.insertText("      ʳ������ ������, ��������� � ������:  ", textCharFormat);
	cursor.insertText( QString::number(query->value(7).toInt()), textCharFormat_bold);
	cursor.insertBlock(blockFormat);
	cursor.insertText("      �������� ����:  ", textCharFormat);
	
	bool isPilga = false;
	if (query->value(8).toInt()>0){
		isPilga = true;
		int pilgaId = query->value(8).toInt();
		q.exec("SELECT Kategoriya, Vidsotok FROM pilgy WHERE id="+QString::number(pilgaId));
		q.next();
		cursor.insertText( "1) "+q.value(0).toString()+" ("+uMToStr(q.value(1).toDouble())+");" , textCharFormat_bold);
	}
	if (query->value(10).toInt()>0){
		isPilga = true;
		int pilgaId = query->value(10).toInt();
		q.exec("SELECT Kategoriya, Vidsotok FROM pilgy WHERE id="+QString::number(pilgaId));
		q.next();
		cursor.insertText( "    2) "+q.value(0).toString()+" ("+uMToStr(q.value(1).toDouble())+");" , textCharFormat_bold);
	}
	if (!isPilga)
		cursor.insertText( "����", textCharFormat_bold);
	
	cursor.insertBlock(blockFormat);
	cursor.insertText("      ����������� �� ��������� ������� � ����������� ���� (��� �����䳿) �� �����:", textCharFormat);
	cursor.insertBlock(blockFormat);
	query->exec("SELECT sum(Narah) \
				FROM narahuvannya \
				WHERE year="+QString::number(year)+" and month="+QString::number(month)+" and Rahunok="+QString::number(rahunok));
	query->seek(0);
	cursor.insertText("         1) �� ���������� �������� ������: ", textCharFormat);
	cursor.insertText(uMToStr2(query->value(0).toDouble()), textCharFormat_bold);
	query->exec("SELECT Lichylnyk FROM budynky WHERE id="+sqlStr(budId));
	query->next();
	cursor.insertText(" ���. ("+(query->value(0).toBool()?QString("� ����.��������"):QString("���� ����.���������"))+");\n", textCharFormat);
	cursor.insertText("         2) ����� � ����� ����� ����������: ________ �2.;\n", textCharFormat);
	cursor.insertText("         3) � ����� �������� �����: "+(suma!=0 ? uMToStr2(suma) : QString("_____________"))+" ���.;\n", textCharFormat);
	query->exec("SELECT sum(Narah_GV) \
				FROM narahuvannya \
				WHERE year="+QString::number(year)+" and month="+QString::number(month)+" and Rahunok="+QString::number(rahunok));
	query->seek(0);
	cursor.insertText("         4) �� ������ ��������������: ", textCharFormat);
	cursor.insertText(uMToStr2(query->value(0).toDouble()), textCharFormat_bold);
	query->exec("SELECT IsLichylnyk FROM lichylnykyAbon \
				WHERE Rahunok="+sqlStr(rahunok)+" and IsLichylnyk=true");
	cursor.insertText(" ���. ("+(query->next() ? QString("� ����.��������") : QString("���� ����.���������"))+").\n", textCharFormat);
	cursor.insertBlock(blockFormat);
	
	cursor.insertText("      ������������� ����������: ", textCharFormat);
	q.exec("SELECT sum(Suma) FROM kvytancii \
			WHERE Rahunok_id="+sqlStr(rahunok)+" \
					and month(Plata_date)="+sqlStr(month)+" \
					and year(Plata_date)="+sqlStr(year));
	q.next();
	cursor.insertText(uMToStr2(vhSaldoInMonth(rahunok, year, month) - q.value(0).toDouble()), textCharFormat_bold);
	cursor.insertText(" ���.      ����� ���������������� �����: ", textCharFormat);
	q.exec("SELECT Kin_month, Kin_year  FROM spl_zaborg WHERE Rahunok_id="+QString::number(rahunok));
	if (q.next()){
		cursor.insertText("�� " +QString::number(q.value(0).toInt()).rightJustified(2,'0')+"."+QString::number(q.value(1).toInt())+"�.", textCharFormat_bold);
	}
	else{
		cursor.insertText("����", textCharFormat_bold);
	}
	cursor.insertBlock(blockFormat);
	cursor.insertText("      ������ ������ � ���� �����䳿.", textCharFormat_bold);
	
	delete query;
	//ϳ�����
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("              "+DocsPosadaPidpysText(1)+"				"+PrizvIB(1), textCharFormat);
	cursor.insertText("\n\n", textCharFormat);
	cursor.insertText("              "+DocsPosadaPidpysText(2)+"				"+PrizvIB(2), textCharFormat);
	if (organizID() == 2) {
		cursor.insertText("\n\n", textCharFormat);
		cursor.insertText("              "+DocsPosadaPidpysText(6)+"				"+PrizvIB(6), textCharFormat);
	}
	
	printform->printer()->setDocName("��²��� � ���� �����䳿 �� "+monthlist.at(month-1)+" "+QString::number(year)+"�.");
	printform->show();
}
//------------------------------------------------------------
void UPrintDocs::print_zagalna_vidomist_narahuvannya_i_postuplennya(int year, int month)
{
	QSqlQuery query, query_2;
	int rowCount=39;
	
	query_2.exec("SELECT count(*) FROM abonent");
	query_2.seek(0);
	UPostupForm *postup_form = new UPostupForm(0, query_2.value(0).toInt());
	postup_form->showLable(true);
	postup_form->setPostupLabel("������� � ������� ���������� � ���� �����");
	postup_form->show();
	
	QDate prevDate=QDate(year, month, 1).addMonths(-3);
	query.exec("SELECT abonent.Rahunok, abonent.Opal, abonent.GarVoda, sum(curVhSaldoTable.Vh_Saldo1) AS CurVhSaldo, \
						sum(curNarah.Narah1) AS Narah, sum(curNarah.Narah_GV1) AS NarahGV, \
						sum(curSubs.Suma1) AS subsSum, sum(curSubs.Suma_d1) AS subsSum_d, \
						sum(curOpl.Suma1) AS oplSum, sum(prevNarah.Vh_Saldo1) AS saldoSumPrev, \
						abonent.NarahStoyak, sum(curNarah.Narah_St1) AS curNarahST, \
						sum(curSubs.Suma1_GV) AS subsSumGV, sum(curSubs.Suma_d1_GV) AS subsSum_d_GV \
				FROM ((((abonent \
				LEFT JOIN (SELECT Rahunok, sum(Narah) AS Narah1, sum(Narah_GV) AS Narah_GV1, \
									sum(Narah_St) AS Narah_St1 FROM narahuvannya \
							WHERE year="+QString::number(year)+" \
								and month="+QString::number(month)+" \
							GROUP BY Rahunok) AS curNarah \
					ON abonent.Rahunok=curNarah.Rahunok) \
				LEFT JOIN (SELECT Rahunok, sum(Vh_Saldo) AS Vh_Saldo1 FROM narahuvannya \
							WHERE status=0 and year="+QString::number(year)+" \
								and month="+QString::number(month)+" \
							GROUP BY Rahunok) AS curVhSaldoTable \
					ON abonent.Rahunok=curVhSaldoTable.Rahunok) \
				LEFT JOIN (SELECT Rahunok_ID, sum(Suma) AS Suma1, sum(Suma_d) AS Suma_d1, sum(Suma_GV) AS Suma1_GV, sum(Suma_d_GV) AS Suma_d1_GV FROM subs \
							WHERE year="+QString::number(year)+" \
								and month="+QString::number(month)+" \
							GROUP BY Rahunok_ID) AS curSubs \
					ON abonent.Rahunok=curSubs.Rahunok_ID) \
				LEFT JOIN (SELECT Rahunok_ID, sum(Suma) AS Suma1 FROM kvytancii \
							WHERE year(Plata_date)="+QString::number(year)+" \
								and month(Plata_date)="+QString::number(month)+" \
							GROUP BY Rahunok_ID) AS curOpl \
					ON abonent.Rahunok=curOpl.Rahunok_ID) \
				LEFT JOIN (SELECT Rahunok, sum(Vh_Saldo) AS Vh_Saldo1 FROM narahuvannya \
							WHERE status=0 and year="+QString::number(prevDate.year())+" \
								and month="+QString::number(prevDate.month())+" \
							GROUP BY Rahunok) AS prevNarah \
					ON abonent.Rahunok=prevNarah.Rahunok \
				GROUP BY abonent.Rahunok, abonent.Opal, abonent.GarVoda, abonent.NarahStoyak");
	double sumaVhSaldo=0, sumaNarah=0, sumaNarah_GV=0, sumaNarah_ST=0, sumaSubs=0, sumaSubs_d=0, sumaKvyt=0; 
	while (query.next()){
		sumaVhSaldo += d_to_m(query.value(3).toDouble());
		sumaNarah += d_to_m(query.value(4).toDouble());
		sumaNarah_GV += d_to_m(query.value(5).toDouble());
		sumaNarah_ST += d_to_m(query.value(11).toDouble());
		//sumaSubs += d_to_m(query.value(6).toDouble() + query.value(12).toDouble());
		//sumaSubs_d += d_to_m(query.value(7).toDouble() + query.value(13).toDouble());
		sumaKvyt += d_to_m(query.value(8).toDouble());
		postup_form->inc_pos();
	}
	double borg=0,sum=0;
	int count=0;
	
	query_2.exec("SELECT sum(Suma), sum(Suma_d), sum(Suma_GV), sum(Suma_d_GV) FROM subs \
					WHERE year="+sqlStr(year)+" and month="+sqlStr(month));
	query_2.seek(0);
	sumaSubs = query_2.value(0).toDouble() + query_2.value(2).toDouble();
	sumaSubs_d = query_2.value(1).toDouble() + query_2.value(3).toDouble();
	
	postup_form->setCurrentPos(0);
	postup_form->setNumPos(rowCount);
	postup_form->setPostupLabel("��������� ��������� ��� �����");
	
	QDate date(year,month,1);
	
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat, textCharFormat_italic, textCharFormat_bold, cur_textCharFormat;
	
	textCharFormat_italic.setFontItalic( true );
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	QFont font = textCharFormat_bold.font();
	font.setBold(true);
	textCharFormat_bold.setFont(font);

	printform = new UPrintForm("³������ �����������, ����������� � ����� �� ��������� �� "+monthlist.at(month-1)+" "+QString::number(year)+"�.", 10,10,10,10);
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 7 );
    cursor.insertText("���������� �� \"�������� ��������\" (������� ������� \"�������\". ���: www.abonpro.com). ��� ����������: "+QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy")+"�." , textCharFormat);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 3 );
	cursor.insertText(" ",textCharFormat);
	//�����������
	blockFormat.setAlignment( Qt::AlignCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat_bold.setFontPointSize( 13 );
	cursor.insertText("²��̲���\n�����������, ����������� � ����� �� ���������", textCharFormat_bold);
	textCharFormat_bold.setFontPointSize( 10 );
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	cursor.insertText("�� " + monthlist.at(month-1) + " " + QString::number(year)+"�.", textCharFormat_bold);
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 3 );
	cursor.insertText(" ",textCharFormat);
	textCharFormat.setFontPointSize( 10 );
	textCharFormat_italic.setFontPointSize( 10 );
	
	//��������� ������ �������
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
	
	constraints << QTextLength(QTextLength::FixedLength, 35);
	constraints << QTextLength(QTextLength::FixedLength, 500);
	constraints << QTextLength(QTextLength::FixedLength, 80);
	constraints << QTextLength(QTextLength::FixedLength, 90);
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(1);
	tableFormat.setBorder(0.5);
	QTextTable *table = cursor.insertTable(rowCount, 4, tableFormat);
	UPopulateTextTableCells tableCell(table);
	
	int iter=1;
		//���������� �����ֲ
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	tableCell.setAlignment(Qt::AlignHCenter);
	tableCell.setTextCharFormat(textCharFormat);
	tableCell.setTextBlockFormat(blockFormat);
	
	//����� �������
	cell = table->cellAt(0, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("� �/�");
	
	cell = table->cellAt(0, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("������������ ���������");
	
	cell = table->cellAt(0, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("ʳ������ ��������");
	
	cell = table->cellAt(0, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("����, ���");
	postup_form->inc_pos();
	
	//����� �����
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("1.");
	
	//ʳ������ ��������
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("�������� ������� �������� �������");
	
	//ʳ������ ��������
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT count(*) FROM abonent");
	query_2.seek(0);
	cellCursor.insertText(query_2.value(0).toString());
	postup_form->inc_pos();
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("-");
	
	
	++iter;
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("   � �.�. ��������, �� ��������� ������� � ��, � ��");
	
	//ʳ������ ��������
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT count(*) FROM abonent WHERE GarVoda=true and Opal=true");
	query_2.seek(0);
	cellCursor.insertText(query_2.value(0).toString());
	postup_form->inc_pos();
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("-");
	
	
	++iter;
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("   � �.�. ��������, �� ��������� ������� ����������� ��������");
	
	//ʳ������ ��������
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT count(*) FROM abonent WHERE opal=true");
	query_2.seek(0);
	cellCursor.insertText(query_2.value(0).toString());
	postup_form->inc_pos();
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("-");
	

	++iter;
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("   � �.�. ��������, �� ��������� ������� ���������� ������ ����");
	
	//ʳ������ ��������
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT count(*) FROM abonent WHERE GarVoda=true");
	query_2.seek(0);
	cellCursor.insertText(query_2.value(0).toString());
	postup_form->inc_pos();
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("-");
	
	
	++iter;
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("   � �.�. ��������, �� ��������� ������� �� ��� ��");
	
	//ʳ������ �������� � ��������� ��� ����
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT count(*) FROM abonent WHERE GarVoda=false and Opal=true");
	query_2.seek(0);
	cellCursor.insertText(query_2.value(0).toString());
	postup_form->inc_pos();
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("-");
	
	
	++iter;
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("   � �.�. ��������, �� ��������� ������� �������� �� ������");
	
	//ʳ������ �������� �� ��������� ������� �������� �� ������
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT count(*) FROM abonent WHERE NarahStoyak=true");
	query_2.seek(0);
	cellCursor.insertText(query_2.value(0).toString());
	postup_form->inc_pos();
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("-");
	
	
	++iter;
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("   � �.�. ��������, �� �� ��������� ������ (��������)");
	
	//ʳ������ ��������
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT count(*) FROM abonent WHERE GarVoda=false and Opal=false and NarahStoyak=false");
	query_2.seek(0);
	cellCursor.insertText(query_2.value(0).toString());
	postup_form->inc_pos();
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("-");
	
	
	++iter;
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("2.");
	
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("ʳ������ ��������, �� ����� ����");
	
	sum=0;
	count=0;
	query.seek(-1);
	while (query.next()){
		borg = query.value(3).toDouble()+query.value(4).toDouble()+query.value(5).toDouble()-query.value(6).toDouble()-query.value(7).toDouble()-query.value(12).toDouble()-query.value(13).toDouble()-query.value(8).toDouble();
		if (borg>0){
			sum += borg;
			++count;
		}
	}
	//ʳ������ �������� � �� ����� ����
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(count));
	
	//�������� ���� �����
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sum));
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("   � �.�. ��������, �� ��'����� �� �� � ����� ����");
	
	sum=0;
	count=0;
	query.seek(-1);
	while (query.next()){
		borg = query.value(3).toDouble()+query.value(4).toDouble()+query.value(5).toDouble()-query.value(6).toDouble()-query.value(7).toDouble()-query.value(12).toDouble()-query.value(13).toDouble()-query.value(8).toDouble();
		if (borg>0 && query.value(1).toInt()==1){
			sum += borg;
			++count;
		}
	}
	//ʳ������ �������� � �� ����� ����
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(count));
	
	//�������� ���� �����
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sum));
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("   � �.�. ��������, �� ��'����� �� �� � ����� ����");
	
	sum=0;
	count=0;
	query.seek(-1);
	while (query.next()){
		borg = query.value(3).toDouble()+query.value(4).toDouble()+query.value(5).toDouble()-query.value(6).toDouble()-query.value(7).toDouble()-query.value(12).toDouble()-query.value(13).toDouble()-query.value(8).toDouble();
		if (borg>0 && query.value(2).toInt()==1){
			sum += borg;
			++count;
		}
	}
	//ʳ������ �������� � �� ����� ����
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(count));
	
	//�������� ���� �����
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sum));
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("   � �.�. ��������, �� �� ��������� ������ � ����� ����");
	
	sum=0;
	count=0;
	query.seek(-1);
	while (query.next()){
		borg = query.value(3).toDouble()+query.value(4).toDouble()+query.value(5).toDouble()-query.value(6).toDouble()-query.value(7).toDouble()-query.value(12).toDouble()-query.value(13).toDouble()-query.value(8).toDouble();
		if (borg>0 && query.value(1).toInt()==0 && query.value(2).toInt()==0){
			sum += borg;
			++count;
		}
	}
	//ʳ������ �������� � �� ����� ����
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(count));
	
	//�������� ���� �����
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sum));
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("   � �.�. ��������, � �� � ��� ��");
	
	sum=0;
	count=0;
	query.seek(-1);
	while (query.next()){
		borg = query.value(3).toDouble()+query.value(4).toDouble()+query.value(5).toDouble()-query.value(6).toDouble()-query.value(7).toDouble()-query.value(12).toDouble()-query.value(13).toDouble()-query.value(8).toDouble();
		if (borg>0 && query.value(1).toInt()==1 && query.value(2).toInt()==0){
			sum += borg;
			++count;
		}
	}
	//ʳ������ �������� � �� ����� ����
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(count));
	
	//�������� ���� �����
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sum));
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("3.");
	
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("���� ������� ����� 3-��� ������");
	
	sum=0;
	count=0;
	query.seek(-1);
	while (query.next()){
		borg = query.value(3).toDouble()+query.value(4).toDouble()+query.value(5).toDouble()-query.value(6).toDouble()-query.value(7).toDouble()-query.value(12).toDouble()-query.value(13).toDouble()-query.value(8).toDouble();
		if (borg>0 && query.value(9).toDouble()>0.00999){
			sum += query.value(9).toDouble();
			++count;
		}
	}
	//ʳ������ �������� � �� ����� ����
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(count));
	
	//�������� ���� �����
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sum));
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("   � �.�. ���� ������� ����� 3-��� ������, �������� � ��");
	
	sum=0;
	count=0;
	query.seek(-1);
	while (query.next()){
		borg = query.value(3).toDouble()+query.value(4).toDouble()+query.value(5).toDouble()-query.value(6).toDouble()-query.value(7).toDouble()-query.value(12).toDouble()-query.value(13).toDouble()-query.value(8).toDouble();
		if (borg>0 && query.value(9).toDouble()>0.00999 && query.value(1).toInt()==1){
			sum += query.value(9).toDouble();
			++count;
		}
	}
	//ʳ������ �������� � �� ����� ����
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(count));
	
	//�������� ���� �����
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sum));
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("   � �.�. ���� ������� ����� 3-��� ������, �������� � ��");
	
	sum=0;
	count=0;
	query.seek(-1);
	while (query.next()){
		borg = query.value(3).toDouble()+query.value(4).toDouble()+query.value(5).toDouble()-query.value(6).toDouble()-query.value(7).toDouble()-query.value(12).toDouble()-query.value(13).toDouble()-query.value(8).toDouble();
		if (borg>0 && query.value(9).toDouble()>0.00999 && query.value(2).toInt()==1){
			sum += query.value(9).toDouble();
			++count;
		}
	}
	//ʳ������ �������� � �� ����� ����
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(count));
	
	//�������� ���� �����
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sum));
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("   � �.�. ���� ������� ����� 3-��� ������, �������� � �� � ��� ��");
	
	sum=0;
	count=0;
	query.seek(-1);
	while (query.next()){
		borg = query.value(3).toDouble()+query.value(4).toDouble()+query.value(5).toDouble()-query.value(6).toDouble()-query.value(7).toDouble()-query.value(12).toDouble()-query.value(13).toDouble()-query.value(8).toDouble();
		if (borg>0 && query.value(9).toDouble()>0.00999 && query.value(1).toInt()==1 && query.value(2).toInt()==0){
			sum += query.value(9).toDouble();
			++count;
		}
	}
	//ʳ������ �������� � �� ����� ����
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(count));
	
	//�������� ���� �����
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sum));
	postup_form->inc_pos();
	
	++iter;
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("   � �.�. ���� ������� ����� 3-��� ������, ��� ���������� ��������");
	
	sum=0;
	count=0;
	query.seek(-1);
	while (query.next()){
		borg = query.value(3).toDouble()+query.value(4).toDouble()+query.value(5).toDouble()-query.value(6).toDouble()-query.value(7).toDouble()-query.value(12).toDouble()-query.value(13).toDouble()-query.value(8).toDouble();
		if (borg>0 && query.value(9).toDouble()>0.00999 && query.value(1).toInt()==0 && query.value(2).toInt()==0){
			sum += query.value(9).toDouble();
			++count;
		}
	}
	//ʳ������ �������� � �� ����� ����
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(count));
	
	//�������� ���� �����
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sum));
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("4.");
	
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("������ ������ ������ �� 01."+date.toString("MM.yyyy")+"�.", textCharFormat_bold);

	//������ ������
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("-");
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sumaVhSaldo), textCharFormat_bold);
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("5.");
	
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("���������� �� ��");

	//���������� �� ��
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT count(*) FROM (SELECT Rahunok FROM narahuvannya \
				WHERE year="+QString::number(year)+" \
					and month="+QString::number(month)+" \
					and (narah>0.009 or narah<-0.009) GROUP BY Rahunok)");
	query_2.seek(0);
	if (query_2.value(0).toInt()>0)
		cellCursor.insertText(query_2.value(0).toString());
	else
		cellCursor.insertText("-");
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT sum(Pilga_sum) FROM narahuvannya WHERE year="+QString::number(year)+" and month="+QString::number(month));
	query_2.seek(0);
	double curPilgaSumCo = query_2.value(0).toDouble();
	cellCursor.insertText(uMToStr2(sumaNarah+curPilgaSumCo));
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("");
	
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("   � �.�. ����������� ��� �����", textCharFormat_bold);

	//� �.�. ���������� ��� �����
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("-");
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sumaNarah), textCharFormat_bold);
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("   � �.�. �����");

	//����� �� ��
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT count(*) FROM (SELECT Rahunok FROM narahuvannya \
				WHERE year="+QString::number(year)+" \
					and month="+QString::number(month)+" \
					and (Pilga_sum>0.009 or Pilga_sum<-0.009) GROUP BY Rahunok)");
	query_2.seek(0);
	if (query_2.value(0).toInt()>0)
		cellCursor.insertText(query_2.value(0).toString());
	else
		cellCursor.insertText("-");
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(curPilgaSumCo));
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("6.");
	
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("���������� �� ��");

	//���������� �� ��
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT count(*) FROM (SELECT Rahunok FROM narahuvannya \
				WHERE year="+QString::number(year)+" \
					and month="+QString::number(month)+" \
					and (Narah_GV>0.009 or Narah_GV<-0.009) GROUP BY Rahunok)");
	query_2.seek(0);
	if (query_2.value(0).toInt()>0)
		cellCursor.insertText(query_2.value(0).toString());
	else
		cellCursor.insertText("-");
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT sum(Pilga_sum_GV) FROM narahuvannya WHERE year="+QString::number(year)+" and month="+QString::number(month));
	query_2.seek(0);
	double curPilgaSumGV = query_2.value(0).toDouble();
	cellCursor.insertText(uMToStr2(sumaNarah_GV + curPilgaSumGV));
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("");
	
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("   � �.�. ����������� �� �� ��� �����", textCharFormat_bold);

	//���������� �� ��
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("-");
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sumaNarah_GV), textCharFormat_bold);
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("   � �.�. �����");
	
	
	//����� �� ��
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT count(*) FROM (SELECT Rahunok FROM narahuvannya \
				WHERE year="+QString::number(year)+" \
					and month="+QString::number(month)+" \
					and (Pilga_sum_GV>0.009 or Pilga_sum_GV<-0.009) GROUP BY Rahunok)");
	query_2.seek(0);
	if (query_2.value(0).toInt()>0)
		cellCursor.insertText(query_2.value(0).toString());
	else
		cellCursor.insertText("-");
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(curPilgaSumGV));
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("7.");
	
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("���������� �� ��", textCharFormat_bold);

	//���������� �� ��
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT count(*) FROM (SELECT Rahunok FROM narahuvannya \
				WHERE year="+QString::number(year)+" \
					and month="+QString::number(month)+" \
					and (Narah_St>0.009 or Narah_St<-0.009) GROUP BY Rahunok)");
	query_2.seek(0);
	if (query_2.value(0).toInt()>0)
		cellCursor.insertText(query_2.value(0).toString());
	else
		cellCursor.insertText("-");
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sumaNarah_ST), textCharFormat_bold);
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("8.");
	
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("��������� ���������� �� ���");

	//��������� �� ���
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT count(*) FROM (SELECT id FROM korektura_s \
				WHERE year="+QString::number(year)+" \
					and month="+QString::number(month)+" \
					and (Suma>0.009 or Suma<-0.009 or Suma_GV>0.009 or Suma_GV<-0.009 or Suma_ST>0.009 or Suma_ST<-0.009) GROUP BY id)");
	query_2.seek(0);
	if (query_2.value(0).toInt()>0)
		cellCursor.insertText(query_2.value(0).toString());
	else
		cellCursor.insertText("-");
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT sum(Suma), sum(Suma_GV), sum(Suma_ST) FROM korektura_s WHERE year="+QString::number(year)+" and month="+QString::number(month));
	query_2.seek(0);
	cellCursor.insertText(uMToStr2(query_2.value(0).toDouble() + query_2.value(1).toDouble() + query_2.value(2).toDouble()));
	postup_form->inc_pos();
	
	++iter;
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("9.");
	
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("��������� ���������� �� �����");

	//��������� �� �����
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT count(*) FROM (SELECT id korektura \
				WHERE year="+QString::number(year)+" \
					and month="+QString::number(month)+" \
					and (Suma>0.009 or Suma<-0.009) GROUP BY id)");
	query_2.seek(0);
	if (query_2.value(0).toInt()>0)
		cellCursor.insertText(query_2.value(0).toString());
	else
		cellCursor.insertText("-");
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT sum(Suma) FROM korektura WHERE year="+QString::number(year)+" and month="+QString::number(month));
	query_2.seek(0);
	cellCursor.insertText(uMToStr2(query_2.value(0).toDouble()));
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("10.");
	
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("���������� �����䳿 �� �����", textCharFormat_bold);

	//����������� �����䳿
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT count(*) FROM (SELECT Rahunok_ID FROM subs \
				WHERE year="+QString::number(year)+" \
					and month="+QString::number(month)+" \
					and (Suma>0.009 or Suma<-0.009 or Suma_d>0.009 or Suma_d<-0.009 or Suma_GV>0.009 or Suma_GV<-0.009 or Suma_d_GV>0.009 or Suma_d_GV<-0.009) GROUP BY Rahunok_ID)");
	query_2.seek(0);
	if (query_2.value(0).toInt()>0)
		cellCursor.insertText(query_2.value(0).toString());
	else
		cellCursor.insertText("-");
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sumaSubs+sumaSubs_d), textCharFormat_bold);
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("   � �.�. ����������� �����䳿");

	//����������� �����䳿
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT count(*) FROM (SELECT Rahunok_ID FROM subs \
				WHERE year="+QString::number(year)+" \
					and month="+QString::number(month)+" \
					and (Suma>0.009 or Suma<-0.009 or Suma_GV>0.009 or Suma_GV<-0.009) GROUP BY Rahunok_ID)");
	query_2.seek(0);
	if (query_2.value(0).toInt()>0)
		cellCursor.insertText(query_2.value(0).toString());
	else
		cellCursor.insertText("-");
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sumaSubs));
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("   � �.�. ������������� �����䳿");

	//������������� �����䳿
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT count(*) FROM (SELECT Rahunok_ID FROM subs \
				WHERE year="+QString::number(year)+" \
					and month="+QString::number(month)+" \
					and (Suma_d>0.009 or Suma_d<-0.009 or Suma_d_GV>0.009 or Suma_d_GV<-0.009) GROUP BY Rahunok_ID)");
	query_2.seek(0);
	if (query_2.value(0).toInt()>0)
		cellCursor.insertText(query_2.value(0).toString());
	else
		cellCursor.insertText("-");
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sumaSubs_d));
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("11.");
	
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("����� ���������� ��� �����䳿");

	//����� ���������� ��� �����䳿
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("-");
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sumaNarah+sumaNarah_GV+sumaNarah_ST-sumaSubs-sumaSubs_d));
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("12.");
	
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("������ �� �����", textCharFormat_bold);

	//������ �� ����������
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT count(*) FROM kvytancii \
					WHERE year(Plata_date)="+sqlStr(year)+" \
						and month(Plata_date)="+sqlStr(month)+" \
						and (Suma>0.009 or Suma<-0.009)");
	query_2.seek(0);
	if (query_2.value(0).toInt()>0)
		cellCursor.insertText(query_2.value(0).toString());
	else
		cellCursor.insertText("-");
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sumaKvyt), textCharFormat_bold);
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("");
	
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("� �.�. �������� ������");

	//�.�.�.�������� ������
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT count(*), sum(Suma) FROM kvytancii \
					WHERE year(Plata_date)="+sqlStr(year)+" \
						and month(Plata_date)="+sqlStr(month)+" \
						and (VydOplaty_ID is null or VydOplaty_ID=0 or VydOplaty_ID=1) \
						and (Suma>0.009 or Suma<-0.009)");
	query_2.seek(0);
	if (query_2.value(0).toInt()>0)
		cellCursor.insertText(query_2.value(0).toString());
	else
		cellCursor.insertText("-");
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	if (query_2.value(1).toDouble()>0)
		cellCursor.insertText(uMToStr2(query_2.value(1).toDouble()));
	else
		cellCursor.insertText("-");
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("");
	
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("� �.�. г����� ����");

	//�.�.�.г����� ����
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT count(*), sum(Suma) FROM kvytancii \
					WHERE year(Plata_date)="+sqlStr(year)+" \
						and month(Plata_date)="+sqlStr(month)+" \
						and VydOplaty_ID=2 \
						and (Suma>0.009 or Suma<-0.009)");
	query_2.seek(0);
	if (query_2.value(0).toInt()>0)
		cellCursor.insertText(query_2.value(0).toString());
	else
		cellCursor.insertText("-");
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	if (query_2.value(1).toDouble()>0)
		cellCursor.insertText(uMToStr2(query_2.value(1).toDouble()));
	else
		cellCursor.insertText("-");
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("");
	
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("� �.�. ��������");

	//�.�.�.��������
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT count(*), sum(Suma) FROM kvytancii \
					WHERE year(Plata_date)="+sqlStr(year)+" \
						and month(Plata_date)="+sqlStr(month)+" \
						and VydOplaty_ID=3 \
						and (Suma>0.009 or Suma<-0.009)");
	query_2.seek(0);
	if (query_2.value(0).toInt()>0)
		cellCursor.insertText(query_2.value(0).toString());
	else
		cellCursor.insertText("-");
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	if (query_2.value(1).toDouble()>0)
		cellCursor.insertText(uMToStr2(query_2.value(1).toDouble()));
	else
		cellCursor.insertText("-");
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("");
	
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("� �.�. ���������");

	//�.�.�.���������
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT count(*), sum(Suma) FROM kvytancii \
					WHERE year(Plata_date)="+sqlStr(year)+" \
						and month(Plata_date)="+sqlStr(month)+" \
						and VydOplaty_ID=4 \
						and (Suma>0.009 or Suma<-0.009)");
	query_2.seek(0);
	if (query_2.value(0).toInt()>0)
		cellCursor.insertText(query_2.value(0).toString());
	else
		cellCursor.insertText("-");
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	if (query_2.value(1).toDouble()>0)
		cellCursor.insertText(uMToStr2(query_2.value(1).toDouble()));
	else
		cellCursor.insertText("-");
	postup_form->inc_pos();
	
	
	++iter;
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("");
	
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("� �.�. �� ������");

	//�.�.�.���������
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	query_2.exec("SELECT count(*), sum(Suma) FROM kvytancii \
					WHERE year(Plata_date)="+sqlStr(year)+" \
						and month(Plata_date)="+sqlStr(month)+" \
						and VydOplaty_ID=5 \
						and (Suma>0.009 or Suma<-0.009)");
	query_2.seek(0);
	if (query_2.value(0).toInt()>0)
		cellCursor.insertText(query_2.value(0).toString());
	else
		cellCursor.insertText("-");
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	if (query_2.value(1).toDouble()>0)
		cellCursor.insertText(uMToStr2(query_2.value(1).toDouble()));
	else
		cellCursor.insertText("-");
	postup_form->inc_pos();
	
	//���������� ��� ����������� �������������
	int rowCountPereved=0;
	double sumaPereved=0;
	int countPereved = 0;
	query_2.exec("SELECT perevedType.Name, count(*) AS perevCount, sum(narahuvannyaPereved.Suma) AS perevSuma \
				FROM narahuvannyaPereved \
				LEFT JOIN perevedType ON perevedType.id=narahuvannyaPereved.PerevedType_Id \
				WHERE year(CDate)="+sqlStr(year)+" \
					and month(CDate)="+sqlStr(month)+" \
					and (Suma>0.0099 or Suma<-0.0099) \
				GROUP BY perevedType.Name ORDER BY perevedType.Name");
	while (query_2.next()){
		++rowCountPereved;
		sumaPereved += query_2.value(2).toDouble();
		countPereved += qRound(query_2.value(1).toDouble());
	}
	table->insertRows( iter+1, rowCountPereved+1 );
	
	//ϳ�������� ������ �����������
	++iter;
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("13.");
	
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("����������� �������������", textCharFormat_bold);
	
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(countPereved));
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sumaPereved), textCharFormat_bold);
	
	query_2.seek(-1);
	for (int perevIter=0; perevIter<rowCountPereved; perevIter++){
		query_2.next();
		++iter;
		
		//������ ����������� � �.�.
		cell = table->cellAt(iter, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("");
	
		cell = table->cellAt(iter, 1);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("   � �.�. "+query_2.value(0).toString());
	
		cell = table->cellAt(iter, 2);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QString::number(qRound(query_2.value(1).toDouble())));
	
		cell = table->cellAt(iter, 3);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(query_2.value(2).toDouble()));
	}
	
	++iter;
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("14.");
	
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("������� ������ ������ �� "+QString::number(date.daysInMonth())+"."+date.toString("MM.yyyy")+"�.", textCharFormat_bold);

	//������� ������
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("-");
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sumaVhSaldo+sumaNarah+sumaNarah_GV+sumaNarah_ST-sumaSubs-sumaSubs_d-sumaKvyt+sumaPereved), textCharFormat_bold);
	postup_form->inc_pos();
	
	/*
	++iter;
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("");
	
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("� �.�. ������� ������ ��� �����. ����");

	//� �.�. ������� ������ ��� ����������� ����
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("-");
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sumaVhSaldo+sumaNarah+sumaNarah_GV+sumaNarah_ST-sumaSubs-sumaSubs_d-sumaKvyt+sumaPereved - curPilgaSumGV - curPilgaSumCo));  
	postup_form->inc_pos();
	*/
	
	//������������ GUI � ��-�����
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	// ����� �� �������� ��������::�������
	bool needBeznadDebitZaborg = settings->value("GUI_View/ShowBeznadDebitZaborg").toBool();
	delete settings;
	
	if (needBeznadDebitZaborg){
		table->insertRows( iter+1, 5 );
		
		query_2.exec("SELECT sum(Vh_saldo), sum(ZaborgAdd), sum(OplataSuma) \
					FROM debitorskaZaborg \
					WHERE year(CDate)="+sqlStr(year)+" \
						and month(CDate)="+sqlStr(month));
		query_2.next();
		double vhSaldoDeb = query_2.value(0).toDouble();
		double addZaborgDeb = query_2.value(1).toDouble();
		double oplataDeb = query_2.value(2).toDouble();
		
		query_2.exec("SELECT sum(Suma) FROM narahuvannyaPereved \
					WHERE PerevedType_id=2 \
						and year(CDate)="+sqlStr(year)+" \
						and month(CDate)="+sqlStr(month));
		query_2.next();
		addZaborgDeb -= query_2.value(0).toDouble();
		
		//��������� ���������� �������������
		++iter;
		tableCell.setAlignment(Qt::AlignHCenter);
		tableCell.set(iter, 0, "15.");
		tableCell.setAlignment(Qt::AlignLeft);
		if (organizID() != 4)
			tableCell.set(iter, 1, "��������� ���������� �������������");
		else
			tableCell.set(iter, 1, "������� ���������� �������������");
		tableCell.setAlignment(Qt::AlignHCenter);
		tableCell.set(iter, 2, "-");
		tableCell.setAlignment(Qt::AlignRight);
		tableCell.set(iter, 3, "-");
		
		++iter;
		tableCell.setAlignment(Qt::AlignHCenter);
		tableCell.set(iter, 0, "");
		tableCell.setAlignment(Qt::AlignLeft);
		tableCell.set(iter, 1, "   ������ ������");
		tableCell.setAlignment(Qt::AlignHCenter);
		tableCell.set(iter, 2, "-");
		tableCell.setAlignment(Qt::AlignRight);
		tableCell.setDash(iter, 3, vhSaldoDeb);
	
		++iter;
		tableCell.setAlignment(Qt::AlignHCenter);
		tableCell.set(iter, 0, "");
		tableCell.setAlignment(Qt::AlignLeft);
		tableCell.set(iter, 1, "   ����������� �������������");
		tableCell.setAlignment(Qt::AlignHCenter);
		tableCell.set(iter, 2, "-");
		tableCell.setAlignment(Qt::AlignRight);
		tableCell.setDash(iter, 3, addZaborgDeb);
		
		++iter;
		tableCell.setAlignment(Qt::AlignHCenter);
		tableCell.set(iter, 0, "");
		tableCell.setAlignment(Qt::AlignLeft);
		tableCell.set(iter, 1, "   ������");
		tableCell.setAlignment(Qt::AlignHCenter);
		tableCell.set(iter, 2, "-");
		tableCell.setAlignment(Qt::AlignRight);
		tableCell.setDash(iter, 3, oplataDeb);
		
		++iter;
		tableCell.setAlignment(Qt::AlignHCenter);
		tableCell.set(iter, 0, "");
		tableCell.setAlignment(Qt::AlignLeft);
		tableCell.set(iter, 1, "   ������� ������");
		tableCell.setAlignment(Qt::AlignHCenter);
		tableCell.set(iter, 2, "-");
		tableCell.setAlignment(Qt::AlignRight);
		tableCell.set(iter, 3, vhSaldoDeb + addZaborgDeb - oplataDeb);
	}
	
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("     "+DocsPosadaPidpysText(3)+"					"+PrizvIB(3));
	
	delete postup_form;
	printform->printer()->setDocName("³������ �����������, ����������� � ����� �� ��������� �� "+monthlist.at(month-1)+" "+QString::number(year)+"�.");
	printform->show();
}
//------------------------------------------------------------
void UPrintDocs::print_vidomost_narahuvannya_dlya_abon_z_lich(short year, short month)
{
	QSqlQuery query;
	/*
	query.exec("SELECT count(*)  \
				FROM abonent \
				LEFT JOIN budynky \
					ON budynky.id=abonent.Budynok_id \
				WHERE abonent.GarVoda=true or (budynky.Lichylnyk=1 and abonent.Opal=true)");
	*/
	UPostupForm *postup_form = new UPostupForm(0, 0);
	postup_form->showLable(true);
	postup_form->setPostupLabel("��������� �������");
	postup_form->show();
	
	query.exec("SELECT count(*) FROM (SELECT abonent.rahunok, sum(narahuvannya.Narah) AS narahCO, \
				sum(narahuvannya.Narah_GV) AS NarahGV \
				FROM (abonent \
				LEFT JOIN budynky ON budynky.id=abonent.Budynok_id) \
				LEFT JOIN (SELECT * FROM narahuvannya \
				WHERE year="+sqlStr(year)+" and month="+sqlStr(month)+") AS narahuvannya ON narahuvannya.Rahunok=abonent.Rahunok \
				WHERE (EXISTS (SELECT lichylnykyAbon.Rahunok FROM lichylnykyAbon WHERE \
					lichylnykyAbon.rahunok=abonent.Rahunok and lichylnykyAbon.isLichylnyk=true)) or (budynky.Lichylnyk=1 \
					and abonent.Opal=true) or (EXISTS (SELECT lichylnykyBudynkovi.id FROM lichylnykyBudynkovi WHERE \
					lichylnykyBudynkovi.Budynok_ID=abonent.Budynok_ID and lichylnykyBudynkovi.isLichylnyk=true)) \
				GROUP BY abonent.rahunok) \
				WHERE narahCO>0.009 or narahGV>0.009");
	
	query.seek(0);
	int rowCount = query.value(0).toInt();
	postup_form->setNumPos(rowCount);
	
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat, textCharFormat_italic, textCharFormat_bold, cur_textCharFormat;
	
	textCharFormat_italic.setFontItalic( true );
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	QFont font = textCharFormat_bold.font();
	font.setBold(true);
	textCharFormat_bold.setFont(font);
	//��������� ��������� ������� ��� ��� ������� �� X �����, Y ��
	printform = new UPrintForm("³������ ����������� ��� �������� � ����������� �� "+monthlist.at(month-1)+" "+QString::number(year)+"�.");
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 7 );
    cursor.insertText("���������� �� \"�������� ��������\" (������� ������� \"�������\". ���: www.abonpro.com). ��� ����������: "+QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy")+"�." , textCharFormat);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertText(" ",textCharFormat);
	//�����������
	blockFormat.setAlignment( Qt::AlignCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat_bold.setFontPointSize( 13 );
	cursor.insertText("²��̲���\n����������� ��� �������� � �����������", textCharFormat_bold);
	textCharFormat_bold.setFontPointSize( 10 );
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	cursor.insertText("�� " + monthlist.at(month-1) + " " + QString::number(year)+"�.", textCharFormat_bold);
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 7 );
	cursor.insertText(" ",textCharFormat);
	textCharFormat.setFontPointSize( 10 );
	textCharFormat_italic.setFontPointSize( 10 );
	textCharFormat_bold.setFontPointSize( 10 );
	cursor.insertBlock(blockFormat);
	
	//��������� ������ �������
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
	
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 150);
	constraints << QTextLength(QTextLength::FixedLength, 200);
	constraints << QTextLength(QTextLength::FixedLength, 60);
	constraints << QTextLength(QTextLength::FixedLength, 60);
	constraints << QTextLength(QTextLength::FixedLength, 60);
	constraints << QTextLength(QTextLength::FixedLength, 60);
	constraints << QTextLength(QTextLength::FixedLength, 60);
	
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(1);
	tableFormat.setBorder(0.5);
	QTextTable *table = cursor.insertTable(rowCount+2, 9, tableFormat);
	
	//����� �������
	cell = table->cellAt(0, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("� �/�");
	
	cell = table->cellAt(0, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("���.");
	
	cell = table->cellAt(0, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("� � �");
	
	cell = table->cellAt(0, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("������");
	
	cell = table->cellAt(0, 4);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("��, �.��.");
	
	cell = table->cellAt(0, 5);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("��, �.���");
	
	cell = table->cellAt(0, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("���� ��, ���");
	
	cell = table->cellAt(0, 7);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("���� ��, ���");
	
	cell = table->cellAt(0, 8);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("����, ���");
	/*
	query.exec("SELECT abonent.rahunok, abonent.Prizv, abonent.Imya, abonent.Batk, budynky.Vulycia, \
						budynky.Bud_num, abonent.Kvartyra, sum(narahuvannya.Narah) AS narahCO, \
						sum(narahuvannya.Narah_GV) AS NarahGV, abonent.ludy, abonent.Ploshcha \
				FROM (abonent \
				LEFT JOIN budynky ON budynky.id=abonent.Budynok_id) \
				LEFT JOIN (SELECT * FROM narahuvannya \
							WHERE year="+QString::number(year)+" \
								and month="+QString::number(month)+") AS narahuvannya \
					ON narahuvannya.Rahunok=abonent.Rahunok \
				WHERE abonent.GarVoda=true or (budynky.Lichylnyk=1 and abonent.Opal=true) \
				GROUP BY abonent.rahunok, abonent.Prizv, abonent.Imya, abonent.Batk, \
					budynky.Vulycia, budynky.Bud_num, abonent.Kvartyra, abonent.ludy, abonent.Ploshcha \
				ORDER BY abonent.rahunok");
	*/
	query.exec("SELECT * FROM (SELECT abonent.rahunok, abonent.Prizv, abonent.Imya, abonent.Batk, budynky.Vulycia, \
					budynky.Bud_num, abonent.Kvartyra, sum(narahuvannya.Narah) AS narahCO, \
					sum(narahuvannya.Narah_GV) AS NarahGV, abonent.ludy, abonent.Ploshcha \
				FROM (abonent \
				LEFT JOIN budynky ON budynky.id=abonent.Budynok_id) \
				LEFT JOIN (SELECT * FROM narahuvannya \
				WHERE year="+sqlStr(year)+" and month="+sqlStr(month)+") AS narahuvannya ON narahuvannya.Rahunok=abonent.Rahunok \
				WHERE (EXISTS (SELECT lichylnykyAbon.Rahunok FROM lichylnykyAbon WHERE \
					lichylnykyAbon.rahunok=abonent.Rahunok and lichylnykyAbon.isLichylnyk=true)) or (budynky.Lichylnyk=1 \
					and abonent.Opal=true) or (EXISTS (SELECT lichylnykyBudynkovi.id FROM lichylnykyBudynkovi WHERE \
					lichylnykyBudynkovi.Budynok_ID=abonent.Budynok_ID and lichylnykyBudynkovi.isLichylnyk=true))\
				GROUP BY abonent.rahunok, abonent.Prizv, abonent.Imya, abonent.Batk, budynky.Vulycia, \
					budynky.Bud_num, abonent.Kvartyra, abonent.ludy, abonent.Ploshcha \
				ORDER BY abonent.rahunok) \
				WHERE narahCO>0.009 or narahGV>0.009");
	
	
	int iter=1;
	QString adrStr;
	UNarah_data *d = new UNarah_data;
	d->nDate = QDate(year, month, 1);
	d->normaGV = normaSpojyvGVNaOsobu(year,month);
	double sSum=0, kubSum=0, opalSum=0, gvSum=0, val;
	while (query.next()){
		//"� �/�"
		cell = table->cellAt(iter, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QString::number(iter));
		
		//"�������"
		cell = table->cellAt(iter, 1);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(0).toString());
		
		//"ϲ�"
		cell = table->cellAt(iter, 2);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(1).toString() + " " + query.value(2).toString() + " " + query.value(3).toString());
		
		//"������"
		cell = table->cellAt(iter, 3);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		adrStr = query.value(4).toString() + " " + query.value(5).toString();
		if (query.value(6).toInt()>0)
			adrStr += ", ��." + query.value(6).toString();
		cellCursor.insertText(adrStr);
		
		//"��, �.��."
		cell = table->cellAt(iter, 4);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		val = query.value(10).toDouble();
		sSum += val;
		cellCursor.insertText(QString::number(val).replace(".",","));
		
		//"�.���"
		cell = table->cellAt(iter, 5);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		d->rah = query.value(0).toInt();
		d->ludy = query.value(9).toInt();
		val = f_read_vykor_GV(d);
		kubSum += val;
		if (val > 0)
			cellCursor.insertText(QString::number(val).replace(".",","));
		else
			cellCursor.insertText("-");
		
		//"���� �� ���"
		cell = table->cellAt(iter, 6);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		val = query.value(7).toDouble();
		opalSum += val;
		cellCursor.insertText(uMToStr2(val));

		//"���� ��, ���"
		cell = table->cellAt(iter, 7);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		val = query.value(8).toDouble();
		gvSum += val;
		cellCursor.insertText(uMToStr2(val));
	
		//"����, ���"
		cell = table->cellAt(iter, 8);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(query.value(7).toDouble() + query.value(8).toDouble()));
		
		++iter;
		postup_form->inc_pos();
	}
	delete d;
	
	table->mergeCells(iter, 0, 1, 4);
	
	//����� ������
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("������");
	
	//"��, �.��."
	cell = table->cellAt(iter, 4);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(sSum).replace(".",","));
		
	//"�.���"
	cell = table->cellAt(iter, 5);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(kubSum).replace(".",","));
		
	//"���� �� ���"
	cell = table->cellAt(iter, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(opalSum));

	//"���� ��, ���"
	cell = table->cellAt(iter, 7);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(gvSum));
	
	//"����, ���"
	cell = table->cellAt(iter, 8);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(opalSum + gvSum));
	
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("     "+DocsPosadaPidpysText(3)+"					"+PrizvIB(3));
	
	delete postup_form;
	printform->printer()->setDocName("³������ ����������� ��� �������� � ����������� �� "+monthlist.at(month-1)+" "+QString::number(year)+"�.");
	printform->show();
}
//------------------------------------------------------------
UPrintDocs::~UPrintDocs()
{

}
//------------------------------------------------------------
