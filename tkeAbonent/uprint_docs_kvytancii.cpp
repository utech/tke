/****************************************************************************
**
** Created: Roman Mulyk 12.01.2008
**
****************************************************************************/

#include "uprint_docs.h"
#include <UPopulateTextTableCells>
#include <USqlAccessible>

//------------------------------------------------------------
void UPrintDocs::print_kvytancii(int prev_month, int prev_year, bool one_kvyt, int rah_bud, QList<int> rahList)
{
	if (rahList.size())
		one_kvyt = false;
	int year = date.year();
	int month = date.month();
	int day = date.day();
	int num_kvyt=0;
	UPostupForm *postup_form = NULL;
	QSqlQuery *query = new QSqlQuery();
	QSqlQuery infoStrQuery;
	if ((!one_kvyt) && (!rahList.size())){
			query->exec("SELECT count(*) FROM abonent WHERE (opal=true) and (Budynok_ID="+QVariant(rah_bud).toString()+")");
			query->seek(0);
			postup_form = new UPostupForm(0, query->value(0).toInt() );
			postup_form->show();
	}
	if (rahList.size()) {
		postup_form = new UPostupForm(0, rahList.size() );
		postup_form->show();
	}
	
	infoStrQuery.exec("SELECT strTemplate FROM slujb_znach WHERE id=6");
	infoStrQuery.seek(0);
	bool needPrintPokaznBudLichCO = infoStrQuery.value(0).toBool();
	
	//Читання опалювальної площі будинку
	int budynokId;
	double opalPlosBud=0;
	double koefBudLichCO=1;
	bool isBudLichCO=false;
	double pokaznBudLichCO=0;
	double vykorTeplaNaKvadrLichCO=0;
	if ( one_kvyt){
		query->exec("SELECT Budynok_ID FROM abonent WHERE Rahunok="+sqlStr(rah_bud));
		query->seek(0);
		budynokId = query->value(0).toInt();
	}
	else
		budynokId = rah_bud;
	
    //---- ПОЧАТОК Частина визначення використання кількості ГКал ЦО абонентом, з використанням площі по стояках
	//---- Якщо змінювати, то потрібно змінювати разом в частині перед циклом абонентів і у ньому, оскільки вони ідентичні
	QSqlQuery pQuery;
	opalPlosBud = 0;
		//Опалювальна площа ЦО
	pQuery.exec("SELECT sum(Ploshcha) FROM abonent WHERE Opal=true and Budynok_ID="+sqlStr(budynokId));
	if (pQuery.next())
		opalPlosBud += pQuery.value(0).toDouble();
		//Площа по стояках
	pQuery.exec("SELECT sum(pok.Plos) AS ZagStPloshcha \
					FROM (SELECT Rahunok_ID, Plos FROM pokaznOpalStoyak \
						WHERE Year="+sqlStr(prev_year)+" \
							and Month="+sqlStr(prev_month)+") AS pok \
					LEFT JOIN abonent \
						ON abonent.Rahunok=pok.Rahunok_ID \
					WHERE abonent.Budynok_ID="+sqlStr(budynokId));
	if (pQuery.next())
		opalPlosBud += pQuery.value(0).toDouble();
		//Читання коефіцієнта будинкового лічильника ЦО і його наявність
	pQuery.exec("SELECT Lichylnyk, Koef_pereved FROM budynky WHERE id="+sqlStr(budynokId));
	pQuery.next();
	if (pQuery.value(0).toInt() == 1){
		koefBudLichCO = pQuery.value(1).toDouble();
		isBudLichCO = true;
		//Читання показників будинкового лічильника ЦО
		pQuery.exec("SELECT Lich_poch, Lich_kin FROM diuch_taryf \
						WHERE Budynok_ID="+sqlStr(budynokId)+" \
							and year="+sqlStr(prev_year)+" \
							and month="+sqlStr(prev_month));
		pQuery.next();
		pokaznBudLichCO = pQuery.value(1).toDouble() - pQuery.value(0).toDouble();
		if (pokaznBudLichCO < 0)
			pokaznBudLichCO = 0;
		
		if (koefBudLichCO>0 && opalPlosBud>0)
			vykorTeplaNaKvadrLichCO = pokaznBudLichCO / (koefBudLichCO * opalPlosBud);
	}
	//---- Якщо змінювати, то потрібно змінювати разом в частині перед циклом абонентів і у ньому, оскільки вони ідентичні
	//---- КІНЕЦЬ Частина визначення використання кількості ГКал ЦО абонентом, з використанням площі по стояках
	
	//Читання тарифів
	UNasTaryfInfo taryfInfo = nasTaryf(prev_year, prev_month);
		
	bool needCO, needGV;
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	needCO = settings->value("GUI_View/ShowNarahCO").toBool();
	needGV = settings->value("GUI_View/ShowNarahGV").toBool();
	delete settings;
	
	//Читання інформації про необхідність друку нульових та від'ємних квитанцій та автоматичного розрахунку субсидії
	query->exec("SELECT strTemplate FROM slujb_znach WHERE id=9");
	query->seek(0);
	bool optionNeedPrintZeroNegativKvyt = query->value(0).toBool();

    enum ObovyazkPlataSubsType {
        Ignore = 0,
        AutoCalcSubs = 1,
        UseObovyazkPlata = 2
    };
	query->exec("SELECT strTemplate FROM slujb_znach WHERE id=10");
	query->seek(0);
    int obovyazkPlataSubsType = query->value(0).toInt();
	
	//читання інформації про необхідність додавання до сальдо дебіторської заборгованості
	query->exec("SELECT strTemplate FROM slujb_znach WHERE id=18");
	query->seek(0);
	const bool needAddBeznDebit = query->value(0).toBool();
	
	//Читання необхідності створення друку нормативного тарифу (false) або діючого тарифу (true)
	//query->exec("SELECT strTemplate FROM slujb_znach WHERE id=20");
	query->exec("SELECT Lichylnyk FROM budynky WHERE id ="+sqlStr(budynokId));
	query->seek(0);
	const bool printTaryfTypeCO = query->value(0).toBool();
	double diuchTaryfBud = diuchTaryfInMonth(budynokId, prev_year, prev_month);
	
	
	//Читання неохідності друку банківських реквізитів, та при потребі самих реквізитів
	query->exec("SELECT strTemplate FROM slujb_znach WHERE id=24");
	query->seek(0);
	const bool needPrintBankRekvizyty = query->value(0).toBool();
	QString bankRekvizyty;
	if (needPrintBankRekvizyty) {
		query->exec("SELECT strTemplate FROM slujb_znach WHERE id=25");
		query->seek(0);
		bankRekvizyty = query->value(0).toString();
	}
	
		//Створення форми для друку
	printform = new UPrintForm("Квитанції", 4, 4, 2, 1);
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat, textCharFormat_bold;
		//Створення документа нарахування оплати за ЦО за X місяць, Y рік
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
		//Налаштування формату майбутніх таблиць
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
	constraints << QTextLength(QTextLength::FixedLength, 155);
	constraints << QTextLength(QTextLength::FixedLength, 60);
	constraints << QTextLength(QTextLength::FixedLength, 5);
	constraints << QTextLength(QTextLength::FixedLength, 220);
	constraints << QTextLength(QTextLength::FixedLength, 175); // 175
	constraints << QTextLength(QTextLength::FixedLength, 60);  // 100
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(2);
		//Створення стрічки для фільтрування по абоненту
	QString filtrString;
	if (rahList.size()) {
		filtrString = " (";
		bool needOr = false;
		foreach(int xRah, rahList){
			filtrString += (needOr?" or":"")+QString(" Rahunok=")+sqlStr(xRah);
			needOr = true;
		}
		filtrString += ") ";
	}
	else {
		if (one_kvyt)
			filtrString = " (Rahunok="+ QVariant(rah_bud).toString() +") ";
		else
			filtrString = " (Budynok_ID="+QVariant(rah_bud).toString()+") ";
	}
	query->exec("SELECT abonent.Rahunok, abonent.Prizv, abonent.Imya, abonent.Batk, \
						budynky.Vulycia, budynky.Bud_num, \
						abonent.Kvartyra, abonent.Ploshcha, \
						narahuvannyaSaldo.Vh_saldo, narahuvannyaSum.Narah_Sum, \
						pilgy.Kategoriya, pilgy.Vidsotok, \
						subs.Suma AS Suma_s, subs.Suma_d, \
						spl_zaborg.Suma AS Suma_u, abonent.Pilga_for, \
						pilgy.Ploshsca, abonent.Pilga_ID, \
						kvytancii.Suma, narahuvannyaSum.Narah_GV_Sum, \
						abonent.Budynok_ID, narahuvannyaSum.Narah_St_Sum, \
						abonent.Opal, abonent.GarVoda, abonent.NarahStoyak, \
						abonent.Ludy, \
						subs.Suma_GV AS Suma_s_GV, subs.Suma_d_GV, \
						narPereved.suma_perev, \
						pilgy2.Kategoriya, pilgy2.Vidsotok, \
						pilgy2.Ploshsca, abonent.Pilga_for_2, \
						abonent.Pilga_ID_2, abonent.SufiksKvart \
					FROM ((((((((((SELECT * FROM abonent \
						WHERE "
						+ filtrString +" ORDER BY Kvartyra) AS abonent \
					LEFT JOIN (SELECT Rahunok, Vh_saldo FROM narahuvannya \
						WHERE (year="+ QVariant(prev_year).toString() +") \
						and (month="+ QVariant(prev_month).toString() +") and Status=0) \
						AS narahuvannyaSaldo ON (abonent.Rahunok=narahuvannyaSaldo.Rahunok)) \
					LEFT JOIN (SELECT Rahunok, sum(Narah) AS Narah_Sum, sum(Narah_GV) AS Narah_GV_Sum, sum(Narah_St) AS Narah_St_Sum FROM narahuvannya \
						WHERE (year="+ QVariant(prev_year).toString() +") \
							and (month="+ QVariant(prev_month).toString() +") \
						GROUP BY Rahunok) \
						AS narahuvannyaSum ON (abonent.Rahunok=narahuvannyaSum.Rahunok)) \
					LEFT JOIN pilgy ON (abonent.Pilga_ID=pilgy.id)) \
					LEFT JOIN (SELECT * FROM pilgy) AS pilgy2 ON (abonent.Pilga_ID_2=pilgy2.id)) \
					LEFT JOIN (SELECT Rahunok_ID, sum(subs.Suma) AS Suma, sum(subs.Suma_d) AS Suma_d, sum(subs.Suma_GV) AS Suma_GV, sum(subs.Suma_d_GV) AS Suma_d_GV FROM subs \
						WHERE subs.year="+ QVariant(prev_year).toString() +" \
						and subs.month="+ QVariant(prev_month).toString() +" GROUP BY Rahunok_ID) \
						AS subs ON abonent.Rahunok=subs.Rahunok_ID) \
					LEFT JOIN (SELECT * FROM spl_zaborg \
						WHERE ((spl_zaborg.Poch_year<"+ QVariant(prev_year).toString() +") \
						or (spl_zaborg.Poch_year="+ QVariant(prev_year).toString() +" \
						and spl_zaborg.Poch_month<="+ QVariant(prev_month).toString() +") ) \
						and ((spl_zaborg.Kin_year>"+ QVariant(prev_year).toString() +") \
						or (spl_zaborg.Kin_year="+ QVariant(prev_year).toString() +" \
						and spl_zaborg.Kin_month>="+ QVariant(prev_month).toString() +") )) \
						AS spl_zaborg ON (abonent.Rahunok=spl_zaborg.Rahunok_ID)) \
					LEFT JOIN budynky ON abonent.Budynok_ID=budynky.id) \
					LEFT JOIN (SELECT kvytancii.Rahunok_Id, sum(kvytancii.Suma) AS Suma FROM kvytancii \
						WHERE (year(Plata_date)="+ QVariant(prev_year).toString() +") \
						and (month(Plata_date)="+ QVariant(prev_month).toString() +") Group BY Rahunok_ID) \
						AS kvytancii ON abonent.Rahunok=kvytancii.Rahunok_ID) \
					 \
					LEFT JOIN (SELECT Rahunok_ID, sum(Suma) AS suma_perev FROM narahuvannyaPereved \
						WHERE year(CDate)="+ QVariant(prev_year).toString() +" \
							and month(CDate)="+ QVariant(prev_month).toString() +" \
						GROUP BY Rahunok_ID) AS narPereved \
						ON narPereved.Rahunok_ID=abonent.Rahunok) \
					 \
					ORDER BY abonent.Kvartyra");
	bool first_kvyt=true;
	QSqlQuery *lichQuery = new QSqlQuery();
	int kvytPosition = 0;
	while (query->next()){
		//Змінні операцій абонента
		double saldo = query->value(8).toDouble();
		if (needAddBeznDebit){
			lichQuery->exec("SELECT Vh_saldo, ZaborgAdd, OplataSuma FROM debitorskaZaborg \
						WHERE CDate <= cDate('"+QDate(prev_year, prev_month, QDate(prev_year, prev_month, 1).daysInMonth()).toString("yyyy-MM-dd")+"') and Rahunok="+query->value(0).toString()+" \
						ORDER BY CDate DESC");
			lichQuery->next();
			saldo += uDToM(lichQuery->value(0).toDouble() + lichQuery->value(1).toDouble() - lichQuery->value(2).toDouble());
		}
		double narah = query->value(9).toDouble() + query->value(21).toDouble();
		if (!needCO)
			narah = 0;
		double narah_GV = query->value(19).toDouble();
		if (!needGV)
			narah_GV = 0;
		double subs = query->value(12).toDouble()+query->value(13).toDouble() + query->value(26).toDouble()+query->value(27).toDouble();
		double zaborg = query->value(14).toDouble();
		double oplacheno = query->value(18).toDouble();
		double oplata_po_zaborg;
		double pererahSum = uDToM(query->value(28).toDouble());
		double oplata = saldo + narah + narah_GV - subs - oplacheno + pererahSum;
		
		if (zaborg==0 || saldo<0)
			oplata_po_zaborg = 0;
		else{
			if (zaborg > saldo)
				zaborg = saldo;
			oplata_po_zaborg = narah + narah_GV + zaborg - subs;
		}
		
		double sumOp = 0;
		bool useOp = false;

        switch (obovyazkPlataSubsType) {
        case AutoCalcSubs: {
            UAbonObPlSubs obPlSubs = oplataZgidnoNarahSubs(query->value(0).toInt(), prev_year, prev_month);
            useOp = obPlSubs.need;
            sumOp = obPlSubs.suma;
            break;
        }
        case UseObovyazkPlata: {
            QSqlQuery obPlataSubsQuery;
            obPlataSubsQuery.exec("SELECT Plata_CO, Plata_GV FROM plataZgidnoSubs \
                                    WHERE Rahunok_id="+sqlStr(query->value(0).toInt())+" \
                                        and Year="+sqlStr(prev_year)+" \
                                        and Month="+sqlStr(prev_month));
            if (obPlataSubsQuery.next()) {
                useOp = true;
                sumOp = obPlataSubsQuery.value(0).toDouble() + obPlataSubsQuery.value(1).toDouble();
            }
            else {
                obovyazkPlataSubsType = Ignore;
            }
            break;
        }
        }
		
		bool neadPrintKvyt = false;
		if ( optionNeedPrintZeroNegativKvyt && (oplata < 0.009) )
			if (query->value(22).toBool() || query->value(23).toBool()){
				oplata = 0;
				neadPrintKvyt = true;
			}
		
		if ( (oplata > 0.009 && (!useOp))|| (sumOp > 0.009 && useOp) || neadPrintKvyt || one_kvyt ){
			
			if (rahList.size()) {
				budynokId = query->value(20).toInt();
				
				//---- ПОЧАТОК Частина визначення використання кількості ГКал ЦО абонентом, з використанням площі по стояках
				//---- Якщо змінювати, то потрібно змінювати разом в частині перед циклом абонентів і у ньому, оскільки вони ідентичні
				QSqlQuery pQuery;
				opalPlosBud = 0;
					//Опалювальна площа ЦО
				pQuery.exec("SELECT sum(Ploshcha) FROM abonent WHERE Opal=true and Budynok_ID="+sqlStr(budynokId));
				if (pQuery.next())
					opalPlosBud += pQuery.value(0).toDouble();
					//Площа по стояках
				pQuery.exec("SELECT sum(pok.Plos) AS ZagStPloshcha \
								FROM (SELECT Rahunok_ID, Plos FROM pokaznOpalStoyak \
									WHERE Year="+sqlStr(prev_year)+" \
										and Month="+sqlStr(prev_month)+") AS pok \
								LEFT JOIN abonent \
									ON abonent.Rahunok=pok.Rahunok_ID \
								WHERE abonent.Budynok_ID="+sqlStr(budynokId));
				if (pQuery.next())
					opalPlosBud += pQuery.value(0).toDouble();
					//Читання коефіцієнта будинкового лічильника ЦО і його наявність
				pQuery.exec("SELECT Lichylnyk, Koef_pereved FROM budynky WHERE id="+sqlStr(budynokId));
				pQuery.next();
				if (pQuery.value(0).toInt() == 1){
					koefBudLichCO = pQuery.value(1).toDouble();
					isBudLichCO = true;
					//Читання показників будинкового лічильника ЦО
					pQuery.exec("SELECT Lich_poch, Lich_kin FROM diuch_taryf \
									WHERE Budynok_ID="+sqlStr(budynokId)+" \
										and year="+sqlStr(prev_year)+" \
										and month="+sqlStr(prev_month));
					pQuery.next();
					pokaznBudLichCO = pQuery.value(1).toDouble() - pQuery.value(0).toDouble();
					if (pokaznBudLichCO < 0)
						pokaznBudLichCO = 0;
					
					if (koefBudLichCO>0 && opalPlosBud>0)
						vykorTeplaNaKvadrLichCO = pokaznBudLichCO / (koefBudLichCO * opalPlosBud);
				}
				//---- Якщо змінювати, то потрібно змінювати разом в частині перед циклом абонентів і у ньому, оскільки вони ідентичні
				//---- КІНЕЦЬ Частина визначення використання кількості ГКал ЦО абонентом, з використанням площі по стояках
			}
			
			if (!first_kvyt){
				QTextCharFormat spliterTextCharFormat;
				spliterTextCharFormat.setFontPointSize(2);
				cursor.movePosition(QTextCursor::End);
				cursor.insertBlock(blockFormat);
				if (kvytPosition != 0)
					cursor.insertText(" \n - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n ", spliterTextCharFormat);
			}
			first_kvyt = false;
			blockFormat.setAlignment( Qt::AlignHCenter );
			cursor.setBlockFormat( blockFormat );
			kvytPosition++;
			if (kvytPosition >= 3){
				tableFormat.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysAfter);
				kvytPosition = 0;
			}
			else
				tableFormat.setPageBreakPolicy(QTextFormat::PageBreak_Auto);
				//створення і налаштування таблиці
			QTextTable *table = cursor.insertTable(9, 6, tableFormat);
			table->mergeCells ( 0, 0, 3, 2 );
			table->mergeCells ( 0, 3, 1, 3 );
			table->mergeCells ( 1, 3, 6, 1 );
			table->mergeCells ( 7, 3, 2, 3 );
			table->mergeCells ( 0, 2, 9, 1 );
			//Заповнення колонок квитанції
				// Шапка
			QString str_month;
			str_month.clear();
			if (month<10)
				str_month = "0";
			str_month += QVariant(month).toString();
				//Частина підприємства
			cell = table->cellAt(0, 0);
			textCharFormat.setFontPointSize( 9 );
			textCharFormat_bold.setFontPointSize( 8 );
			QFont font = textCharFormat_bold.font();
			font.setBold( true );
			textCharFormat_bold.setFont(font);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Повідомлення від ")
				+ QVariant(day).toString()
				+ "."
				+ str_month
				+ "."
				+ QVariant(year).toString()
				+ codec->toUnicode("р.\nКасир:\n\n")
				+ codec->toUnicode("Одержувач - "), textCharFormat );
			cellCursor.insertText( OrganizName()+"\n", textCharFormat_bold );
			cellCursor.insertText( codec->toUnicode("центр.опал. за ")
									+ monthlist[prev_month-1]
									+ " "
									+ QVariant(prev_year).toString()
									+ codec->toUnicode(" р.")
									+ codec->toUnicode("\nОсобовий рахунок "), textCharFormat );
			cellCursor.insertText( query->value(0).toString()+ "\n", textCharFormat_bold );
			QString strIPB="";
			if (query->value(2).toString().size()>0){
				strIPB += QString(" ") + query->value(2).toString().at(0) + ".";
				if (query->value(3).toString().size()>0)
					strIPB += QString(" ") + query->value(3).toString().at(0) + ".";
			}
			cellCursor.insertText( (query->value(1).toString() + strIPB).toUpper(), textCharFormat_bold );
			cellCursor.insertText( codec->toUnicode("\nвул. ")+query->value(4).toString() + " "
										+query->value(5).toString(), textCharFormat );
			cellCursor.insertText( codec->toUnicode(" кв. ")+query->value(6).toString()+query->value(34).toString(), textCharFormat );
				//Частина абонента
			cell = table->cellAt(0, 3);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Рахунок-квитанція            від ")
				+ QVariant(day).toString()
				+ "."
				+ str_month
				+ "."
				+ QVariant(year).toString()
				+ codec->toUnicode("р.       за ")
				+ monthlist[prev_month-1]/*codec->toUnicode("жовтень-листопад")*/
				+ " "
				+ QVariant(prev_year).toString()
				+ codec->toUnicode(" р.\nКасир:                                   Одержувач платежу - "), textCharFormat );
			cellCursor.insertText( OrganizName(), textCharFormat_bold );
			
			// !!! Частина додана для створення таблиці, у якій записуватимуться банківські реквізити
			QTextTableFormat tableFormatBankRekvizyty;
			QVector<QTextLength> constraintsBankRekvizyty;
			constraintsBankRekvizyty << QTextLength(QTextLength::FixedLength, 330);
			
			tableFormatBankRekvizyty.setColumnWidthConstraints(constraintsBankRekvizyty);
			tableFormatBankRekvizyty.setCellSpacing(0);
			tableFormatBankRekvizyty.setCellPadding(0);
			tableFormatBankRekvizyty.setBorder(0);
			tableFormatBankRekvizyty.setAlignment(Qt::AlignRight);
			
			UPopulateCell tableCellBankRekvizyty(cellCursor.insertTable(1, 1, tableFormatBankRekvizyty));
			// Друк при необхідності банківських реквізитів
			if (needPrintBankRekvizyty) {
				cellCursor.insertText( bankRekvizyty + "\n", textCharFormat );
			}
			// !!! Частина додана для створення таблиці, у якій записуватимуться банківські реквізити
			
			cellCursor.insertText( "Вид платежу - Послуги за центральне опалення\n"
									"Особовий рахунок ", textCharFormat );
			cellCursor.insertText( query->value(0).toString()+ "\n", textCharFormat_bold );
			QString abonPibStr = query->value(1).toString() + " " + query->value(2).toString() 
									+ " " + query->value(3).toString();
			if (abonPibStr.size() > 35)
				abonPibStr = abonPibStr.mid(0,34)+".";
			cellCursor.insertText( (abonPibStr).toUpper(), textCharFormat_bold );

				//Частина підприємства
			cell = table->cellAt(3, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( (saldo < 0) ? "Переплата" : "Борг на початок місяця", textCharFormat );
			cell = table->cellAt(3, 1);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2((saldo < 0) ? saldo*-1 : saldo), textCharFormat );
			
			cell = table->cellAt(4, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Нараховано за місяць"), textCharFormat );
			cell = table->cellAt(4, 1);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(narah+narah_GV), textCharFormat );
			
			cell = table->cellAt(5, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Нарахування субсидії"), textCharFormat );
			cell = table->cellAt(5, 1);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(subs), textCharFormat );
			
			QTextCharFormat curTextCharFormat;
			if (oplata_po_zaborg>0.00999 || useOp)
				curTextCharFormat = textCharFormat;
			else
				curTextCharFormat = textCharFormat_bold;
			cell = table->cellAt(6, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Сума до оплати\n"), curTextCharFormat );
			// додає два переноси на наступний рядок у випадку друку без банківських реквізитів
			if (!needPrintBankRekvizyty)
				cellCursor.insertText("\n\n\n", curTextCharFormat);
			cell = table->cellAt(6, 1);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
            if ((oplata > 0.009) && (!useOp || obovyazkPlataSubsType == UseObovyazkPlata))
				cellCursor.insertText( uMToStr2(oplata), curTextCharFormat );
			if (oplata < 0)
			    cellCursor.insertText( "0.00", curTextCharFormat );
			if (oplata_po_zaborg>0.00999 || useOp)
				curTextCharFormat = textCharFormat_bold; 
			else
			curTextCharFormat = textCharFormat;
			cell = table->cellAt(7, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			if (!useOp)
                cellCursor.insertText( "До опл. згідно угоди", curTextCharFormat );
            else {
                if (obovyazkPlataSubsType == UseObovyazkPlata)
                    cellCursor.insertText( "Обов'язк. опл. по субс.", curTextCharFormat );
                else
                    cellCursor.insertText( "До опл.згідно субсидії", curTextCharFormat );
            }

			cell = table->cellAt(7, 1);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			if (useOp && (sumOp>0.009))
				cellCursor.insertText( uMToStr2(sumOp+(oplata_po_zaborg>0.009999?oplata_po_zaborg:0)), curTextCharFormat );
			else if ((oplata_po_zaborg > 0.009999) && (!useOp) )
				cellCursor.insertText( uMToStr2(oplata_po_zaborg), curTextCharFormat );
			else
				cellCursor.insertText( "---", curTextCharFormat );
			
			cell = table->cellAt(8, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Оплачено абонентом \n\n"), textCharFormat );
			
				//Частина абонента
			cell = table->cellAt(1, 4);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			if (needCO && !needGV)
				cellCursor.insertText( (saldo < 0) ? "Переплата\nНараховано за місяць" : "Борг на початок місяця\Нараховано за місяць", textCharFormat );
			else{
				cellCursor.insertText((saldo < 0) ? "Переплата\nНараховано за" : "Борг на початок місяця\nНараховано за", textCharFormat );
				if (needCO)
					cellCursor.insertText(" ЦО /", textCharFormat );
				if (needGV)
					cellCursor.insertText(" ГВ", textCharFormat );
			}
			cell = table->cellAt(1, 5);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2((saldo < 0) ? saldo*-1 : saldo)+"\n", textCharFormat );
			if (needCO)
				cellCursor.insertText( uMToStr2(narah), textCharFormat );
			if (needCO && needGV)
				cellCursor.insertText(" / ", textCharFormat );
			if (needGV)
				cellCursor.insertText( uMToStr2(narah_GV), textCharFormat );
			cell = table->cellAt(2, 4);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Нарахування субсидії"), textCharFormat );
			cell = table->cellAt(2, 5);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(subs), textCharFormat );
			
			cell = table->cellAt(3, 4);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Оплачено в поточному місяці"), textCharFormat );
			cell = table->cellAt(3, 5);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(oplacheno), textCharFormat );
			
			if (oplata_po_zaborg>0.00999 || useOp)
				curTextCharFormat = textCharFormat;
			else
				curTextCharFormat = textCharFormat_bold;
			cell = table->cellAt(4, 4);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Сума до оплати"), curTextCharFormat );
			cell = table->cellAt(4, 5);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
            if ((oplata > 0.009) && (!useOp || obovyazkPlataSubsType == UseObovyazkPlata))
				cellCursor.insertText( uMToStr2(oplata), curTextCharFormat );
			
			if (oplata_po_zaborg>0.00999 || useOp)
				curTextCharFormat = textCharFormat_bold;
			else
				curTextCharFormat = textCharFormat;
			cell = table->cellAt(5, 4);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			if (!useOp)
				cellCursor.insertText( codec->toUnicode("До опл. згідно угоди"), curTextCharFormat );
            else {
                if (obovyazkPlataSubsType == UseObovyazkPlata)
                    cellCursor.insertText( "Обов'язк. опл. згідно субс.", curTextCharFormat );
                else
                    cellCursor.insertText( "До опл.згідно субсидії", curTextCharFormat );
            }
			cell = table->cellAt(5, 5);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			if (useOp && (sumOp>0.009))
				cellCursor.insertText( uMToStr2(sumOp+(oplata_po_zaborg>0.009999?oplata_po_zaborg:0)), curTextCharFormat );
			else if ((oplata_po_zaborg > 0.009999) && (!useOp) )
				cellCursor.insertText( uMToStr2(oplata_po_zaborg), curTextCharFormat );
			else
				cellCursor.insertText( "---", curTextCharFormat );
			
			cell = table->cellAt(6, 4);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Оплачено абонентом"), textCharFormat );
			
						//Інформація про абонента
			QTextCharFormat textCharFormatAI = textCharFormat;
			if (needCO && needGV)
				textCharFormatAI.setFontPointSize(8);
			
			double ploshcha_pilgy;
			if (query->value(16).toDouble() == 0)
				ploshcha_pilgy = query->value(15).toDouble() * 21 + 10.5;
			else
				ploshcha_pilgy = query->value(15).toDouble() * query->value(16).toDouble();
			
			double ploshcha_pilgy_2;
			if (query->value(31).toDouble() == 0)
				ploshcha_pilgy_2 = query->value(32).toDouble() * 21;
			else
				ploshcha_pilgy_2 = query->value(31).toDouble() * query->value(32).toDouble();
			
			cell = table->cellAt(1, 3);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Адреса: вул.")+query->value(4).toString() + " "
										+query->value(5).toString()
										+", кв. "+query->value(6).toString()+query->value(34).toString()+"\n", textCharFormatAI );
			if (needGV && (query->value(25).toInt()>0))
				cellCursor.insertText( "Пропис.: "+query->value(25).toString()+" осіб.  ", textCharFormatAI );
			if (needCO && (query->value(22).toBool() || query->value(24).toBool())){
				double abonOpalPlos=0;
				if (query->value(22).toBool())
					abonOpalPlos = query->value(7).toDouble();
				if (query->value(24).toBool()){
					QSqlQuery stQuery("SELECT sum(Plos) FROM pokaznOpalStoyak \
										WHERE Rahunok_ID="+query->value(0).toString()+" \
											and Year="+sqlStr(prev_year)+" \
											and Month="+sqlStr(prev_month));
					stQuery.seek(0);
					abonOpalPlos += stQuery.value(0).toDouble();
				}
				cellCursor.insertText( codec->toUnicode("Опал. площа: ")+uMToStr2(abonOpalPlos)+"\n", textCharFormatAI );
				
				qDebug() << "---------------------------";
				qDebug() << isBudLichCO;
				qDebug() << vykorTeplaNaKvadrLichCO;
				qDebug() << needPrintPokaznBudLichCO;
				
				if (isBudLichCO && (vykorTeplaNaKvadrLichCO>0) && needPrintPokaznBudLichCO)
					cellCursor.insertText( codec->toUnicode("Пок.ліч.ЦО: ")+QString::number(vykorTeplaNaKvadrLichCO*abonOpalPlos,'f',4).replace(".",",")+"\n", textCharFormatAI );
			}
			cellCursor.insertText( codec->toUnicode("Пільга: "), textCharFormatAI );
			if (query->value(17).toInt() > 0)
				cellCursor.insertText( (query->value(33).toInt()>0?"1)":"")+query->value(10).toString()+(query->value(33).toInt()>0?"-":"\n    ")+uMToStr(query->value(11).toDouble())
										+"% на "+query->value(15).toString()
										+" чол., "+uMToStr2(ploshcha_pilgy)
										+"м.кв.", textCharFormatAI );
			else
				cellCursor.insertText( "немає", textCharFormatAI );
				
			if (query->value(33).toInt() > 0)
				cellCursor.insertText( ";2)"+query->value(29).toString()+"-"+uMToStr(query->value(30).toDouble())
										+"% на "+query->value(32).toString()
										+" чол., "+uMToStr2(ploshcha_pilgy_2)
										+"м.кв.", textCharFormatAI );
			cellCursor.insertText( "\n", textCharFormatAI ); //Задає нову стрічку після пільг. НЕ ЧІПАТИ!!!
			
			if (needCO && (query->value(22).toBool() || query->value(24).toBool())){
				cellCursor.insertText( codec->toUnicode("Тариф ЦО: - ")
														+uMToStr2(printTaryfTypeCO ? taryfInfo.normat_taryf_kvadr : diuchTaryfBud)
														+codec->toUnicode(" грн. за кв.м.\n")+
										"                  - " + uMToStr2(taryfInfo.vart_g_kal)
										+codec->toUnicode(" грн. за ГКал\n"), textCharFormatAI );
			}
			if (needGV && query->value(23).toBool()){
				cellCursor.insertText( codec->toUnicode("Тариф ГВ: - ")
														+uMToStr2(taryfInfo.vart_kuba_GV)
														+codec->toUnicode(" грн. за куб.м.\n")
										+"                  - "
														+uMToStr2(taryfInfo.vart_na_lud_in_month_GV)
														+codec->toUnicode(" грн. за особу\n"), textCharFormatAI );
				QDate curLichDate = QDate(prev_year, prev_month, 1);
				lichQuery->exec("SELECT pokaznCur.Pokazn, pokaznCur.Korektura, lich.id \
								FROM (SELECT * FROM lichylnykyAbon \
										WHERE Rahunok="+query->value(0).toString()+") AS lich \
								LEFT JOIN (SELECT * FROM pokaznLichAbon \
										WHERE year(pokaznDate)="+QString::number(prev_year)+" \
											and month(pokaznDate)="+QString::number(prev_month)+") AS pokaznCur \
									ON lich.id=pokaznCur.AbonLich_id");
				while (lichQuery->next()){
					QSqlQuery prevPokaznQuery("SELECT Pokazn FROM pokaznLichAbon \
									WHERE ((year(pokaznDate)="+QString::number(curLichDate.year())+" and month(pokaznDate)<"+QString::number(curLichDate.month())+") \
											or year(pokaznDate)<"+QString::number(curLichDate.year())+") \
											and AbonLich_ID="+lichQuery->value(2).toString()+" \
									ORDER BY pokaznDate DESC");
					prevPokaznQuery.seek(0);
					double prevPokazn = prevPokaznQuery.value(0).toDouble();
					double curKorect = lichQuery->value(1).toDouble();
					double curPokazn = lichQuery->value(0).toDouble();
					if (curPokazn < 0.0000009)
						curPokazn = prevPokazn;
					if ( prevPokazn>0 || curPokazn>0){
						cellCursor.insertText( "Ліч.ГВ.:поч: "+QString::number(prevPokazn+curKorect ).replace('.',',')+" - кінц: "+QString::number(curPokazn).replace('.',',')+".\n", textCharFormatAI);
						
						//cellCursor.insertText( "Попер.показн.ліч.ГВ: "+QString::number(prevPokazn+curKorect)+".\n", textCharFormatAI );
						//cellCursor.insertText( "Поточн.показн.ліч.ГВ: "+QString::number(curPokazn)+".\n", textCharFormatAI );
						//break;
					}
				}
			}
			if (zaborg > 0)
				cellCursor.insertText( "Угода сплати заборг.: "+uMToStr2(zaborg)	+"грн/міс\n", textCharFormatAI );
			if (pererahSum != 0)
				cellCursor.insertText( "Перерахунок.: "+uMToStr2(pererahSum)	+"грн.\n", textCharFormatAI );
			
			cellCursor.deletePreviousChar(); //Видалення останнього переносу
			
						//Повідомлення для абонента
			cell = table->cellAt(7, 3);
			cellCursor = cell.firstCursorPosition();
			textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
			cell.setFormat(textCharFormat);
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			
					//Читання інформаційних стрічок з бази даних і запис їх до інформаційного блоку квитанції
			infoStrQuery.exec("SELECT value FROM slujb_znach WHERE id=4");
			infoStrQuery.seek(0);
			cellCursor.insertText( "Просимо оплатити до "+QString::number(infoStrQuery.value(0).toDouble(),'f',0)+" числа поточного місяця.\n", textCharFormat_bold );
	
			infoStrQuery.exec("SELECT strTemplate FROM slujb_znach WHERE id=2");
			infoStrQuery.seek(0);
			cellCursor.insertText( infoStrQuery.value(0).toString()+"\n", textCharFormat );
	
			infoStrQuery.exec("SELECT strTemplate FROM slujb_znach WHERE id=3");
			infoStrQuery.seek(0);
			cellCursor.insertText( infoStrQuery.value(0).toString(), textCharFormat_bold );
			
			infoStrQuery.exec("SELECT strTemplate FROM slujb_znach WHERE id=23");
			if ( infoStrQuery.next() && infoStrQuery.value(0).toString().trimmed().size() ) {
				cellCursor.insertText( "\n" + infoStrQuery.value(0).toString(), textCharFormat_bold );
			}
			
			num_kvyt++;
		  }
		  if (! one_kvyt)
			postup_form->inc_pos();
	}
	delete lichQuery;
	
	printform->printer()->setDocName("Квитанції за "+monthlist.at(prev_month-1)+" "+QString::number(prev_year)+"р.");
	QMessageBox::information(
				0,
				codec->toUnicode("!!! Закінчено створення квитанцій  !!!"),
				codec->toUnicode("Створено ")+ QVariant(num_kvyt).toString() +codec->toUnicode(" квитанцій."),
				QMessageBox::Ok,
				QMessageBox::NoButton);
	printform->show();
	delete query;
	if (! one_kvyt)
		delete postup_form;
}
//------------------------------------------------------------
void UPrintDocs::print_kvytanciiDlyaBeznadiynoiZaborg(int prev_month, int prev_year, bool one_kvyt, int rah_bud)
{
	int year = date.year();
	int month = date.month();
	int day = date.day();
	int num_kvyt=0;
	UPostupForm *postup_form = NULL;
	QSqlQuery *query = new QSqlQuery();
	QSqlQuery infoStrQuery;
	if (! one_kvyt){
			query->exec("SELECT count(*)  FROM (SELECT Rahunok_ID AS Rahunok FROM narahuvannyaPereved \
						WHERE year(CDate)="+sqlStr(prev_year)+" \
							and month(CDate)="+sqlStr(prev_month)+" \
							and PerevedType_ID=2 \
						GROUP BY Rahunok_ID \
						UNION \
						SELECT Rahunok FROM debitorskaZaborg \
						WHERE year(CDate)="+sqlStr(prev_year)+" \
							and month(CDate)="+sqlStr(prev_month)+") AS unDebRah \
						INNER JOIN (SELECT Rahunok, Budynok_ID FROM abonent WHERE Budynok_ID="+sqlStr(rah_bud)+") AS abonBudFilterTable \
						ON abonBudFilterTable.Rahunok=unDebRah.Rahunok");
			query->seek(0);
			postup_form = new UPostupForm(0, query->value(0).toInt() );
			postup_form->show();
	}
	
	//Читання неохідності друку банківських реквізитів, та при потребі самих реквізитів
	query->exec("SELECT strTemplate FROM slujb_znach WHERE id=24");
	query->seek(0);
	const bool needPrintBankRekvizyty = query->value(0).toBool();
	QString bankRekvizyty;
	if (needPrintBankRekvizyty) {
		query->exec("SELECT strTemplate FROM slujb_znach WHERE id=25");
		query->seek(0);
		bankRekvizyty = query->value(0).toString();
	}
	
		//Створення форми для друку
	printform = new UPrintForm("Квитанції", 4, 4, 2, 1);
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat, textCharFormat_bold;
		//Створення документа нарахування оплати за ЦО за X місяць, Y рік
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
		//Налаштування формату майбутніх таблиць
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
	constraints << QTextLength(QTextLength::FixedLength, 170);
	constraints << QTextLength(QTextLength::FixedLength, 60);
	constraints << QTextLength(QTextLength::FixedLength, 5);
	constraints << QTextLength(QTextLength::FixedLength, 200); //220
	constraints << QTextLength(QTextLength::FixedLength, 175); //175
	constraints << QTextLength(QTextLength::FixedLength, 70);
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(2);
		//Створення стрічки для фільтрування по абоненту
	QString filtrString;
	if (one_kvyt)
		filtrString = " (Rahunok="+ QVariant(rah_bud).toString() +") ";
	else
		filtrString = " (Budynok_ID="+QVariant(rah_bud).toString()+") ";
	query->exec("SELECT abonent.Rahunok, abonent.Prizv, abonent.Imya, abonent.Batk, \
						budynky.Vulycia, budynky.Bud_num, abonent.Kvartyra, abonent.Ploshcha, \
						abonent.Budynok_ID, abonent.Ludy, narPereved.suma_perev, \
						debitorskaZaborg.Vh_saldo, debitorskaZaborg.ZaborgAdd, debitorskaZaborg.OplataSuma \
					FROM ((((( \
					 \
						SELECT unDebRah.Rahunok, abonBudFilterTable.Budynok_ID  FROM (SELECT Rahunok_ID AS Rahunok FROM narahuvannyaPereved \
						WHERE year(CDate)="+sqlStr(prev_year)+" \
							and month(CDate)="+sqlStr(prev_month)+" \
							and PerevedType_ID=2 \
						GROUP BY Rahunok_ID \
						UNION \
						SELECT Rahunok FROM debitorskaZaborg \
						WHERE year(CDate)="+sqlStr(prev_year)+" \
							and month(CDate)="+sqlStr(prev_month)+") AS unDebRah \
						INNER JOIN (SELECT Rahunok, Budynok_ID FROM abonent WHERE "+filtrString+") AS abonBudFilterTable \
						ON abonBudFilterTable.Rahunok=unDebRah.Rahunok \
					 \
						) AS abonToPrint \
					 \
					LEFT JOIN abonent ON abonent.Rahunok=abonToPrint.Rahunok) \
					 \
					LEFT JOIN budynky ON abonToPrint.Budynok_ID=budynky.id) \
					 \
					LEFT JOIN (SELECT Rahunok_ID, sum(Suma) AS suma_perev FROM narahuvannyaPereved \
						WHERE year(CDate)="+sqlStr(prev_year)+" \
							and month(CDate)="+sqlStr(prev_month)+" \
							and PerevedType_id=2 \
						GROUP BY Rahunok_ID) AS narPereved \
						ON narPereved.Rahunok_ID=abonToPrint.Rahunok) \
					 \
					LEFT JOIN (SELECT Rahunok, Vh_saldo, ZaborgAdd, OplataSuma FROM debitorskaZaborg \
						WHERE year(CDate)="+sqlStr(prev_year)+" \
							and month(CDate)="+sqlStr(prev_month)+") AS debZaborg \
						ON debZaborg.Rahunok=abonToPrint.Rahunok) \
					 \
					ORDER BY abonent.Kvartyra");
	bool first_kvyt=true;
	QSqlQuery *lichQuery = new QSqlQuery();
	int kvytPosition = 0;
	while (query->next()){
		//Змінні операцій абонента
		double saldo = query->value(11).toDouble();
		double narah = query->value(12).toDouble() - query->value(10).toDouble();
		double oplacheno = query->value(13).toDouble();
		double oplata = saldo + narah - oplacheno;
		
		if ((oplata > 0.009) || one_kvyt){
			if (!first_kvyt){
				QTextCharFormat spliterTextCharFormat;
				spliterTextCharFormat.setFontPointSize(2);
				cursor.movePosition(QTextCursor::End);
				cursor.insertBlock(blockFormat);
				if (kvytPosition != 0)
					cursor.insertText(" \n - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n ", spliterTextCharFormat);
			}
			first_kvyt = false;
			blockFormat.setAlignment( Qt::AlignHCenter );
			cursor.setBlockFormat( blockFormat );
			kvytPosition++;
			if (kvytPosition >= 3){
				tableFormat.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysAfter);
				kvytPosition = 0;
			}
			else
				tableFormat.setPageBreakPolicy(QTextFormat::PageBreak_Auto);
				//створення і налаштування таблиці
			QTextTable *table = cursor.insertTable(9, 6, tableFormat);
			table->mergeCells ( 0, 0, 3, 2 );
			table->mergeCells ( 0, 3, 1, 3 );
			table->mergeCells ( 1, 3, 6, 1 );
			table->mergeCells ( 7, 3, 2, 3 );
			table->mergeCells ( 0, 2, 9, 1 );
			
			table->mergeCells ( 7, 0, 2, 1 );
			table->mergeCells ( 7, 1, 2, 1 );
			table->mergeCells ( 5, 4, 2, 1 );
			table->mergeCells ( 5, 5, 2, 1 );
			
			//Заповнення колонок квитанції
				// Шапка
			QString str_month;
			str_month.clear();
			if (month<10)
				str_month = "0";
			str_month += QVariant(month).toString();
				//Частина підприємства
			cell = table->cellAt(0, 0);
			textCharFormat.setFontPointSize( 9 );
			textCharFormat_bold.setFontPointSize( 8 );
			QFont font = textCharFormat_bold.font();
			font.setBold( true );
			textCharFormat_bold.setFont(font);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Повідомлення від ")
				+ QVariant(day).toString()
				+ "."
				+ str_month
				+ "."
				+ QVariant(year).toString()
				+ codec->toUnicode("р.\nКасир:\n\n")
				+ codec->toUnicode("Одержувач - "), textCharFormat );
			cellCursor.insertText( OrganizName()+"\n", textCharFormat_bold );
			cellCursor.insertText( codec->toUnicode("центр.опал. за ")
									+ monthlist[prev_month-1]
									+ " "
									+ QVariant(prev_year).toString()
									+ codec->toUnicode(" р.")
									+ codec->toUnicode("\nОсобовий рахунок "), textCharFormat );
			cellCursor.insertText( query->value(0).toString()+ "\n", textCharFormat_bold );
			QString strIPB="";
			if (query->value(2).toString().size()>0){
				strIPB += QString(" ") + query->value(2).toString().at(0) + ".";
				if (query->value(3).toString().size()>0)
					strIPB += QString(" ") + query->value(3).toString().at(0) + ".";
			}
			cellCursor.insertText( (query->value(1).toString() + strIPB).toUpper(), textCharFormat_bold );
			cellCursor.insertText( codec->toUnicode("\nвул. ")+query->value(4).toString() + " "
										+query->value(5).toString(), textCharFormat );
			cellCursor.insertText( codec->toUnicode(" кв. ")+query->value(6).toString(), textCharFormat );
				//Частина абонента
			cell = table->cellAt(0, 3);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Рахунок-квитанція            від ")
				+ QVariant(day).toString()
				+ "."
				+ str_month
				+ "."
				+ QVariant(year).toString()
				+ codec->toUnicode("р.       за ")
				+ monthlist[prev_month-1]
				+ " "
				+ QVariant(prev_year).toString()
				+ codec->toUnicode(" р.\nКасир:                                   Одержувач платежу - "), textCharFormat );
			cellCursor.insertText( OrganizName()+"\n", textCharFormat_bold );
			
			// !!! Частина додана для створення таблиці, у якій записуватимуться банківські реквізити
			QTextTableFormat tableFormatBankRekvizyty;
			QVector<QTextLength> constraintsBankRekvizyty;
			constraintsBankRekvizyty << QTextLength(QTextLength::FixedLength, 330);
			
			tableFormatBankRekvizyty.setColumnWidthConstraints(constraintsBankRekvizyty);
			tableFormatBankRekvizyty.setCellSpacing(0);
			tableFormatBankRekvizyty.setCellPadding(0);
			tableFormatBankRekvizyty.setBorder(0);
			tableFormatBankRekvizyty.setAlignment(Qt::AlignRight);
			
			UPopulateCell tableCellBankRekvizyty(cellCursor.insertTable(1, 1, tableFormatBankRekvizyty));
			// Друк при необхідності банківських реквізитів
			if (needPrintBankRekvizyty) {
				cellCursor.insertText( bankRekvizyty + "\n", textCharFormat );
			}
			// !!! Частина додана для створення таблиці, у якій записуватимуться банківські реквізити
			
			cellCursor.insertText( "Вид платежу - Дебіторська заборгованість\n"
									"Особовий рахунок ", textCharFormat );
			cellCursor.insertText( query->value(0).toString()+ "\n", textCharFormat_bold );
			QString abonPibStr = query->value(1).toString() + " " + query->value(2).toString() 
									+ " " + query->value(3).toString();
			if (abonPibStr.size() > 35)
				abonPibStr = abonPibStr.mid(0,34)+".";
			cellCursor.insertText( (abonPibStr).toUpper(), textCharFormat_bold );

				//Частина підприємства
			cell = table->cellAt(3, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Борг на початок місяця"), textCharFormat );
			cell = table->cellAt(3, 1);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(saldo), textCharFormat );
			
			cell = table->cellAt(4, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Перев. в дебіторську заборг."), textCharFormat );
			cell = table->cellAt(4, 1);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(narah), textCharFormat );
			
			cell = table->cellAt(5, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Оплата в попер.місяці"), textCharFormat );
			cell = table->cellAt(5, 1);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(oplacheno), textCharFormat );
			
			QTextCharFormat curTextCharFormat;
			curTextCharFormat = textCharFormat_bold;
			cell = table->cellAt(6, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Сума до оплати\n"), curTextCharFormat );
			// додає два переноси на наступний рядок у випадку друку без банківських реквізитів
			if (!needPrintBankRekvizyty)
				cellCursor.insertText( "\n\n", curTextCharFormat );
			cell = table->cellAt(6, 1);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			if (oplata > 0.009)
				cellCursor.insertText( uMToStr2(oplata), curTextCharFormat );
			
			cell = table->cellAt(8, 0);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Оплачено абонентом \n\n"), textCharFormat );
			
				//Частина абонента
			cell = table->cellAt(1, 4);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( "Борг на початок місяця", textCharFormat );

			cell = table->cellAt(1, 5);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(saldo)+"\n", textCharFormat );
			cell = table->cellAt(2, 4);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Переведено в дебіторську заборгованість"), textCharFormat );
			cell = table->cellAt(2, 5);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(narah), textCharFormat );
			
			cell = table->cellAt(3, 4);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Оплачено в поточному місяці"), textCharFormat );
			cell = table->cellAt(3, 5);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( uMToStr2(oplacheno), textCharFormat );
			
			curTextCharFormat = textCharFormat_bold;
			cell = table->cellAt(4, 4);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Сума до оплати"), curTextCharFormat );
			cell = table->cellAt(4, 5);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignRight );
			cellCursor.setBlockFormat( blockFormat );
			if (oplata > 0.009)
				cellCursor.insertText( uMToStr2(oplata), curTextCharFormat );
			
			cell = table->cellAt(6, 4);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Оплачено абонентом"), textCharFormat );
			
						//Інформація про абонента
			cell = table->cellAt(1, 3);
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			QTextCharFormat textCharFormatAI = textCharFormat;
			cellCursor = cell.firstCursorPosition();
			blockFormat.setAlignment( Qt::AlignLeft );
			cellCursor.setBlockFormat( blockFormat );
			cellCursor.insertText( codec->toUnicode("Адреса: вул.")+query->value(4).toString() + " "
										+query->value(5).toString()
										+", кв. "+query->value(6).toString()+"\n", textCharFormatAI );			
			cellCursor.deletePreviousChar(); //Видалення останнього переносу
			
						//Повідомлення для абонента
			cell = table->cellAt(7, 3);
			cellCursor = cell.firstCursorPosition();
			textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
			cell.setFormat(textCharFormat);
			blockFormat.setAlignment( Qt::AlignCenter );
			cellCursor.setBlockFormat( blockFormat );
			
					//Читання інформаційних стрічок з бази даних і запис їх до інформаційного блоку квитанції
			infoStrQuery.exec("SELECT value FROM slujb_znach WHERE id=4");
			infoStrQuery.seek(0);
			cellCursor.insertText( "Просимо оплатити до "+QString::number(infoStrQuery.value(0).toDouble(),'f',0)+" числа поточного місяця.\n", textCharFormat_bold );
			
			infoStrQuery.exec("SELECT strTemplate FROM slujb_znach WHERE id=2");
			infoStrQuery.seek(0);
			cellCursor.insertText( infoStrQuery.value(0).toString()+"\n", textCharFormat );
	
			infoStrQuery.exec("SELECT strTemplate FROM slujb_znach WHERE id=3");
			infoStrQuery.seek(0);
			cellCursor.insertText( infoStrQuery.value(0).toString(), textCharFormat_bold );
			
			infoStrQuery.exec("SELECT strTemplate FROM slujb_znach WHERE id=23");
			if ( infoStrQuery.next() && infoStrQuery.value(0).toString().trimmed().size() ) {
				cellCursor.insertText( "\n" + infoStrQuery.value(0).toString(), textCharFormat_bold );
			}
			
			num_kvyt++;
		  }
		  if (! one_kvyt)
			postup_form->inc_pos();
	}
	delete lichQuery;
	
	printform->printer()->setDocName("Квитанції по безнадійній дебіторській заборгованості за "+monthlist.at(prev_month-1)+" "+QString::number(prev_year)+"р.");
	QMessageBox::information(
				0,
				codec->toUnicode("!!! Закінчено створення квитанцій  !!!"),
				codec->toUnicode("Створено ")+ QVariant(num_kvyt).toString() +codec->toUnicode(" квитанцій."),
				QMessageBox::Ok,
				QMessageBox::NoButton);
	printform->show();
	delete query;
	if (! one_kvyt)
		delete postup_form;
}
//------------------------------------------------------------
