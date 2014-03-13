/****************************************************************************
**
** Created: Roman Mulyk 02.02.2009
**
****************************************************************************/

#include "uprint_docs_organiz.h"
#include <UPopulateTextTableCells>
#include <USqlAccessible>

#include "UReplaceStringsParser.h"

//------------------------------------------------------------
void UPrintDocsOrganiz::print_podatkova_nakladna(int cur_year, int cur_month, int ugodaNum, double sumCO, double sumGV)
{
	QSqlQuery *query = new QSqlQuery();
	QFont font;
	
	QTextTable *tableFrame;
	QTextTable *table;
	QTextTable *tableIndPodNum;
	
	//��������� ����� �������
	printform = new UPrintForm(0, "��������� ��������");
	QTextBlockFormat blockFormat;
	QTextTableCell cell;
	QTextCharFormat textCharFormat, textCharFormat_bold, textCharFormat_italic, textCharFormat_small, textCharFormat_header;
	QTextCharFormat textCharFormat_underline;
	textCharFormat_bold.setFontPointSize(9);
	font = textCharFormat_bold.font();
	font.setBold( true );
	textCharFormat_bold.setFont(font);
	textCharFormat_italic.setFontPointSize(9);
	textCharFormat_italic.setFontItalic(true);
	//��������� ��������� 
	printform->document()->clear();
	QTextCursor cursor(printform->document()), cellCursor;
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.setBlockFormat( blockFormat );
	textCharFormat.setFontPointSize(9);
	textCharFormat_bold.setFontPointSize(9);
	textCharFormat_small.setFontPointSize( 8 );
	textCharFormat_header.setFontPointSize(11);
	font = textCharFormat_header.font();
	font.setBold( true );
	textCharFormat_header.setFont(font);
	textCharFormat_underline.setFontPointSize(9);
	textCharFormat_underline.setFontUnderline(true);
	
	//�������� ����� ������� - ����
	QTextTableFormat tableFormat;
	QVector<QTextLength> constraints;
	constraints << QTextLength(QTextLength::FixedLength, 250);
	constraints << QTextLength(QTextLength::FixedLength, 450);
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setAlignment(Qt::AlignHCenter);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(2);
	tableFormat.setBorder(0);
	tableFrame = cursor.insertTable(1, 2, tableFormat);
	
		//������ �������, ����
		cell = tableFrame->cellAt(0, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		constraints.clear();
		constraints << QTextLength(QTextLength::FixedLength, 250);
		constraints << QTextLength(QTextLength::FixedLength, 30);
		tableFormat.setBorderBrush(QBrush(Qt::black));
		tableFormat.setColumnWidthConstraints(constraints);
		tableFormat.setAlignment(Qt::AlignHCenter);
		tableFormat.setCellSpacing(0);
		tableFormat.setCellPadding(2);
		tableFormat.setBorder(0.8);
		table = cursor.insertTable(2, 2, tableFormat);
		
		cell = table->cellAt(0, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("���ò��� ",textCharFormat_bold);
		cellCursor.insertText("(�������� �������)",textCharFormat);
		
		cell = table->cellAt(0, 1);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("",textCharFormat_bold);
		
		cell = table->cellAt(1, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignLeft );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("��ϲ� ",textCharFormat_bold);
		cellCursor.insertText("(���������� � ��������)",textCharFormat);
		
		cell = table->cellAt(1, 1);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText("",textCharFormat_bold);
		
		cell = tableFrame->cellAt(0, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.lastCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.insertBlock( blockFormat );
		cellCursor.insertText("(��������� ������� ������� �ջ)", textCharFormat);
		
	cell = tableFrame->cellAt(0, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignRight );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("����������� ������� ��� ������ �� 30.05.97 �. No165 \n "
							"(� �������� ������ ��� ������ �� 08.10.98 �. No469) � ������,"
							"��������� ����� � ������� ���� �� 30.06.2005 �. No244",textCharFormat);
	
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignCenter );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("��������� ��������", textCharFormat_header);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	
	QSqlQuery prodavecQuery, pokupecQuery;
	prodavecQuery.exec("SELECT this_organiz_name, this_organiz_adresa, this_organiz_tel, this_organiz_num_svid_pdv, \
								this_organiz_ind_pod_nomer \
						FROM this_organiz WHERE id="+sqlStr(organizID()));
	prodavecQuery.seek(0);
	
	pokupecQuery.exec("SELECT Nazva, Adresa, Tel1, num_svid_pdv, Date_ugody, ind_pod_nomer \
						FROM organizacii WHERE Ugoda="+sqlStr(ugodaNum));
	pokupecQuery.seek(0);
	
	constraints.clear();
	constraints << QTextLength(QTextLength::FixedLength, 100);
	constraints << QTextLength(QTextLength::FixedLength, 250);
	constraints << QTextLength(QTextLength::FixedLength, 100);
	constraints << QTextLength(QTextLength::FixedLength, 250);
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setAlignment(Qt::AlignHCenter);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(2);
	tableFormat.setBorder(0);
	table = cursor.insertTable(9, 4, tableFormat);
	
	UPopulateCell *pCell = new UPopulateCell(table);
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignTop);
	pCell->setAlignment( Qt::AlignLeft );
	pCell->setTextCharFormat(textCharFormat);
	pCell->setTextBlockFormat(blockFormat);
	
	table->mergeCells ( 0, 0, 1, 2 );
	table->mergeCells ( 0, 2, 1, 2 );
	
	table->mergeCells ( 2, 0, 2, 1 );
	table->mergeCells ( 2, 2, 2, 1 );
	
	table->mergeCells ( 4, 0, 1, 2 );
	table->mergeCells ( 4, 2, 1, 2 );
	table->mergeCells ( 5, 0, 1, 2 );
	table->mergeCells ( 5, 2, 1, 2 );
	table->mergeCells ( 6, 0, 1, 2 );
	table->mergeCells ( 6, 2, 1, 2 );
	
	table->mergeCells ( 7, 0, 1, 4 );
	table->mergeCells ( 8, 0, 1, 4 );
	
	cell = table->cellAt(0, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("���� ������� ��������� �������� "+date.toString("dd.MM.yyyy"),textCharFormat);
	
	cell = table->cellAt(0, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("���������� ����� __________________",textCharFormat);
	
	cell = table->cellAt(1, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("���������",textCharFormat);
	
	cell = table->cellAt(1, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("��������",textCharFormat);
	
	cell = table->cellAt(2, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("����� (������� �������)-���������",textCharFormat);
	
	pCell->setWithFrame(2, 1, prodavecQuery.value(0).toString(), textCharFormat_bold);
	
	cell = table->cellAt(2, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("����� (������� �������)-��������",textCharFormat);
	
	pCell->setWithFrame(2, 3, pokupecQuery.value(0).toString(), textCharFormat_bold);
	
	cell = table->cellAt(3, 1);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("(�����)",textCharFormat);
		//������� �������������� ����������� ������ ��������
		constraints.clear();
		constraints << QTextLength(QTextLength::FixedLength, 17);
		constraints << QTextLength(QTextLength::FixedLength, 17);
		constraints << QTextLength(QTextLength::FixedLength, 17);
		constraints << QTextLength(QTextLength::FixedLength, 17);
		constraints << QTextLength(QTextLength::FixedLength, 17);
		constraints << QTextLength(QTextLength::FixedLength, 17);
		constraints << QTextLength(QTextLength::FixedLength, 17);
		constraints << QTextLength(QTextLength::FixedLength, 17);
		constraints << QTextLength(QTextLength::FixedLength, 17);
		constraints << QTextLength(QTextLength::FixedLength, 17);
		constraints << QTextLength(QTextLength::FixedLength, 17);
		constraints << QTextLength(QTextLength::FixedLength, 17);
		tableFormat.setColumnWidthConstraints(constraints);
		tableFormat.setAlignment(Qt::AlignLeft);
		tableFormat.setCellSpacing(0);
		tableFormat.setCellPadding(0.5);
		tableFormat.setBorder(1);
		tableIndPodNum = cellCursor.insertTable(1, 12, tableFormat);
		
		pCell = new UPopulateCell(tableIndPodNum);
		pCell->setAlignment( Qt::AlignCenter);
		textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
		pCell->setTextCharFormat(textCharFormat);
		QString indPodNumber = prodavecQuery.value(4).toString();
		if (indPodNumber.size() > 11){
			pCell->set(0, 0, indPodNumber.at(0));
			pCell->set(0, 1, indPodNumber.at(1));
			pCell->set(0, 2, indPodNumber.at(2));
			pCell->set(0, 3, indPodNumber.at(3));
			pCell->set(0, 4, indPodNumber.at(4));
			pCell->set(0, 5, indPodNumber.at(5));
			pCell->set(0, 6, indPodNumber.at(6));
			pCell->set(0, 7, indPodNumber.at(7));
			pCell->set(0, 8, indPodNumber.at(8));
			pCell->set(0, 9, indPodNumber.at(9));
			pCell->set(0, 10, indPodNumber.at(10));
			pCell->set(0, 11, indPodNumber.at(11));
		}
		
		delete pCell;
		cellCursor = cell.lastCursorPosition();
	cellCursor.insertText("(������������� ���������� ����� ��������)",textCharFormat);
	
	cell = table->cellAt(3, 3);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("(�����)",textCharFormat);
		//������� �������������� ����������� ������ ��������
		constraints.clear();
		constraints << QTextLength(QTextLength::FixedLength, 17);
		constraints << QTextLength(QTextLength::FixedLength, 17);
		constraints << QTextLength(QTextLength::FixedLength, 17);
		constraints << QTextLength(QTextLength::FixedLength, 17);
		constraints << QTextLength(QTextLength::FixedLength, 17);
		constraints << QTextLength(QTextLength::FixedLength, 17);
		constraints << QTextLength(QTextLength::FixedLength, 17);
		constraints << QTextLength(QTextLength::FixedLength, 17);
		constraints << QTextLength(QTextLength::FixedLength, 17);
		constraints << QTextLength(QTextLength::FixedLength, 17);
		constraints << QTextLength(QTextLength::FixedLength, 17);
		constraints << QTextLength(QTextLength::FixedLength, 17);
		tableFormat.setColumnWidthConstraints(constraints);
		tableFormat.setAlignment(Qt::AlignLeft);
		tableFormat.setCellSpacing(0);
		tableFormat.setCellPadding(0.5);
		tableFormat.setBorder(1);
		tableIndPodNum = cellCursor.insertTable(1, 12, tableFormat);
		
		pCell = new UPopulateCell(tableIndPodNum);
		pCell->setAlignment( Qt::AlignCenter);
		textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
		pCell->setTextCharFormat(textCharFormat);
		indPodNumber = pokupecQuery.value(5).toString();
		if (indPodNumber.size() > 11){
			pCell->set(0, 0, indPodNumber.at(0));
			pCell->set(0, 1, indPodNumber.at(1));
			pCell->set(0, 2, indPodNumber.at(2));
			pCell->set(0, 3, indPodNumber.at(3));
			pCell->set(0, 4, indPodNumber.at(4));
			pCell->set(0, 5, indPodNumber.at(5));
			pCell->set(0, 6, indPodNumber.at(6));
			pCell->set(0, 7, indPodNumber.at(7));
			pCell->set(0, 8, indPodNumber.at(8));
			pCell->set(0, 9, indPodNumber.at(9));
			pCell->set(0, 10, indPodNumber.at(10));
			pCell->set(0, 11, indPodNumber.at(11));
		}
		
		delete pCell;
		
		cellCursor = cell.lastCursorPosition();
	cellCursor.insertText("(������������� ���������� ����� ��������)",textCharFormat);
	
	cell = table->cellAt(4, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("̳�������������� ��������  ",textCharFormat);
	cellCursor.insertText(prodavecQuery.value(1).toString(),textCharFormat_underline);
	
	cell = table->cellAt(4, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("̳�������������� �������  ",textCharFormat);
	cellCursor.insertText(pokupecQuery.value(1).toString(),textCharFormat_underline);
	
	cell = table->cellAt(5, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("����� ��������  ",textCharFormat);
	cellCursor.insertText(prodavecQuery.value(2).toString(),textCharFormat_underline);
	
	cell = table->cellAt(5, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("����� ��������  ",textCharFormat);
	cellCursor.insertText(pokupecQuery.value(2).toString(),textCharFormat_underline);
	
	cell = table->cellAt(6, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("����� �������� ��� ��������� �������� ������� �� ������ ������� (��������)  ",textCharFormat);
	//����� �������� ��� ��������� �������� ������� �� ������
		constraints.clear();
		constraints << QTextLength(QTextLength::FixedLength, 100);

		tableFormat.setColumnWidthConstraints(constraints);
		tableFormat.setAlignment(Qt::AlignRight);
		tableFormat.setCellSpacing(0);
		tableFormat.setCellPadding(0.5);
		tableFormat.setBorder(1);
		tableIndPodNum = cellCursor.insertTable(1, 1, tableFormat);
		
		cell = tableIndPodNum->cellAt(0, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(prodavecQuery.value(3).toString(),textCharFormat_bold);
	
	cell = table->cellAt(6, 2);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("����� �������� ��� ��������� �������� ������� �� ������ ������� (�������)  ",textCharFormat);
		//����� �������� ��� ��������� �������� ������� �� ������
		constraints.clear();
		constraints << QTextLength(QTextLength::FixedLength, 100);

		tableFormat.setColumnWidthConstraints(constraints);
		tableFormat.setAlignment(Qt::AlignRight);
		tableFormat.setCellSpacing(0);
		tableFormat.setCellPadding(0.5);
		tableFormat.setBorder(1);
		tableIndPodNum = cellCursor.insertTable(1, 1, tableFormat);
		
		cell = tableIndPodNum->cellAt(0, 0);
		cell.setFormat(textCharFormat);
		cellCursor = cell.firstCursorPosition();
		blockFormat.setAlignment( Qt::AlignCenter );
		cellCursor.setBlockFormat( blockFormat );
		cellCursor.insertText(pokupecQuery.value(3).toString(),textCharFormat_bold);
	
	cell = table->cellAt(7, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("����� ��������   ", textCharFormat);
	cellCursor.insertText("����� �"+QString::number(ugodaNum)+" �� "+pokupecQuery.value(4).toDate().toString("dd.MM.yyyy")+"�.", textCharFormat_underline);
	
	cell = table->cellAt(8, 0);
	cell.setFormat(textCharFormat);
	cellCursor = cell.firstCursorPosition();
	blockFormat.setAlignment( Qt::AlignLeft );
	cellCursor.setBlockFormat( blockFormat );
	cellCursor.insertText("����� ���������� ����������   ", textCharFormat);
	cellCursor.insertText("������ � �������������� �������.", textCharFormat_underline);
	
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	
	constraints.clear();
	constraints << QTextLength(QTextLength::FixedLength, 30);
	constraints << QTextLength(QTextLength::FixedLength, 96);
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
	tableFormat.setAlignment(Qt::AlignHCenter);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(1);
	tableFormat.setBorder(0.8);
	table = cursor.insertTable(13, 11, tableFormat);
	
	pCell = new UPopulateCell(table);
	textCharFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
	pCell->setAlignment( Qt::AlignCenter );
	pCell->setTextCharFormat(textCharFormat);
	pCell->setTextBlockFormat(blockFormat);
	
	table->mergeCells ( 0, 0, 2, 1 );
	table->mergeCells ( 0, 1, 2, 1 );
	table->mergeCells ( 0, 2, 2, 1 );
	table->mergeCells ( 0, 3, 2, 1 );
	table->mergeCells ( 0, 4, 2, 1 );
	table->mergeCells ( 0, 5, 2, 1 );
	table->mergeCells ( 0, 10, 2, 1 );
	
	table->mergeCells ( 0, 6, 1, 4 );
	table->mergeCells ( 5, 1, 1, 2 );
	table->mergeCells ( 6, 1, 1, 2 );
	table->mergeCells ( 7, 1, 1, 2 );
	table->mergeCells ( 8, 0, 2, 1 );
	table->mergeCells ( 8, 1, 2, 1 );
	table->mergeCells ( 10, 1, 1, 2 );
	table->mergeCells ( 11, 1, 1, 2 );
	table->mergeCells ( 12, 1, 1, 2 );
	
	pCell->set( 0, 0, "���-��");
	pCell->set( 0, 1, "���� ������������ (���������, �������� (������*) ������ (������)");
	pCell->set( 0, 2, "������������ �������� ������ (������) ��������");
	pCell->set( 0, 3, "������� ����� ������");
	pCell->set( 0, 4, "ʳ������ (�ᒺ�, �����)");
	pCell->set( 0, 5, "ֳ�� �������� ������� ��������� ��� ���������� ���");
	pCell->set( 0, 6, "������ �������� (���� �������������) ��� ���������� ���, �� ��������� ������������� �� ��������");
	pCell->set( 1, 6, "20%");
	pCell->set( 1, 7, "0% (��������� �� ����� ��-����� ������)");
	pCell->set( 1, 8, "0% (�������)");
	pCell->set( 1, 9, "��������� �� ���");
	pCell->set( 0, 10, "�������� ���� �����, �� ������ �����");
	
	pCell->set( 2, 0, "1");
	pCell->set( 2, 1, "2");
	pCell->set( 2, 2, "3");
	pCell->set( 2, 3, "4");
	pCell->set( 2, 4, "5");
	pCell->set( 2, 5, "6");
	pCell->set( 2, 6, "7");
	pCell->set( 2, 7, "8");
	pCell->set( 2, 8, "9");
	pCell->set( 2, 9, "10");
	pCell->set( 2, 10, "11");
	
	QDate naklDate(cur_year, cur_month, 1);
	
	UOrgTaryfInfo orgTaryf(cur_year, cur_month);
	double pdv = OrganizPdv();
	
	double taryfCO = orgTaryf.vart_g_kal(financeTypeForOrganiz(ugodaNum))*100/(100+pdv);
	double taryfGV = orgTaryf.vart_kuba_GV*100/(100+pdv);
	double sumNoPdvCO = sumCO*100/(100+pdv);
	double sumNoPdvGV = sumGV*100/(100+pdv);
	
	double kilkCO = sumNoPdvCO / taryfCO;
	double kilkGV = sumNoPdvGV / taryfGV;
	
	double pdvSum = (sumCO+sumGV)*pdv/(100+pdv);
	//���������� ��������
	pCell->set( 3, 0, "I.1");
	pCell->set( 3, 1, naklDate.toString("dd.MM")+" - "+QString::number(naklDate.daysInMonth())+"."+naklDate.toString("MM"));
	pCell->set( 3, 2, "��");
	pCell->set( 3, 3, "����");
	pCell->set( 3, 4, QString::number(kilkCO, 'f', 3));
	pCell->set( 3, 5, uMToStr2(taryfCO));
	pCell->set( 3, 6, uMToStr2(sumNoPdvCO));
	pCell->set( 3, 10, uMToStr2(sumNoPdvCO));
	
	//������ ����
	pCell->set( 4, 0, "I.2");
	pCell->set( 4, 1, naklDate.toString("dd.MM")+" - "+QString::number(naklDate.daysInMonth())+"."+naklDate.toString("MM"));
	pCell->set( 4, 2, "��");
	pCell->set( 4, 3, "�.���");
	pCell->set( 4, 4, QString::number(kilkGV, 'f', 3));
	pCell->set( 4, 5, uMToStr2(taryfGV));
	pCell->set( 4, 6, uMToStr2(sumNoPdvGV));
	pCell->set( 4, 10, uMToStr2(sumNoPdvGV));
	
	pCell->set( 5, 1, "������ �� ������ I");
	pCell->set( 5, 6, uMToStr2(sumNoPdvCO+sumNoPdvGV));
	pCell->set( 5, 10, uMToStr2(sumNoPdvCO+sumNoPdvGV));
	
	pCell->set( 6, 0, "II");
	pCell->set( 6, 1, "���������������� �������");
	
	pCell->set( 7, 0, "III");
	pCell->set( 7, 1, "�������� (��������) ����");
	pCell->set( 7, 3, "X");
	pCell->set( 7, 4, "X");
	pCell->set( 7, 5, "X");
	pCell->set( 7, 6, "X");
	pCell->set( 7, 7, "X");
	pCell->set( 7, 8, "X");
	pCell->set( 7, 9, "X");
	
	pCell->set( 8, 0, "IV");
	pCell->set( 8, 1, "������ �������:");
	pCell->set( 8, 2, "�������� (+)");
	pCell->set( 9, 2, "������ (-)");
	
	pCell->set( 10, 0, "V");
	pCell->set( 10, 1, "������ �� ������� I +II + IV");
	pCell->set( 10, 6, uMToStr2(sumNoPdvCO+sumNoPdvGV));
	pCell->set( 10, 10, uMToStr2(sumNoPdvCO+sumNoPdvGV));
	
	pCell->set( 11, 0, "VI");
	pCell->set( 11, 1, "������� �� ������ �������");
	pCell->set( 11, 6, uMToStr2(pdvSum));
	pCell->set( 11, 10, uMToStr2(pdvSum));
	
	pCell->set( 12, 0, "VII");
	pCell->set( 12, 1, "�������� ���� � ���");
	pCell->set( 12, 6, uMToStr2(sumCO+sumGV));
	pCell->set( 12, 10, uMToStr2(sumCO+sumGV));
	
	//��������� ���������� ������
	if ( (sumCO < 0.009) && (sumGV < 0.009) )
		table->removeRows( 3, 2 );
	else if (sumCO < 0.009)
		table->removeRows( 3, 1 );
	else if (sumGV < 0.009)
		table->removeRows( 4, 1 );
	
	delete pCell;
	
	cursor.movePosition(QTextCursor::End);
	blockFormat.setAlignment( Qt::AlignLeft );
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("���� ���, ��������� (�������) � ������ � ��������� ������ (������), ���������� � ��� ��������, �������� ���������, ���������� ��� ���������� ���������� �������� � ������� �� ������ ��������� �� ������� ���������� ���������.", textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("	�.�.", textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("				________________________________________________", textCharFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("				(����� � ������� �����, ��� ������ ��������� ��������)", textCharFormat_small);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertBlock(blockFormat);
	cursor.insertText("* ���� ������ ��������� � ��� ���������� ������ ��������, �� ��� ���������� ��������� ��������; ��� ��������� �������� � ���� �������������� ������, ���������� ��������� � ������ ��������; ���� ������������ ��������� ������ � ������ ��������; ��� �������� � �������� ������ (������) �� ������� ������� �������� �� ������ ������ ���� ������� �� ������ ��������.", textCharFormat_small);
	
	
	printform->printer()->setDocName("��������� �������� �� "+monthlist.at(cur_month-1)+" "+QString::number(cur_year)+"�.");
	printform->show();
	delete query;
}
//------------------------------------------------------------
void UPrintDocsOrganiz::print_act_vykon_robit(QDate dt, int ugodaNum)
{
    QString *fileName = new QString(QCoreApplication::applicationDirPath ()
                                    +"/templates/organizActVykonRobit_1.html");
    QFile *file = new QFile(*fileName);
    if (file->open(QFile::ReadOnly)) {
        printform = new UPrintForm(0, "��� ��������� ����");
        printform->printer()->setPageMargins( 20, 10, 10, 10, QPrinter::Millimeter );

        QHash<QString, QString> hash;
        UReplaceStringParser *parser = new UReplaceStringParser;
        parser->setBeginTagChar('[');
        parser->setEndTagChar(']');

        QByteArray *data = new QByteArray();
        *data = file->readAll();
        QTextCodec *c = Qt::codecForHtml(*data);
        parser->setInputString(c->toUnicode(*data));

        file->close();

        QSqlQuery query;

        hash.insert(QString("ThisOrganizTown"), OrganizCity());
        hash.insert(QString("ThisOrganizShortName"), OrganizName());
        hash.insert(QString("ThisDyrector"), PrizvIB(1));

        query.exec("SELECT Nazva, Date_ugody, Finans, Adresa, Rozr_rah "
                   "FROM organizacii WHERE Ugoda = " + sqlStr(ugodaNum));
        query.next();
        QString zamovnykName = query.value(0).toString();
        bool isNasel = false;
        if (query.value(2).toInt() == 6)
            isNasel = true;

        hash.insert(QString("ZamovnykName"), zamovnykName);
        hash.insert(QString("DogovirNum"), QVariant(ugodaNum).toString());
        QDate dtUgody = query.value(1).toDate();
        hash.insert(QString("DogovirDate"), dtUgody.toString("d")
                    + " " + monthListRod[dtUgody.month()-1]
                    + " " + dtUgody.toString("yyyy"));
        hash.insert(QString("ZamovnykAdresa"), query.value(3).toString());
        QString strRozrRahZamovnyka = query.value(4).toString();
        hash.insert(QString("ZamovnykRozrRah"), !strRozrRahZamovnyka.isEmpty() ? strRozrRahZamovnyka : "______________________________");

        hash.insert(QString("PoslugyMonthLong"), monthlist[dt.month()-1]);
        hash.insert(QString("PoslugyYear"), dt.toString("yyyy"));

        // �����
        UNasTaryfInfo taryfNas = nasTaryf(dt.year(), dt.month());
		UOrgTaryfInfo orgTaryf(dt);
        double gKalOpal = orgTaryf.vart_g_kal(financeTypeForOrganiz(ugodaNum));

        if (taryfNas.vart_g_kal == 0) {
            QMessageBox::critical(0,"������� ������","�������� ����� �� �� ��� ���������");
            taryfNas.vart_g_kal = 0.1;
        }
        if (gKalOpal == 0) {
            QMessageBox::critical(0,"������� ������","�������� ����� �� �� ��� ����������");
            gKalOpal = 0.01;
        }

        // ���� �����������
        query.exec("SELECT sum(oper.Narah_opal), sum(oper.Narah_voda) "
                   "FROM pidrozdily "
                   "LEFT JOIN (SELECT * FROM operacii "
                   "WHERE year="+sqlStr(dt.year())+" "
                   "and month="+sqlStr(dt.month())+") AS oper ON oper.Pidrozdil=pidrozdily.id "
                   "WHERE pidrozdily.Ugoda="+sqlStr(ugodaNum));
        query.seek(0);
        double narahOpal = query.value(0).toDouble();
        double narahVoda = 0 /*query.value(1).toDouble()*/;
        double zagNarah = narahOpal + narahVoda;
        double pdv = OrganizPdv();
        double pdvSuma = uDToM((zagNarah)*pdv/(100+pdv));

        // ʳ������ ����
        hash.insert(QString("PoslugyOpalGKal"), uMToStr2(narahOpal/(isNasel ? taryfNas.vart_g_kal : gKalOpal)));
        hash.insert(QString("PoslugyOpalSuma"), uMToStr2(narahOpal));
        hash.insert(QString("PoslugyZagSuma"), uMToStr2(zagNarah));

        QString zagSumaPropys(uMToStr2(zagNarah));
        hash.insert(QString("PoslugyZagSumaPropys"), numberToString(zagSumaPropys));

        hash.insert(QString("PoslugyZagPDV"), uMToStr2(pdvSuma));

        QString zagPdvPropys(uMToStr2(pdvSuma));
        hash.insert(QString("PoslugyZagPDVPropys"), numberToString(zagPdvPropys));

        parser->setTagsStringsHash(hash);
        parser->convertString();

        printform->document()->setHtml(parser->curOutputString());
        printform->printer()->setDocName("��� ��������� ���� "+zamovnykName+"�� "+monthlist.at(dt.month()-1)
                                         +" "+QString::number(dt.year())+"�.");
        printform->show();

        delete data;
    }
    else{
        QMessageBox::critical(
                0,
                codec->toUnicode("!!! �� �������� ������ ����� !!!"),
                codec->toUnicode("�� �������� ����� ")+*fileName+".",
                QMessageBox::Ok,
                QMessageBox::NoButton,
                QMessageBox::NoButton);
    }
    delete file;
    delete fileName;
}
