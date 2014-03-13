/****************************************************************************
**
** Created: Roman Mulyk 04.01.2007
**
****************************************************************************/

#include "uprint_docs_organiz.h"
#include <UPopulateTextTableCells>

//------------------------------------------------------------
UPrintDocsOrganiz::UPrintDocsOrganiz()
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

    monthListRod 	<< "січня"
                    << "лютого"
                    << "березня"
                    << "квітня"
                    << "травня"
                    << "червня"
                    << "липня"
                    << "серпня"
                    << "вересня"
                    << "жовтня"
                    << "листопада"
                    << "грудня";
}
//------------------------------------------------------------
void UPrintDocsOrganiz::print_vid_narah_za_gar_vodu(int cur_year, int cur_month)
{
	QSqlQuery *query = new QSqlQuery();
	//виведення форми поступу
	query->exec("SELECT count(*) FROM organizacii");
	query->seek(0);
	UPostupForm *postup_form = new UPostupForm(0, query->value(0).toInt() );
	postup_form->show();
	printform = new UPrintForm(0, codec->toUnicode("Відомість нарахування оплати за гарячу воду."));
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat;
	//Створення документа 
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 13 );
	cursor.insertText(codec->toUnicode("ВІДОМІСТЬ"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("нарахування оплати за гарячу воду\n")+OrganizName(), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 5 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("за ") + monthlist.at(cur_month-1) +
						codec->toUnicode(" ") + QVariant(cur_year).toString() + codec->toUnicode("р."), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 5 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	textCharFormat.setFontPointSize( 11 );
		//створення і налаштування таблиці
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
	constraints << QTextLength(QTextLength::FixedLength, 25);
	constraints << QTextLength(QTextLength::FixedLength, 25);
	constraints << QTextLength(QTextLength::FixedLength, 450);
	constraints << QTextLength(QTextLength::FixedLength, 80);
	constraints << QTextLength(QTextLength::FixedLength, 80);
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(2);
	QTextTable *table = cursor.insertTable(2, 5, tableFormat);
	//заповнення шапки таблиці
	cell = table->cellAt(0, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("№ п\\п"));
	
	cell = table->cellAt(0, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Угода"));
	
	cell = table->cellAt(0, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Назва організації"));
	
	cell = table->cellAt(0, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Нарахована сума"));
	
	cell = table->cellAt(0, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("В т.ч. ПДВ"));
	
	QSqlQuery *organizaciiQuery = new QSqlQuery();
	query->exec("SELECT value FROM slujb_znach WHERE id=1");
	query->seek(0);
	double pdv = query->value(0).toDouble();
	double narahOrg = 0, pdvOrg=0;
	organizaciiQuery->exec("SELECT Ugoda, Nazva FROM organizacii ORDER BY Ugoda");
	int row=1;
	double zagSumNarah=0, zagPdv=0;
	while(organizaciiQuery->next()){
		query->exec("SELECT sum(operacii.Narah_voda), count(*) \
					FROM pidrozdily \
					LEFT JOIN operacii \
						ON pidrozdily.id=operacii.Pidrozdil \
					WHERE operacii.year="+QVariant(cur_year).toString()+" \
						and operacii.month="+QVariant(cur_month).toString()+" \
						and pidrozdily.Ugoda="+organizaciiQuery->value(0).toString());
		query->seek(0);
		QString orgNameStr, NarahStr="", PdvStr="";
		if (d_to_m(query->value(0).toDouble())>0){
			short rowCount=query->value(1).toInt();
			narahOrg = d_to_m(query->value(0).toDouble());
			zagSumNarah += narahOrg;
			query->exec("SELECT pidrozdily.Name, operacii.Narah_voda \
							FROM pidrozdily \
							LEFT JOIN operacii \
								ON pidrozdily.id=operacii.Pidrozdil \
							WHERE operacii.year="+QVariant(cur_year).toString()+" \
								and operacii.month="+QVariant(cur_month).toString()+" \
								and pidrozdily.Ugoda="+organizaciiQuery->value(0).toString());
			orgNameStr = organizaciiQuery->value(1).toString()+" >> ";
			if (rowCount>1){
				NarahStr = uMToStr2(narahOrg);
				pdvOrg = d_to_m(narahOrg*pdv/(100+pdv));
				//zagPdv += pdvOrg;
				PdvStr = uMToStr2(pdvOrg);
			}
			while (query->next()){
				if (rowCount>1){
					orgNameStr += "\n    >> ";
					NarahStr += '\n';
					PdvStr += '\n';
				}
				orgNameStr += query->value(0).toString();
				narahOrg = query->value(1).toDouble();
				NarahStr += uMToStr2(narahOrg);
				pdvOrg = d_to_m(narahOrg*pdv/(100+pdv));
				zagPdv += pdvOrg;
				PdvStr += uMToStr2(pdvOrg);
			}
			//Занесення сформованих стрічок до таблці;
			table->insertRows( row, 1);
			
			cell = table->cellAt(row, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QVariant(row).toString());
			
			cell = table->cellAt(row, 1);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(organizaciiQuery->value(0).toString());
	
			cell = table->cellAt(row, 2);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(orgNameStr);
	
			cell = table->cellAt(row, 3);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(NarahStr);
	
			cell = table->cellAt(row, 4);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(PdvStr);
			
			row++;
		}
		postup_form->inc_pos();
	}

	//Зведене значення
	table->mergeCells ( row, 0, 1, 3 );
	
	cell = table->cellAt(row, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Всього по підприємству"));

	cell = table->cellAt(row, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(zagSumNarah));
	
	cell = table->cellAt(row, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(zagPdv));
	
	//Підписи
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("     "+DocsPosadaPidpysText(5)+"					"+PrizvIB(5));

			//Кінець створення документу для друку

	printform->printer()->setDocName("Відомість нарахування по ГВ за "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"р.");
	printform->show();
	delete query;
	delete organizaciiQuery;
	delete postup_form;
}
//------------------------------------------------------------
void UPrintDocsOrganiz::print_vidomist_narahuvannya_i_postuplennya_dohodiv(int cur_year, int cur_month)
{
	printform = new UPrintForm(0, codec->toUnicode("Відомість нарахування і поступлення доходів по підприємству."));
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat;
	QSqlQuery *query = new QSqlQuery();
	//Створення документа 
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 13 );
	cursor.insertText(codec->toUnicode("ВІДОМІСТЬ"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("нарахування і поступлення доходів по\n")+OrganizName(), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 5 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("за ") + monthlist.at(cur_month-1) +
						codec->toUnicode(" ") + QVariant(cur_year).toString() + codec->toUnicode("р."), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 5 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	textCharFormat.setFontPointSize( 11 );
		//створення і налаштування таблиці
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
	constraints << QTextLength(QTextLength::FixedLength, 25);
	constraints << QTextLength(QTextLength::FixedLength, 180);
	constraints << QTextLength(QTextLength::FixedLength, 95);
	constraints << QTextLength(QTextLength::FixedLength, 95);
	constraints << QTextLength(QTextLength::FixedLength, 95);
	constraints << QTextLength(QTextLength::FixedLength, 95);
	constraints << QTextLength(QTextLength::FixedLength, 95);
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(2);
	QTextTable *table = cursor.insertTable(10, 7, tableFormat);
	//заповнення шапки таблиці
	cell = table->cellAt(0, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("№ п\\п"));
	
	cell = table->cellAt(0, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Суб'єкти обслуговування"));
	
	cell = table->cellAt(0, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Вх.сальдо"));
	
	cell = table->cellAt(0, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Нараховано (опал.)"));
	
	cell = table->cellAt(0, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Нараховано (гар.вода)"));
	
	cell = table->cellAt(0, 5);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Оплата"));
	
	cell = table->cellAt(0, 6);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Борг"));
	
	double vhSaldo, narahOpal, narahVoda, oplata;
	double vhSaldoZag=0, narahOpalZag=0, narahVodaZag=0, oplataZag=0;
//Заповнення стрічки ПІДПРИЄМСТВА
	cell = table->cellAt(1, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("1."));
	
	cell = table->cellAt(1, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Підприємства"));
		//Вхідне сальдо
	query->exec("SELECT sum(saldo.Vh_saldo) \
					FROM organizacii \
					LEFT JOIN saldo \
						ON organizacii.Ugoda=saldo.Ugoda_id \
					WHERE year(saldo.CurDate)="+QVariant(cur_year).toString()+" \
						and month(saldo.CurDate)="+QVariant(cur_month).toString()+" \
						and organizacii.Finans=5");
	query->seek(0);
	cell = table->cellAt(1, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	vhSaldo = query->value(0).toDouble();
	cellCursor.insertText(uMToStr2(vhSaldo));
		//Нарахування за опалення і гарячу воду
	query->exec("SELECT sum(narah_table.Narah_opal), sum(narah_table.Narah_voda) \
				FROM organizacii \
				LEFT JOIN (SELECT pidrozdily.Ugoda, operacii.Narah_opal, operacii.Narah_voda \
						FROM pidrozdily \
						LEFT JOIN operacii \
							ON operacii.Pidrozdil=pidrozdily.id \
						WHERE operacii.Year="+QVariant(cur_year).toString()+" \
						and operacii.Month="+QVariant(cur_month).toString()+") AS narah_table \
					ON organizacii.Ugoda=narah_table.Ugoda \
				WHERE organizacii.Finans=5");
	query->seek(0);
	cell = table->cellAt(1, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	narahOpal = query->value(0).toDouble();
	cellCursor.insertText(uMToStr2(narahOpal));
	
	cell = table->cellAt(1, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	narahVoda = (query->value(1).toDouble());
	cellCursor.insertText(uMToStr2(narahVoda));
		//Оплата
	query->exec("SELECT sum(oplata.Suma) \
				FROM organizacii \
				LEFT JOIN oplata \
					ON organizacii.Ugoda=oplata.Organiz_id \
				WHERE year(Pererah_date)="+QVariant(cur_year).toString()+" \
					and month(Pererah_date)="+QVariant(cur_month).toString()+" \
					and organizacii.Finans=5");
	query->seek(0);
	cell = table->cellAt(1, 5);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	oplata = query->value(0).toDouble();
	cellCursor.insertText(uMToStr2(oplata));
	
	cell = table->cellAt(1, 6);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhSaldo + narahOpal + narahVoda - oplata));
	
	vhSaldoZag += vhSaldo;
	narahOpalZag += narahOpal;
	narahVodaZag += narahVoda;
	oplataZag += oplata;
	
//Заповнення стрічки БЮДЖЕТНІ ОРГАНІЗАЦІЇ
	cell = table->cellAt(2, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("2."));
	
	cell = table->cellAt(2, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Бюджетні організації"));
		//Вхідне сальдо
	query->exec("SELECT sum(saldo.Vh_saldo) \
					FROM organizacii \
					LEFT JOIN saldo \
						ON organizacii.Ugoda=saldo.Ugoda_id \
					WHERE year(saldo.CurDate)="+QVariant(cur_year).toString()+" \
						and month(saldo.CurDate)="+QVariant(cur_month).toString()+" \
						and (organizacii.Finans=1 or organizacii.Finans=2 or organizacii.Finans=3 or organizacii.Finans=4)");
	query->seek(0);
	cell = table->cellAt(2, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	vhSaldo = query->value(0).toDouble();
	cellCursor.insertText(uMToStr2(vhSaldo));
		//Нарахування за опалення і гарячу воду
	query->exec("SELECT sum(narah_table.Narah_opal), sum(narah_table.Narah_voda) \
				FROM organizacii \
				LEFT JOIN (SELECT pidrozdily.Ugoda, operacii.Narah_opal, operacii.Narah_voda \
						FROM pidrozdily \
						LEFT JOIN operacii \
							ON operacii.Pidrozdil=pidrozdily.id \
						WHERE operacii.Year="+QVariant(cur_year).toString()+" \
						and operacii.Month="+QVariant(cur_month).toString()+") AS narah_table \
					ON organizacii.Ugoda=narah_table.Ugoda \
				WHERE (organizacii.Finans=1 or organizacii.Finans=2 or organizacii.Finans=3 or organizacii.Finans=4)");
	query->seek(0);
	cell = table->cellAt(2, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	narahOpal = query->value(0).toDouble();
	cellCursor.insertText(uMToStr2(narahOpal));
	
	cell = table->cellAt(2, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	narahVoda = (query->value(1).toDouble());
	cellCursor.insertText(uMToStr2(narahVoda));
		//Оплата
	query->exec("SELECT sum(oplata.Suma) \
				FROM organizacii \
				LEFT JOIN oplata \
					ON organizacii.Ugoda=oplata.Organiz_id \
				WHERE year(Pererah_date)="+QVariant(cur_year).toString()+" \
					and month(Pererah_date)="+QVariant(cur_month).toString()+" \
					and (organizacii.Finans=1 or organizacii.Finans=2 or organizacii.Finans=3 or organizacii.Finans=4)");
	query->seek(0);
	cell = table->cellAt(2, 5);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	oplata = query->value(0).toDouble();
	cellCursor.insertText(uMToStr2(oplata));
	
	cell = table->cellAt(2, 6);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhSaldo + narahOpal + narahVoda - oplata));
	
	vhSaldoZag += vhSaldo;
	narahOpalZag += narahOpal;
	narahVodaZag += narahVoda;
	oplataZag += oplata;
	
	double vhSaldoNas, narahOpalNas, narahVodaNas, oplataNas;
	double vhsaldoUgody, narahCOUgody, narahGVUgody, oplataUgody, vyhSaldoUgody;
//Заповнення стрічки НАСЕЛЕННЯ
	query->exec("SELECT sum(Narah_opal), sum(Narah_voda) \
					FROM (organizacii \
					LEFT JOIN pidrozdily \
						ON pidrozdily.Ugoda=organizacii.Ugoda) \
					LEFT JOIN operacii \
						ON pidrozdily.id=operacii.Pidrozdil  \
					WHERE operacii.year="+QVariant(cur_year).toString()+" \
						and operacii.month="+QVariant(cur_month).toString()+" \
						and organizacii.Finans=6");
	query->seek(0);
	narahCOUgody = query->value(0).toDouble();
	narahGVUgody = query->value(1).toDouble();
	query->exec("SELECT sum(Vh_saldo) \
					FROM organizacii \
					LEFT JOIN saldo \
						ON saldo.Ugoda_id=organizacii.Ugoda \
					WHERE year(CurDate)="+QVariant(cur_year).toString()+" \
						and month(CurDate)="+QVariant(cur_month).toString()+" \
						and organizacii.Finans=6");
	query->seek(0);
	vhsaldoUgody = query->value(0).toDouble();
	query->exec("SELECT sum(Suma) \
				FROM organizacii \
				LEFT JOIN oplata \
					ON oplata.Organiz_id=organizacii.Ugoda \
				WHERE year(Pererah_date)="+QVariant(cur_year).toString()+" \
					and month(Pererah_date)="+QVariant(cur_month).toString()+" \
					and organizacii.Finans=6");
	query->seek(0);
	oplataUgody = query->value(0).toDouble();
	vyhSaldoUgody = vhsaldoUgody + narahCOUgody + narahGVUgody - oplataUgody;
	query->exec("SELECT sum(operacii_rahunkiv.Vh_saldo), sum(operacii_rahunkiv.Narah), sum(operacii_rahunkiv.Oplata) \
				FROM slujb_rahunky \
				LEFT JOIN operacii_rahunkiv \
					ON operacii_rahunkiv.Slujb_rahunok_id=slujb_rahunky.Rahunok \
				WHERE operacii_rahunkiv.year="+QVariant(cur_year).toString()+" \
					and operacii_rahunkiv.month="+QVariant(cur_month).toString()+" \
					and slujb_rahunky.Rahunok_type_id=3");
	query->seek(0);
	vhSaldoNas = query->value(0).toDouble();
	narahOpalNas = query->value(1).toDouble();
	narahVodaNas = 0;
	oplataNas = query->value(2).toDouble();
	
	cell = table->cellAt(3, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("3."));
	
	cell = table->cellAt(3, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Населення"));
	
	cell = table->cellAt(3, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhSaldoNas+vhsaldoUgody));
	
	cell = table->cellAt(3, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahOpalNas+narahCOUgody));
	
	cell = table->cellAt(3, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahVodaNas+narahGVUgody));
	
	cell = table->cellAt(3, 5);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(oplataNas+oplataUgody));
	
	cell = table->cellAt(3, 6);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhSaldoNas + narahOpalNas + narahVodaNas - oplataNas + vyhSaldoUgody));
	
	vhSaldoZag += vhSaldoNas + vhsaldoUgody;
	narahOpalZag += narahOpalNas + narahCOUgody;
	narahVodaZag += narahVodaNas + narahGVUgody;
	oplataZag += oplataNas + oplataUgody;
	
//Заповнення стрічки В ТОМУ ЧИСЛІ НАСЕЛЕННЯ В УГОДАХ
	cell = table->cellAt(4, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("  в т.ч. населення в угодах"));
	
	cell = table->cellAt(4, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhsaldoUgody));
	
	cell = table->cellAt(4, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahCOUgody));
	
	cell = table->cellAt(4, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahGVUgody));
	
	cell = table->cellAt(4, 5);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(oplataUgody));
	
	cell = table->cellAt(4, 6);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vyhSaldoUgody));
	
//Заповнення стрічки В ТОМУ ЧИСЛІ СУБСИДІЯ
	query->exec("SELECT sum(operacii_rahunkiv.Vh_saldo), sum(operacii_rahunkiv.Narah), sum(operacii_rahunkiv.Oplata) \
				FROM slujb_rahunky \
				LEFT JOIN operacii_rahunkiv \
					ON operacii_rahunkiv.Slujb_rahunok_id=slujb_rahunky.Rahunok \
				WHERE operacii_rahunkiv.year="+QVariant(cur_year).toString()+" \
					and operacii_rahunkiv.month="+QVariant(cur_month).toString()+" \
					and slujb_rahunky.Rahunok_type_id=1");
	query->seek(0);
	vhSaldo = query->value(0).toDouble();
	narahOpal = query->value(1).toDouble();
	narahVoda = 0;
	oplata = query->value(2).toDouble();
	
	cell = table->cellAt(5, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("  в т.ч. субсидій"));
	
	cell = table->cellAt(5, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhSaldo));
	
	cell = table->cellAt(5, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahOpal));
	
	cell = table->cellAt(5, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahVoda));
	
	cell = table->cellAt(5, 5);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(oplata));
	
	cell = table->cellAt(5, 6);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhSaldo + narahOpal + narahVoda - oplata));
	
