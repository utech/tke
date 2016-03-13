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
	
	monthlist 	<< codec->toUnicode("січень")
				<< codec->toUnicode("лютий")
				<< codec->toUnicode("березень")
				<< codec->toUnicode("квітень")
				<< codec->toUnicode("травень")
				<< codec->toUnicode("червень")
				<< codec->toUnicode("липень")
				<< codec->toUnicode("серпень")
				<< codec->toUnicode("вересень")
				<< codec->toUnicode("жовтень")
				<< codec->toUnicode("листопад")
				<< codec->toUnicode("грудень");
}
//------------------------------------------------------------
void UPrintDocs::print_vidomist_pilg(int cur_year, int cur_month)
{
		int year = cur_year;
		int month = cur_month;
		printform = new UPrintForm(0, codec->toUnicode("Зведена відомість пільг"));
		QTextBlockFormat blockFormat;
		QTextTableCell cell;
		QTextCharFormat textCharFormat;
		//Створення документа тарифів для всіх будинків за X місяць, Y рік
		printform->document()->clear();
		QTextCursor cursor(printform->document()), cellCursor;
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.setBlockFormat( blockFormat );
		textCharFormat.setFontPointSize( 13 );
		cursor.insertText(codec->toUnicode("ЗВЕДЕНА ВІДОМІСТЬ ПІЛЬГОВИКІВ"), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 8 );
		cursor.insertText(codec->toUnicode(" "), textCharFormat);
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 11 );
		cursor.insertText(codec->toUnicode("за ") + monthlist.at(month-1) +
							codec->toUnicode(" ") + QVariant(year).toString() + codec->toUnicode("р."), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText(codec->toUnicode("по абонентах ")+OrganizName()+"\n");
		textCharFormat.setFontPointSize( 8 );
		cursor.insertText(codec->toUnicode(" "), textCharFormat);
			//створення і налаштування таблиці
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
			//заповнення шапки таблиці
		cell = table->cellAt(0, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("№ п/п"));
	
		cell = table->cellAt(0, 1);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Категорія пільги"));
	
		cell = table->cellAt(0, 2);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Відсоток пільги, %"));
		
		cell = table->cellAt(0, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Пільга на площу, м.кв."));
		
		cell = table->cellAt(0, 4);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Сума пільги, грн"));
			//налаштування кількості стрічок
		QSqlQuery query, query2;
		query.exec("SELECT count(*) FROM pilgy");
		query.next();
		table->insertRows( 2, query.value(0).toInt() );	
		//виведення форми поступу
		UPostupForm *postup_form = new UPostupForm(0, query.value(0).toInt() );
		postup_form->show();
			//заповнення стрічок інформацією
		query.exec("SELECT id, Kategoriya, Vidsotok, Ploshsca FROM pilgy ORDER BY Kategoriya");
		int iter=0;
		double zved_pilga=0;
		while (query.next()){
			// № п/п
			cell = table->cellAt(1+iter, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( QVariant(iter+1).toString() );
			// Категорія пільги
			cell = table->cellAt(1+iter, 1);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( query.value(1).toString() );
			// Відсоток пільги
			cell = table->cellAt(1+iter, 2);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( query.value(2).toString() );
			// Пільга на площу
			cell = table->cellAt(1+iter, 3);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			if (query.value(3).toDouble() != 0)
			cellCursor.insertText( query.value(3).toString() );
			
			//Обрахунок зведеної суми пільги для кожної з категорій
			query2.exec("SELECT sum(Pilga_sum) FROM narahuvannya \
							WHERE month="+QVariant(month).toString()+" \
							and year="+QVariant(year).toString()+" \
							and Pilga_ID="+query.value(0).toString());
			query2.next();
			double pilga = query2.value(0).toDouble();
			zved_pilga += pilga;
			// Сума пільги
			cell = table->cellAt(1+iter, 4);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(pilga) );
			iter ++;
			postup_form->inc_pos();
		}
		// Налаштування вигляду і вмісту останньої стрічки
		table->mergeCells ( 1+iter, 0, 1, 4 );
		//вивід надпису ->зведена пільга
		cell = table->cellAt(1+iter, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( codec->toUnicode("Загальна сума, грн.") );
		//вивід значення зведеної пільги
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

			//Кінець створення документу для друку
		postup_form->close();
		printform->printer()->setDocName("Відомість пільговиків по категоріях за "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"р.");
		printform->show();
}
//------------------------------------------------------------
void UPrintDocs::print_vidomist_pilg_kategor(int cur_year, int cur_month)
{
			//Ініціалізація форми поступу
		QSqlQuery query, query_2;
		query.exec("SELECT count(*) FROM pilgy");
		query.next();
		UPostupForm *postup_form = new UPostupForm(0, query.value(0).toInt() );
		postup_form->show();
			//Створення форми
		int year = cur_year;
		int month = cur_month;
		printform = new UPrintForm(0, codec->toUnicode("Відомість пільговиків по категоріях"));
		
		//*****************************************
		//Створення зведеної таблиці по категоріях пільг кількість абонентів + сума пільги в категорії
		//*****************************************
		
		QTextBlockFormat blockFormat;
		QTextTableCell cell;
		QTextCharFormat textCharFormat;
		//Створення документа тарифів для всіх будинків за X місяць, Y рік
		printform->document()->clear();
		QTextCursor cursor(printform->document()), cellCursor;
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.setBlockFormat( blockFormat );
		textCharFormat.setFontPointSize( 13 );
		cursor.insertText(codec->toUnicode("ЗВЕДЕНА ВІДОМІСТЬ ПІЛЬГОВИКІВ ПО КАТЕГОРІЯХ"), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 8 );
		cursor.insertText(codec->toUnicode(" "), textCharFormat);
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 11 );
		cursor.insertText(codec->toUnicode("за ") + monthlist.at(month-1) +
							codec->toUnicode(" ") + QVariant(year).toString() + codec->toUnicode("р."), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText(codec->toUnicode("по абонентах ")+OrganizName()+"\n");

			//Створення зведеної таблиці по кількості пільговиків в кожній категорії
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
			//заповнення шапки таблиці
		cell = table->cellAt(0, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("№ п/п"));
	
		cell = table->cellAt(0, 1);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Категорія пільги"));
	
		cell = table->cellAt(0, 2);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Кількість абонентів"));
		
		cell = table->cellAt(0, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Сума по категорії"));

			//налаштування кількості стрічок
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
			// № п/п
			cell = table->cellAt(1+iter, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( QVariant(iter+1).toString() );
			// Категорія пільги
			cell = table->cellAt(1+iter, 1);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( query.value(1).toString() );
			// Кількість абонентів
			cell = table->cellAt(1+iter, 2);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( query.value(2).toString() );
			// Сума по категорії
			cell = table->cellAt(1+iter, 3);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			zved_pilga += query.value(3).toDouble();
			cellCursor.insertText( uMToStr2(query.value(3).toDouble()) );
			
			iter ++;
		}
		//Остання стрічка таблиці (сума)
		table->mergeCells ( 1+iter, 0, 1, 3 );
		//вивід надпису ->зведена пільга
		cell = table->cellAt(1+iter, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( codec->toUnicode("Загальна сума, грн.") );
		//вивід значення зведеної пільги
		cell = table->cellAt(1+iter, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( uMToStr2(zved_pilga) );
		
			//Створення закінчення для підпису
		cursor.insertText(codec->toUnicode(" "), textCharFormat);
		cursor.movePosition(QTextCursor::End);
		blockFormat.setAlignment( Qt::AlignLeft );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText(codec->toUnicode("     Економіст					")+PrizvIB(3));
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		
		//*****************************************
		//Створення документів з абонентами-пільговиками
		//*****************************************
		
		query.first();
		do{
			//Виведення заголовка документа
			blockFormat.setAlignment( Qt::AlignHCenter );
			cursor.setBlockFormat( blockFormat );
			textCharFormat.setFontPointSize( 13 );
			cursor.insertText(codec->toUnicode("Пільговики категорії - ")+query.value(1).toString(), textCharFormat);
			blockFormat.setAlignment( Qt::AlignHCenter );
			cursor.insertBlock(blockFormat);
			textCharFormat.setFontPointSize( 8 );
			cursor.insertText(codec->toUnicode(" "), textCharFormat);
			cursor.insertBlock(blockFormat);
			textCharFormat.setFontPointSize( 11 );
			cursor.insertText(codec->toUnicode("за ") + monthlist.at(month-1) +
							codec->toUnicode(" ") + QVariant(year).toString() + codec->toUnicode("р."), textCharFormat);
			blockFormat.setAlignment( Qt::AlignHCenter );
			cursor.insertBlock(blockFormat);
			textCharFormat.setFontPointSize( 8 );
		
		
			//Створення кістяка таблиці
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
				//заповнення шапки таблиці
			cell = table_2->cellAt(0, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(codec->toUnicode("№ п/п"));
	
			cell = table_2->cellAt(0, 1);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(codec->toUnicode("Рахунок"));
	
			cell = table_2->cellAt(0, 2);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(codec->toUnicode("П І Б"));
		
			cell = table_2->cellAt(0, 3);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(codec->toUnicode("Домашня адреса"));
		
			cell = table_2->cellAt(0, 4);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(codec->toUnicode("Сума пільги"));
			
				//налаштування кількості стрічок
			table_2->insertRows( 2, query.value(2).toInt() );

			query_2.exec("SELECT narahuvannya.Rahunok, abonent.Prizv & ' ' & abonent.Imya & ' ' & abonent.Batk, \
								budynky.Vulycia & ' ' & budynky.Bud_num & ' кв.' & abonent.Kvartyra, narahuvannya.Pilga_sum \
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
				// № п/п
				cell = table_2->cellAt(1+iter, 0);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignRight );
				cellCursor.setBlockFormat( blockFormat );
				cellCursor.insertText( QVariant(iter+1).toString() );
				// Рахунок
				cell = table_2->cellAt(1+iter, 1);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				cellCursor.insertText( query_2.value(0).toString() );
				// ПІБ пільговика, а в разі відсутності - абонента
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
				// Адреса
				cell = table_2->cellAt(1+iter, 3);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				cellCursor.insertText( query_2.value(2).toString() );
				// Сума пільги
				cell = table_2->cellAt(1+iter, 4);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				pilgy_kateg_sum +=query_2.value(3).toDouble();
				cellCursor.insertText( uMToStr2(query_2.value(3).toDouble()) );
				
				iter ++;
			}
			//Остання стрічка таблиці (сума)
			table_2->mergeCells ( 1+iter, 0, 1, 4 );
			//вивід надпису ->зведена пільга
			cell = table_2->cellAt(1+iter, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Загальна сума, грн.") );
			//вивід значення зведеної пільги
			cell = table_2->cellAt(1+iter, 4);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(pilgy_kateg_sum) );
			//Створення закінчення для підпису
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
		
			//Кінець створення документу для друку
		postup_form->close();
		printform->printer()->setDocName("Зведена відомість пільговиків по категоріях за "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"р.");
		printform->show();
}
//------------------------------------------------------------
void UPrintDocs::print_zvit_pilg_kategor(int cur_year, int cur_month)
{
			//Ініціалізація форми поступу
	QSqlQuery query, query_2, query_vidsotok, query_pilga_for;
	query.exec("SELECT count(*) FROM pilgy");
	query.next();
	UPostupForm *postup_form = new UPostupForm(0, query.value(0).toInt() );
	postup_form->show();
		//Створення форми
	int year = cur_year;
	int month = cur_month;
	printform = new UPrintForm(0, codec->toUnicode("Відомість пільговиків по категоріях"));
	int iter = 0;
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat;
	//Створення документа тарифів для всіх будинків за X місяць, Y рік Форма №2
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 12 );
	cursor.insertText(codec->toUnicode("Форма № 2-пільга"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("Затверджена наказом Мінпраці України"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("від 21.04.2003 № 100"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("за погодженням з Держкомстатом"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 12 );
	cursor.insertText(OrganizName(), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("КОД ЄДРПОУ ")+OrganizEDRPOU(), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 13 );
	cursor.insertText(codec->toUnicode("Р О З Р А Х У Н О К"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("витрат на відшкодування збитків, пов'язаних з наданням пільг"), textCharFormat);
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("за ") + monthlist.at(month-1) +
						codec->toUnicode(" ") + QVariant(year).toString() + codec->toUnicode("р."), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
						
	//*****************************************
	//Створення документів з абонентами-пільговиками
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
		//Виведення заголовка документа
		blockFormat.setAlignment( Qt::AlignLeft );
		cursor.setBlockFormat( blockFormat );
		textCharFormat.setFontPointSize( 11 );
		cursor.insertText(codec->toUnicode("Пільговики категорії - ")+query.value(1).toString(), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 8 );
		cursor.insertText(codec->toUnicode(" "), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 8 );
	
	
		//Створення кістяка таблиці
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
			//заповнення шапки таблиці
		cell = table_2->cellAt(0, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("№ п/п"));
		cell = table_2->cellAt(0, 1);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("П І Б"));
		
		cell = table_2->cellAt(0, 2);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Адреса"));
	
		cell = table_2->cellAt(0, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Ідентиф."));
	
		cell = table_2->cellAt(0, 4);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Кільк. осіб"));
		
		cell = table_2->cellAt(0, 5);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Розмір пільги"));
	
		cell = table_2->cellAt(0, 6);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Рах.пільговика"));
		
		cell = table_2->cellAt(0, 7);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Теплопостачання"));
			//налаштування кількості стрічок
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
			// № п/п
			cell = table_2->cellAt(1+iter, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( QVariant(iter+1).toString() );
				// ПІБ пільговика, а в разі відсутності - абонента
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
				// Ідентифікаційний код при наявності пільговика
				cell = table_2->cellAt(1+iter, 3);
				cellCursor = cell.lastCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				if (pilgovyky_e)
					cellCursor.insertText( "\n" );
				cellCursor.insertText( pilgovykyQuery.value(3).toString() );
				
					//Запам'ятовування групи пільговика для майбутного використання прививеденні інформації про групи
				curGrupaPilgovyka = pilgovykyQuery.value(4).toInt();
				
				pilgovyky_e=true;
			}
			if ( !pilgovyky_e ){
				cell = table_2->cellAt(1+iter, 1);
				cellCursor = cell.lastCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				cellCursor.insertText( query_2.value(1).toString() );
				// Ідентифікаційний код при відсутності пільговика
				cell = table_2->cellAt(1+iter, 3);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				cellCursor.insertText( query_2.value(4).toString() );
			}
			// Адреса
			cell = table_2->cellAt(1+iter, 2);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( query_2.value(2).toString() );
			// Кількість осіб, що отр. пільги
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
			// Розимір пільги
			cell = table_2->cellAt(1+iter, 5);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( query_vidsotok.value(0).toString() );
			// Рахунок
			cell = table_2->cellAt(1+iter, 6);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( query_2.value(0).toString() );
			// Сума пільги
			cell = table_2->cellAt(1+iter, 7);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			pilgy_kateg_sum +=query_2.value(3).toDouble();
			cellCursor.insertText( uMToStr2(query_2.value(3).toDouble()) );
				//При необхідності сортування за групою
			if (curPilgyGrupy > 0){
				if (curGrupaPilgovyka>3)
					curGrupaPilgovyka = 0;
				grupPilgaSum[curGrupaPilgovyka] += query_2.value(3).toDouble();
				grupPilgovykyCount[curGrupaPilgovyka] ++;
			}
			
			iter ++;
		}
		//Остання стрічка таблиці (сума)
		table_2->mergeCells ( 1+iter, 0, 1, 7 );
		//вивід надпису ->зведена пільга
		cell = table_2->cellAt(1+iter, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( codec->toUnicode("Загальна сума, грн.") );
		//вивід значення зведеної пільги
		cell = table_2->cellAt(1+iter, 7);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( uMToStr2(pilgy_kateg_sum) );
		//При необхідності сортування за групою
		if (curPilgyGrupy > 0){
			table_2->insertRows( 2+iter, 4 );
			table_2->mergeCells ( 2+iter, 0, 1, 6 );
			table_2->mergeCells ( 3+iter, 0, 1, 6 );
			table_2->mergeCells ( 4+iter, 0, 1, 6 );
			table_2->mergeCells ( 5+iter, 0, 1, 6 );
			
			//вивід надпису ->Група 1
			cell = table_2->cellAt(2+iter, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Група 1") );
			//вивід значення кількості пільговиків у групі
			cell = table_2->cellAt(2+iter, 6);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( QVariant(grupPilgovykyCount[1]).toString() + codec->toUnicode(" чол.") );
			//вивід значення суми пільг для категорії
			cell = table_2->cellAt(2+iter, 7);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(grupPilgaSum[1]) + codec->toUnicode(" грн.") );
			
			//вивід надпису ->Група 2
			cell = table_2->cellAt(3+iter, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Група 2") );
			//вивід значення кількості пільговиків у групі
			cell = table_2->cellAt(3+iter, 6);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( QVariant(grupPilgovykyCount[2]).toString() + codec->toUnicode(" чол.") );
			//вивід значення суми пільг для категорії
			cell = table_2->cellAt(3+iter, 7);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(grupPilgaSum[2]) + codec->toUnicode(" грн.") );
			
			//вивід надпису ->Група 3
			cell = table_2->cellAt(4+iter, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Група 3") );
			//вивід значення кількості пільговиків у групі
			cell = table_2->cellAt(4+iter, 6);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( QVariant(grupPilgovykyCount[3]).toString() + codec->toUnicode(" чол.") );
			//вивід значення суми пільг для категорії
			cell = table_2->cellAt(4+iter, 7);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(grupPilgaSum[3]) + codec->toUnicode(" грн.") );
			
			//вивід надпису ->Без групи
			cell = table_2->cellAt(5+iter, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Без групи") );
			//вивід значення кількості пільговиків у групі
			cell = table_2->cellAt(5+iter, 6);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( QVariant(grupPilgovykyCount[0]).toString() + codec->toUnicode(" чол.") );
			//вивід значення суми пільг для категорії
			cell = table_2->cellAt(5+iter, 7);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(grupPilgaSum[0]) + codec->toUnicode(" грн.") );
		}
		
		cursor.movePosition(QTextCursor::End);
		blockFormat.setAlignment( Qt::AlignLeft );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		
		postup_form->inc_pos();
	} while (query.next());
	
	//Створення закінчення для підпису
	textCharFormat.setFontPointSize( 12 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("	Директор ")+OrganizName()+"					"+PrizvIB(1));
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("    М.П."));
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("	Гол.Бухгалтер")+"								 "+PrizvIB(2));
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("Печатка"));
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("№ розрахункового рахунку"));
		//Кінець створення документу для друку
	postup_form->close();
	printform->printer()->setOrientation( QPrinter::Landscape );
	printform->printer()->setDocName("Форма №2-пільга (2003р.) за "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"р.");
	printform->show();	
}
//------------------------------------------------------------
void UPrintDocs::print_vidomist_narah(int cur_year, int cur_month)
{
		int year = cur_year;
		int month = cur_month;
		printform = new UPrintForm(0, codec->toUnicode("Зведена відомість нарахування оплати за ЦО"));
		QTextBlockFormat blockFormat;
		QTextTableCell cell;
		QTextCharFormat textCharFormat;
		//Створення документа нарахування оплати за ЦО за X місяць, Y рік
		printform->document()->clear();
		QTextCursor cursor(printform->document()), cellCursor;
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.setBlockFormat( blockFormat );
		textCharFormat.setFontPointSize( 13 );
		cursor.insertText(codec->toUnicode("ЗВЕДЕНА ВІДОМІСТЬ"), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		cursor.insertText(codec->toUnicode("нарахування оплати за центральне опалення"), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 8 );
		cursor.insertText(codec->toUnicode(" "), textCharFormat);
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 11 );
		cursor.insertText(codec->toUnicode("за ") + monthlist.at(month-1) +
							codec->toUnicode(" ") + QVariant(year).toString() + codec->toUnicode("р."), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText(codec->toUnicode("по абонентах ")+OrganizName()+"\n");
		textCharFormat.setFontPointSize( 8 );
		cursor.insertText(codec->toUnicode(" "), textCharFormat);
			//створення і налаштування таблиці
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
			//заповнення шапки таблиці
		cell = table->cellAt(0, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("№ п/п"));
	
		cell = table->cellAt(0, 1);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Категорія пільги"));
		
		cell = table->cellAt(0, 2);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Нарахування"));
			//налаштування кількості стрічок
		QSqlQuery query, query2;
		query.exec("SELECT count(*) FROM (SELECT Pilga_ID FROM narahuvannya \
					WHERE (year="+QVariant(year).toString()+") \
					and (month="+QVariant(month).toString()+") GROUP BY Pilga_ID)");
		query.next();
		table->insertRows( 2, query.value(0).toInt()+1 );	
		//виведення форми поступу
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
			// № п/п
			cell = table->cellAt(1+iter, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( QVariant(iter+1).toString() );
			// Категорія
			query2.exec("SELECT Kategoriya FROM pilgy WHERE id=" + query.value(0).toString());
			query2.next();
			cell = table->cellAt(1+iter, 1);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			if (query.value(0).toInt() !=0 )
				cellCursor.insertText( query2.value(0).toString() );
			else
				cellCursor.insertText( codec->toUnicode("Без пільги") );
			// Нараховано
			cell = table->cellAt(1+iter, 2);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(query.value(1).toDouble()) );
			pilgy_kateg_sum += query.value(1).toDouble();
			
			postup_form->inc_pos();
			iter ++;
		}
		//Остання стрічка таблиці (сума)
		table->mergeCells ( 1+iter, 0, 1, 2 );
		//вивід надпису ->загальна сума
		cell = table->cellAt(1+iter, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( codec->toUnicode("Загальна сума, грн.") );
		//вивід значення зведеної суми нарахування за місяць
		cell = table->cellAt(1+iter, 2);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( uMToStr2(pilgy_kateg_sum) );
		table->mergeCells ( 2+iter, 0, 1, 2 );
		//вивід надпису ->загальна площа
		cell = table->cellAt(2+iter, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( codec->toUnicode("Загальна площа опалення, м.кв.") );
		//вивід значення зведеної площі опалення
		query2.exec("SELECT SUM(Ploshcha) FROM abonent WHERE opal=true");
		query2.next();
		cell = table->cellAt(2+iter, 2);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( uMToStr2(query2.value(0).toDouble()) );
		//Створення закінчення для підпису
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
		printform->printer()->setDocName("Відомість нарахування за "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"р.");
		printform->show();
}
//------------------------------------------------------------
void UPrintDocs::print_dovidka_oderj_subs(int cur_year, int cur_month)
{
		int year = cur_year;
		int month = cur_month;
		printform = new UPrintForm(0, codec->toUnicode("Довідка про суму одержання субсидій"));
		QTextBlockFormat blockFormat;
		QTextTableCell cell;
		QTextCharFormat textCharFormat;
		//Створення документа нарахування оплати за ЦО за X місяць, Y рік
		printform->document()->clear();
		QTextCursor cursor(printform->document()), cellCursor;
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.setBlockFormat( blockFormat );
		textCharFormat.setFontPointSize( 14 );
		cursor.insertText(codec->toUnicode("ДОВІДКА"), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		cursor.insertText(codec->toUnicode("про суму одержаних субсидій"), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 8 );
		cursor.insertText(codec->toUnicode(" "), textCharFormat);
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 11 );
		cursor.insertText(codec->toUnicode("за ") + monthlist.at(month-1) +
							codec->toUnicode(" ") + QVariant(year).toString() + codec->toUnicode("р."), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText(codec->toUnicode("по абонентах ")+OrganizName()+"\n");
		textCharFormat.setFontPointSize( 8 );
		cursor.insertText(codec->toUnicode(" "), textCharFormat);
		
		// Дані довідки
		QSqlQuery query;
		query.exec("SELECT SUM(Suma), SUM(Suma_d), SUM(Suma_GV), SUM(Suma_d_GV) FROM subs \
					WHERE (year="+QVariant(year).toString()+") \
					and (month="+QVariant(month).toString()+")");
		query.next();
		//Нараховано субсидії ЦО
		blockFormat.setAlignment( Qt::AlignLeft );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 12 );
		cursor.insertText(codec->toUnicode("    Нараховано субсидії	ЦО		") + uMToStr2(query.value(0).toDouble()) 
												+ codec->toUnicode(" грн"), textCharFormat);
		//Донараховано субсидії ЦО
		blockFormat.setAlignment( Qt::AlignLeft );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 12 );
		cursor.insertText(codec->toUnicode("    Донараховано субсидії ЦО			") + uMToStr2(query.value(1).toDouble()) 
												+ codec->toUnicode(" грн"), textCharFormat);
		//Нараховано субсидії ГВ
		blockFormat.setAlignment( Qt::AlignLeft );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 12 );
		cursor.insertText(codec->toUnicode("    Нараховано субсидії	ГВ		") + uMToStr2(query.value(2).toDouble()) 
												+ codec->toUnicode(" грн"), textCharFormat);
		//Донараховано субсидії ГВ
		blockFormat.setAlignment( Qt::AlignLeft );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 12 );
		cursor.insertText(codec->toUnicode("    Донараховано субсидії ГВ			") + uMToStr2(query.value(3).toDouble()) 
												+ codec->toUnicode(" грн"), textCharFormat);
		//Всього субсидії
		double suma = query.value(0).toDouble() + query.value(1).toDouble() + query.value(2).toDouble() + query.value(3).toDouble();
		blockFormat.setAlignment( Qt::AlignLeft );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 12 );
		cursor.insertText(codec->toUnicode("    Всього по підприємству			") + uMToStr2(suma) 
												+ codec->toUnicode(" грн"), textCharFormat);
		
		//Створення закінчення для підпису
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
		
		printform->printer()->setDocName("Довідка про суму одержаної субсидії за "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"р.");
		printform->show();
}
//------------------------------------------------------------
void UPrintDocs::print_forma_akt_vytrat_teploenergii()
{
	printform = new UPrintForm(0, codec->toUnicode("АКТ про витрату теплоенергії"));
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat;
	//Створення документа тарифів для всіх будинків за X місяць, Y рік
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 13 );
	cursor.insertText(codec->toUnicode("А К Т"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 8 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("Про витрату теплоенергії по ______________ ")+OrganizCity(), textCharFormat);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("					Місяць ") + monthlist.at(date.month()-1)
						+ codec->toUnicode(" ") + QVariant(date.year()).toString() + codec->toUnicode("р.\n"));
	textCharFormat.setFontPointSize( 8 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
		//створення і налаштування таблиці
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
		//заповнення шапки таблиці
	cell = table->cellAt(0, 0);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("№ п/п"));
	
	cell = table->cellAt(0, 1);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Назва об'єкта"));
	
	cell = table->cellAt(0, 2);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Показники лічильника"));
	
	cell = table->cellAt(0, 5);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("К-сть т.е. в Гкал."));
	
	cell = table->cellAt(0, 6);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Тариф"));
	
	cell = table->cellAt(0, 7);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Сума, грн."));
	
	cell = table->cellAt(1, 2);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("кін. місяця"));
	
	cell = table->cellAt(1, 3);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("поч. місяця"));
	
	cell = table->cellAt(1, 4);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("різниця"));
	
	
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("     Відповідальний \n     за розрахунок"));
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("     Представник\n     ")+OrganizName());
	
	//Кінець створення документу для друку
	printform->printer()->setDocName("АКТ витрат теплоенергії (форма)");
	printform->show();
}
//------------------------------------------------------------
void UPrintDocs::print_diuch_taryf(int cur_year, int cur_month)
{
	printform = new UPrintForm(0, codec->toUnicode("АКТ про витрату теплоенергії"));
	short diuchTaryfPrecision = tochnistDiuchogoTaryfu();
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat;
	//Створення документа тарифів для всіх будинків за X місяць, Y рік
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 13 );
	cursor.insertText(codec->toUnicode("А К Т"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 8 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("Про витрату теплоенергії по ______________ ")+OrganizCity(), textCharFormat);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("					Місяць ") + monthlist.at(cur_month-1)
						+ codec->toUnicode(" ") + QVariant(cur_year).toString() + codec->toUnicode("р.\n"));
	textCharFormat.setFontPointSize( 8 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
		//створення і налаштування таблиці
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
		//заповнення шапки таблиці
	cell = table->cellAt(0, 0);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("№ п/п"));
	
	cell = table->cellAt(0, 1);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Назва об'єкта"));
	
	cell = table->cellAt(0, 2);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Показники лічильника"));
	
	cell = table->cellAt(0, 5);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("К-сть т.е. в Гкал."));
	
	cell = table->cellAt(0, 6);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Тариф"));
	
	cell = table->cellAt(0, 7);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Сума, грн."));
	
	cell = table->cellAt(1, 2);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("кін. місяця"));
	
	cell = table->cellAt(1, 3);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("поч. місяця"));
	
	cell = table->cellAt(1, 4);
    cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("різниця"));
	
		//налаштування кількості стрічок
	QSqlQuery query;
	query.exec("SELECT count(*) FROM budynky");
	//query.exec("SELECT count(*) FROM budynky WHERE Lichylnyk <> 0");
	query.next();
	table->insertRows( 3, query.value(0).toInt());
	
		//заповнення таблиці даними про тарифи для будинків
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
		// № п/п
		cell = table->cellAt(2+i, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( QVariant(i+1).toString() );
		// Назва об'єкта
		cell = table->cellAt(2+i, 1);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( query.value(0).toString() );
		// Показник лічильника -> кінець місяця
		cell = table->cellAt(2+i, 2);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( query.value(3).toString() );
		// Показник лічильника -> початок місяця
		cell = table->cellAt(2+i, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( query.value(2).toString() );
		// Показник лічильника -> різниця
		cell = table->cellAt(2+i, 4);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		pokaznLichRizn = query.value(4).toDouble();
		cellCursor.insertText( QString::number(pokaznLichRizn) );
		
		// Коефіцієнт переведення
		cell = table->cellAt(2+i, 5);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		koef = query.value(1).toDouble();
		cellCursor.insertText( QString::number(koef) );
		
		if (koef > 0)
			gKalSum += pokaznLichRizn/koef;
		
		// Тариф
		cell = table->cellAt(2+i, 6);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( QString::number(query.value(5).toDouble(),'f', diuchTaryfPrecision).replace('.',',') );
		// Сума
		cell = table->cellAt(2+i, 7);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		dVal = query.value(6).toDouble();
		narahSum += dVal;
		cellCursor.insertText( uMToStr2(dVal) );
		
		i++;
	}
		//підсумок
	
	table->mergeCells ( 2+i, 0, 1, 4 );
	// № п/п
	cell = table->cellAt(2+i, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText( "Всього" );
	// Показник лічильника -> різниця
	cell = table->cellAt(2+i, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText( QString::number(gKalSum)+" ГКал" );
	// Коефіцієнт переведення
	cell = table->cellAt(2+i, 5);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText( "-" );
	// Тариф
	cell = table->cellAt(2+i, 6);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText( "-" );
	// Сума
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
	cursor.insertText(codec->toUnicode("     Відповідальний\n     за розрахунок"));
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("     Представник\n     ")+OrganizName());
	
	//Кінець створення документу для друку
	printform->printer()->setDocName("АКТ витрат теплоенергії за "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"р.");
	printform->show();
}
//------------------------------------------------------------
void UPrintDocs::print_vidomist_borjnykiv(QDate pochDate, QDate kincDate, double minSum, int BudId, unsigned char abonType, bool isPereplata)
{
    pochDate.setYMD(pochDate.year(), pochDate.month(), 1);
    kincDate.setYMD(kincDate.year(), kincDate.month(), kincDate.daysInMonth());
    //Заголовок документа
    printform = new UPrintForm(0,codec->toUnicode((isPereplata)?"Відомість по переплаті":"Відомість боржників"));
    QTextBlockFormat blockFormat;
    QTextTableCell cell;
    QTextCharFormat textCharFormat;
    printform->document()->clear();
    QTextCursor cursor(printform->document()), cellCursor;
    blockFormat.setAlignment( Qt::AlignHCenter );
    cursor.setBlockFormat( blockFormat );
    textCharFormat.setFontPointSize( 13 );
    cursor.insertText(codec->toUnicode("ВІДОМІСТЬ"), textCharFormat);
    blockFormat.setAlignment( Qt::AlignHCenter );
    cursor.insertBlock(blockFormat);
    textCharFormat.setFontPointSize( 12 );
    if (abonType == 0)
        cursor.insertText("УСІХ ", textCharFormat);
    else if (abonType == 1)
        cursor.insertText("під'єднаних до ЦО ", textCharFormat);
    else if (abonType == 2)
        cursor.insertText("від'єднаних від ЦО ", textCharFormat);
    QString informStr;
    if (isPereplata)
        informStr = "абонентів, переплата яких перевищує ";
    else
        informStr = "абонентів, борг яких перевищує ";
        cursor.insertText( informStr +
                        uMToStr2(minSum) +
                        codec->toUnicode(" грн."), textCharFormat);
    blockFormat.setAlignment( Qt::AlignRight );
    cursor.insertBlock(blockFormat);
    textCharFormat.setFontPointSize( 9 );
    cursor.insertText(codec->toUnicode("Аналізований інтервал місяців: ") +
                        QVariant(pochDate.month()).toString() + "." +
                        QVariant(pochDate.year()).toString() + "." +
                        codec->toUnicode("р. - ") +
                        QVariant(kincDate.month()).toString() + "." +
                        QVariant(kincDate.year()).toString() + "." +
                        codec->toUnicode("р."), textCharFormat);
    cursor.insertBlock(blockFormat);
    blockFormat.setAlignment( Qt::AlignCenter );
    cursor.insertBlock(blockFormat);
    textCharFormat.setFontPointSize( 12 );
    cursor.insertText("ЗВЕДЕНА ТАБЛИЦЯ ПО БУДИНКАХ", textCharFormat);
    cursor.insertBlock(blockFormat);
    cursor.insertBlock(blockFormat);
    textCharFormat.setFontPointSize( 9 );

    //Налаштування таблиці
    QTextTable *table = NULL;
    QTextTableFormat tableFormat;
    QVector<QTextLength> constraints;

    //Зведена Таблиця боргу по будинках
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
    tableCell.set(0,0,"№ з/п");
    tableCell.set(0,1,"Будинок");
    tableCell.set(0,2,(isPereplata)?"К-сть переплатників":"К-сть боржників");
    tableCell.set(0,3,(isPereplata)?"Сума переплати, грн":"Сума боргу, грн.");

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
    cursor.insertText((isPereplata)?"ПЕРЕЛІК АБОНЕНТІВ - ПЕРЕПЛАТНИКІВ":"ПЕРЕЛІК АБОНЕНТІВ - БОРЖНИКІВ", textCharFormat);
    blockFormat.setAlignment( Qt::AlignLeft );
    cursor.insertBlock(blockFormat);
    textCharFormat.setFontPointSize( 9 );


    //Таблиця боржників по будинках
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

    //Запит з фільтром абонентів по будинках
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
        //Обрахунок поточних показників
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


            //Створення нового заголовка і таблиці для нового будинку
            if (old_bud != abonQuery->value(1).toInt()){
                cursor.movePosition(QTextCursor::End);
                blockFormat.setAlignment( Qt::AlignLeft );
                cursor.insertBlock(blockFormat);
                textCharFormat.setFontPointSize( 9 );
                budQuery->exec("SELECT Vulycia & ' ' & Bud_num FROM budynky WHERE id="+abonQuery->value(1).toString());
                budQuery->seek(0);

                //Створення шапки таблиці боржників для поточного будинку
                table = cursor.insertTable(2, 12, tableFormat);
                table->mergeCells ( 0, 0, 1, 12);

                //заповнення шапки таблиці
                cell = table->cellAt(0, 0);
                cellCursor = cell.firstCursorPosition();
                blockFormat.setAlignment( Qt::AlignCenter );
                cellCursor.setBlockFormat( blockFormat );
                cellCursor.insertText(codec->toUnicode("Будинок ") + budQuery->value(0).toString(), textCharFormat);

                cell = table->cellAt(1, 0);
                cellCursor = cell.firstCursorPosition();
                blockFormat.setAlignment( Qt::AlignCenter );
                cellCursor.setBlockFormat( blockFormat );
                cellCursor.insertText(codec->toUnicode("№ п/п"), textCharFormat);

                cell = table->cellAt(1, 1);
                cellCursor = cell.firstCursorPosition();
                blockFormat.setAlignment( Qt::AlignCenter );
                cellCursor.setBlockFormat( blockFormat );
                cellCursor.insertText(codec->toUnicode("Рахунок"), textCharFormat);

                cell = table->cellAt(1, 2);
                cellCursor = cell.firstCursorPosition();
                blockFormat.setAlignment( Qt::AlignCenter );
                cellCursor.setBlockFormat( blockFormat );
                cellCursor.insertText(codec->toUnicode("№ тел."), textCharFormat);

                cell = table->cellAt(1, 3);
                cellCursor = cell.firstCursorPosition();
                blockFormat.setAlignment( Qt::AlignCenter );
                cellCursor.setBlockFormat( blockFormat );
                cellCursor.insertText(codec->toUnicode("П І Б"), textCharFormat);

                cell = table->cellAt(1, 4);
                cellCursor = cell.firstCursorPosition();
                blockFormat.setAlignment( Qt::AlignCenter );
                cellCursor.setBlockFormat( blockFormat );
                cellCursor.insertText(codec->toUnicode("Кварт."), textCharFormat);

                cell = table->cellAt(1, 5);
                cellCursor = cell.firstCursorPosition();
                blockFormat.setAlignment( Qt::AlignCenter );
                cellCursor.setBlockFormat( blockFormat );
                cellCursor.insertText(codec->toUnicode("Вх. сальдо"), textCharFormat);

                cell = table->cellAt(1, 6);
                cellCursor = cell.firstCursorPosition();
                blockFormat.setAlignment( Qt::AlignCenter );
                cellCursor.setBlockFormat( blockFormat );
                cellCursor.insertText(codec->toUnicode("Нарах."), textCharFormat);

                cell = table->cellAt(1, 7);
                cellCursor = cell.firstCursorPosition();
                blockFormat.setAlignment( Qt::AlignCenter );
                cellCursor.setBlockFormat( blockFormat );
                cellCursor.insertText(codec->toUnicode("Субс."), textCharFormat);

                cell = table->cellAt(1, 8);
                cellCursor = cell.firstCursorPosition();
                blockFormat.setAlignment( Qt::AlignCenter );
                cellCursor.setBlockFormat( blockFormat );
                cellCursor.insertText(codec->toUnicode("Оплач."), textCharFormat);

                cell = table->cellAt(1, 9);
                cellCursor = cell.firstCursorPosition();
                blockFormat.setAlignment( Qt::AlignCenter );
                cellCursor.setBlockFormat( blockFormat );
                cellCursor.insertText(codec->toUnicode("Перев. заборг. + Дебіт."), textCharFormat);

                cell = table->cellAt(1, 10);
                cellCursor = cell.firstCursorPosition();
                blockFormat.setAlignment( Qt::AlignCenter );
                cellCursor.setBlockFormat( blockFormat );
                cellCursor.insertText(codec->toUnicode("Вих. сальдо"), textCharFormat);

                cell = table->cellAt(1, 11);
                cellCursor = cell.firstCursorPosition();
                blockFormat.setAlignment( Qt::AlignCenter );
                cellCursor.setBlockFormat( blockFormat );
                cellCursor.insertText(codec->toUnicode("Ост. нарах."), textCharFormat);
                //Кінець роботи з таблицею

            //Запис даних в зведену таблицю
                sumaryTable->insertRows(sumaryTableRow, 1);


                tableCell.setAlignment(Qt::AlignCenter);
                //"№ з/п"
                tableCell.set(sumaryTableRow,0,sumaryTableRow);
                //"Будинок"
                tableCell.setAlignment(Qt::AlignLeft);
                tableCell.set(sumaryTableRow,1,budQuery->value(0).toString());
                if (old_bud > 0){
                    tableCell.setAlignment(Qt::AlignRight);
                    //"К-сть боржників"
                    tableCell.set(sumaryTableRow-1,2,sumaryTableBorgBudCount);
                    //"Сума боргу, грн."
                    tableCell.set(sumaryTableRow-1,3,(isPereplata) ? sumaryTableBorgBudSum*-1 : sumaryTableBorgBudSum);
                }
                sumaryTableBorgAllCount += sumaryTableBorgBudCount;
                sumaryTableBorgAllSum += sumaryTableBorgBudSum;
                sumaryTableRow++;
                sumaryTableBorgBudCount = 0;
                sumaryTableBorgBudSum = 0;
                //Кінець запису даних в зведену таблицю

                old_bud = abonQuery->value(1).toInt();
                iterBorj = 0;
            }

            table->insertRows( iterBorj+2, 1 );
            //Внесення даних в стрічку
            pokaznQuery->exec("SELECT Prizv, Imya, Batk, Kvartyra, tel FROM abonent \
                                WHERE Rahunok=" + QVariant(Rahunok).toString());
            pokaznQuery->seek(0);
                //№ п/п"
            cell = table->cellAt(iterBorj+2, 0);
            cellCursor = cell.firstCursorPosition();
            blockFormat.setAlignment( Qt::AlignCenter );
            cellCursor.setBlockFormat( blockFormat );
            cellCursor.insertText(QVariant(iterBorj+1).toString(), textCharFormat);
                //Рахунок
            cell = table->cellAt(iterBorj+2, 1);
            cellCursor = cell.firstCursorPosition();
            blockFormat.setAlignment( Qt::AlignCenter );
            cellCursor.setBlockFormat( blockFormat );
            cellCursor.insertText(QVariant(Rahunok).toString(), textCharFormat);
                //Телефон
            cell = table->cellAt(iterBorj+2, 2);
            cellCursor = cell.firstCursorPosition();
            blockFormat.setAlignment( Qt::AlignCenter );
            cellCursor.setBlockFormat( blockFormat );
            cellCursor.insertText(pokaznQuery->value(4).toString());
            //П І Б
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
            //Квартира
            cell = table->cellAt(iterBorj+2, 4);
            cellCursor = cell.firstCursorPosition();
            blockFormat.setAlignment( Qt::AlignCenter );
            cellCursor.setBlockFormat( blockFormat );
            cellCursor.insertText(pokaznQuery->value(3).toString(), textCharFormat);
             //Вх.сальдо
            cell = table->cellAt(iterBorj+2, 5);
            cellCursor = cell.firstCursorPosition();
            blockFormat.setAlignment( Qt::AlignCenter );
            cellCursor.setBlockFormat( blockFormat );
            cellCursor.insertText(uMToStr2(zagVh_Saldo), textCharFormat);
            //Нарахув.
            cell = table->cellAt(iterBorj+2, 6);
            cellCursor = cell.firstCursorPosition();
            blockFormat.setAlignment( Qt::AlignCenter );
            cellCursor.setBlockFormat( blockFormat );
            cellCursor.insertText(uMToStr2(zagNarah + zagNarahGV + zagNarahST), textCharFormat);
              //Субс.
            cell = table->cellAt(iterBorj+2, 7);
            cellCursor = cell.firstCursorPosition();
            blockFormat.setAlignment( Qt::AlignCenter );
            cellCursor.setBlockFormat( blockFormat );
            cellCursor.insertText(uMToStr2(zagSubs), textCharFormat);
            //Оплач.
            cell = table->cellAt(iterBorj+2, 8);
            cellCursor = cell.firstCursorPosition();
            blockFormat.setAlignment( Qt::AlignCenter );
            cellCursor.setBlockFormat( blockFormat );
            cellCursor.insertText(uMToStr2(zagOplata), textCharFormat);
            //Переведення заборгованості
            cell = table->cellAt(iterBorj+2, 9);
            cellCursor = cell.firstCursorPosition();
            blockFormat.setAlignment( Qt::AlignCenter );
            cellCursor.setBlockFormat( blockFormat );
            cellCursor.insertText(uMToStr2(zagPereved + zagDebitorska), textCharFormat);
            //Вих. сальдо
            cell = table->cellAt(iterBorj+2, 10);
            cellCursor = cell.firstCursorPosition();
            blockFormat.setAlignment( Qt::AlignCenter );
            cellCursor.setBlockFormat( blockFormat );
            cellCursor.insertText((isPereplata)? uMToStr2(zagVyh_Saldo*-1) : uMToStr2(zagVyh_Saldo), textCharFormat);
            //Ост.нарах.
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

     //Запис кількості боржників та суми боргу для останнього будинку та запис сумарної кількості боржників та суми боргу
    if (old_bud > 0){
         tableCell.setAlignment(Qt::AlignRight);
         //"К-сть боржників"
        tableCell.set(sumaryTableRow-1,2,sumaryTableBorgBudCount);
        //"Сума боргу, грн."
        tableCell.set(sumaryTableRow-1,3,(isPereplata) ? sumaryTableBorgBudSum*-1 : sumaryTableBorgBudSum);
    }
    sumaryTableBorgAllCount += sumaryTableBorgBudCount;
    sumaryTableBorgAllSum += sumaryTableBorgBudSum;
    sumaryTable->mergeCells(sumaryTableRow,0,1,2);
    tableCell.set(sumaryTableRow,0,"Всього");
    //"К-сть боржників"
    tableCell.set(sumaryTableRow,2,sumaryTableBorgAllCount);
    //"Сума боргу, грн."
    tableCell.set(sumaryTableRow,3,(isPereplata) ? sumaryTableBorgAllSum*-1 : sumaryTableBorgAllSum);


    delete abonQuery;
    delete budQuery;
    delete pokaznQuery;
    delete postup_form;
    printform->printer()->setDocName((isPereplata)? "Відомість переплати" : "Відомість боржників");
    printform->show();
}
//------------------------------------------------------------
void UPrintDocs::print_vidomist_borjnykiv_riznyc(QDate pochDate, QDate kincDate, double minBorg, int BudId)
{
	if (minBorg==0)
		minBorg = 0.1;
	//Заголовок документа
	printform = new UPrintForm(0, codec->toUnicode("Відомість боржників по сумнівному боргу"));
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat;
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 13 );
	cursor.insertText(codec->toUnicode("ВІДОМІСТЬ ПО СУМНВІНОМУ БОРГУ"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 12 );
	cursor.insertText(codec->toUnicode("абонентів, сумнівний борг яких перевищує ") + 
						uMToStr2(minBorg) +
						codec->toUnicode(" грн."), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 9 );
	cursor.insertText(codec->toUnicode("Аналізований інтервал місяців: ") +
						QVariant(pochDate.month()).toString() + "." +
						QVariant(pochDate.year()).toString() + "." +
						codec->toUnicode("р. - ") +
						QVariant(kincDate.month()).toString() + "." +
						QVariant(kincDate.year()).toString() + "." +
						codec->toUnicode("р."), textCharFormat);
	cursor.insertBlock(blockFormat);
	
	//Налаштування таблиці
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
	
	//Запит з фільтром абонентів по будинках
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
		//Обрахунок поточних показників
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
					//Створення нового заголовка і таблиці для нового будинку
				if (old_bud != abonQuery->value(1).toInt()){
					cursor.movePosition(QTextCursor::End);
					blockFormat.setAlignment( Qt::AlignLeft );
					cursor.insertBlock(blockFormat);
					textCharFormat.setFontPointSize( 9 );
					budQuery->exec("SELECT Vulycia & ' ' & Bud_num FROM budynky WHERE id="+abonQuery->value(1).toString());
					budQuery->seek(0);
					
					//Створення шапки таблиці боржників для поточного будинку
					table = cursor.insertTable(2, 6, tableFormat);
					table->mergeCells ( 0, 0, 1, 6);
				
						//заповнення шапки таблиці
					cell = table->cellAt(0, 0);
					cellCursor = cell.firstCursorPosition();
					blockFormat.setAlignment( Qt::AlignCenter );
					cellCursor.setBlockFormat( blockFormat );
					cellCursor.insertText(codec->toUnicode("Будинок ") + budQuery->value(0).toString(), textCharFormat);	
					
					cell = table->cellAt(1, 0);
					cellCursor = cell.firstCursorPosition();
					blockFormat.setAlignment( Qt::AlignCenter );
					cellCursor.setBlockFormat( blockFormat );
					cellCursor.insertText(codec->toUnicode("№ п/п"), textCharFormat);
	
					cell = table->cellAt(1, 1);
					cellCursor = cell.firstCursorPosition();
					blockFormat.setAlignment( Qt::AlignCenter );
					cellCursor.setBlockFormat( blockFormat );
					cellCursor.insertText(codec->toUnicode("Рахунок"), textCharFormat);
	
					cell = table->cellAt(1, 2);
					cellCursor = cell.firstCursorPosition();
					blockFormat.setAlignment( Qt::AlignCenter );
					cellCursor.setBlockFormat( blockFormat );
					cellCursor.insertText(codec->toUnicode("П І Б"), textCharFormat);
	
					cell = table->cellAt(1, 3);
					cellCursor = cell.firstCursorPosition();
					blockFormat.setAlignment( Qt::AlignCenter );
					cellCursor.setBlockFormat( blockFormat );
					cellCursor.insertText(codec->toUnicode("Кварт."), textCharFormat);
	
					cell = table->cellAt(1, 4);
					cellCursor = cell.firstCursorPosition();
					blockFormat.setAlignment( Qt::AlignCenter );
					cellCursor.setBlockFormat( blockFormat );
					cellCursor.insertText(codec->toUnicode("Сумнівн. борг"), textCharFormat);
	
					cell = table->cellAt(1, 5);
					cellCursor = cell.firstCursorPosition();
					blockFormat.setAlignment( Qt::AlignCenter );
					cellCursor.setBlockFormat( blockFormat );
					cellCursor.insertText(codec->toUnicode("Вих. борг"), textCharFormat);
					//Кінець роботи з таблицею
			
					old_bud = abonQuery->value(1).toInt();
					iterBorj = 0;
				}
			
				table->insertRows( iterBorj+2, 1 );
				//Внесення даних в стрічку
				pokaznQuery->exec("SELECT Prizv, Imya, Batk, Kvartyra FROM abonent \
									WHERE Rahunok=" + QVariant(Rahunok).toString());
				pokaznQuery->seek(0);
					//№ п/п"
				cell = table->cellAt(iterBorj+2, 0);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				cellCursor.insertText(QVariant(iterBorj+1).toString(), textCharFormat);
					//Рахунок
				cell = table->cellAt(iterBorj+2, 1);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				cellCursor.insertText(QVariant(Rahunok).toString(), textCharFormat);
					//П І Б
				cell = table->cellAt(iterBorj+2, 2);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				cellCursor.insertText(pokaznQuery->value(0).toString() + " " + pokaznQuery->value(1).toString().at(0) + "." +
										pokaznQuery->value(2).toString().at(0) + ".", textCharFormat);
				//Квартира
				cell = table->cellAt(iterBorj+2, 3);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				cellCursor.insertText(pokaznQuery->value(3).toString(), textCharFormat);
					//Сумнівн. борг
				cell = table->cellAt(iterBorj+2, 4);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				cellCursor.insertText(uMToStr2(zagVyh_Saldo - zagNarah), textCharFormat);
					//Вихідний борг
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
	cursor.insertText(codec->toUnicode("     Загальна кількість боржників - ") + QVariant(zagKilkBorjn).toString() + codec->toUnicode("."));
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("     Загальна сума сумнівного боргу - ") + uMToStr2(zagSumnivnBorg) + codec->toUnicode("грн."));
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("     Загальна сума вихідного боргу на кінець періоду - ") + uMToStr2(zagVyhBorg) + codec->toUnicode("грн."));
	
	delete abonQuery;
	delete budQuery;
	delete pokaznQuery;
	delete postup_form;
	printform->printer()->setDocName("Відомість боржників");
	printform->show();
}
//------------------------------------------------------------
void UPrintDocs::print_vidomist_wrong_subs(int cur_year, int cur_month)
{
		int year = cur_year;
		int month = cur_month;
		printform = new UPrintForm(0, codec->toUnicode("Відомість помилково нарахованих субсидій ЦО"));
		QTextBlockFormat blockFormat;
		QTextTableCell cell;
		QTextCharFormat textCharFormat;
		//Створення документа тарифів для всіх будинків за X місяць, Y рік
		printform->document()->clear();
		QTextCursor cursor(printform->document()), cellCursor;
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.setBlockFormat( blockFormat );
		textCharFormat.setFontPointSize( 13 );
		cursor.insertText(codec->toUnicode("ВІДОМІСТЬ\nпомилково нарахованої субсидії ЦО"), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 8 );
		cursor.insertText(codec->toUnicode(" "), textCharFormat);
		cursor.insertBlock(blockFormat);
		textCharFormat.setFontPointSize( 11 );
		cursor.insertText(codec->toUnicode("за ") + monthlist.at(month-1) +
							codec->toUnicode(" ") + QVariant(year).toString() + codec->toUnicode("р."), textCharFormat);
		blockFormat.setAlignment( Qt::AlignHCenter );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText(codec->toUnicode("по абонентах ")+OrganizName()+"\n");
		textCharFormat.setFontPointSize( 8 );
		cursor.insertText(codec->toUnicode(" "), textCharFormat);
			//створення і налаштування таблиці
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
			//заповнення шапки таблиці
		cell = table->cellAt(0, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("№ п/п"));
	
		cell = table->cellAt(0, 1);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Рахунок"));
	
		cell = table->cellAt(0, 2);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Номер субс."));
		
		cell = table->cellAt(0, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("П І Б"));
		
		cell = table->cellAt(0, 4);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Адреса"));
		
		cell = table->cellAt(0, 5);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Сума нарах. за ЦО, грн"));
		
		cell = table->cellAt(0, 6);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Сума нарах субс., грн"));
			//налаштування кількості стрічок
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
		//виведення форми поступу
		UPostupForm *postup_form = new UPostupForm(0, query.value(0).toInt() );
		postup_form->show();
			//заповнення стрічок інформацією
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
			// № п/п
			cell = table->cellAt(1+iter, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( QVariant(iter+1).toString() );
			// Рахунок
			cell = table->cellAt(1+iter, 1);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( query.value(0).toString() );
			// Номер субс
			cell = table->cellAt(1+iter, 2);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( query.value(1).toString() );
			// П І Б
			cell = table->cellAt(1+iter, 3);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( query.value(2).toString() );
			// Адреса
			cell = table->cellAt(1+iter, 4);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			budynokQuery->exec("SELECT Vulycia & ' ' & Bud_num FROM budynky WHERE id="+query.value(5).toString());
			budynokQuery->seek(0);
			cellCursor.insertText( budynokQuery->value(0).toString() + codec->toUnicode(", кв.") + query.value(6).toString() );
			// Сума нарах. за ЦО
			cell = table->cellAt(1+iter, 5);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(query.value(3).toDouble()) );
			// Сума нарах субс.
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
		
			//Кінець створення документу для друку
		delete budynokQuery;
		postup_form->close();
		printform->printer()->setDocName("Відомість помилково нарахованих субсидій за "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"р.");
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
	printform = new UPrintForm(0, codec->toUnicode("Відомість нарах. опл. за ЦО для Відділу субсидій"));
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat;
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignTop);
	//Створення документа тарифів для всіх будинків за X місяць, Y рік
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 13 );
	cursor.insertText(codec->toUnicode("ВІДОМІСТЬ\n нарахування оплати за ЦО для Відділу субсидій"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 8 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("по абонентах ")+OrganizName()+"\n");
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("за ") + monthlist.at(month-1) +
						codec->toUnicode(" ") + QVariant(year).toString() + codec->toUnicode("р."), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 10 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
		//створення і налаштування таблиці
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
		//заповнення шапки таблиці
	cell = table->cellAt(0, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("№ п/п"), textCharFormat);

	cell = table->cellAt(0, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Рах."), textCharFormat);

	cell = table->cellAt(0, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Номер субс."), textCharFormat);
	
	cell = table->cellAt(0, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("П І Б"), textCharFormat);
	
	cell = table->cellAt(0, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Адреса"), textCharFormat);
	
	cell = table->cellAt(0, 5);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Пільга"), textCharFormat);
	
	cell = table->cellAt(0, 6);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Площа"), textCharFormat);
	
	cell = table->cellAt(0, 7);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("К-сть прож."), textCharFormat);
	
	cell = table->cellAt(0, 8);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Зайва площа."), textCharFormat);
	
	cell = table->cellAt(0, 9);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Сума нарах."), textCharFormat);
	
	query->exec("SELECT abonent.Rahunok, IIf(spravy_subs.Nom_spravy>0, spravy_subs.Nom_spravy, '-'), abonent.Prizv & ' ' & abonent.Imya & ' ' & abonent.Batk, \
					budynky.Vulycia & ' ' & budynky.Bud_num & ', "+codec->toUnicode("кв")+".' & abonent.Kvartyra, \
					IIF(abonent.Pilga_ID>0, IIf(abonent.Pilga_ID_2>0, '1)' & pilgy.Vidsotok & '% "+codec->toUnicode("для")+" ' & abonent.Pilga_for & ' "+codec->toUnicode("чол")+". 2)' & pilgy_2.Vidsotok & '% "+codec->toUnicode("для")+" ' & abonent.Pilga_for_2 & ' "+codec->toUnicode("чол")+".'    ,\
						pilgy.Vidsotok & '% "+codec->toUnicode("для")+" ' & abonent.Pilga_for & ' "+codec->toUnicode("чол")+".'), '-'), \
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
	
			//Кінець створення документу для друку
	printform->printer()->setDocName("Відомість нарахування для відділу субсидії за "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"р.");
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
	printform = new UPrintForm(0, codec->toUnicode("Довідник абонентів"));
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat, textCharFormat_italic;
	//Створення документа тарифів для всіх будинків за X місяць, Y рік
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 13 );
	cursor.insertText(codec->toUnicode("ДОВІДНИК АБОНЕНТІВ\n")+OrganizName(), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 8 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("Створений ПК \"ТКЕнерго менеджер\" ") + date.toString("dd.MM.yyyy") + codec->toUnicode("р."), textCharFormat);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("Абонентів у базі даних - ") + QVariant(numAbon).toString(), textCharFormat);
	
	textCharFormat.setFontPointSize( 9 );
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	textCharFormat_italic.setFontPointSize( 9 );
	textCharFormat_italic.setFontItalic( true );
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	//читання необхідності показування CO і ГВ
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	bool needCO = settings->value("GUI_View/ShowNarahCO").toBool();
	bool needGV = settings->value("GUI_View/ShowNarahGV").toBool();

	delete settings;
	
	int pidsCO[3] = {0,0,0};
	int pidsGV[3] = {0,0,0};
	double pidsPLOS[3] = {0,0,0};
	int pidsLUDY[3] = {0,0,0};
	
	//Заповнення довідника пільг
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
		//створення і налаштування таблиці
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
				
				//Заповнення шапки таблиці
		//Адреса будинку
		cell = table->cellAt(0, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Буд. ") + budynkyQuery->value(1).toString()+" "+budynkyQuery->value(2).toString(), textCharFormat);
		
		//№ п/п
		cell = table->cellAt(1, 0);
		cell.setFormat(textCharFormat_italic);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("№ п/п"), textCharFormat_italic);
		
		//Рахунок
		cell = table->cellAt(1, 1);
		cell.setFormat(textCharFormat_italic);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Рах."), textCharFormat_italic);
		
		//ПІБ
		cell = table->cellAt(1, 2);
		cell.setFormat(textCharFormat_italic);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("П І Б"), textCharFormat_italic);
		
		//Квартира
		cell = table->cellAt(1, 3);
		cell.setFormat(textCharFormat_italic);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Кварт."), textCharFormat_italic);
		
		//Площа
		cell = table->cellAt(1, 4);
		cell.setFormat(textCharFormat_italic);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Площа"), textCharFormat_italic);
		
		//Кількість людей
		cell = table->cellAt(1, 5);
		cell.setFormat(textCharFormat_italic);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("К.люд."), textCharFormat_italic);
		
		//Зайва площа
		cell = table->cellAt(1, 6);
		cell.setFormat(textCharFormat_italic);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Зайва площа"), textCharFormat_italic);
		
		//Пільги
		cell = table->cellAt(1, 7);
		cell.setFormat(textCharFormat_italic);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Пільги"), textCharFormat_italic);
		
		//№ тел.
		cell = table->cellAt(1, 8);
		cell.setFormat(textCharFormat_italic);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("№ телефону"), textCharFormat_italic);
		
		int colNum=8;
		//Опалення
		if (needCO){
			++colNum;
			cell = table->cellAt(1, colNum);
			cell.setFormat(textCharFormat_italic);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(codec->toUnicode("ЦО"), textCharFormat_italic);
		}
		
		//Опалення
		if (needGV){
			++colNum;
			cell = table->cellAt(1, colNum);
			cell.setFormat(textCharFormat_italic);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(codec->toUnicode("ГВ"), textCharFormat_italic);
		}
		
		int budIter=2;
		query->exec("SELECT * FROM abonent WHERE Budynok_ID="+budynkyQuery->value(0).toString()+" ORDER BY Kvartyra");
		while (query->next()){
			//№ п/п
			cell = table->cellAt(budIter, 0);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QVariant(abonIter).toString(), textCharFormat);
		
			//Рахунок
			cell = table->cellAt(budIter, 1);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(query->value(1).toString(), textCharFormat);
		
			//ПІБ
			cell = table->cellAt(budIter, 2);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(query->value(2).toString()+" "+
								query->value(3).toString()+" "+
								query->value(4).toString(), textCharFormat);
		
			//Квартира
			cell = table->cellAt(budIter, 3);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(query->value(7).toString(), textCharFormat);
		
			//Площа
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
		
			//Кількість людей
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
		
			//Зайва площа
			cell = table->cellAt(budIter, 6);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(uMToStr2(query->value(9).toDouble()), textCharFormat);
			
			//Пільги
			QString pilgyStr="-";
			if (query->value(10).toInt()>0){
				pilgyStr = hash.value(query->value(10).toInt()) 
							+ codec->toUnicode(" на ")
							+ query->value(11).toString()
							+ codec->toUnicode("чол.");
			}
			if (query->value(12).toInt()>0){
				pilgyStr += ", " + hash.value(query->value(12).toInt()) 
							+ codec->toUnicode(" на ")
							+ query->value(13).toString()
							+ codec->toUnicode("чол.");
			}
			cell = table->cellAt(budIter, 7);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(pilgyStr, textCharFormat);
		
			//№ тел.
			cell = table->cellAt(budIter, 8);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(query->value(14).toString(), textCharFormat);
			
			colNum = 8;
			//Опалення
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
			
			//ГВ
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
		
			//Підсумки по абонентах, які відключені від ЦО
		//Назва підсумка
		cell = table->cellAt(budIter, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Всього по абонентах, яким не надаються послуги"), textCharFormat);
		
		//Кількість квартир
		cell = table->cellAt(budIter, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("-", textCharFormat);
		
		//Площа
		cell = table->cellAt(budIter, 4);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(vidklPloshcha), textCharFormat);
		
		//Кількість людей
		cell = table->cellAt(budIter, 5);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QVariant(kilkLudVidkl).toString(), textCharFormat);
		
		//Зайва площа
		cell = table->cellAt(budIter, 6);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("-", textCharFormat);
		
		//Пільги
		cell = table->cellAt(budIter, 7);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("-", textCharFormat);
		
		//№ тел.
		cell = table->cellAt(budIter, 8);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("-", textCharFormat);
		
		colNum = 8;
		//Опалення
		if (needCO){
			++colNum;
			cell = table->cellAt(budIter, colNum);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QVariant(kilkVidkl).toString(), textCharFormat);
		}
		
		//ГВ
		if (needGV){
			++colNum;
			cell = table->cellAt(budIter, colNum);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QVariant(kilkVidklGV).toString(), textCharFormat);
		}
		
			//Підсумки по абонентах з ЦО
		//Назва підсумка
		cell = table->cellAt(budIter+1, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Всього по абонентах, яким надаються послуги"), textCharFormat);
		
		//Кількість квартир
		cell = table->cellAt(budIter+1, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("-", textCharFormat);
		
		//Площа
		cell = table->cellAt(budIter+1, 4);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(opalPloshcha), textCharFormat);
		
		//Кількість людей
		cell = table->cellAt(budIter+1, 5);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QVariant(kilkLudOpal).toString(), textCharFormat);
		
		//Зайва площа
		cell = table->cellAt(budIter+1, 6);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("-", textCharFormat);
		
		//Пільги
		cell = table->cellAt(budIter+1, 7);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("-", textCharFormat);
		
		//№ тел.
		cell = table->cellAt(budIter+1, 8);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("-", textCharFormat);
		
		colNum = 8;
		//Опалення
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
		
			//Підсумки по будинку
		//Назва підсумка
		cell = table->cellAt(budIter+2, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Всього по будинку"), textCharFormat);
		
		//Кількість квартир
		cell = table->cellAt(budIter+2, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("-", textCharFormat);
		
		//Площа
		cell = table->cellAt(budIter+2, 4);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(opalPloshcha+vidklPloshcha), textCharFormat);
		pidsPLOS[0] += opalPloshcha;
		pidsPLOS[1] += vidklPloshcha;
		pidsPLOS[2] += opalPloshcha + vidklPloshcha;
		
		//Кількість людей
		cell = table->cellAt(budIter+2, 5);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QVariant(kilkLudOpal+kilkLudVidkl).toString(), textCharFormat);
		pidsLUDY[0] += kilkLudOpal;
		pidsLUDY[1] += kilkLudVidkl;
		pidsLUDY[2] += kilkLudOpal + kilkLudVidkl;
		
		//Зайва площа
		cell = table->cellAt(budIter+2, 6);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("-", textCharFormat);
		
		//Пільги
		cell = table->cellAt(budIter+2, 7);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("-", textCharFormat);
		
		//№ тел.
		cell = table->cellAt(budIter+2, 8);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("-", textCharFormat);
		
		colNum=8;
		//Опалення
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
	cursor.insertText( "		ПІДСУМКИ ПО ПІДПРИЄМСТВУ", textCharFormat);
	blockFormat.setPageBreakPolicy( QTextFormat::PageBreak_Auto );
	cursor.insertBlock(blockFormat);
	if (needCO){
		cursor.insertBlock(blockFormat);
		cursor.insertText("	ЦЕНТРАЛЬНЕ ОПАЛЕННЯ\n", textCharFormat);
		cursor.insertText("Кількість абонентів, які отримують послуги ЦО - "+QString::number(pidsCO[0])+"\n", textCharFormat);
		cursor.insertText("Кількість абонентів, які не отримують послуги ЦО - "+QString::number(pidsCO[1])+"\n", textCharFormat);
		cursor.insertText("Всього абонентів - "+QString::number(pidsCO[2])+"\n", textCharFormat);
		
		cursor.insertText("Площа квартир абонентів, які отримують послуги ЦО - "+QString::number(pidsPLOS[0],'f',4)+"\n", textCharFormat);
		cursor.insertText("Площа квартир абонентів, які не отримують послуги ЦО - "+QString::number(pidsPLOS[1],'f',4)+"\n", textCharFormat);
		cursor.insertText("Загальна площа квартир абонентів по підприємству - "+QString::number(pidsPLOS[2],'f',4)+"\n", textCharFormat);
	}
	
	if (needGV){
		cursor.insertBlock(blockFormat);
		cursor.insertText("	ГАРЯЧА ВОДА\n", textCharFormat);
		cursor.insertText("Кількість абонентів, які отримують послуги ГВ - "+QString::number(pidsGV[0])+"\n", textCharFormat);
		cursor.insertText("Кількість абонентів, які не отримують послуги ГВ - "+QString::number(pidsGV[1])+"\n", textCharFormat);
		cursor.insertText("Всього абонентів - "+QString::number(pidsGV[2])+"\n", textCharFormat);
		
		cursor.insertText("Кількість прописаних людей, які отримують послуги ГВ - "+QString::number(pidsLUDY[0])+"\n", textCharFormat);
		cursor.insertText("Кількість прописаних людей, які не отримують послуги ЦО - "+QString::number(pidsLUDY[1])+"\n", textCharFormat);
		cursor.insertText("Загальна кількість прописаних людей по підприємству - "+QString::number(pidsLUDY[2])+"\n", textCharFormat);
	}
	
	//Кінець створення документу для друку
	printform->printer()->setDocName("ДОВІДНИК АБОНЕНТІВ");
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
	printform = new UPrintForm(0, codec->toUnicode("Відомість заборгованості подання документів на пільги"));
	
	QHash<int, QString> pilgyHash;
	query->exec("SELECT id, Kategoriya FROM pilgy");
	while (query->next()){
		pilgyHash.insert(query->value(0).toInt(), query->value(1).toString());
	}
	
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat, textCharFormat_italic;
	//Створення документа тарифів для всіх будинків за X місяць, Y рік
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 13 );
	cursor.insertText(codec->toUnicode("ВІДОМІСТЬ\n заборгованості подання документів на пільги")+OrganizName(), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 8 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("Створений ПК \"ТКЕнерго менеджер\" ") + date.toString("dd.MM.yyyy") + codec->toUnicode("р."), textCharFormat);
	
	textCharFormat.setFontPointSize( 9 );
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	textCharFormat_italic.setFontPointSize( 9 );
	textCharFormat_italic.setFontItalic( true );
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	//створення і налаштування таблиці
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
		//заповнення шапки таблиці
	cell = table->cellAt(0, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("№ п/п"));
	
	cell = table->cellAt(0, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Рахунок"));
	
	cell = table->cellAt(0, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("П І Б"));
	
	cell = table->cellAt(0, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Пільги"));
	
	cell = table->cellAt(0, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Документи"));

	
		//заповнення стрічок
	query->exec("SELECT Rahunok, Prizv, Imya, Batk, \
						Pilga_ID, Pilga_for, Pilga_ID_2, Pilga_for_2, Pilgy_zdani \
				FROM abonent \
				WHERE Opal=true and Pilga_ID>0 and Pilgy_zdani=false \
				ORDER BY Pilga_ID, Rahunok");
	for (int row=0; (row<numRows) && query->next(); row++){
		QString str, tStr;
		
		//№ п/п
		cell = table->cellAt(row+1, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QString::number(row+1));
		
		//Рахунок
		cell = table->cellAt(row+1, 1);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query->value(0).toString());
		
		//П І Б
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
		
		//Пільги
		str=pilgyHash.value(query->value(4).toInt())+" на "+query->value(5).toString();
		if (query->value(6).toInt() > 0)
			str += "; 2)" + pilgyHash.value(query->value(6).toInt())+" на "+query->value(7).toString();
		cell = table->cellAt(row+1, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(str);
		
		//Документи
		cell = table->cellAt(row+1, 4);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		if (query->value(8).toBool())
			cellCursor.insertText(codec->toUnicode("ЗданО"));
		
		postup_form->inc_pos();
	}
		//Кінець створення документу для друку
	printform->printer()->setDocName("Відомість подання документів на пільги");
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
	
	//Читання необхідності друку реквізитів для підпису після кожної категорії пільги
	bool needPrintPidpysyAfterAllKat;
	query.exec("SELECT strTemplate FROM slujb_znach WHERE id=7");
	query.seek(0);
	needPrintPidpysyAfterAllKat = query.value(0).toBool();
	
	//Читання тарифів
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
	//Створення документа тарифів для всіх будинків за X місяць, Y рік
	printform = new UPrintForm("Форма № 2-пільга за "+monthlist.at(month-1)+" "+QString::number(year)+"р.", 10, 10, 10, 10);
	printform->document()->clear();
	printform->printer()->setOrientation( QPrinter::Landscape );
	printform->printer()->setDocName("ЗВІТ - Форма №2-пільга за "+monthlist.at(month-1)+" "+QString::number(year)+"р.");
	QTextCursor cursor(printform->document()), cellCursor;
		
	//Створення таблиці
	pilgyQuery.exec("SELECT id, Kategoriya, Vidsotok, Grupy FROM pilgy ORDER BY Kategoriya");
	int curPilgyGrupy;
	double curVidsotokPilgy;
	int curKategoriyaPilgy;
	int iter;
	int numRowsForPilgovyky;
	QSqlQuery pilgovykyQuery;
	int curGrupaPilgovyka;
	QTextTable *table=NULL;
	//Створення кістяка таблиці
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
		
		//Чтання кількості стрічок нарахування пільг у поточному місяці для поточної пільги
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
            cursor.insertText("Сформовано ПК \"ТКЕнерго менеджер\" (супровід Компанія \"АбонПро\". веб: www.abonpro.com). Час формування: "+QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy")+"р." , textCharFormat);
			cursor.insertBlock(blockFormat);
			cursor.insertBlock(blockFormat);
			//Затверджено
			textCharFormat.setFontPointSize( 9 );
			cursor.insertText("ЗАТВЕРДЖЕНО", textCharFormat); 
			cursor.insertBlock(blockFormat);
			cursor.insertText("Наказ Міністерства праці", textCharFormat);
			cursor.insertBlock(blockFormat);
			cursor.insertText("та соціальної політики України ", textCharFormat);
			cursor.insertBlock(blockFormat);
			cursor.insertText("04.10.2007  N 535", textCharFormat);
			cursor.insertBlock(blockFormat);
			cursor.insertBlock(blockFormat);
			cursor.insertText("Форма N 2-пільга", textCharFormat);
			blockFormat.setAlignment( Qt::AlignLeft );
			cursor.insertBlock(blockFormat);
			textCharFormat_italic.setFontPointSize( 7 );
			cursor.insertText("Заповнюється підприємством (організацією), що надає послуги ", textCharFormat_italic);
			blockFormat.setAlignment( Qt::AlignCenter );
			cursor.insertBlock(blockFormat);
			textCharFormat_bold.setFontPointSize( 11 );
			cursor.insertText("Підприємство: "+OrganizName(), textCharFormat_bold);
			blockFormat.setAlignment( Qt::AlignCenter );
			cursor.insertBlock(blockFormat);
			cursor.insertBlock(blockFormat);
			textCharFormat_bold.setFontPointSize( 12 );
			cursor.insertText("Р О З Р А Х У Н О К", textCharFormat_bold);
			cursor.insertBlock(blockFormat);
			cursor.insertText("видатків на відшкодування витрат, пов'язаних з наданням пільг", textCharFormat_bold);
			cursor.insertBlock(blockFormat);
			cursor.insertText("за " + monthlist.at(month-1) + " " + QString::number(year), textCharFormat_bold);
		
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
			//заповнення шапки таблиці
		table->mergeCells(0,0,1,13);
		cell = table->cellAt(0, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("Пільговики категорії - "+pilgyQuery.value(1).toString()+" ("+pilgyQuery.value(0).toString()+")", textCharFormat_bold);
		
		table->mergeCells(1,0,2,1);
		cell = table->cellAt(1, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("№ п/п");
		
		table->mergeCells(1,1,1,6);
		cell = table->cellAt(1, 1);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("Дані про пільговика");
		
		table->mergeCells(1,7,2,1);
		cell = table->cellAt(1, 7);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("Рік, за який про-ведено обраху-вання");
		
		table->mergeCells(1,8,2,1);
		cell = table->cellAt(1, 8);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("Місяць, за який про-ведено обраху-вання");
		
		table->mergeCells(1,9,2,1);
		cell = table->cellAt(1, 9);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("К-сть днів, за які про-ведено обраху-вання");
		
		table->mergeCells(1,10,1,2);
		cell = table->cellAt(1, 10);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("Види послуг");
		
		table->mergeCells(1,12,2,1);
		cell = table->cellAt(1, 12);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("Разом, грн");
		
		cell = table->cellAt(2, 1);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("Прізвище, ім'я, по батькові");
		
		cell = table->cellAt(2, 2);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("Адреса");
	
		cell = table->cellAt(2, 3);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("Іденти-фікаційний номер");
	
		cell = table->cellAt(2, 4);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("Кільк. осіб");
		
		cell = table->cellAt(2, 5);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("Код кате-горії / Розмір пільги");
	
		cell = table->cellAt(2, 6);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("№ особового рахунка");
		
		cell = table->cellAt(2, 10);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("Центра-лізоване тепло-поста-чання\n"+coPilKod);
		
		cell = table->cellAt(2, 11);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("Центра-лізоване поста-чання гарячої води\n"+gvPilKod);
		
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
			// № п/п
			table->mergeCells(3+iter*3,0,3,1);
			cell = table->cellAt(3+iter*3, 0);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( QVariant(iter+1).toString() );
				// ПІБ пільговика, а в разі відсутності - абонента
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
				// Ідентифікаційний код при наявності пільговика
				table->mergeCells(3+iter*3,3,3,1);
				cell = table->cellAt(3+iter*3, 3);
				cell.setFormat(cur_textCharFormat);
				cellCursor = cell.lastCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				if (pilgovyky_e)
					cellCursor.insertText( "\n" );
				cellCursor.insertText( pilgovykyQuery.value(3).toString() );
				
					//Запам'ятовування групи пільговика для майбутного використання прививеденні інформації про групи
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
				// Ідентифікаційний код при відсутності пільговика
				table->mergeCells(3+iter*3,3,3,1);
				cell = table->cellAt(3+iter*3, 3);
				cell.setFormat(cur_textCharFormat);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				cellCursor.insertText( query_2.value(3).toString() );
			}
			// Адреса
			table->mergeCells(3+iter*3,2,3,1);
			cell = table->cellAt(3+iter*3, 2);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( query_2.value(2).toString() );
			// Кількість осіб, що отр. пільги
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

			// Розимір пільги
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
			
			// Рахунок
			table->mergeCells(3+iter*3,6,3,1);
			cell = table->cellAt(3+iter*3, 6);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( query_2.value(0).toString() );
			
			// Рік, за який проведено обрахування
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
			
			// Місяць, за який проведено обрахування
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
			
			// Кількість днів, за які проведено обрахування
			table->mergeCells(3+iter*3,9,3,1);
			cell = table->cellAt(3+iter*3, 9);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			int numPilgDays = QDate(year, month, 1).daysInMonth();
			if (d_to_m(query_2.value(4).toDouble()) != 0) //якщо є нарахування пільги за ЦО
				numPilgDays = abs(query_2.value(13).toInt() - query_2.value(12).toInt() + 1);
			else
				numPilgDays = abs(query_2.value(15).toInt() - query_2.value(14).toInt() + 1);
			//QDate pilgyDateVidr(year, month, 1);
			if (numPilgDays == QDate(year, month, 1).daysInMonth())
				cellCursor.insertText( "-" );
			else
				cellCursor.insertText( QString::number(numPilgDays) );
			
			double curNarahPilgaCO, curNarahPilgaGV;
			// Сума пільги по центральному опаленню
				//Фактичне використання
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
				//Сума пільги
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
				//Тариф
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
			
			// Сума пільги по гарячій воді
				//Фактичне використання
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
				//Сума пільги
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
				//Тариф
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
			
			// Загальна сума
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
		
		//Остання стрічка таблиці (сума)
		table->mergeCells ( 3+iter, 0, 1, 4 );
		table->mergeCells ( 3+iter, 4, 1, 3 );
		table->mergeCells ( 3+iter, 7, 1, 3 );
		//Всього
		cell = table->cellAt(3+iter, 0);
		cell.setFormat(textCharFormat_bold);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( codec->toUnicode("Всього"));
		
		//Кількість пільговиків
		cell = table->cellAt(3+iter, 4);
		cell.setFormat(textCharFormat_bold);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( codec->toUnicode("Пільговики: ") + QString::number(pilgovykyKatCount));
		
		//вивід надпису ->зведена пільга
		cell = table->cellAt(3+iter, 7);
		cell.setFormat(textCharFormat_bold);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( codec->toUnicode("Заг. сума, грн.") );
		//вивід значення cсуми пільг по ЦО
		cell = table->cellAt(3+iter, 10);
		cell.setFormat(textCharFormat_bold);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( uMToStr2(grupPilgaSumCO[0]+grupPilgaSumCO[1]+grupPilgaSumCO[2]+grupPilgaSumCO[3]));
		//вивід значення cсуми пільг по ГВ
		cell = table->cellAt(3+iter, 11);
		cell.setFormat(textCharFormat_bold);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( uMToStr2(grupPilgaSumGV[0]+grupPilgaSumGV[1]+grupPilgaSumGV[2]+grupPilgaSumGV[3]));
		//вивід значення зведеної пільги
		cell = table->cellAt(3+iter, 12);
		cell.setFormat(textCharFormat_bold);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText( uMToStr2(grupPilgaSumCO[0]+grupPilgaSumCO[1]+grupPilgaSumCO[2]+grupPilgaSumCO[3]
										 +grupPilgaSumGV[0]+grupPilgaSumGV[1]+grupPilgaSumGV[2]+grupPilgaSumGV[3]));
		//При необхідності сортування за групою
		if (curPilgyGrupy > 0){
			table->insertRows( 4+iter, 4 );
			table->mergeCells ( 4+iter, 0, 1, 10 );
			table->mergeCells ( 5+iter, 0, 1, 10 );
			table->mergeCells ( 6+iter, 0, 1, 10 );
			table->mergeCells ( 7+iter, 0, 1, 10 );
			
			//вивід надпису ->Група 1
			cell = table->cellAt(4+iter, 0);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText("Група 1");
			//вивід значення кількості пільговиків ЦО
			cell = table->cellAt(4+iter, 10);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QString::number(grupPilgovykyCountCO[1])+" чол.\n"+ uMToStr2(grupPilgaSumCO[1])+"грн." );
			//вивід значення кількості пільговиків ГВ
			cell = table->cellAt(4+iter, 11);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QString::number(grupPilgovykyCountGV[1])+" чол.\n"+ uMToStr2(grupPilgaSumGV[1])+"грн." );
			//Вивід підсумкової величини по групі
			cell = table->cellAt(4+iter, 12);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QString::number(grupPilgovykyCountCO[1]+grupPilgovykyCountGV[1])+" чол.\n"
									+uMToStr2(grupPilgaSumCO[1]+grupPilgaSumGV[1])+"грн." );
			
			//вивід надпису ->Група 2
			cell = table->cellAt(5+iter, 0);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Група 2") );
			//вивід значення кількості пільговиків ЦО
			cell = table->cellAt(5+iter, 10);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QString::number(grupPilgovykyCountCO[2])+" чол.\n"+ uMToStr2(grupPilgaSumCO[2])+"грн." );
			//вивід значення кількості пільговиків ГВ
			cell = table->cellAt(5+iter, 11);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QString::number(grupPilgovykyCountGV[2])+" чол.\n"+ uMToStr2(grupPilgaSumGV[2])+"грн." );
			//Вивід підсумкової величини по групі
			cell = table->cellAt(5+iter, 12);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QString::number(grupPilgovykyCountCO[2]+grupPilgovykyCountGV[2])+" чол.\n"
									+uMToStr2(grupPilgaSumCO[2]+grupPilgaSumGV[2])+"грн." );
			
			//вивід надпису ->Група 3
			cell = table->cellAt(6+iter, 0);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Група 3") );
			//вивід значення кількості пільговиків ЦО
			cell = table->cellAt(6+iter, 10);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QString::number(grupPilgovykyCountCO[3])+" чол.\n"+ uMToStr2(grupPilgaSumCO[3])+"грн." );
			//вивід значення кількості пільговиків ГВ
			cell = table->cellAt(6+iter, 11);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QString::number(grupPilgovykyCountGV[3])+" чол.\n"+ uMToStr2(grupPilgaSumGV[3])+"грн." );
			//Вивід підсумкової величини по групі
			cell = table->cellAt(6+iter, 12);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QString::number(grupPilgovykyCountCO[3]+grupPilgovykyCountGV[3])+" чол.\n"
									+uMToStr2(grupPilgaSumCO[3]+grupPilgaSumGV[3])+"грн." );
			
			//вивід надпису ->Без групи
			cell = table->cellAt(7+iter, 0);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Без групи") );
			//вивід значення кількості пільговиків ЦО
			cell = table->cellAt(7+iter, 10);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QString::number(grupPilgovykyCountCO[0])+" чол.\n"+ uMToStr2(grupPilgaSumCO[0])+"грн." );
			//вивід значення кількості пільговиків ГВ
			cell = table->cellAt(7+iter, 11);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QString::number(grupPilgovykyCountGV[0])+" чол.\n"+ uMToStr2(grupPilgaSumGV[0])+"грн." );
			//Вивід підсумкової величини по групі
			cell = table->cellAt(7+iter, 12);
			cell.setFormat(cur_textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QString::number(grupPilgovykyCountCO[0]+grupPilgovykyCountGV[0])+" чол.\n"
									+uMToStr2(grupPilgaSumCO[0]+grupPilgaSumGV[0])+"грн." );
			
		}
		
		
		cursor.movePosition(QTextCursor::End);
		if (needPrintPidpysyAfterAllKat){
			//Створення закінчення для підпису
			textCharFormat.setFontPointSize( 12 );
			cursor.insertText(codec->toUnicode(" "), textCharFormat);
			blockFormat.setAlignment( Qt::AlignLeft );
			cursor.insertBlock(blockFormat);
			cursor.insertBlock(blockFormat);
			cursor.insertBlock(blockFormat);
			cursor.insertText(codec->toUnicode("	Директор ")+OrganizName()+"					"+PrizvIB(1));
			cursor.insertBlock(blockFormat);
			cursor.insertBlock(blockFormat);
			cursor.insertBlock(blockFormat);
			cursor.insertText(codec->toUnicode("    М.П."));
			cursor.insertBlock(blockFormat);
			cursor.insertBlock(blockFormat);
			blockFormat.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysAfter);
			cursor.insertBlock(blockFormat);
			cursor.insertText(codec->toUnicode("	Гол.Бухгалтер")+"							 "+PrizvIB(2));
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
	
	//Після останньої стрічки не повинно бути переходу на нову сторінку
	if (needPrintPidpysyAfterAllKat)
		cursor.setBlockFormat(blockFormat);
	
	if (!needPrintPidpysyAfterAllKat){
		//Створення закінчення для підпису
		textCharFormat.setFontPointSize( 12 );
		cursor.insertText(codec->toUnicode(" "), textCharFormat);
		cursor.movePosition(QTextCursor::End);
		blockFormat.setAlignment( Qt::AlignLeft );
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText(codec->toUnicode("	Директор ")+OrganizName()+"					"+PrizvIB(1));
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText(codec->toUnicode("    М.П."));
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText(codec->toUnicode("	Гол.Бухгалтер")+"								 "+PrizvIB(2));
	}

		//Кінець створення документу для друку
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
	//Створення документа тарифів для всіх будинків за X місяць, Y рік
	printform = new UPrintForm("Відношення про відшкодування пільг по ЦО за "+monthlist.at(month-1)+" "+QString::number(year)+"р.", 10, 10, 10, 10);
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 8 );
    cursor.insertText("Сформовано ПК \"ТКЕнерго менеджер\" (супровід Компанія \"АбонПро\". веб: www.abonpro.com). Час формування: "+QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy")+"р." , textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	//Затверджено
	textCharFormat.setFontPointSize( 12 );
	cursor.insertText("НАЧАЛЬНИКУ", textCharFormat); 
	cursor.insertBlock(blockFormat);
	cursor.insertText("Управління праці та соціального", textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("захисту населення", textCharFormat);
	cursor.insertBlock(blockFormat);
	blockFormat.setAlignment( Qt::AlignJustify );
	cursor.insertBlock(blockFormat);
	textCharFormat_bold.setFontPointSize( 12 );
	cursor.insertText("   "+OrganizName()+" просить Вас відшкодувати нараховані пільги за користування центральним опаленням, що надається пільговикам усіх категорій за " + monthlist.at(month-1) + " " + QString::number(year)+" року.", textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	
	//Створення таблиці
	textCharFormat.setFontPointSize( 10 );
	textCharFormat_bold.setFontPointSize( 9 );
	textCharFormat_italic.setFontPointSize( 10 );
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
		
	//Створення кістяка таблиці
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
	

	//заповнення шапки таблиці
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
	cellCursor.insertText("№ з/п", textCharFormat_italic);
	
	cell = table->cellAt(0, 1);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Пільга", textCharFormat_italic);
	
	cell = table->cellAt(0, 2);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Категорія пільги", textCharFormat_italic);
	
	cell = table->cellAt(0, 3);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("К-сть пільго-виків", textCharFormat_italic);
	
	cell = table->cellAt(0, 4);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Площа пільги, м.кв.", textCharFormat_italic);
	
	cell = table->cellAt(0, 5);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("К-сть членів сімей пільго-виків", textCharFormat_italic);
	
	cell = table->cellAt(0, 6);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Сума нарах. пільг", textCharFormat_italic);
	
	cell = table->cellAt(0, 7);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("в тому числі", textCharFormat_italic);
	
	cell = table->cellAt(1, 7);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("к-сть знятих пільг", textCharFormat_italic);
	
	cell = table->cellAt(1, 8);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("сума знятих пільг", textCharFormat_italic);
	
	cell = table->cellAt(1, 9);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("к-сть донарах. пільг", textCharFormat_italic);
	
	cell = table->cellAt(1, 10);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("сума донарах. пільг", textCharFormat_italic);
	
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
	//Змінні підсумків для виду
	bool notFirstRowInTable=false;
	int oldVyd=0, pilgovykyCountVyd=0, utrymCountVyd=0, znyatoCountVyd=0, donarahCountVyd=0;
	double pilgaSumVyd=0, znyatoSumVyd=0, donarahSumVyd=0, pilgaPlosSumVyd=0;
	
	while (query.next()){
		if (oldVyd != query.value(11).toInt()){
			if (notFirstRowInTable){
				//Підсумкова стрічка по виду
				//"№ з/п"
				table->insertRows(table->rows(), 1);
				table->mergeCells(iter,0,1,3);
				cell = table->cellAt(iter, 0);
				cell.setFormat(textCharFormat);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignRight );
				cellCursor.setBlockFormat( blockFormat );
				cellCursor.insertText("Всього по виду", textCharFormat_bold);

					//"К-сть пільго-виків"
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
				
					//"Площа пільги
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
				
					//"К-сть членів сімей пільго-виків"
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
	
					//"Сума нарах. пільг"
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
	
					//"К-сть знятих. пільг"
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
	
					//"Сума знятих. пільг"
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
	
				//"К-сть донарахованих пільг"
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
	
					//"Сума донарахованих пільг"
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
		
		
		//"№ з/п"
		cell = table->cellAt(iter, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QString::number(iterForRows), textCharFormat);
		iterForRows++;
		
		//"Назва пільги
		cell = table->cellAt(iter, 1);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(1).toString(), textCharFormat);
		
		//"Категорія пільги"
		cell = table->cellAt(iter, 2);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(0).toString(), textCharFormat);
	
		//"К-сть пільго-виків"
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
		
		//"Площа пільг
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
		
		//"К-сть членів сімей пільго-виків"
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
	
		//"Сума нарах. пільг"
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
	
		//"К-сть знятих. пільг"
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
	
		//"Сума знятих. пільг"
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
		
		//"К-сть донарахованих пільг"
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
	
		//"Сума донарахованих пільг"
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
	
	//Підсумкова стрічка
		//"№ з/п"
	table->mergeCells(iter,0,1,2);
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Всього", textCharFormat_bold);

		//"К-сть пільго-виків"
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
	
		//"Площа пільг
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
	
		//"К-сть членів сімей пільго-виків"
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
	
		//"Сума нарах. пільг"
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
	
		//"К-сть знятих. пільг"
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
	
		//"Сума знятих. пільг"
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
	
	//"К-сть донарахованих пільг"
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
	
		//"Сума донарахованих пільг"
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
	
	//Створення закінчення для підпису
	textCharFormat.setFontPointSize( 12 );
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("	Директор						"+PrizvIB(1));
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("    М.П.");
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("	Гол.Бухгалтер					"+PrizvIB(2));

		//Кінець створення документу для друку
	delete postup_form;
	printform->printer()->setOrientation( QPrinter::Portrait );
	printform->printer()->setDocName("Відношення на відшкодування пільг по ЦО за "+monthlist.at(month-1)+" "+QString::number(year)+"р.");
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
	postup_form->setPostupLabel("1. Друк знятих пільг");
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
	//Створення документа тарифів для всіх будинків за X місяць, Y рік
	printform = new UPrintForm("Відомість перерахунку пільг по ЦО за "+monthlist.at(month-1)+" "+QString::number(year)+"р.", 10, 10, 10, 10);
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 8 );
    cursor.insertText("Сформовано ПК \"ТКЕнерго менеджер\" (супровід Компанія \"АбонПро\". веб: www.abonpro.com). Час формування: "+QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy")+"р." , textCharFormat);
	cursor.insertBlock(blockFormat);
	blockFormat.setAlignment( Qt::AlignCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 12 );
	textCharFormat_bold.setFontPointSize( 12 );
	cursor.insertText("ВІДОМІСТЬ", textCharFormat_bold); 
	cursor.insertBlock(blockFormat);
	cursor.insertText("перерахунку пільг за ЦО по "+OrganizName(), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	cursor.insertText("за " + monthlist.at(month-1) + " " + QString::number(year)+" року.", textCharFormat);
	cursor.insertBlock(blockFormat);
	
	
	//Зняті пільги
	//Створення таблиці
	textCharFormat.setFontPointSize( 11 );
	textCharFormat_bold.setFontPointSize( 11 );
	textCharFormat_italic.setFontPointSize( 11 );
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertText("1. Зняті пільги", textCharFormat_bold);
	cursor.insertBlock(blockFormat);
	
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
		
	//Створення кістяка таблиці
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
			//заповнення шапки таблиці
	cell = table->cellAt(0, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("№ з/п", textCharFormat_italic);
	
	cell = table->cellAt(0, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Категорія пільги", textCharFormat_italic);
	
	cell = table->cellAt(0, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("П І Б", textCharFormat_italic);
	
	cell = table->cellAt(0, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Адреса", textCharFormat_italic);
	
	cell = table->cellAt(0, 4);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Рік", textCharFormat_italic);
	
	cell = table->cellAt(0, 5);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Місяць", textCharFormat_italic);
	
	cell = table->cellAt(0, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Сума пільги", textCharFormat_italic);
	
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
		//"№ з/п"
		cell = table->cellAt(iter, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QString::number(iter), textCharFormat);
		
		//"Категорія пільги"
		cell = table->cellAt(iter, 1);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(0).toString(), textCharFormat);
	
		// ПІБ пільговика, а в разі відсутності - абонента
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
			//"ПІБ"
			cell = table->cellAt(iter, 2);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(query.value(1).toString(), textCharFormat);
		}
	
		//Адреса
		cell = table->cellAt(iter, 3);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(2).toString(), textCharFormat);
	
		//Рік
		cell = table->cellAt(iter, 4);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(3).toString(), textCharFormat);
	
		//Місяць
		cell = table->cellAt(iter, 5);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(4).toString(), textCharFormat);
	
		//"Сума
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
	//Підсумкова стрічка
	table->mergeCells(iter,0,1,6);
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText( "Всього", textCharFormat);
	
	cell = table->cellAt(iter, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText( uMToStr2(pilgSum), textCharFormat);
	
	//Донараховані пільги
	countQuery.exec("SELECT count(*) FROM narahuvannya \
				WHERE Pilga_sum>0.009 \
					and Year="+QString::number(year)+" \
					and Month="+QString::number(month)+" \
					and Year_pilg_pererah>2000 and Month_pilg_pererah>0 and Month_pilg_pererah<13");
	countQuery.seek(0);
	rowCount = countQuery.value(0).toInt();
	postup_form->setCurrentPos(0);
	postup_form->setNumPos(rowCount);
	postup_form->setPostupLabel("2. Друк донарахованих пільг");
	
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("2. Донараховані пільги", textCharFormat_bold);
	cursor.insertBlock(blockFormat);
	//Створення таблиці
	table = cursor.insertTable(rowCount+2, 7, tableFormat);
			//заповнення шапки таблиці
	cell = table->cellAt(0, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("№ з/п", textCharFormat_italic);
	
	cell = table->cellAt(0, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Категорія пільги", textCharFormat_italic);
	
	cell = table->cellAt(0, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("П І Б", textCharFormat_italic);
	
	cell = table->cellAt(0, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Адреса", textCharFormat_italic);
	
	cell = table->cellAt(0, 4);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Рік", textCharFormat_italic);
	
	cell = table->cellAt(0, 5);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Місяць", textCharFormat_italic);
	
	cell = table->cellAt(0, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Сума пільги", textCharFormat_italic);
	
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
		//"№ з/п"
		cell = table->cellAt(iter, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QString::number(iter), textCharFormat);
		
		//"Категорія пільги"
		cell = table->cellAt(iter, 1);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(0).toString(), textCharFormat);
		
		
		// ПІБ пільговика, а в разі відсутності - абонента
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
			//"ПІБ"
			cell = table->cellAt(iter, 2);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(query.value(1).toString(), textCharFormat);
		}
	
		//Адреса
		cell = table->cellAt(iter, 3);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(2).toString(), textCharFormat);
	
		//Рік
		cell = table->cellAt(iter, 4);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(3).toString(), textCharFormat);
	
		//Місяць
		cell = table->cellAt(iter, 5);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(4).toString(), textCharFormat);
	
		//"Сума
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
	//Підсумкова стрічка
	table->mergeCells(iter,0,1,6);
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText( "Всього", textCharFormat);
	
	cell = table->cellAt(iter, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText( uMToStr2(pilgSum), textCharFormat);
	
	//Створення закінчення для підпису
	textCharFormat.setFontPointSize( 12 );
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("	Директор						"+PrizvIB(1));
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("    М.П.");
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("	Гол.Бухгалтер					"+PrizvIB(2));

		//Кінець створення документу для друку
	delete postup_form;
	printform->printer()->setOrientation( QPrinter::Portrait );
	printform->printer()->setDocName("Відомість перерахунку пільг по ЦО за "+monthlist.at(month-1)+" "+QString::number(year)+"р.");
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
	//Створення документа тарифів для всіх будинків за X місяць, Y рік
	printform = new UPrintForm("Відношення про відшкодування пільг по ГВ за "+monthlist.at(month-1)+" "+QString::number(year)+"р.", 10, 10, 10, 10);
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 8 );
    cursor.insertText("Сформовано ПК \"ТКЕнерго менеджер\" (супровід Компанія \"АбонПро\". веб: www.abonpro.com). Час формування: "+QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy")+"р." , textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	//Затверджено
	textCharFormat.setFontPointSize( 12 );
	cursor.insertText("НАЧАЛЬНИКУ", textCharFormat); 
	cursor.insertBlock(blockFormat);
	cursor.insertText("Управління праці та соціального", textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("захисту населення", textCharFormat);
	cursor.insertBlock(blockFormat);
	blockFormat.setAlignment( Qt::AlignJustify );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	textCharFormat_bold.setFontPointSize( 12 );
	cursor.insertText("   "+OrganizName()+" просить Вас відшкодувати нараховані пільги за користування гарячою водою, що надається пільговикам усіх категорій за " + monthlist.at(month-1) + " " + QString::number(year)+" року.", textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	
	//Створення таблиці
	textCharFormat.setFontPointSize( 11 );
	textCharFormat_bold.setFontPointSize( 10 );
	textCharFormat_italic.setFontPointSize( 11 );
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
		
	//Створення кістяка таблиці
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
			//заповнення шапки таблиці
	cell = table->cellAt(0, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("№ з/п", textCharFormat_italic);
	
	cell = table->cellAt(0, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Категорія пільги", textCharFormat_italic);
	
	cell = table->cellAt(0, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("К-сть пільго-виків", textCharFormat_italic);
	
	cell = table->cellAt(0, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("К-сть членів сімей пільго-виків", textCharFormat_italic);
	
	cell = table->cellAt(0, 4);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Сума нарах. пільг", textCharFormat_italic);
	
	cell = table->cellAt(0, 5);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("К-сть знятих. пільг", textCharFormat_italic);
	
	cell = table->cellAt(0, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Сума знятих. пільг", textCharFormat_italic);
	
	cell = table->cellAt(0, 7);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("К-сть донарах. пільг", textCharFormat_italic);
	
	cell = table->cellAt(0, 8);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Сума донарах. пільг", textCharFormat_italic);
	
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
		//"№ з/п"
		cell = table->cellAt(iter, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QString::number(iter), textCharFormat);
		
		//"Категорія пільги"
		cell = table->cellAt(iter, 1);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(1).toString(), textCharFormat);
	
		//"К-сть пільго-виків"
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
	
		//"К-сть членів сімей пільго-виків"
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
	
		//"Сума нарах. пільг"
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
	
		//"К-сть знятих. пільг"
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
	
		//"Сума знятих. пільг"
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
		
		//"К-сть донарахованих пільг"
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
	
		//"Сума донарахованих пільг"
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
	
	//Підсумкова стрічка
		//"№ з/п"
	table->mergeCells(iter,0,1,2);
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Всього", textCharFormat);

		//"К-сть пільго-виків"
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
	
		//"К-сть членів сімей пільго-виків"
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
	
		//"Сума нарах. пільг"
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
	
		//"К-сть знятих. пільг"
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
	
		//"Сума знятих. пільг"
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
	
	//"К-сть донарахованих пільг"
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
	
		//"Сума донарахованих пільг"
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
	
	//Створення закінчення для підпису
	textCharFormat.setFontPointSize( 12 );
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("	Директор						"+PrizvIB(1));
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("    М.П.");
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("	Гол.Бухгалтер					"+PrizvIB(2));

		//Кінець створення документу для друку
	delete postup_form;
	printform->printer()->setOrientation( QPrinter::Portrait );
	printform->printer()->setDocName("Відношення на відшкодування пільг по ГВ за "+monthlist.at(month-1)+" "+QString::number(year)+"р.");
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
	postup_form->setPostupLabel("1. Друк знятих пільг");
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
	//Створення документа тарифів для всіх будинків за X місяць, Y рік
	printform = new UPrintForm("Відомість перерахунку пільг по ГВ за "+monthlist.at(month-1)+" "+QString::number(year)+"р.", 10, 10, 10, 10);
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 8 );
    cursor.insertText("Сформовано ПК \"ТКЕнерго менеджер\" (супровід Компанія \"АбонПро\". веб: www.abonpro.com). Час формування: "+QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy")+"р." , textCharFormat);
	cursor.insertBlock(blockFormat);
	blockFormat.setAlignment( Qt::AlignCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 12 );
	textCharFormat_bold.setFontPointSize( 12 );
	cursor.insertText("ВІДОМІСТЬ", textCharFormat_bold); 
	cursor.insertBlock(blockFormat);
	cursor.insertText("перерахунку пільг за ГВ по "+OrganizName(), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	cursor.insertText("за " + monthlist.at(month-1) + " " + QString::number(year)+" року.", textCharFormat);
	cursor.insertBlock(blockFormat);
	
	
	//Зняті пільги
	//Створення таблиці
	textCharFormat.setFontPointSize( 11 );
	textCharFormat_bold.setFontPointSize( 11 );
	textCharFormat_italic.setFontPointSize( 11 );
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertText("1. Зняті пільги", textCharFormat_bold);
	cursor.insertBlock(blockFormat);
	
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
		
	//Створення кістяка таблиці
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
			//заповнення шапки таблиці
	cell = table->cellAt(0, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("№ з/п", textCharFormat_italic);
	
	cell = table->cellAt(0, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Категорія пільги", textCharFormat_italic);
	
	cell = table->cellAt(0, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("П І Б", textCharFormat_italic);
	
	cell = table->cellAt(0, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Адреса", textCharFormat_italic);
	
	cell = table->cellAt(0, 4);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Рік", textCharFormat_italic);
	
	cell = table->cellAt(0, 5);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Місяць", textCharFormat_italic);
	
	cell = table->cellAt(0, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Сума пільги", textCharFormat_italic);
	
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
		//"№ з/п"
		cell = table->cellAt(iter, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QString::number(iter), textCharFormat);
		
		//"Категорія пільги"
		cell = table->cellAt(iter, 1);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(0).toString(), textCharFormat);
	
		// ПІБ пільговика, а в разі відсутності - абонента
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
			//"ПІБ"
			cell = table->cellAt(iter, 2);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(query.value(1).toString(), textCharFormat);
		}
	
		//Адреса
		cell = table->cellAt(iter, 3);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(2).toString(), textCharFormat);
	
		//Рік
		cell = table->cellAt(iter, 4);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(3).toString(), textCharFormat);
	
		//Місяць
		cell = table->cellAt(iter, 5);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(4).toString(), textCharFormat);
	
		//"Сума
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
	//Підсумкова стрічка
	table->mergeCells(iter,0,1,6);
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText( "Всього", textCharFormat);
	
	cell = table->cellAt(iter, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText( uMToStr2(pilgSum), textCharFormat);
	
	//Донараховані пільги
	countQuery.exec("SELECT count(*) FROM narahuvannya \
				WHERE Pilga_sum_GV>0.009 \
					and Year="+QString::number(year)+" \
					and Month="+QString::number(month)+" \
					and Year_pilg_pererah>2000 and Month_pilg_pererah>0 and Month_pilg_pererah<13");
	countQuery.seek(0);
	rowCount = countQuery.value(0).toInt();
	postup_form->setCurrentPos(0);
	postup_form->setNumPos(rowCount);
	postup_form->setPostupLabel("2. Друк донарахованих пільг");
	
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("2. Донараховані пільги", textCharFormat_bold);
	cursor.insertBlock(blockFormat);
	//Створення таблиці
	table = cursor.insertTable(rowCount+2, 7, tableFormat);
			//заповнення шапки таблиці
	cell = table->cellAt(0, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("№ з/п", textCharFormat_italic);
	
	cell = table->cellAt(0, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Категорія пільги", textCharFormat_italic);
	
	cell = table->cellAt(0, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("П І Б", textCharFormat_italic);
	
	cell = table->cellAt(0, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Адреса", textCharFormat_italic);
	
	cell = table->cellAt(0, 4);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Рік", textCharFormat_italic);
	
	cell = table->cellAt(0, 5);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Місяць", textCharFormat_italic);
	
	cell = table->cellAt(0, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Сума пільги", textCharFormat_italic);
	
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
		//"№ з/п"
		cell = table->cellAt(iter, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QString::number(iter), textCharFormat);
		
		//"Категорія пільги"
		cell = table->cellAt(iter, 1);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(0).toString(), textCharFormat);
	
		// ПІБ пільговика, а в разі відсутності - абонента
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
			//"ПІБ"
			cell = table->cellAt(iter, 2);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(query.value(1).toString(), textCharFormat);
		}
	
		//Адреса
		cell = table->cellAt(iter, 3);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(2).toString(), textCharFormat);
	
		//Рік
		cell = table->cellAt(iter, 4);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(3).toString(), textCharFormat);
	
		//Місяць
		cell = table->cellAt(iter, 5);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(4).toString(), textCharFormat);
	
		//"Сума
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
	//Підсумкова стрічка
	table->mergeCells(iter,0,1,6);
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText( "Всього", textCharFormat);
	
	cell = table->cellAt(iter, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText( uMToStr2(pilgSum), textCharFormat);
	
	//Створення закінчення для підпису
	textCharFormat.setFontPointSize( 12 );
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("	Директор						"+PrizvIB(1));
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("    М.П.");
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("	Гол.Бухгалтер					"+PrizvIB(2));

		//Кінець створення документу для друку
	delete postup_form;
	printform->printer()->setOrientation( QPrinter::Portrait );
	printform->printer()->setDocName("Відомість перерахунку пільг по ГВ за "+monthlist.at(month-1)+" "+QString::number(year)+"р.");
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
	//Створення документа тарифів для всіх будинків за X місяць, Y рік
	
	printform = new UPrintForm("Довідка у відділ субсидії за "+monthlist.at(month-1)+" "+QString::number(year)+"р.", 5,5,5,5);
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 7 );
	
	//cursor.insertBlock(blockFormat);
    cursor.insertText("Сформовано ПК \"ТКЕнерго менеджер\" (супровід Компанія \"АбонПро\". веб: www.abonpro.com). Час формування: "+QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy")+"р.      _" , textCharFormat);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	QSqlQuery *query = new QSqlQuery;
	//Затверджено
	blockFormat.setAlignment( Qt::AlignCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat_bold.setFontPointSize( 12.5 );
	cursor.insertText("ДОВІДКА №__________    АБОНЕНТСЬКИЙ РАХУНОК № "+QString::number(rahunok), textCharFormat_bold);
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
	adrStr = "вул. "+query->value(3).toString()+" "+query->value(4).toString();
	if (query->value(5).toInt()>0);
		adrStr += " кв. "+ query->value(5).toString();
	cursor.insertText("     Видана абоненту ", textCharFormat);
	cursor.insertText(nprStr+" "+", "+adrStr+".", textCharFormat_bold);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("     про те, що сума платежів за ", textCharFormat);
	cursor.insertText(monthlist.at(month-1)+" "+QString::number(year), textCharFormat_bold);
	cursor.insertText("р. становить:", textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	query->exec("SELECT sum(Narah) \
				FROM narahuvannya \
				WHERE year="+QString::number(year)+" and month="+QString::number(month)+" and Rahunok="+QString::number(rahunok));
	query->seek(0);
	cursor.insertText("         1) за центральне опалення всього ", textCharFormat);
	cursor.insertText(uMToStr2(query->value(0).toDouble()), textCharFormat_bold);
	cursor.insertText(" грн.", textCharFormat);
	cursor.insertBlock(blockFormat);
	if (suma==0)
		tempStr = "________________________________";
	else
		tempStr = uMToStr2(suma);
	cursor.insertText("         2) за центральне опалення в межах санітарної норми "+tempStr+" грн.", textCharFormat);
	query->exec("SELECT sum(Narah_GV) \
				FROM narahuvannya \
				WHERE year="+QString::number(year)+" and month="+QString::number(month)+" and Rahunok="+QString::number(rahunok));
	query->seek(0);
	cursor.insertBlock(blockFormat);
	cursor.insertText("         3) за гаряче водопостачання ", textCharFormat);
	cursor.insertText(uMToStr2(query->value(0).toDouble()), textCharFormat_bold);
	cursor.insertText(" грн.", textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	if (suma==0)
		tempStr = "__________________________________";
	else
		tempStr = uMToStr2(suma);
	cursor.insertText("     Сума, яка підлягає включенню до розрахунку на субсидію "+tempStr+" грн.", textCharFormat);
	delete query;
	//Підписи
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("	"+DocsPosadaPidpysText(1)+"					"+PrizvIB(1), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("	"+DocsPosadaPidpysText(3)+"				"+PrizvIB(3));
	
	printform->printer()->setDocName("ДОВІДКА у відділ субсидії за "+monthlist.at(month-1)+" "+QString::number(year)+"р.");
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
	//Створення документа тарифів для всіх будинків за X місяць, Y рік
	
	printform = new UPrintForm("Довідка у відділ субсидії за "+monthlist.at(month-1)+" "+QString::number(year)+"р.", 5,5,5,5);
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 7 );
	
	//cursor.insertBlock(blockFormat);
    cursor.insertText("Сформовано ПК \"ТКЕнерго менеджер\" (супровід Компанія \"АбонПро\". веб: www.abonpro.com). Час формування: "+QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy")+"р.      _" , textCharFormat);
	cursor.insertBlock(blockFormat);
	blockFormat.setAlignment( Qt::AlignLeft );
	QSqlQuery *query = new QSqlQuery;
	QSqlQuery q;
	//Затверджено
	blockFormat.setAlignment( Qt::AlignCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 12.5 );
	textCharFormat_bold.setFontPointSize( 12.5 );
	cursor.insertText(OrganizName(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("ДОВІДКА №____", textCharFormat_bold);
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
	adrStr = "вул. "+query->value(3).toString()+" "+query->value(4).toString();
	if (query->value(5).toInt()>0);
		adrStr += " кв. "+ query->value(5).toString();
	cursor.insertText("      Видана власнику (наймачеві) ", textCharFormat);
	cursor.insertText( nprStr , textCharFormat_bold);
	cursor.insertBlock(blockFormat);
	cursor.insertText("      Загальна площа: ", textCharFormat);
	cursor.insertText(uMToStr2(query->value(6).toDouble()), textCharFormat_bold);
	cursor.insertText(" кв.м.", textCharFormat);
	cursor.insertText("      Прописаний за адресою:  ", textCharFormat);
	cursor.insertText( adrStr , textCharFormat_bold);
	cursor.insertBlock(blockFormat);
	cursor.insertText("      Особовий рахунок:  ", textCharFormat);
	cursor.insertText( QString::number(rahunok) + "     " , textCharFormat_bold);
	cursor.insertText("      Кількість чоловік, включених в оплату:  ", textCharFormat);
	cursor.insertText( QString::number(query->value(7).toInt()), textCharFormat_bold);
	cursor.insertBlock(blockFormat);
	cursor.insertText("      Наявність пільг:  ", textCharFormat);
	
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
		cursor.insertText( "немає", textCharFormat_bold);
	
	cursor.insertBlock(blockFormat);
	cursor.insertText("      Нарахування за комунальні послуги з урахуванням пільг (без субсидії) за місяць:", textCharFormat);
	cursor.insertBlock(blockFormat);
	query->exec("SELECT sum(Narah) \
				FROM narahuvannya \
				WHERE year="+QString::number(year)+" and month="+QString::number(month)+" and Rahunok="+QString::number(rahunok));
	query->seek(0);
	cursor.insertText("         1) за центральне опалення всього: ", textCharFormat);
	cursor.insertText(uMToStr2(query->value(0).toDouble()), textCharFormat_bold);
	query->exec("SELECT Lichylnyk FROM budynky WHERE id="+sqlStr(budId));
	query->next();
	cursor.insertText(" грн. ("+(query->value(0).toBool()?QString("є тепл.лічильник"):QString("немає тепл.лічильника"))+");\n", textCharFormat);
	cursor.insertText("         2) площа в межах норми споживання: ________ м2.;\n", textCharFormat);
	cursor.insertText("         3) в межах санітарної норми: "+(suma!=0 ? uMToStr2(suma) : QString("_____________"))+" грн.;\n", textCharFormat);
	query->exec("SELECT sum(Narah_GV) \
				FROM narahuvannya \
				WHERE year="+QString::number(year)+" and month="+QString::number(month)+" and Rahunok="+QString::number(rahunok));
	query->seek(0);
	cursor.insertText("         4) за гаряче водопостачання: ", textCharFormat);
	cursor.insertText(uMToStr2(query->value(0).toDouble()), textCharFormat_bold);
	query->exec("SELECT IsLichylnyk FROM lichylnykyAbon \
				WHERE Rahunok="+sqlStr(rahunok)+" and IsLichylnyk=true");
	cursor.insertText(" грн. ("+(query->next() ? QString("є абон.лічильник") : QString("немає абон.лічильника"))+").\n", textCharFormat);
	cursor.insertBlock(blockFormat);
	
	cursor.insertText("      Заборгованість отримувача: ", textCharFormat);
	q.exec("SELECT sum(Suma) FROM kvytancii \
			WHERE Rahunok_id="+sqlStr(rahunok)+" \
					and month(Plata_date)="+sqlStr(month)+" \
					and year(Plata_date)="+sqlStr(year));
	q.next();
	cursor.insertText(uMToStr2(vhSaldoInMonth(rahunok, year, month) - q.value(0).toDouble()), textCharFormat_bold);
	cursor.insertText(" грн.      Угода реструктуризації боргу: ", textCharFormat);
	q.exec("SELECT Kin_month, Kin_year  FROM spl_zaborg WHERE Rahunok_id="+QString::number(rahunok));
	if (q.next()){
		cursor.insertText("до " +QString::number(q.value(0).toInt()).rightJustified(2,'0')+"."+QString::number(q.value(1).toInt())+"р.", textCharFormat_bold);
	}
	else{
		cursor.insertText("немає", textCharFormat_bold);
	}
	cursor.insertBlock(blockFormat);
	cursor.insertText("      Довідка видана у відділ субсидії.", textCharFormat_bold);
	
	delete query;
	//Підписи
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("              "+DocsPosadaPidpysText(1)+"				"+PrizvIB(1), textCharFormat);
	cursor.insertText("\n\n", textCharFormat);
	cursor.insertText("              "+DocsPosadaPidpysText(2)+"				"+PrizvIB(2), textCharFormat);
	if (organizID() == 2) {
		cursor.insertText("\n\n", textCharFormat);
		cursor.insertText("              "+DocsPosadaPidpysText(6)+"				"+PrizvIB(6), textCharFormat);
	}
	
	printform->printer()->setDocName("ДОВІДКА у відділ субсидії за "+monthlist.at(month-1)+" "+QString::number(year)+"р.");
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
	postup_form->setPostupLabel("Читання і обробка інформації з бази даних");
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
	postup_form->setPostupLabel("Створення документа для друку");
	
	QDate date(year,month,1);
	
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat, textCharFormat_italic, textCharFormat_bold, cur_textCharFormat;
	
	textCharFormat_italic.setFontItalic( true );
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	QFont font = textCharFormat_bold.font();
	font.setBold(true);
	textCharFormat_bold.setFont(font);

	printform = new UPrintForm("Відомість нарахування, поступлення і боргу по населенню за "+monthlist.at(month-1)+" "+QString::number(year)+"р.", 10,10,10,10);
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 7 );
    cursor.insertText("Сформовано ПК \"ТКЕнерго менеджер\" (супровід Компанія \"АбонПро\". веб: www.abonpro.com). Час формування: "+QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy")+"р." , textCharFormat);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 3 );
	cursor.insertText(" ",textCharFormat);
	//Затверджено
	blockFormat.setAlignment( Qt::AlignCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat_bold.setFontPointSize( 13 );
	cursor.insertText("ВІДОМІСТЬ\nнарахування, поступлення і боргу по населенню", textCharFormat_bold);
	textCharFormat_bold.setFontPointSize( 10 );
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	cursor.insertText("за " + monthlist.at(month-1) + " " + QString::number(year)+"р.", textCharFormat_bold);
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 3 );
	cursor.insertText(" ",textCharFormat);
	textCharFormat.setFontPointSize( 10 );
	textCharFormat_italic.setFontPointSize( 10 );
	
	//Створення кістяка таблиці
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
		//ЗАПОВНЕННЯ ТАБЛИЦІ
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	tableCell.setAlignment(Qt::AlignHCenter);
	tableCell.setTextCharFormat(textCharFormat);
	tableCell.setTextBlockFormat(blockFormat);
	
	//Шапка таблиці
	cell = table->cellAt(0, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("№ з/п");
	
	cell = table->cellAt(0, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Найменування показника");
	
	cell = table->cellAt(0, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Кількість абонентів");
	
	cell = table->cellAt(0, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Сума, грн");
	postup_form->inc_pos();
	
	//перша графа
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("1.");
	
	//Кількість абонентів
	cell = table->cellAt(iter, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Загальна кількість особових рахунків");
	
	//Кількість абонентів
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
	cellCursor.insertText("   в т.ч. абоненти, що отримують послуги і ЦО, і ГВ");
	
	//Кількість абонентів
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
	cellCursor.insertText("   в т.ч. абоненти, що отримують послуги центального опалення");
	
	//Кількість абонентів
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
	cellCursor.insertText("   в т.ч. абоненти, що отримують послуги постачання гарячої води");
	
	//Кількість абонентів
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
	cellCursor.insertText("   в т.ч. абоненти, що отримують послуги ЦО без ГВ");
	
	//Кількість абонентів з опаленням без води
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
	cellCursor.insertText("   в т.ч. абоненти, що отримують послуги опалення від стояків");
	
	//Кількість абонентів що отримують послуги опалення від стояків
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
	cellCursor.insertText("   в т.ч. абоненти, що не отримують послуг (відключені)");
	
	//Кількість абонентів
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
	cellCursor.insertText("Кількість абонентів, які мають борг");
	
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
	//Кількість абонентів з які мають борг
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(count));
	
	//Загальна сума боргу
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
	cellCursor.insertText("   в т.ч. абоненти, що під'єднані до ЦО і мають борг");
	
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
	//Кількість абонентів з які мають борг
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(count));
	
	//Загальна сума боргу
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
	cellCursor.insertText("   в т.ч. абоненти, що під'єднані до ГВ і мають борг");
	
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
	//Кількість абонентів з які мають борг
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(count));
	
	//Загальна сума боргу
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
	cellCursor.insertText("   в т.ч. абоненти, що не отримують послуг і мають борг");
	
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
	//Кількість абонентів з які мають борг
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(count));
	
	//Загальна сума боргу
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
	cellCursor.insertText("   в т.ч. абоненти, з ЦО і без ГВ");
	
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
	//Кількість абонентів з які мають борг
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(count));
	
	//Загальна сума боргу
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
	cellCursor.insertText("Борг терміном більше 3-ьох місяців");
	
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
	//Кількість абонентів з які мають борг
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(count));
	
	//Загальна сума боргу
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
	cellCursor.insertText("   в т.ч. борг терміном більше 3-ьох місяців, абоненти з ЦО");
	
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
	//Кількість абонентів з які мають борг
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(count));
	
	//Загальна сума боргу
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
	cellCursor.insertText("   в т.ч. борг терміном більше 3-ьох місяців, абоненти з ГВ");
	
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
	//Кількість абонентів з які мають борг
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(count));
	
	//Загальна сума боргу
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
	cellCursor.insertText("   в т.ч. борг терміном більше 3-ьох місяців, абоненти з ЦО і без ГВ");
	
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
	//Кількість абонентів з які мають борг
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(count));
	
	//Загальна сума боргу
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
	cellCursor.insertText("   в т.ч. борг терміном більше 3-ьох місяців, для відключених абонентів");
	
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
	//Кількість абонентів з які мають борг
	cell = table->cellAt(iter, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(count));
	
	//Загальна сума боргу
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
	cellCursor.insertText("Вхідне сальдо станом на 01."+date.toString("MM.yyyy")+"р.", textCharFormat_bold);

	//Вхідне сальдо
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
	cellCursor.insertText("Нараховано за ЦО");

	//Нараховано за ЦО
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
	cellCursor.insertText("   в т.ч. Нарахування без пільги", textCharFormat_bold);

	//в т.ч. Нараховано без пільги
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
	cellCursor.insertText("   в т.ч. пільги");

	//пільги за ЦО
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
	cellCursor.insertText("Нараховано за ГВ");

	//Нараховано за ГВ
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
	cellCursor.insertText("   в т.ч. Нарахування за ГВ без пільги", textCharFormat_bold);

	//Нараховано за ГВ
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
	cellCursor.insertText("   в т.ч. пільги");
	
	
	//пільги за ГВ
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
	cellCursor.insertText("Нараховано за СТ", textCharFormat_bold);

	//Нараховано за СТ
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
	cellCursor.insertText("Коректура нарахувань по сумі");

	//Коректура по сумі
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
	cellCursor.insertText("Коректура нарахувань по площі");

	//Коректура по площі
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
	cellCursor.insertText("Нарахувано субсидії за місяць", textCharFormat_bold);

	//Нарахування субсидії
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
	cellCursor.insertText("   в т.ч. нарахування субсидії");

	//Нарахування субсидії
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
	cellCursor.insertText("   в т.ч. донарахування субсидії");

	//ДоНарахування субсидії
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
	cellCursor.insertText("Разом нараховано без субсидії");

	//Разом Нараховано без субсидії
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
	cellCursor.insertText("Оплата за місяць", textCharFormat_bold);

	//Оплата по квитанціях
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
	cellCursor.insertText("в т.ч. Звичайна оплата");

	//в.т.ч.Звичайна оплата
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
	cellCursor.insertText("в т.ч. Рішення суду");

	//в.т.ч.Рішення суду
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
	cellCursor.insertText("в т.ч. Зарплата");

	//в.т.ч.Зарплата
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
	cellCursor.insertText("в т.ч. Взаємозалік");

	//в.т.ч.Взаємозалік
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
	cellCursor.insertText("в т.ч. По угодах");

	//в.т.ч.Взаємозалік
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
	
	//Інформація про переведення заборгованості
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
	
	//Підсумкова стрічка переведення
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
	cellCursor.insertText("Переведення заборгованості", textCharFormat_bold);
	
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
		
		//стрічки переведення в т.ч.
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
		cellCursor.insertText("   в т.ч. "+query_2.value(0).toString());
	
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
	cellCursor.insertText("Вихідне сальдо станом на "+QString::number(date.daysInMonth())+"."+date.toString("MM.yyyy")+"р.", textCharFormat_bold);

	//Вихідне сальдо
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
	cellCursor.insertText("в т.ч. вихідне сальдо без нарах. пільг");

	//в т.ч. Вихідне сальдо без нарахованих пільг
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
	
	//Налаштування GUI з іні-файлу
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	// Розділ ПК ТКЕнерго менеджер::абонент
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
		
		//Безнадійна дебіторська заборгованість
		++iter;
		tableCell.setAlignment(Qt::AlignHCenter);
		tableCell.set(iter, 0, "15.");
		tableCell.setAlignment(Qt::AlignLeft);
		if (organizID() != 4)
			tableCell.set(iter, 1, "Безнадійна дебіторська заборгованість");
		else
			tableCell.set(iter, 1, "Списана дебіторська заборгованість");
		tableCell.setAlignment(Qt::AlignHCenter);
		tableCell.set(iter, 2, "-");
		tableCell.setAlignment(Qt::AlignRight);
		tableCell.set(iter, 3, "-");
		
		++iter;
		tableCell.setAlignment(Qt::AlignHCenter);
		tableCell.set(iter, 0, "");
		tableCell.setAlignment(Qt::AlignLeft);
		tableCell.set(iter, 1, "   Вхідне сальдо");
		tableCell.setAlignment(Qt::AlignHCenter);
		tableCell.set(iter, 2, "-");
		tableCell.setAlignment(Qt::AlignRight);
		tableCell.setDash(iter, 3, vhSaldoDeb);
	
		++iter;
		tableCell.setAlignment(Qt::AlignHCenter);
		tableCell.set(iter, 0, "");
		tableCell.setAlignment(Qt::AlignLeft);
		tableCell.set(iter, 1, "   Переведення заборгованості");
		tableCell.setAlignment(Qt::AlignHCenter);
		tableCell.set(iter, 2, "-");
		tableCell.setAlignment(Qt::AlignRight);
		tableCell.setDash(iter, 3, addZaborgDeb);
		
		++iter;
		tableCell.setAlignment(Qt::AlignHCenter);
		tableCell.set(iter, 0, "");
		tableCell.setAlignment(Qt::AlignLeft);
		tableCell.set(iter, 1, "   Оплата");
		tableCell.setAlignment(Qt::AlignHCenter);
		tableCell.set(iter, 2, "-");
		tableCell.setAlignment(Qt::AlignRight);
		tableCell.setDash(iter, 3, oplataDeb);
		
		++iter;
		tableCell.setAlignment(Qt::AlignHCenter);
		tableCell.set(iter, 0, "");
		tableCell.setAlignment(Qt::AlignLeft);
		tableCell.set(iter, 1, "   Вихідне сальдо");
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
	printform->printer()->setDocName("Відомість нарахування, поступлення і боргу по населенню за "+monthlist.at(month-1)+" "+QString::number(year)+"р.");
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
	postup_form->setPostupLabel("Створення відомості");
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
	//Створення документа тарифів для всіх будинків за X місяць, Y рік
	printform = new UPrintForm("Відомість нарахування для абонентів з лічильниками за "+monthlist.at(month-1)+" "+QString::number(year)+"р.");
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 7 );
    cursor.insertText("Сформовано ПК \"ТКЕнерго менеджер\" (супровід Компанія \"АбонПро\". веб: www.abonpro.com). Час формування: "+QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy")+"р." , textCharFormat);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertText(" ",textCharFormat);
	//Затверджено
	blockFormat.setAlignment( Qt::AlignCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat_bold.setFontPointSize( 13 );
	cursor.insertText("ВІДОМІСТЬ\nнарахування для абонентів з лічильниками", textCharFormat_bold);
	textCharFormat_bold.setFontPointSize( 10 );
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	cursor.insertText("за " + monthlist.at(month-1) + " " + QString::number(year)+"р.", textCharFormat_bold);
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 7 );
	cursor.insertText(" ",textCharFormat);
	textCharFormat.setFontPointSize( 10 );
	textCharFormat_italic.setFontPointSize( 10 );
	textCharFormat_bold.setFontPointSize( 10 );
	cursor.insertBlock(blockFormat);
	
	//Створення кістяка таблиці
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
	
	//Шапка таблиці
	cell = table->cellAt(0, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("№ з/п");
	
	cell = table->cellAt(0, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Рах.");
	
	cell = table->cellAt(0, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("П І Б");
	
	cell = table->cellAt(0, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Адреса");
	
	cell = table->cellAt(0, 4);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("ЦО, м.кв.");
	
	cell = table->cellAt(0, 5);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("ГВ, м.куб");
	
	cell = table->cellAt(0, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Сума ЦО, грн");
	
	cell = table->cellAt(0, 7);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Сума ГВ, грн");
	
	cell = table->cellAt(0, 8);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Сума, грн");
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
		//"№ з/п"
		cell = table->cellAt(iter, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QString::number(iter));
		
		//"рахунок"
		cell = table->cellAt(iter, 1);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(0).toString());
		
		//"ПІБ"
		cell = table->cellAt(iter, 2);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query.value(1).toString() + " " + query.value(2).toString() + " " + query.value(3).toString());
		
		//"адреса"
		cell = table->cellAt(iter, 3);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		adrStr = query.value(4).toString() + " " + query.value(5).toString();
		if (query.value(6).toInt()>0)
			adrStr += ", кв." + query.value(6).toString();
		cellCursor.insertText(adrStr);
		
		//"ЦО, м.кв."
		cell = table->cellAt(iter, 4);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		val = query.value(10).toDouble();
		sSum += val;
		cellCursor.insertText(QString::number(val).replace(".",","));
		
		//"м.куб"
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
		
		//"Сума ЦО грн"
		cell = table->cellAt(iter, 6);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		val = query.value(7).toDouble();
		opalSum += val;
		cellCursor.insertText(uMToStr2(val));

		//"Сума ГВ, грн"
		cell = table->cellAt(iter, 7);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignRight );
		cellCursor.setBlockFormat( blockFormat );
		val = query.value(8).toDouble();
		gvSum += val;
		cellCursor.insertText(uMToStr2(val));
	
		//"Сума, грн"
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
	
	//Слово Всього
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("Всього");
	
	//"ЦО, м.кв."
	cell = table->cellAt(iter, 4);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(sSum).replace(".",","));
		
	//"м.куб"
	cell = table->cellAt(iter, 5);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QString::number(kubSum).replace(".",","));
		
	//"Сума ЦО грн"
	cell = table->cellAt(iter, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(opalSum));

	//"Сума ГВ, грн"
	cell = table->cellAt(iter, 7);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(gvSum));
	
	//"Сума, грн"
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
	printform->printer()->setDocName("Відомість нарахування для абонентів з лічильниками за "+monthlist.at(month-1)+" "+QString::number(year)+"р.");
	printform->show();
}
//------------------------------------------------------------
UPrintDocs::~UPrintDocs()
{

}
//------------------------------------------------------------
