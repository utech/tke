//------------------------------------------------------------
//
// Implamantation of tke_Dialog_narah class.
// Created by Roman Mulyk 24.11.2006
//
//------------------------------------------------------------

#include "tke_form_insert_subs.h"
#include "../tkeOrganizacii/udiff_func.h"
#include <UMath>
#include <USqlAccessible>

//------------------------------------------------------------
tke_Form_insert_subs::tke_Form_insert_subs(QWidget *parent, short year, short month, bool donarah)
        : QWidget(parent)
{
    ui.setupUi(this);
	
	isDonarah = donarah;
	
	date = QDate::currentDate();
	codec = QTextCodec::codecForName("CP1251");
	if (date.month() == 12)
		ui.spinBox_year->setMaximum(date.year()+1);
	else
		ui.spinBox_year->setMaximum(date.year());
	//ui.spinBox_year->parentWidget()->setWindowFlags ( ui.spinBox_year->parentWidget()->windowFlags() | Qt::WindowStaysOnTopHint );
	if (donarah){
		subsCol = "Suma_d";
		ui.textEdit->parentWidget()->setWindowTitle( codec->toUnicode("�������� ������������� �������"));
		//subsVydText = ( codec->toUnicode("�����."));
		ui.label_subs_donarah->setText("�������������");
	}
	else{
		subsCol = "Suma";
		ui.textEdit->parentWidget()->setWindowTitle( codec->toUnicode("�������� ����������� �������"));
		//subsVydText = ( codec->toUnicode("�������."));
		ui.label_subs_donarah->setText("�����������");
	}
	
	
	ui.spinBox_year->setValue(year);
	ui.spinBox_month->setValue(month);
	query = new QSqlQuery();
	zapys = 0;
	cursor = ui.textEdit->textCursor();
	
	textCharFormat.setFontPointSize( 9 );
	textCharFormat_bold.setFontPointSize( 9 );
	textCharFormat_red.setFontPointSize( 9 );
	textCharFormat_green.setFontPointSize( 9 );
	QFont font = textCharFormat_bold.font();
	font.setBold( true );
	textCharFormat_bold.setFont(font);
	QPen pen = textCharFormat_red.textOutline();
	pen.setColor(Qt::red);
	textCharFormat_red.setTextOutline(pen);
	pen.setColor(Qt::green);
	textCharFormat_green.setTextOutline(pen);
	
	clear_subs_form();
	
		//connectors
	connect(ui.pushButton_close_form, SIGNAL(clicked()), this, SLOT(close ())); //�������� �����
	connect(ui.spinBox_Rahunok, SIGNAL(editingFinished()), this, SLOT(read_data())); //���������� ����� ��� �����䳿 ��������
	connect(ui.doubleSpinBox_subs_donarah, SIGNAL(editingFinished()), this, SLOT(save_data())); //����� �����
	connect(ui.pushButton_clear, SIGNAL(clicked()), this, SLOT(clear_subs_form()));
}
//------------------------------------------------------------
int tke_Form_insert_subs::get_Month()
{
	return ui.spinBox_month->value();
}
//------------------------------------------------------------
int tke_Form_insert_subs::get_Year()
{
	return ui.spinBox_year->value();
}
//------------------------------------------------------------
void tke_Form_insert_subs::read_data()
{
	emit  u_abonent_rahunok_changes(ui.spinBox_Rahunok->value());

	query->exec("SELECT sum(Suma) AS Suma_subs, \
						sum(Suma_d) AS Suma_donarah \
					FROM subs \
					WHERE (Rahunok_ID="+QVariant(ui.spinBox_Rahunok->value()).toString()+") \
						and (Year="+QVariant(ui.spinBox_year->value()).toString()+") \
						and (Month="+QVariant(ui.spinBox_month->value()).toString()+")");
	if (query->seek(0)){
		ui.label_subs_potochn->setText(codec->toUnicode("���� ����������� ����. - ") +
						uMToStr2(query->value(0).toDouble()) +
						codec->toUnicode(" ���.;\n���� ������������� ����. - ") +
						uMToStr2(query->value(1).toDouble()) +
						codec->toUnicode(" ���."));
		subs_donarah = query->value(1).toDouble();
	}
	else{
		ui.label_subs_potochn->setText(codec->toUnicode("��������� ��������� ��� ��� ������� ��������"));
		subs_donarah = 0;
	}
		
	if (ui.spinBox_Rahunok->hasFocus()){
		disconnect(ui.spinBox_Rahunok, SIGNAL(editingFinished()), this, SLOT(read_data())); //���������� ����� ��� �����䳿 ��������
		ui.doubleSpinBox_subs_donarah->setFocus();
		ui.doubleSpinBox_subs_donarah->selectAll();
		connect(ui.spinBox_Rahunok, SIGNAL(editingFinished()), this, SLOT(read_data())); //���������� ����� ��� �����䳿 ��������
	}
}
//------------------------------------------------------------
void tke_Form_insert_subs::save_data()
{
	double new_donarah_subs;
	
	if (ui.doubleSpinBox_subs_donarah->value() !=0 ){
		QSqlRecord record;
		query->exec("SELECT count(*) AS Count FROM subs \
						WHERE (Rahunok_ID="+QVariant(ui.spinBox_Rahunok->value()).toString()+") \
						and (Year="+QVariant(ui.spinBox_year->value()).toString()+") \
						and (Month="+QVariant(ui.spinBox_month->value()).toString()+")");
		query->seek(0);
		if (query->value(0).toInt() == 0){
			tableModel.setTable("subs");
			tableModel.select();
			tableModel.insertRows(0, 1);
			record = tableModel.record(0);
			record.setValue("id", nextLichylnyk("subs"));
			record.setValue("Rahunok_ID", QVariant(ui.spinBox_Rahunok->value()));
			record.setValue("Year", QVariant(ui.spinBox_year->value()));
			record.setValue("Month", QVariant(ui.spinBox_month->value()));
			new_donarah_subs = ui.doubleSpinBox_subs_donarah->value();
			record.setValue(subsCol, QVariant(new_donarah_subs));
			tableModel.setRecord(0, record);
			q_ok = tableModel.submitAll();
			operaciya = 0;
		}
		else{
			tableModel.setTable("subs");
			tableModel.setFilter("(Rahunok_ID="+QVariant(ui.spinBox_Rahunok->value()).toString()+
						") and (Year="+QVariant(ui.spinBox_year->value()).toString()+
						") and (Month="+QVariant(ui.spinBox_month->value()).toString()+")");
			tableModel.select();
			record = tableModel.record(0);
			new_donarah_subs = ui.doubleSpinBox_subs_donarah->value() + record.value(subsCol).toDouble();
			record.setValue(subsCol, QVariant(new_donarah_subs));
			tableModel.setRecord(0, record);
			q_ok = tableModel.submitAll();
			operaciya = 1;
		}
		zapys++;
			//��������� ���������� �� �����
		query->exec("SELECT Prizv, Imya, Batk FROM abonent WHERE Rahunok="+QVariant(ui.spinBox_Rahunok->value()).toString());
		query->seek(0);
		blockFormat.setAlignment( Qt::AlignLeft );
		cursor.setBlockFormat( blockFormat );
		
		cursor.insertText( QVariant(zapys).toString() +
							". /", textCharFormat);
		cursor.insertText( QVariant(ui.spinBox_Rahunok->value()).toString(), textCharFormat_bold);
		cursor.insertText( "/ " +
							query->value(0).toString() +
							" " +
							query->value(1).toString() +
							" " +
							query->value(2).toString(), textCharFormat);
		if (isDonarah)
			cursor.insertText( codec->toUnicode(" (�������.����.( ") +
							uMToStr2(subs_donarah) +
							codec->toUnicode(" -> "), textCharFormat);
		else
			cursor.insertText( codec->toUnicode(" (�����.����.( ") +
							uMToStr2(subs_donarah) +
							codec->toUnicode(" -> "), textCharFormat);
		cursor.insertText( uMToStr2(new_donarah_subs), textCharFormat_bold);
		cursor.insertText( codec->toUnicode(" )"), textCharFormat);
		
		if (q_ok)
			temp_textCharFormat = textCharFormat_green;
		else
			temp_textCharFormat = textCharFormat_red;
		
		if (operaciya == 0)
			cursor.insertText( codec->toUnicode(" (����� ��������)."), temp_textCharFormat);
		else if (operaciya == 1)
			cursor.insertText( codec->toUnicode(" (����� ������)."), temp_textCharFormat);
		else if (operaciya == 2)
			cursor.insertText( codec->toUnicode(" (����� ��������)."), temp_textCharFormat);
		cursor.insertBlock(blockFormat);

		if (ui.doubleSpinBox_subs_donarah->hasFocus()){
			disconnect(ui.doubleSpinBox_subs_donarah, SIGNAL(editingFinished()), this, SLOT(save_data())); //����� �����
			ui.spinBox_Rahunok->setFocus();
			ui.spinBox_Rahunok->selectAll();
			connect(ui.doubleSpinBox_subs_donarah, SIGNAL(editingFinished()), this, SLOT(save_data())); //����� �����
		}
		if (q_ok){
			sum_donarah += ui.doubleSpinBox_subs_donarah->value();
			show_subs_sum();
		}
	}
}
//------------------------------------------------------------
void tke_Form_insert_subs::clear_subs_form()
{
	sum_donarah=0;
	ui.spinBox_Rahunok->setValue(1);
	ui.doubleSpinBox_subs_donarah->setValue(0);
	ui.textEdit->clear();
	show_subs_sum();
}
//------------------------------------------------------------
void tke_Form_insert_subs::show_subs_sum()
{
	if (isDonarah)
		ui.label_subs_sum->setText(codec->toUnicode("���� ��������� ������������� ������� - ") +
						uMToStr2(sum_donarah) + 
						codec->toUnicode(" ���."));
	else
		ui.label_subs_sum->setText(codec->toUnicode("���� ��������� ����������� ������� - ") +
						uMToStr2(sum_donarah) + 
						codec->toUnicode(" ���."));
}
//------------------------------------------------------------
tke_Form_insert_subs::~tke_Form_insert_subs()
{
	delete query;
}
//------------------------------------------------------------