//Заповнення стрічки В ТОМУ ЧИСЛІ НАСЕЛЕННЯ БЕЗ СУБСИДІЙ
	
	cell = table->cellAt(6, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("  в т.ч. населення без субсидій"));
	
	cell = table->cellAt(6, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhSaldoNas-vhSaldo+vhsaldoUgody));
	
	cell = table->cellAt(6, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahOpalNas-narahOpal+narahCOUgody));
	
	cell = table->cellAt(6, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahVodaNas-narahVoda+narahGVUgody));
	
	cell = table->cellAt(6, 5);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(oplataNas-oplata));
	
	cell = table->cellAt(6, 6);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2((vhSaldoNas + narahOpalNas + narahVodaNas - oplataNas) - (vhSaldo + narahOpal + narahVoda - oplata)+ vyhSaldoUgody));
	
//Заповнення стрічки ПІЛЬГИ
	query->exec("SELECT sum(operacii_rahunkiv.Vh_saldo), sum(operacii_rahunkiv.Narah), sum(operacii_rahunkiv.Oplata) \
				FROM slujb_rahunky \
				LEFT JOIN operacii_rahunkiv \
					ON operacii_rahunkiv.Slujb_rahunok_id=slujb_rahunky.Rahunok \
				WHERE operacii_rahunkiv.year="+QVariant(cur_year).toString()+" \
					and operacii_rahunkiv.month="+QVariant(cur_month).toString()+" \
					and slujb_rahunky.Rahunok_type_id=2");
	query->seek(0);
	vhSaldo = query->value(0).toDouble();
	narahOpal = query->value(1).toDouble();
	narahVoda = 0;
	oplata = query->value(2).toDouble();
	
	cell = table->cellAt(7, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("4."));
	
	cell = table->cellAt(7, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Пільги"));
	
	cell = table->cellAt(7, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhSaldo));
	
	cell = table->cellAt(7, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahOpal));
	
	cell = table->cellAt(7, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahVoda));
	
	cell = table->cellAt(7, 5);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(oplata));
	
	cell = table->cellAt(7, 6);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhSaldo + narahOpal + narahVoda - oplata));
	
	vhSaldoZag += vhSaldo;
	narahOpalZag += narahOpal;
	narahVodaZag += narahVoda;
	oplataZag += oplata;
	
//Заповнення стрічки ДОТАЦІЯ
	query->exec("SELECT sum(operacii_rahunkiv.Vh_saldo), sum(operacii_rahunkiv.Narah), sum(operacii_rahunkiv.Oplata) \
				FROM slujb_rahunky \
				LEFT JOIN operacii_rahunkiv \
					ON operacii_rahunkiv.Slujb_rahunok_id=slujb_rahunky.Rahunok \
				WHERE operacii_rahunkiv.year="+QVariant(cur_year).toString()+" \
					and operacii_rahunkiv.month="+QVariant(cur_month).toString()+" \
					and slujb_rahunky.Rahunok_type_id=4");
	query->seek(0);
	vhSaldo = query->value(0).toDouble();
	narahOpal = query->value(1).toDouble();
	narahVoda = 0;
	oplata = query->value(2).toDouble();
	
	cell = table->cellAt(8, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("5."));
	
	cell = table->cellAt(8, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Дотація"));
	
	cell = table->cellAt(8, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhSaldo));
	
	cell = table->cellAt(8, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahOpal));
	
	cell = table->cellAt(8, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahVoda));
	
	cell = table->cellAt(8, 5);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(oplata));
	
	cell = table->cellAt(8, 6);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhSaldo + narahOpal + narahVoda - oplata));
	
	vhSaldoZag += vhSaldo;
	narahOpalZag += narahOpal;
	narahVodaZag += narahVoda;
	oplataZag += oplata;

