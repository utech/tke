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

    monthListRod 	<< "����"
                    << "������"
                    << "�������"
                    << "�����"
                    << "������"
                    << "������"
                    << "�����"
                    << "������"
                    << "�������"
                    << "������"
                    << "���������"
                    << "������";
}
//------------------------------------------------------------
void UPrintDocsOrganiz::print_vid_narah_za_gar_vodu(int cur_year, int cur_month)
{
	QSqlQuery *query = new QSqlQuery();
	//��������� ����� �������
	query->exec("SELECT count(*) FROM organizacii");
	query->seek(0);
	UPostupForm *postup_form = new UPostupForm(0, query->value(0).toInt() );
	postup_form->show();
	printform = new UPrintForm(0, codec->toUnicode("³������ ����������� ������ �� ������ ����."));
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat;
	//��������� ��������� 
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 13 );
	cursor.insertText(codec->toUnicode("²��̲���"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("����������� ������ �� ������ ����\n")+OrganizName(), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 5 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("�� ") + monthlist.at(cur_month-1) +
						codec->toUnicode(" ") + QVariant(cur_year).toString() + codec->toUnicode("�."), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 5 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	textCharFormat.setFontPointSize( 11 );
		//��������� � ������������ �������
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
	//���������� ����� �������
	cell = table->cellAt(0, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("� �\\�"));
	
	cell = table->cellAt(0, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("�����"));
	
	cell = table->cellAt(0, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("����� ����������"));
	
	cell = table->cellAt(0, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("���������� ����"));
	
	cell = table->cellAt(0, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("� �.�. ���"));
	
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
			//��������� ����������� ������ �� ������;
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

	//������� ��������
	table->mergeCells ( row, 0, 1, 3 );
	
	cell = table->cellAt(row, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("������ �� ����������"));

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
	
	//ϳ�����
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("     "+DocsPosadaPidpysText(5)+"					"+PrizvIB(5));

			//ʳ���� ��������� ��������� ��� �����

	printform->printer()->setDocName("³������ ����������� �� �� �� "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"�.");
	printform->show();
	delete query;
	delete organizaciiQuery;
	delete postup_form;
}
//------------------------------------------------------------
void UPrintDocsOrganiz::print_vidomist_narahuvannya_i_postuplennya_dohodiv(int cur_year, int cur_month)
{
	printform = new UPrintForm(0, codec->toUnicode("³������ ����������� � ����������� ������ �� ����������."));
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat;
	QSqlQuery *query = new QSqlQuery();
	//��������� ��������� 
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 13 );
	cursor.insertText(codec->toUnicode("²��̲���"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("����������� � ����������� ������ ��\n")+OrganizName(), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 5 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("�� ") + monthlist.at(cur_month-1) +
						codec->toUnicode(" ") + QVariant(cur_year).toString() + codec->toUnicode("�."), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 5 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	textCharFormat.setFontPointSize( 11 );
		//��������� � ������������ �������
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
	//���������� ����� �������
	cell = table->cellAt(0, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("� �\\�"));
	
	cell = table->cellAt(0, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("���'���� ��������������"));
	
	cell = table->cellAt(0, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("��.������"));
	
	cell = table->cellAt(0, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("���������� (����.)"));
	
	cell = table->cellAt(0, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("���������� (���.����)"));
	
	cell = table->cellAt(0, 5);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("������"));
	
	cell = table->cellAt(0, 6);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("����"));
	
	double vhSaldo, narahOpal, narahVoda, oplata;
	double vhSaldoZag=0, narahOpalZag=0, narahVodaZag=0, oplataZag=0;
//���������� ������ ϲ���Ȫ�����
	cell = table->cellAt(1, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("1."));
	
	cell = table->cellAt(1, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("ϳ���������"));
		//������ ������
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
		//����������� �� �������� � ������ ����
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
		//������
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
	
//���������� ������ ������Ͳ ����Ͳ��ֲ�
	cell = table->cellAt(2, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("2."));
	
	cell = table->cellAt(2, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("������� ����������"));
		//������ ������
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
		//����������� �� �������� � ������ ����
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
		//������
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
//���������� ������ ���������
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
	cellCursor.insertText(codec->toUnicode("���������"));
	
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
	
//���������� ������ � ���� ���˲ ��������� � ������
	cell = table->cellAt(4, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("  � �.�. ��������� � ������"));
	
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
	
//���������� ������ � ���� ���˲ �����Ĳ�
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
	cellCursor.insertText(codec->toUnicode("  � �.�. �������"));
	
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
	
//���������� ������ � ���� ���˲ ��������� ��� �����Ĳ�
	
	cell = table->cellAt(6, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("  � �.�. ��������� ��� �������"));
	
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
	
//���������� ������ ϲ����
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
	cellCursor.insertText(codec->toUnicode("ϳ����"));
	
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
	
//���������� ������ ����ֲ�
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
	cellCursor.insertText(codec->toUnicode("�������"));
	
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

//���������� ������ ������ �� �.1-�.5	
	
	cell = table->cellAt(9, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("������ �� �.1-�.5"));
	
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

			//ʳ���� ��������� ��������� ��� �����
	printform->printer()->setDocName("³������ ����������� � ����������� ������ �� "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"�.");
	printform->show();
	delete query;
}
//------------------------------------------------------------
void UPrintDocsOrganiz::print_nakopychuvalna_vidomist_narahuvannya_i_postuplennya_dohodiv(int cur_year, int cur_month)
{
	printform = new UPrintForm(0, codec->toUnicode("³������ ����������� � ����������� ������ �� ����������."));
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat;
	QSqlQuery *query = new QSqlQuery();
	//��������� ��������� 
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 13 );
	cursor.insertText(codec->toUnicode("�������������� ²��̲���"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("����������� � ����������� ������ ��\n")+OrganizName(), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 5 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("�� ����� � ���� �� ") + monthlist.at(cur_month-1) +
						codec->toUnicode(" ") + QVariant(cur_year).toString() + codec->toUnicode("�."), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 5 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	textCharFormat.setFontPointSize( 11 );
		//��������� � ������������ �������
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
	//���������� ����� �������
	cell = table->cellAt(0, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("� �\\�"));
	
	cell = table->cellAt(0, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("���'���� ��������������"));
	
	cell = table->cellAt(0, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("��.������"));
	
	cell = table->cellAt(0, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("���������� (����.)"));
	
	cell = table->cellAt(0, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("���������� (���.����)"));
	
	cell = table->cellAt(0, 5);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("������"));
	
	cell = table->cellAt(0, 6);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("����"));
	
	double vhSaldo, narahOpal, narahVoda, oplata;
	double vhSaldoZag=0, narahOpalZag=0, narahVodaZag=0, oplataZag=0;
