//------------------------------------------------------------
//
// Implamantation of tke_Dialog_narah class.
// Created by Roman Mulyk 26.11.2006
//
//------------------------------------------------------------

#include "tke_form_insert_kvyt.h"
#include "../tkeOrganizacii/udiff_func.h"
#include <UMath>
#include <USqlAccessible>

//------------------------------------------------------------
tke_Form_insert_kvyt::tke_Form_insert_kvyt(QWidget *parent, short year, short month)
        : QWidget(parent)
{
    ui.setupUi(this);

	date = QDate::currentDate();
	codec = QTextCodec::codecForName("CP1251");
	if (date.month() == 12)
		ui.spinBox_year->setMaximum(date.year()+1);
	else
		ui.spinBox_year->setMaximum(date.year());
	//ui.spinBox_year->parentWidget()->setWindowFlags ( ui.spinBox_year->parentWidget()->windowFlags() | Qt::WindowStaysOnTopHint );
	ui.spinBox_year->setValue(year);
	ui.spinBox_month->setValue(month);
	query = new QSqlQuery();
	zapys = 0;
	pachka_sum = 0;
	new_pack(); //Новий пакет
	show_sum(); //Показати суму
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
	
		//connectors 
	connect(ui.pushButton_close_form, SIGNAL(clicked()), this, SLOT(close())); //закриття форми
	connect(ui.pushButton_new_packet, SIGNAL(clicked()), this, SLOT(new_pack())); //новий пакет
	connect(ui.spinBox_Rahunok, SIGNAL(editingFinished()), this, SLOT(read_data())); //зчитування даних про субсидії абонента
	connect(ui.spinBox_oplata_day, SIGNAL(editingFinished()), this, SLOT(save_data())); //запис даних
	connect(ui.doubleSpinBox_suma, SIGNAL(editingFinished()), this, SLOT(narah_suma_spinbox_acepted()));
}
//------------------------------------------------------------
int tke_Form_insert_kvyt::get_Month()
{
	return ui.spinBox_month->value();
}
//------------------------------------------------------------
int tke_Form_insert_kvyt::get_Year()
{
	return ui.spinBox_year->value();
}
//------------------------------------------------------------
void tke_Form_insert_kvyt::read_data()
{
	emit  u_abonent_rahunok_changes(ui.spinBox_Rahunok->value());

	query->exec("SELECT sum(Suma) AS Suma_opl \
					FROM kvytancii \
					WHERE year(Plata_date)="+QVariant(ui.spinBox_year->value()).toString()+" \
					and month(Plata_date)="+QVariant(ui.spinBox_month->value()).toString()+" \
					and Rahunok_ID="+QVariant(ui.spinBox_Rahunok->value()).toString());
	if (query->seek(0))
		ui.doubleSpinBox_suma->setValue(query->value(0).toDouble());
	if (ui.spinBox_Rahunok->hasFocus()){
		disconnect(ui.spinBox_Rahunok, SIGNAL(editingFinished()), this, SLOT(read_data())); //зчитування даних про субсидії абонента
		ui.doubleSpinBox_suma->setFocus();
		ui.doubleSpinBox_suma->selectAll();
		connect(ui.spinBox_Rahunok, SIGNAL(editingFinished()), this, SLOT(read_data())); //зчитування даних про субсидії абонента
	}
}
//------------------------------------------------------------
void tke_Form_insert_kvyt::save_data()
{
	if ((ui.spinBox_Rahunok!=0) && (ui.doubleSpinBox_suma->value()!=0)){
		QSqlRecord record;
		x_date.setYMD(ui.spinBox_year->value(), ui.spinBox_month->value(), ui.spinBox_oplata_day->value());
		tableModel.setTable("kvytancii");
		tableModel.select();
		tableModel.insertRows(0, 1);
		record = tableModel.record(0);
		record.setValue("id", nextLichylnyk("kvytancii"));
		record.setValue("Rahunok_ID", QVariant(ui.spinBox_Rahunok->value()));
		record.setValue("Plata_date", QVariant(x_date));
		record.setValue("Perevid_date", QVariant(x_date));
		record.setValue("Suma", QVariant(ui.doubleSpinBox_suma->value()));
		record.setValue("Pachka", QVariant(ui.spinBox_packet->value()));
		tableModel.setRecord(0, record);
		q_ok = tableModel.submitAll();

		zapys++;
			//Виведення інформації на форму
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
							query->value(2).toString() +
							" " +
							codec->toUnicode("сума.оплати ( "), textCharFormat);
		cursor.insertText( uMToStr2(ui.doubleSpinBox_suma->value()), textCharFormat_bold);
		cursor.insertText( codec->toUnicode("грн.) дата опл.( "), textCharFormat);
		cursor.insertText( x_date.toString("d.MMM.yyyy") + 
							codec->toUnicode("р."), textCharFormat_bold);
		cursor.insertText( codec->toUnicode(" )"), textCharFormat);
		
		if (q_ok){
			cursor.insertText( codec->toUnicode(" Ок"), textCharFormat_green);
			pachka_sum += ui.doubleSpinBox_suma->value();
			show_sum();
		}
		else
			cursor.insertText( codec->toUnicode(" Не внесено"), textCharFormat_red);
		cursor.insertBlock(blockFormat);
	}
	if (ui.spinBox_oplata_day->hasFocus()){
		disconnect(ui.spinBox_oplata_day, SIGNAL(editingFinished()), this, SLOT(save_data())); //запис даних
		ui.spinBox_Rahunok->setFocus();
		ui.spinBox_Rahunok->selectAll();
		connect(ui.spinBox_oplata_day, SIGNAL(editingFinished()), this, SLOT(save_data())); //запис даних
	}
}
//------------------------------------------------------------
void tke_Form_insert_kvyt::narah_suma_spinbox_acepted()
{
	if (ui.doubleSpinBox_suma->hasFocus()){
		disconnect(ui.doubleSpinBox_suma, SIGNAL(editingFinished()), this, SLOT(narah_suma_spinbox_acepted()));
		ui.spinBox_oplata_day->setFocus();
		ui.spinBox_oplata_day->selectAll();
		connect(ui.doubleSpinBox_suma, SIGNAL(editingFinished()), this, SLOT(narah_suma_spinbox_acepted()));
	}
}
//------------------------------------------------------------
void tke_Form_insert_kvyt::new_pack()
{
	query->exec("SELECT max(Pachka) FROM kvytancii WHERE year(Plata_date)="+QVariant(ui.spinBox_year->value()).toString());
	if (query->seek(0))
		ui.spinBox_packet->setValue(query->value(0).toInt() + 1);
	else 
		ui.spinBox_packet->setValue(1);
	ui.textEdit->clear();
	zapys = 0;
	pachka_sum = 0;
}
//------------------------------------------------------------
void tke_Form_insert_kvyt::show_sum()
{
	ui.label_show_sum->setText(codec->toUnicode("Сума, внесена\nв пакеті: ") + uMToStr2(pachka_sum) + codec->toUnicode("грн."));
}
//------------------------------------------------------------
tke_Form_insert_kvyt::~tke_Form_insert_kvyt()
{
	delete query;
}
//------------------------------------------------------------