//Заповнення стрічки ВСЬОГО по п.1-п.5	
	
	cell = table->cellAt(9, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Всього по п.1-п.5"));
	
	cell = table->cellAt(9, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhSaldoZag));
	
	cell = table->cellAt(9, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahOpalZag));
	
	cell = table->cellAt(9, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahVodaZag));
	
	cell = table->cellAt(9, 5);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(oplataZag));
	
	cell = table->cellAt(9, 6);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhSaldoZag + narahOpalZag + narahVodaZag - oplataZag));	
	
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("     "+DocsPosadaPidpysText(5)+"					"+PrizvIB(5));
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("     "+DocsPosadaPidpysText(4)+"					"+PrizvIB(4));

			//Кінець створення документу для друку
	printform->printer()->setDocName("Відомість нарахування і поступлення доходів за "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"р.");
	printform->show();
	delete query;
}
//------------------------------------------------------------
void UPrintDocsOrganiz::print_nakopychuvalna_vidomist_narahuvannya_i_postuplennya_dohodiv(int cur_year, int cur_month)
{
	printform = new UPrintForm(0, codec->toUnicode("Відомість нарахування і поступлення доходів по підприємству."));
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat;
	QSqlQuery *query = new QSqlQuery();
	//Створення документа 
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 13 );
	cursor.insertText(codec->toUnicode("НАКОПИЧУВАЛЬНА ВІДОМІСТЬ"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("нарахування і поступлення доходів по\n")+OrganizName(), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 5 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("за період з січня по ") + monthlist.at(cur_month-1) +
						codec->toUnicode(" ") + QVariant(cur_year).toString() + codec->toUnicode("р."), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 5 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	textCharFormat.setFontPointSize( 11 );
		//створення і налаштування таблиці
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
	constraints << QTextLength(QTextLength::FixedLength, 25);
	constraints << QTextLength(QTextLength::FixedLength, 180);
	constraints << QTextLength(QTextLength::FixedLength, 95);
	constraints << QTextLength(QTextLength::FixedLength, 95);
	constraints << QTextLength(QTextLength::FixedLength, 95);
	constraints << QTextLength(QTextLength::FixedLength, 95);
	constraints << QTextLength(QTextLength::FixedLength, 95);
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(2);
	QTextTable *table = cursor.insertTable(10, 7, tableFormat);
	//заповнення шапки таблиці
	cell = table->cellAt(0, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("№ п\\п"));
	
	cell = table->cellAt(0, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Суб'єкти обслуговування"));
	
	cell = table->cellAt(0, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Вх.сальдо"));
	
	cell = table->cellAt(0, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Нараховано (опал.)"));
	
	cell = table->cellAt(0, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Нараховано (гар.вода)"));
	
	cell = table->cellAt(0, 5);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Оплата"));
	
	cell = table->cellAt(0, 6);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Борг"));
	
	double vhSaldo, narahOpal, narahVoda, oplata;
	double vhSaldoZag=0, narahOpalZag=0, narahVodaZag=0, oplataZag=0;
//Заповнення стрічки ПІДПРИЄМСТВА
	cell = table->cellAt(1, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("1."));
	
	cell = table->cellAt(1, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Підприємства"));
		//Вхідне сальдо
	query->exec("SELECT sum(saldo.Vh_saldo) \
					FROM organizacii \
					LEFT JOIN saldo \
						ON organizacii.Ugoda=saldo.Ugoda_id \
					WHERE year(saldo.CurDate)="+QVariant(cur_year).toString()+" \
						and month(saldo.CurDate)=1 \
						and organizacii.Finans=5");
	query->seek(0);
	cell = table->cellAt(1, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	vhSaldo = query->value(0).toDouble();
	cellCursor.insertText(uMToStr2(vhSaldo));
		//Нарахування за опалення і гарячу воду
	query->exec("SELECT sum(narah_table.Narah_opal), sum(narah_table.Narah_voda) \
				FROM organizacii \
				LEFT JOIN (SELECT pidrozdily.Ugoda, operacii.Narah_opal, operacii.Narah_voda \
						FROM pidrozdily \
						LEFT JOIN operacii \
							ON operacii.Pidrozdil=pidrozdily.id \
						WHERE operacii.Year="+QVariant(cur_year).toString()+" \
						and operacii.Month<="+QVariant(cur_month).toString()+") AS narah_table \
					ON organizacii.Ugoda=narah_table.Ugoda \
				WHERE organizacii.Finans=5");
	query->seek(0);
	cell = table->cellAt(1, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	narahOpal = query->value(0).toDouble();
	cellCursor.insertText(uMToStr2(narahOpal));
	
	cell = table->cellAt(1, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	narahVoda = (query->value(1).toDouble());
	cellCursor.insertText(uMToStr2(narahVoda));
		//Оплата
	query->exec("SELECT sum(oplata.Suma) \
				FROM organizacii \
				LEFT JOIN oplata \
					ON organizacii.Ugoda=oplata.Organiz_id \
				WHERE year(Pererah_date)="+QVariant(cur_year).toString()+" \
					and month(Pererah_date)<="+QVariant(cur_month).toString()+" \
					and organizacii.Finans=5");
	query->seek(0);
	cell = table->cellAt(1, 5);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	oplata = query->value(0).toDouble();
	cellCursor.insertText(uMToStr2(oplata));
	
	cell = table->cellAt(1, 6);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhSaldo + narahOpal + narahVoda - oplata));
	
	vhSaldoZag += vhSaldo;
	narahOpalZag += narahOpal;
	narahVodaZag += narahVoda;
	oplataZag += oplata;
	
//Заповнення стрічки БЮДЖЕТНІ ОРГАНІЗАЦІЇ
	cell = table->cellAt(2, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("2."));
	
	cell = table->cellAt(2, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Бюджетні організації"));
		//Вхідне сальдо
	query->exec("SELECT sum(saldo.Vh_saldo) \
					FROM organizacii \
					LEFT JOIN saldo \
						ON organizacii.Ugoda=saldo.Ugoda_id \
					WHERE year(saldo.CurDate)="+QVariant(cur_year).toString()+" \
						and month(saldo.CurDate)=1 \
						and (organizacii.Finans=1 or organizacii.Finans=2 or organizacii.Finans=3 or organizacii.Finans=4)");
	query->seek(0);
	cell = table->cellAt(2, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	vhSaldo = query->value(0).toDouble();
	cellCursor.insertText(uMToStr2(vhSaldo));
		//Нарахування за опалення і гарячу воду
	query->exec("SELECT sum(narah_table.Narah_opal), sum(narah_table.Narah_voda) \
				FROM organizacii \
				LEFT JOIN (SELECT pidrozdily.Ugoda, operacii.Narah_opal, operacii.Narah_voda \
						FROM pidrozdily \
						LEFT JOIN operacii \
							ON operacii.Pidrozdil=pidrozdily.id \
						WHERE operacii.Year="+QVariant(cur_year).toString()+" \
						and operacii.Month<="+QVariant(cur_month).toString()+") AS narah_table \
					ON organizacii.Ugoda=narah_table.Ugoda \
				WHERE (organizacii.Finans=1 or organizacii.Finans=2 or organizacii.Finans=3 or organizacii.Finans=4)");
	query->seek(0);
	cell = table->cellAt(2, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	narahOpal = query->value(0).toDouble();
	cellCursor.insertText(uMToStr2(narahOpal));
	
	cell = table->cellAt(2, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	narahVoda = (query->value(1).toDouble());
	cellCursor.insertText(uMToStr2(narahVoda));
		//Оплата
	query->exec("SELECT sum(oplata.Suma) \
				FROM organizacii \
				LEFT JOIN oplata \
					ON organizacii.Ugoda=oplata.Organiz_id \
				WHERE year(Pererah_date)="+QVariant(cur_year).toString()+" \
					and month(Pererah_date)<="+QVariant(cur_month).toString()+" \
					and (organizacii.Finans=1 or organizacii.Finans=2 or organizacii.Finans=3 or organizacii.Finans=4)");
	query->seek(0);
	cell = table->cellAt(2, 5);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	oplata = query->value(0).toDouble();
	cellCursor.insertText(uMToStr2(oplata));
	
	cell = table->cellAt(2, 6);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhSaldo + narahOpal + narahVoda - oplata));
	
	vhSaldoZag += vhSaldo;
	narahOpalZag += narahOpal;
	narahVodaZag += narahVoda;
	oplataZag += oplata;
	
	double vhSaldoNas, narahOpalNas, narahVodaNas, oplataNas;
	double vhsaldoUgody, narahCOUgody, narahGVUgody, oplataUgody, vyhSaldoUgody;
//Заповнення стрічки НАСЕЛЕННЯ
	query->exec("SELECT sum(Narah_opal), sum(Narah_voda) \
					FROM (organizacii \
					LEFT JOIN pidrozdily \
						ON pidrozdily.Ugoda=organizacii.Ugoda) \
					LEFT JOIN operacii \
						ON pidrozdily.id=operacii.Pidrozdil  \
					WHERE operacii.year="+QVariant(cur_year).toString()+" \
						and operacii.month<="+QVariant(cur_month).toString()+" \
						and operacii.month>=1 \
						and organizacii.Finans=6");
	query->next();
	narahCOUgody = query->value(0).toDouble();
	narahGVUgody = query->value(1).toDouble();
	query->exec("SELECT sum(Vh_saldo) \
					FROM organizacii \
					LEFT JOIN saldo \
						ON saldo.Ugoda_id=organizacii.Ugoda \
					WHERE year(CurDate)="+QVariant(cur_year).toString()+" \
						and month(CurDate)=1 \
						and organizacii.Finans=6");
	query->next();
	vhsaldoUgody = query->value(0).toDouble();
	query->exec("SELECT sum(Suma) \
				FROM organizacii \
				LEFT JOIN oplata \
					ON oplata.Organiz_id=organizacii.Ugoda \
				WHERE year(Pererah_date)="+QVariant(cur_year).toString()+" \
					and month(Pererah_date)<="+QVariant(cur_month).toString()+" \
					and month(Pererah_date)>=1 \
					and organizacii.Finans=6");
	query->seek(0);
	oplataUgody = query->value(0).toDouble();
	vyhSaldoUgody = vhsaldoUgody + narahCOUgody + narahGVUgody - oplataUgody;
	query->exec("SELECT sum(operacii_rahunkiv.Vh_saldo) \
				FROM slujb_rahunky \
				LEFT JOIN operacii_rahunkiv \
					ON operacii_rahunkiv.Slujb_rahunok_id=slujb_rahunky.Rahunok \
				WHERE operacii_rahunkiv.year="+QVariant(cur_year).toString()+" \
					and operacii_rahunkiv.month=1 \
					and slujb_rahunky.Rahunok_type_id=3");
	query->seek(0);
	vhSaldoNas = query->value(0).toDouble();
	query->exec("SELECT sum(operacii_rahunkiv.Narah), sum(operacii_rahunkiv.Oplata) \
				FROM slujb_rahunky \
				LEFT JOIN operacii_rahunkiv \
					ON operacii_rahunkiv.Slujb_rahunok_id=slujb_rahunky.Rahunok \
				WHERE operacii_rahunkiv.year="+QVariant(cur_year).toString()+" \
					and operacii_rahunkiv.month<="+QVariant(cur_month).toString()+" \
					and slujb_rahunky.Rahunok_type_id=3");
	query->seek(0);
	narahOpalNas = query->value(0).toDouble();
	narahVodaNas = 0;
	oplataNas = query->value(1).toDouble();
	
	cell = table->cellAt(3, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("3."));
	
	cell = table->cellAt(3, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Населення"));
	
	cell = table->cellAt(3, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhSaldoNas+vhsaldoUgody));
	
	cell = table->cellAt(3, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahOpalNas+narahCOUgody));
	
	cell = table->cellAt(3, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahVodaNas+narahGVUgody));
	
	cell = table->cellAt(3, 5);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(oplataNas+oplataUgody));
	
	cell = table->cellAt(3, 6);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhSaldoNas + narahOpalNas + narahVodaNas - oplataNas + vyhSaldoUgody));
	
	vhSaldoZag += vhSaldoNas+vhsaldoUgody;
	narahOpalZag += narahOpalNas+narahCOUgody;
	narahVodaZag += narahVodaNas+narahGVUgody;
	oplataZag += oplataNas+oplataUgody;
	
	//Заповнення стрічки В ТОМУ ЧИСЛІ НАСЕЛЕННЯ В УГОДАХ
	cell = table->cellAt(4, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("  в т.ч. населення в угодах"));
	
	cell = table->cellAt(4, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhsaldoUgody));
	
	cell = table->cellAt(4, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahCOUgody));
	
	cell = table->cellAt(4, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahGVUgody));
	
	cell = table->cellAt(4, 5);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(oplataUgody));
	
	cell = table->cellAt(4, 6);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vyhSaldoUgody));
	
//Заповнення стрічки В ТОМУ ЧИСЛІ СУБСИДІЯ
	query->exec("SELECT sum(operacii_rahunkiv.Vh_saldo) \
				FROM slujb_rahunky \
				LEFT JOIN operacii_rahunkiv \
					ON operacii_rahunkiv.Slujb_rahunok_id=slujb_rahunky.Rahunok \
				WHERE operacii_rahunkiv.year="+QVariant(cur_year).toString()+" \
					and operacii_rahunkiv.month=1 \
					and slujb_rahunky.Rahunok_type_id=1");
	query->seek(0);
	vhSaldo = query->value(0).toDouble();
	query->exec("SELECT sum(operacii_rahunkiv.Narah), sum(operacii_rahunkiv.Oplata) \
				FROM slujb_rahunky \
				LEFT JOIN operacii_rahunkiv \
					ON operacii_rahunkiv.Slujb_rahunok_id=slujb_rahunky.Rahunok \
				WHERE operacii_rahunkiv.year="+QVariant(cur_year).toString()+" \
					and operacii_rahunkiv.month<="+QVariant(cur_month).toString()+" \
					and slujb_rahunky.Rahunok_type_id=1");
	query->seek(0);
	narahOpal = query->value(0).toDouble();
	narahVoda = 0;
	oplata = query->value(1).toDouble();
	
	cell = table->cellAt(5, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("  в т.ч. субсидій"));
	
	cell = table->cellAt(5, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhSaldo));
	
	cell = table->cellAt(5, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahOpal));
	
	cell = table->cellAt(5, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahVoda));
	
	cell = table->cellAt(5, 5);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(oplata));
	
	cell = table->cellAt(5, 6);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhSaldo + narahOpal + narahVoda - oplata));
	
//Заповнення стрічки В ТОМУ ЧИСЛІ НАСЕЛЕННЯ БЕЗ СУБСИДІЙ
	
	cell = table->cellAt(6, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("  в т.ч. населення без субсидій"));
	
	cell = table->cellAt(6, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhSaldoNas-vhSaldo+vhsaldoUgody));
	
	cell = table->cellAt(6, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahOpalNas-narahOpal+narahCOUgody));
	
	cell = table->cellAt(6, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahVodaNas-narahVoda+narahGVUgody));
	
	cell = table->cellAt(6, 5);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(oplataNas-oplata));
	
	cell = table->cellAt(6, 6);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2((vhSaldoNas + narahOpalNas + narahVodaNas - oplataNas) - (vhSaldo + narahOpal + narahVoda - oplata)+ vyhSaldoUgody));
	