//���������� ������ ϲ���Ȫ�����
	cell = table->cellAt(1, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("1."));
	
	cell = table->cellAt(1, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("ϳ���������"));
		//������ ������
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
		//����������� �� �������� � ������ ����
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
		//������
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
	
//���������� ������ ������Ͳ ����Ͳ��ֲ�
	cell = table->cellAt(2, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("2."));
	
	cell = table->cellAt(2, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("������� ����������"));
		//������ ������
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
		//����������� �� �������� � ������ ����
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
		//������
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
//���������� ������ ���������
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
	cellCursor.insertText(codec->toUnicode("���������"));
	
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
	
	//���������� ������ � ���� ���˲ ��������� � ������
	cell = table->cellAt(4, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("  � �.�. ��������� � ������"));
	
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
	
//���������� ������ � ���� ���˲ �����Ĳ�
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
	cellCursor.insertText(codec->toUnicode("  � �.�. �������"));
	
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
	
//���������� ������ � ���� ���˲ ��������� ��� �����Ĳ�
	
	cell = table->cellAt(6, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("  � �.�. ��������� ��� �������"));
	
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
	
//���������� ������ ϲ����
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
	cellCursor.insertText(codec->toUnicode("ϳ����"));
	
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
	
//���������� ������ ����ֲ�
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
	cellCursor.insertText(codec->toUnicode("�������"));
	
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

