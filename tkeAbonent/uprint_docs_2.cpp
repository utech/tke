/****************************************************************************
**
** Created: Roman Mulyk 30.12.2008
**
****************************************************************************/

#include "uprint_docs.h"
#include <UPopulateTextTableCells>
#include <USqlAccessible>

//------------------------------------------------------------
void UPrintDocs::print_vidomistOtrymannyaPoslugPoBudynkah(short year, short month)
{
	QSqlQuery query;
	query.exec("SELECT count(*) FROM budynky");
	query.seek(0);
	int rowCount = query.value(0).toInt();
	UPostupForm *postupForm = new UPostupForm(0, rowCount);
	postupForm->show();
	
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
	printform = new UPrintForm("Відомість отримання послуг по будинках за "+monthlist.at(month-1)+" "+QString::number(year)+"р.",7,7,7,10);
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
	cursor.insertText("Відомість отримання послуг по будинках за "+monthlist.at(month-1)+" "+QString::number(year)+"р.", textCharFormat_bold);
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
	
	constraints << QTextLength(QTextLength::FixedLength, 30);
	constraints << QTextLength(QTextLength::FixedLength, 160);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(1);
	tableFormat.setBorder(0.5);
	tableFormat.setHeaderRowCount(3);
	QTextTable *table = cursor.insertTable(4+rowCount, 15, tableFormat);
	
	table->mergeCells ( 0, 0, 3, 1 );
	table->mergeCells ( 0, 1, 3, 1 );
	table->mergeCells ( 0, 2, 1, 4 );
	table->mergeCells ( 1, 2, 1, 2 );
	table->mergeCells ( 1, 4, 1, 2 );
	
	table->mergeCells ( 0, 6, 1, 6 );
	table->mergeCells ( 1, 6, 2, 1 );
	table->mergeCells ( 1, 7, 2, 1 );
	table->mergeCells ( 1, 8, 2, 1 );
	table->mergeCells ( 1, 9, 2, 1 );
	table->mergeCells ( 1, 10, 2, 1 );
	table->mergeCells ( 1, 11, 2, 1 );
	
	table->mergeCells ( 0, 12, 1, 3 );
	table->mergeCells ( 1, 12, 2, 1 );
	table->mergeCells ( 1, 13, 2, 1 );
	table->mergeCells ( 1, 14, 2, 1 );
	
	UPopulateCell tableCell(table);
	tableCell.setAlignment(Qt::AlignHCenter);
	tableCell.setTextCharFormat(textCharFormat);
	tableCell.setTextBlockFormat(blockFormat);
	
	tableCell.set(0,0,"№ з/п");
	tableCell.set(0,1,"Будинок");
	tableCell.set(0,2,"Гаряча вода");
	tableCell.set(1,2,"Без лічильника");
	tableCell.set(2,2,"Кільк. абон.");
	tableCell.set(2,3,"Кільк. проп.");
	tableCell.set(1,4,"Лічильник");
	tableCell.set(2,4,"Кільк. абон.");
	tableCell.set(2,5,"Викор.води, м.куб.");
	tableCell.set(0,6,"Центральне опалення");
	tableCell.set(1,6,"Використ. ГКал ЦО+СТ");
	tableCell.set(1,7,"Заг.Опал. площа ЦО+СТ");
	tableCell.set(1,8,"Оп. Площа без ліч.");
	tableCell.set(1,9,"Оп. Площа з ліч.");
	tableCell.set(1,10,"К-сть під'єдн. абон. ЦО");
	tableCell.set(1,11,"К-сть від'єдн. абон. ЦО");
	tableCell.set(0,12,"Опал від стояків");
	tableCell.set(1,12,"К-сть абон.");
	tableCell.set(1,13,"Площа, м.кв.");
	tableCell.set(1,14,"Викор. тепла, ГКал");
	
	query.setForwardOnly(true);
	query.exec("SELECT budynky.id, budynky.Vulycia, budynky.Bud_num, budynky.Lichylnyk, \
						budynky.Koef_pereved, opalPlosTable.PlosCO, opal_st_ploshcha_bud.ZagStPloshcha, opal_st_ploshcha_bud.ZagStGKal, \
						abonCOCountTable.abonCOCount, abonNoCOCountTable.abonNoCOCount, diuchTar.Lich_kin, diuchTar.Lich_poch, \
						opal_st_ploshcha_bud.abonSTCountReal, abonSTCountTable.abonSTCount, GVLichSpoj.ZagVykorVody, GVLichSpoj.ZagKilkLudei, \
						GVLichSpoj.abonCount, GVNoLichSpoj.ZagVykorVody, GVNoLichSpoj.ZagKilkLudei, GVNoLichSpoj.abonCount \
				FROM ((((((((budynky \
				LEFT JOIN (SELECT Budynok_ID, sum(Ploshcha) AS PlosCO FROM abonent WHERE opal=true GROUP BY Budynok_ID) AS opalPlosTable ON opalPlosTable.Budynok_ID=budynky.id) \
				LEFT JOIN (SELECT abonent.Budynok_ID, sum(pok.Plos) AS ZagStPloshcha, sum(GKal) AS ZagStGKal, count(*) AS abonSTCountReal FROM (SELECT Rahunok_ID, Plos, GKal  \
							FROM pokaznOpalStoyak  \
							WHERE Year="+sqlStr(year)+" and Month="+sqlStr(month)+") AS pok  \
							LEFT JOIN abonent ON abonent.Rahunok=pok.Rahunok_ID  \
							GROUP BY abonent.Budynok_ID) AS opal_st_ploshcha_bud ON budynky.id = opal_st_ploshcha_bud.Budynok_ID) \
				LEFT JOIN (SELECT Budynok_ID, count(*) AS abonCOCount FROM abonent WHERE opal=true GROUP BY Budynok_ID) AS abonCOCountTable ON abonCOCountTable.Budynok_ID=budynky.id) \
				LEFT JOIN (SELECT Budynok_ID, count(*) AS abonNoCOCount FROM abonent WHERE opal=false GROUP BY Budynok_ID) AS abonNoCOCountTable ON abonNoCOCountTable.Budynok_ID=budynky.id) \
				LEFT JOIN (SELECT Budynok_ID, Lich_kin, Lich_poch FROM diuch_taryf \
							WHERE year="+sqlStr(year)+" and month="+sqlStr(month)+") AS diuchTar ON diuchTar.Budynok_ID=budynky.id) \
				LEFT JOIN (SELECT Budynok_ID, count(*) AS abonSTCount FROM abonent WHERE NarahStoyak=true GROUP BY Budynok_ID) AS abonSTCountTable ON abonSTCountTable.Budynok_ID=budynky.id) \
				LEFT JOIN (SELECT abonent.Budynok_ID, sum(spojGV.VykorVody) AS ZagVykorVody, sum(spojGV.Ludy) AS ZagKilkLudei, count(*) AS abonCount FROM (SELECT Rahunok_ID, VykorVody, Ludy  \
							FROM spojyvannyaGV  \
							WHERE Year="+sqlStr(year)+" and Month="+sqlStr(month)+" and IsLichGV=true and IsGarVoda=true) AS spojGV  \
							LEFT JOIN abonent ON abonent.Rahunok=spojGV.Rahunok_ID  \
							GROUP BY abonent.Budynok_ID) AS GVLichSpoj ON budynky.id = GVLichSpoj.Budynok_ID) \
				LEFT JOIN (SELECT abonent.Budynok_ID, sum(spojGV.VykorVody) AS ZagVykorVody, sum(spojGV.Ludy) AS ZagKilkLudei, count(*) AS abonCount FROM (SELECT Rahunok_ID, VykorVody, Ludy  \
							FROM spojyvannyaGV  \
							WHERE Year="+sqlStr(year)+" and Month="+sqlStr(month)+" and IsLichGV=false and IsGarVoda=true) AS spojGV  \
							LEFT JOIN abonent ON abonent.Rahunok=spojGV.Rahunok_ID  \
							GROUP BY abonent.Budynok_ID) AS GVNoLichSpoj ON budynky.id = GVNoLichSpoj.Budynok_ID) \
				ORDER BY budynky.Vulycia, budynky.Bud_num");
	int iter=0;
	double dVal;
	int iVal;
	int GVNoLichAbonCount=0, GVNoLichLudyCount=0, GVLichAbonCount=0, COOpalAbonCount=0, CONoOpalAbonCount=0, STAbonCount=0;
	double GVLichVykor=0, COVykorGKal=0, COOpalPlos=0, CONoLichOpalPlos=0, COLichOpalPlos=0, STVykorPlos=0, STVykorGKal=0;
	while (query.next() && (iter<rowCount)){
		tableCell.setAlignment(Qt::AlignHCenter);
		tableCell.set(3+iter,0,iter+1);
		tableCell.setAlignment(Qt::AlignLeft);
		tableCell.set(3+iter,1,query.value(1).toString()+" "+query.value(2).toString());
		tableCell.setAlignment(Qt::AlignRight);
			//ГВ без лічильника
		//Кількість абонентів
		iVal = query.value(19).toInt();
		GVNoLichAbonCount += iVal;
		tableCell.setDash(3+iter,2,iVal);
		//Кількість прописаних
		iVal = query.value(18).toInt();
		GVNoLichLudyCount += iVal;
		tableCell.setDash(3+iter,3,iVal);
			//ГВ з лічильником
		//Кількість абонентів
		iVal = query.value(16).toInt();
		GVLichAbonCount += iVal;
		tableCell.setDash(3+iter,4,iVal);
		//Викорристання води м.Куб
		dVal = query.value(17).toDouble() + query.value(14).toDouble();
		GVLichVykor += dVal;
		tableCell.setDash(3+iter,5,dVal);
			//ЦО
		//Використано ГКал
		dVal=query.value(4).toDouble();
		if (dVal !=0){
			dVal = (query.value(10).toDouble()-query.value(11).toDouble())/dVal;
			COVykorGKal += dVal;
			tableCell.set(3+iter,6,QString::number(dVal,'f', 4).replace('.',','));
		}
		else
			tableCell.set(3+iter,6,"Пом. коеф.");
		//Загальна опалювальна площа
		dVal = query.value(5).toDouble() + query.value(6).toDouble();
		COOpalPlos += dVal;
		tableCell.setDash(3+iter,7,dVal);
		//Опалювальна площа без лічильника
		if (query.value(3).toInt() != 1){
			dVal = query.value(5).toDouble() + query.value(6).toDouble();
			CONoLichOpalPlos += dVal;
		}
		else
			dVal = 0;
		tableCell.setDash(3+iter,8,dVal);
		//Опалювальна площа з лічильником
		if (query.value(3).toInt() == 1){
			dVal = query.value(5).toDouble() + query.value(6).toDouble();
			COLichOpalPlos += dVal;
		}
		else
			dVal = 0;
		tableCell.setDash(3+iter,9,dVal);
		//Кількість під'єднаних абонентів, тільки ЦО
		iVal = query.value(8).toInt();
		COOpalAbonCount += iVal;
		tableCell.setDash(3+iter,10,iVal);
		//Кількість від'єднаних абонентів, тільки ЦО
		iVal = query.value(9).toInt();
		CONoOpalAbonCount += iVal;
		tableCell.setDash(3+iter,11,iVal);
			// СТ
		//Кількість абонентів
		iVal = query.value(12).toInt();
		STAbonCount += iVal;
		tableCell.setDash(3+iter,12,iVal);
		//Площа м.кв.
		dVal = query.value(6).toDouble();
		STVykorPlos += dVal;
		tableCell.setDash(3+iter,13,dVal);
		//Викор тепла ГКал
		dVal = query.value(7).toDouble();
		STVykorGKal += dVal;
		tableCell.setDash(3+iter,14,dVal);
		
		iter++;
		postupForm->incPos();
	}
	
	table->mergeCells ( 3+iter, 0, 1, 2 );
	tableCell.set(3+iter,0,"ВСЬОГО");
		//ГВ без лічильника
	//Кількість абонентів
	tableCell.setDash(3+iter,2,GVNoLichAbonCount);
	//Кількість прописаних
	tableCell.setDash(3+iter,3,GVNoLichLudyCount);
		//ГВ з лічильником
	//Кількість абонентів
	tableCell.setDash(3+iter,4,GVLichAbonCount);
	//Викорристання води м.Куб
	tableCell.setDash(3+iter,5,GVLichVykor);
		//ЦО
	//Використано ГКал
	tableCell.set(3+iter,6,QString::number(COVykorGKal,'f', 4).replace('.',','));
	//Загальна опалювальна площа
	tableCell.setDash(3+iter,7,COOpalPlos);
	//Опалювальна площа без лічильника
	tableCell.setDash(3+iter,8,CONoLichOpalPlos);
	//Опалювальна площа з лічильником
	tableCell.setDash(3+iter,9,COLichOpalPlos);
	//Кількість під'єднаних абонентів, тільки ЦО
	tableCell.setDash(3+iter,10,COOpalAbonCount);
	//Кількість від'єднаних абонентів, тільки ЦО
	tableCell.setDash(3+iter,11,CONoOpalAbonCount);
		// СТ
	//Кількість абонентів
	tableCell.setDash(3+iter,12,STAbonCount);
	//Площа м.кв.
	tableCell.setDash(3+iter,13,STVykorPlos);
	//Викор тепла ГКал
	tableCell.setDash(3+iter,14,STVykorGKal);
	
	
	printform->printer()->setDocName("Відомість споживання послуг за "+monthlist.at(month-1)+" "+QString::number(year)+"р.");
	printform->printer()->setOrientation( QPrinter::Landscape );
	printform->show();
	
	delete postupForm;
}
//------------------------------------------------------------
void UPrintDocs::print_vidomistOtrymannyaPoslugPoKotelnyah(short year, short month)
{
	QSqlQuery query;
	query.exec("SELECT count(*) FROM budynky");
	query.seek(0);
	int rowCount = query.value(0).toInt();
	UPostupForm *postupForm = new UPostupForm(0, rowCount);
	postupForm->show();
	
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
	printform = new UPrintForm("Відомість отримання послуг по котельнях за "+monthlist.at(month-1)+" "+QString::number(year)+"р.",7,7,7,10);
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
	cursor.insertText("Відомість отримання послуг за "+monthlist.at(month-1)+" "+QString::number(year)+"р.", textCharFormat_bold);
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
	
	constraints << QTextLength(QTextLength::FixedLength, 30);
	constraints << QTextLength(QTextLength::FixedLength, 160);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(1);
	tableFormat.setBorder(0.5);
	tableFormat.setHeaderRowCount(3);
	QTextTable *table = cursor.insertTable(4, 15, tableFormat);
	
	table->mergeCells ( 0, 0, 3, 1 );
	table->mergeCells ( 0, 1, 3, 1 );
	table->mergeCells ( 0, 2, 1, 4 );
	table->mergeCells ( 1, 2, 1, 2 );
	table->mergeCells ( 1, 4, 1, 2 );
	
	table->mergeCells ( 0, 6, 1, 6 );
	table->mergeCells ( 1, 6, 2, 1 );
	table->mergeCells ( 1, 7, 2, 1 );
	table->mergeCells ( 1, 8, 2, 1 );
	table->mergeCells ( 1, 9, 2, 1 );
	table->mergeCells ( 1, 10, 2, 1 );
	table->mergeCells ( 1, 11, 2, 1 );
	
	table->mergeCells ( 0, 12, 1, 3 );
	table->mergeCells ( 1, 12, 2, 1 );
	table->mergeCells ( 1, 13, 2, 1 );
	table->mergeCells ( 1, 14, 2, 1 );
	
	UPopulateCell tableCell(table);
	tableCell.setAlignment(Qt::AlignHCenter);
	tableCell.setTextCharFormat(textCharFormat);
	tableCell.setTextBlockFormat(blockFormat);
	
	tableCell.set(0,0,"№ з/п");
	tableCell.set(0,1,"Котельня");
	tableCell.set(0,2,"Гаряча вода");
	tableCell.set(1,2,"Без лічильника");
	tableCell.set(2,2,"Кільк. абон.");
	tableCell.set(2,3,"Кільк. проп.");
	tableCell.set(1,4,"Лічильник");
	tableCell.set(2,4,"Кільк. абон.");
	tableCell.set(2,5,"Викор.води, м.куб.");
	tableCell.set(0,6,"Центральне опалення");
	tableCell.set(1,6,"Використ. ГКал ЦО+СТ");
	tableCell.set(1,7,"Заг.Опал. площа ЦО+СТ");
	tableCell.set(1,8,"Оп. Площа без ліч.");
	tableCell.set(1,9,"Оп. Площа з ліч.");
	tableCell.set(1,10,"К-сть під'єдн. абон. ЦО");
	tableCell.set(1,11,"К-сть від'єдн. абон. ЦО");
	tableCell.set(0,12,"Опал від стояків");
	tableCell.set(1,12,"К-сть абон.");
	tableCell.set(1,13,"Площа, м.кв.");
	tableCell.set(1,14,"Викор. тепла, ГКал");
	
	query.setForwardOnly(true);
	query.exec("SELECT kotelni.id, kotelni.Vulycia, kotelni.Nomer, budynky.Lichylnyk, budynky.Koef_pereved, opalPlosTable.PlosCO, opal_st_ploshcha_bud.ZagStPloshcha, opal_st_ploshcha_bud.ZagStGKal, \
						abonCOCountTable.abonCOCount, abonNoCOCountTable.abonNoCOCount, diuchTar.Lich_kin, diuchTar.Lich_poch, \
						opal_st_ploshcha_bud.abonSTCountReal, abonSTCountTable.abonSTCount, GVLichSpoj.ZagVykorVody, GVLichSpoj.ZagKilkLudei, \
						GVLichSpoj.abonCount, GVNoLichSpoj.ZagVykorVody, GVNoLichSpoj.ZagKilkLudei, GVNoLichSpoj.abonCount \
				FROM (((((((((budynky \
				LEFT JOIN (SELECT Budynok_ID, sum(Ploshcha) AS PlosCO FROM abonent WHERE opal=true GROUP BY Budynok_ID) AS opalPlosTable ON opalPlosTable.Budynok_ID=budynky.id) \
				LEFT JOIN (SELECT abonent.Budynok_ID, sum(pok.Plos) AS ZagStPloshcha, sum(GKal) AS ZagStGKal, count(*) AS abonSTCountReal FROM (SELECT Rahunok_ID, Plos, GKal  \
							FROM pokaznOpalStoyak  \
							WHERE Year="+sqlStr(year)+" and Month="+sqlStr(month)+") AS pok  \
							LEFT JOIN abonent ON abonent.Rahunok=pok.Rahunok_ID  \
							GROUP BY abonent.Budynok_ID) AS opal_st_ploshcha_bud ON budynky.id = opal_st_ploshcha_bud.Budynok_ID) \
				LEFT JOIN (SELECT Budynok_ID, count(*) AS abonCOCount FROM abonent WHERE opal=true GROUP BY Budynok_ID) AS abonCOCountTable ON abonCOCountTable.Budynok_ID=budynky.id) \
				LEFT JOIN (SELECT Budynok_ID, count(*) AS abonNoCOCount FROM abonent WHERE opal=false GROUP BY Budynok_ID) AS abonNoCOCountTable ON abonNoCOCountTable.Budynok_ID=budynky.id) \
				LEFT JOIN (SELECT Budynok_ID, Lich_kin, Lich_poch FROM diuch_taryf \
							WHERE year="+sqlStr(year)+" and month="+sqlStr(month)+") AS diuchTar ON diuchTar.Budynok_ID=budynky.id) \
				LEFT JOIN (SELECT Budynok_ID, count(*) AS abonSTCount FROM abonent WHERE NarahStoyak=true GROUP BY Budynok_ID) AS abonSTCountTable ON abonSTCountTable.Budynok_ID=budynky.id) \
				LEFT JOIN (SELECT abonent.Budynok_ID, sum(spojGV.VykorVody) AS ZagVykorVody, sum(spojGV.Ludy) AS ZagKilkLudei, count(*) AS abonCount FROM (SELECT Rahunok_ID, VykorVody, Ludy  \
							FROM spojyvannyaGV  \
							WHERE Year="+sqlStr(year)+" and Month="+sqlStr(month)+" and IsLichGV=true and IsGarVoda=true) AS spojGV  \
							LEFT JOIN abonent ON abonent.Rahunok=spojGV.Rahunok_ID  \
							GROUP BY abonent.Budynok_ID) AS GVLichSpoj ON budynky.id = GVLichSpoj.Budynok_ID) \
				LEFT JOIN (SELECT abonent.Budynok_ID, sum(spojGV.VykorVody) AS ZagVykorVody, sum(spojGV.Ludy) AS ZagKilkLudei, count(*) AS abonCount FROM (SELECT Rahunok_ID, VykorVody, Ludy  \
							FROM spojyvannyaGV  \
							WHERE Year="+sqlStr(year)+" and Month="+sqlStr(month)+" and IsLichGV=false and IsGarVoda=true) AS spojGV  \
							LEFT JOIN abonent ON abonent.Rahunok=spojGV.Rahunok_ID  \
							GROUP BY abonent.Budynok_ID) AS GVNoLichSpoj ON budynky.id = GVNoLichSpoj.Budynok_ID)) \
				LEFT JOIN kotelni ON kotelni.id = budynky.Kotelnya_ID \
				ORDER BY kotelni.Vulycia, kotelni.Nomer, kotelni.id");
	int iter=0;
	double dVal;
	int GVNoLichAbonCount=0, GVNoLichLudyCount=0, GVLichAbonCount=0, COOpalAbonCount=0, CONoOpalAbonCount=0, STAbonCount=0;
	double GVLichVykor=0, COVykorGKal=0, COOpalPlos=0, CONoLichOpalPlos=0, COLichOpalPlos=0, STVykorPlos=0, STVykorGKal=0;
	int SumGVNoLichAbonCount=0, SumGVNoLichLudyCount=0, SumGVLichAbonCount=0, SumCOOpalAbonCount=0, SumCONoOpalAbonCount=0, SumSTAbonCount=0;
	double SumGVLichVykor=0, SumCOVykorGKal=0, SumCOOpalPlos=0, SumCONoLichOpalPlos=0, SumCOLichOpalPlos=0, SumSTVykorPlos=0, SumSTVykorGKal=0;
	int prevKotelnyaId=0;
	bool firstRow=true;
	QString curKotelnya;
	int iterBud=0;
	while (query.next()){
		iterBud++;
		if (firstRow){
			prevKotelnyaId = query.value(0).toInt();
			firstRow = false;
			curKotelnya = query.value(1).toString()+" №"+query.value(2).toString();
		}
		
		if ((prevKotelnyaId != query.value(0).toInt()) || (iterBud==rowCount)){
			prevKotelnyaId = query.value(0).toInt();
			
			if (iterBud==rowCount){ //Додавання останньої стрічки до  до показників при  зміні необхідно
				curKotelnya = query.value(1).toString()+" №"+query.value(2).toString();
					//ГВ без лічильника
				//Кількість абонентів
				GVNoLichAbonCount += query.value(19).toInt();
				//Кількість прописаних
				GVNoLichLudyCount += query.value(18).toInt();
					//ГВ з лічильником
				//Кількість абонентів
				GVLichAbonCount += query.value(16).toInt();
				//Викорристання води м.Куб
				GVLichVykor += query.value(17).toDouble() + query.value(14).toDouble();
					//ЦО
				//Використано ГКал
				dVal=query.value(4).toDouble();
				if (dVal !=0){
					dVal = (query.value(10).toDouble()-query.value(11).toDouble())/dVal;
					COVykorGKal += dVal;
				}
				//Загальна опалювальна площа
				COOpalPlos += query.value(5).toDouble() + query.value(6).toDouble();
				//Опалювальна площа без лічильника
				if (query.value(3).toInt() != 1){
					dVal = query.value(5).toDouble() + query.value(6).toDouble();
					CONoLichOpalPlos += dVal;
				}
				//Опалювальна площа з лічильником
				if (query.value(3).toInt() == 1){
					dVal = query.value(5).toDouble() + query.value(6).toDouble();
					COLichOpalPlos += dVal;
				}
				//Кількість під'єднаних абонентів, тільки ЦО
				COOpalAbonCount += query.value(8).toInt();
				//Кількість від'єднаних абонентів, тільки ЦО
				CONoOpalAbonCount += query.value(9).toInt();
					// СТ
				//Кількість абонентів
				STAbonCount += query.value(12).toInt();
				//Площа м.кв.
				STVykorPlos += query.value(6).toDouble();
				//Викор тепла ГКал
				STVykorGKal += query.value(7).toDouble();
			}
			
			//Додавання стрічки в таблиці
			table->insertRows(iter+3,1);
			
			tableCell.setAlignment(Qt::AlignHCenter);
			tableCell.set(3+iter,0,iter+1);
			tableCell.setAlignment(Qt::AlignLeft);
			tableCell.set(3+iter,1,curKotelnya);
			tableCell.setAlignment(Qt::AlignRight);
			
					//ГВ без лічильника
			//Кількість абонентів
			tableCell.setDash(3+iter,2,GVNoLichAbonCount);
			//Кількість прописаних
			tableCell.setDash(3+iter,3,GVNoLichLudyCount);
				//ГВ з лічильником
			//Кількість абонентів
			tableCell.setDash(3+iter,4,GVLichAbonCount);
			//Викорристання води м.Куб
			tableCell.setDash(3+iter,5,GVLichVykor);
				//ЦО
			//Використано ГКал
			tableCell.set(3+iter,6,QString::number(COVykorGKal,'f', 4).replace('.',','));
			//Загальна опалювальна площа
			tableCell.setDash(3+iter,7,COOpalPlos);
			//Опалювальна площа без лічильника
			tableCell.setDash(3+iter,8,CONoLichOpalPlos);
			//Опалювальна площа з лічильником
			tableCell.setDash(3+iter,9,COLichOpalPlos);
			//Кількість під'єднаних абонентів, тільки ЦО
			tableCell.setDash(3+iter,10,COOpalAbonCount);
			//Кількість від'єднаних абонентів, тільки ЦО
			tableCell.setDash(3+iter,11,CONoOpalAbonCount);
				// СТ
			//Кількість абонентів
			tableCell.setDash(3+iter,12,STAbonCount);
			//Площа м.кв.
			tableCell.setDash(3+iter,13,STVykorPlos);
			//Викор тепла ГКал
			tableCell.setDash(3+iter,14,STVykorGKal);
			
			SumGVNoLichAbonCount += GVNoLichAbonCount;
			SumGVNoLichLudyCount += GVNoLichLudyCount;
			SumGVLichAbonCount += GVLichAbonCount;
			SumCOOpalAbonCount += COOpalAbonCount;
			SumCONoOpalAbonCount += CONoOpalAbonCount;
			SumSTAbonCount += STAbonCount;
			SumGVLichVykor += GVLichVykor;
			SumCOVykorGKal += COVykorGKal;
			SumCOOpalPlos += COOpalPlos;
			SumCONoLichOpalPlos += CONoLichOpalPlos;
			SumCOLichOpalPlos += COLichOpalPlos;
			SumSTVykorPlos += STVykorPlos;
			SumSTVykorGKal += STVykorGKal;
			
			GVNoLichAbonCount = 0;
			GVNoLichLudyCount = 0;
			GVLichAbonCount = 0;
			COOpalAbonCount = 0;
			CONoOpalAbonCount = 0;
			STAbonCount = 0;
			GVLichVykor = 0;
			COVykorGKal = 0;
			COOpalPlos = 0;
			CONoLichOpalPlos = 0;
			COLichOpalPlos = 0;
			STVykorPlos = 0;
			STVykorGKal = 0;
			
			curKotelnya = query.value(1).toString()+" №"+query.value(2).toString();
			
			iter++;
		}
		
			//ГВ без лічильника
		//Кількість абонентів
		GVNoLichAbonCount += query.value(19).toInt();
		//Кількість прописаних
		GVNoLichLudyCount += query.value(18).toInt();
			//ГВ з лічильником
		//Кількість абонентів
		GVLichAbonCount += query.value(16).toInt();
		//Викорристання води м.Куб
		GVLichVykor += query.value(17).toDouble() + query.value(14).toDouble();
			//ЦО
		//Використано ГКал
		dVal=query.value(4).toDouble();
		if (dVal !=0){
			dVal = (query.value(10).toDouble()-query.value(11).toDouble())/dVal;
			COVykorGKal += dVal;
		}
		//Загальна опалювальна площа
		COOpalPlos += query.value(5).toDouble() + query.value(6).toDouble();
		//Опалювальна площа без лічильника
		if (query.value(3).toInt() != 1){
			dVal = query.value(5).toDouble() + query.value(6).toDouble();
			CONoLichOpalPlos += dVal;
		}
		//Опалювальна площа з лічильником
		if (query.value(3).toInt() == 1){
			dVal = query.value(5).toDouble() + query.value(6).toDouble();
			COLichOpalPlos += dVal;
		}
		//Кількість під'єднаних абонентів, тільки ЦО
		COOpalAbonCount += query.value(8).toInt();
		//Кількість від'єднаних абонентів, тільки ЦО
		CONoOpalAbonCount += query.value(9).toInt();
			// СТ
		//Кількість абонентів
		STAbonCount += query.value(12).toInt();
		//Площа м.кв.
		STVykorPlos += query.value(6).toDouble();
		//Викор тепла ГКал
		STVykorGKal += query.value(7).toDouble();
		
		
		postupForm->incPos();
	}
	
	table->mergeCells ( 3+iter, 0, 1, 2 );
	tableCell.set(3+iter,0,"ВСЬОГО");
		//ГВ без лічильника
	//Кількість абонентів
	tableCell.setDash(3+iter,2,SumGVNoLichAbonCount);
	//Кількість прописаних
	tableCell.setDash(3+iter,3,SumGVNoLichLudyCount);
		//ГВ з лічильником
	//Кількість абонентів
	tableCell.setDash(3+iter,4,SumGVLichAbonCount);
	//Викорристання води м.Куб
	tableCell.setDash(3+iter,5,SumGVLichVykor);
		//ЦО
	//Використано ГКал
	tableCell.set(3+iter,6,QString::number(SumCOVykorGKal,'f', 4).replace('.',','));
	//Загальна опалювальна площа
	tableCell.setDash(3+iter,7,SumCOOpalPlos);
	//Опалювальна площа без лічильника
	tableCell.setDash(3+iter,8,SumCONoLichOpalPlos);
	//Опалювальна площа з лічильником
	tableCell.setDash(3+iter,9,SumCOLichOpalPlos);
	//Кількість під'єднаних абонентів, тільки ЦО
	tableCell.setDash(3+iter,10,SumCOOpalAbonCount);
	//Кількість від'єднаних абонентів, тільки ЦО
	tableCell.setDash(3+iter,11,SumCONoOpalAbonCount);
		// СТ
	//Кількість абонентів
	tableCell.setDash(3+iter,12,SumSTAbonCount);
	//Площа м.кв.
	tableCell.setDash(3+iter,13,SumSTVykorPlos);
	//Викор тепла ГКал
	tableCell.setDash(3+iter,14,SumSTVykorGKal);
	
	
	printform->printer()->setDocName("Відомість отримання послуг по котельнях за "+monthlist.at(month-1)+" "+QString::number(year)+"р.");
	printform->printer()->setOrientation( QPrinter::Landscape );
	printform->show();
	
	delete postupForm;
}
//------------------------------------------------------------
void UPrintDocs::print_vidomistSamovilnoVidkluchenyh(short year, short month)
{
	QSqlQuery query, narahQuery;
	query.exec("SELECT count(*) FROM abonent WHERE SamovVidednCO=true");
	query.seek(0);
	int rowCount = query.value(0).toInt();
	UPostupForm *postupForm = new UPostupForm(0, rowCount);
	postupForm->show();
	
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
	printform = new UPrintForm("Відомість самовільно відключених за "+monthlist.at(month-1)+" "+QString::number(year)+"р.",7,7,7,10);
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
	cursor.insertText("Відомість самовільно відключених за "+monthlist.at(month-1)+" "+QString::number(year)+"р.", textCharFormat_bold);
	textCharFormat_bold.setFontPointSize( 10 );
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	cursor.insertText("за " + monthlist.at(month-1) + " " + QString::number(year)+"р.", textCharFormat_bold);
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 7 );
	cursor.insertText(" ",textCharFormat);
	textCharFormat.setFontPointSize( 9 );
	textCharFormat_italic.setFontPointSize( 9 );
	textCharFormat_bold.setFontPointSize( 9 );
	cursor.insertBlock(blockFormat);
	
	//Створення кістяка таблиці
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
	
	constraints << QTextLength(QTextLength::FixedLength, 30);
	constraints << QTextLength(QTextLength::FixedLength, 55);
	constraints << QTextLength(QTextLength::FixedLength, 150);
	constraints << QTextLength(QTextLength::FixedLength, 120);
	constraints << QTextLength(QTextLength::FixedLength, 63);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 120);
	constraints << QTextLength(QTextLength::FixedLength, 54);
	constraints << QTextLength(QTextLength::FixedLength, 54);
	constraints << QTextLength(QTextLength::FixedLength, 54);
	constraints << QTextLength(QTextLength::FixedLength, 54);
	constraints << QTextLength(QTextLength::FixedLength, 54);
	constraints << QTextLength(QTextLength::FixedLength, 54);
	constraints << QTextLength(QTextLength::FixedLength, 54);
	constraints << QTextLength(QTextLength::FixedLength, 54);
	
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(1);
	tableFormat.setBorder(0.5);
	tableFormat.setHeaderRowCount(1);
	QTextTable *table = cursor.insertTable(rowCount+2, 15, tableFormat);
	
	UPopulateCell tableCell(table);
	tableCell.setAlignment(Qt::AlignHCenter);
	tableCell.setTextCharFormat(textCharFormat);
	tableCell.setTextBlockFormat(blockFormat);
	
	tableCell.set(0,0,"№ з/п");
	tableCell.set(0,1,"Рахунок");
	tableCell.set(0,2,"П І Б");
	tableCell.set(0,3,"Адреса");
	tableCell.set(0,4,"Площа");
	tableCell.set(0,5,"К-сть пропис.");
	tableCell.set(0,6,"Пільги");
	tableCell.set(0,7,"Вхідне\nсальдо");
	tableCell.set(0,8,"Нарах. ЦО");
	tableCell.set(0,9,"Нарах. ГВ");
	tableCell.set(0,10,"Нарах. СТ");
	tableCell.set(0,11,"Субсидія");
	tableCell.set(0,12,"Оплата");
	tableCell.set(0,13,"Перев. заборг.");
	tableCell.set(0,14,"Вихідне\nсальдо");
	
	int iter;
	double sumVhSaldo=0, sumNarahCO=0, sumNarahGV=0, sumNarahST=0, sumSubs=0, sumOpl=0, sumPlos=0, sumPereved=0, sumVyhSaldo=0;
	int countLudy=0;
	double dVal, curVyhSaldo;
	
	query.exec("SELECT abonent.Rahunok, abonent.Prizv, abonent.Imya, abonent.Batk, budynky.Vulycia, \
						budynky.Bud_num, abonent.Kvartyra, abonent.Ploshcha, abonent.Ludy, abonent.Pilga_ID, \
						pilgy1.Kategoriya, abonent.Pilga_for, abonent.Pilga_ID_2, pilgy2.Kategoriya, \
						abonent.Pilga_for_2 \
				FROM (((abonent \
				LEFT JOIN budynky ON budynky.id = abonent.Budynok_ID) \
				LEFT JOIN pilgy AS pilgy1 ON pilgy1.id=abonent.Pilga_ID) \
				LEFT JOIN pilgy AS pilgy2 ON pilgy2.id=abonent.Pilga_ID_2) \
				WHERE abonent.SamovVidednCO=true \
				ORDER BY abonent.Prizv, abonent.Imya, abonent.Batk");
	int row=1;
	for (iter=0; (iter<rowCount) && query.next(); iter++){
		row = iter+1;
		tableCell.setAlignment(Qt::AlignHCenter);
		//"№ з/п"
		tableCell.set(row,0,row);
		//"Рахунок"
		tableCell.set(row,1,query.value(0).toString());
		//"П І Б"
		tableCell.setAlignment(Qt::AlignLeft);
		tableCell.set(row,2,query.value(1).toString()+" "+query.value(2).toString()+" "+query.value(3).toString());
		//"Адреса"
		tableCell.set(row,3,query.value(4).toString()+query.value(5).toString()+(query.value(6).toString().size()>0?" кв."+query.value(6).toString():""));
		//"Площа"
		dVal = query.value(7).toDouble();
		sumPlos += dVal;
		tableCell.setAlignment(Qt::AlignRight);
		tableCell.set(row,4,QString::number(dVal).replace('.',','));
		//"К-сть пропис."
		countLudy += query.value(8).toInt();
		tableCell.set(row,5,query.value(8).toString());
		//"Пільги"
		tableCell.setAlignment(Qt::AlignLeft);
		tableCell.set(row,6,( (query.value(9).toInt()>0 || query.value(12).toInt()>0) ? 
			( (query.value(9).toInt()>0) ? ("1."+(query.value(10).toString()+" "+query.value(11).toString()+"чол.")) : (("немає.") + 
			(query.value(12).toInt()>0 ? ("2."+query.value(13).toString()+" "+query.value(14).toString()+"чол.") : "" ) )) : ("") ));
		//"Вх.сальдо"
		narahQuery.exec("SELECT Vh_saldo FROM narahuvannya \
						WHERE Rahunok="+query.value(0).toString()+" \
							and year="+sqlStr(year)+" \
							and month="+sqlStr(month));
		narahQuery.seek(0);
		dVal = narahQuery.value(0).toDouble();
		curVyhSaldo = dVal;
		sumVhSaldo += dVal;
		tableCell.setAlignment(Qt::AlignRight);
		tableCell.set(row,7,dVal);
		//"Нарах.ЦО"
		narahQuery.exec("SELECT sum(Narah), sum(Narah_GV), sum(Narah_St) FROM narahuvannya \
						WHERE Rahunok="+query.value(0).toString()+" \
							and year="+sqlStr(year)+" \
							and month="+sqlStr(month));
		narahQuery.seek(0);
		dVal = narahQuery.value(0).toDouble();
		curVyhSaldo += dVal;
		sumNarahCO += dVal;
		tableCell.set(row,8,dVal);
		//"Нарах.ГВ"
		dVal = narahQuery.value(1).toDouble();
		curVyhSaldo += dVal;
		sumNarahGV += dVal;
		tableCell.set(row,9,dVal);
		//"Нарах.СТ"
		dVal = narahQuery.value(2).toDouble();
		curVyhSaldo += dVal;
		sumNarahST += dVal;
		tableCell.set(row,10,dVal);
		//"Субсидія"
		narahQuery.exec("SELECT sum(Suma), sum(Suma_d), sum(Suma_GV), sum(Suma_d_GV) FROM subs  \
						WHERE Rahunok_id="+query.value(0).toString()+" \
							and year="+sqlStr(year)+" \
							and month="+sqlStr(month));
		narahQuery.seek(0);
		dVal = narahQuery.value(0).toDouble()+narahQuery.value(1).toDouble()+narahQuery.value(2).toDouble()+narahQuery.value(3).toDouble();
		curVyhSaldo -= dVal;
		sumSubs += dVal;
		tableCell.set(row,11,dVal);
		//"Оплата"
		narahQuery.exec("SELECT sum(Suma) FROM kvytancii  \
						WHERE Rahunok_id="+query.value(0).toString()+" \
							and year="+sqlStr(year)+" \
							and month="+sqlStr(month));
		narahQuery.seek(0);
		dVal = narahQuery.value(0).toDouble();
		curVyhSaldo -= dVal;
		sumOpl += dVal;
		tableCell.set(row,12,dVal);
		//Переведення заборгованості
		narahQuery.exec("SELECT sum(Suma) FROM narahuvannyaPereved \
						WHERE Rahunok_id="+query.value(0).toString()+" \
							and year(CDate)="+sqlStr(year)+" \
							and month(CDate)="+sqlStr(month));
		narahQuery.next();
		dVal = uDToM(narahQuery.value(0).toDouble());
		curVyhSaldo += dVal;
		sumPereved += dVal;
		tableCell.set(row,13,dVal);
		//"Вих.сальдо"
		sumVyhSaldo += curVyhSaldo;
		tableCell.set(row,14,curVyhSaldo);
		
		postupForm->incPos();
	}
	
	row++;
	//"Всього
	table->mergeCells ( row, 0, 1, 4 );
	tableCell.setAlignment(Qt::AlignRight);
	tableCell.set(row,0,"Всього");
	//"Площа"
	tableCell.set(row,4,QString::number(sumPlos).replace('.',','));
	//"К-сть пропис."
	tableCell.set(row,5,countLudy);
	//Пільги
	tableCell.set(row,6,"-");
	//"Вх.сальдо"
	tableCell.set(row,7,sumVhSaldo);
	//"Нарах.ЦО"
	tableCell.set(row,8,sumNarahCO);
	//"Нарах.ГВ"
	tableCell.set(row,9,sumNarahGV);
	//"Нарах.СТ"
	tableCell.set(row,10,sumNarahST);
	//"Субсидія"
	tableCell.set(row,11,sumSubs);
	//"Оплата"
	tableCell.set(row,12,sumOpl);
	//Переведення заборгованості
	tableCell.set(row,13,sumPereved);
	//"Вих.сальдо"
	tableCell.set(row,14,sumVyhSaldo);
	
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("     "+DocsPosadaPidpysText(3)+"					"+PrizvIB(3));
	
	printform->printer()->setDocName("Відомість отримання послуг по котельнях за "+monthlist.at(month-1)+" "+QString::number(year)+"р.");
	printform->printer()->setOrientation( QPrinter::Landscape );
	printform->show();
	
	delete postupForm;
}
//------------------------------------------------------------
void UPrintDocs::print_vidomistDebetoveKredytoveSaldo(short year, short month)
{
	QSqlQuery query;
	query.exec("SELECT count(*) FROM abonent");
	query.seek(0);
	int rowCount = query.value(0).toInt();
	UPostupForm *postupForm = new UPostupForm(0, rowCount);
	postupForm->show();
	
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
	printform = new UPrintForm("Відомість вихідного дебетового-кредитового сальдо за "+monthlist.at(month-1)+" "+QString::number(year)+"р.",7,7,7,10);
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
	cursor.insertText("Відомість вихідного дебетового-кредитового сальдо за "+monthlist.at(month-1)+" "+QString::number(year)+"р.", textCharFormat_bold);
	textCharFormat_bold.setFontPointSize( 10 );
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	cursor.insertText("за " + monthlist.at(month-1) + " " + QString::number(year)+"р.", textCharFormat_bold);
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 7 );
	cursor.insertText(" ",textCharFormat);
	textCharFormat.setFontPointSize( 12 );
	textCharFormat_italic.setFontPointSize( 12 );
	textCharFormat_bold.setFontPointSize( 12 );
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	
	query.exec("SELECT Rahunok FROM abonent");
	double debetSaldo=0, kredytSaldo=0, dVal;
	QDate vidomDate(year, month, 1);
	vidomDate = vidomDate.addMonths(1);
	while (query.next()){
		dVal = vhSaldoInMonth(query.value(0).toInt(), vidomDate.year(), vidomDate.month());
		if (dVal > 0)
			debetSaldo += dVal;
		else
			kredytSaldo += dVal;
		postupForm->incPos();
	}
	
	cursor.insertText("Дебетове сальдо: " + uMToStr2(debetSaldo)+" грн.", textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("Кредитове сальдо: " + uMToStr2(kredytSaldo)+" грн.", textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("Загальне сальдо: " + uMToStr2(debetSaldo+kredytSaldo)+" грн.", textCharFormat);
	
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("     "+DocsPosadaPidpysText(3)+"					"+PrizvIB(3));
	
	printform->printer()->setDocName("Відомість вихідного дебетового-кредитового сальдо за "+monthlist.at(month-1)+" "+QString::number(year)+"р.");
	//printform->printer()->setOrientation( QPrinter::Landscape );
	printform->show();
	
	delete postupForm;
}
//------------------------------------------------------------
void UPrintDocs::print_vidomistNadannyaPoslugPoBudTaKot(int kId, int bId)
{
	QSqlQuery query, narahQuery;
	if (kId==0 && bId>0){
		query.exec("SELECT Kotelnya_id FROM budynky WHERE id="+sqlStr(bId));
		query.next();
		kId = query.value(0).toInt();
	}
	
	query.exec("SELECT count(*) FROM budynky"+(kId>0?QString(" WHERE Kotelnya_ID=")+QString::number(kId):QString()));
	query.seek(0);
	int rowCount = query.value(0).toInt();
	UPostupForm *postupForm = new UPostupForm(0, rowCount);
	postupForm->show();
	qDebug() << query.lastQuery();
	qDebug() << "rowCount: " << rowCount;
	
	//Змінні таблиці
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
	QTextTable *table=NULL;
	UPopulateCell tableCell(table);
	
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
	printform = new UPrintForm("Відомість надання послуг по будинках та котельнях",10,7,7,10);
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 7 );
    cursor.insertText("Сформовано ПК \"ТКЕнерго менеджер\" (супровід Компанія \"АбонПро\". веб: www.abonpro.com). Час формування: "+QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy")+"р." , textCharFormat);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertText(" ",textCharFormat);
	textCharFormat.setFontPointSize( 10 );
	textCharFormat_bold.setFontPointSize( 10 );
	blockFormat.setAlignment( Qt::AlignCenter );
	cursor.insertBlock(blockFormat);
	cursor.insertText("ВІДОМІСТЬ\nнадання послуг по будинках та котельнях", textCharFormat_bold);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	
	bool firstKotelnya=true;
	QSqlQuery kQuery, bQuery, aQuery;
	kQuery.exec("SELECT id, Nas_punkt, Vulycia, Nomer, Kot_num FROM kotelni"+(kId>0?QString(" WHERE id=")+QString::number(kId):QString())+QString(" ORDER BY Vulycia, Nomer"));
	while (kQuery.next()){
		cursor.movePosition(QTextCursor::End);
		blockFormat.setAlignment( Qt::AlignCenter );
		if (firstKotelnya){
			cursor.insertBlock(blockFormat);
			firstKotelnya = false;
		}
		else{
			blockFormat.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysBefore);
			cursor.insertBlock(blockFormat);
			blockFormat.setPageBreakPolicy(QTextFormat::PageBreak_Auto);
		}
		cursor.insertText("Котельня: "+kQuery.value(2).toString()+" "+kQuery.value(3).toString()+" №"+kQuery.value(4).toString()+", "+kQuery.value(1).toString(),textCharFormat_bold);
		
		constraints.clear();
		constraints << QTextLength(QTextLength::FixedLength, 40);
		constraints << QTextLength(QTextLength::FixedLength, 60);
		constraints << QTextLength(QTextLength::FixedLength, 300);
		constraints << QTextLength(QTextLength::FixedLength, 50);
		constraints << QTextLength(QTextLength::FixedLength, 70);
		constraints << QTextLength(QTextLength::FixedLength, 70);
		constraints << QTextLength(QTextLength::FixedLength, 70);
	
		tableFormat.setColumnWidthConstraints(constraints);
		tableFormat.setCellSpacing(0);
		tableFormat.setCellPadding(1);
		tableFormat.setBorder(0.5);
		tableFormat.setHeaderRowCount(1);
		bQuery.exec("SELECT count(*) FROM budynky \
					WHERE Kotelnya_ID="+kQuery.value(0).toString()+(bId>0?QString(" and id=")+QString::number(bId):QString()));
		bQuery.next();
		table = cursor.insertTable(4+bQuery.value(0).toInt(), 7, tableFormat);
		
		tableCell.setTextTable(table);
	
		blockFormat.setBottomMargin(0);
		blockFormat.setTopMargin(0);
		
		//Заповнення шапки таблиці
		textCharFormat.setFontPointSize( 10 );
		textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
		textCharFormat_bold.setFontPointSize( 10 );
		textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignMiddle);
		tableCell.setAlignment(Qt::AlignCenter);
		tableCell.set(0,0,"№ з/п", textCharFormat);
		tableCell.set(0,1,"Рахунок", textCharFormat);
		tableCell.set(0,2,"П І Б", textCharFormat);
		tableCell.set(0,3,"Квар-тира", textCharFormat);
		tableCell.set(0,4,"ЦО", textCharFormat);
		tableCell.set(0,5,"ГВ", textCharFormat);
		tableCell.set(0,6,"СТ", textCharFormat);
		
		textCharFormat.setFontPointSize( 10 );
		textCharFormat.setVerticalAlignment(QTextCharFormat::AlignTop);
		textCharFormat_bold.setFontPointSize( 10 );
		textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignTop);
		
		bQuery.exec("SELECT id, Vulycia, Bud_num FROM budynky \
					WHERE Kotelnya_ID="+kQuery.value(0).toString()+(bId>0?QString(" and id=")+QString::number(bId):QString()));
		int row=0;
		int COOnCount=0, COOutCount=0, GVOnCount=0, GVOutCount=0, STOnCount=0, STOutCount=0;
		int AllCOOnCount=0, AllCOOutCount=0, AllGVOnCount=0, AllGVOutCount=0, AllSTOnCount=0, AllSTOutCount=0;
		while (bQuery.next()){
			row++;
			table->mergeCells( row, 0, 1, 7 );
			tableCell.setAlignment(Qt::AlignLeft);
			tableCell.set(row,0,"   "+bQuery.value(1).toString()+" "+bQuery.value(2).toString(), textCharFormat_bold);
			
			aQuery.exec("SELECT count(*) FROM abonent WHERE Budynok_ID="+bQuery.value(0).toString());
			aQuery.next();
			int aRowCount = aQuery.value(0).toInt();
			table->insertRows(row+1, aRowCount+3);
			
			aQuery.exec("SELECT Rahunok, Prizv, Imya, Batk, Kvartyra, Opal, GarVoda, NarahStoyak FROM abonent \
						WHERE Budynok_ID="+bQuery.value(0).toString()+" \
						ORDER BY Kvartyra");
			int iter=0;
			while (aQuery.next()){
				row++;
				iter++;
				tableCell.setAlignment(Qt::AlignCenter);
				tableCell.set(row,0,iter, textCharFormat);
				tableCell.setAlignment(Qt::AlignRight);
				tableCell.set(row,1,aQuery.value(0).toString(), textCharFormat);
				tableCell.setAlignment(Qt::AlignLeft);
				tableCell.set(row,2,aQuery.value(1).toString()+" "+aQuery.value(2).toString()+" "+aQuery.value(3).toString(), textCharFormat);
				tableCell.setAlignment(Qt::AlignCenter);
				tableCell.set(row,3,aQuery.value(4).toString(), textCharFormat);
				
				if (aQuery.value(5).toBool()){
					tableCell.set(row,4,"+", textCharFormat);
					++COOnCount;
				}
				else{
					tableCell.set(row,4,"-", textCharFormat);
					++COOutCount;
				}
				if (aQuery.value(6).toBool()){
					tableCell.set(row,5,"+", textCharFormat);
					++GVOnCount;
				}
				else{
					tableCell.set(row,5,"-", textCharFormat);
					++GVOutCount;
				}
				if (aQuery.value(7).toBool()){
					tableCell.set(row,6,"+", textCharFormat);
					++STOnCount;
				}
				else{
					tableCell.set(row,6,"-", textCharFormat);
					++STOutCount;
				}
			}
			
			row++;
			table->mergeCells( row, 0, 1, 4 );
			tableCell.setAlignment(Qt::AlignRight);
			tableCell.set(row,0,"Всього:", textCharFormat);
			tableCell.setAlignment(Qt::AlignCenter);
			tableCell.set(row,4,COOnCount+COOutCount, textCharFormat);
			tableCell.set(row,5,GVOnCount+GVOutCount, textCharFormat);
			tableCell.set(row,6,STOnCount+STOutCount, textCharFormat);
			
			row++;
			table->mergeCells( row, 0, 1, 4 );
			tableCell.setAlignment(Qt::AlignRight);
			tableCell.set(row,0,"Всього абонентів, яким надаються послуги:", textCharFormat);
			tableCell.setAlignment(Qt::AlignCenter);
			tableCell.set(row,4,COOnCount, textCharFormat);
			tableCell.set(row,5,GVOnCount, textCharFormat);
			tableCell.set(row,6,STOnCount, textCharFormat);
			
			row++;
			table->mergeCells( row, 0, 1, 4 );
			tableCell.setAlignment(Qt::AlignRight);
			tableCell.set(row,0,"Всього абонентів, яким не надаються послуги:", textCharFormat);
			tableCell.setAlignment(Qt::AlignCenter);
			tableCell.set(row,4,COOutCount, textCharFormat);
			tableCell.set(row,5,GVOutCount, textCharFormat);
			tableCell.set(row,6,STOutCount, textCharFormat);
			
			AllCOOnCount += COOnCount;
			AllCOOutCount += COOutCount;
			AllGVOnCount += GVOnCount;
			AllGVOutCount += GVOutCount;
			AllSTOnCount += STOnCount;
			AllSTOutCount += STOutCount;
			
			COOnCount = 0;
			COOutCount = 0;
			GVOnCount = 0;
			GVOutCount = 0;
			STOnCount = 0;
			STOutCount = 0;
			
			postupForm->incPos();
		}
		
		row++;
		table->mergeCells( row, 0, 1, 4 );
		tableCell.setAlignment(Qt::AlignRight);
		tableCell.set(row,0,"Всього:", textCharFormat_bold);
		tableCell.setAlignment(Qt::AlignCenter);
		tableCell.set(row,4,AllCOOnCount+AllCOOutCount, textCharFormat_bold);
		tableCell.set(row,5,AllGVOnCount+AllGVOutCount, textCharFormat_bold);
		tableCell.set(row,6,AllSTOnCount+AllSTOutCount, textCharFormat_bold);
		
		row++;
		table->mergeCells( row, 0, 1, 4 );
		tableCell.setAlignment(Qt::AlignRight);
		tableCell.set(row,0,"Всього абонентів, яким надаються послуги:", textCharFormat_bold);
		tableCell.setAlignment(Qt::AlignCenter);
		tableCell.set(row,4,AllCOOnCount, textCharFormat_bold);
		tableCell.set(row,5,AllGVOnCount, textCharFormat_bold);
		tableCell.set(row,6,AllSTOnCount, textCharFormat_bold);
		
		row++;
		table->mergeCells( row, 0, 1, 4 );
		tableCell.setAlignment(Qt::AlignRight);
		tableCell.set(row,0,"Всього абонентів, яким не надаються послуги:", textCharFormat_bold);
		tableCell.setAlignment(Qt::AlignCenter);
		tableCell.set(row,4,AllCOOutCount, textCharFormat_bold);
		tableCell.set(row,5,AllGVOutCount, textCharFormat_bold);
		tableCell.set(row,6,AllSTOutCount, textCharFormat_bold);
	}
	
	printform->printer()->setDocName("Відомість надання послуг по будинках та котельнях");
	//printform->printer()->setOrientation( QPrinter::Landscape );
	printform->show();
	
	delete postupForm;
}
//------------------------------------------------------------
void UPrintDocs::print_vidomistSpojyvannyaCOZaRik(short year, short month, int budId, int rah)
{
	//Створення документа тарифів для всіх будинків за X місяць, Y рік
	printform = new UPrintForm("Відомість споживання ЦО за рік",10,7,7,10);
	QTextCursor cursor(printform->document()), cellCursor;
	
	QSqlQuery bQuery, aQuery, query;
	
	//Перевстановлення ідентифікатора будинку, якщо рахунок >0
	if ( rah>0 ) {
		query.exec("SELECT Budynok_id FROM abonent WHERE Rahunok="+sqlStr(rah));
		query.next();
		budId = query.value(0).toInt();
	}
	
	UPostupForm *postupForm = new UPostupForm(0, 0);
	postupForm->showLable(true);
	postupForm->show();
	
	QDate endDate(year, month, 1);
	QDate begDate = endDate.addMonths(-11);
	
	//Ініціалізація тарифів у році
	double taryf[12];
	int iter=0;
	for (QDate xDate=begDate; xDate<=endDate; xDate=xDate.addMonths(1)) {
		taryf[iter] = nasTaryf(xDate.year(), xDate.month()).vart_g_kal;
		iter++;
	}
	
	//Змінні таблиці
	QTextTableFormat tableFormat;
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(1);
	tableFormat.setBorder(0.5);
	tableFormat.setHeaderRowCount(4);
	QVector<QTextLength> constraints;
	constraints.clear();
	constraints << QTextLength(QTextLength::FixedLength, 25);
	constraints << QTextLength(QTextLength::FixedLength, 30);
	constraints << QTextLength(QTextLength::FixedLength, 100);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	tableFormat.setColumnWidthConstraints(constraints);
	QTextTable *table=NULL;
	UPopulateCell tableCell(table);
	
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
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 7 );
    cursor.insertText("Сформовано ПК \"ТКЕнерго менеджер\" (супровід Компанія \"АбонПро\". веб: www.abonpro.com). Час формування: "+QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy")+"р." , textCharFormat);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertText(" ",textCharFormat);
	textCharFormat.setFontPointSize( 11 );
	textCharFormat_bold.setFontPointSize( 11 );
	blockFormat.setAlignment( Qt::AlignCenter );
	cursor.insertBlock(blockFormat);
	cursor.insertText("ВІДОМІСТЬ\nспоживання ЦО за рік", textCharFormat_bold);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	cursor.insertText("Аналізований період: "+begDate.toString("MM.yyyy")+"р. - "+endDate.toString("MM.yyyy")+"р.", textCharFormat);
	//cursor.insertBlock(blockFormat);
	
	bool firstBud=true;
	bQuery.exec("SELECT id, Vulycia, Bud_num, Lichylnyk, Koef_pereved FROM budynky "
				"WHERE Lichylnyk=true"+(budId>0?QString(" and id=")+sqlStr(budId):QString(""))+
				QString(" ORDER BY Vulycia, Bud_num"));
	while (bQuery.next()) {
		cursor.movePosition(QTextCursor::End);
		blockFormat.setAlignment( Qt::AlignLeft );
		if (firstBud){
			cursor.insertBlock(blockFormat);
			firstBud = false;
		}
		else{
			blockFormat.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysBefore);
			cursor.insertBlock(blockFormat);
			blockFormat.setPageBreakPolicy(QTextFormat::PageBreak_Auto);
		}
		textCharFormat.setFontPointSize( 9 );
		textCharFormat_bold.setFontPointSize( 9 );
		cursor.insertText("Будинок: "+bQuery.value(1).toString()+" "+bQuery.value(2).toString(),textCharFormat_bold);
		
		aQuery.exec("SELECT count(*) FROM abonent \
					WHERE Budynok_ID="+bQuery.value(0).toString()
					+(rah>0 ? " and Rahunok="+sqlStr(rah) : ""));
		aQuery.next();
		int aRowCount = aQuery.value(0).toInt();
		postupForm->setNumPos(aRowCount);
		postupForm->setPostupLabel("Формування: "+bQuery.value(1).toString()+" "+bQuery.value(2).toString());
		postupForm->setCurrentPos(0);
		
		table = cursor.insertTable(4+aRowCount*2, 16, tableFormat);
		
		tableCell.setTextTable(table);
	
		blockFormat.setBottomMargin(0);
		blockFormat.setTopMargin(0);
		
		//Заповнення шапки таблиці
		textCharFormat.setFontPointSize( 9 );
		textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
		textCharFormat_bold.setFontPointSize( 9 );
		textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignMiddle);
		tableCell.setAlignment(Qt::AlignCenter);
		
		table->mergeCells(0, 0, 4, 1);
		table->mergeCells(0, 1, 4, 1);
		table->mergeCells(0, 2, 4, 1);
		table->mergeCells(0, 3, 4, 1);
		
		table->mergeCells(0, 4, 1, 12-begDate.month()+1);
		table->mergeCells(0, 4+12-begDate.month()+1, 1, endDate.month());
		
		tableCell.set(0,0,"№ з/п", textCharFormat);
		tableCell.set(0,1,"Рахунок", textCharFormat);
		tableCell.set(0,2,"П І Б", textCharFormat);
		tableCell.set(0,3,"Квар-тира", textCharFormat);
		
		//Заповнення номера місяця
		int col=4;
		for (QDate xDate=begDate; xDate<=endDate; xDate=xDate.addMonths(1)) {
			tableCell.set(1,col,QString::number(xDate.month()).rightJustified(2,'0'), textCharFormat);
			tableCell.set(2,col,"Нарах.", textCharFormat);
			tableCell.set(3,col,"Викор.", textCharFormat);
			col++;
		}
		
		tableCell.set(0,4,begDate.year(), textCharFormat);
		if (begDate.month() != 1)
			tableCell.set(0,4+12-begDate.month()+1,endDate.year(), textCharFormat);
		
		textCharFormat.setFontPointSize( 8 );
		textCharFormat.setVerticalAlignment(QTextCharFormat::AlignTop);
		textCharFormat_bold.setFontPointSize( 8 );
		textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignTop);
		
		int iter = 0;
		aQuery.exec("SELECT Rahunok, Prizv, Imya, Batk, Kvartyra, Opal, GarVoda, NarahStoyak, Ploshcha FROM abonent \
					WHERE Budynok_ID="+bQuery.value(0).toString()+" \
						" + (rah>0 ? QString(" and Rahunok=")+sqlStr(rah) : QString(""))+" \
					ORDER BY Kvartyra");
		int row;
		while (aQuery.next()) {
			row = 4+iter*2;
			table->mergeCells(row, 0, 2, 1);
			table->mergeCells(row, 1, 2, 1);
			table->mergeCells(row, 2, 2, 1);
			table->mergeCells(row, 3, 2, 1);
			
			//№ з/п
			tableCell.setAlignment(Qt::AlignCenter);
			tableCell.set(row,0,iter+1, textCharFormat);
			//Рахунок
			tableCell.setAlignment(Qt::AlignRight);
			tableCell.set(row,1,aQuery.value(0).toInt(), textCharFormat);
			//ПІБ
			tableCell.setAlignment(Qt::AlignLeft);
			tableCell.set(row,2,aQuery.value(1).toString()+" "+aQuery.value(2).toString()
								+" "+aQuery.value(3).toString(), textCharFormat);
			//Квартира
			tableCell.setAlignment(Qt::AlignRight);
			tableCell.set(row,3,aQuery.value(4).toInt(), textCharFormat);
			
			//Сума + використання
			int col=4;
			for (QDate xDate=begDate; xDate<=endDate; xDate=xDate.addMonths(1)) {
				query.exec("SELECT sum(Narah) FROM narahuvannya \
							WHERE Rahunok="+aQuery.value(0).toString()+" \
								and year="+sqlStr(xDate.year())+" \
								and month="+sqlStr(xDate.month()));
				query.next();
				tableCell.setDash(row,col,query.value(0).toDouble(), textCharFormat);
				
				double diuchTaryf = diuchTaryfInMonth(bQuery.value(0).toInt(), xDate.year(), xDate.month());
				double plos = aQuery.value(8).toDouble();
				double t = taryf[col-4];
				double gkal = 0;
				if ( t>0.009 )
					gkal = diuchTaryf * plos / t;
				tableCell.set(row+1,col,gkal>0.00009 ? QString::number(gkal,'f',4).replace('.',',') : "-", textCharFormat);
				
				col++;
			}
			
			iter++;
			postupForm->incPos();
		}
	}
	
	printform->printer()->setDocName("Відомість споживання ЦО за рік");
	//printform->printer()->setOrientation( QPrinter::Landscape );
	printform->show();
	
	delete postupForm;
}
//------------------------------------------------------------
void UPrintDocs::print_vidomistSpojyvannyaGVZaRik(short year, short month, int budId, int rah)
{
	//Створення документа тарифів для всіх будинків за X місяць, Y рік
	printform = new UPrintForm("Відомість споживання ГВ за рік",10,7,7,10);
	QTextCursor cursor(printform->document()), cellCursor;
	
	QSqlQuery bQuery, aQuery, query;
	
	//Перевстановлення ідентифікатора будинку, якщо рахунок >0
	if (rah>0) {
		query.exec("SELECT Budynok_id FROM abonent WHERE Rahunok="+sqlStr(rah));
		query.next();
		budId = query.value(0).toInt();
	}
	
	UPostupForm *postupForm = new UPostupForm(0, 0);
	postupForm->showLable(true);
	postupForm->show();
	
	QDate endDate(year, month, 1);
	QDate begDate = endDate.addMonths(-11);
	
	//Змінні таблиці
	QTextTableFormat tableFormat;
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(1);
	tableFormat.setBorder(0.5);
	tableFormat.setHeaderRowCount(4);
	QVector<QTextLength> constraints;
	constraints.clear();
	constraints << QTextLength(QTextLength::FixedLength, 25);
	constraints << QTextLength(QTextLength::FixedLength, 30);
	constraints << QTextLength(QTextLength::FixedLength, 100);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 50);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	tableFormat.setColumnWidthConstraints(constraints);
	QTextTable *table=NULL;
	UPopulateCell tableCell(table);
	
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
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 7 );
    cursor.insertText("Сформовано ПК \"ТКЕнерго менеджер\" (супровід Компанія \"АбонПро\". веб: www.abonpro.com). Час формування: "+QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy")+"р." , textCharFormat);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertText(" ",textCharFormat);
	textCharFormat.setFontPointSize( 11 );
	textCharFormat_bold.setFontPointSize( 11 );
	blockFormat.setAlignment( Qt::AlignCenter );
	cursor.insertBlock(blockFormat);
	cursor.insertText("ВІДОМІСТЬ\nспоживання ГВ за рік", textCharFormat_bold);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	cursor.insertText("Аналізований період: "+begDate.toString("MM.yyyy")+"р. - "+endDate.toString("MM.yyyy")+"р.", textCharFormat);
	//cursor.insertBlock(blockFormat);
	
	bool firstBud=true;
	bQuery.exec("SELECT id, Vulycia, Bud_num, Lichylnyk, Koef_pereved FROM budynky "
				+(budId>0?QString("WHERE id=")+sqlStr(budId):QString(""))+
				QString(" ORDER BY Vulycia, Bud_num"));
	while (bQuery.next()){
		cursor.movePosition(QTextCursor::End);
		blockFormat.setAlignment( Qt::AlignLeft );
		if (firstBud){
			cursor.insertBlock(blockFormat);
			firstBud = false;
		}
		else{
			blockFormat.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysBefore);
			cursor.insertBlock(blockFormat);
			blockFormat.setPageBreakPolicy(QTextFormat::PageBreak_Auto);
		}
		textCharFormat.setFontPointSize( 9 );
		textCharFormat_bold.setFontPointSize( 9 );
		cursor.insertText("Будинок: "+bQuery.value(1).toString()+" "+bQuery.value(2).toString(),textCharFormat_bold);
		
		aQuery.exec("SELECT count(*) FROM abonent \
					WHERE Budynok_ID="+bQuery.value(0).toString()
					+(rah>0 ? " and Rahunok="+sqlStr(rah) : ""));
		aQuery.next();
		int aRowCount = aQuery.value(0).toInt();
		postupForm->setNumPos(aRowCount);
		postupForm->setPostupLabel("Формування: "+bQuery.value(1).toString()+" "+bQuery.value(2).toString());
		postupForm->setCurrentPos(0);
		
		table = cursor.insertTable(4+aRowCount*2, 16, tableFormat);
		
		tableCell.setTextTable(table);
	
		blockFormat.setBottomMargin(0);
		blockFormat.setTopMargin(0);
		
		//Заповнення шапки таблиці
		textCharFormat.setFontPointSize( 9 );
		textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
		textCharFormat_bold.setFontPointSize( 9 );
		textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignMiddle);
		tableCell.setAlignment(Qt::AlignCenter);
		
		table->mergeCells(0, 0, 4, 1);
		table->mergeCells(0, 1, 4, 1);
		table->mergeCells(0, 2, 4, 1);
		table->mergeCells(0, 3, 4, 1);
		
		table->mergeCells(0, 4, 1, 12-begDate.month()+1);
		table->mergeCells(0, 4+12-begDate.month()+1, 1, endDate.month());
		
		tableCell.set(0,0,"№ з/п", textCharFormat);
		tableCell.set(0,1,"Рахунок", textCharFormat);
		tableCell.set(0,2,"П І Б", textCharFormat);
		tableCell.set(0,3,"Квар-тира", textCharFormat);
		
		//Заповнення номера місяця
		int col=4;
		for (QDate xDate=begDate; xDate<=endDate; xDate=xDate.addMonths(1)) {
			tableCell.set(1,col,QString::number(xDate.month()).rightJustified(2,'0'), textCharFormat);
			tableCell.set(2,col,"Нарах.", textCharFormat);
			tableCell.set(3,col,"Викор.", textCharFormat);
			col++;
		}
		
		tableCell.set(0,4,begDate.year(), textCharFormat);
		if (begDate.month() != 1)
			tableCell.set(0,4+12-begDate.month()+1,endDate.year(), textCharFormat);
		
		textCharFormat.setFontPointSize( 8 );
		textCharFormat.setVerticalAlignment(QTextCharFormat::AlignTop);
		textCharFormat_bold.setFontPointSize( 8 );
		textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignTop);
		
		int iter = 0;
		aQuery.exec("SELECT Rahunok, Prizv, Imya, Batk, Kvartyra, Opal, GarVoda, NarahStoyak, Ploshcha FROM abonent \
					WHERE Budynok_ID="+bQuery.value(0).toString()+" \
						" + (rah>0 ? QString(" and Rahunok=")+sqlStr(rah) : QString(""))+" \
					ORDER BY Kvartyra");
		int row;
		while (aQuery.next()) {
			row = 4+iter*2;
			table->mergeCells(row, 0, 2, 1);
			table->mergeCells(row, 1, 2, 1);
			table->mergeCells(row, 2, 2, 1);
			table->mergeCells(row, 3, 2, 1);
			
			//№ з/п
			tableCell.setAlignment(Qt::AlignCenter);
			tableCell.set(row,0,iter+1, textCharFormat);
			//Рахунок
			tableCell.setAlignment(Qt::AlignRight);
			tableCell.set(row,1,aQuery.value(0).toInt(), textCharFormat);
			//ПІБ
			tableCell.setAlignment(Qt::AlignLeft);
			tableCell.set(row,2,aQuery.value(1).toString()+" "+aQuery.value(2).toString()
								+" "+aQuery.value(3).toString(), textCharFormat);
			//Квартира
			tableCell.setAlignment(Qt::AlignRight);
			tableCell.set(row,3,aQuery.value(4).toInt(), textCharFormat);
			
			//Сума + використання
			int col=4;
			for (QDate xDate=begDate; xDate<=endDate; xDate=xDate.addMonths(1)) {
				query.exec("SELECT sum(Narah_GV) FROM narahuvannya \
							WHERE Rahunok="+aQuery.value(0).toString()+" \
								and year="+sqlStr(xDate.year())+" \
								and month="+sqlStr(xDate.month()));
				query.next();
				tableCell.setDash(row,col,query.value(0).toDouble(), textCharFormat);
				
				query.exec("SELECT sum(VykorVody) FROM spojyvannyaGV \
							WHERE Rahunok_ID="+aQuery.value(0).toString()+" \
								and year="+sqlStr(xDate.year())+" \
								and month="+sqlStr(xDate.month()));
				query.next();
				tableCell.setDash(row+1,col,query.value(0).toDouble(), textCharFormat);
				
				col++;
			}
			
			iter++;
			postupForm->incPos();
		}
	}
	
	printform->printer()->setDocName("Відомість споживання ГВ за рік");
	//printform->printer()->setOrientation( QPrinter::Landscape );
	printform->show();
	
	delete postupForm;
}
//------------------------------------------------------------
void UPrintDocs::print_rozrahunok_pererahuvannya_obsyagu_tepl_energ(int rozrahId)
{
	//Створення документа тарифів для всіх будинків за X місяць, Y рік
	printform = new UPrintForm("Розрахунок обрахування обсягу теплової енергії",10,7,7,10);
	QTextCursor cursor(printform->document()), cellCursor;
	
	QSqlQuery query;
	query.exec("SELECT * FROM gas_monthRozrah WHERE id="+sqlStr(rozrahId));
	query.next();
	QSqlRecord rec = query.record();
	QDate dt = rec.value("CDate").toDate();
	
	UPostupForm *postupForm = new UPostupForm(0, 0);
	postupForm->show();
	
	//Змінні таблиці
	QTextTableFormat tableFormat;
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(1);
	tableFormat.setBorder(0.7);
	tableFormat.setHeaderRowCount(2);
	tableFormat.setBorderBrush(QColor(Qt::black));
	QVector<QTextLength> constraints;
	constraints.clear();
	constraints << QTextLength(QTextLength::FixedLength, 400);
	constraints << QTextLength(QTextLength::FixedLength, 150);
	constraints << QTextLength(QTextLength::FixedLength, 80);
	
	tableFormat.setColumnWidthConstraints(constraints);
	
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
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 7 );
    cursor.insertText("Сформовано ПК \"ТКЕнерго менеджер\" (супровід Компанія \"АбонПро\". веб: www.abonpro.com). Час формування: "+QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy")+"р." , textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	cursor.insertText(" ",textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 10 );
	textCharFormat_bold.setFontPointSize( 10 );
	cursor.insertText("Додаток №3\n", textCharFormat);
	cursor.insertText("до Методики визначення обсягів природного газу,\n", textCharFormat);
	cursor.insertText("які використовуються для виробництва теплової\n", textCharFormat);
	cursor.insertText("енергії для населення в разі, якщо суб'єкти\n", textCharFormat);
	cursor.insertText("господарювання здійснюють постачання таплової енергії\n", textCharFormat);
	cursor.insertText("різним категоріям споживачів\n", textCharFormat);
	blockFormat.setAlignment( Qt::AlignLeft );
	textCharFormat.setFontPointSize( 12 );
	textCharFormat_bold.setFontPointSize( 12 );
	blockFormat.setAlignment( Qt::AlignCenter );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("РОЗРАХУНОК\nперерахування обсягу теплової енергії,\n", textCharFormat_bold);
	cursor.insertText("отриманої населенням, в обсяг газу,\nвикористаного на її вироблення\n", textCharFormat_bold);
	cursor.insertText("за "+monthlist[dt.month()-1]+" місяць "+QString::number(dt.year())+" року\n", textCharFormat_bold);
	cursor.insertText("теплогенеруючим підприємством\n", textCharFormat_bold);
	cursor.insertText("ПТМ\"Самбіртеплокомуненерго\"\n", textCharFormat_bold);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	
	QTextTable *table = cursor.insertTable(10, 3, tableFormat);
	UPopulateCell tableCell(table);
	
	blockFormat.setBottomMargin(0);
	blockFormat.setTopMargin(0);
	
	//Заповнення шапки таблиці
	textCharFormat.setFontPointSize( 10 );
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_bold.setFontPointSize( 10 );
	textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	tableCell.setAlignment(Qt::AlignCenter);	
	tableCell.set(0,0,"Назва", textCharFormat);
	tableCell.set(0,1,"Номер формули (за потреби),\nпознака показника,\nодиниця виміру", textCharFormat);
	tableCell.set(0,2,"Значення\nпоказника", textCharFormat);
	tableCell.set(1,0,"1", textCharFormat);
	tableCell.set(1,1,"2", textCharFormat);
	tableCell.set(1,2,"3", textCharFormat);
		
	textCharFormat.setFontPointSize( 10 );
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignTop);
	textCharFormat_bold.setFontPointSize( 10 );
	textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignTop);
	
	// 1.
	tableCell.setAlignment(Qt::AlignLeft);
	tableCell.set(2,0,"1. Загальний обсяг газу, поставленого теплогенеруючим підприємством відгазотранспортного підприємства", textCharFormat);
	tableCell.set(2,1,"Q, тис.куб.м (приведених до стандартних умов)", textCharFormat);
	tableCell.setAlignment(Qt::AlignRight);
	tableCell.set(2,2,QString::number(rec.value("ObsyagPostavl").toDouble(),'f',3).replace('.',','), textCharFormat);
	
	// 2.
	tableCell.setAlignment(Qt::AlignLeft);
	table->mergeCells(3,0,1,3);
	tableCell.set(3,0,"2. Загальний обсяг виробленої теплогенеруючим підприємством теплової енергії", textCharFormat);
	
	// 2.1
	tableCell.setAlignment(Qt::AlignLeft);
	tableCell.set(4,0,"2.1. Середній ККД теплогенераційного обладнання теплогенеруючого підприємства", textCharFormat);
	tableCell.set(4,1,"(ета) сер", textCharFormat);
	tableCell.setAlignment(Qt::AlignRight);
	tableCell.set(4,2,QString::number(rec.value("SerKKD").toDouble(),'f',4).replace('.',','), textCharFormat);
	
	// 2.2
	tableCell.setAlignment(Qt::AlignLeft);
	tableCell.set(5,0,"2.2. Середня за звітний період нижча теплотворча здатність газу за сертифікатом газотранспортного підприємства", textCharFormat);
	tableCell.set(5,1,"q н, Гкал", textCharFormat);
	tableCell.setAlignment(Qt::AlignRight);
	tableCell.set(5,2,QString::number(rec.value("TeplotvZdatn").toDouble(),'f',3).replace('.',','), textCharFormat);
	
	// 2.3
	tableCell.setAlignment(Qt::AlignLeft);
	tableCell.set(6,0,"2.3. Загальний обсяг теплової енергії, виробленої обладнанням теплогенеруючого підприємства з звітний період", textCharFormat);
	tableCell.set(6,1,"q вир, Гкал", textCharFormat);
	tableCell.setAlignment(Qt::AlignRight);
	tableCell.set(6,2,QString::number(rec.value("ObsyagVyroblTeploenerg").toDouble(),'f',3).replace('.',','), textCharFormat);
	
	// 3.
	tableCell.setAlignment(Qt::AlignLeft);
	table->mergeCells(7,0,1,3);
	tableCell.set(7,0,"3. Обсяг теплової енергії, відпущеної за завтний період теплогенеруючим підприємством населенню", textCharFormat);
	
	QSqlQuery budQuery;
	budQuery.exec("SELECT count(*) \
				FROM gas_BudPokazn \
				LEFT JOIN budynky ON budynky.id=gas_BudPokazn.Budynok_id \
				WHERE gas_BudPokazn.UseBud and gas_BudPokazn.Rozrah_id="+sqlStr(rozrahId));
	budQuery.next();
	int rowCount = budQuery.value(0).toInt();
	table->insertRows(8, rowCount);
	
	budQuery.exec("SELECT budynky.Vulycia, budynky.Bud_Num, gas_BudPokazn.ObsyagVidpushchTE \
				FROM gas_BudPokazn \
				LEFT JOIN budynky ON budynky.id=gas_BudPokazn.Budynok_id \
				WHERE gas_BudPokazn.UseBud and gas_BudPokazn.Rozrah_id="+sqlStr(rozrahId)+" \
				ORDER BY budynky.Vulycia, budynky.Bud_Num");
	int counter = 1;
	while (budQuery.next()) {
		// 3x
		tableCell.setAlignment(Qt::AlignLeft);
		tableCell.set(7+counter,0,"3."+QString::number(counter)
							+" Обсяг теплової енергії для надання послуг з опалення для населення за звітний період по буд. "
							+budQuery.value(0).toString()+" "+budQuery.value(1).toString(), textCharFormat);
		tableCell.set(7+counter,1,"q вир, Гкал", textCharFormat);
		tableCell.setAlignment(Qt::AlignRight);
		tableCell.set(7+counter,2,QString::number(budQuery.value(2).toDouble(),'f',3).replace('.',','), textCharFormat);
		
		counter++;
	}
	
	int row = 7 + counter;
	
	// 3.Xn+1
	tableCell.setAlignment(Qt::AlignLeft);
	tableCell.set(row,0,"3."+QString::number(counter)+". Відпущена населенню теплова енергія", textCharFormat);
	tableCell.set(row,1,"q від, Гкал", textCharFormat);
	tableCell.setAlignment(Qt::AlignRight);
	tableCell.set(row,2,QString::number(rec.value("ObsyagVidpushchTeploenerg").toDouble(),'f',3).replace('.',','), textCharFormat);
	
	// 4/
	row++;
	tableCell.setAlignment(Qt::AlignLeft);
	tableCell.set(row,0,"4. Перерахований обсяг газу, використаний для виробництва теплової енергії для населення", textCharFormat);
	tableCell.set(row,1,"q насел., куб.м (приведених до стандартних умов)", textCharFormat);
	tableCell.setAlignment(Qt::AlignRight);
	tableCell.set(row,2,QString::number(rec.value("PererahObsyagGazu").toDouble(),'f',0).replace('.',','), textCharFormat);
	
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	textCharFormat_bold.setFontPointSize( 11 );
	cursor.insertText("Інженер ВТВ ПТМ \"Самбіртеплокомуненерго\"			Галайко О.С.", textCharFormat);
	
	printform->printer()->setDocName("Розрахунок обрахування обсягу теплової енергії");
	printform->show();
	
	delete postupForm;
}
//------------------------------------------------------------	
void UPrintDocs::print_dovidnykAbonentivVRozriziNadannyaPoslug(int budId)
{
	QSqlQuery budQuery, query;
	budQuery.exec("SELECT count(*) FROM budynky "
					+(budId>0?QString("WHERE id=")+QString::number(budId):QString()));
	budQuery.next();
	UPostupForm *postupForm = new UPostupForm(0, budQuery.value(0).toInt());
	postupForm->show();
	
	printform = new UPrintForm("Довідник абонентів  в розрізі надання послуг",10,7,7,10);
	QTextCursor cursor(printform->document()), cellCursor;
	
	QTextTableFormat tableFormat;
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(1);
	tableFormat.setBorder(0.5);
	tableFormat.setHeaderRowCount(1);
	QVector<QTextLength> constraints;
	constraints.clear();
	constraints << QTextLength(QTextLength::FixedLength, 260);
	constraints << QTextLength(QTextLength::FixedLength, 160);
	constraints << QTextLength(QTextLength::FixedLength, 160);
	constraints << QTextLength(QTextLength::FixedLength, 130);
	tableFormat.setColumnWidthConstraints(constraints);
	QTextTable *table=NULL;
	UPopulateCell tableCell(table);
	
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
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 7 );
    cursor.insertText("Сформовано ПК \"ТКЕнерго менеджер\" (супровід Компанія \"АбонПро\". веб: www.abonpro.com). Час формування: "+QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy")+"р." , textCharFormat);
	blockFormat.setAlignment( Qt::AlignCenter );
	cursor.insertBlock(blockFormat);
	cursor.insertText(" ",textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	textCharFormat_bold.setFontPointSize( 11 );
	cursor.insertText("ДОВІДНИК АБОНЕНТІВ\nв розрізі надання послуг", textCharFormat_bold);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText("Організація: "+OrganizName(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("Інформація станом на: "+QDateTime().currentDateTime().toString("dd.MM.yyyyр. hh:mm"), textCharFormat);
	
	textCharFormat.setFontPointSize( 9 );
	textCharFormat_bold.setFontPointSize( 9 );
	textCharFormat_italic.setFontPointSize( 9 );
	
	int abonCount[4] = {0,0,0,0};
	double plosSum[4] = {0,0,0,0};
	double dVal;
	budQuery.exec("SELECT id, Vulycia, Bud_num FROM budynky "
					+(budId>0?QString("WHERE id=")+QString::number(budId):QString()));
	while (budQuery.next()) {
		cursor.movePosition(QTextCursor::End);
		cursor.insertBlock(blockFormat);
		
		blockFormat.setAlignment( Qt::AlignLeft );
		cursor.insertBlock(blockFormat);
		cursor.insertText("Будинок по вул. "+budQuery.value(1).toString()+" "+budQuery.value(2).toString(), textCharFormat_bold);
		cursor.insertBlock(blockFormat);
		
		table = cursor.insertTable(3, 4, tableFormat);
		tableCell.setTextTable(table);
		
		textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
		textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
		textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignMiddle);
		
		tableCell.setAlignment(Qt::AlignCenter);
		tableCell.setTextCharFormat(textCharFormat_italic);
		tableCell.set(0,0,"Всього абонентів (кварт.:пл.;)");
		tableCell.set(0,1,"Отримують послуги ЦО\n(кварт.:пл.;)");
		tableCell.set(0,2,"Законно від'єднані\nвід ЦО (кварт.:пл.;)");
		tableCell.set(0,3,"Самов. від'єднані\nвід ЦО (кварт.:пл.;)");
		
		textCharFormat.setVerticalAlignment(QTextCharFormat::AlignTop);
		textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignTop);
		textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignTop);
		
		bool prevPresent;
		tableCell.setAlignment(Qt::AlignLeft);
		tableCell.setTextCharFormat(textCharFormat);
		//"Всього абонентів"
		tableCell.set(1,0,"");
		prevPresent = false;
		query.exec("SELECT id, Kvartyra, SufiksKvart, Ploshcha  FROM abonent \
					WHERE Budynok_id="+budQuery.value(0).toString());
		while (query.next()) {
			dVal = uDToM(query.value(3).toDouble());
			tableCell.insertText( prevPresent ? QString(" ") : QString(" ") );
			tableCell.insertText( query.value(1).toString()+(query.value(2).toString().size()>0 ? QString("-")
								+ query.value(2).toString():QString()), textCharFormat_bold );
			tableCell.insertText( ":"+QString::number(dVal).replace('.',',')+";");
			abonCount[0]++;
			plosSum[0] += dVal;
			prevPresent = true;
		}
		//"Отримують послуги ЦО"
		tableCell.set(1,1,"");
		prevPresent = false;
		query.exec("SELECT id, Kvartyra, SufiksKvart, Ploshcha  FROM abonent \
					WHERE Budynok_id="+budQuery.value(0).toString()+" \
						and Opal=true and SamovVidednCO=false");
		while (query.next()) {
			dVal = uDToM(query.value(3).toDouble());
			tableCell.insertText( prevPresent ? QString(" ") : QString(" ") );
			tableCell.insertText( query.value(1).toString()+(query.value(2).toString().size()>0 ? QString("-")
								+ query.value(2).toString():QString()), textCharFormat_bold );
			tableCell.insertText( ":"+QString::number(dVal).replace('.',',')+";");
			abonCount[1]++;
			plosSum[1] += dVal;
			prevPresent = true;
		}
		//"Законно від'єднані від ЦО"
		tableCell.set(1,2,"");
		prevPresent = false;
		query.exec("SELECT id, Kvartyra, SufiksKvart, Ploshcha  FROM abonent \
					WHERE Budynok_id="+budQuery.value(0).toString()+" \
						and Opal=false and SamovVidednCO=false");
		while (query.next()) {
			dVal = uDToM(query.value(3).toDouble());
			tableCell.insertText( prevPresent ? QString(" ") : QString(" ") );
			tableCell.insertText( query.value(1).toString()+(query.value(2).toString().size()>0 ? QString("-")
								+ query.value(2).toString():QString()), textCharFormat_bold );
			tableCell.insertText( ":"+QString::number(dVal).replace('.',',')+";");
			abonCount[2]++;
			plosSum[2] += dVal;
			prevPresent = true;
		}
		//"Самовільно від'єднані від ЦО"
		tableCell.set(1,3,"");
		prevPresent = false;
		query.exec("SELECT id, Kvartyra, SufiksKvart, Ploshcha  FROM abonent \
					WHERE Budynok_id="+budQuery.value(0).toString()+" \
						and SamovVidednCO=true");
		while (query.next()) {
			dVal = uDToM(query.value(3).toDouble());
			tableCell.insertText( prevPresent ? QString(" ") : QString(" ") );
			tableCell.insertText( query.value(1).toString()+(query.value(2).toString().size()>0 ? QString("-")
								+ query.value(2).toString():QString()), textCharFormat_bold );
			tableCell.insertText( ":"+QString::number(dVal).replace('.',',')+";");
			abonCount[3]++;
			plosSum[3] += dVal;
			prevPresent = true;
		}
		
		tableCell.setAlignment(Qt::AlignLeft);
		//"Всього абонентів"
		tableCell.set(2,0,"Кількість абонентів: "+QString::number(abonCount[0])
							+";\nПлоща: "+QString::number(plosSum[0]).replace('.',',')+".");
		//"Отримують послуги ЦО"
		tableCell.set(2,1,"Кількість абонентів: "+QString::number(abonCount[1])
							+";\nПлоща: "+QString::number(plosSum[1]).replace('.',',')+".");
		//"Законно від'єднані від ЦО"
		tableCell.set(2,2,"Кількість абонентів: "+QString::number(abonCount[2])
							+";\nПлоща: "+QString::number(plosSum[2]).replace('.',',')+".");
		//"Самовільно від'єднані від ЦО"
		tableCell.set(2,3,"К-сть абон.: "+QString::number(abonCount[3])
							+";\nПлоща: "+QString::number(plosSum[3]).replace('.',',')+".");
		
		postupForm->incPos();
	}
	
	printform->printer()->setDocName("Довідник абонентів  в розрізі надання послуг");
	printform->show();
	
	delete postupForm;
}
//------------------------------------------------------------	
void UPrintDocs::print_printVidomistBorjnykivOtrymSubs(QDate dt)
{
	QDate prevDate = dt.addMonths(-1);
	QDate nextDate = dt.addMonths(1);
	
	QSqlQuery query;
	query.exec("SELECT count(*) \
				FROM \
				((( \
				(SELECT Rahunok_ID, first(iif(Plata_CO is null,0,Plata_CO)) AS plCO, \
						first(iif(Plata_GV is null,0,Plata_GV)) AS plGV \
				FROM plataZgidnoSubs \
				WHERE year="+sqlStr(prevDate.year())+" \
					and month="+sqlStr(prevDate.month())+" \
				GROUP BY Rahunok_id) AS obpl \
				 \
				LEFT JOIN \
					(SELECT Rahunok_id, sum(Suma) AS oplSuma FROM kvytancii \
					WHERE year(Plata_date)="+sqlStr(dt.year())+" \
						and month(Plata_date)="+sqlStr(dt.month())+" \
					GROUP BY Rahunok_id) AS opl \
				ON opl.Rahunok_id=obpl.Rahunok_id) \
				LEFT JOIN abonent ON abonent.Rahunok=obpl.Rahunok_id) \
				LEFT JOIN budynky ON abonent.Budynok_id=budynky.id) \
				WHERE obpl.plCO+obpl.plGV-iif(opl.oplSuma is null,0,opl.oplSuma)>0");
	query.next();
	int rowCount = query.value(0).toInt();
	UPostupForm *postupForm = new UPostupForm(0, rowCount);
	postupForm->show();
	
	printform = new UPrintForm("Відомість боржників серед отримувачів субсидії",10,7,7,10);
	QTextCursor cursor(printform->document()), cellCursor;
	
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
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 7 );
    cursor.insertText("Сформовано ПК \"ТКЕнерго менеджер\" (супровід Компанія \"АбонПро\". веб: www.abonpro.com). Час формування: "+QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy")+"р." , textCharFormat);
	blockFormat.setAlignment( Qt::AlignCenter );
	cursor.insertBlock(blockFormat);
	cursor.insertText(" ",textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	textCharFormat_bold.setFontPointSize( 11 );
	cursor.insertText("ВІДОМІСТЬ БОРЖНИКІВ\nсеред отримувачів субсидій", textCharFormat_bold);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText("Організація: "+OrganizName(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("Аналіз оплати за: "+dt.toString("MM.yyyy"), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	
	// Створення таблиці
	QTextTableFormat tableFormat;
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(1);
	tableFormat.setBorder(0.5);
	tableFormat.setHeaderRowCount(1);
	QVector<QTextLength> constraints;
	constraints.clear();
	constraints << QTextLength(QTextLength::FixedLength, 30);
	constraints << QTextLength(QTextLength::FixedLength, 40);
	constraints << QTextLength(QTextLength::FixedLength, 170);
	constraints << QTextLength(QTextLength::FixedLength, 160);
	constraints << QTextLength(QTextLength::FixedLength, 60);
	constraints << QTextLength(QTextLength::FixedLength, 60);
	constraints << QTextLength(QTextLength::FixedLength, 60);
	constraints << QTextLength(QTextLength::FixedLength, 60);
	constraints << QTextLength(QTextLength::FixedLength, 60);
	tableFormat.setColumnWidthConstraints(constraints);
	QTextTable *table = cursor.insertTable(1+rowCount, 9, tableFormat);;
	UPopulateCell tableCell(table);
	
	textCharFormat.setFontPointSize( 8 );
	textCharFormat_bold.setFontPointSize( 8 );
	textCharFormat_italic.setFontPointSize( 8 );
	
	// Заповнення заголовка таблиці
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignMiddle);	
	tableCell.setAlignment(Qt::AlignCenter);
	tableCell.setTextCharFormat(textCharFormat_italic);
	tableCell.set(0,0,"№ з/п");
	tableCell.set(0,1,"Рах.");
	tableCell.set(0,2,"ПІБ");
	tableCell.set(0,3,"Адреса");
	tableCell.set(0,4,"Об.пл.ЦО у "+prevDate.toString("MM.yyyy"));
	tableCell.set(0,5,"Об.пл.ГВ у "+prevDate.toString("MM.yyyy"));
	tableCell.set(0,6,"Оплата у "+dt.toString("MM.yyyy"));
	tableCell.set(0,7,"Різн.");
	tableCell.set(0,8,"Борг за "+dt.toString("MM.yyyy"));
	
	int row = 0;
	double dVal, rizn;
	query.exec("SELECT obpl.Rahunok_id, abonent.Prizv, abonent.Imya, abonent.Batk, budynky.Vulycia, \
						budynky.Bud_num, abonent.Kvartyra, abonent.SufiksKvart, \
						obpl.plCO, obpl.plGV, opl.oplSuma \
				FROM \
				((( \
				(SELECT Rahunok_ID, first(iif(Plata_CO is null,0,Plata_CO)) AS plCO, \
						first(iif(Plata_GV is null,0,Plata_GV)) AS plGV \
				FROM plataZgidnoSubs \
				WHERE year="+sqlStr(prevDate.year())+" \
					and month="+sqlStr(prevDate.month())+" \
				GROUP BY Rahunok_id) AS obpl \
				 \
				LEFT JOIN \
					(SELECT Rahunok_id, sum(Suma) AS oplSuma FROM kvytancii \
					WHERE year(Plata_date)="+sqlStr(dt.year())+" \
						and month(Plata_date)="+sqlStr(dt.month())+" \
					GROUP BY Rahunok_id) AS opl \
				ON opl.Rahunok_id=obpl.Rahunok_id) \
				LEFT JOIN abonent ON abonent.Rahunok=obpl.Rahunok_id) \
				LEFT JOIN budynky ON abonent.Budynok_id=budynky.id) \
				WHERE obpl.plCO+obpl.plGV-iif(opl.oplSuma is null,0,opl.oplSuma)>0 \
				ORDER BY obpl.Rahunok_id");
	for (row = 1; row <= rowCount && query.next(); row++ ) {
		textCharFormat.setVerticalAlignment(QTextCharFormat::AlignTop);
		textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignTop);
		textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignTop);	
		tableCell.setTextCharFormat(textCharFormat);
		
		// "№ з/п"
		tableCell.setAlignment(Qt::AlignRight);
		tableCell.set(row,0,row);
		// "Рах."
		tableCell.set(row,1,query.value(0).toInt());
		// "ПІБ"
		tableCell.setAlignment(Qt::AlignLeft);
		tableCell.set(row,2,query.value(1).toString()+" "+query.value(2).toString()+" "+query.value(3).toString());
		// "Адреса"
		tableCell.set(row,3,query.value(4).toString()+" "+query.value(5).toString()
							+(query.value(6).toInt()>0 ? QString(" кв.")+query.value(6).toString() : QString())
							+(query.value(7).toString().size()>0 ? QString("-")+query.value(7).toString() : QString())
						);
		// "Об.пл.ЦО"
		tableCell.setAlignment(Qt::AlignRight);
		dVal = uDToM(query.value(8).toDouble());
		rizn = dVal;
		tableCell.setDash(row,4,dVal);
		// "Об.пл.ГВ"
		dVal = uDToM(query.value(9).toDouble());
		rizn += dVal;
		tableCell.setDash(row,5,dVal);
		// "Оплата"
		dVal = uDToM(query.value(10).toDouble());
		rizn -= dVal;
		tableCell.setDash(row,6,dVal);
		// "Різн."
		tableCell.setDash(row,7,rizn);
		// "Борг"
		tableCell.setDash(row,8,vhSaldoInMonth(query.value(0).toInt(), nextDate.year(), nextDate.month()));
		
		postupForm->incPos();
	}
	
	printform->printer()->setDocName("Відомість боржників серед отримувачів субсидії");
	printform->show();
	
	delete postupForm;
}
//------------------------------------------------------------	
void UPrintDocs::print_printVidomistZaborgPoPeriodah(QDate dt)
{
	unsigned char rowCount = 9;
	UPostupForm *postupForm = new UPostupForm(0, rowCount);
	postupForm->show();
	
	printform = new UPrintForm("Відомість заборгованості за періоди",15,7,10,10);
	QTextCursor cursor(printform->document()), cellCursor;
	
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
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 7 );
    cursor.insertText("Сформовано ПК \"ТКЕнерго менеджер\" (супровід Компанія \"АбонПро\". веб: www.abonpro.com). Час формування: "+QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy")+"р." , textCharFormat);
	blockFormat.setAlignment( Qt::AlignCenter );
	cursor.insertBlock(blockFormat);
	cursor.insertText(" ",textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	textCharFormat_bold.setFontPointSize( 11 );
	cursor.insertText("ВІДОМІСТЬ ЗАБОРГОВАНОСТІ\nу розрізі давності боргу", textCharFormat_bold);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText("Організація: "+OrganizName(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("Базовий місяць аналізу: "+dt.toString("MM.yyyy")+"р.", textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("Дата формування: "+QDate::currentDate().toString("dd.MM.yyyy")+"р.", textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	
	// Створення таблиці
	QTextTableFormat tableFormat;
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(3);
	tableFormat.setBorder(0.5);
	tableFormat.setHeaderRowCount(1);
	QVector<QTextLength> constraints;
	constraints.clear();
	constraints << QTextLength(QTextLength::FixedLength, 200);
	constraints << QTextLength(QTextLength::FixedLength, 100);
	constraints << QTextLength(QTextLength::FixedLength, 100);
	constraints << QTextLength(QTextLength::FixedLength, 100);
	constraints << QTextLength(QTextLength::FixedLength, 100);
	tableFormat.setColumnWidthConstraints(constraints);
	QTextTable *table = cursor.insertTable(rowCount+2, 5, tableFormat);;
	UPopulateCell tableCell(table);
	
	textCharFormat.setFontPointSize( 10 );
	textCharFormat_bold.setFontPointSize( 10 );
	textCharFormat_italic.setFontPointSize( 10 );
	
	// Заповнення заголовка таблиці
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat_bold.setVerticalAlignment(QTextCharFormat::AlignMiddle);	
	tableCell.setAlignment(Qt::AlignCenter);
	tableCell.setTextCharFormat(textCharFormat_italic);
	
	table->mergeCells(0,0,2,1);
	table->mergeCells(0,1,1,2);
	table->mergeCells(0,3,1,2);
	
	tableCell.set(0,0,"Термін");
	tableCell.set(0,1,"Населеня");
	tableCell.set(1,1,"К-сть абон.");
	tableCell.set(1,2,"Сума, грн");
	tableCell.set(0,3,"Організації");
	tableCell.set(1,3,"К-сть організ.");
	tableCell.set(1,4,"Сума, грн");
	
	QSqlQuery query;
	
	short row = 1;
		// Заповнення інформації
	tableCell.setTextCharFormat(textCharFormat);
	// Поточна заборгов
	tableCell.setAlignment(Qt::AlignLeft);
	tableCell.set(++row,0,"Заборгов на початок місяця, без від'ємних сальдо");
	tableCell.setAlignment(Qt::AlignRight);
	query.exec("SELECT count(*), sum(Vh_saldo) FROM narahuvannya \
				WHERE Year="+sqlStr(dt.year())+" \
					and month="+sqlStr(dt.month())+" \
					and Status=0 and Vh_saldo>0");
	query.next();
	tableCell.set(row,1,query.value(0).toInt());
	tableCell.set(row,2,uMToStr2(query.value(1).toDouble()));
	query.exec("SELECT count(*), sum(Vh_saldo) FROM saldo \
				WHERE year(CurDate)="+sqlStr(dt.year())+" \
					and month(CurDate)="+sqlStr(dt.month())+" and vh_saldo>0");
	query.next();
	tableCell.set(row,3,query.value(0).toInt());
	tableCell.set(row,4,uMToStr2(query.value(1).toDouble()));
	postupForm->incPos();
	
	// 1 місяць
	tableCell.setAlignment(Qt::AlignLeft);
	tableCell.set(++row,0,"до 1-го місяця");
	query.exec(_zaborgNaselQuery(dt.addMonths(-1), dt));
	query.next();
	tableCell.setAlignment(Qt::AlignRight);
	tableCell.set(row,1,query.value(0).toInt());
	tableCell.set(row,2,uMToStr2(query.value(1).toDouble()));
	query.exec(_zaborgOrgQuery(dt.addMonths(-1), dt));
	query.next();
	tableCell.set(row,3,query.value(0).toInt());
	tableCell.set(row,4,uMToStr2(query.value(1).toDouble()));
	postupForm->incPos();
	
	// 3 місяці
	tableCell.setAlignment(Qt::AlignLeft);
	tableCell.set(++row,0,"до 3-ьох місяців");
	query.exec(_zaborgNaselQuery(dt.addMonths(-3), dt));
	query.next();
	tableCell.setAlignment(Qt::AlignRight);
	tableCell.set(row,1,query.value(0).toInt());
	tableCell.set(row,2,uMToStr2(query.value(1).toDouble()));
	query.exec(_zaborgOrgQuery(dt.addMonths(-3), dt));
	query.next();
	tableCell.set(row,3,query.value(0).toInt());
	tableCell.set(row,4,uMToStr2(query.value(1).toDouble()));
	postupForm->incPos();
	
	// 6 місяці
	tableCell.setAlignment(Qt::AlignLeft);
	tableCell.set(++row,0,"до 6-ти місяців");
	query.exec(_zaborgNaselQuery(dt.addMonths(-6), dt));
	query.next();
	tableCell.setAlignment(Qt::AlignRight);
	tableCell.set(row,1,query.value(0).toInt());
	tableCell.set(row,2,uMToStr2(query.value(1).toDouble()));
	query.exec(_zaborgOrgQuery(dt.addMonths(-6), dt));
	query.next();
	tableCell.set(row,3,query.value(0).toInt());
	tableCell.set(row,4,uMToStr2(query.value(1).toDouble()));
	postupForm->incPos();
	
	// 9 місяці
	tableCell.setAlignment(Qt::AlignLeft);
	tableCell.set(++row,0,"до 9-ти місяців");
	query.exec(_zaborgNaselQuery(dt.addMonths(-9), dt));
	query.next();
	tableCell.setAlignment(Qt::AlignRight);
	tableCell.set(row,1,query.value(0).toInt());
	tableCell.set(row,2,uMToStr2(query.value(1).toDouble()));
	query.exec(_zaborgOrgQuery(dt.addMonths(-9), dt));
	query.next();
	tableCell.set(row,3,query.value(0).toInt());
	tableCell.set(row,4,uMToStr2(query.value(1).toDouble()));
	postupForm->incPos();
	
	// 1 рік
	tableCell.setAlignment(Qt::AlignLeft);
	tableCell.set(++row,0,"до 1-го року");
	query.exec(_zaborgNaselQuery(dt.addMonths(-12), dt));
	query.next();
	tableCell.setAlignment(Qt::AlignRight);
	tableCell.set(row,1,query.value(0).toInt());
	tableCell.set(row,2,uMToStr2(query.value(1).toDouble()));
	query.exec(_zaborgOrgQuery(dt.addMonths(-12), dt));
	query.next();
	tableCell.set(row,3,query.value(0).toInt());
	tableCell.set(row,4,uMToStr2(query.value(1).toDouble()));
	postupForm->incPos();
	
	// 2 роки
	tableCell.setAlignment(Qt::AlignLeft);
	tableCell.set(++row,0,"до 2-ох років");
	query.exec(_zaborgNaselQuery(dt.addMonths(-24), dt));
	query.next();
	tableCell.setAlignment(Qt::AlignRight);
	tableCell.set(row,1,query.value(0).toInt());
	tableCell.set(row,2,uMToStr2(query.value(1).toDouble()));
	query.exec(_zaborgOrgQuery(dt.addMonths(-24), dt));
	query.next();
	tableCell.set(row,3,query.value(0).toInt());
	tableCell.set(row,4,uMToStr2(query.value(1).toDouble()));
	postupForm->incPos();
	
	// 3 роки
	tableCell.setAlignment(Qt::AlignLeft);
	tableCell.set(++row,0,"до 3-ьох років");
	query.exec(_zaborgNaselQuery(dt.addMonths(-36), dt));
	query.next();
	tableCell.setAlignment(Qt::AlignRight);
	tableCell.set(row,1,query.value(0).toInt());
	tableCell.set(row,2,uMToStr2(query.value(1).toDouble()));
	query.exec(_zaborgOrgQuery(dt.addMonths(-36), dt));
	query.next();
	tableCell.set(row,3,query.value(0).toInt());
	tableCell.set(row,4,uMToStr2(query.value(1).toDouble()));
	postupForm->incPos();
	
	// Безнадійна заборгованість, більше 3-ьох років
	tableCell.setAlignment(Qt::AlignLeft);
	tableCell.set(++row,0,"Більше трьох років (безнадійна)");
	query.exec(_zaborgNaselQuery(dt.addMonths(-37), dt));
	query.next();
	tableCell.setAlignment(Qt::AlignRight);
	tableCell.set(row,1,query.value(0).toInt());
	tableCell.set(row,2,uMToStr2(np(query.value(1).toDouble())));
	query.exec(_zaborgOrgQuery(dt.addMonths(-37), dt));
	query.next();
	tableCell.set(row,3,query.value(0).toInt());
	tableCell.set(row,4,uMToStr2(query.value(1).toDouble()));
	postupForm->incPos();
	
	
	// Підписи
	textCharFormat.setFontPointSize( 8 );
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("_____________",textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("Спосіб формування: підсумовуються більші за нуль вхідні сальдо початку періоду давності, за мінусом оплати протягом періоду, якщо ця різниця більша за нуль", textCharFormat);
	textCharFormat.setFontPointSize( 10 );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("     "+DocsPosadaPidpysText(3)+"					"+PrizvIB(3), textCharFormat);
	
	printform->printer()->setDocName("Відомість заборгованості за періоди");
	printform->show();
	
	delete postupForm;
}
//------------------------------------------------------------
QString UPrintDocs::_zaborgNaselQuery(const QDate & begDate, const QDate & curDate)
{
	QDate _begDt(begDate.year(), begDate.month(), 1);
	QDate _curDt(curDate.year(), curDate.month(), curDate.daysInMonth());
	QString q = "SELECT count(*), sum(tSaldo.Vh_saldo - iif(tKvt.sumaKv is NULL, 0, tKvt.sumaKv)) FROM \
				(SELECT Rahunok, Vh_saldo FROM narahuvannya \
				WHERE Year="+sqlStr(_begDt.year())+" \
						and month="+sqlStr(_begDt.month())+" and Status=0 and vh_saldo>0) AS tSaldo \
				LEFT JOIN (SELECT Rahunok_id, sum(Suma) AS sumaKv FROM kvytancii \
							WHERE Plata_date BETWEEN cdate('"+_begDt.toString("yyyy-MM-dd")+"') \
								AND cdate('"+_curDt.toString("yyyy-MM-dd")+"') \
							GROUP BY Rahunok_id) AS tKvt \
				ON tSaldo.Rahunok=tKvt.Rahunok_id \
				WHERE tSaldo.Vh_saldo - iif(tKvt.sumaKv is NULL, 0, tKvt.sumaKv) > 0.009";
	return q;
}
//------------------------------------------------------------
QString UPrintDocs::_zaborgOrgQuery(const QDate & begDate,const QDate & curDate)
{
	QDate _begDt(begDate.year(), begDate.month(), 1);
	QDate _curDt(curDate.year(), curDate.month(), curDate.daysInMonth());
	QString q = "SELECT count(*), sum(tSaldo.Vh_saldo - iif(tKvt.sumaKv is NULL, 0, tKvt.sumaKv)) FROM \
				(SELECT Ugoda_id, Vh_saldo FROM saldo \
				WHERE year(CurDate)="+sqlStr(_begDt.year())+" \
					and month(CurDate)="+sqlStr(_begDt.month())+" and vh_saldo>0) AS tSaldo \
				LEFT JOIN (SELECT Organiz_id, sum(Suma) AS sumaKv FROM oplata \
							WHERE Pererah_date BETWEEN cdate('"+_begDt.toString("yyyy-MM-dd")+"') \
								AND cdate('"+_curDt.toString("yyyy-MM-dd")+"') \
							GROUP BY Organiz_id) AS tKvt \
				ON tSaldo.Ugoda_id=tKvt.Organiz_id \
				WHERE tSaldo.Vh_saldo - iif(tKvt.sumaKv is NULL, 0, tKvt.sumaKv) > 0.009";
	return q;
}
//------------------------------------------------------------
double UPrintDocs::np(double val)
{
	if (val > 0)
		return val;
	else
		return 0;
}
//------------------------------------------------------------