//Заповнення стрічки ПІЛЬГИ
	query->exec("SELECT sum(operacii_rahunkiv.Vh_saldo) \
				FROM slujb_rahunky \
				LEFT JOIN operacii_rahunkiv \
					ON operacii_rahunkiv.Slujb_rahunok_id=slujb_rahunky.Rahunok \
				WHERE operacii_rahunkiv.year="+QVariant(cur_year).toString()+" \
					and operacii_rahunkiv.month=1 \
					and slujb_rahunky.Rahunok_type_id=2");
	query->seek(0);
	vhSaldo = query->value(0).toDouble();
	query->exec("SELECT sum(operacii_rahunkiv.Narah), sum(operacii_rahunkiv.Oplata) \
				FROM slujb_rahunky \
				LEFT JOIN operacii_rahunkiv \
					ON operacii_rahunkiv.Slujb_rahunok_id=slujb_rahunky.Rahunok \
				WHERE operacii_rahunkiv.year="+QVariant(cur_year).toString()+" \
					and operacii_rahunkiv.month<="+QVariant(cur_month).toString()+" \
					and slujb_rahunky.Rahunok_type_id=2");
	query->seek(0);
	narahOpal = query->value(0).toDouble();
	narahVoda = 0;
	oplata = query->value(1).toDouble();
	
	cell = table->cellAt(7, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("4."));
	
	cell = table->cellAt(7, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Пільги"));
	
	cell = table->cellAt(7, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhSaldo));
	
	cell = table->cellAt(7, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahOpal));
	
	cell = table->cellAt(7, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahVoda));
	
	cell = table->cellAt(7, 5);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(oplata));
	
	cell = table->cellAt(7, 6);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhSaldo + narahOpal + narahVoda - oplata));
	
	vhSaldoZag += vhSaldo;
	narahOpalZag += narahOpal;
	narahVodaZag += narahVoda;
	oplataZag += oplata;
	
//Заповнення стрічки ДОТАЦІЯ
	query->exec("SELECT sum(operacii_rahunkiv.Vh_saldo) \
				FROM slujb_rahunky \
				LEFT JOIN operacii_rahunkiv \
					ON operacii_rahunkiv.Slujb_rahunok_id=slujb_rahunky.Rahunok \
				WHERE operacii_rahunkiv.year="+QVariant(cur_year).toString()+" \
					and operacii_rahunkiv.month=1 \
					and slujb_rahunky.Rahunok_type_id=4");
	query->seek(0);
	vhSaldo = query->value(0).toDouble();
	query->exec("SELECT sum(operacii_rahunkiv.Narah), sum(operacii_rahunkiv.Oplata) \
				FROM slujb_rahunky \
				LEFT JOIN operacii_rahunkiv \
					ON operacii_rahunkiv.Slujb_rahunok_id=slujb_rahunky.Rahunok \
				WHERE operacii_rahunkiv.year="+QVariant(cur_year).toString()+" \
					and operacii_rahunkiv.month<="+QVariant(cur_month).toString()+" \
					and slujb_rahunky.Rahunok_type_id=4");
	query->seek(0);
	narahOpal = query->value(0).toDouble();
	narahVoda = 0;
	oplata = query->value(1).toDouble();
	
	cell = table->cellAt(8, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("5."));
	
	cell = table->cellAt(8, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Дотація"));
	
	cell = table->cellAt(8, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhSaldo));
	
	cell = table->cellAt(8, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahOpal));
	
	cell = table->cellAt(8, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahVoda));
	
	cell = table->cellAt(8, 5);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(oplata));
	
	cell = table->cellAt(8, 6);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhSaldo + narahOpal + narahVoda - oplata));
	
	vhSaldoZag += vhSaldo;
	narahOpalZag += narahOpal;
	narahVodaZag += narahVoda;
	oplataZag += oplata;