//���������� ������ ������ �� �.1-�.5	
	
	cell = table->cellAt(9, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("������ �� �.1-�.5"));
	
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

			//ʳ���� ��������� ��������� ��� �����
	printform->printer()->setDocName("�������������� ������� �� "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"�.");
	printform->show();
	delete query;
}
//------------------------------------------------------------
void UPrintDocsOrganiz::print_vidomist_oplaty_za_CO(int cur_year, int cur_month)
{
	QSqlQuery *query = new QSqlQuery();
	//��������� ����� �������
	query->exec("SELECT count(*) FROM organizacii");
	query->seek(0);
	UPostupForm *postup_form = new UPostupForm(0, query->value(0).toInt() );
	postup_form->show();
	printform = new UPrintForm(0, codec->toUnicode("³������ ����������� ������ �� ��."));
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat;
	//��������� ��������� 
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 13 );
	cursor.insertText(codec->toUnicode("²��̲���"), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("����������� ������ �� ���������� ��������\n")+OrganizName(), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 5 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("�� ") + monthlist.at(cur_month-1) +
						codec->toUnicode(" ") + QVariant(cur_year).toString() + codec->toUnicode("�."), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 5 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	textCharFormat.setFontPointSize( 11 );
		//��������� � ������������ �������
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
	//���������� ����� �������
	cell = table->cellAt(0, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("� �\\�"));
	
	cell = table->cellAt(0, 1);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("�����"));
	
	cell = table->cellAt(0, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("����� ����������"));
	
	cell = table->cellAt(0, 3);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("���������� ����"));
	
	cell = table->cellAt(0, 4);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("� �.�. ���"));
	
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
			//��������� ����������� ������ �� ������;
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
	
	//�������� � �������� �������
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
	cellCursor.insertText(codec->toUnicode("��."));
	
	cell = table->cellAt(row, 2);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("�� ������ �������"));
	
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
	
	//������� ��������
	table->mergeCells ( row, 0, 1, 3 );
	
	cell = table->cellAt(row, 0);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("������ �� ����������"));

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
	
	//ϳ�����
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("     "+DocsPosadaPidpysText(5)+"					"+PrizvIB(5));

			//ʳ���� ��������� ��������� ��� �����

	printform->printer()->setDocName("³������ ������ �� �� �� "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"�.");
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
	//��������� ����� �������
	query->exec("SELECT count(*) FROM organizacii");
	query->seek(0);
	int orgCount = query->value(0).toInt();
	query->exec("SELECT count(*) FROM slujb_rahunky");
	query->seek(0);
	orgCount += query->value(0).toInt();
	UPostupForm *postup_form = new UPostupForm(0, orgCount );
	postup_form->show();
	printform = new UPrintForm(0, codec->toUnicode("³������ ���������"));
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat, textCharFormat_bold;
	textCharFormat_bold.setFontPointSize( 9 );
	font = textCharFormat_bold.font();
	font.setBold( true );
	textCharFormat_bold.setFont(font);
	//��������� ��������� 
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 13 );
	textCharFormat_bold.setFontPointSize( 13 );
	cursor.insertText(codec->toUnicode("²��̲��� ���˲��ֲ�"), textCharFormat_bold);
	textCharFormat_bold.setFontPointSize( 9 );
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("�� ")+OrganizName(), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 5 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	blockFormat.setAlignment( Qt::AlignRight );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 11 );
	cursor.insertText(codec->toUnicode("�� ") + monthlist.at(cur_month-1) +
						codec->toUnicode(" ") + QVariant(cur_year).toString() + codec->toUnicode("�."), textCharFormat);
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	textCharFormat.setFontPointSize( 5 );
	cursor.insertText(codec->toUnicode(" "), textCharFormat);
	
		//��������� � ������������ �������
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
	//���������� ����� �������
	cell = table->cellAt(0, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("� �\\�"),textCharFormat);
	
	cell = table->cellAt(0, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("�����"),textCharFormat);
	
	cell = table->cellAt(0, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("����� ����������"),textCharFormat);
	
	cell = table->cellAt(0, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("ϳ������"),textCharFormat);
	
	cell = table->cellAt(0, 4);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("��.������"),textCharFormat);
	
	cell = table->cellAt(0, 5);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("����������"),textCharFormat);
	
	cell = table->cellAt(0, 6);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("��������"),textCharFormat);
	
	cell = table->cellAt(0, 7);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("���.������"),textCharFormat);
	
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
			//���������� ���������� ��� ����������
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
				
				//���������� ������� ��������
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
	
	//������ ��� �� ����������� � �����������
	table->insertRows( row, 1);
	table->mergeCells( row, 0, 1, 4 );
	
	cell = table->cellAt(row, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("������ �� ����������� � �����������"),textCharFormat_bold);
	
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
	
	//����� ����������� �������
	textCharFormat_bold.setFontItalic( true );
	table->insertRows( row, 1);
	table->mergeCells( row, 0, 1, 8 );
	cell = table->cellAt(row, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("��������� �������"), textCharFormat_bold);
	textCharFormat_bold.setFontItalic( false );
	row++;
	
	double zagSlujbVhSaldo=0;
	double zagSlujbNarah=0;
	double zagSlujbOplata=0;
	double zagSlujbVyhSaldo=0;
	//���������
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
	cellCursor.insertText(codec->toUnicode("��."),textCharFormat);
	
	cell = table->cellAt(row, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("���������"),textCharFormat);
	
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
	
	//� �.�.�������
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
	cellCursor.insertText(codec->toUnicode("��."),textCharFormat);
	
	cell = table->cellAt(row, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("� �.�. �����䳿"),textCharFormat);
	
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
	
	//������� ϲ���
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
	
	
		//������ ��� ������������
	table->insertRows( row, 1);
	table->mergeCells( row, 0, 1, 4 );
	
	cell = table->cellAt(row, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("������ �� ������������"),textCharFormat_bold);
	
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
	
		//������ ��� �� �������
	//table->insertRows( row, 1);
	table->mergeCells( row, 0, 1, 4 );
	
	cell = table->cellAt(row, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("������ �� �������"),textCharFormat_bold);
	
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
	
			//ϳ�����
	textCharFormat.setFontPointSize( 13 );
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("     "+DocsPosadaPidpysText(5)+"					"+PrizvIB(5));
	
	printform->printer()->setDocName("³������ ��������� �� "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"�.");
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
	//��������� ����� �������
	printform = new UPrintForm(0, codec->toUnicode("�������"));
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat, textCharFormat_bold, textCharFormat_italic, textCharFormat_small;
	textCharFormat_bold.setFontPointSize( 9 );
	font = textCharFormat_bold.font();
	font.setBold( true );
	textCharFormat_bold.setFont(font);
	textCharFormat_italic.setFontPointSize( 9 );
	textCharFormat_italic.setFontItalic(true);
	//��������� ��������� 
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 10 );
	textCharFormat_bold.setFontPointSize( 10 );
	textCharFormat_small.setFontPointSize( 1 );
	
	//���������� �� ���������� ����������� ������� �����
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
	
	//������� �������������
	cursor.insertText(codec->toUnicode(" "), textCharFormat_small);
	cursor.insertHtml("<hr>");
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("	������������: "), textCharFormat_italic);
	cursor.insertText(OrganizName(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		��.������: "), textCharFormat_italic);
	cursor.insertText(OrganizAdresa(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		���. "), textCharFormat_italic);
	cursor.insertText(OrganizTel(), textCharFormat);
	cursor.insertText(codec->toUnicode("   �-�����: "), textCharFormat_italic);
	cursor.insertText(OrganizEMail(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		����: "), textCharFormat_italic);
	cursor.insertText(OrganizBank(), textCharFormat);
	cursor.insertText(codec->toUnicode(", �/�:"), textCharFormat_italic);
	cursor.insertText(OrganizRozrahRahunok(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		������: "), textCharFormat_italic);
	cursor.insertText(OrganizEDRPOU(), textCharFormat);
	
	//������ �����
	cursor.insertHtml("<hr>");
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	if (isAvans==1)
		cursor.insertText(codec->toUnicode("��������� "), textCharFormat_bold);
	cursor.insertText(codec->toUnicode("������� �_______ �� \"___\"______________20___�."), textCharFormat_bold);
	
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	
	//������� ���������
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
	cursor.insertText(codec->toUnicode("	�������: "), textCharFormat_italic);
	cursor.insertText(query->value(0).toString(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		�����: � "), textCharFormat_italic);
	cursor.insertText(QVariant(ugodaNum).toString(), textCharFormat);
	cursor.insertText(codec->toUnicode(" �� "), textCharFormat_italic);
	cursor.insertText(query->value(1).toDate().toString("d.MM.yyyy")+codec->toUnicode("�."), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		����: "), textCharFormat_italic);
	cursor.insertText(query->value(2).toString(), textCharFormat);
	cursor.insertText(codec->toUnicode(", ��� "), textCharFormat_italic);
	cursor.insertText(query->value(3).toString(), textCharFormat);
	cursor.insertText(codec->toUnicode(", �/� "), textCharFormat_italic);
	cursor.insertText(query->value(4).toString(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		������: "), textCharFormat_italic);
	cursor.insertText(query->value(5).toString(), textCharFormat);
	cursor.insertHtml("<hr>");
	
	double pdv = OrganizPdv();
	
	//������� ������
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
		cursor.insertText(codec->toUnicode("	�� ")+monthlist.at(cur_month-1)+" "
							+QVariant(cur_year).toString()+codec->toUnicode(" ����"), textCharFormat);
		QTextTable *table = cursor.insertTable(7, 4, tableFormat);
		
		table->mergeCells( 1, 0, 1, 3 );
		table->mergeCells( 3, 0, 1, 3 );
		table->mergeCells( 4, 0, 1, 3 );
		table->mergeCells( 5, 0, 1, 3 );
		table->mergeCells( 6, 0, 1, 3 );
		
		//���������� ����� �������
				//�������������
		cell = table->cellAt(0, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("�����"), textCharFormat_italic);
	
		cell = table->cellAt(0, 1);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("�-��� ����"), textCharFormat_italic);
	
		cell = table->cellAt(0, 2);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("����.���.����"), textCharFormat_italic);
	
		cell = table->cellAt(0, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("����"), textCharFormat_italic);
				//�����������
		cell = table->cellAt(1, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("���� �� ������� �����"), textCharFormat);
	
		cell = table->cellAt(2, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("����������:\n     � �.�. �� ���������� ��������"), textCharFormat);
		if (isGV)
			cellCursor.insertText(codec->toUnicode("\n     � �.�. �� ������ ����"), textCharFormat);
		
		cell = table->cellAt(3, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("	�.�.�.���"), textCharFormat);
	
		cell = table->cellAt(4, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("�������� �� �����"), textCharFormat);
		
		cell = table->cellAt(6, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("	�.�.�.���"), textCharFormat);

		UNasTaryfInfo taryfNas = nasTaryf(cur_year, cur_month);
		UOrgTaryfInfo orgTaryf(cur_year, cur_month);
		double gKalOpal=orgTaryf.vart_g_kal(financeTypeForOrganiz(ugodaNum));
		double gKalVoda=orgTaryf.vart_g_kal_gar_vody;
		
		if (taryfNas.vart_g_kal == 0) {
			QMessageBox::critical(0,"������� ������","�������� ����� �� �� ��� ���������");
			taryfNas.vart_g_kal = 0.1;
		}
		if (gKalOpal == 0) {
			QMessageBox::critical(0,"������� ������","�������� ����� �� �� ��� ����������");
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
		
		//������ ������
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
		
		//�����������
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
		
		//ʳ������ �����
		cell = table->cellAt(2, 1);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText('\n'+uMToStr2(narahOpal/(isNasel ? taryfNas.vart_g_kal : gKalOpal)), textCharFormat);
		if (isGV)
			cellCursor.insertText('\n'+uMToStr2(narahVoda/gKalVoda), textCharFormat);
		//qDebug() << "narahOpal: " << narahOpal<< " gKalOpal: " << gKalOpal<< " narahVoda: " << narahVoda<< " gKalVoda: " << gKalVoda;
		
		
		//������ �� �����
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
		
		//���
		cell = table->cellAt(3, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2((narahOpal+narahVoda)*pdv/(100+pdv)), textCharFormat);
		
		avansSum = vhSaldo + narahOpal + narahVoda - oplata;
		
		//�� ������
		cell = table->cellAt(5, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		QString sumaDoOplaty(uMToStr2(avansSum));
		cellCursor.insertText(codec->toUnicode("�� ������:\n(")+numberToString(sumaDoOplaty)+')', textCharFormat_bold);
	
		cell = table->cellAt(5, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(avansSum), textCharFormat_bold);
	
		//��� ������
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
		
		//�� ������
		cell = table->cellAt(0, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		QString sumaDoOplaty(uMToStr2(avansSum));
		cellCursor.insertText(codec->toUnicode("�� ������:\n(")+numberToString(sumaDoOplaty)+')', textCharFormat_bold);
	
		cell = table->cellAt(0, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(avansSum), textCharFormat_bold);
	
		//��� ������
		cell = table->cellAt(1, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("	�.�.�.���"), textCharFormat);
		
		cell = table->cellAt(1, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(avansSum*pdv/(100+pdv)), textCharFormat);
	}
	
	
	//ϳ�����
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
	
	printform->printer()->setDocName("������� �� "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"�.");
	printform->show();
	delete query;
}
//------------------------------------------------------------
void UPrintDocsOrganiz::print_rahunok_2(int cur_year, int cur_month, int ugodaNum, int isAvans, double avansSum)
{
	QSqlQuery *query = new QSqlQuery();
	QFont font;
	//��������� ����� �������
	printform = new UPrintForm(0, codec->toUnicode("�������"));
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat, textCharFormat_bold, textCharFormat_italic, textCharFormat_small;
	textCharFormat_bold.setFontPointSize( 9 );
	font = textCharFormat_bold.font();
	font.setBold( true );
	textCharFormat_bold.setFont(font);
	textCharFormat_italic.setFontPointSize( 9 );
	textCharFormat_italic.setFontItalic(true);
	//��������� ��������� 
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize( 10 );
	textCharFormat_bold.setFontPointSize( 10 );
	textCharFormat_small.setFontPointSize( 1 );
	
	//���������� �� ���������� ����������� ������� �����
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
	
	//������� �������������
	cursor.insertText(codec->toUnicode(" "), textCharFormat_small);
	cursor.insertHtml("<hr>");
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("	������������: "), textCharFormat_italic);
	cursor.insertText(OrganizName(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		��.������: "), textCharFormat_italic);
	cursor.insertText(OrganizAdresa(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		���. "), textCharFormat_italic);
	cursor.insertText(OrganizTel(), textCharFormat);
	cursor.insertText(codec->toUnicode("   �-�����: "), textCharFormat_italic);
	cursor.insertText(OrganizEMail(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		����: "), textCharFormat_italic);
	cursor.insertText(OrganizBank(), textCharFormat);
	cursor.insertText(codec->toUnicode(", �/�:"), textCharFormat_italic);
	cursor.insertText(OrganizRozrahRahunok(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		������: "), textCharFormat_italic);
	cursor.insertText(OrganizEDRPOU(), textCharFormat);
	
	//������ �����
	cursor.insertHtml("<hr>");
	blockFormat.setAlignment( Qt::AlignHCenter );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	if (isAvans==1)
		cursor.insertText(codec->toUnicode("��������� "), textCharFormat_bold);
	cursor.insertText(codec->toUnicode("������� �_______ �� \"___\"______________20___�."), textCharFormat_bold);
	
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	
	//������� ���������
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
	cursor.insertText(codec->toUnicode("	�������: "), textCharFormat_italic);
	cursor.insertText(query->value(0).toString(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		�����: � "), textCharFormat_italic);
	cursor.insertText(QVariant(ugodaNum).toString(), textCharFormat);
	cursor.insertText(codec->toUnicode(" �� "), textCharFormat_italic);
	cursor.insertText(query->value(1).toDate().toString("d.MM.yyyy")+codec->toUnicode("�."), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		����: "), textCharFormat_italic);
	cursor.insertText(query->value(2).toString(), textCharFormat);
	cursor.insertText(codec->toUnicode(", ��� "), textCharFormat_italic);
	cursor.insertText(query->value(3).toString(), textCharFormat);
	cursor.insertText(codec->toUnicode(", �/� "), textCharFormat_italic);
	cursor.insertText(query->value(4).toString(), textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode("		������: "), textCharFormat_italic);
	cursor.insertText(query->value(5).toString(), textCharFormat);
	cursor.insertHtml("<hr>");
	
	query->exec("SELECT value FROM slujb_znach WHERE id=1");
	query->seek(0);
	double pdv = query->value(0).toDouble();
	
	//������� ������
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
		cursor.insertText(codec->toUnicode("	�� ")+monthlist.at(cur_month-1)+" "
							+QVariant(cur_year).toString()+codec->toUnicode(" ����"), textCharFormat_bold);
		
		cursor.insertBlock(blockFormat);
		
		UOrgTaryfInfo orgTaryf(cur_year, cur_month);
		cursor.insertText("	�����:	�� 1 ����. ��: "+uMToStr2(orgTaryf.vart_g_kal(financeTypeForOrganiz(ugodaNum))), textCharFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText("		�� 1 �.��.  ��: "+uMToStr2(orgTaryf.vart_kvadr_CO), textCharFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText("		�� 1 ����.  ��: "+uMToStr2(orgTaryf.vart_g_kal_gar_vody), textCharFormat);
		cursor.insertBlock(blockFormat);
		cursor.insertText("		�� 1 �.���. ��: "+uMToStr2(orgTaryf.vart_kuba_GV), textCharFormat);
		
		QTextTable *table = cursor.insertTable(7, 4, tableFormat);
		
		table->mergeCells( 0, 0, 1, 3 );
		table->mergeCells( 1, 0, 1, 3 );
		table->mergeCells( 2, 0, 1, 3 );
		table->mergeCells( 3, 0, 1, 3 );
		table->mergeCells( 4, 0, 1, 3 );
		table->mergeCells( 5, 0, 1, 3 );
		table->mergeCells( 6, 0, 1, 3 );
		
		//���������� ����� �������
				//�������������
		cell = table->cellAt(0, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("�����"), textCharFormat_italic);
	
		
		//cell = table->cellAt(0, 1);
		//cellCursor = cell.firstCursorPosition();
		//blockFormat.setAlignment( Qt::AlignCenter );
		//cellCursor.setBlockFormat( blockFormat );
		//cellCursor.insertText(codec->toUnicode("�-��� ����"), textCharFormat_italic);
	
		//cell = table->cellAt(0, 2);
		//cellCursor = cell.firstCursorPosition();
		//blockFormat.setAlignment( Qt::AlignCenter );
		//cellCursor.setBlockFormat( blockFormat );
		//cellCursor.insertText(codec->toUnicode("����.���.����"), textCharFormat_italic);
	
		cell = table->cellAt(0, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("����"), textCharFormat_italic);
				//�����������
		cell = table->cellAt(1, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("���� �� ������� �����"), textCharFormat);
	
		cell = table->cellAt(2, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("����������:\n     � �.�. �� ���������� ��������"), textCharFormat);
		if (isGV)
			cellCursor.insertText(codec->toUnicode("\n     � �.�. �� ������ ����"), textCharFormat);
		
		cell = table->cellAt(3, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("	�.�.�.���"), textCharFormat);
	
		cell = table->cellAt(4, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("�������� �� �����"), textCharFormat);
		
		cell = table->cellAt(6, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("	�.�.�.���"), textCharFormat);

		//������ ������
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
		
		//�����������
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
		
		//ʳ������ �����
		//cell = table->cellAt(2, 1);
		//cellCursor = cell.firstCursorPosition();
		//blockFormat.setAlignment( Qt::AlignCenter );
		//cellCursor.setBlockFormat( blockFormat );
		//cellCursor.insertText('\n'+uMToStr2(narahOpal/gKalOpal), textCharFormat);
		//if (isGV)
		//	cellCursor.insertText('\n'+uMToStr2(narahVoda/gKalVoda), textCharFormat);
		//qDebug() << "narahOpal: " << narahOpal<< " gKalOpal: " << gKalOpal<< " narahVoda: " << narahVoda<< " gKalVoda: " << gKalVoda;
		
		
		//������ �� �����
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
		
		//���
		cell = table->cellAt(3, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2((narahOpal+narahVoda)*pdv/(100+pdv)), textCharFormat);
		
		avansSum = vhSaldo + narahOpal + narahVoda - oplata;
		
		//�� ������
		cell = table->cellAt(5, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		QString sumaDoOplaty(uMToStr2(avansSum));
		cellCursor.insertText(codec->toUnicode("�� ������:\n(")+numberToString(sumaDoOplaty)+')', textCharFormat_bold);
	
		cell = table->cellAt(5, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(avansSum), textCharFormat_bold);
	
		//��� ������
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
		
		//�� ������
		cell = table->cellAt(0, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		QString sumaDoOplaty(uMToStr2(avansSum));
		cellCursor.insertText(codec->toUnicode("�� ������:\n(")+numberToString(sumaDoOplaty)+')', textCharFormat_bold);
	
		cell = table->cellAt(0, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(avansSum), textCharFormat_bold);
	
		//��� ������
		cell = table->cellAt(1, 0);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("	�.�.�.���"), textCharFormat);
		
		cell = table->cellAt(1, 3);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(uMToStr2(avansSum*pdv/(100+pdv)), textCharFormat);
	}
	
	
	//ϳ�����
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
	
	printform->printer()->setDocName("������� �� "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"�.");
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
	//��������� ����� �������
	printform = new UPrintForm(0, codec->toUnicode("³������ ��������"));
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat, textCharFormat_bold, textCharFormat_italic, textCharFormat_small;
	textCharFormat_bold.setFontPointSize( 9 );
	font = textCharFormat_bold.font();
	font.setBold( true );
	textCharFormat_bold.setFont(font);
	textCharFormat_italic.setFontPointSize( 10 );
	textCharFormat_italic.setFontItalic(true);
	//��������� ��������� 
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
	cursor.insertText(codec->toUnicode("²��̲���\n����Ͳ��ֲ� ������ʲ� (")+query->value(0).toString()+")", textCharFormat_bold);
	textCharFormat_bold.setFontPointSize( 10 );
	cursor.insertBlock(blockFormat);
	cursor.insertText(codec->toUnicode(" "), textCharFormat_small);
	cursor.insertBlock(blockFormat);
	if ((pochDate.year()==kincDate.year()) && (pochDate.month()==kincDate.month())){
		cursor.insertText(codec->toUnicode("������ �� ")+monthlist.at(pochDate.month()-1)+" " + QVariant(pochDate.year()).toString()
							+codec->toUnicode("�."), textCharFormat);
	}
	else{
		cursor.insertText(codec->toUnicode("�� ����� � ")+pochDate.toString("MM.yyyy") +
							codec->toUnicode("�. �� ")+kincDate.toString("MM.yyyy")+codec->toUnicode("�."), textCharFormat);
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
	
	//���������� ����� �������
	textCharFormat_italic.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	
	cell = table->cellAt(0, 0);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("� �/�"), textCharFormat_italic);
	
	cell = table->cellAt(0, 1);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("�����"), textCharFormat_italic);
	
	cell = table->cellAt(0, 2);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText(codec->toUnicode("����� ����������"), textCharFormat_italic);
	
	cell = table->cellAt(0, 3);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	if ((pochDate.year()==kincDate.year()) && (pochDate.month()==kincDate.month()))
		cellCursor.insertText(codec->toUnicode("���� �� ������� �����"), textCharFormat_italic);
	else
		cellCursor.insertText(codec->toUnicode("���� ������ �� 1.")+pochDate.toString("MM.yyyy")+codec->toUnicode("�."), textCharFormat_italic);
	
	cell = table->cellAt(0, 4);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	if ((pochDate.year()==kincDate.year()) && (pochDate.month()==kincDate.month()))
		cellCursor.insertText(codec->toUnicode("���������� �� �����"), textCharFormat_italic);
	else
		cellCursor.insertText(codec->toUnicode("���������� � ������� ������"), textCharFormat_italic);
	
	cell = table->cellAt(0, 5);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	if ((pochDate.year()==kincDate.year()) && (pochDate.month()==kincDate.month()))
		cellCursor.insertText(codec->toUnicode("�������� �� �����"), textCharFormat_italic);
	else
		cellCursor.insertText(codec->toUnicode("�������� �� �����"), textCharFormat_italic);
	
	cell = table->cellAt(0, 6);
	cell.setFormat(textCharFormat_italic);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	if ((pochDate.year()==kincDate.year()) && (pochDate.month()==kincDate.month()))
		cellCursor.insertText(codec->toUnicode("���� �� ����� �����"), textCharFormat_italic);
	else
		cellCursor.insertText(codec->toUnicode("���� ������ �� ")+kincDate.toString("dd.MM.yyyy")+codec->toUnicode("�."), textCharFormat_italic);
	
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
			
			if ((finansFromId==1) && (organizQuery->value(11).toInt()==1)){//�����
				sumOsvitaPochVhSaldo+=pochVhSaldo;
				sumOsvitaNarah+=narah;
				sumOsvitaOplata+=oplata;
				sumOsvitaKincVyhSaldo+=kincVyhSaldo;
			}
			else if ((finansFromId==1) && (organizQuery->value(11).toInt()==2)){//������� ������'�
				sumOhorZdorPochVhSaldo+=pochVhSaldo;
				sumOhorZdorNarah+=narah;
				sumOhorZdorOplata+=oplata;
				sumOhorZdorKincVyhSaldo+=kincVyhSaldo;
			}
			
			iter++;
		}
		postup_form->inc_pos();
	}
	//������ �� �����
	table->mergeCells( iter, 0, 1, 3 );
	textCharFormat_bold.setFontPointSize( 10 );
	
	cell = table->cellAt(iter, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignCenter );
	cellCursor.setBlockFormat( blockFormat );
	if ((pochDate.year()==kincDate.year()) && (pochDate.month()==kincDate.month()))
		cellCursor.insertText(codec->toUnicode("������ �� �����"),textCharFormat_bold);
	else
		cellCursor.insertText(codec->toUnicode("������ �� �����"),textCharFormat_bold);
	
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
		//� �.�. �����
		table->mergeCells( iter, 0, 1, 3 );
		textCharFormat_bold.setFontPointSize( 10 );
	
		cell = table->cellAt(iter, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("    � �.�. ��²��"),textCharFormat_bold);
	
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
	
		//� �.�. ������� ������'�
		table->mergeCells( iter, 0, 1, 3 );
		textCharFormat_bold.setFontPointSize( 10 );

		cell = table->cellAt(iter, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(codec->toUnicode("    � �.�. ������� ������'�"),textCharFormat_bold);

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
	
	//ϳ�����
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("		"+DocsPosadaPidpysText(5)+"				"+PrizvIB(5), textCharFormat);
	
	printform->printer()->setDocName("³������ ��������");
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