//Заповнення стрічки ВСЬОГО по п.1-п.5	
	
	cell = table->cellAt(9, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Всього по п.1-п.5"));
	
	cell = table->cellAt(9, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhSaldoZag));
	
	cell = table->cellAt(9, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahOpalZag));
	
	cell = table->cellAt(9, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahVodaZag));
	
	cell = table->cellAt(9, 5);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(oplataZag));
	
	cell = table->cellAt(9, 6);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(vhSaldoZag + narahOpalZag + narahVodaZag - oplataZag));	
	
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("     "+DocsPosadaPidpysText(5)+"					"+PrizvIB(5));
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("     "+DocsPosadaPidpysText(4)+"					"+PrizvIB(4));

			//Кінець створення документу для друку
	printform->printer()->setDocName("Накопичувальна відомість за "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"р.");
	printform->show();
	delete query;
}
//------------------------------------------------------------
void UPrintDocsOrganiz::print_vidomist_oplaty_za_CO(int cur_year, int cur_month)
{
	QSqlQuery *query = new QSqlQuery();
	//виведення форми поступу
	query->exec("SELECT count(*) FROM organizacii");
	query->seek(0);
	UPostupForm *postup_form = new UPostupForm(0, query->value(0).toInt() );
	postup_form->show();
	printform = new UPrintForm(0, codec->toUnicode("Відомість нарахування оплати за ЦО."));
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat;
	//Створення документа 
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 13 );
	cursor.insertText(codec->toUnicode("ВІДОМІСТЬ"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("нарахування оплати за центральне опалення\n")+OrganizName(), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 5 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("за ") + monthlist.at(cur_month-1) +
						codec->toUnicode(" ") + QVariant(cur_year).toString() + codec->toUnicode("р."), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 5 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	textCharFormat.setFontPointSize( 11 );
		//створення і налаштування таблиці
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
	constraints << QTextLength(QTextLength::FixedLength, 25);
	constraints << QTextLength(QTextLength::FixedLength, 25);
	constraints << QTextLength(QTextLength::FixedLength, 450);
	constraints << QTextLength(QTextLength::FixedLength, 80);
	constraints << QTextLength(QTextLength::FixedLength, 80);
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(2);
	QTextTable *table = cursor.insertTable(2, 5, tableFormat);
	//заповнення шапки таблиці
	cell = table->cellAt(0, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("№ п\\п"));
	
	cell = table->cellAt(0, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Угода"));
	
	cell = table->cellAt(0, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Назва організації"));
	
	cell = table->cellAt(0, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Нарахована сума"));
	
	cell = table->cellAt(0, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("В т.ч. ПДВ"));
	
	QSqlQuery *organizaciiQuery = new QSqlQuery();
	query->exec("SELECT value FROM slujb_znach WHERE id=1");
	query->seek(0);
	double pdv = query->value(0).toDouble();
	double narahOrg = 0, pdvOrg=0;
	organizaciiQuery->exec("SELECT Ugoda, Nazva FROM organizacii ORDER BY Ugoda");
	int row=1;
	double zagSumNarah=0, zagPdv=0;
	while(organizaciiQuery->next()){
		query->exec("SELECT sum(operacii.Narah_opal), count(*) \
					FROM pidrozdily \
					LEFT JOIN operacii \
						ON pidrozdily.id=operacii.Pidrozdil \
					WHERE operacii.year="+QVariant(cur_year).toString()+" \
						and operacii.month="+QVariant(cur_month).toString()+" \
						and pidrozdily.Ugoda="+organizaciiQuery->value(0).toString());
		query->seek(0);
		QString orgNameStr, NarahStr="", PdvStr="";
		if (d_to_m(query->value(0).toDouble())>0){
			short rowCount=query->value(1).toInt();
			narahOrg = d_to_m(query->value(0).toDouble());
			zagSumNarah += narahOrg;
			query->exec("SELECT pidrozdily.Name, operacii.Narah_opal \
							FROM pidrozdily \
							LEFT JOIN operacii \
								ON pidrozdily.id=operacii.Pidrozdil \
							WHERE operacii.year="+QVariant(cur_year).toString()+" \
								and operacii.month="+QVariant(cur_month).toString()+" \
								and pidrozdily.Ugoda="+organizaciiQuery->value(0).toString());
			orgNameStr = organizaciiQuery->value(1).toString()+" -> ";
			if (rowCount>1){
				NarahStr = uMToStr2(narahOrg);
				pdvOrg = d_to_m(narahOrg*pdv/(100+pdv));
				//zagPdv += pdvOrg;
				PdvStr = uMToStr2(pdvOrg);
			}
			while (query->next()){
				if (rowCount>1){
					orgNameStr += "\n    >> ";
					NarahStr += '\n';
					PdvStr += '\n';
				}
				orgNameStr += query->value(0).toString();
				narahOrg = query->value(1).toDouble();
				NarahStr += uMToStr2(narahOrg);
				pdvOrg = d_to_m(narahOrg*pdv/(100+pdv));
				zagPdv += pdvOrg;
				PdvStr += uMToStr2(pdvOrg);
			}
			//Занесення сформованих стрічок до таблці;
			table->insertRows( row, 1);
			
			cell = table->cellAt(row, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QVariant(row).toString());
			
			cell = table->cellAt(row, 1);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(organizaciiQuery->value(0).toString());
	
			cell = table->cellAt(row, 2);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(orgNameStr);
	
			cell = table->cellAt(row, 3);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(NarahStr);
	
			cell = table->cellAt(row, 4);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(PdvStr);
			
			row++;
		}
		postup_form->inc_pos();
	}
	
	//Значення з житлових будинків
	table->insertRows( row, 1);
	
	cell = table->cellAt(row, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(QVariant(row).toString());
	
	cell = table->cellAt(row, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Сл."));
	
	cell = table->cellAt(row, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Усі житлові будинки"));
	
	organizaciiQuery->exec("SELECT sum(operacii_rahunkiv.Narah) \
							FROM slujb_rahunky \
							LEFT JOIN operacii_rahunkiv \
								ON operacii_rahunkiv.Slujb_rahunok_id=slujb_rahunky.Rahunok \
							WHERE operacii_rahunkiv.Year="+QVariant(cur_year).toString()+" and \
								operacii_rahunkiv.month="+QVariant(cur_month).toString()+" and \
								slujb_rahunky.Rahunok_type_id=3");
	organizaciiQuery->seek(0);
	narahOrg = organizaciiQuery->value(0).toDouble();
	zagSumNarah += narahOrg;
	cell = table->cellAt(row, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(narahOrg));
	
	pdvOrg = d_to_m(narahOrg*pdv/(100+pdv));
	zagPdv += pdvOrg;
	cell = table->cellAt(row, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(pdvOrg));
	
	row++;
	
	//Зведене значення
	table->mergeCells ( row, 0, 1, 3 );
	
	cell = table->cellAt(row, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Всього по підприємству"));

	cell = table->cellAt(row, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(zagSumNarah));
	
	cell = table->cellAt(row, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(zagPdv));
	
	//Підписи
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("     "+DocsPosadaPidpysText(5)+"					"+PrizvIB(5));

			//Кінець створення документу для друку

	printform->printer()->setDocName("Відомість оплати за ЦО за "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"р.");
	printform->show();
	delete query;
	delete organizaciiQuery;
	delete postup_form;
}
//------------------------------------------------------------
void UPrintDocsOrganiz::print_vidomist_realizacii(int cur_year, int cur_month)
{
	QSqlQuery *query = new QSqlQuery();
	QFont font;
	//виведення форми поступу
	query->exec("SELECT count(*) FROM organizacii");
	query->seek(0);
	int orgCount = query->value(0).toInt();
	query->exec("SELECT count(*) FROM slujb_rahunky");
	query->seek(0);
	orgCount += query->value(0).toInt();
	UPostupForm *postup_form = new UPostupForm(0, orgCount );
	postup_form->show();
	printform = new UPrintForm(0, codec->toUnicode("Відомість реалізації"));
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat, textCharFormat_bold;
	textCharFormat_bold.setFontPointSize( 9 );
	font = textCharFormat_bold.font();
	font.setBold( true );
	textCharFormat_bold.setFont(font);
	//Створення документа 
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 13 );
	textCharFormat_bold.setFontPointSize( 13 );
	cursor.insertText(codec->toUnicode("ВІДОМІСТЬ РЕАЛІЗАЦІЇ"), textCharFormat_bold);
	textCharFormat_bold.setFontPointSize( 9 );
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("по ")+OrganizName(), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 5 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("за ") + monthlist.at(cur_month-1) +
						codec->toUnicode(" ") + QVariant(cur_year).toString() + codec->toUnicode("р."), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 5 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	
		//створення і налаштування таблиці
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
	constraints << QTextLength(QTextLength::FixedLength, 25);
	constraints << QTextLength(QTextLength::FixedLength, 25);
	constraints << QTextLength(QTextLength::FixedLength, 200);
	constraints << QTextLength(QTextLength::FixedLength, 100);
	constraints << QTextLength(QTextLength::FixedLength, 80);
	constraints << QTextLength(QTextLength::FixedLength, 80);
	constraints << QTextLength(QTextLength::FixedLength, 80);
	constraints << QTextLength(QTextLength::FixedLength, 80);
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(2);
	QTextTable *table = cursor.insertTable(2, 8, tableFormat);
	textCharFormat.setFontPointSize( 9 );
	textCharFormat.setFontItalic( true );
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	//заповнення шапки таблиці
	cell = table->cellAt(0, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("№ п\\п"),textCharFormat);
	
	cell = table->cellAt(0, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Угода"),textCharFormat);
	
	cell = table->cellAt(0, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Назва організації"),textCharFormat);
	
	cell = table->cellAt(0, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Підрозділ"),textCharFormat);
	
	cell = table->cellAt(0, 4);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Вх.сальдо"),textCharFormat);
	
	cell = table->cellAt(0, 5);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Нараховано"),textCharFormat);
	
	cell = table->cellAt(0, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Оплачено"),textCharFormat);
	
	cell = table->cellAt(0, 7);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Вих.сальдо"),textCharFormat);
	
	textCharFormat.setFontItalic( false );
	
	int row=1, orgNum=1;
	int iter;
	QSqlQuery *organizaciiQuery = new QSqlQuery();
	organizaciiQuery->exec("SELECT organizacii.Ugoda, organizacii.Nazva, saldoEdited.Vh_saldo, \
								(SELECT sum(Suma) FROM oplata \
								WHERE oplata.Organiz_id=organizacii.Ugoda \
									and year(Pererah_date)="+QVariant(cur_year).toString()+" \
									and month(Pererah_date)="+QVariant(cur_month).toString()+") AS oplataEdited \
							FROM (organizacii \
							LEFT JOIN (SELECT Ugoda_id, Vh_saldo \
										FROM saldo \
										WHERE year(saldo.CurDate)="+QVariant(cur_year).toString()+" \
											and month(saldo.CurDate)="+QVariant(cur_month).toString()+") AS saldoEdited \
								ON saldoEdited.Ugoda_id=organizacii.Ugoda) \
							ORDER BY organizacii.Ugoda");
	double zagVhSaldo=0;
	double zagNarah=0;
	double zagOplata=0;
	double zagVyhSaldo=0;
	double VhSaldo, Narah, Oplata;
	double vyhSaldo=0;
	
	while (organizaciiQuery->next()){
		query->exec("SELECT count(*) FROM pidrozdily \
						WHERE Ugoda="+organizaciiQuery->value(0).toString());
		query->seek(0);
		int numPidrozdiliv = query->value(0).toInt();
		if (numPidrozdiliv>0){
			table->insertRows( row, numPidrozdiliv);
			if (numPidrozdiliv>1){
				table->mergeCells( row, 0, numPidrozdiliv, 1 );
				table->mergeCells( row, 1, numPidrozdiliv, 1 );
				table->mergeCells( row, 2, numPidrozdiliv, 1 );
				table->mergeCells( row, 4, numPidrozdiliv, 1 );
				table->mergeCells( row, 6, numPidrozdiliv, 1 );
				table->mergeCells( row, 7, numPidrozdiliv, 1 );
			}
			//Заповнення інформації про організацію
			cell = table->cellAt(row, 0);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QVariant(orgNum).toString(),textCharFormat);
	
			cell = table->cellAt(row, 1);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(organizaciiQuery->value(0).toString(),textCharFormat);
	
			cell = table->cellAt(row, 2);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(organizaciiQuery->value(1).toString(),textCharFormat);
			
			cell = table->cellAt(row, 4);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			VhSaldo = organizaciiQuery->value(2).toDouble();
			zagVhSaldo += VhSaldo;
			cellCursor.insertText(uMToStr2(VhSaldo),textCharFormat);
			
			vyhSaldo = d_to_m(organizaciiQuery->value(2).toDouble());
			
			cell = table->cellAt(row, 6);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			Oplata = organizaciiQuery->value(3).toDouble();
			zagOplata += Oplata; 
			cellCursor.insertText(uMToStr2(Oplata),textCharFormat);
			
			vyhSaldo -= d_to_m(Oplata);
			
			query->exec("SELECT pidrozdily.Name, oper.Narah \
							FROM pidrozdily \
							LEFT JOIN (SELECT Pidrozdil, Narah_opal+Narah_voda AS Narah \
											FROM operacii \
											WHERE operacii.Month="+QVariant(cur_month).toString()+" \
												and operacii.Year="+QVariant(cur_year).toString()+") AS oper \
								ON oper.Pidrozdil=pidrozdily.id \
							WHERE pidrozdily.Ugoda="+organizaciiQuery->value(0).toString()+"  \
							ORDER by pidrozdily.Name");
			for (iter=0; iter<numPidrozdiliv; iter++){
				query->next();
				
				//Заповнення частини підрозділу
				cell = table->cellAt(row+iter, 3);
				cell.setFormat(textCharFormat);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				QString pidrozdName = query->value(0).toString();
				if (pidrozdName.size()>0){
					cellCursor.insertText(pidrozdName,textCharFormat);
				}
				else{
					cellCursor.insertText("-",textCharFormat);
				}
				
				cell = table->cellAt(row+iter, 5);
				cell.setFormat(textCharFormat);
				cellCursor = cell.firstCursorPosition();
				blockFormat.setAlignment( Qt::AlignCenter );
				cellCursor.setBlockFormat( blockFormat );
				Narah = query->value(1).toDouble();
				zagNarah += Narah;
				cellCursor.insertText(uMToStr2(Narah),textCharFormat);
				
				vyhSaldo += d_to_m(Narah);
			}
			
			cell = table->cellAt(row, 7);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			zagVyhSaldo += vyhSaldo;
			cellCursor.insertText(uMToStr2(vyhSaldo),textCharFormat);
		}
		row+=numPidrozdiliv;
		orgNum++;
		postup_form->inc_pos();
		
	}
	
	//Зведені дані по організаціях і підприємствах
	table->insertRows( row, 1);
	table->mergeCells( row, 0, 1, 4 );
	
	cell = table->cellAt(row, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Всього по організаціях і підприємствах"),textCharFormat_bold);
	
	cell = table->cellAt(row, 4);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(zagVhSaldo),textCharFormat_bold);
	
	cell = table->cellAt(row, 5);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(zagNarah),textCharFormat_bold);
	
	cell = table->cellAt(row, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(zagOplata),textCharFormat_bold);
	
	cell = table->cellAt(row, 7);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(zagVyhSaldo),textCharFormat_bold);
	
	row++;
	
	//Розділ спеціальних рахунків
	textCharFormat_bold.setFontItalic( true );
	table->insertRows( row, 1);
	table->mergeCells( row, 0, 1, 8 );
	cell = table->cellAt(row, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Спеціальні рахунки"), textCharFormat_bold);
	textCharFormat_bold.setFontItalic( false );
	row++;
	
	double zagSlujbVhSaldo=0;
	double zagSlujbNarah=0;
	double zagSlujbOplata=0;
	double zagSlujbVyhSaldo=0;
	//населення
	query->exec("SELECT sum(Vh_saldo), sum(Narah), sum(Oplata) \
				FROM operacii_rahunkiv \
				RIGHT JOIN slujb_rahunky \
					ON slujb_rahunky.Rahunok=operacii_rahunkiv.Slujb_rahunok_id \
				WHERE year="+QVariant(cur_year).toString()+" \
					and month="+QVariant(cur_month).toString()+" \
					and Rahunok_type_id=3");
	query->seek(0);
	table->insertRows( row, 1);
	
	cell = table->cellAt(row, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("1.",textCharFormat);
	
	cell = table->cellAt(row, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("сл."),textCharFormat);
	
	cell = table->cellAt(row, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Населення"),textCharFormat);
	
	cell = table->cellAt(row, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("-"),textCharFormat);
	
	cell = table->cellAt(row, 4);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	VhSaldo = query->value(0).toDouble();
	zagSlujbVhSaldo += VhSaldo;
	vyhSaldo = VhSaldo;
	cellCursor.insertText(uMToStr2(VhSaldo),textCharFormat);
	
	cell = table->cellAt(row, 5);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	Narah = query->value(1).toDouble();
	zagSlujbNarah += Narah;
	vyhSaldo +=Narah;
	cellCursor.insertText(uMToStr2(Narah),textCharFormat);
	
	cell = table->cellAt(row, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	Oplata = query->value(2).toDouble();
	zagSlujbOplata += Oplata;
	vyhSaldo -= Oplata;
	cellCursor.insertText(uMToStr2(Oplata),textCharFormat);
	
	cell = table->cellAt(row, 7);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	zagSlujbVyhSaldo += vyhSaldo;
	cellCursor.insertText(uMToStr2(vyhSaldo),textCharFormat);
	
	row++;
	postup_form->inc_pos();
	
	//в т.ч.Субсидія
	query->exec("SELECT sum(Vh_saldo), sum(Narah), sum(Oplata) \
				FROM operacii_rahunkiv \
				RIGHT JOIN slujb_rahunky \
					ON slujb_rahunky.Rahunok=operacii_rahunkiv.Slujb_rahunok_id \
				WHERE year="+QVariant(cur_year).toString()+" \
					and month="+QVariant(cur_month).toString()+" \
					and Rahunok_type_id=1");
	query->seek(0);
	table->insertRows( row, 1);
	
	cell = table->cellAt(row, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("2.",textCharFormat);
	
	cell = table->cellAt(row, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("сл."),textCharFormat);
	
	cell = table->cellAt(row, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("в т.ч. субсидії"),textCharFormat);
	
	cell = table->cellAt(row, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("-"),textCharFormat);
	
	cell = table->cellAt(row, 4);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	VhSaldo = query->value(0).toDouble();
	//zagSlujbVhSaldo += VhSaldo;
	vyhSaldo = VhSaldo;
	cellCursor.insertText(uMToStr2(VhSaldo),textCharFormat);
	
	cell = table->cellAt(row, 5);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	Narah = query->value(1).toDouble();
	//zagSlujbNarah += Narah;
	vyhSaldo +=Narah;
	cellCursor.insertText(uMToStr2(Narah),textCharFormat);
	
	cell = table->cellAt(row, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	Oplata = query->value(2).toDouble();
	//zagSlujbOplata += Oplata;
	vyhSaldo -= Oplata;
	cellCursor.insertText(uMToStr2(Oplata),textCharFormat);
	
	cell = table->cellAt(row, 7);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	//zagSlujbVyhSaldo += vyhSaldo;
	cellCursor.insertText(uMToStr2(vyhSaldo),textCharFormat);
	
	row++;
	postup_form->inc_pos();
	
	//рахунки ПІЛЬГ
	query->exec("SELECT Rahunok, Rahunok_name, Vh_saldo, Narah, Oplata \
				FROM slujb_rahunky \
				LEFT JOIN (SELECT * FROM operacii_rahunkiv \
					WHERE year="+QVariant(cur_year).toString()+" \
						and month="+QVariant(cur_month).toString()+") AS oper ON slujb_rahunky.Rahunok=oper.Slujb_rahunok_id WHERE  Rahunok_type_id=2");
	iter = 3;
	while (query->next()){
		table->insertRows( row, 1);
	
		cell = table->cellAt(row, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(QVariant(iter).toString()+".",textCharFormat);
	
		cell = table->cellAt(row, 1);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query->value(0).toString(),textCharFormat);
		
		cell = table->cellAt(row, 2);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(query->value(1).toString(),textCharFormat);
	
		cell = table->cellAt(row, 3);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("-"),textCharFormat);
	
		cell = table->cellAt(row, 4);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		VhSaldo = query->value(2).toDouble();
		zagSlujbVhSaldo += VhSaldo;
		vyhSaldo = VhSaldo;
		cellCursor.insertText(uMToStr2(VhSaldo),textCharFormat);
	
		cell = table->cellAt(row, 5);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		Narah = query->value(3).toDouble();
		zagSlujbNarah += Narah;
		vyhSaldo +=Narah;
		cellCursor.insertText(uMToStr2(Narah),textCharFormat);
	
		cell = table->cellAt(row, 6);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		Oplata = query->value(4).toDouble();
		zagSlujbOplata += Oplata;
		vyhSaldo -= Oplata;
		cellCursor.insertText(uMToStr2(Oplata),textCharFormat);
	
		cell = table->cellAt(row, 7);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		zagSlujbVyhSaldo += vyhSaldo;
		cellCursor.insertText(uMToStr2(vyhSaldo),textCharFormat);
	
		row++;
		iter++;
		postup_form->inc_pos();
	}
	
	
		//Зведені дані спецрахунках
	table->insertRows( row, 1);
	table->mergeCells( row, 0, 1, 4 );
	
	cell = table->cellAt(row, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Всього по спецрахунках"),textCharFormat_bold);
	
	cell = table->cellAt(row, 4);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(zagSlujbVhSaldo),textCharFormat_bold);
	
	cell = table->cellAt(row, 5);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(zagSlujbNarah),textCharFormat_bold);
	
	cell = table->cellAt(row, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(zagSlujbOplata),textCharFormat_bold);
	
	cell = table->cellAt(row, 7);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(zagSlujbVyhSaldo),textCharFormat_bold);
	
	row++;
	
		//Зведені дані по відомості
	//table->insertRows( row, 1);
	table->mergeCells( row, 0, 1, 4 );
	
	cell = table->cellAt(row, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Всього по відомості"),textCharFormat_bold);
	
	cell = table->cellAt(row, 4);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(zagSlujbVhSaldo+zagVhSaldo),textCharFormat_bold);
	
	cell = table->cellAt(row, 5);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(zagSlujbNarah+zagNarah),textCharFormat_bold);
	
	cell = table->cellAt(row, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(zagSlujbOplata+zagOplata),textCharFormat_bold);
	
	cell = table->cellAt(row, 7);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(zagSlujbVyhSaldo+zagVyhSaldo),textCharFormat_bold);
	
	row++;
	
			//Підписи
	textCharFormat.setFontPointSize( 13 );
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("     "+DocsPosadaPidpysText(5)+"					"+PrizvIB(5));
	
	printform->printer()->setDocName("Відомість реалізації за "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"р.");
	printform->show();
	delete query;
	delete organizaciiQuery;
	delete postup_form;
}
//------------------------------------------------------------
void UPrintDocsOrganiz::print_rahunok_1(int cur_year, int cur_month, int ugodaNum, int isAvans, double avansSum)
{
	QSqlQuery *query = new QSqlQuery();
	QFont font;
	//виведення форми поступу
	printform = new UPrintForm(0, codec->toUnicode("Рахунок"));
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat, textCharFormat_bold, textCharFormat_italic, textCharFormat_small;
	textCharFormat_bold.setFontPointSize( 9 );
	font = textCharFormat_bold.font();
	font.setBold( true );
	textCharFormat_bold.setFont(font);
	textCharFormat_italic.setFontPointSize( 9 );
	textCharFormat_italic.setFontItalic(true);
	//Створення документа 
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 10 );
	textCharFormat_bold.setFontPointSize( 10 );
	textCharFormat_small.setFontPointSize( 1 );
	
	//визначення чи організація користується гарячою водою
	bool isGV=false;
	if (isAvans==0){
		query->exec("SELECT pidrozdily.Gar_voda \
					FROM organizacii \
					LEFT JOIN pidrozdily \
						ON pidrozdily.Ugoda=organizacii.Ugoda \
					WHERE organizacii.Ugoda="+QVariant(ugodaNum).toString());
		while (query->next()){
			isGV = isGV || query->value(0).toBool();
		}
	}
	
	//Частина постачальника
	cursor.insertText(codec->toUnicode(" "), textCharFormat_small);
	cursor.insertHtml("<hr>");
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("	Постачальник: "), textCharFormat_italic);
	cursor.insertText(OrganizName(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		Юр.Адреса: "), textCharFormat_italic);
	cursor.insertText(OrganizAdresa(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		Тел. "), textCharFormat_italic);
	cursor.insertText(OrganizTel(), textCharFormat);
	cursor.insertText(codec->toUnicode("   Е-пошта: "), textCharFormat_italic);
	cursor.insertText(OrganizEMail(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		Банк: "), textCharFormat_italic);
	cursor.insertText(OrganizBank(), textCharFormat);
	cursor.insertText(codec->toUnicode(", р/р:"), textCharFormat_italic);
	cursor.insertText(OrganizRozrahRahunok(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		ЄДРПОУ: "), textCharFormat_italic);
	cursor.insertText(OrganizEDRPOU(), textCharFormat);
	
	//Надпис угода
	cursor.insertHtml("<hr>");
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	if (isAvans==1)
		cursor.insertText(codec->toUnicode("АВАНСОВИЙ "), textCharFormat_bold);
	cursor.insertText(codec->toUnicode("РАХУНОК №_______ від \"___\"______________20___р."), textCharFormat_bold);
	
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	
	//Частина замовника
	query->exec("SELECT organizacii.Nazva, organizacii.Date_ugody, banky.Nazva, banky.Mfo, \
					organizacii.Rozr_rah, organizacii.Edrpou, organizacii.Finans \
				FROM organizacii \
				LEFT JOIN banky \
					ON banky.id=organizacii.Bank_ID \
				WHERE organizacii.Ugoda="+QVariant(ugodaNum).toString());
	query->seek(0);
	bool isNasel = false;
	if (query->value(6).toInt() == 6)
		isNasel = true;
	
	cursor.insertHtml("<hr>");
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("	Платник: "), textCharFormat_italic);
	cursor.insertText(query->value(0).toString(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		Угода: № "), textCharFormat_italic);
	cursor.insertText(QVariant(ugodaNum).toString(), textCharFormat);
	cursor.insertText(codec->toUnicode(" від "), textCharFormat_italic);
	cursor.insertText(query->value(1).toDate().toString("d.MM.yyyy")+codec->toUnicode("р."), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		Банк: "), textCharFormat_italic);
	cursor.insertText(query->value(2).toString(), textCharFormat);
	cursor.insertText(codec->toUnicode(", МФО "), textCharFormat_italic);
	cursor.insertText(query->value(3).toString(), textCharFormat);
	cursor.insertText(codec->toUnicode(", р/р "), textCharFormat_italic);
	cursor.insertText(query->value(4).toString(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		ЄДРПОУ: "), textCharFormat_italic);
	cursor.insertText(query->value(5).toString(), textCharFormat);
	cursor.insertHtml("<hr>");
	
	double pdv = OrganizPdv();
	
	//Частина оплати
		QTextTableFormat tableFormat;
		QVector<QTextLength> constraints;
		constraints << QTextLength(QTextLength::FixedLength, 250);
		constraints << QTextLength(QTextLength::FixedLength, 100);
		constraints << QTextLength(QTextLength::FixedLength, 100);
		constraints << QTextLength(QTextLength::FixedLength, 100);
		tableFormat.setColumnWidthConstraints(constraints);
		tableFormat.setAlignment(Qt::AlignHCenter);
		tableFormat.setCellSpacing(0);
		tableFormat.setCellPadding(2);
	if (isAvans==0){
		cursor.insertBlock(blockFormat);
		cursor.insertText(codec->toUnicode("	За ")+monthlist.at(cur_month-1)+" "
							+QVariant(cur_year).toString()+codec->toUnicode(" року"), textCharFormat);
		QTextTable *table = cursor.insertTable(7, 4, tableFormat);
		
		table->mergeCells( 1, 0, 1, 3 );
		table->mergeCells( 3, 0, 1, 3 );
		table->mergeCells( 4, 0, 1, 3 );
		table->mergeCells( 5, 0, 1, 3 );
		table->mergeCells( 6, 0, 1, 3 );
		
		//Заповнення шапки таблиці
				//Горизонтальна
		cell = table->cellAt(0, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Назва"), textCharFormat_italic);
	
		cell = table->cellAt(0, 1);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("К-сть Гкал"), textCharFormat_italic);
	
		cell = table->cellAt(0, 2);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Варт.одн.Гкал"), textCharFormat_italic);
	
		cell = table->cellAt(0, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Сума"), textCharFormat_italic);
				//вертикальна
		cell = table->cellAt(1, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Борг на початок місяця"), textCharFormat);
	
		cell = table->cellAt(2, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Нараховано:\n     в т.ч. за центральне опалення"), textCharFormat);
		if (isGV)
			cellCursor.insertText(codec->toUnicode("\n     в т.ч. за гарячу воду"), textCharFormat);
		
		cell = table->cellAt(3, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("	в.т.ч.ПДВ"), textCharFormat);
	
		cell = table->cellAt(4, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Оплачено за місяць"), textCharFormat);
		
		cell = table->cellAt(6, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("	в.т.ч.ПДВ"), textCharFormat);

		UNasTaryfInfo taryfNas = nasTaryf(cur_year, cur_month);
		UOrgTaryfInfo orgTaryf(cur_year, cur_month);
		double gKalOpal=orgTaryf.vart_g_kal(financeTypeForOrganiz(ugodaNum));
		double gKalVoda=orgTaryf.vart_g_kal_gar_vody;
		
		if (taryfNas.vart_g_kal == 0) {
			QMessageBox::critical(0,"Помилка тарифу","Нульовий тариф за ЦО для населення");
			taryfNas.vart_g_kal = 0.1;
		}
		if (gKalOpal == 0) {
			QMessageBox::critical(0,"Помилка тарифу","Нульовий тариф за ЦО для організацій");
			gKalOpal = 0.01;
		}
		
		//qDebug() << " gKalOpal: " << gKalOpal<<  " gKalVoda: " << gKalVoda;
		cell = table->cellAt(2, 2);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText('\n'+uMToStr2(isNasel ? taryfNas.vart_g_kal : gKalOpal), textCharFormat);
		if (isGV)
			cellCursor.insertText('\n'+uMToStr2(gKalVoda), textCharFormat);
		
		//Вхідне сальдо
		query->exec("SELECT sald.Vh_saldo \
					FROM organizacii \
					LEFT JOIN (SELECT * FROM saldo \
									WHERE year(CurDate)="+ QVariant(cur_year).toString() +" \
										and month(CurDate)="+ QVariant(cur_month).toString() +") AS sald ON sald.Ugoda_id=organizacii.Ugoda \
					WHERE organizacii.Ugoda="+QVariant(ugodaNum).toString());
		query->seek(0);
		double vhSaldo=query->value(0).toDouble();
		cell = table->cellAt(1, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(vhSaldo), textCharFormat);
		
		//Нарахування
		query->exec("SELECT sum(oper.Narah_opal), sum(oper.Narah_voda) \
						FROM pidrozdily \
						LEFT JOIN (SELECT * FROM operacii \
									WHERE year="+ QVariant(cur_year).toString() +" \
										and month="+ QVariant(cur_month).toString() +") AS oper ON oper.Pidrozdil=pidrozdily.id \
					WHERE pidrozdily.Ugoda="+QVariant(ugodaNum).toString());
		query->seek(0);
		double narahOpal=query->value(0).toDouble();
		double narahVoda=query->value(1).toDouble();
		cell = table->cellAt(2, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(narahOpal+narahVoda)+'\n'+uMToStr2(narahOpal), textCharFormat);
		if (isGV)
			cellCursor.insertText('\n'+uMToStr2(narahVoda), textCharFormat);
		
		//Кількість тепла
		cell = table->cellAt(2, 1);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText('\n'+uMToStr2(narahOpal/(isNasel ? taryfNas.vart_g_kal : gKalOpal)), textCharFormat);
		if (isGV)
			cellCursor.insertText('\n'+uMToStr2(narahVoda/gKalVoda), textCharFormat);
		//qDebug() << "narahOpal: " << narahOpal<< " gKalOpal: " << gKalOpal<< " narahVoda: " << narahVoda<< " gKalVoda: " << gKalVoda;
		
		
		//Оплата за місяць
		query->exec("SELECT sum(oplata.Suma) \
					FROM organizacii \
					LEFT JOIN oplata \
						ON oplata.Organiz_id=organizacii.Ugoda \
					WHERE Organizacii.Ugoda="+QVariant(ugodaNum).toString()+" \
						and year(oplata.Pererah_date)="+ QVariant(cur_year).toString() +" \
						and month(oplata.Pererah_date)="+ QVariant(cur_month).toString() +"");
		query->seek(0);
		double oplata = query->value(0).toDouble();
		cell = table->cellAt(4, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(oplata), textCharFormat);
		
		//ПДВ
		cell = table->cellAt(3, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2((narahOpal+narahVoda)*pdv/(100+pdv)), textCharFormat);
		
		avansSum = vhSaldo + narahOpal + narahVoda - oplata;
		
		//До оплати
		cell = table->cellAt(5, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		QString sumaDoOplaty(uMToStr2(avansSum));
		cellCursor.insertText(codec->toUnicode("До оплати:\n(")+numberToString(sumaDoOplaty)+')', textCharFormat_bold);
	
		cell = table->cellAt(5, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(avansSum), textCharFormat_bold);
	
		//ПДВ оплати
		cell = table->cellAt(6, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(avansSum*pdv/(100+pdv)), textCharFormat);
	}
	else{
		QTextTable *table = cursor.insertTable(2, 4, tableFormat);
		
		table->mergeCells( 0, 0, 1, 3 );
		table->mergeCells( 1, 0, 1, 3 );
		
		//До оплати
		cell = table->cellAt(0, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		QString sumaDoOplaty(uMToStr2(avansSum));
		cellCursor.insertText(codec->toUnicode("До оплати:\n(")+numberToString(sumaDoOplaty)+')', textCharFormat_bold);
	
		cell = table->cellAt(0, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(avansSum), textCharFormat_bold);
	
		//ПДВ оплати
		cell = table->cellAt(1, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("	в.т.ч.ПДВ"), textCharFormat);
		
		cell = table->cellAt(1, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(avansSum*pdv/(100+pdv)), textCharFormat);
	}
	
	
	//Підписи
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignCenter );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(DocsPosadaPidpysText(1)+"														"+PrizvIB(1));
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(DocsPosadaPidpysText(2)+"													"+PrizvIB(2));
	
	printform->printer()->setDocName("Рахунок за "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"р.");
	printform->show();
	delete query;
}
//------------------------------------------------------------
void UPrintDocsOrganiz::print_rahunok_2(int cur_year, int cur_month, int ugodaNum, int isAvans, double avansSum)
{
	QSqlQuery *query = new QSqlQuery();
	QFont font;
	//виведення форми поступу
	printform = new UPrintForm(0, codec->toUnicode("Рахунок"));
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat, textCharFormat_bold, textCharFormat_italic, textCharFormat_small;
	textCharFormat_bold.setFontPointSize( 9 );
	font = textCharFormat_bold.font();
	font.setBold( true );
	textCharFormat_bold.setFont(font);
	textCharFormat_italic.setFontPointSize( 9 );
	textCharFormat_italic.setFontItalic(true);
	//Створення документа 
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 10 );
	textCharFormat_bold.setFontPointSize( 10 );
	textCharFormat_small.setFontPointSize( 1 );
	
	//визначення чи організація користується гарячою водою
	bool isGV=false;
	if (isAvans==0){
		query->exec("SELECT pidrozdily.Gar_voda \
					FROM organizacii \
					LEFT JOIN pidrozdily \
						ON pidrozdily.Ugoda=organizacii.Ugoda \
					WHERE organizacii.Ugoda="+QVariant(ugodaNum).toString());
		while (query->next()){
			isGV = isGV || query->value(0).toBool();
		}
	}
	
	//Частина постачальника
	cursor.insertText(codec->toUnicode(" "), textCharFormat_small);
	cursor.insertHtml("<hr>");
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("	Постачальник: "), textCharFormat_italic);
	cursor.insertText(OrganizName(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		Юр.Адреса: "), textCharFormat_italic);
	cursor.insertText(OrganizAdresa(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		Тел. "), textCharFormat_italic);
	cursor.insertText(OrganizTel(), textCharFormat);
	cursor.insertText(codec->toUnicode("   Е-пошта: "), textCharFormat_italic);
	cursor.insertText(OrganizEMail(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		Банк: "), textCharFormat_italic);
	cursor.insertText(OrganizBank(), textCharFormat);
	cursor.insertText(codec->toUnicode(", р/р:"), textCharFormat_italic);
	cursor.insertText(OrganizRozrahRahunok(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		ЄДРПОУ: "), textCharFormat_italic);
	cursor.insertText(OrganizEDRPOU(), textCharFormat);
	
	//Надпис угода
	cursor.insertHtml("<hr>");
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	if (isAvans==1)
		cursor.insertText(codec->toUnicode("АВАНСОВИЙ "), textCharFormat_bold);
	cursor.insertText(codec->toUnicode("РАХУНОК №_______ від \"___\"______________20___р."), textCharFormat_bold);
	
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	
	//Частина замовника
	query->exec("SELECT organizacii.Nazva, organizacii.Date_ugody, banky.Nazva, banky.Mfo, \
					organizacii.Rozr_rah, organizacii.Edrpou \
				FROM organizacii \
				LEFT JOIN banky \
					ON banky.id=organizacii.Bank_ID \
				WHERE organizacii.Ugoda="+QVariant(ugodaNum).toString());
	query->seek(0);
	cursor.insertHtml("<hr>");
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("	Платник: "), textCharFormat_italic);
	cursor.insertText(query->value(0).toString(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		Угода: № "), textCharFormat_italic);
	cursor.insertText(QVariant(ugodaNum).toString(), textCharFormat);
	cursor.insertText(codec->toUnicode(" від "), textCharFormat_italic);
	cursor.insertText(query->value(1).toDate().toString("d.MM.yyyy")+codec->toUnicode("р."), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		Банк: "), textCharFormat_italic);
	cursor.insertText(query->value(2).toString(), textCharFormat);
	cursor.insertText(codec->toUnicode(", МФО "), textCharFormat_italic);
	cursor.insertText(query->value(3).toString(), textCharFormat);
	cursor.insertText(codec->toUnicode(", р/р "), textCharFormat_italic);
	cursor.insertText(query->value(4).toString(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		ЄДРПОУ: "), textCharFormat_italic);
	cursor.insertText(query->value(5).toString(), textCharFormat);
	cursor.insertHtml("<hr>");
	
	query->exec("SELECT value FROM slujb_znach WHERE id=1");
	query->seek(0);
	double pdv = query->value(0).toDouble();
	
	//Частина оплати
		QTextTableFormat tableFormat;
		QVector<QTextLength> constraints;
		constraints << QTextLength(QTextLength::FixedLength, 250);
		constraints << QTextLength(QTextLength::FixedLength, 100);
		constraints << QTextLength(QTextLength::FixedLength, 100);
		constraints << QTextLength(QTextLength::FixedLength, 100);
		tableFormat.setColumnWidthConstraints(constraints);
		tableFormat.setAlignment(Qt::AlignHCenter);
		tableFormat.setCellSpacing(0);
		tableFormat.setCellPadding(2);
	if (isAvans==0){
		cursor.insertBlock(blockFormat);
		cursor.insertText(codec->toUnicode("	За ")+monthlist.at(cur_month-1)+" "
							+QVariant(cur_year).toString()+codec->toUnicode(" року"), textCharFormat_bold);
		
		cursor.insertBlock(blockFormat);
		
		UOrgTaryfInfo orgTaryf(cur_year, cur_month);
		cursor.insertText("	Тариф:	за 1 Гкал. ЦО: "+uMToStr2(orgTaryf.vart_g_kal(financeTypeForOrganiz(ugodaNum))), textCharFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText("		за 1 м.кв.  ЦО: "+uMToStr2(orgTaryf.vart_kvadr_CO), textCharFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText("		за 1 Гкал.  ГВ: "+uMToStr2(orgTaryf.vart_g_kal_gar_vody), textCharFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText("		за 1 м.куб. ГВ: "+uMToStr2(orgTaryf.vart_kuba_GV), textCharFormat);
		
		QTextTable *table = cursor.insertTable(7, 4, tableFormat);
		
		table->mergeCells( 0, 0, 1, 3 );
		table->mergeCells( 1, 0, 1, 3 );
		table->mergeCells( 2, 0, 1, 3 );
		table->mergeCells( 3, 0, 1, 3 );
		table->mergeCells( 4, 0, 1, 3 );
		table->mergeCells( 5, 0, 1, 3 );
		table->mergeCells( 6, 0, 1, 3 );
		
		//Заповнення шапки таблиці
				//Горизонтальна
		cell = table->cellAt(0, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Назва"), textCharFormat_italic);
	
		
		//cell = table->cellAt(0, 1);
		//cellCursor = cell.firstCursorPosition();
		//blockFormat.setAlignment( Qt::AlignCenter );
		//cellCursor.setBlockFormat( blockFormat );
		//cellCursor.insertText(codec->toUnicode("К-сть Гкал"), textCharFormat_italic);
	
		//cell = table->cellAt(0, 2);
		//cellCursor = cell.firstCursorPosition();
		//blockFormat.setAlignment( Qt::AlignCenter );
		//cellCursor.setBlockFormat( blockFormat );
		//cellCursor.insertText(codec->toUnicode("Варт.одн.Гкал"), textCharFormat_italic);
	
		cell = table->cellAt(0, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Сума"), textCharFormat_italic);
				//вертикальна
		cell = table->cellAt(1, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Борг на початок місяця"), textCharFormat);
	
		cell = table->cellAt(2, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Нараховано:\n     в т.ч. за центральне опалення"), textCharFormat);
		if (isGV)
			cellCursor.insertText(codec->toUnicode("\n     в т.ч. за гарячу воду"), textCharFormat);
		
		cell = table->cellAt(3, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("	в.т.ч.ПДВ"), textCharFormat);
	
		cell = table->cellAt(4, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("Оплачено за місяць"), textCharFormat);
		
		cell = table->cellAt(6, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("	в.т.ч.ПДВ"), textCharFormat);

		//Вхідне сальдо
		query->exec("SELECT sald.Vh_saldo \
					FROM organizacii \
					LEFT JOIN (SELECT * FROM saldo \
									WHERE year(CurDate)="+ QVariant(cur_year).toString() +" \
										and month(CurDate)="+ QVariant(cur_month).toString() +") AS sald ON sald.Ugoda_id=organizacii.Ugoda \
					WHERE organizacii.Ugoda="+QVariant(ugodaNum).toString());
		query->seek(0);
		double vhSaldo=query->value(0).toDouble();
		cell = table->cellAt(1, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(vhSaldo), textCharFormat);
		
		//Нарахування
		query->exec("SELECT sum(oper.Narah_opal), sum(oper.Narah_voda) \
						FROM pidrozdily \
						LEFT JOIN (SELECT * FROM operacii \
									WHERE year="+ QVariant(cur_year).toString() +" \
										and month="+ QVariant(cur_month).toString() +") AS oper ON oper.Pidrozdil=pidrozdily.id \
					WHERE pidrozdily.Ugoda="+QVariant(ugodaNum).toString());
		query->seek(0);
		double narahOpal=query->value(0).toDouble();
		double narahVoda=query->value(1).toDouble();
		cell = table->cellAt(2, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(narahOpal+narahVoda)+'\n'+uMToStr2(narahOpal), textCharFormat);
		if (isGV)
			cellCursor.insertText('\n'+uMToStr2(narahVoda), textCharFormat);
		
		//Кількість тепла
		//cell = table->cellAt(2, 1);
		//cellCursor = cell.firstCursorPosition();
		//blockFormat.setAlignment( Qt::AlignCenter );
		//cellCursor.setBlockFormat( blockFormat );
		//cellCursor.insertText('\n'+uMToStr2(narahOpal/gKalOpal), textCharFormat);
		//if (isGV)
		//	cellCursor.insertText('\n'+uMToStr2(narahVoda/gKalVoda), textCharFormat);
		//qDebug() << "narahOpal: " << narahOpal<< " gKalOpal: " << gKalOpal<< " narahVoda: " << narahVoda<< " gKalVoda: " << gKalVoda;
		
		
		//Оплата за місяць
		query->exec("SELECT sum(oplata.Suma) \
					FROM organizacii \
					LEFT JOIN oplata \
						ON oplata.Organiz_id=organizacii.Ugoda \
					WHERE Organizacii.Ugoda="+QVariant(ugodaNum).toString()+" \
						and year(oplata.Pererah_date)="+ QVariant(cur_year).toString() +" \
						and month(oplata.Pererah_date)="+ QVariant(cur_month).toString() +"");
		query->seek(0);
		double oplata = query->value(0).toDouble();
		cell = table->cellAt(4, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(oplata), textCharFormat);
		
		//ПДВ
		cell = table->cellAt(3, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2((narahOpal+narahVoda)*pdv/(100+pdv)), textCharFormat);
		
		avansSum = vhSaldo + narahOpal + narahVoda - oplata;
		
		//До оплати
		cell = table->cellAt(5, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		QString sumaDoOplaty(uMToStr2(avansSum));
		cellCursor.insertText(codec->toUnicode("До оплати:\n(")+numberToString(sumaDoOplaty)+')', textCharFormat_bold);
	
		cell = table->cellAt(5, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(avansSum), textCharFormat_bold);
	
		//ПДВ оплати
		cell = table->cellAt(6, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(avansSum*pdv/(100+pdv)), textCharFormat);
	}
	else{
		QTextTable *table = cursor.insertTable(2, 4, tableFormat);
		
		table->mergeCells( 0, 0, 1, 3 );
		table->mergeCells( 1, 0, 1, 3 );
		
		//До оплати
		cell = table->cellAt(0, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		QString sumaDoOplaty(uMToStr2(avansSum));
		cellCursor.insertText(codec->toUnicode("До оплати:\n(")+numberToString(sumaDoOplaty)+')', textCharFormat_bold);
	
		cell = table->cellAt(0, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(avansSum), textCharFormat_bold);
	
		//ПДВ оплати
		cell = table->cellAt(1, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("	в.т.ч.ПДВ"), textCharFormat);
		
		cell = table->cellAt(1, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(avansSum*pdv/(100+pdv)), textCharFormat);
	}
	
	
	//Підписи
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignCenter );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(DocsPosadaPidpysText(1)+"														"+PrizvIB(1));
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(DocsPosadaPidpysText(2)+"														"+PrizvIB(2));
	
	printform->printer()->setDocName("Рахунок за "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"р.");
	printform->show();
	delete query;
}
//------------------------------------------------------------
void UPrintDocsOrganiz::print_vidomist_borjnykiv(QDate pochDate, QDate kincDate, int finansFromId, bool needMinSum, double minSum)
{
	kincDate.setDate(kincDate.year(), kincDate.month(), kincDate.daysInMonth());
	QSqlQuery *query = new QSqlQuery();
	QSqlQuery *organizQuery = new QSqlQuery();
	query->exec("SELECT count(*) FROM organizacii WHERE Finans="+QVariant(finansFromId).toString());
	query->seek(0);
	UPostupForm *postup_form = new UPostupForm(0, query->value(0).toInt() );
	
	QFont font;
	//виведення форми поступу
	printform = new UPrintForm(0, codec->toUnicode("Відомість боржників"));
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat, textCharFormat_bold, textCharFormat_italic, textCharFormat_small;
	textCharFormat_bold.setFontPointSize( 9 );
	font = textCharFormat_bold.font();
	font.setBold( true );
	textCharFormat_bold.setFont(font);
	textCharFormat_italic.setFontPointSize( 10 );
	textCharFormat_italic.setFontItalic(true);
	//Створення документа 
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 10 );
	textCharFormat_bold.setFontPointSize( 10 );
	textCharFormat_small.setFontPointSize( 5 );
	
	textCharFormat_bold.setFontPointSize( 11 );
	query->exec("SELECT Finans_name FROM finans_from WHERE id="+QVariant(finansFromId).toString());
	query->seek(0);
	cursor.insertText(codec->toUnicode("ВІДОМІСТЬ\nОРГАНІЗАЦІЙ БОРЖНИКІВ (")+query->value(0).toString()+")", textCharFormat_bold);
	textCharFormat_bold.setFontPointSize( 10 );
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode(" "), textCharFormat_small);
	cursor.insertBlock(blockFormat);
	if ((pochDate.year()==kincDate.year()) && (pochDate.month()==kincDate.month())){
		cursor.insertText(codec->toUnicode("станом на ")+monthlist.at(pochDate.month()-1)+" " + QVariant(pochDate.year()).toString()
							+codec->toUnicode("р."), textCharFormat);
	}
	else{
		cursor.insertText(codec->toUnicode("за період з ")+pochDate.toString("MM.yyyy") +
							codec->toUnicode("р. по ")+kincDate.toString("MM.yyyy")+codec->toUnicode("р."), textCharFormat);
	}
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode(" "), textCharFormat_small);
	
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
	constraints << QTextLength(QTextLength::FixedLength, 10);
	constraints << QTextLength(QTextLength::FixedLength, 20);
	constraints << QTextLength(QTextLength::FixedLength, 270);
	constraints << QTextLength(QTextLength::FixedLength, 80);
	constraints << QTextLength(QTextLength::FixedLength, 80);
	constraints << QTextLength(QTextLength::FixedLength, 80);
	constraints << QTextLength(QTextLength::FixedLength, 80);
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setAlignment(Qt::AlignHCenter);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(2);
	QTextTable *table = cursor.insertTable(2, 7, tableFormat);
	
	//Заповнення шапки таблиці
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	cell = table->cellAt(0, 0);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("№ п/п"), textCharFormat_italic);
	
	cell = table->cellAt(0, 1);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Угода"), textCharFormat_italic);
	
	cell = table->cellAt(0, 2);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("Назва організації"), textCharFormat_italic);
	
	cell = table->cellAt(0, 3);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	if ((pochDate.year()==kincDate.year()) && (pochDate.month()==kincDate.month()))
		cellCursor.insertText(codec->toUnicode("Борг на початок місяця"), textCharFormat_italic);
	else
		cellCursor.insertText(codec->toUnicode("Борг станом на 1.")+pochDate.toString("MM.yyyy")+codec->toUnicode("р."), textCharFormat_italic);
	
	cell = table->cellAt(0, 4);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	if ((pochDate.year()==kincDate.year()) && (pochDate.month()==kincDate.month()))
		cellCursor.insertText(codec->toUnicode("Нараховано за місяць"), textCharFormat_italic);
	else
		cellCursor.insertText(codec->toUnicode("Нараховано з початку періоду"), textCharFormat_italic);
	
	cell = table->cellAt(0, 5);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	if ((pochDate.year()==kincDate.year()) && (pochDate.month()==kincDate.month()))
		cellCursor.insertText(codec->toUnicode("Оплачено за місяць"), textCharFormat_italic);
	else
		cellCursor.insertText(codec->toUnicode("Оплачено за період"), textCharFormat_italic);
	
	cell = table->cellAt(0, 6);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	if ((pochDate.year()==kincDate.year()) && (pochDate.month()==kincDate.month()))
		cellCursor.insertText(codec->toUnicode("Борг на кінець місяця"), textCharFormat_italic);
	else
		cellCursor.insertText(codec->toUnicode("Борг станом на ")+kincDate.toString("dd.MM.yyyy")+codec->toUnicode("р."), textCharFormat_italic);
	
	organizQuery->exec("SELECT * FROM organizacii WHERE Finans="+QVariant(finansFromId).toString()+" ORDER BY Ugoda");
	int iter=1;
	double pochVhSaldo, monthVhSaldo, monthNarah, monthOplata, kincVyhSaldo, narah, oplata;
	double sumPochVhSaldo=0, sumNarah=0, sumOplata=0, sumKincVyhSaldo=0;
	double sumOsvitaPochVhSaldo=0, sumOsvitaNarah=0, sumOsvitaOplata=0, sumOsvitaKincVyhSaldo=0;
	double sumOhorZdorPochVhSaldo=0, sumOhorZdorNarah=0, sumOhorZdorOplata=0, sumOhorZdorKincVyhSaldo=0;
	while (organizQuery->next()){
		query->exec("SELECT Vh_Saldo FROM saldo \
					WHERE Ugoda_id="+organizQuery->value(1).toString()+" \
						and year(CurDate)="+QVariant(kincDate.year()).toString()+" \
						and month(CurDate)="+QVariant(kincDate.month()).toString());
		query->seek(0);
		monthVhSaldo = d_to_m(query->value(0).toDouble());
		query->exec("SELECT sum(Narah_opal+Narah_voda) \
					FROM (SELECT id FROM pidrozdily WHERE Ugoda="+organizQuery->value(1).toString()+") AS pidr \
					LEFT JOIN operacii \
						ON operacii.Pidrozdil=pidr.id \
					WHERE operacii.year="+QVariant(kincDate.year()).toString()+" \
						and month="+QVariant(kincDate.month()).toString());
		query->seek(0);
		monthNarah = d_to_m(query->value(0).toDouble());
		query->exec("SELECT sum(Suma) FROM oplata \
						WHERE Organiz_id="+organizQuery->value(1).toString()+" \
							and year(Pererah_date)="+QVariant(kincDate.year()).toString()+" \
							and month(Pererah_date)="+QVariant(kincDate.month()).toString());
		query->seek(0);
		monthOplata = d_to_m(query->value(0).toDouble());
		kincVyhSaldo = monthVhSaldo + monthNarah - monthOplata;
		if (((needMinSum==true) and (kincVyhSaldo>=minSum)) or (needMinSum==false)){
			table->insertRows( iter, 1);
			
			cell = table->cellAt(iter, 0);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(QVariant(iter).toString(),textCharFormat);
			
			cell = table->cellAt(iter, 1);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(organizQuery->value(1).toString(),textCharFormat);
			
			cell = table->cellAt(iter, 2);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(organizQuery->value(2).toString(),textCharFormat);
			
			query->exec("SELECT Vh_Saldo FROM saldo \
					WHERE Ugoda_id="+organizQuery->value(1).toString()+" \
						and year(CurDate)="+QVariant(pochDate.year()).toString()+" \
						and month(CurDate)="+QVariant(pochDate.month()).toString());
			query->seek(0);
			pochVhSaldo = d_to_m(query->value(0).toDouble());
			cell = table->cellAt(iter, 3);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(uMToStr2(pochVhSaldo),textCharFormat);
			
			query->exec("SELECT sum(Narah_opal+Narah_voda) \
						FROM (SELECT id FROM pidrozdily WHERE Ugoda="+organizQuery->value(1).toString()+") AS pidr \
						LEFT JOIN operacii ON operacii.Pidrozdil=pidr.id \
						WHERE cdate('01.'&operacii.month&'.'&operacii.year) \
							BETWEEN cdate('1."+pochDate.toString("MM.yyyy")+"') \
								AND cdate('"+kincDate.toString("dd.MM.yyyy")+"')");
			query->seek(0);
			narah = d_to_m(query->value(0).toDouble());
			cell = table->cellAt(iter, 4);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(uMToStr2(narah),textCharFormat);
			
			query->exec("SELECT sum(Suma) FROM oplata \
						WHERE Organiz_id="+organizQuery->value(1).toString()+" \
							and (Pererah_date BETWEEN cdate('1."+pochDate.toString("MM.yyyy")+"') \
								AND cdate('"+kincDate.toString("dd.MM.yyyy")+"'))");
			query->seek(0);
			oplata = d_to_m(query->value(0).toDouble());
			cell = table->cellAt(iter, 5);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(uMToStr2(oplata),textCharFormat);
			
			cell = table->cellAt(iter, 6);
			cell.setFormat(textCharFormat);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText(uMToStr2(kincVyhSaldo),textCharFormat);
			
			sumPochVhSaldo+=pochVhSaldo;
			sumNarah+=narah;
			sumOplata+=oplata;
			sumKincVyhSaldo+=kincVyhSaldo;
			
			if ((finansFromId==1) && (organizQuery->value(11).toInt()==1)){//освіта
				sumOsvitaPochVhSaldo+=pochVhSaldo;
				sumOsvitaNarah+=narah;
				sumOsvitaOplata+=oplata;
				sumOsvitaKincVyhSaldo+=kincVyhSaldo;
			}
			else if ((finansFromId==1) && (organizQuery->value(11).toInt()==2)){//охорона здоров'я
				sumOhorZdorPochVhSaldo+=pochVhSaldo;
				sumOhorZdorNarah+=narah;
				sumOhorZdorOplata+=oplata;
				sumOhorZdorKincVyhSaldo+=kincVyhSaldo;
			}
			
			iter++;
		}
		postup_form->inc_pos();
	}
	//Всього за місяць
	table->mergeCells( iter, 0, 1, 3 );
	textCharFormat_bold.setFontPointSize( 10 );
	
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	if ((pochDate.year()==kincDate.year()) && (pochDate.month()==kincDate.month()))
		cellCursor.insertText(codec->toUnicode("Всього за місяць"),textCharFormat_bold);
	else
		cellCursor.insertText(codec->toUnicode("Всього за період"),textCharFormat_bold);
	
	cell = table->cellAt(iter, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sumPochVhSaldo),textCharFormat_bold);
	
	cell = table->cellAt(iter, 4);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sumNarah),textCharFormat_bold);
	
	cell = table->cellAt(iter, 5);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sumOplata),textCharFormat_bold);
	
	cell = table->cellAt(iter, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(uMToStr2(sumKincVyhSaldo),textCharFormat_bold);
	iter++;
	
	if (finansFromId==1){
		table->insertRows (iter, 2 );
		//в т.ч. освіта
		table->mergeCells( iter, 0, 1, 3 );
		textCharFormat_bold.setFontPointSize( 10 );
	
		cell = table->cellAt(iter, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("    в т.ч. ОСВІТА"),textCharFormat_bold);
	
		cell = table->cellAt(iter, 3);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(sumOsvitaPochVhSaldo),textCharFormat_bold);
	
		cell = table->cellAt(iter, 4);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(sumOsvitaNarah),textCharFormat_bold);
	
		cell = table->cellAt(iter, 5);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(sumOsvitaOplata),textCharFormat_bold);
	
		cell = table->cellAt(iter, 6);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(sumOsvitaKincVyhSaldo),textCharFormat_bold);
		iter++;
	
		//в т.ч. охорона здоров'я
		table->mergeCells( iter, 0, 1, 3 );
		textCharFormat_bold.setFontPointSize( 10 );

		cell = table->cellAt(iter, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("    в т.ч. ОХОРОНА ЗДОРОВ'Я"),textCharFormat_bold);

		cell = table->cellAt(iter, 3);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(sumOhorZdorPochVhSaldo),textCharFormat_bold);
	
		cell = table->cellAt(iter, 4);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(sumOhorZdorNarah),textCharFormat_bold);
	
		cell = table->cellAt(iter, 5);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(sumOhorZdorOplata),textCharFormat_bold);
	
		cell = table->cellAt(iter, 6);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(sumOhorZdorKincVyhSaldo),textCharFormat_bold);
		iter++;
	}
	
	//Підписи
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("		"+DocsPosadaPidpysText(5)+"				"+PrizvIB(5), textCharFormat);
	
	printform->printer()->setDocName("Відомість боржників");
	printform->show();
	delete postup_form;
	delete organizQuery;
	delete query;
}
//------------------------------------------------------------
UPrintDocsOrganiz::~UPrintDocsOrganiz()
{

}
//------------------------------------------------------------
