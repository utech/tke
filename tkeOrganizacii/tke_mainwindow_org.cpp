//------------------------------------------------------------
//
// Implamantation of tke_MainWindow class.
// Created by Roman Mulyk 19.02.2007
//
//------------------------------------------------------------

#include <USqlAccessible>
#include "tke_mainwindow_org.h"
#include "../tkeSqlBrowser/src/browser.h"
#include "tke_dialog_print_nakladna.h"

//------------------------------------------------------------
tke_MainWindow_org::tke_MainWindow_org(QWidget *parent)
        : QMainWindow(parent)
{
	ui.setupUi(this);
	dateEdit_PotochnaData = new QDateEdit();
    dateEdit_PotochnaData->setObjectName(QString::fromUtf8("dateEdit_PotochnaData"));
	dateEdit_PotochnaData->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    dateEdit_PotochnaData->setCorrectionMode(QAbstractSpinBox::CorrectToNearestValue);
    //dateEdit_PotochnaData->setMaximumDate(QDate(2100, 12, 31));
    dateEdit_PotochnaData->setMinimumDate(QDate(2005, 1, 1));
	dateEdit_PotochnaData->setDisplayFormat("Поточний місяць: MM.yyyy");
    dateEdit_PotochnaData->setCalendarPopup(true);
    dateEdit_PotochnaData->setCurrentSectionIndex(1);
	statusBar()->addPermanentWidget(dateEdit_PotochnaData);
	calculatorButton = new QPushButton(QIcon(QString::fromUtf8(":/pictures/pictures/calculator.png")), "Калькулятор");
	calculatorButton->setIconSize(QSize(12,12));
	statusBar()->addPermanentWidget(calculatorButton);
	
	setAttribute(Qt::WA_DeleteOnClose);
	
	setSettingsFromIni();
	//Змінні
	controlObjectsHash = new QHash<int, QObject *>;
	codec = QTextCodec::codecForName("CP1251");
	palette = new QPalette();   palette->setColor ( QPalette::Active, QPalette::WindowText, Qt::red );
	default_palette = new QPalette();  *default_palette = ui.line->palette();
	date = QDate::currentDate();
	record_edited = 0;
	new_record = 0;
	defValuesThread = new UCalcDefValuesThread();
	connect(defValuesThread, SIGNAL(changeCell(int, int, double)), this, SLOT(changeCellContents(int, int, double)));
	
	dateEdit_PotochnaData->setMaximumDate(date.addMonths(2));
	
	orgSelectQuery = new QSqlQuery();
	specRahSelectQuery = new QSqlQuery();
	insertSqlModel = new QSqlTableModel();
	insertSqlModel->setTable("organizacii");
	modelInsertPidrozdily = new QSqlTableModel();
	modelInsertPidrozdily->setTable("pidrozdily");
	modelOplata = new QSqlTableModel();
	modelOplata->setTable("oplata");
	dniOpalQueryModel = new QSqlQueryModel();
	
	//initialize days_in_month array
	days_in_month[0]=31;
	days_in_month[1]=28;
	days_in_month[2]=31;
	days_in_month[3]=30;
	days_in_month[4]=31;
	days_in_month[5]=30;
	days_in_month[6]=31;
	days_in_month[7]=31;
	days_in_month[8]=30;
	days_in_month[9]=31;
	days_in_month[10]=30;
	days_in_month[11]=31;
	
	d_Vulycia = "";							// Вулиця
	d_Koef = 1.163;							// Коефіцієнт переведення
	d_Kotelnya_ID = 1;						// Ідентифікатор котельні
	d_City = codec->toUnicode("Самбір");	// Назва населеного пункту
	d_Year = date.year();					// Рік
	d_Month = date.month();					// Місяць
	d_Sprava = 0;							// Справа
	orgSelectQuery->exec("SELECT min(Ugoda) FROM organizacii");
	orgSelectQuery->seek(0);
	d_Ugoda = orgSelectQuery->value(0).toInt();		// Угода
	d_Month_poch = date.month();			// Місяць початку
	d_Year_poch = date.year();				// Рік початку
	d_Month_end = date.month();				// Місяць кінця
	d_Year_end = date.year();				// Рік кінця
	d_Kategoriya_pilgy_ID = 0;				// Ідентифікатор пільги
	d_Splata_date = QDate::currentDate();	// Дата оплати
	d_Perevid_Date = QDate::currentDate();	// Дата переведення
	d_Pachka = 0;							// № пачки
	
	//Вигляд форми
	ui.line->setFrameShadow(QFrame::Plain);
	ui.line_2->setFrameShadow(QFrame::Plain);
	ui.line_3->setFrameShadow(QFrame::Plain);
	ui.line_4->setFrameShadow(QFrame::Plain);
	ui.line_5->setFrameShadow(QFrame::Plain);
	ui.line_slujb_1->setFrameShadow(QFrame::Plain);
	ui.line_slujb_2->setFrameShadow(QFrame::Plain);
	ui.line_slujb_3->setFrameShadow(QFrame::Plain);
	
	ui.tableWidget_slujb_rah_operacii->setColumnHidden(0, true);

	//Невидимі dockWidget
	ui.dockWidget_taryf->setVisible(false);
	ui.dockWidget_pokaz_lich_table->setVisible(false);
	ui.dockWidget_pokaz_lich_gar_voda_table->setVisible(false);
	ui.dockWidget_meteor_table->setVisible(false);
	ui.dockWidget_dni_opal->setVisible(false);
	ui.dockWidget_oplata->setVisible(false);
	
	//Контекстне меню
	ui.tableView_pidrozdily->addAction(ui.actionEditPidrozdil);
	ui.tableView_pidrozdily->addAction(ui.actionAddPidrozdil);
	ui.tableView_pidrozdily->addAction(ui.actionDelPidrozdil);
	ui.tableView_pidrozdily->setContextMenuPolicy(Qt::ActionsContextMenu);

		//set table pidrozdily
	modelQueryStr = "SELECT pidrozdily.id, pidrozdily.Ugoda, pidrozdily.Name, ivt1.opal_val, \
							ivt2.opal_val, pidrozdily.Ploshcha, pidrozdily.Obem, pidrozdily.Tepl_harakt, \
							pidrozdily.T_vnutr, pidrozdily.Tepl_navantaj, ivt3.lich_values, \
							pidrozdily.Koef_lich, kotln.Vulycia & ' ' & kotln.Nomer  \
					FROM (((pidrozdily \
					LEFT JOIN (SELECT * FROM int_values) as ivt1 ON ivt1.id=pidrozdily.Opal) \
					LEFT JOIN (SELECT * FROM int_values) as ivt2 ON ivt2.id=pidrozdily.Gar_voda) \
					LEFT JOIN (SELECT * FROM int_values) as ivt3 ON ivt3.id=pidrozdily.Lich) \
					LEFT JOIN (SELECT * FROM kotelni) as kotln ON kotln.id=pidrozdily.Kotelnya_ID "; //Вкінці обов'язково пробіл
	modelPidrozdily = new QSqlQueryModel(this);
	ui.tableView_pidrozdily->setModel(modelPidrozdily);
	ui.tableView_pidrozdily->setAlternatingRowColors(true);
		// end of set table kotelni
	
		//set table taryf
	model_taryf = new QSqlRelationalTableModel(this);
	model_taryf->setTable("normat_taryf_organiz");
	model_taryf->setSort(1, Qt::AscendingOrder);
	model_taryf->setHeaderData(1, Qt::Horizontal, codec->toUnicode("Рік") );
	model_taryf->setHeaderData(2, Qt::Horizontal, codec->toUnicode("Місяць") );
	model_taryf->setHeaderData(3, Qt::Horizontal, codec->toUnicode("Варт.Гкал.ЦО") );
	model_taryf->setHeaderData(4, Qt::Horizontal, codec->toUnicode("Варт.Гкал.Гар.В.") );
	model_taryf->setHeaderData(5, Qt::Horizontal, codec->toUnicode("Варт.квадр.ЦО") );
	model_taryf->setHeaderData(6, Qt::Horizontal, codec->toUnicode("Варт.куба.Гар.В.") );
	model_taryf->setHeaderData(7, Qt::Horizontal, codec->toUnicode("Варт.Гкал.ЦО госп.") );
	model_taryf->select();
	ui.tableView_taryf->setModel(model_taryf);
	ui.tableView_taryf->setColumnHidden(0, true);
	ui.tableView_taryf->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.tableView_taryf->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableView_taryf->setItemDelegate(new UTaryf_Delegate(ui.tableView_taryf));
	ui.tableView_taryf->resizeColumnsToContents();
	ui.tableView_taryf->setAlternatingRowColors(true);
		//end of set table taryf
	
		//set table kilk_tepla
	pokazLichDateChanged();
	ui.tableWidget_pokaz_lich->setColumnHidden(0, true);
	ui.tableWidget_pokaz_lich->setColumnHidden(3, true);
	ui.tableWidget_pokaz_lich->setItemDelegate(new UPokazLichOpalDelegate(ui.tableWidget_pokaz_lich));
		//end of set table pokaz_lich_opal
	
		//set table pokaznLich gar_voda
	pokazLichGarVodaDateChanged();
	ui.tableWidget_pokaz_lich_gar_voda->setColumnHidden(0, true);
	ui.tableWidget_pokaz_lich_gar_voda->setColumnHidden(4, true);
	ui.tableWidget_pokaz_lich_gar_voda->setItemDelegate(new UPokazLichOpalDelegate(ui.tableWidget_pokaz_lich_gar_voda));
		//end of set table pokaznLich gar_voda
	
		//set table meteorologichni umovy
	meteorDateChanged();
	ui.tableWidget_meteor->setColumnHidden(0, true);
	ui.tableWidget_meteor->setItemDelegate(new UPokazLichOpalDelegate(ui.tableWidget_pokaz_lich));
		//end of set table meteorologichni umovy
	
		//set table dniOpalennya
	dniOpalQueryModel->setHeaderData(2, Qt::Horizontal, codec->toUnicode("Підприємство/Підрозділ") );
	dniOpalQueryModel->setHeaderData(3, Qt::Horizontal, codec->toUnicode("Дні опал б/л") );
	dniOpalQueryModel->setHeaderData(4, Qt::Horizontal, codec->toUnicode("Дні опал ліч") );
	dniOpalDateChanged();
	//dniOpalQueryModel->select();
	ui.tableView_dni_opal->setModel(dniOpalQueryModel);
	ui.tableView_dni_opal->setColumnHidden(0, true);
	ui.tableView_dni_opal->setColumnHidden(1, true);
	ui.tableView_dni_opal->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.tableView_dni_opal->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableView_dni_opal->setItemDelegate(new UDniOpalQueryModelDelegate(ui.tableView_dni_opal));
	ui.tableView_dni_opal->resizeColumnsToContents();
		//end of set table dniOpalennya
	
		//set table Oplata
	modelOplata->setHeaderData(1, Qt::Horizontal, codec->toUnicode("Угода") );
	modelOplata->setHeaderData(2, Qt::Horizontal, codec->toUnicode("Дата") );
	modelOplata->setHeaderData(3, Qt::Horizontal, codec->toUnicode("Сума") );
	oplataDateChanged();
	modelOplata->select();
	ui.tableView_oplata->setModel(modelOplata);
	ui.tableView_oplata->setColumnHidden(0, true);
	ui.tableView_oplata->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.tableView_oplata->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableView_oplata->setItemDelegate(new UOplata_Delegate(ui.tableView_oplata));
		//end of set table Oplata
	
		//set TAB narah_pidrozdily
	ui.tableWidget_pidrozdily_narah_pidrozdily->setColumnHidden(0, true);
	populateNarahPidrozdilyTable();
	ui.tableWidget_pidrozdily_narah_pidrozdily->setCurrentCell(0, 1);
		//end of set TAB narah_pidrozdily
	
		// set operacci table
	for (int col=0; col<ui.tableWidget_rozrahunky->columnCount(); col++){
		QColor itemColor;
		if (col==1 or col==2)
			itemColor.setRgb(168,224,251);
		else
			itemColor.setRgb(214,247,168);
		for (int row=0; row<ui.tableWidget_rozrahunky->rowCount(); row++){
			QTableWidgetItem *tableWidgetItem = new QTableWidgetItem();
			tableWidgetItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			tableWidgetItem->setBackgroundColor(itemColor);
			ui.tableWidget_rozrahunky->setItem(row, col, tableWidgetItem );
		}
	}
	//ui.tableWidget_rozrahunky->resizeColumnsToContents();
	ui.tableWidget_rozrahunky->resizeRowsToContents();
	ui.tableWidget_rozrahunky->verticalHeader()->resize(10, ui.tableWidget_rozrahunky->verticalHeader()->size().height());
	ui.tableWidget_rozrahunky->verticalHeader()->setDefaultAlignment ( Qt::AlignRight );
	
		// set operacci table for specRahunky
	for (int col=0; col<ui.tableWidget_slujb_rah_operacii->columnCount(); col++){
		QColor itemColor;
		if (col==3 or col==5)
			itemColor.setRgb(214,247,168);
		else if (col==2 or col==4)
			itemColor.setRgb(255,255,255);
		else
			itemColor.setRgb(168,224,251);
		for (int row=0; row<ui.tableWidget_rozrahunky->rowCount(); row++){
			QTableWidgetItem *tableWidgetItem = new QTableWidgetItem();
			tableWidgetItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			tableWidgetItem->setBackgroundColor(itemColor);
			ui.tableWidget_slujb_rah_operacii->setItem(row, col, tableWidgetItem );
		}
	}
	ui.tableWidget_slujb_rah_operacii->resizeRowsToContents();
	ui.tableWidget_slujb_rah_operacii->verticalHeader()->resize(10, ui.tableWidget_slujb_rah_operacii->verticalHeader()->size().height());
	ui.tableWidget_slujb_rah_operacii->verticalHeader()->setDefaultAlignment ( Qt::AlignRight );
	ui.checkBox_mor_info_spec_rah->setChecked(false);
	showOperaciiSpecRahMorInfo();

	
	//prepare first view record in organiz
	populateOrganizComboboxes();
	execOrganizSelectQuery();
	ui.spinBox_navigator->setValue( 1 ); // 1 - bo pershyi zapys
	enableNavigationButtons(1); // 1 - bo pershyi zapys
	populateOrganizForm(1); // 1 - bo pershyi zapys
	
	//prepare first view record in slujbRah
	execOrganizSlujbRahSelectQuery();
	ui.spinBox_slujb_rah_navigator->setValue( 1 ); // 1 - bo pershyi zapys
	enableSlujbRahNavigationButtons(1); // 1 - bo pershyi zapys
	populateOrganizSlujbRahForm(1); // 1 - bo pershyi zapys
	
		//ugody form connectors
	connect(ui.spinBox_navigator, SIGNAL(valueChanged (int)), this, SLOT(populateOrganizForm(int))); //  populate abonent form after spinbox_navigator val. ch.
	connect(ui.comboBox_Poshuk, SIGNAL(currentIndexChanged (int)), ui.spinBox_navigator, SLOT(setValue(int))); //spinbox_navigator val. ch. after comboBox_Poshuk val.ch.
	connect(ui.comboBox_Poshuk_ugody, SIGNAL(currentIndexChanged (int)), ui.spinBox_navigator, SLOT(setValue(int))); //spinbox_navigator val. ch. after comboBox_Poshuk_rahunok val.ch.
	connect(ui.comboBox_Poshuk_ugody, SIGNAL(currentIndexChanged (int)), this, SLOT(organizFiltrComboboxIndexChanged(int))); //spinbox_navigator val. ch. after comboBox_Poshuk_rahunok val.ch.
	connect(ui.pushButton_first, SIGNAL(clicked ()), this, SLOT(organizFormFirst())); //first button --> spinBox_navigator
	connect(ui.pushButton_previous, SIGNAL(clicked ()), this, SLOT(organizFormPrevious())); //previous button --> spinBox_navigator
	connect(ui.pushButton_next, SIGNAL(clicked ()), this, SLOT(organizFormNext())); //next button --> spinBox_navigator
	connect(ui.pushButton_last, SIGNAL(clicked ()), this, SLOT(organizFormLast())); //last button --> spinBox_navigator
	connect(ui.pushButton_new, SIGNAL(clicked ()), this, SLOT(organizFormNew())); //new button --> new Record
	connect(ui.spinBox_navigator, SIGNAL(valueChanged (int)), this, SLOT(enableNavigationButtons(int))); //enable/disable navigation buttons
	connect(ui.spinBox_year_filtr, SIGNAL(valueChanged (int)), this, SLOT(organizPopulateOperaciiTable()));
	
	connect(ui.pushButton_organiz_form_save, SIGNAL(clicked ()), this, SLOT(organizButtonSaveClicked())); //save 
	connect(ui.pushButton_organiz_form_abort, SIGNAL(clicked ()), this, SLOT(organizButtonCancelClicked())); //відмова від редагування і повернення початкових значень
	
	connect(ui.actionEditPidrozdil, SIGNAL(activated()), this, SLOT(actionEditPidrozdil_activated())); //редагування підрозділу 
	connect(ui.tableView_pidrozdily, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(actionEditPidrozdil_activated()));
	connect(ui.actionAddPidrozdil, SIGNAL(activated()), this, SLOT(actionAddPidrozdil_activated())); //додавання підрозділу
	connect(ui.actionDelPidrozdil, SIGNAL(activated()), this, SLOT(actionDelPidrozdil_activated())); //видалення підрозділу
	connect(ui.action_sqlBrowser,SIGNAL(activated()),this,SLOT(openSqlBrowser()));
	//connect(ui.tableView_pidrozdily->selectionModel(), SIGNAL(selectionChanged (const QItemSelection &, const QItemSelection &)), this, SLOT(organizPopulateOperaciiTable()));//Зміна позначення в tableView_pidrozdily
	
		//Конектори спецрахунків
	connect(ui.spinBox_slujb_rah_navigator, SIGNAL(valueChanged (int)), this, SLOT(populateOrganizSlujbRahForm(int))); //  populate abonent form after spinbox_navigator val. ch.
	connect(ui.comboBox_Poshuk_RahName, SIGNAL(currentIndexChanged (int)), ui.spinBox_slujb_rah_navigator, SLOT(setValue(int))); //spinbox_navigator val. ch. after comboBox_Poshuk val.ch.
	connect(ui.comboBox_PoshukSpecRahunok, SIGNAL(currentIndexChanged (int)), ui.spinBox_slujb_rah_navigator, SLOT(setValue(int))); //spinbox_navigator val. ch. after comboBox_Poshuk_rahunok val.ch.
	connect(ui.comboBox_PoshukSpecRahunok, SIGNAL(currentIndexChanged (int)), this, SLOT(organizSlujbRahPopulateFiltrComboboxIndexChanged(int))); //spinbox_navigator val. ch. after comboBox_Poshuk_rahunok val.ch.
	connect(ui.pushButton_slujb_rah_first, SIGNAL(clicked ()), this, SLOT(organizSlujbRahFormFirst())); //first button --> spinBox_navigator
	connect(ui.pushButton_slujb_rah_previous, SIGNAL(clicked ()), this, SLOT(organizSlujbRahFormPrevious())); //previous button --> spinBox_navigator
	connect(ui.pushButton_slujb_rah_next, SIGNAL(clicked ()), this, SLOT(organizSlujbRahFormNext())); //next button --> spinBox_navigator
	connect(ui.pushButton_slujb_rah_last, SIGNAL(clicked ()), this, SLOT(organizSlujbRahFormLast())); //last button --> spinBox_navigator
	connect(ui.pushButton_slujb_rah_new, SIGNAL(clicked ()), this, SLOT(organizSlujbRahFormNew())); //new button --> new Record
	connect(ui.spinBox_slujb_rah_navigator, SIGNAL(valueChanged (int)), this, SLOT(enableSlujbRahNavigationButtons(int))); //enable/disable navigation buttons
	connect(ui.spinBox_slujb_rah_year_filtr, SIGNAL(valueChanged (int)), this, SLOT(organizSlujbRahPopulateOperaciiTable()));
	connect(ui.checkBox_mor_info_spec_rah, SIGNAL(stateChanged(int)), this, SLOT(showOperaciiSpecRahMorInfo()));
	
	connect(ui.pushButton_organiz_form_slujb_rah_save, SIGNAL(clicked ()), this, SLOT(organizButtonSlujbRahSaveClicked())); //save 
	connect(ui.pushButton_organiz_form_slujb_rah_abort, SIGNAL(clicked ()), this, SLOT(organizButtonSlujbRahCancelClicked())); //відмова від редагування і повернення початкових значень
	
	//action connectors
	connect(ui.action_registracion, SIGNAL(activated()), this, SLOT(action_registracion_activated()));
	connect(ui.action_close_login, SIGNAL(activated()), this, SLOT(action_close_login_activated()));
	connect(ui.action_close_program, SIGNAL(activated()), this, SLOT(close())); //close form
	connect(ui.action_taryf_table, SIGNAL(activated()), this, SLOT(action_taryf_table_activated()));
	connect(ui.action_pokaz_lich_table, SIGNAL(activated()), this, SLOT(action_pokaz_lich_table_activated()));
	connect(ui.action_pokaz_lich_gar_voda, SIGNAL(activated()), this, SLOT(action_pokaz_lich_gar_voda_activated()));
	connect(ui.action_meteor_table, SIGNAL(activated()), this, SLOT(action_meteor_table_activated()));
	connect(ui.action_dni_opalennya, SIGNAL(activated()), this, SLOT(action_dni_opalennya_activated()));
	connect(ui.action_oplata, SIGNAL(activated()), this, SLOT(action_oplata_activated()));
	
	connect(ui.action_debuger, SIGNAL(activated()), this, SLOT(action_debuger_activated()));
	connect(ui.action_calc_tepl_navantaj, SIGNAL(activated()), this, SLOT(action_calc_tepl_navantaj_activated()));
	connect(ui.action_narah_opal, SIGNAL(activated()), this, SLOT(action_narah_opal_activated()));
	connect(ui.action_narah_gar_voda, SIGNAL(activated()), this, SLOT(action_narah_gar_voda_activated()));
	connect(ui.action_print_narah_gar_voda, SIGNAL(activated()), this, SLOT(action_print_narah_gar_voda_activated()));
	connect(ui.action_print_vidomist_narahuvannya_i_postuplennya_dohodiv, SIGNAL(activated()), this, SLOT(action_print_vidomist_narahuvannya_i_postuplennya_dohodiv_activated()));
	connect(ui.action_print_nakopychuvalna_vvidomist_narahuvannya, SIGNAL(activated()), this, SLOT(action_print_nakopychuvalna_vidomist_narahuvannya_activated()));
	
	connect(ui.action_calc_saldo_spec_rahunkiv, SIGNAL(activated()), this, SLOT(action_calc_saldo_spec_rahunkiv_activated()));
	connect(ui.action_pererahunok_saldo_usih_spec_rahunkiv, SIGNAL(activated()), this, SLOT(action_pererahunok_saldo_usih_spec_rahunkiv_activated()));
	connect(ui.action_print_vidomist_oplaty_za_CO, SIGNAL(activated()), this, SLOT(action_print_vidomist_oplaty_za_CO_activated()));
	connect(ui.action_print_vidomist_realizacii, SIGNAL(activated()), this, SLOT(action_print_vidomist_realizacii_activated()));
	connect(ui.action_print_rahunok, SIGNAL(activated()), this, SLOT(action_print_rahunok_activated()));  
    connect(ui.action_print_act_vykon_robit, SIGNAL(activated()), this, SLOT(action_print_act_vykon_robit_activated()));
    connect(ui.action_print_vidomist_borjnykiv_organiz, SIGNAL(activated()), this, SLOT(action_print_vidomist_borjnykiv_organiz_activated()));
	
	connect(ui.action_print_podatkova_nakladna, SIGNAL(activated()), this, SLOT(action_print_podatkova_nakladna_activated()));
	
	connect(ui.action_pererah_vhidnogo_saldo_potochnoi_ugody, SIGNAL(activated()), this, SLOT(action_pererah_vhidnogo_saldo_potochnoi_ugody_activated()));  
	connect(ui.action_pererah_vhidnyh_saldo_usih_ugod, SIGNAL(activated()), this, SLOT(action_pererah_vhidnyh_saldo_usih_ugod_activated()));  
	
	connect(ui.action_Options, SIGNAL(activated()), this, SLOT(action_Options_activated()));
	connect(ui.action_saveDatabase, SIGNAL(activated()), this, SLOT(action_saveDatabase_activated()));
	connect(ui.action_assistent,SIGNAL(activated()),this,SLOT(action_assistent_activated()));
	connect(ui.action_about,SIGNAL(activated()),this,SLOT(action_about_activated()));
	
	//Taryf table connectors
	connect(ui.pushButton_add_taryf, SIGNAL(clicked ()), this, SLOT(buttonclick_add_taryf())); // dodavannya taryf
	connect(ui.pushButton_del_taryf, SIGNAL(clicked ()), this, SLOT(buttonclick_del_taryf())); // vydalennya taryf
	
	//Pokazn_lich table connectors
	connect(ui.spinBox_month_pokaz_lich, SIGNAL(valueChanged (int)), this, SLOT(pokazLichDateChanged())); //month changed
	connect(ui.spinBox_year_pokaz_lich, SIGNAL(valueChanged (int)), this, SLOT(pokazLichDateChanged())); //year changed
	connect(ui.pushButton_pokaz_lich_save, SIGNAL(clicked ()), this, SLOT(buttonclickSavePokazLichOpal())); // save pokaz lich
	
	//Pokazn_lich_gar_voda table connectors
	connect(ui.spinBox_month_pokaz_lich_gar_voda, SIGNAL(valueChanged (int)), this, SLOT(pokazLichGarVodaDateChanged())); //month changed
	connect(ui.spinBox_year_pokaz_lich_gar_voda, SIGNAL(valueChanged (int)), this, SLOT(pokazLichGarVodaDateChanged())); //year changed
	connect(ui.pushButton_pokaz_lich_gar_voda_save, SIGNAL(clicked ()), this, SLOT(buttonclickSavePokazLichGarVodaOpal())); // save pokaz lich

	//Meteorolog connectors
	connect(ui.spinBox_month_meteor_table, SIGNAL(valueChanged (int)), this, SLOT(meteorDateChanged())); //month changed
	connect(ui.spinBox_year_meteor_table, SIGNAL(valueChanged (int)), this, SLOT(meteorDateChanged())); //year changed
	connect(ui.pushButton_meteor_table_save, SIGNAL(clicked ()), this, SLOT(buttonclickSaveMeteorOpal())); // save meteorolog
	
	//Dni opalennannya connectors
	connect(ui.spinBox_month_dni_opal, SIGNAL(valueChanged (int)), this, SLOT(dniOpalDateChanged())); //month changed
	connect(ui.spinBox_year_dni_opal, SIGNAL(valueChanged (int)), this, SLOT(dniOpalDateChanged())); //year changed
	connect(ui.tableView_dni_opal, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(dniOpalOpenEditForm(const QModelIndex &)));
	
	//Oplata table connectors
	connect(ui.spinBox_month_oplata, SIGNAL(valueChanged (int)), this, SLOT(oplataDateChanged())); //month changed
	connect(ui.spinBox_year_oplata, SIGNAL(valueChanged (int)), this, SLOT(oplataDateChanged())); //year changed
	connect(ui.pushButton_add_oplata, SIGNAL(clicked ()), this, SLOT(buttonclick_add_oplata())); // dodavannya oplata
	connect(ui.pushButton_del_oplata, SIGNAL(clicked ()), this, SLOT(buttonclick_del_oplata())); // vydalennya oplata

	//Перед внесення записів у базу даних
	connect(modelOplata, SIGNAL(beforeInsert(QSqlRecord &)), this, SLOT(organizBeforeChangeDefaultOplata(QSqlRecord &))); //beforeInsert
	
	//Відкривання позначеної угоди в додаткових таблицях
	connect(ui.tableView_oplata->selectionModel(), SIGNAL(currentRowChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(look_ugoda_info( const QModelIndex &, const QModelIndex & ))); //позначено запис --> висвітлення інформації про абонента
	connect(ui.tableView_oplata->selectionModel(), SIGNAL(currentColumnChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(activ_column_changes( const QModelIndex &, const QModelIndex &)));
	
	//Закладка нарахування по підрозділах
	connect(ui.dateEdit_narah_pidrozdily, SIGNAL(dateChanged(const QDate &)), this, SLOT(showNarahPidrozdilInfo())); //date changed
	connect(ui.pushButton_reload_narah_pidrozdily, SIGNAL(clicked ()), this, SLOT(reloadNarahPidrozdilInfo()));
	connect(ui.tableWidget_dni_opal_narah_pidrozdily, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(showDniOpalennyaForm()));
	connect(ui.pushButton_do_narah, SIGNAL(clicked ()), this, SLOT(narahNarahPidrozdily()));
	connect(ui.pushButton_print_rahunok, SIGNAL(clicked ()), this, SLOT(printRahunokNarahPidrozdily()));
	connect(ui.pushButton_save_narah_pidrozdily, SIGNAL(clicked ()), this, SLOT(saveNarahPidrozdilyZnach()));
	connect(ui.tableWidget_pidrozdily_narah_pidrozdily, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(showPidrozdilInfo()));
	
	connect(dateEdit_PotochnaData, SIGNAL(dateChanged(const QDate &)), this, SLOT(curDateChanged()));
	connect(calculatorButton, SIGNAL(clicked ()), this, SLOT(calculatorButton_clicked()));
	
	populateControlObjectsHash();
	action_close_login_activated();
	
	action_registracion_activated();
}
//------------------------------------------------------------
void tke_MainWindow_org::execOrganizSelectQuery()
{
	disconnect(ui.comboBox_Poshuk, SIGNAL(currentIndexChanged (int)), ui.spinBox_navigator, SLOT(setValue(int)));
	disconnect(ui.comboBox_Poshuk_ugody, SIGNAL(currentIndexChanged (int)), ui.spinBox_navigator, SLOT(setValue(int)));
	
	orgSelectQuery->clear();
	orgSelectQuery->exec("SELECT * FROM organizacii ORDER BY Nazva");
	int query_size=0;
	ui.comboBox_Poshuk->clear();
	ui.comboBox_Poshuk->addItem("");
	ui.comboBox_Poshuk_ugody->clear();
	ui.comboBox_Poshuk_ugody->addItem("");
	orgSelectQuery->seek( -1 );
	abonentsList.clear();
	while (orgSelectQuery->next()){
		query_size++;
		xRecord = orgSelectQuery->record();
		ui.comboBox_Poshuk->addItem(xRecord.value("Nazva").toString() + " - " +	xRecord.value("Ugoda").toString() );
		ui.comboBox_Poshuk_ugody->addItem(xRecord.value("Ugoda").toString());
		abonentsList << xRecord.value("Ugoda").toInt();
	}
	ui.spinBox_navigator->setRange( 1, query_size );
	ui.label_max_rows_organiz->setText(codec->toUnicode("із ") + QVariant(query_size).toString() + codec->toUnicode(".") );
	connect(ui.comboBox_Poshuk, SIGNAL(currentIndexChanged (int)), ui.spinBox_navigator, SLOT(setValue(int)));
	connect(ui.comboBox_Poshuk_ugody, SIGNAL(currentIndexChanged (int)), ui.spinBox_navigator, SLOT(setValue(int)));
}
//------------------------------------------------------------
void tke_MainWindow_org::populateOrganizComboboxes()
{
	QSqlQuery *query = new QSqlQuery;
	//Комбо джерела фінансування
	query->exec("SELECT * FROM finans_from ORDER BY Finans_name");
	query->seek(-1);
	finansList.clear();
	ui.comboBox_Finans->clear();
	finansList << 0;
	ui.comboBox_Finans->addItem("");
	while (query->next()){
		xRecord = query->record();
		finansList << xRecord.value("id").toInt();
		ui.comboBox_Finans->addItem(xRecord.value("Finans_name").toString());
	}
	
	//Комбо призначення організації
	query->exec("SELECT * FROM pryznach_org ORDER BY Pryznach");
	query->seek(-1);
	pryznachList.clear();
	ui.comboBox_Pryznach->clear();
	pryznachList << 0;
	ui.comboBox_Pryznach->addItem("");
	while (query->next()){
		xRecord = query->record();
		pryznachList << xRecord.value("id").toInt();
		ui.comboBox_Pryznach->addItem(xRecord.value("Pryznach").toString());
	}
	
	//Комбо банку
	query->exec("SELECT * FROM banky ORDER BY Nazva");
	query->seek(-1);
	bankyList.clear();
	ui.comboBox_Bank->clear();
	bankyList << 0;
	ui.comboBox_Bank->addItem("");
	while (query->next()){
		xRecord = query->record();
		bankyList << xRecord.value("id").toInt();
		ui.comboBox_Bank->addItem(xRecord.value("Nazva").toString() + " " + xRecord.value("Mfo").toString());
	}
	
	//Комбо пошуку спеціальних угод
	query->exec("SELECT id, name FROM rahunok_type ORDER BY name");
	slujbRahunkyTypeList.clear();
	slujbRahunkyTypeList << 0;
	ui.comboBox_Rahunok_type->addItem("");
	while (query->next()){
		slujbRahunkyTypeList << query->value(0).toInt();
		ui.comboBox_Rahunok_type->addItem(query->value(1).toString());
	}
	
	delete query;
}
//------------------------------------------------------------
void tke_MainWindow_org::organizConnectEditres()
{
	connect(ui.spinBox_Ugoda, SIGNAL(valueChanged (int)), this, SLOT(organizFormRecordEditing()));
	connect(ui.lineEdit_Nazva, SIGNAL(textEdited (const QString &)), this, SLOT(organizFormRecordEditing()));
	connect(ui.lineEdit_Ur_adresa, SIGNAL(textEdited (const QString &)), this, SLOT(organizFormRecordEditing()));
	connect(ui.lineEdit_Edrpou, SIGNAL(textEdited (const QString &)), this, SLOT(organizFormRecordEditing()));
	connect(ui.comboBox_Bank, SIGNAL(currentIndexChanged(int)), this, SLOT(organizFormRecordEditing()));
	connect(ui.lineEdit_Rozr_Rah, SIGNAL(textEdited (const QString &)), this, SLOT(organizFormRecordEditing()));
	connect(ui.lineEdit_Vidpovidaln, SIGNAL(textEdited (const QString &)), this, SLOT(organizFormRecordEditing()));
	connect(ui.lineEdit_Tel1, SIGNAL(textEdited (const QString &)), this, SLOT(organizFormRecordEditing()));
	connect(ui.lineEdit_Tel2, SIGNAL(textEdited (const QString &)), this, SLOT(organizFormRecordEditing()));
	connect(ui.comboBox_Finans, SIGNAL(currentIndexChanged (int)), this, SLOT(organizFormRecordEditing()));
	connect(ui.comboBox_Pryznach, SIGNAL(currentIndexChanged (int)), this, SLOT(organizFormRecordEditing()));
	connect(ui.dateEdit_date_ugody, SIGNAL(dateChanged (const QDate &)), this, SLOT(organizFormRecordEditing()));
	connect(ui.lineEdit_nomerSvidPlatnPod, SIGNAL(textEdited (const QString &)), this, SLOT(organizFormRecordEditing()));
	connect(ui.lineEdit_indyvidPodNomer, SIGNAL(textEdited (const QString &)), this, SLOT(organizFormRecordEditing()));
}
//------------------------------------------------------------
void tke_MainWindow_org::organizDisConnectEditres()
{
	disconnect(ui.spinBox_Ugoda, SIGNAL(valueChanged (int)), this, SLOT(organizFormRecordEditing()));
	disconnect(ui.lineEdit_Nazva, SIGNAL(textEdited (const QString &)), this, SLOT(organizFormRecordEditing()));
	disconnect(ui.lineEdit_Ur_adresa, SIGNAL(textEdited (const QString &)), this, SLOT(organizFormRecordEditing()));
	disconnect(ui.lineEdit_Edrpou, SIGNAL(textEdited (const QString &)), this, SLOT(organizFormRecordEditing()));
	disconnect(ui.comboBox_Bank, SIGNAL(currentIndexChanged(int)), this, SLOT(organizFormRecordEditing()));
	disconnect(ui.lineEdit_Rozr_Rah, SIGNAL(textEdited (const QString &)), this, SLOT(organizFormRecordEditing()));
	disconnect(ui.lineEdit_Vidpovidaln, SIGNAL(textEdited (const QString &)), this, SLOT(organizFormRecordEditing()));
	disconnect(ui.lineEdit_Tel1, SIGNAL(textEdited (const QString &)), this, SLOT(organizFormRecordEditing()));
	disconnect(ui.lineEdit_Tel2, SIGNAL(textEdited (const QString &)), this, SLOT(organizFormRecordEditing()));
	disconnect(ui.comboBox_Finans, SIGNAL(currentIndexChanged (int)), this, SLOT(organizFormRecordEditing()));
	disconnect(ui.comboBox_Pryznach, SIGNAL(currentIndexChanged (int)), this, SLOT(organizFormRecordEditing()));
	disconnect(ui.dateEdit_date_ugody, SIGNAL(dateChanged (const QDate &)), this, SLOT(organizFormRecordEditing()));
	disconnect(ui.lineEdit_nomerSvidPlatnPod, SIGNAL(textEdited (const QString &)), this, SLOT(organizFormRecordEditing()));
	disconnect(ui.lineEdit_indyvidPodNomer, SIGNAL(textEdited (const QString &)), this, SLOT(organizFormRecordEditing()));
}
//------------------------------------------------------------
void tke_MainWindow_org::organizFormRecordEditing()
{
	ui.line->setPalette ( *palette );
	ui.line_2->setPalette ( *palette );
	ui.line_3->setPalette ( *palette );
	ui.line_4->setPalette ( *palette );
	ui.line_5->setPalette ( *palette );
	ui.pushButton_new->setEnabled( true );
	ui.pushButton_organiz_form_save->setEnabled( true );
	record_edited = 1;
}
//------------------------------------------------------------
void tke_MainWindow_org::organizFormRecordEndEditing()
{
	ui.line->setPalette ( *default_palette );
	ui.line_2->setPalette ( *default_palette );
	ui.line_3->setPalette ( *default_palette );
	ui.line_4->setPalette ( *default_palette );
	ui.line_5->setPalette ( *default_palette );
	ui.pushButton_organiz_form_save->setEnabled( false );
	record_edited = 0;
}
//------------------------------------------------------------
void tke_MainWindow_org::organizFormUpdateRecord()
{
	QSqlTableModel *updateSqlModel = new QSqlTableModel();
	updateSqlModel->setTable("organizacii");
	updateSqlModel->setFilter("ID = " + orgSelectQuery->value(0).toString());
	updateSqlModel->select();
	if (updateSqlModel->rowCount() == 1) {
		xRecord = updateSqlModel->record(0);
		xRecord.setValue("Ugoda", ui.spinBox_Ugoda->value());
		xRecord.setValue("Nazva", ui.lineEdit_Nazva->text());
		xRecord.setValue("Adresa", ui.lineEdit_Ur_adresa->text());
		xRecord.setValue("Edrpou", ui.lineEdit_Edrpou->text());
		xRecord.setValue("Bank_ID", bankyList.at(ui.comboBox_Bank->currentIndex()));
		xRecord.setValue("Rozr_rah", ui.lineEdit_Rozr_Rah->text());
		xRecord.setValue("Tel1", ui.lineEdit_Tel1->text());
		xRecord.setValue("Tel2", ui.lineEdit_Tel2->text());
		xRecord.setValue("Vidpovid", ui.lineEdit_Vidpovidaln->text());
		xRecord.setValue("Finans", finansList.at(ui.comboBox_Finans->currentIndex()));
		xRecord.setValue("Pryznach", pryznachList.at(ui.comboBox_Pryznach->currentIndex()));
		xRecord.setValue("Date_ugody", ui.dateEdit_date_ugody->date());
		xRecord.setValue("Num_svid_pdv", ui.lineEdit_nomerSvidPlatnPod->text());
		xRecord.setValue("Ind_pod_nomer", ui.lineEdit_indyvidPodNomer->text());
		
		updateSqlModel->setRecord(0, xRecord);
		updateSqlModel->submitAll();
	}
	delete updateSqlModel;
}
//------------------------------------------------------------
void tke_MainWindow_org::organizFormInsertRecord()
{
	insertSqlModel->insertRows(0,1);

	//Читання наступного значення лічильника для таблиці organizacii
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT max(ID) FROM organizacii");
	query->seek(0);
	int iterId = query->value(0).toInt();
	delete query;
	xRecord = insertSqlModel->record(0);
	
	xRecord.setValue("id", iterId+1);
	xRecord.setValue("Ugoda", ui.spinBox_Ugoda->value());
	xRecord.setValue("Nazva", ui.lineEdit_Nazva->text());
	xRecord.setValue("Adresa", ui.lineEdit_Ur_adresa->text());
	xRecord.setValue("Edrpou", ui.lineEdit_Edrpou->text());
	xRecord.setValue("Bank_ID", bankyList.at(ui.comboBox_Bank->currentIndex()));
	xRecord.setValue("Rozr_rah", ui.lineEdit_Rozr_Rah->text());
	xRecord.setValue("Tel1", ui.lineEdit_Tel1->text());
	xRecord.setValue("Tel2", ui.lineEdit_Tel2->text());
	xRecord.setValue("Vidpovid", ui.lineEdit_Vidpovidaln->text());
	xRecord.setValue("Finans", finansList.at(ui.comboBox_Finans->currentIndex()));
	xRecord.setValue("Pryznach", pryznachList.at(ui.comboBox_Pryznach->currentIndex()));
	xRecord.setValue("Date_ugody", ui.dateEdit_date_ugody->date());
	xRecord.setValue("Num_svid_pdv", ui.lineEdit_nomerSvidPlatnPod->text());
	xRecord.setValue("Ind_pod_nomer", ui.lineEdit_indyvidPodNomer->text());
	
	insertSqlModel->setRecord(0, xRecord);
	
	if (insertSqlModel->submitAll()){
	QMessageBox::information(this,
				codec->toUnicode("!!! Додавання запису !!!"),
				codec->toUnicode("Запис успішно додано до бази даних."),
				QMessageBox::Ok,
				QMessageBox::NoButton);
	}
	else
		QMessageBox::critical(this,
				codec->toUnicode("!!! Невдале додавання запису !!!"),
				codec->toUnicode("Програма не змогла додати запис до бази даних./nПричина - ")
								+ insertSqlModel->lastError().text(),
				QMessageBox::Ok,
				QMessageBox::NoButton);
}
//------------------------------------------------------------
void tke_MainWindow_org::organizFormFirst()
{
	ui.spinBox_navigator->setValue( 1 );
}
//------------------------------------------------------------
void tke_MainWindow_org::organizFormPrevious()
{
	ui.spinBox_navigator->setValue( ui.spinBox_navigator->value()-1 );
}
//------------------------------------------------------------
void tke_MainWindow_org::organizFormNext()
{
	ui.spinBox_navigator->setValue( ui.spinBox_navigator->value()+1 );
}
//------------------------------------------------------------
void tke_MainWindow_org::organizFormLast()
{
	ui.spinBox_navigator->setValue( ui.spinBox_navigator->maximum() );
}
//------------------------------------------------------------
void tke_MainWindow_org::organizFormNew()
{
#ifndef TKE_DEMO
	organizDisConnectEditres();
	disconnect(ui.spinBox_navigator, SIGNAL(valueChanged (int)), this, SLOT(populateOrganizForm(int)));

	ui.pushButton_new->setEnabled( FALSE );
	if ( (new_record==1) && (record_edited==1) ){
		organizFormRecordEndEditing();
		organizFormInsertRecord();
		execOrganizSelectQuery();
	}
	ui.spinBox_navigator->setMaximum( ui.spinBox_navigator->maximum()+1 );
	ui.spinBox_navigator->setValue( ui.spinBox_navigator->maximum() );
	//zadannya znachen za zamovchuvannyam
	QSqlQuery * query = new QSqlQuery;
	query->exec("SELECT max(Ugoda) FROM organizacii");
	if (query->seek(0))
		ui.spinBox_Ugoda->setValue(query->value(0).toInt()+1);
	else
		ui.spinBox_Ugoda->setValue(1);
	ui.lineEdit_Nazva->setText("");
	ui.lineEdit_Ur_adresa->setText("");
	ui.lineEdit_Edrpou->setText("");
	ui.comboBox_Bank->setCurrentIndex(0);
	ui.lineEdit_Rozr_Rah->setText("");
	ui.lineEdit_Vidpovidaln->setText("");
	ui.lineEdit_Tel1->setText("");
	ui.lineEdit_Tel2->setText("");
	ui.dateEdit_date_ugody->setDate(date);
	ui.comboBox_Finans->setCurrentIndex(0);
	ui.comboBox_Pryznach->setCurrentIndex(0);
	
	ui.lineEdit_nomerSvidPlatnPod->setText("");
	ui.lineEdit_indyvidPodNomer->setText("");
	
	organizConnectEditres();
	connect(ui.spinBox_navigator, SIGNAL(valueChanged (int)), this, SLOT(populateOrganizForm(int)));
	new_record = 1;
	record_edited=0;
	delete query;
#else
	QMessageBox::information(this,
				"!!! Демо-версія !!!",
				"З метою економії розміру у демо-версії не доступна ця функція.",
				QMessageBox::Ok,
				QMessageBox::NoButton);
#endif
}
//------------------------------------------------------------
void tke_MainWindow_org::organizButtonSaveClicked()
{
	populateOrganizForm( ui.spinBox_navigator->value() );
}
//------------------------------------------------------------
void tke_MainWindow_org::organizButtonCancelClicked()
{
	organizFormRecordEndEditing();
	populateOrganizForm( ui.spinBox_navigator->value() );
}
//------------------------------------------------------------
void tke_MainWindow_org::organizPopulateOperaciiTable()
{
	QModelIndexList indexList = ui.tableView_pidrozdily->selectionModel()->selectedRows(0);
	QString filtrString="";
	bool needOr = false;
	for (int i=0; i<indexList.count();i++){
		if (needOr)
			filtrString += " or ";
		filtrString += "pidrozdil=" + indexList.at(i).data().toString();
		needOr = true;
	}
	if (filtrString.length() == 0){
		filtrString += "pidrozdil=" + modelPidrozdily->record(ui.tableView_pidrozdily->currentIndex().row()).value("id").toString();
	}
	
	// Відладчик стрічки фільтру ф кфлькості викликів функції
	//QMessageBox::information(this,
	//			codec->toUnicode("!!! Значення фільтру !!!"),
	//			codec->toUnicode("Фільтр - ")+ filtrString,
	//			QMessageBox::Ok,
	//			QMessageBox::NoButton);
	
	int table_month;
	//Виведення інформації про нарахування
	QSqlQuery * operaciiQuery = new QSqlQuery();
	operaciiQuery->exec("SELECT Month, sum(Narah_opal), sum(Narah_voda) FROM operacii WHERE Year=" 
						+ QVariant(ui.spinBox_year_filtr->value()).toString() + " and (" + filtrString + ") GROUP BY Month");
	clearOperaciiTable();
	while (operaciiQuery->next()){
		table_month = operaciiQuery->value(0).toInt();
		ui.tableWidget_rozrahunky->item( table_month-1, 1)->setText( uMToStr2(operaciiQuery->value(1).toDouble()));
		ui.tableWidget_rozrahunky->item( table_month-1, 2)->setText( uMToStr2(operaciiQuery->value(2).toDouble()));
	}
	//Виведення вхідних сальдо
	operaciiQuery->exec("SELECT CurDate, Vh_Saldo FROM saldo WHERE year(CurDate)=" 
						+ QVariant(ui.spinBox_year_filtr->value()).toString()+" \
						and Ugoda_id="+QVariant(ui.spinBox_Ugoda->value()).toString());
	while (operaciiQuery->next()){
		table_month = operaciiQuery->value(0).toDate().month();
		ui.tableWidget_rozrahunky->item( table_month-1, 0)->setText( uMToStr2(operaciiQuery->value(1).toDouble()));
	}
	//Виведення оплати
	operaciiQuery->exec("SELECT month(Pererah_date), sum(Suma) FROM oplata WHERE year(Pererah_date)=" 
						+ QVariant(ui.spinBox_year_filtr->value()).toString()+" \
						and Organiz_id="+QVariant(ui.spinBox_Ugoda->value()).toString() + " GROUP BY month(Pererah_date)");
	while (operaciiQuery->next()){
		table_month = operaciiQuery->value(0).toInt();
		ui.tableWidget_rozrahunky->item( table_month-1, 3)->setText( uMToStr2(operaciiQuery->value(1).toDouble()));
	}
	
	//Обчислення сумарних величин
	int col, row;
	double sum;
	for (col=1; col<ui.tableWidget_rozrahunky->columnCount()-1; col++){  //Обчислення сум по нарахуваннях стовбцях таблиці
		sum = 0;
		for (row=0; row<ui.tableWidget_rozrahunky->rowCount()-1; row++){
			sum += ui.tableWidget_rozrahunky->item(row, col)->text().toDouble();
		}
		ui.tableWidget_rozrahunky->item(ui.tableWidget_rozrahunky->rowCount()-1, col)->setText(uMToStr2(sum));
	}
	for (row=0; row<ui.tableWidget_rozrahunky->rowCount()-1; row++){
		sum  = ui.tableWidget_rozrahunky->item(row, 0)->text().toDouble();
		sum += ui.tableWidget_rozrahunky->item(row, 1)->text().toDouble();
		sum += ui.tableWidget_rozrahunky->item(row, 2)->text().toDouble();
		sum -= ui.tableWidget_rozrahunky->item(row, 3)->text().toDouble();
		
		ui.tableWidget_rozrahunky->item(row, 4)->setText(uMToStr2(sum));
	}
	
	filtrString.clear();
	delete operaciiQuery;
}
//------------------------------------------------------------
void tke_MainWindow_org::organizBeforeChangeDefaultOplata(QSqlRecord & record)
{
	if (!record.isNull("Pererah_date"))
		d_Perevid_Date = record.value("Pererah_date").toDate();
	
	record.setValue("id", nextLichylnyk("oplata"));
}
//------------------------------------------------------------
void tke_MainWindow_org::clearOperaciiTable()
{
	for (int col=0; col<ui.tableWidget_rozrahunky->columnCount()-1; col++)
		for (int row=0; row<ui.tableWidget_rozrahunky->rowCount()-1; row++)
			ui.tableWidget_rozrahunky->item(row, col)->setText("-");
}
//------------------------------------------------------------
void tke_MainWindow_org::organizFiltrComboboxIndexChanged(int index)
{
	disconnect(ui.comboBox_Poshuk_ugody, SIGNAL(currentIndexChanged (int)), ui.spinBox_navigator, SLOT(setValue(int))); //Обов'язковий
	disconnect(ui.comboBox_Poshuk_ugody, SIGNAL(currentIndexChanged (int)), this, SLOT(organizFiltrComboboxIndexChanged(int)));
	
	ui.comboBox_Poshuk_ugody->clearEditText();
	
	connect(ui.comboBox_Poshuk_ugody, SIGNAL(currentIndexChanged (int)), ui.spinBox_navigator, SLOT(setValue(int))); //Обов'язковий
	connect(ui.comboBox_Poshuk_ugody, SIGNAL(currentIndexChanged (int)), this, SLOT(organizFiltrComboboxIndexChanged(int)));
}
void tke_MainWindow_org::populateOrganizForm( int seek_query )
{
	organizDisConnectEditres(); //Обов'язковва
	disconnect(ui.spinBox_navigator, SIGNAL(valueChanged (int)), this, SLOT(populateOrganizForm(int)));
	disconnect(ui.tableView_pidrozdily->selectionModel(), SIGNAL(selectionChanged (const QItemSelection &, const QItemSelection &)), this, SLOT(organizPopulateOperaciiTable()));
	
	if ( (new_record==1) && (record_edited==1) ){ // novyi zapys, yakyi bulo redagovano
		organizFormRecordEndEditing();
		organizFormInsertRecord();
		execOrganizSelectQuery();
		new_record = 0;
	}
	else if ( record_edited==1 ){ // zmina zapysu
		organizFormRecordEndEditing();
		organizFormUpdateRecord();
		execOrganizSelectQuery();
		ui.spinBox_navigator->setValue(seek_query);
		enableNavigationButtons(ui.spinBox_navigator->value());
	}
	else if ( (new_record==1) && (record_edited==0) ){
		execOrganizSelectQuery();
		enableNavigationButtons(ui.spinBox_navigator->value());
		ui.pushButton_new->setEnabled( TRUE );
		new_record = 0;
	}
	organizDisConnectEditres();
	
	orgSelectQuery->seek( seek_query-1 );
	xRecord = orgSelectQuery->record();
	//Заповнення полів
	ui.spinBox_Ugoda->setValue(xRecord.value("Ugoda").toInt());
	ui.lineEdit_Nazva->setText(xRecord.value("Nazva").toString());
	ui.lineEdit_Ur_adresa->setText(xRecord.value("Adresa").toString());
	ui.lineEdit_Edrpou->setText(xRecord.value("Edrpou").toString());
	ui.comboBox_Bank->setCurrentIndex(bankyList.indexOf(xRecord.value("Bank_ID").toInt()));
	ui.lineEdit_Rozr_Rah->setText(xRecord.value("Rozr_rah").toString());
	ui.lineEdit_Vidpovidaln->setText(xRecord.value("Vidpovid").toString());
	ui.lineEdit_Tel1->setText(xRecord.value("Tel1").toString());
	ui.lineEdit_Tel2->setText(xRecord.value("Tel2").toString());
	ui.comboBox_Finans->setCurrentIndex(finansList.indexOf(xRecord.value("Finans").toInt()));
	ui.comboBox_Pryznach->setCurrentIndex(pryznachList.indexOf(xRecord.value("Pryznach").toInt()));
	ui.dateEdit_date_ugody->setDate(xRecord.value("Date_ugody").toDate());
	
	ui.lineEdit_nomerSvidPlatnPod->setText(xRecord.value("Num_svid_pdv").toString());
	ui.lineEdit_indyvidPodNomer->setText(xRecord.value("Ind_pod_nomer").toString());
	
	//populate pidrozdily view
	modelPidrozdily->setQuery(modelQueryStr + " WHERE Ugoda="+QVariant(ui.spinBox_Ugoda->value()).toString()+" ORDER BY pidrozdily.Name");
	if (modelPidrozdily->rowCount() == 0){
		__insertPidrozdilRecord();
		modelPidrozdily->setQuery(modelQueryStr + " WHERE Ugoda="+QVariant(ui.spinBox_Ugoda->value()).toString()+" ORDER BY pidrozdily.Name");
	}
	modelPidrozdily->setHeaderData(2, Qt::Horizontal, codec->toUnicode("Назва") );
	modelPidrozdily->setHeaderData(3, Qt::Horizontal, codec->toUnicode("Опал.") );
	modelPidrozdily->setHeaderData(4, Qt::Horizontal, codec->toUnicode("Г.Вода") );
	modelPidrozdily->setHeaderData(5, Qt::Horizontal, codec->toUnicode("Площа") );
	modelPidrozdily->setHeaderData(6, Qt::Horizontal, codec->toUnicode("Об'єм") );
	modelPidrozdily->setHeaderData(7, Qt::Horizontal, codec->toUnicode("Тепл.хар.") );
	modelPidrozdily->setHeaderData(8, Qt::Horizontal, codec->toUnicode("Внутр. T") );
	modelPidrozdily->setHeaderData(9, Qt::Horizontal, codec->toUnicode("Тепл.нав.") );
	modelPidrozdily->setHeaderData(10, Qt::Horizontal, codec->toUnicode("Лічильник") );
	modelPidrozdily->setHeaderData(11, Qt::Horizontal, codec->toUnicode("Коеф.ліч.") );
	modelPidrozdily->setHeaderData(12, Qt::Horizontal, codec->toUnicode("Котельня") );
	ui.tableView_pidrozdily->setColumnHidden(0, true);
	ui.tableView_pidrozdily->setColumnHidden(1, true);
	ui.tableView_pidrozdily->resizeColumnsToContents();
	ui.tableView_pidrozdily->resizeRowsToContents();
	
	ui.tableView_pidrozdily->selectAll();
	organizPopulateOperaciiTable();
	
	//Обов'язкові
	connect(ui.tableView_pidrozdily->selectionModel(), SIGNAL(selectionChanged (const QItemSelection &, const QItemSelection &)), this, SLOT(organizPopulateOperaciiTable()));
	connect(ui.spinBox_navigator, SIGNAL(valueChanged (int)), this, SLOT(populateOrganizForm(int)));
	organizConnectEditres();	//Обов'язкова
}
//------------------------------------------------------------
void tke_MainWindow_org::enableNavigationButtons(int index)
{
	ui.pushButton_first->setEnabled ( TRUE );
	ui.pushButton_previous->setEnabled ( TRUE );
	ui.pushButton_next->setEnabled ( TRUE );
	ui.pushButton_last->setEnabled ( TRUE );
	if ( index < 2 ){
		ui.pushButton_first->setEnabled ( FALSE );
		ui.pushButton_previous->setEnabled ( FALSE );
	}
	if ( index >= ui.spinBox_navigator->maximum() ){
		ui.pushButton_next->setEnabled ( FALSE );
		ui.pushButton_last->setEnabled ( FALSE );
	}
}
//------------------------------------------------------------
void tke_MainWindow_org::execOrganizSlujbRahSelectQuery()
{
	disconnect(ui.comboBox_Poshuk_RahName, SIGNAL(currentIndexChanged (int)), ui.spinBox_slujb_rah_navigator, SLOT(setValue(int)));
	disconnect(ui.comboBox_PoshukSpecRahunok, SIGNAL(currentIndexChanged (int)), ui.spinBox_slujb_rah_navigator, SLOT(setValue(int)));
	
	specRahSelectQuery->clear();
	specRahSelectQuery->exec("SELECT * FROM slujb_rahunky ORDER BY Rahunok_name");
	int query_size=0;
	ui.comboBox_Poshuk_RahName->clear();
	ui.comboBox_Poshuk_RahName->addItem("");
	ui.comboBox_PoshukSpecRahunok->clear();
	ui.comboBox_PoshukSpecRahunok->addItem("");
	specRahSelectQuery->seek( -1 );
	slujbRahList.clear();
	while (specRahSelectQuery->next()){
		query_size++;
		xRecord = specRahSelectQuery->record();
		ui.comboBox_Poshuk_RahName->addItem(xRecord.value("Rahunok_name").toString() + " //" + xRecord.value("Rahunok").toString() + "//" );
		ui.comboBox_PoshukSpecRahunok->addItem(xRecord.value("Rahunok").toString());
		slujbRahList << xRecord.value("Rahunok").toInt();
	}
	ui.spinBox_slujb_rah_navigator->setRange( 1, query_size );
	ui.label_max_rows_slujb_rah_organiz->setText(codec->toUnicode("із ") + QVariant(query_size).toString() + codec->toUnicode(".") );
	connect(ui.comboBox_Poshuk_RahName, SIGNAL(currentIndexChanged (int)), ui.spinBox_slujb_rah_navigator, SLOT(setValue(int)));
	connect(ui.comboBox_PoshukSpecRahunok, SIGNAL(currentIndexChanged (int)), ui.spinBox_slujb_rah_navigator, SLOT(setValue(int)));
}
//------------------------------------------------------------
void tke_MainWindow_org::organizSlujbRahConnectEditres()
{
	connect(ui.spinBox_Rahunok, SIGNAL(valueChanged (int)), this, SLOT(organizSlujbRahFormRecordEditing()));
	connect(ui.lineEdit_RahunokName, SIGNAL(textEdited(const QString &)), this, SLOT(organizSlujbRahFormRecordEditing()));
	connect(ui.comboBox_Rahunok_type, SIGNAL(currentIndexChanged(int)), this, SLOT(organizSlujbRahFormRecordEditing()));
}
//------------------------------------------------------------
void tke_MainWindow_org::organizSlujbRahDisConnectEditres()
{
	disconnect(ui.spinBox_Rahunok, SIGNAL(valueChanged (int)), this, SLOT(organizSlujbRahFormRecordEditing()));
	disconnect(ui.lineEdit_RahunokName, SIGNAL(textEdited(const QString &)), this, SLOT(organizSlujbRahFormRecordEditing()));
	disconnect(ui.comboBox_Rahunok_type, SIGNAL(currentIndexChanged(int)), this, SLOT(organizSlujbRahFormRecordEditing()));
}
//------------------------------------------------------------
//------------------------------------------------------------
void tke_MainWindow_org::organizSlujbRahFormRecordEditing()
{
	ui.line_slujb_1->setPalette ( *palette );
	ui.line_slujb_2->setPalette ( *palette );
	ui.line_slujb_3->setPalette ( *palette );

	ui.pushButton_slujb_rah_new->setEnabled( true );
	ui.pushButton_organiz_form_slujb_rah_save->setEnabled( true );
	record_slujbRah_edited = 1;
}
//------------------------------------------------------------
void tke_MainWindow_org::organizSlujbRahFormRecordEndEditing()
{
	ui.line_slujb_1->setPalette ( *default_palette );
	ui.line_slujb_2->setPalette ( *default_palette );
	ui.line_slujb_3->setPalette ( *default_palette );
	ui.pushButton_organiz_form_slujb_rah_save->setEnabled( false );
	record_slujbRah_edited = 0;
}
//------------------------------------------------------------
void tke_MainWindow_org::organizSlujbRahFormUpdateRecord()
{
	QSqlTableModel *updateSqlModel = new QSqlTableModel();
	updateSqlModel->setTable("slujb_rahunky");
	updateSqlModel->setFilter("ID = " + specRahSelectQuery->value(0).toString());
	updateSqlModel->select();
	if (updateSqlModel->rowCount() == 1) {
		xRecord = updateSqlModel->record(0);
		xRecord.setValue("Rahunok", ui.spinBox_Rahunok->value());
		xRecord.setValue("Rahunok_name", ui.lineEdit_RahunokName->text());
		xRecord.setValue("Rahunok_type_id", slujbRahunkyTypeList.at(ui.comboBox_Rahunok_type->currentIndex()));

		updateSqlModel->setRecord(0, xRecord);
		updateSqlModel->submitAll();
	}
	delete updateSqlModel;
}
//------------------------------------------------------------
void tke_MainWindow_org::organizSlujbRahFormInsertRecord()
{
	QSqlTableModel *insertSlujbRahSqlModel = new QSqlTableModel;
	insertSlujbRahSqlModel->setTable("slujb_rahunky");
	insertSlujbRahSqlModel->insertRows(0,1);
	

	//Читання наступного значення лічильника для таблиці organizacii
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT max(ID) FROM slujb_rahunky");
	query->seek(0);
	int iterId = query->value(0).toInt();
	delete query;
	xRecord = insertSlujbRahSqlModel->record(0);
	
	xRecord.setValue("id", iterId+1);
	xRecord.setValue("Rahunok", ui.spinBox_Rahunok->value());
	xRecord.setValue("Rahunok_name", ui.lineEdit_RahunokName->text());
	xRecord.setValue("Rahunok_type_id", slujbRahunkyTypeList.at(ui.comboBox_Rahunok_type->currentIndex()));
	
	insertSlujbRahSqlModel->setRecord(0, xRecord);
	
	if (insertSlujbRahSqlModel->submitAll()){
	QMessageBox::information(this,
				codec->toUnicode("!!! Додавання запису !!!"),
				codec->toUnicode("Запис успішно додано до бази даних."),
				QMessageBox::Ok,
				QMessageBox::NoButton);
	}
	else
		QMessageBox::critical(this,
				codec->toUnicode("!!! Невдале додавання запису !!!"),
				codec->toUnicode("Програма не змогла додати запис до бази даних./nПричина - ")
								+ insertSlujbRahSqlModel->lastError().text(),
				QMessageBox::Ok,
				QMessageBox::NoButton);
	delete insertSlujbRahSqlModel;
}
//------------------------------------------------------------
void tke_MainWindow_org::organizSlujbRahFormFirst()
{
	ui.spinBox_slujb_rah_navigator->setValue( 1 );
}
//------------------------------------------------------------
void tke_MainWindow_org::organizSlujbRahFormPrevious()
{
	ui.spinBox_slujb_rah_navigator->setValue( ui.spinBox_slujb_rah_navigator->value()-1 );
}
//------------------------------------------------------------
void tke_MainWindow_org::organizSlujbRahFormNext()
{
	ui.spinBox_slujb_rah_navigator->setValue( ui.spinBox_slujb_rah_navigator->value()+1 );
}
//------------------------------------------------------------
void tke_MainWindow_org::organizSlujbRahFormLast()
{
	ui.spinBox_slujb_rah_navigator->setValue( ui.spinBox_slujb_rah_navigator->maximum() );
}
//------------------------------------------------------------
void tke_MainWindow_org::organizSlujbRahFormNew()
{
#ifndef TKE_DEMO
	organizDisConnectEditres();
	disconnect(ui.spinBox_slujb_rah_navigator, SIGNAL(valueChanged (int)), this, SLOT(populateOrganizForm(int)));

	ui.pushButton_new->setEnabled( FALSE );
	if ( (new_record==1) && (record_edited==1) ){
		organizFormRecordEndEditing();
		organizFormInsertRecord();
		execOrganizSelectQuery();
	}
	ui.spinBox_navigator->setMaximum( ui.spinBox_navigator->maximum()+1 );
	ui.spinBox_navigator->setValue( ui.spinBox_navigator->maximum() );
	//zadannya znachen za zamovchuvannyam
	QSqlQuery * query = new QSqlQuery;
	query->exec("SELECT max(Ugoda) FROM slujb_rahunky");
	if (query->seek(0))
		ui.spinBox_Rahunok->setValue(query->value(0).toInt()+1);
	else
		ui.spinBox_Rahunok->setValue(1);
	ui.lineEdit_RahunokName->setText("");
	ui.comboBox_Bank->setCurrentIndex(0);
	
	organizConnectEditres();
	disconnect(ui.spinBox_slujb_rah_navigator, SIGNAL(valueChanged (int)), this, SLOT(populateOrganizForm(int)));
	new_slujbRah_record = 1;
	record_slujbRah_edited=0;
	delete query;
#else
	QMessageBox::information(this,
				"!!! Демо-версія !!!",
				"З метою економії розміру у демо-версії не доступна ця функція.",
				QMessageBox::Ok,
				QMessageBox::NoButton);
#endif
}
//------------------------------------------------------------
void tke_MainWindow_org::populateOrganizSlujbRahForm( int seek_query )
{
	organizSlujbRahDisConnectEditres(); //Обов'язковва
	disconnect(ui.spinBox_slujb_rah_navigator, SIGNAL(valueChanged (int)), this, SLOT(populateOrganizSlujbRahForm(int)));
	
	if ( (new_slujbRah_record==1) && (record_slujbRah_edited==1) ){ // novyi zapys, yakyi bulo redagovano
		organizSlujbRahFormRecordEndEditing();
		organizSlujbRahFormInsertRecord();
		execOrganizSlujbRahSelectQuery();
		new_slujbRah_record = 0;
	}
	else if ( record_slujbRah_edited==1 ){ // zmina zapysu
		organizSlujbRahFormRecordEndEditing();
		organizSlujbRahFormUpdateRecord();
		execOrganizSlujbRahSelectQuery();
		ui.spinBox_slujb_rah_navigator->setValue(seek_query);
		enableSlujbRahNavigationButtons(ui.spinBox_slujb_rah_navigator->value());
	}
	else if ( (new_slujbRah_record==1) && (record_slujbRah_edited==0) ){
		execOrganizSlujbRahSelectQuery();
		enableSlujbRahNavigationButtons(ui.spinBox_slujb_rah_navigator->value());
		ui.pushButton_slujb_rah_new->setEnabled( TRUE );
		new_slujbRah_record = 0;
	}
	organizSlujbRahDisConnectEditres();
	
	specRahSelectQuery->seek( seek_query-1 );
	xRecord = specRahSelectQuery->record();
	//Заповнення полів
	ui.spinBox_Rahunok->setValue(xRecord.value("Rahunok").toInt());
	ui.lineEdit_RahunokName->setText(xRecord.value("Rahunok_name").toString());
	ui.comboBox_Rahunok_type->setCurrentIndex(slujbRahunkyTypeList.indexOf(xRecord.value("Rahunok_type_id").toInt()));

	organizSlujbRahPopulateOperaciiTable();
	
	//Обов'язкові
	connect(ui.spinBox_slujb_rah_navigator, SIGNAL(valueChanged (int)), this, SLOT(populateOrganizSlujbRahForm(int)));;
	organizSlujbRahConnectEditres();	//Обов'язкова
}
//------------------------------------------------------------
void tke_MainWindow_org::enableSlujbRahNavigationButtons(int index)
{
	ui.pushButton_slujb_rah_first->setEnabled ( TRUE );
	ui.pushButton_slujb_rah_previous->setEnabled ( TRUE );
	ui.pushButton_slujb_rah_next->setEnabled ( TRUE );
	ui.pushButton_slujb_rah_last->setEnabled ( TRUE );
	if ( index < 2 ){
		ui.pushButton_slujb_rah_first->setEnabled ( FALSE );
		ui.pushButton_slujb_rah_previous->setEnabled ( FALSE );
	}
	if ( index >= ui.spinBox_slujb_rah_navigator->maximum() ){
		ui.pushButton_slujb_rah_next->setEnabled ( FALSE );
		ui.pushButton_slujb_rah_last->setEnabled ( FALSE );
	}
}
//------------------------------------------------------------
void tke_MainWindow_org::clearSlujbRahOperaciiTable()
{
	for (int col=0; col<ui.tableWidget_slujb_rah_operacii->columnCount(); col++)
		for (int row=0; row<ui.tableWidget_slujb_rah_operacii->rowCount(); row++){
			ui.tableWidget_slujb_rah_operacii->item(row, col)->setText("-");
			ui.tableWidget_slujb_rah_operacii->item(row, col)->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
		}
}
//------------------------------------------------------------
void tke_MainWindow_org::organizSlujbRahPopulateFiltrComboboxIndexChanged(int index)
{
	disconnect(ui.comboBox_PoshukSpecRahunok, SIGNAL(currentIndexChanged (int)), ui.spinBox_slujb_rah_navigator, SLOT(setValue(int)));
	disconnect(ui.comboBox_PoshukSpecRahunok, SIGNAL(currentIndexChanged (int)), this, SLOT(organizFiltrComboboxIndexChanged(int)));
	
	ui.comboBox_Poshuk_ugody->clearEditText();
	
	connect(ui.comboBox_PoshukSpecRahunok, SIGNAL(currentIndexChanged (int)), ui.spinBox_slujb_rah_navigator, SLOT(setValue(int)));
	connect(ui.comboBox_PoshukSpecRahunok, SIGNAL(currentIndexChanged (int)), this, SLOT(organizFiltrComboboxIndexChanged(int)));
}
//------------------------------------------------------------
void tke_MainWindow_org::organizSlujbRahPopulateOperaciiTable()
{
	disconnect(ui.tableWidget_slujb_rah_operacii, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(organizSlujbRahUpdateData(QTableWidgetItem *))); //update changes
	disconnect(ui.tableWidget_slujb_rah_operacii, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(organizSlujbRahOperaciiTableCalcPidsumky())); //update changes
	
	if (defValuesThread->isRunning()){
		defValuesThread->quit();
	}
	
	QSqlQuery * operaciiQuery = new QSqlQuery();
	
	clearSlujbRahOperaciiTable();
	
	operaciiQuery->exec("SELECT month, Vh_saldo, Narah, Oplata, id \
				FROM operacii_rahunkiv \
				WHERE Slujb_rahunok_id="+QVariant(ui.spinBox_Rahunok->value()).toString()+" \
					and year="+QVariant(ui.spinBox_slujb_rah_year_filtr->value()).toString());
	while (operaciiQuery->next()){
		int table_month = operaciiQuery->value(0).toInt();
		ui.tableWidget_slujb_rah_operacii->item( table_month-1, 0)->setText( operaciiQuery->value(4).toString() );
		ui.tableWidget_slujb_rah_operacii->item( table_month-1, 1)->setText( uMToStr2(operaciiQuery->value(1).toDouble()));
		ui.tableWidget_slujb_rah_operacii->item( table_month-1, 3)->setText( uMToStr2(operaciiQuery->value(2).toDouble()));
		ui.tableWidget_slujb_rah_operacii->item( table_month-1, 5)->setText( uMToStr2(operaciiQuery->value(3).toDouble()));
			//set editable
		ui.tableWidget_slujb_rah_operacii->item(table_month-1, 3)->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
		ui.tableWidget_slujb_rah_operacii->item(table_month-1, 5)->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
	}
	
	defValuesThread->setTableWidget(ui.tableWidget_slujb_rah_operacii);
	defValuesThread->setSpecRahunok(ui.spinBox_Rahunok->value());
	defValuesThread->setYear(ui.spinBox_slujb_rah_year_filtr->value());
	defValuesThread->start();
	
	organizSlujbRahOperaciiTableCalcPidsumky();
	
	delete operaciiQuery;
	
	connect(ui.tableWidget_slujb_rah_operacii, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(organizSlujbRahUpdateData(QTableWidgetItem *))); //update changes
	connect(ui.tableWidget_slujb_rah_operacii, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(organizSlujbRahOperaciiTableCalcPidsumky())); //update changes
}
//------------------------------------------------------------
void tke_MainWindow_org::organizButtonSlujbRahSaveClicked()
{
	populateOrganizSlujbRahForm( ui.spinBox_slujb_rah_navigator->value() );
}
//------------------------------------------------------------
void tke_MainWindow_org::organizButtonSlujbRahCancelClicked()
{
	organizSlujbRahFormRecordEndEditing();
	populateOrganizSlujbRahForm( ui.spinBox_slujb_rah_navigator->value() );
}
//------------------------------------------------------------
void tke_MainWindow_org::organizSlujbRahUpdateData(QTableWidgetItem * item)
{
	QSqlQuery *query = new QSqlQuery();
	QString row_id_str = item->tableWidget()->item(item->row(),0)->text();
	if (item->column() == 3){
		query->exec("UPDATE operacii_rahunkiv SET Narah="+uMToStr2Krapka(item->text().toDouble())+" WHERE id="+row_id_str);
		query->exec("SELECT Narah FROM operacii_rahunkiv WHERE id="+row_id_str);
		query->seek(0);
		item->setText(uMToStr2(query->value(0).toDouble()));
	}
	if (item->column() == 5){
		query->exec("UPDATE operacii_rahunkiv SET Oplata="+uMToStr2Krapka(item->text().toDouble())+" WHERE id="+row_id_str);
		query->exec("SELECT Oplata FROM operacii_rahunkiv WHERE id="+row_id_str);
		query->seek(0);
		item->setText(uMToStr2(query->value(0).toDouble()));
	}
	delete query;
}
//------------------------------------------------------------
void tke_MainWindow_org::organizSlujbRahOperaciiTableCalcPidsumky()
{
	disconnect(ui.tableWidget_slujb_rah_operacii, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(organizSlujbRahUpdateData(QTableWidgetItem *))); //update changes
	disconnect(ui.tableWidget_slujb_rah_operacii, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(organizSlujbRahOperaciiTableCalcPidsumky())); //update changes
	
	double sum=0;
	int row, col;
	for (col=2; col<ui.tableWidget_slujb_rah_operacii->columnCount()-1; col++){  //Обчислення сум по нарахуваннях стовбцях таблиці
		sum = 0;
		for (row=0; row<ui.tableWidget_slujb_rah_operacii->rowCount()-1; row++){
			sum += ui.tableWidget_slujb_rah_operacii->item(row, col)->text().toDouble();
		}
		ui.tableWidget_slujb_rah_operacii->item(ui.tableWidget_slujb_rah_operacii->rowCount()-1, col)->setText(uMToStr2(sum));
	}
	sum=0;
	for (int row=0; row<ui.tableWidget_slujb_rah_operacii->rowCount()-1; row++){
		sum  = ui.tableWidget_slujb_rah_operacii->item(row, 1)->text().toDouble();
		sum += ui.tableWidget_slujb_rah_operacii->item(row, 3)->text().toDouble();
		sum -= ui.tableWidget_slujb_rah_operacii->item(row, 5)->text().toDouble();
		
		ui.tableWidget_slujb_rah_operacii->item(row, 6)->setText(uMToStr2(sum));
	}
	
	connect(ui.tableWidget_slujb_rah_operacii, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(organizSlujbRahUpdateData(QTableWidgetItem *))); //update changes
	connect(ui.tableWidget_slujb_rah_operacii, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(organizSlujbRahOperaciiTableCalcPidsumky())); //update changes
}
//------------------------------------------------------------
void tke_MainWindow_org::populateNarahPidrozdilyTable()
{
	disconnect(ui.tableWidget_pidrozdily_narah_pidrozdily->selectionModel(), SIGNAL(currentRowChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(showNarahPidrozdilInfo()));
	
	QSqlQuery *query = new QSqlQuery();
	ui.tableWidget_pidrozdily_narah_pidrozdily->setSortingEnabled(false);
	query->exec("SELECT count(*) FROM pidrozdily");
	query->seek(0);
	int rowCount=query->value(0).toInt();
	ui.tableWidget_pidrozdily_narah_pidrozdily->setRowCount(rowCount);
	query->exec("SELECT pidrozdily.id, pidrozdily.Ugoda, pidrozdily.Name, pidrozdily.Opal, \
						pidrozdily.Gar_voda, pidrozdily.Lich, organizacii.Nazva \
				FROM pidrozdily \
				LEFT JOIN organizacii ON organizacii.Ugoda=pidrozdily.Ugoda \
				ORDER BY pidrozdily.Ugoda");
	for (int iter=0; iter<rowCount; iter++){
		query->next();
		ui.tableWidget_pidrozdily_narah_pidrozdily->setItem(iter, 0, new QTableWidgetItem(query->value(0).toString()));
		ui.tableWidget_pidrozdily_narah_pidrozdily->setItem(iter, 1, new QTableWidgetItem(query->value(1).toString()));
		ui.tableWidget_pidrozdily_narah_pidrozdily->item(iter,1)->setTextAlignment( Qt::AlignCenter );
		ui.tableWidget_pidrozdily_narah_pidrozdily->setItem(iter, 2, new QTableWidgetItem(query->value(6).toString()));
		ui.tableWidget_pidrozdily_narah_pidrozdily->item(iter,2)->setTextAlignment( Qt::AlignCenter );
		ui.tableWidget_pidrozdily_narah_pidrozdily->setItem(iter, 3, new QTableWidgetItem(query->value(2).toString()));
		ui.tableWidget_pidrozdily_narah_pidrozdily->item(iter,3)->setTextAlignment( Qt::AlignCenter );
		if (query->value(3).toBool()){
			ui.tableWidget_pidrozdily_narah_pidrozdily->setItem(iter, 4, new QTableWidgetItem(codec->toUnicode("Так")));
			ui.tableWidget_pidrozdily_narah_pidrozdily->item(iter,4)->setTextAlignment( Qt::AlignCenter );
		}
		else
			ui.tableWidget_pidrozdily_narah_pidrozdily->setItem(iter, 4, new QTableWidgetItem(codec->toUnicode("")));
		if (query->value(5).toBool()){
			ui.tableWidget_pidrozdily_narah_pidrozdily->setItem(iter, 5, new QTableWidgetItem(codec->toUnicode("Так")));
			ui.tableWidget_pidrozdily_narah_pidrozdily->item(iter,5)->setTextAlignment( Qt::AlignCenter );
		}
		else
			ui.tableWidget_pidrozdily_narah_pidrozdily->setItem(iter, 5, new QTableWidgetItem(codec->toUnicode("")));
		if (query->value(4).toBool()){
			ui.tableWidget_pidrozdily_narah_pidrozdily->setItem(iter, 6, new QTableWidgetItem(codec->toUnicode("Так")));
			ui.tableWidget_pidrozdily_narah_pidrozdily->item(iter,6)->setTextAlignment( Qt::AlignCenter );
		}
		else
			ui.tableWidget_pidrozdily_narah_pidrozdily->setItem(iter, 6, new QTableWidgetItem(codec->toUnicode("")));
	}
	ui.tableWidget_pidrozdily_narah_pidrozdily->resizeColumnToContents(1);
	ui.tableWidget_pidrozdily_narah_pidrozdily->resizeColumnToContents(4);
	ui.tableWidget_pidrozdily_narah_pidrozdily->resizeColumnToContents(5);
	ui.tableWidget_pidrozdily_narah_pidrozdily->resizeColumnToContents(6);
	
	ui.tableWidget_pidrozdily_narah_pidrozdily->setSortingEnabled(true);
	delete query;
	
	connect(ui.tableWidget_pidrozdily_narah_pidrozdily->selectionModel(), SIGNAL(currentRowChanged( const QModelIndex &, const QModelIndex &)), this, SLOT(showNarahPidrozdilInfo()));
}
//------------------------------------------------------------
void tke_MainWindow_org::clearNarahPidrozdilyForm()
{
	ui.doubleSpinBox_osnovna_kilkist_tepla->setValue(0);
	ui.doubleSpinBox_dodatkova_kilkist_tepla->setValue(0);
	ui.doubleSpinBox_taryf_za_CO_narah_pidrozdily->setValue(0);
	ui.tableWidget_dni_opal_narah_pidrozdily->setItem(0, 0, new QTableWidgetItem('0'));
	ui.tableWidget_dni_opal_narah_pidrozdily->item(0,0)->setTextAlignment( Qt::AlignCenter );
	ui.tableWidget_dni_opal_narah_pidrozdily->setItem(0, 1, new QTableWidgetItem('0'));
	ui.tableWidget_dni_opal_narah_pidrozdily->item(0,1)->setTextAlignment( Qt::AlignCenter );
	ui.doubleSpinBox_pochatk_pokazn_lich_narah_pidrozdily->setValue(0);
	ui.doubleSpinBox_kinc_pochatk_pokazn_lich_narah_pidrozdily->setValue(0);
	ui.doubleSpinBox_kilkist_bilyzny_narah_pidrozdily->setValue(0);
	ui.doubleSpinBox_taryf_za_gar_vodu_narah_pidrozdily->setValue(0);
	ui.doubleSpinBox_zag_kilkist_tepla_CO_narah_pidrozdily->setValue(0);
	ui.doubleSpinBox_zag_narah_za_CO_narah_pidrozdily->setValue(0);
	ui.doubleSpinBox_zag_kilkist_tepla_GV_narah_pidrozdily->setValue(0);
	ui.doubleSpinBox_zag_narah_GV_narah_pidrozdily->setValue(0);
	
	ui.doubleSpinBox_pochatk_pokazn_lich_narah_pidrozdily_kub->setValue(0);
	ui.doubleSpinBox_kinc_pokazn_lich_narah_pidrozdily_kub->setValue(0);
	ui.doubleSpinBox_taryf_za_gar_vodu_kub_narah_pidrozdily->setValue(0);
}
//------------------------------------------------------------
void tke_MainWindow_org::populateNarahPidrozdilyForm(int pidrozdilId, QDate curDate)
{
	disconnect(ui.tableWidget_dni_opal_narah_pidrozdily, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(dniOpalTableChanged()));
	disconnect(ui.doubleSpinBox_osnovna_kilkist_tepla, SIGNAL(valueChanged(double)), this, SLOT(opalLichZnachChanged()));
	disconnect(ui.doubleSpinBox_dodatkova_kilkist_tepla, SIGNAL(valueChanged(double)), this, SLOT(opalLichZnachChanged()));
	disconnect(ui.doubleSpinBox_pochatk_pokazn_lich_narah_pidrozdily, SIGNAL(valueChanged(double)), this, SLOT(opalGVLichZnachChanged()));
	disconnect(ui.doubleSpinBox_kinc_pochatk_pokazn_lich_narah_pidrozdily, SIGNAL(valueChanged(double)), this, SLOT(opalGVLichZnachChanged()));
	disconnect(ui.doubleSpinBox_kilkist_bilyzny_narah_pidrozdily, SIGNAL(valueChanged(double)), this, SLOT(opalGVLichZnachChanged()));
	disconnect(ui.doubleSpinBox_pochatk_pokazn_lich_narah_pidrozdily_kub, SIGNAL(valueChanged(double)), this, SLOT(opalGVLichZnachChanged()));
	disconnect(ui.doubleSpinBox_kinc_pokazn_lich_narah_pidrozdily_kub, SIGNAL(valueChanged(double)), this, SLOT(opalGVLichZnachChanged()));
	disconnect(ui.doubleSpinBox_zag_kilkist_tepla_CO_narah_pidrozdily, SIGNAL(valueChanged(double)), this, SLOT(narahZnachChanged()));
	disconnect(ui.doubleSpinBox_zag_narah_za_CO_narah_pidrozdily, SIGNAL(valueChanged(double)), this, SLOT(narahZnachChanged()));
	disconnect(ui.doubleSpinBox_zag_kilkist_tepla_GV_narah_pidrozdily, SIGNAL(valueChanged(double)), this, SLOT(narahZnachChanged()));
	disconnect(ui.doubleSpinBox_zag_narah_GV_narah_pidrozdily, SIGNAL(valueChanged(double)), this, SLOT(narahZnachChanged()));
	
	clearNarahPidrozdilyForm();
	
	daysOpalCh=false;
	opalLichCh=false;
	opalGVLichCh=false;
	
	QSqlQuery *query = new QSqlQuery();
	//Основна і додаткова кількість тела
	query->exec("SELECT Osn_kilk_tepla, Dod_kilk_tepla \
				FROM pokaz_lich_opal_organiz \
				WHERE Pidrozdil_ID="+QVariant(pidrozdilId).toString()+" \
					and Year="+QVariant(curDate.year()).toString()+" \
					and Month="+QVariant(curDate.month()).toString());
	if (query->seek(0)){
		ui.doubleSpinBox_osnovna_kilkist_tepla->setValue(query->value(0).toDouble());
		ui.doubleSpinBox_dodatkova_kilkist_tepla->setValue(query->value(1).toDouble());
	}
	//Тариф по ЦО
	UOrgTaryfInfo orgTaryf(curDate);
	double iGKalTaryf = orgTaryf.isValid() ? orgTaryf.vart_g_kal(financeTypeForPidrozdil(pidrozdilId)) : 0;
	ui.doubleSpinBox_taryf_za_CO_narah_pidrozdily->setValue(iGKalTaryf);
	
	populateTableKilkistDnivOpal();
	
	//Показники лічильника по гарячій воді
	query->exec("SELECT PochLich, KincLich, KilkistBilyzny, PochLichKub, KincLichKub \
				FROM pokaz_lich_gar_voda_organiz \
				WHERE Pidrozdil_id="+QVariant(pidrozdilId).toString()+" \
					and Year="+QVariant(curDate.year()).toString()+" \
					and Month="+QVariant(curDate.month()).toString());
	if (query->seek(0)){
		ui.doubleSpinBox_pochatk_pokazn_lich_narah_pidrozdily->setValue(query->value(0).toDouble());
		ui.doubleSpinBox_kinc_pochatk_pokazn_lich_narah_pidrozdily->setValue(query->value(1).toDouble());
		ui.doubleSpinBox_kilkist_bilyzny_narah_pidrozdily->setValue(query->value(2).toDouble());
		ui.doubleSpinBox_pochatk_pokazn_lich_narah_pidrozdily_kub->setValue(query->value(3).toDouble());
		ui.doubleSpinBox_kinc_pokazn_lich_narah_pidrozdily_kub->setValue(query->value(4).toDouble());
	}
	
	//Тариф за гарячу воду
	ui.doubleSpinBox_taryf_za_gar_vodu_narah_pidrozdily->setValue(orgTaryf.isValid() ? orgTaryf.vart_g_kal_gar_vody : 0);
	ui.doubleSpinBox_taryf_za_gar_vodu_kub_narah_pidrozdily->setValue(orgTaryf.isValid() ? orgTaryf.vart_kuba_GV : 0);
	
	//Нараховані суми
	query->exec("SELECT Narah_opal, Kilkist_tepla_opal, Narah_voda, Kilkist_tepla_gar_voda \
				FROM operacii \
				WHERE Pidrozdil="+QVariant(pidrozdilId).toString()+" \
					and Year="+QVariant(curDate.year()).toString()+" \
					and Month="+QVariant(curDate.month()).toString());
	if (query->seek(0)){
		ui.doubleSpinBox_zag_kilkist_tepla_CO_narah_pidrozdily->setValue(query->value(1).toDouble());
		ui.doubleSpinBox_zag_narah_za_CO_narah_pidrozdily->setValue(query->value(0).toDouble());
		ui.doubleSpinBox_zag_kilkist_tepla_GV_narah_pidrozdily->setValue(query->value(3).toDouble());
		ui.doubleSpinBox_zag_narah_GV_narah_pidrozdily->setValue(query->value(2).toDouble());
	}
	
	delete query;
	
	//Активність компонент
	if (ui.tableWidget_pidrozdily_narah_pidrozdily->
								item(ui.tableWidget_pidrozdily_narah_pidrozdily->currentRow(),5)->text()==codec->toUnicode("Так"))
		ui.groupBox_pokaz_lich_narah_pidrozdil->setEnabled(true);
	else
		ui.groupBox_pokaz_lich_narah_pidrozdil->setEnabled(false);
	
	if (ui.tableWidget_pidrozdily_narah_pidrozdily->
								item(ui.tableWidget_pidrozdily_narah_pidrozdily->currentRow(),6)->text()==codec->toUnicode("Так"))
		ui.groupBox_pokaz_lich_gar_voda_narah_pidrozdil->setEnabled(true);
	else
		ui.groupBox_pokaz_lich_gar_voda_narah_pidrozdil->setEnabled(false);
	
	if (ui.tableWidget_pidrozdily_narah_pidrozdily->
								item(ui.tableWidget_pidrozdily_narah_pidrozdily->currentRow(),4)->text()==codec->toUnicode("Так")){
		ui.groupBox_dni_opal_pidrozdily->setEnabled(true);
		ui.groupBox_narahovani_sumy_narah_pidrozdil->setEnabled(true);
	}
	else{
		ui.groupBox_dni_opal_pidrozdily->setEnabled(false);
		ui.groupBox_pokaz_lich_narah_pidrozdil->setEnabled(false);
		ui.groupBox_pokaz_lich_gar_voda_narah_pidrozdil->setEnabled(false);
		ui.groupBox_narahovani_sumy_narah_pidrozdil->setEnabled(false);	}
	
	ui.pushButton_save_narah_pidrozdily->setEnabled(false);//Закрити кнопку збереження
	
	connect(ui.tableWidget_dni_opal_narah_pidrozdily, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(dniOpalTableChanged()));
	connect(ui.doubleSpinBox_osnovna_kilkist_tepla, SIGNAL(valueChanged(double)), this, SLOT(opalLichZnachChanged()));
	connect(ui.doubleSpinBox_dodatkova_kilkist_tepla, SIGNAL(valueChanged(double)), this, SLOT(opalLichZnachChanged()));
	connect(ui.doubleSpinBox_pochatk_pokazn_lich_narah_pidrozdily, SIGNAL(valueChanged(double)), this, SLOT(opalGVLichZnachChanged()));
	connect(ui.doubleSpinBox_kinc_pochatk_pokazn_lich_narah_pidrozdily, SIGNAL(valueChanged(double)), this, SLOT(opalGVLichZnachChanged()));
	connect(ui.doubleSpinBox_kilkist_bilyzny_narah_pidrozdily, SIGNAL(valueChanged(double)), this, SLOT(opalGVLichZnachChanged()));
	connect(ui.doubleSpinBox_pochatk_pokazn_lich_narah_pidrozdily_kub, SIGNAL(valueChanged(double)), this, SLOT(opalGVLichZnachChanged()));
	connect(ui.doubleSpinBox_kinc_pokazn_lich_narah_pidrozdily_kub, SIGNAL(valueChanged(double)), this, SLOT(opalGVLichZnachChanged()));
	connect(ui.doubleSpinBox_zag_kilkist_tepla_CO_narah_pidrozdily, SIGNAL(valueChanged(double)), this, SLOT(narahZnachChanged()));
	connect(ui.doubleSpinBox_zag_narah_za_CO_narah_pidrozdily, SIGNAL(valueChanged(double)), this, SLOT(narahZnachChanged()));
	connect(ui.doubleSpinBox_zag_kilkist_tepla_GV_narah_pidrozdily, SIGNAL(valueChanged(double)), this, SLOT(narahZnachChanged()));
	connect(ui.doubleSpinBox_zag_narah_GV_narah_pidrozdily, SIGNAL(valueChanged(double)), this, SLOT(narahZnachChanged()));
}
//------------------------------------------------------------
void tke_MainWindow_org::showPidrozdilInfo()
{
	dialogEditPidrozdil = new tkeDialogEditPidrozdil(0, ui.tableWidget_pidrozdily_narah_pidrozdily->
								item(ui.tableWidget_pidrozdily_narah_pidrozdily->currentRow(),0)->text().toInt());
	if (dialogEditPidrozdil->exec() == 1){
		//Виконується коли редаговано запис
		modelPidrozdily->setQuery(modelQueryStr + " WHERE Ugoda="+QVariant(ui.spinBox_Ugoda->value()).toString()+" ORDER BY pidrozdily.Name");
		reloadNarahPidrozdilInfo();
		ui.tableView_pidrozdily->resizeColumnsToContents();
		ui.tableView_pidrozdily->resizeRowsToContents();
	}
	delete dialogEditPidrozdil;
}
//------------------------------------------------------------
void tke_MainWindow_org::showNarahPidrozdilInfo()
{
	populateNarahPidrozdilyForm(ui.tableWidget_pidrozdily_narah_pidrozdily->
								item(ui.tableWidget_pidrozdily_narah_pidrozdily->currentRow(),0)->text().toInt(),
								ui.dateEdit_narah_pidrozdily->date());
}
//------------------------------------------------------------
void tke_MainWindow_org::showDniOpalennyaForm()
{
	tkeFormDaysOpalSimple *formDaysOpal = new tkeFormDaysOpalSimple(0, ui.tableWidget_pidrozdily_narah_pidrozdily->
								item(ui.tableWidget_pidrozdily_narah_pidrozdily->currentRow(),0)->text().toInt(),
								ui.dateEdit_narah_pidrozdily->date().year(),
								ui.dateEdit_narah_pidrozdily->date().month(),
								ui.tableWidget_pidrozdily_narah_pidrozdily->
								item(ui.tableWidget_pidrozdily_narah_pidrozdily->currentRow(),2)->text()+"/"+
								ui.tableWidget_pidrozdily_narah_pidrozdily->
								item(ui.tableWidget_pidrozdily_narah_pidrozdily->currentRow(),3)->text());
	formDaysOpal->setWindowModality(Qt::ApplicationModal);
	formDaysOpal->show();
	connect(formDaysOpal, SIGNAL(destroyed (QObject *)), this, SLOT(populateTableKilkistDnivOpal()));
}
//------------------------------------------------------------
void tke_MainWindow_org::populateTableKilkistDnivOpal()
{
	int pidrozdilId=ui.tableWidget_pidrozdily_narah_pidrozdily->
								item(ui.tableWidget_pidrozdily_narah_pidrozdily->currentRow(),0)->text().toInt(); 
	QDate curDate=ui.dateEdit_narah_pidrozdily->date();
	QSqlQuery *query = new QSqlQuery();
	//Кількість днів опалення
	query->exec("SELECT sum(Day_kinc_opal-Day_poch_opal+1) \
				FROM grafik_opal \
				WHERE Opal_lich=2 \
					and Pidrozdil="+QVariant(pidrozdilId).toString()+" \
					and year(Opal_date)="+QVariant(curDate.year()).toString()+" \
					and month(Opal_date)="+QVariant(curDate.month()).toString());
	if (query->seek(0)){
		ui.tableWidget_dni_opal_narah_pidrozdily->setItem(0, 0, new QTableWidgetItem(query->value(0).toString()));
		ui.tableWidget_dni_opal_narah_pidrozdily->item(0,0)->setTextAlignment( Qt::AlignCenter );
	}
	
	query->exec("SELECT sum(Day_kinc_opal-Day_poch_opal+1) \
				FROM grafik_opal \
				WHERE Opal_lich=1 \
					and Pidrozdil="+QVariant(pidrozdilId).toString()+" \
					and year(Opal_date)="+QVariant(curDate.year()).toString()+" \
					and month(Opal_date)="+QVariant(curDate.month()).toString());
	if (query->seek(0)){
		ui.tableWidget_dni_opal_narah_pidrozdily->setItem(0, 1, new QTableWidgetItem(query->value(0).toString()));
		ui.tableWidget_dni_opal_narah_pidrozdily->item(0,1)->setTextAlignment( Qt::AlignCenter );
	}
	delete query;
}
//------------------------------------------------------------
void tke_MainWindow_org::dniOpalTableChanged()
{
	daysOpalCh=true;
	if (ui.checkBox_avtomatychne_narahuvannya_pidrozdily->isChecked())
		narahNarahPidrozdily();
}
//------------------------------------------------------------
void tke_MainWindow_org::opalLichZnachChanged()
{
	opalLichCh=true;
	if (ui.checkBox_avtomatychne_narahuvannya_pidrozdily->isChecked())
		narahNarahPidrozdily();
	ui.pushButton_save_narah_pidrozdily->setEnabled(true);
}
void tke_MainWindow_org::opalGVLichZnachChanged()
{
	opalGVLichCh=true;
	if (ui.checkBox_avtomatychne_narahuvannya_pidrozdily->isChecked())
		narahNarahPidrozdily();
	ui.pushButton_save_narah_pidrozdily->setEnabled(true);
}
//------------------------------------------------------------
void tke_MainWindow_org::narahZnachChanged()
{
	ui.pushButton_save_narah_pidrozdily->setEnabled(true);
}
//------------------------------------------------------------
void tke_MainWindow_org::narahNarahPidrozdily()
{
	disconnect(ui.doubleSpinBox_zag_kilkist_tepla_CO_narah_pidrozdily, SIGNAL(valueChanged(double)), this, SLOT(narahZnachChanged()));
	disconnect(ui.doubleSpinBox_zag_narah_za_CO_narah_pidrozdily, SIGNAL(valueChanged(double)), this, SLOT(narahZnachChanged()));
	disconnect(ui.doubleSpinBox_zag_kilkist_tepla_GV_narah_pidrozdily, SIGNAL(valueChanged(double)), this, SLOT(narahZnachChanged()));
	disconnect(ui.doubleSpinBox_zag_narah_GV_narah_pidrozdily, SIGNAL(valueChanged(double)), this, SLOT(narahZnachChanged()));
	
	int year=ui.dateEdit_narah_pidrozdily->date().year();
	int month=ui.dateEdit_narah_pidrozdily->date().month();
	double kilkistTeplaLich = ui.doubleSpinBox_osnovna_kilkist_tepla->value() + ui.doubleSpinBox_dodatkova_kilkist_tepla->value();
	double narahLich = (kilkistTeplaLich)*ui.doubleSpinBox_taryf_za_CO_narah_pidrozdily->value();
	double *monthTemper = new double[31];
	double kilkistTeplaNoLich=0;
	double teplNavantaj, tVnutr, tp0, narahNoLich, tSum;
	int numDaysOpalNoLich;
	QSqlQuery *narahTempQuery = new QSqlQuery();
	//ініціалізація метеорологічних показників обраного місяця
	for (int i=0; i<31; i++)
		monthTemper[i] = 0;
	narahTempQuery->exec("SELECT day(Date_pokazn) AS Day, Temper FROM meteorolog \
							WHERE year(Date_pokazn)="+ QVariant(year).toString() +" \
							and month(Date_pokazn)="+ QVariant(month).toString());
	narahTempQuery->seek(-1);
	while (narahTempQuery->next())
		monthTemper[narahTempQuery->value(0).toInt()-1] = narahTempQuery->value(1).toDouble();
	
	numDaysOpalNoLich = ui.tableWidget_dni_opal_narah_pidrozdily->item(0,1)->text().toInt();
	
	UOrgTaryfInfo orgTaryf(year, month);
	
	narahTempQuery->exec("SELECT * FROM pidrozdily WHERE id="+ui.tableWidget_pidrozdily_narah_pidrozdily->
								item(ui.tableWidget_pidrozdily_narah_pidrozdily->currentRow(),0)->text());
	if (numDaysOpalNoLich>0 && narahTempQuery->seek(0)){
		if (narahTempQuery->value(13).toInt() == 0){
			narahNoLich = narahTempQuery->value(5).toDouble() * orgTaryf.vart_kvadr_CO * numDaysOpalNoLich / QDate(year,month,1).daysInMonth();
		}
		else if (narahTempQuery->value(13).toInt() == 1){
			tSum=0;
			narahTempQuery->exec("SELECT Day_poch_opal, Day_kinc_opal FROM grafik_opal \
									WHERE Pidrozdil="+ui.tableWidget_pidrozdily_narah_pidrozdily->
									item(ui.tableWidget_pidrozdily_narah_pidrozdily->currentRow(),0)->text()+" \
										and year(Opal_date)="+QVariant(year).toString()+" \
										and month(Opal_date)="+QVariant(month).toString()+" \
										and Opal_lich=1");
			narahTempQuery->seek(-1);
			while (narahTempQuery->next()){
				int kinec=narahTempQuery->value(1).toInt();
				for (int i=narahTempQuery->value(0).toInt()-1; i<kinec; i++){
					tSum += monthTemper[i];
				}
			}
			tp0 = tSum/numDaysOpalNoLich;
		
			teplNavantaj = narahTempQuery->value(9).toDouble();
			tVnutr = narahTempQuery->value(8).toDouble();
		
			//Обчислення нарахування (без лічильника)
			kilkistTeplaNoLich = teplNavantaj * (tVnutr-tp0) * numDaysOpalNoLich * 24 / (tVnutr-Tcp);
			narahNoLich = kilkistTeplaNoLich * ui.doubleSpinBox_taryf_za_CO_narah_pidrozdily->value();
			//  . . . .  . . . . . . . . . . . . . . . . . . . . . . . . . .
		}
		else{
			narahNoLich = 0;
			kilkistTeplaNoLich = 0;
		}
	}
	else{
		narahNoLich = 0;
		kilkistTeplaNoLich = 0;
	}
	
	//Нарахування по гарячій воді
	double kilkistTeplaGarVoda = (ui.doubleSpinBox_kinc_pochatk_pokazn_lich_narah_pidrozdily->value()-
							ui.doubleSpinBox_pochatk_pokazn_lich_narah_pidrozdily->value())*0.05+
							ui.doubleSpinBox_kilkist_bilyzny_narah_pidrozdily->value()*2.375;
	double narahGarVoda = kilkistTeplaGarVoda*ui.doubleSpinBox_taryf_za_gar_vodu_narah_pidrozdily->value();
	if (narahGarVoda<0)
		narahGarVoda = 0;
	double naragGarVodaKub = (ui.doubleSpinBox_kinc_pokazn_lich_narah_pidrozdily_kub->value()-ui.doubleSpinBox_pochatk_pokazn_lich_narah_pidrozdily_kub->value())*
							ui.doubleSpinBox_taryf_za_gar_vodu_kub_narah_pidrozdily->value();
	if (naragGarVodaKub>0)
		narahGarVoda += naragGarVodaKub;
	
	ui.doubleSpinBox_zag_kilkist_tepla_CO_narah_pidrozdily->setValue(kilkistTeplaLich+kilkistTeplaNoLich);
	ui.doubleSpinBox_zag_narah_za_CO_narah_pidrozdily->setValue(narahLich+narahNoLich);
	ui.doubleSpinBox_zag_kilkist_tepla_GV_narah_pidrozdily->setValue(kilkistTeplaGarVoda);
	ui.doubleSpinBox_zag_narah_GV_narah_pidrozdily->setValue(narahGarVoda);
	
	delete narahTempQuery;
	
	connect(ui.doubleSpinBox_zag_kilkist_tepla_CO_narah_pidrozdily, SIGNAL(valueChanged(double)), this, SLOT(narahZnachChanged()));
	connect(ui.doubleSpinBox_zag_narah_za_CO_narah_pidrozdily, SIGNAL(valueChanged(double)), this, SLOT(narahZnachChanged()));
	connect(ui.doubleSpinBox_zag_kilkist_tepla_GV_narah_pidrozdily, SIGNAL(valueChanged(double)), this, SLOT(narahZnachChanged()));
	connect(ui.doubleSpinBox_zag_narah_GV_narah_pidrozdily, SIGNAL(valueChanged(double)), this, SLOT(narahZnachChanged()));
}
//------------------------------------------------------------
void tke_MainWindow_org::printRahunokNarahPidrozdily()
{
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	int rahType = settings->value("PrintDocs/OrganizRahunokType").toInt();
	delete settings;
	printDocsOrganiz = new UPrintDocsOrganiz();
	if (rahType == 2)
		printDocsOrganiz->print_rahunok_2(ui.dateEdit_narah_pidrozdily->date().year(), ui.dateEdit_narah_pidrozdily->date().month(), 
						ui.tableWidget_pidrozdily_narah_pidrozdily->
								item(ui.tableWidget_pidrozdily_narah_pidrozdily->currentRow(),1)->text().toInt(),0, 0);
	else
		printDocsOrganiz->print_rahunok_1(ui.dateEdit_narah_pidrozdily->date().year(), ui.dateEdit_narah_pidrozdily->date().month(), 
						ui.tableWidget_pidrozdily_narah_pidrozdily->
								item(ui.tableWidget_pidrozdily_narah_pidrozdily->currentRow(),1)->text().toInt(),0, 0);
	connect(this, SIGNAL(closeLoginSignal()), printDocsOrganiz->printform, SLOT(close()));
	connect(this, SIGNAL(mainWindowClosed()), printDocsOrganiz->printform, SLOT(close()));
	delete printDocsOrganiz;
}
//------------------------------------------------------------
void tke_MainWindow_org::saveNarahPidrozdilyZnach()
{
	QSqlTableModel * updateModel = new QSqlTableModel();
	QSqlRecord record;
	
	if (opalLichCh){
		//Збереження показників ліильника по опаленню
		updateModel->setTable("pokaz_lich_opal_organiz");
		updateModel->setFilter("(Pidrozdil_id=" + ui.tableWidget_pidrozdily_narah_pidrozdily->
								item(ui.tableWidget_pidrozdily_narah_pidrozdily->currentRow(),0)->text() + ") \
								and (Year=" + QVariant(ui.dateEdit_narah_pidrozdily->date().year()).toString() + ") \
								and (Month=" + QVariant(ui.dateEdit_narah_pidrozdily->date().month()).toString() +")");
		updateModel->select();
		if (updateModel->rowCount() == 0){
			updateModel->insertRows(0, 1);
			record = updateModel->record(0);
			record.setValue("id", nextLichylnyk("pokaz_lich_opal_organiz"));
			record.setValue("Pidrozdil_id", ui.tableWidget_pidrozdily_narah_pidrozdily->
								item(ui.tableWidget_pidrozdily_narah_pidrozdily->currentRow(),0)->text());
			record.setValue("Year", ui.dateEdit_narah_pidrozdily->date().year());
			record.setValue("Month", ui.dateEdit_narah_pidrozdily->date().month());
			record.setValue("Osn_kilk_tepla", ui.doubleSpinBox_osnovna_kilkist_tepla->value());
			record.setValue("Dod_kilk_tepla", ui.doubleSpinBox_dodatkova_kilkist_tepla->value());
			updateModel->setRecord(0, record);
			updateModel->submitAll();
		}
		else{
			record = updateModel->record(0);
			record.setValue("Osn_kilk_tepla", ui.doubleSpinBox_osnovna_kilkist_tepla->value());
			record.setValue("Dod_kilk_tepla", ui.doubleSpinBox_dodatkova_kilkist_tepla->value());
			updateModel->setRecord(0, record);
			updateModel->submitAll();
		}
	}
	//Збереження показників ліильника по гарячій воді
	if (opalGVLichCh){
		updateModel->setTable("pokaz_lich_gar_voda_organiz");
		updateModel->setFilter("(Pidrozdil_id=" + ui.tableWidget_pidrozdily_narah_pidrozdily->
								item(ui.tableWidget_pidrozdily_narah_pidrozdily->currentRow(),0)->text() + ") \
								and (Year=" + QVariant(ui.dateEdit_narah_pidrozdily->date().year()).toString() + ") \
								and (Month=" + QVariant(ui.dateEdit_narah_pidrozdily->date().month()).toString() +")");
		updateModel->select();
		if (updateModel->rowCount() == 0){
			updateModel->insertRows(0, 1);
			record = updateModel->record(0);
			record.setValue("id", nextLichylnyk("pokaz_lich_gar_voda_organiz"));
			record.setValue("Pidrozdil_id", ui.tableWidget_pidrozdily_narah_pidrozdily->
									item(ui.tableWidget_pidrozdily_narah_pidrozdily->currentRow(),0)->text());
			record.setValue("Year", ui.dateEdit_narah_pidrozdily->date().year());
			record.setValue("Month", ui.dateEdit_narah_pidrozdily->date().month());
			record.setValue("PochLich", ui.doubleSpinBox_pochatk_pokazn_lich_narah_pidrozdily->value());
			record.setValue("KincLich", ui.doubleSpinBox_kinc_pochatk_pokazn_lich_narah_pidrozdily->value());
			record.setValue("KilkistBilyzny", ui.doubleSpinBox_kilkist_bilyzny_narah_pidrozdily->value());
			record.setValue("PochLichKub", ui.doubleSpinBox_pochatk_pokazn_lich_narah_pidrozdily_kub->value());
			record.setValue("KincLichKub", ui.doubleSpinBox_kinc_pokazn_lich_narah_pidrozdily_kub->value());
			updateModel->setRecord(0, record);
			updateModel->submitAll();
		}
		else{
			record = updateModel->record(0);
			record.setValue("PochLich", ui.doubleSpinBox_pochatk_pokazn_lich_narah_pidrozdily->value());
			record.setValue("KincLich", ui.doubleSpinBox_kinc_pochatk_pokazn_lich_narah_pidrozdily->value());
			record.setValue("KilkistBilyzny", ui.doubleSpinBox_kilkist_bilyzny_narah_pidrozdily->value());
			record.setValue("PochLichKub", ui.doubleSpinBox_pochatk_pokazn_lich_narah_pidrozdily_kub->value());
			record.setValue("KincLichKub", ui.doubleSpinBox_kinc_pokazn_lich_narah_pidrozdily_kub->value());
			updateModel->setRecord(0, record);
			updateModel->submitAll();
		}
	}
	
	//Запис нарахування до бази даних
	updateModel->setTable("operacii");
	updateModel->setFilter("(Pidrozdil=" + ui.tableWidget_pidrozdily_narah_pidrozdily->
								item(ui.tableWidget_pidrozdily_narah_pidrozdily->currentRow(),0)->text() + ") \
									and (year=" + QVariant(ui.dateEdit_narah_pidrozdily->date().year()).toString() + ") \
									and (month=" + QVariant(ui.dateEdit_narah_pidrozdily->date().month()).toString()+")");
	qDebug() << updateModel->filter();
	updateModel->select();
	if (updateModel->rowCount() == 0){
		updateModel->insertRows(0, 1);
		QSqlRecord record = updateModel->record(0);
		record.setValue("id", nextLichylnyk("operacii"));
		record.setValue("Pidrozdil", ui.tableWidget_pidrozdily_narah_pidrozdily->
							item(ui.tableWidget_pidrozdily_narah_pidrozdily->currentRow(),0)->text());
		record.setValue("Year", ui.dateEdit_narah_pidrozdily->date().year());
		record.setValue("Month", ui.dateEdit_narah_pidrozdily->date().month());
		record.setValue("Narah_opal", ui.doubleSpinBox_zag_narah_za_CO_narah_pidrozdily->value());
		record.setValue("Narah_voda", ui.doubleSpinBox_zag_narah_GV_narah_pidrozdily->value());
		record.setValue("Kilkist_tepla_opal", ui.doubleSpinBox_zag_kilkist_tepla_CO_narah_pidrozdily->value());
		record.setValue("Kilkist_tepla_gar_voda", ui.doubleSpinBox_zag_kilkist_tepla_GV_narah_pidrozdily->value());
		
		updateModel->setRecord(0, record);
		updateModel->submitAll();
	}
	else{
		QSqlRecord record = updateModel->record(0);
		record.setValue("Narah_opal", ui.doubleSpinBox_zag_narah_za_CO_narah_pidrozdily->value());
		record.setValue("Narah_voda", ui.doubleSpinBox_zag_narah_GV_narah_pidrozdily->value());
		record.setValue("Kilkist_tepla_opal", ui.doubleSpinBox_zag_kilkist_tepla_CO_narah_pidrozdily->value());
		record.setValue("Kilkist_tepla_gar_voda", ui.doubleSpinBox_zag_kilkist_tepla_GV_narah_pidrozdily->value());
		updateModel->setRecord(0, record);
		updateModel->submitAll();
	}

	showNarahPidrozdilInfo();
	
	delete updateModel;
}
//------------------------------------------------------------
void tke_MainWindow_org::reloadNarahPidrozdilInfo()
{
	int activRow = ui.tableWidget_pidrozdily_narah_pidrozdily->currentRow();
	populateNarahPidrozdilyTable();
	showNarahPidrozdilInfo();
	ui.tableWidget_pidrozdily_narah_pidrozdily->selectRow(activRow);
}
//------------------------------------------------------------
void tke_MainWindow_org::action_registracion_activated()
{
	int dialogResult;
	tke_Dialog_login *dialogLogin = new tke_Dialog_login();
	if (dialogLogin->exec() == 1){
		dialogResult = dialogLogin->dialogResult();
		if (dialogResult > 100){
			tke_Dialog_change_password * dlgChPswrd = new tke_Dialog_change_password(0, dialogLogin->userLogin(), dialogLogin->userId());
			dlgChPswrd->exec();
			delete dlgChPswrd;
			dialogResult -= 100;
		}
		ui.centralwidget->parentWidget()->setWindowTitle(codec->toUnicode("ТКЕнерго менеджер :: організації --> ") + dialogLogin->userLogin() + " (" + OrganizName() + "). Версія "+TkeProgramVersion());
		workMode(dialogResult);
	}
	delete dialogLogin;
}
//------------------------------------------------------------
void tke_MainWindow_org::action_close_login_activated()
{
	ui.centralwidget->parentWidget()->setWindowTitle(codec->toUnicode("ТКЕнерго менеджер :: організації --> загальний") + " (" + OrganizName() + "). Версія "+TkeProgramVersion());
	saveMode();
}
//------------------------------------------------------------
void tke_MainWindow_org::actionEditPidrozdil_activated()
{
	xRecord = modelPidrozdily->record(ui.tableView_pidrozdily->selectionModel()->currentIndex().row());
	dialogEditPidrozdil = new tkeDialogEditPidrozdil(0, xRecord.value("id").toInt());
	if (dialogEditPidrozdil->exec() == 1){
		//Виконується коли редаговано запис
		modelPidrozdily->setQuery(modelQueryStr + " WHERE Ugoda="+QVariant(ui.spinBox_Ugoda->value()).toString()+" ORDER BY pidrozdily.Name");
	}
	delete dialogEditPidrozdil;
}
//------------------------------------------------------------
void tke_MainWindow_org::actionAddPidrozdil_activated()
{
	__insertPidrozdilRecord();
	modelPidrozdily->setQuery(modelQueryStr + " WHERE Ugoda="+QVariant(ui.spinBox_Ugoda->value()).toString()+" ORDER BY pidrozdily.Name");
}
//------------------------------------------------------------
void tke_MainWindow_org::actionDelPidrozdil_activated()
{
	if	( ! QMessageBox::question(
                this,
                codec->toUnicode("!!! Видалення підрозділу !!!"),
                codec->toUnicode("Ви дійсно бажаєте видалити підрозділ?\nНазва підрозділу - ")
								+ modelPidrozdily->record(ui.tableView_pidrozdily->currentIndex().row()).value("Name").toString(),
                codec->toUnicode("Так"),
				codec->toUnicode("Ні-ні-ні"),
                QString(), 0, 1)){
				
		QSqlQuery * query = new QSqlQuery;
		query->exec("DELETE FROM pidrozdily WHERE id=" 
			+ modelPidrozdily->record(ui.tableView_pidrozdily->currentIndex().row()).value("id").toString());
		modelPidrozdily->setQuery(modelQueryStr + " WHERE Ugoda="+QVariant(ui.spinBox_Ugoda->value()).toString()+" ORDER BY pidrozdily.Name");
		delete query;
		
	}
}
//------------------------------------------------------------
void tke_MainWindow_org::action_taryf_table_activated()
{
	ui.dockWidget_taryf->setVisible( ui.action_taryf_table->isChecked() );
}
//------------------------------------------------------------
void tke_MainWindow_org::action_pokaz_lich_table_activated()
{
	ui.dockWidget_pokaz_lich_table->setVisible( ui.action_pokaz_lich_table->isChecked() );
}
//------------------------------------------------------------
void tke_MainWindow_org::action_pokaz_lich_gar_voda_activated()
{
	ui.dockWidget_pokaz_lich_gar_voda_table->setVisible( ui.action_pokaz_lich_gar_voda->isChecked() );
}
//------------------------------------------------------------
void tke_MainWindow_org::action_meteor_table_activated()
{
	ui.dockWidget_meteor_table->setVisible( ui.action_meteor_table->isChecked() );
}
//------------------------------------------------------------
void tke_MainWindow_org::action_dni_opalennya_activated()
{
	ui.dockWidget_dni_opal->setVisible( ui.action_dni_opalennya->isChecked() );
}
//------------------------------------------------------------
void tke_MainWindow_org::action_oplata_activated()
{
	ui.dockWidget_oplata->setVisible( ui.action_oplata->isChecked() );
}
//------------------------------------------------------------
void tke_MainWindow_org::action_debuger_activated()
{
	emit requestDebugWindow();
}
//------------------------------------------------------------
void tke_MainWindow_org::action_calc_tepl_navantaj_activated()
{
	QSqlTableModel *tableModel = new QSqlTableModel();
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT count(*) FROM pidrozdily");
	query->seek(0);
	
	UPostupForm *postupForm = new UPostupForm(0, query->value(0).toInt());
	postupForm->show();
	tableModel->setTable("pidrozdily");
	tableModel->select();
	for (int i=0; i<tableModel->rowCount(); i++){
		QSqlRecord record = tableModel->record(i);
		double obem, teplHar, tVnutr;
		obem = record.value("Obem").toDouble();
		teplHar = record.value("Tepl_harakt").toDouble();
		tVnutr = record.value("T_vnutr").toDouble();
		record.setValue("Tepl_navantaj", obem*teplHar*(tVnutr-Tcp)*0.001);
		tableModel->setRecord(i, record);
		
		postupForm->inc_pos();
	}
	
	delete query;
	delete postupForm;
	
	if (tableModel->submitAll())
		QMessageBox::information(this,
				codec->toUnicode("!!! Закінчено розрахунок теплових навантажень !!!"),
				codec->toUnicode("Успішно закінчено розрахунок теплових навантажень усіх підрозділів."),
				QMessageBox::Ok,
				QMessageBox::NoButton);
	else
		QMessageBox::critical(this,
				codec->toUnicode("!!! Невдалий розрахунок теплових навантажень !!!"),
				codec->toUnicode("Програма не змогла записати одне(кілька) теплових навантажень.\nСпробуйте ще раз."),
				QMessageBox::Ok,
				QMessageBox::NoButton);
				
	delete tableModel;
	
	populateOrganizForm( ui.spinBox_navigator->value() );
}
//------------------------------------------------------------
void tke_MainWindow_org::action_narah_opal_activated()
{
	int month, year;
	tke_Dialog_narah *dialogNarah = new tke_Dialog_narah(this, codec->toUnicode("Нарахування за ЦО"), currentSysDate.year(), currentSysDate.month());
	if (dialogNarah->exec() != 1){
		delete dialogNarah;
		return;
	}
	year=dialogNarah->get_Year();
	month=dialogNarah->get_Month();
	
	QSqlQuery *narahPidrozdilyQuery = new QSqlQuery();
	QSqlQuery *narahTempQuery = new QSqlQuery();
	QSqlTableModel * narahUpdateModel = new QSqlTableModel();
	narahPidrozdilyQuery->exec("SELECT count(*) FROM pidrozdily WHERE Opal=1");
	narahPidrozdilyQuery->seek(0);
	
	//налаштування форми поступу
	UPostupForm *postupForm = new UPostupForm(0, narahPidrozdilyQuery->value(0).toInt());
	postupForm->showLable(true);
	postupForm->setPostupLabel(codec->toUnicode("Нарахування за ЦО для підрозділів організацій"));
	postupForm->show();
	
	//Створення і інціалізація змінних, які використовуються в нарахуванні
	double *monthTemper = new double[31];
	double kilkistTeplaLich=0, kilkistTeplaNoLich=0, zagalnKilkistTepla=0;
	double teplNavantaj, tVnutr, tp0, narahLich, narahNoLich, zagalnNarah, tSum;
	int numDaysOpalLich, numDaysOpalNolich;
		//ініціалізація метеорологічних показників обраного місяця
	for (int i=0; i<31; i++)
		monthTemper[i] = 0;
	narahTempQuery->exec("SELECT day(Date_pokazn) AS Day, Temper FROM meteorolog \
							WHERE year(Date_pokazn)="+ QVariant(year).toString() +" \
							and month(Date_pokazn)="+ QVariant(month).toString());
	narahTempQuery->seek(-1);
	while (narahTempQuery->next())
		monthTemper[narahTempQuery->value(0).toInt()-1] = narahTempQuery->value(1).toDouble();
	
	UOrgTaryfInfo orgTaryf(year, month);
	qDebug() << "Taryf = " + uMToStr2(orgTaryf.vart_g_kal(FinaceTypeAny));
	
	narahPidrozdilyQuery->exec("SELECT * FROM pidrozdily WHERE Opal=1");
	narahPidrozdilyQuery->seek(-1);
	while (narahPidrozdilyQuery->next()){
			//кількість днів опалення по лічильнику і без
		narahTempQuery->exec("SELECT sum(Day_kinc_opal)-sum(Day_poch_opal)+count(*) \
								FROM (SELECT * FROM grafik_opal \
										WHERE Opal_lich=2 \
											and year(Opal_date)="+QVariant(year).toString()+" \
											and month(Opal_date)="+QVariant(month).toString()+" \
											and Pidrozdil="+narahPidrozdilyQuery->value(0).toString()+") AS opalGr");
		narahTempQuery->seek(0);
		numDaysOpalLich = narahTempQuery->value(0).toInt();
		narahTempQuery->exec("SELECT sum(Day_kinc_opal)-sum(Day_poch_opal)+count(*) \
								FROM (SELECT * FROM grafik_opal \
										WHERE Opal_lich=1 \
											and year(Opal_date)="+QVariant(year).toString()+" \
											and month(Opal_date)="+QVariant(month).toString()+" \
											and Pidrozdil="+narahPidrozdilyQuery->value(0).toString()+") AS opalGr");
		narahTempQuery->seek(0);
		numDaysOpalNolich = narahTempQuery->value(0).toInt();
		qDebug() << "organiz-"+narahPidrozdilyQuery->value(1).toString()+" daysLich-"+QVariant(numDaysOpalLich).toString()+
						" daysNolich"+QVariant(numDaysOpalNolich).toString();
		
		//Розрахунок (при потребі) нарахування по метеорологічних показниках (без лічильника)
		if (numDaysOpalNolich>0){
			if (narahPidrozdilyQuery->value(13).toInt() == 0){
				narahNoLich = narahPidrozdilyQuery->value(5).toDouble() * orgTaryf.vart_kvadr_CO * numDaysOpalNolich / QDate(year,month,1).daysInMonth();
			}
			else if (narahPidrozdilyQuery->value(13).toInt() == 1){
				tSum=0;
				narahTempQuery->exec("SELECT Day_poch_opal, Day_kinc_opal FROM grafik_opal \
										WHERE Pidrozdil="+narahPidrozdilyQuery->value(0).toString()+" \
											and year(Opal_date)="+QVariant(year).toString()+" \
											and month(Opal_date)="+QVariant(month).toString()+" \
											and Opal_lich=1");
				narahTempQuery->seek(-1);
				while (narahTempQuery->next()){
					int kinec=narahTempQuery->value(1).toInt();
					for (int i=narahTempQuery->value(0).toInt()-1; i<kinec; i++){
						tSum += monthTemper[i];
						qDebug() << "Temper - monthTemper[i] - " + QVariant(monthTemper[i]).toString() + " i - " + QVariant(i+1).toString();
					}
				}
				qDebug() << "tSum - " + QVariant(tSum).toString();
				qDebug() << "numDaysOpalNolich - " + QVariant(numDaysOpalNolich).toString();
				tp0 = tSum/numDaysOpalNolich;
				qDebug() << "tp0 - " + QVariant(tp0).toString();
				teplNavantaj = narahPidrozdilyQuery->value(9).toDouble();
				qDebug() << "teplNavantaj - " + QVariant(teplNavantaj).toString();
				tVnutr = narahPidrozdilyQuery->value(8).toDouble();
				qDebug() << "tVnutr - " + QVariant(tVnutr).toString();
				qDebug() << "Tcp - " + QVariant(Tcp).toString();
			
				//Обчислення нарахування (без лічильника)
				kilkistTeplaNoLich = teplNavantaj * (tVnutr-tp0) * numDaysOpalNolich * 24 / (tVnutr-Tcp);
				narahNoLich = kilkistTeplaNoLich * orgTaryf.vart_g_kal(financeTypeForOrganiz(narahPidrozdilyQuery->value(1).toInt())); 
				//  . . . .  . . . . . . . . . . . . . . . . . . . . . . . . . .
			}
			else{
				narahNoLich = 0;
				kilkistTeplaNoLich = 0;
			}
			if (narahNoLich<0)
					narahNoLich = 0;
		}
		else{
			narahNoLich = 0;
			kilkistTeplaNoLich = 0;
		}
		
		//Розрахунок (при потребі) нарахування по показниках лічильника
		if (narahPidrozdilyQuery->value(0).toInt()==1){
			narahTempQuery->exec("SELECT Osn_kilk_tepla, Dod_kilk_tepla FROM pokaz_lich_opal_organiz \
									WHERE pidrozdil_ID="+narahPidrozdilyQuery->value(0).toString()+" \
									and year="+QVariant(year).toString()+" \
									and month="+QVariant(month).toString());
			narahTempQuery->seek(0);
			double osnKilkTepla, dodKilkTepla, koefPereved;
			osnKilkTepla = narahTempQuery->value(0).toDouble();
			dodKilkTepla = narahTempQuery->value(1).toDouble();
			koefPereved = narahPidrozdilyQuery->value(12).toDouble();
			//Обчислення нарахування (по лічильника)
			kilkistTeplaNoLich = osnKilkTepla + dodKilkTepla;
			narahLich = kilkistTeplaNoLich * orgTaryf.vart_g_kal(financeTypeForOrganiz(narahPidrozdilyQuery->value(1).toInt()));
			if (narahLich<0)
				narahLich=0;
			//  . . . .  . . . . . . . . . . . . . . . . . . . . . . . . . .
		}
		else{
			narahLich = 0;
			kilkistTeplaLich = 0;

		}
		zagalnNarah = narahLich + narahNoLich;
		zagalnKilkistTepla = kilkistTeplaLich + kilkistTeplaNoLich;
		
		//Запис нарахування до бази даних
		narahUpdateModel->setTable("operacii");
		narahUpdateModel->setFilter("(Pidrozdil=" + narahPidrozdilyQuery->value(0).toString() + ") \
										and (year=" + QVariant(year).toString() + ") \
										and (month=" + QVariant(month).toString()+")");
		//qDebug() << narahUpdateModel->filter();
		narahUpdateModel->select();
		if (narahUpdateModel->rowCount() == 0){
			narahUpdateModel->insertRows(0, 1);
			QSqlRecord record = narahUpdateModel->record(0);
			record.setValue("id", nextLichylnyk("operacii"));
			record.setValue("Pidrozdil", narahPidrozdilyQuery->value(0));
			record.setValue("Year", year);
			record.setValue("Month", month);
			record.setValue("Narah_opal", d_to_m(zagalnNarah));
			record.setValue("Narah_voda", 0);
			record.setValue("Kilkist_tepla_opal", zagalnKilkistTepla);
			record.setValue("Kilkist_tepla_gar_voda", 0);
			narahUpdateModel->setRecord(0, record);
			narahUpdateModel->submitAll();
		}
		else{
			QSqlRecord record = narahUpdateModel->record(0);
			record.setValue("Narah_opal", d_to_m(zagalnNarah));
			record.setValue("Kilkist_tepla_opal", zagalnKilkistTepla);
			narahUpdateModel->setRecord(0, record);
			narahUpdateModel->submitAll();
		}
		
		postupForm->inc_pos();
	}
	
	//Перерахунок вхідних сальдо
	__pererahAllSaldoMonth(month, year, postupForm);
	organizPopulateOperaciiTable();
	
	delete narahUpdateModel;
	delete monthTemper;
	delete postupForm;
	delete narahPidrozdilyQuery;
	delete narahTempQuery;
	delete dialogNarah;
}
//------------------------------------------------------------
void tke_MainWindow_org::action_narah_gar_voda_activated()
{
	int month, year;

	tke_Dialog_narah *dialogNarah = new tke_Dialog_narah(this, codec->toUnicode("Нарахування за ГВ"), currentSysDate.year(), currentSysDate.month());
	if (dialogNarah->exec() != 1){
		delete dialogNarah;
		return;
	}
	year=dialogNarah->get_Year();
	month=dialogNarah->get_Month();
	
	QSqlQuery *narahPidrozdilyQuery = new QSqlQuery();
	QSqlQuery *query = new QSqlQuery();
	QSqlTableModel * narahUpdateModel = new QSqlTableModel();
	narahPidrozdilyQuery->exec("SELECT count(*) FROM pidrozdily WHERE Gar_voda=true");
	narahPidrozdilyQuery->seek(0);
	
	//налаштування форми поступу
	UPostupForm *postupForm = new UPostupForm(0, narahPidrozdilyQuery->value(0).toInt());
	postupForm->show();
	
	UOrgTaryfInfo orgTaryf(year, month);
	double narahGarVoda, kilkistTeplaGarVoda;
	
	narahPidrozdilyQuery->exec("SELECT id FROM pidrozdily WHERE Gar_voda=true");
	while (narahPidrozdilyQuery->next()){
		//Проведення нарахування по підрозділу
		query->exec("SELECT PochLich, KincLich, KilkistBilyzny, PochLichKub, KincLichKub \
					FROM pokaz_lich_gar_voda_organiz \
					WHERE Pidrozdil_id="+narahPidrozdilyQuery->value(0).toString()+" \
						and year="+QVariant(year).toString()+" \
						and month="+QVariant(month).toString()+"");
		if (query->seek(0)){
			kilkistTeplaGarVoda=(query->value(1).toDouble()-query->value(0).toDouble())*0.05+ //по лічильнику
							query->value(2).toDouble()*2.375;	//по кількості білизни
			narahGarVoda = d_to_m(kilkistTeplaGarVoda * orgTaryf.vart_g_kal_gar_vody);
			if (narahGarVoda<0)
				narahGarVoda=0;
			double narahGarVodaKub = (query->value(4).toDouble()-query->value(3).toDouble()) * orgTaryf.vart_kuba_GV;
			if (narahGarVodaKub>0)
				narahGarVoda += narahGarVodaKub;
		}
		else{
			kilkistTeplaGarVoda=0;
			narahGarVoda=0;
		}
		
		//Запис нарахування до бази даних
		narahUpdateModel->setTable("operacii");
		narahUpdateModel->setFilter("(Pidrozdil=" + narahPidrozdilyQuery->value(0).toString() + ") \
										and (year=" + QVariant(year).toString() + ") \
										and (month=" + QVariant(month).toString()+")");
		narahUpdateModel->select();
		if (narahUpdateModel->rowCount() == 0){
			narahUpdateModel->insertRows(0, 1);
			QSqlRecord record = narahUpdateModel->record(0);
			record.setValue("id", nextLichylnyk("operacii"));
			record.setValue("Pidrozdil", narahPidrozdilyQuery->value(0));
			record.setValue("Year", year);
			record.setValue("Month", month);
			record.setValue("Narah_opal", 0);
			record.setValue("Narah_voda", narahGarVoda);
			record.setValue("Kilkist_tepla_opal", 0);
			record.setValue("Kilkist_tepla_gar_voda", kilkistTeplaGarVoda);
			narahUpdateModel->setRecord(0, record);
			narahUpdateModel->submitAll();
		}
		else{
			QSqlRecord record = narahUpdateModel->record(0);
			record.setValue("Narah_voda", narahGarVoda);
			record.setValue("Kilkist_tepla_gar_voda", kilkistTeplaGarVoda);
			narahUpdateModel->setRecord(0, record);
			narahUpdateModel->submitAll();
		}
		
		postupForm->inc_pos();
	}
	//Перерахунок вхідних сальдо
	__pererahAllSaldoMonth(month, year, postupForm);
	organizPopulateOperaciiTable();
	
	delete narahUpdateModel;
	delete postupForm;
	delete narahPidrozdilyQuery;
	delete query;
	delete dialogNarah;
}
//------------------------------------------------------------
void tke_MainWindow_org::action_pererah_vhidnogo_saldo_potochnoi_ugody_activated()
{
	tke_Dialog_narah *dialog = new tke_Dialog_narah(this, codec->toUnicode("Вибір місяця для перерахунку"), currentSysDate.year(), currentSysDate.month() );
	if (dialog->exec() == 1){
		if (__pererahCurUgodaSaldo(ui.spinBox_Ugoda->value(), dialog->get_Year(), dialog->get_Month()))
			QMessageBox::information(this,
				codec->toUnicode("!!! Закінчено перерахунок вхідних сальдо !!!"),
				codec->toUnicode("Успішно закінчено перерахунок вхідних сальдо."),
				QMessageBox::Ok,
				QMessageBox::NoButton);
		else
			QMessageBox::critical(this,
				codec->toUnicode("!!! Невдалий перерахунок вхідних сальдо !!!"),
				codec->toUnicode("Програма не змогла записати одне(кілька) вхідних сальдо.\nСпробуйте ще раз.\nПогляньте в відладочне вікно на закладку \"Попередження\" для визначення сальдо, по яких при записі пройшов збій"),
				QMessageBox::Ok,
				QMessageBox::NoButton);
	}
	delete dialog;
	organizPopulateOperaciiTable();
}
//------------------------------------------------------------
void tke_MainWindow_org::action_pererah_vhidnyh_saldo_usih_ugod_activated()
{
	tke_Dialog_narah *dialog = new tke_Dialog_narah(this, codec->toUnicode("Вибір початкового місяця перерахунку вхідних сальдо"), currentSysDate.year(), currentSysDate.month() );
	if (dialog->exec() == 1){
		if (__pererahAllUgodaSaldo(dialog->get_Year(), dialog->get_Month()))
			QMessageBox::information(this,
				codec->toUnicode("!!! Закінчено перерахунок вхідних сальдо !!!"),
				codec->toUnicode("Успішно закінчено перерахунок вхідних сальдо усіх угод."),
				QMessageBox::Ok,
				QMessageBox::NoButton);
		else
			QMessageBox::critical(this,
				codec->toUnicode("!!! Невдалий перерахунок вхідних сальдо !!!"),
				codec->toUnicode("Програма не змогла записати одне(кілька) вхідних сальдо.\nСпробуйте ще раз.\nПогляньте в відладочне вікно на закладку \"Попередження\" для визначення сальдо, по яких при записі пройшов збій"),
				QMessageBox::Ok,
				QMessageBox::NoButton);
	}
	delete dialog;
	organizPopulateOperaciiTable();
}
//------------------------------------------------------------
void tke_MainWindow_org::action_print_narah_gar_voda_activated()
{
	tke_Dialog_narah *dialog_print_narah = new tke_Dialog_narah(this, codec->toUnicode("Відомість нарахування оплати за гарячу воду"), currentSysDate.year(), currentSysDate.month() );
	if (dialog_print_narah->exec() == 1){
		printDocsOrganiz = new UPrintDocsOrganiz();
		printDocsOrganiz->print_vid_narah_za_gar_vodu(dialog_print_narah->get_Year(), dialog_print_narah->get_Month());
		connect(this, SIGNAL(closeLoginSignal()), printDocsOrganiz->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocsOrganiz->printform, SLOT(close()));
		delete printDocsOrganiz;
	}
	delete dialog_print_narah;
}
//------------------------------------------------------------
void tke_MainWindow_org::action_print_vidomist_narahuvannya_i_postuplennya_dohodiv_activated()
{
	tke_Dialog_narah *dialog_print_narah = new tke_Dialog_narah(this, codec->toUnicode("Відомість нарахування і поступлення доходів по підприємству"), currentSysDate.year(), currentSysDate.month() );
	if (dialog_print_narah->exec() == 1){
		printDocsOrganiz = new UPrintDocsOrganiz();
		printDocsOrganiz->print_vidomist_narahuvannya_i_postuplennya_dohodiv(dialog_print_narah->get_Year(), dialog_print_narah->get_Month());
		connect(this, SIGNAL(closeLoginSignal()), printDocsOrganiz->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocsOrganiz->printform, SLOT(close()));
		delete printDocsOrganiz;
	}
	delete dialog_print_narah;
}
//------------------------------------------------------------
void tke_MainWindow_org::action_print_nakopychuvalna_vidomist_narahuvannya_activated()
{
	tke_Dialog_narah *dialog_print_narah = new tke_Dialog_narah(this, codec->toUnicode("Накопичувальна відомість нарахування і поступлення доходів по підприємству"), currentSysDate.year(), currentSysDate.month() );
	if (dialog_print_narah->exec() == 1){
		printDocsOrganiz = new UPrintDocsOrganiz();
		printDocsOrganiz->print_nakopychuvalna_vidomist_narahuvannya_i_postuplennya_dohodiv(dialog_print_narah->get_Year(), dialog_print_narah->get_Month());
		connect(this, SIGNAL(closeLoginSignal()), printDocsOrganiz->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocsOrganiz->printform, SLOT(close()));
		delete printDocsOrganiz;
	}
	delete dialog_print_narah;
}
//------------------------------------------------------------
void tke_MainWindow_org::action_print_vidomist_oplaty_za_CO_activated()
{
	tke_Dialog_narah *dialog_print_narah = new tke_Dialog_narah(this, codec->toUnicode("Відомість нарахування оплати за ЦО"), currentSysDate.year(), currentSysDate.month() );
	if (dialog_print_narah->exec() == 1){
		printDocsOrganiz = new UPrintDocsOrganiz();
		printDocsOrganiz->print_vidomist_oplaty_za_CO(dialog_print_narah->get_Year(), dialog_print_narah->get_Month());
		connect(this, SIGNAL(closeLoginSignal()), printDocsOrganiz->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocsOrganiz->printform, SLOT(close()));
		delete printDocsOrganiz;
	}
	delete dialog_print_narah;
}
//------------------------------------------------------------
void tke_MainWindow_org::action_print_vidomist_realizacii_activated()
{
	tke_Dialog_narah *dialog_print_narah = new tke_Dialog_narah(this, codec->toUnicode("Відомість реалізації"), currentSysDate.year(), currentSysDate.month() );
	if (dialog_print_narah->exec() == 1){
		printDocsOrganiz = new UPrintDocsOrganiz();
		printDocsOrganiz->print_vidomist_realizacii(dialog_print_narah->get_Year(), dialog_print_narah->get_Month());
		connect(this, SIGNAL(closeLoginSignal()), printDocsOrganiz->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocsOrganiz->printform, SLOT(close()));
		delete printDocsOrganiz;
	}
	delete dialog_print_narah;
}
//------------------------------------------------------------
void tke_MainWindow_org::action_print_rahunok_activated()
{
	tkeDialogPrintRahunok *dialogPrintRahunok = new tkeDialogPrintRahunok(currentSysDate.year(),currentSysDate.month(),this);
	if (dialogPrintRahunok->exec() == 1){
		QDir::setCurrent(QCoreApplication::applicationDirPath());
		QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
		int rahType = settings->value("PrintDocs/OrganizRahunokType").toInt();
		delete settings;
		printDocsOrganiz = new UPrintDocsOrganiz();
		if (rahType == 2)
			printDocsOrganiz->print_rahunok_2(dialogPrintRahunok->year(), dialogPrintRahunok->month(), 
								ui.spinBox_Ugoda->value(), dialogPrintRahunok->rahunokType(), dialogPrintRahunok->suma());
		else
			printDocsOrganiz->print_rahunok_1(dialogPrintRahunok->year(), dialogPrintRahunok->month(), 
								ui.spinBox_Ugoda->value(), dialogPrintRahunok->rahunokType(), dialogPrintRahunok->suma());
		connect(this, SIGNAL(closeLoginSignal()), printDocsOrganiz->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocsOrganiz->printform, SLOT(close()));
		delete printDocsOrganiz;
	}
	delete dialogPrintRahunok;
}
//------------------------------------------------------------
void tke_MainWindow_org::action_print_act_vykon_robit_activated()
{
    tke_Dialog_narah *dialogPrintRahunok = new tke_Dialog_narah(this, "Вибір місяця", currentSysDate.year(),currentSysDate.month());
    if (dialogPrintRahunok->exec() == 1){
        QDir::setCurrent(QCoreApplication::applicationDirPath());
        printDocsOrganiz = new UPrintDocsOrganiz();
        printDocsOrganiz->print_act_vykon_robit(QDate(dialogPrintRahunok->get_Year(), dialogPrintRahunok->get_Month(),1),
                                ui.spinBox_Ugoda->value());
        connect(this, SIGNAL(closeLoginSignal()), printDocsOrganiz->printform, SLOT(close()));
        connect(this, SIGNAL(mainWindowClosed()), printDocsOrganiz->printform, SLOT(close()));
        delete printDocsOrganiz;
    }
    delete dialogPrintRahunok;
}
//------------------------------------------------------------
void tke_MainWindow_org::action_print_vidomist_borjnykiv_organiz_activated()
{
	tkeDialogFiltrBorjnykivOrganiz *filtrBorjnykiv = new tkeDialogFiltrBorjnykivOrganiz(this);
	QDate xdate(currentSysDate.year(),currentSysDate.month(),1);
	//xdate = xdate.addMonths(-1);
	filtrBorjnykiv->setPochDate(xdate);
	filtrBorjnykiv->setKincDate(xdate);
	if (filtrBorjnykiv->exec() == 1){
		printDocsOrganiz = new UPrintDocsOrganiz();
		printDocsOrganiz->print_vidomist_borjnykiv(filtrBorjnykiv->pochDate(), filtrBorjnykiv->kincDate(), filtrBorjnykiv->curFinansFromId(), 
													filtrBorjnykiv->neadMinSum(), filtrBorjnykiv->minSum());
		connect(this, SIGNAL(closeLoginSignal()), printDocsOrganiz->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocsOrganiz->printform, SLOT(close()));
		delete printDocsOrganiz;
	}
	delete filtrBorjnykiv;
}
//------------------------------------------------------------
void tke_MainWindow_org::action_print_podatkova_nakladna_activated()
{
	
	
	tke_dialog_print_nakladna *dialog = new tke_dialog_print_nakladna(currentSysDate.year(),currentSysDate.month(),this);
	if (dialog->exec() == 1){
		printDocsOrganiz = new UPrintDocsOrganiz();
		printDocsOrganiz->print_podatkova_nakladna(dialog->year(), dialog->month(), 
							ui.spinBox_Ugoda->value(), dialog->sumaCO(), dialog->sumaGV());
		connect(this, SIGNAL(closeLoginSignal()), printDocsOrganiz->printform, SLOT(close()));
		connect(this, SIGNAL(mainWindowClosed()), printDocsOrganiz->printform, SLOT(close()));
		delete printDocsOrganiz;
	}
	delete dialog;
}
//------------------------------------------------------------
void tke_MainWindow_org::action_calc_saldo_spec_rahunkiv_activated()
{
	int month, year;
	tke_Dialog_narah *dialogNarah = new tke_Dialog_narah(this, codec->toUnicode("Перекидування сальдо в спеціальних рахунках"), currentSysDate.year(), currentSysDate.month());
	if (dialogNarah->exec() != 1){
		delete dialogNarah;
		return;
	}
	year=dialogNarah->get_Year();
	month=dialogNarah->get_Month();
	
	QSqlQuery *query = new QSqlQuery();
	QSqlTableModel *operaciiModel = new QSqlTableModel();
	query->exec("SELECT count(*) FROM slujb_rahunky");
	query->seek(0);
	
	UPostupForm *postupForm = new UPostupForm(0, query->value(0).toInt());
	postupForm->showLable(true);
	postupForm->setPostupLabel(codec->toUnicode("Перекидування сальдо в спеціальних рахунках на ")+ QVariant(month).toString()+
									"."+QVariant(year).toString()+"p.");
	postupForm->show();
	
	int prevYear=year;
	int prevMonth=month-1;
	if (prevMonth==0){
		prevMonth=12;
		prevYear--;
	}
	
	query->exec("SELECT slujb_rahunky.Rahunok, operacii_rahunkiv.Vh_saldo, operacii_rahunkiv.Narah, "
						"operacii_rahunkiv.Oplata "
				"FROM slujb_rahunky "
				"LEFT JOIN (SELECT * FROM operacii_rahunkiv WHERE year="+QVariant(prevYear).toString()+" and month="+QVariant(prevMonth).toString()+") AS operacii_rahunkiv "
				"ON operacii_rahunkiv.Slujb_rahunok_id=slujb_rahunky.Rahunok");
	while (query->next()){
		operaciiModel->setTable("operacii_rahunkiv");
		operaciiModel->setFilter("Slujb_rahunok_id="+query->value(0).toString()+" and year="+QVariant(year).toString()+" and month=" +QVariant(month).toString());
		operaciiModel->select();
		QSqlRecord record;
		if (operaciiModel->rowCount() == 0){
			qDebug() << "new record";
			operaciiModel->insertRows(0, 1);
			record = operaciiModel->record(0);
			record.setValue("id", nextLichylnyk("operacii_rahunkiv"));
			record.setValue("year", year);
			record.setValue("month", month);
			record.setValue("Slujb_rahunok_id", query->value(0));
			record.setValue("Vh_saldo", d_to_m(query->value(1).toDouble()+query->value(2).toDouble()-query->value(3).toDouble()));
			record.setValue("Narah", 0);
			record.setValue("Oplata", 0);
			operaciiModel->setRecord(0, record);
			operaciiModel->submitAll();
		}
		else{
			qDebug() << "update record";
			record = operaciiModel->record(0);
			record.setValue("Vh_saldo", d_to_m(query->value(1).toDouble()+query->value(2).toDouble()-query->value(3).toDouble()));
			operaciiModel->setRecord(0, record);
			operaciiModel->submitAll();
		}
		postupForm->inc_pos();
	}
	
	organizSlujbRahPopulateOperaciiTable();
	
	QMessageBox::information(this,
			codec->toUnicode("!!! Закінчено перекидування вхідних сальдо !!!"),
			codec->toUnicode("Закінчено перекидування вхідних сальдо."),
			QMessageBox::Ok,
			QMessageBox::NoButton);

	
	delete query;
	delete operaciiModel;
	delete postupForm;
}
//------------------------------------------------------------
void tke_MainWindow_org::action_pererahunok_saldo_usih_spec_rahunkiv_activated()
{
	int month, year;
	tke_Dialog_narah *dialogNarah = new tke_Dialog_narah(this, codec->toUnicode("Перерах. вх.сальдо в усіх спеціальних рахунках з дати..."), currentSysDate.year(), currentSysDate.month());
	if (dialogNarah->exec() != 1){
		delete dialogNarah;
		return;
	}
	year=dialogNarah->get_Year();
	month=dialogNarah->get_Month();
	
	QSqlQuery *query = new QSqlQuery();
	QSqlQuery *rahunkyQuery = new QSqlQuery();
	QSqlQuery *updateQuery = new QSqlQuery();
	query->exec("SELECT count(*) FROM slujb_rahunky");
	query->seek(0);
	
	UPostupForm *postupForm = new UPostupForm(0, query->value(0).toInt());
	postupForm->showLable(true);
	postupForm->setPostupLabel(codec->toUnicode("Перерах. вх.сальдо в усіх спеціальних рахунках з ")+ QVariant(month).toString()+
									"."+QVariant(year).toString()+"p.");
	postupForm->show();
	
	rahunkyQuery->exec("SELECT Rahunok FROM slujb_rahunky");
	bool done_ok=true;
	double prevVyhSaldo=0;
	while (rahunkyQuery->next()){
		query->exec("SELECT id, Vh_saldo, Narah, Oplata \
						FROM operacii_rahunkiv \
						WHERE Slujb_rahunok_id="+rahunkyQuery->value(0).toString()+
								" and (year>"+QVariant(year).toString()+
								" or (year="+QVariant(year).toString()+" and month>="+QVariant(month).toString()+")) \
						ORDER BY year, month");
		if (query->seek(0)){
			prevVyhSaldo = query->value(1).toDouble() + query->value(2).toDouble() - query->value(3).toDouble();
		}
		while (query->next()){
			done_ok = done_ok && updateQuery->exec("UPDATE operacii_rahunkiv SET Vh_saldo="+uMToStr2Krapka(prevVyhSaldo)+" \
								WHERE id="+query->value(0).toString());
			prevVyhSaldo = prevVyhSaldo + query->value(2).toDouble() - query->value(3).toDouble();
		}
		postupForm->inc_pos();
	}
	
	organizSlujbRahPopulateOperaciiTable();
	
	if (done_ok)
		QMessageBox::information(this,
				codec->toUnicode("!!! Закінчено перерахунок вхідних сальдо !!!"),
				codec->toUnicode("Успішно закінчено перерахунок вхідних сальдо."),
				QMessageBox::Ok,
				QMessageBox::NoButton);
	else
		QMessageBox::critical(this,
				codec->toUnicode("!!! Невдалий перерахунок вхідних сальдо !!!"),
				codec->toUnicode("Програма не змогла записати одне(кілька) вхідних сальдо.\nСпробуйте ще раз."),
				QMessageBox::Ok,
				QMessageBox::NoButton);
	
	delete query;
	delete rahunkyQuery;
	delete updateQuery;
	delete postupForm;
}
//------------------------------------------------------------
void tke_MainWindow_org::action_Options_activated()
{
	tke_dialog_options * dOptions = new tke_dialog_options();
	connect(dOptions, SIGNAL(requestReloadIniSettings()), this, SLOT(loadGUISettingsFromIni()));
	dOptions->exec();
	delete dOptions;
}
//------------------------------------------------------------
void tke_MainWindow_org::action_saveDatabase_activated()
{
	saveDataBase();
}
//------------------------------------------------------------
void tke_MainWindow_org::action_assistent_activated()
{
    /*
    QStringList list;
	list << "-profile" << "../docs/tkeAssistant.adp";
	if (assistant == NULL){
		assistant = new QAssistantClient("./", this);
		assistant->setArguments(list);
		connect(this, SIGNAL(mainWindowClosed()), assistant, SLOT(closeAssistant()));
	}
	assistant->openAssistant();
    */

    QMessageBox::information(this, "Not implemented", "Functionality not implemeted");
}
//------------------------------------------------------------
void tke_MainWindow_org::action_about_activated()
{
#ifndef TKE_DEMO
	QFile *aboutFile = new QFile("about.html");
#else
	QFile *aboutFile = new QFile("aboutDemo.html");
#endif
	aboutFile->open(QFile::ReadOnly);
	QTextStream *aboutText = new QTextStream(aboutFile);
	QMessageBox::about (this,codec->toUnicode("Про програму"),aboutText->readAll());
	delete aboutFile;
	delete aboutText;
}
//------------------------------------------------------------
void tke_MainWindow_org::buttonclick_add_taryf()
{
	int row = model_taryf->rowCount();
	model_taryf->insertRow(row);
	
	QSqlQuery *tempQuery = new QSqlQuery();
	tempQuery->exec("SELECT max(id) FROM normat_taryf_organiz");
	tempQuery->seek(0);
	int newId = tempQuery->value(0).toInt();
	delete tempQuery;
	
	QSqlRecord record = model_taryf->record(row);
	record.setValue("id", QVariant(newId+1));
	record.setValue("Year", QVariant(d_Year));
	record.setValue("Month", QVariant(d_Month));
	model_taryf->setRecord(row, record);
	
	QModelIndex index = model_taryf->index(row, 1);
	ui.tableView_taryf->setCurrentIndex(index);
	ui.tableView_taryf->edit(index);
}
//------------------------------------------------------------
void tke_MainWindow_org::buttonclick_del_taryf()
{
	if	( ! QMessageBox::question(
                this,
                codec->toUnicode("!!! Видалення стрічки !!!"),
                codec->toUnicode("Ви дійсно бажаєте видалити сирічку?"),
                codec->toUnicode("Так"),
				codec->toUnicode("Ні-ні-ні"),
                QString(), 0, 1)
		)
		model_taryf->removeRow(ui.tableView_taryf->currentIndex().row());
}
//------------------------------------------------------------
void tke_MainWindow_org::buttonclick_add_oplata()
{
	int row = modelOplata->rowCount();
	modelOplata->insertRow(row);
	
	QSqlRecord record = modelOplata->record(row);
	record.setValue("Organiz_id", QVariant(d_Ugoda));
	record.setValue("Pererah_date", d_Perevid_Date);
	record.setValue("Suma", 100);
	modelOplata->setRecord(row, record);
	
	QModelIndex index = modelOplata->index(row, 1);
	ui.tableView_oplata->setCurrentIndex(index);
	ui.tableView_oplata->edit(index);
}
//------------------------------------------------------------
void tke_MainWindow_org::buttonclick_del_oplata()
{
	if	( ! QMessageBox::question(
                this,
                codec->toUnicode("!!! Видалення стрічки !!!"),
                codec->toUnicode("Ви дійсно бажаєте видалити сирічку?"),
                codec->toUnicode("Так"),
				codec->toUnicode("Ні-ні-ні"),
                QString(), 0, 1)
		)
		modelOplata->removeRow(ui.tableView_oplata->currentIndex().row());
}
//------------------------------------------------------------
void tke_MainWindow_org::buttonclickSavePokazLichOpal()
{
	QSqlTableModel *model = new QSqlTableModel();
	QSqlQuery *query = new QSqlQuery();
	QSqlRecord record;
	
	model->setTable("pokaz_lich_opal_organiz");
	for (int i=0; i<ui.tableWidget_pokaz_lich->rowCount(); i++){
		if (ui.tableWidget_pokaz_lich->item( i, 0)->text().toInt() ==0 ){  //insert record
			query->exec("SELECT max(id) FROM pokaz_lich_opal_organiz");
			query->seek(0);
			int newRowId = query->value(0).toInt();
			model->insertRow(model->rowCount());
			record = model->record(model->rowCount()-1);
			record.setValue("id", newRowId+1);
			record.setValue("Year", ui.spinBox_year_pokaz_lich->value());
			record.setValue("Month", ui.spinBox_month_pokaz_lich->value());
			record.setValue("Osn_kilk_tepla", ui.tableWidget_pokaz_lich->item( i, 1)->text().toDouble());
			record.setValue("Dod_kilk_tepla", ui.tableWidget_pokaz_lich->item( i, 2)->text().toDouble());
			record.setValue("Pidrozdil_ID", ui.tableWidget_pokaz_lich->item( i, 3)->text());
			model->setRecord(model->rowCount()-1, record);
			model->submitAll();
		}
		else{   //Update Record
			model->setFilter("id="+ui.tableWidget_pokaz_lich->item( i, 0)->text());
			model->select();
			if (model->rowCount() == 1){
				record = model->record(0);
				record.setValue("Osn_kilk_tepla", ui.tableWidget_pokaz_lich->item( i, 1)->text().toDouble());
				record.setValue("Dod_kilk_tepla", ui.tableWidget_pokaz_lich->item( i, 2)->text().toDouble());
				model->setRecord(0, record);
				model->submitAll();
			}
		}
	}
	__execPokazLichSelectQuery(ui.spinBox_month_pokaz_lich->value(), ui.spinBox_year_pokaz_lich->value());
	delete model;
	delete query;
}
//------------------------------------------------------------
void tke_MainWindow_org::buttonclickSavePokazLichGarVodaOpal()
{
	QSqlTableModel *model = new QSqlTableModel();
	QSqlQuery *query = new QSqlQuery();
	QSqlRecord record;
	
	model->setTable("pokaz_lich_gar_voda_organiz");
	for (int i=0; i<ui.tableWidget_pokaz_lich_gar_voda->rowCount(); i++){
		if (ui.tableWidget_pokaz_lich_gar_voda->item( i, 0)->text().toInt() ==0 ){  //insert record
			model->insertRow(model->rowCount());
			record = model->record(model->rowCount()-1);
			record.setValue("id", nextLichylnyk("pokaz_lich_gar_voda_organiz"));
			record.setValue("Year", ui.spinBox_year_pokaz_lich_gar_voda->value());
			record.setValue("Month", ui.spinBox_month_pokaz_lich_gar_voda->value());
			record.setValue("PochLich", ui.tableWidget_pokaz_lich_gar_voda->item( i, 1)->text().toDouble());
			record.setValue("KincLich", ui.tableWidget_pokaz_lich_gar_voda->item( i, 2)->text().toDouble());
			record.setValue("KilkistBilyzny", ui.tableWidget_pokaz_lich_gar_voda->item( i, 3)->text().toDouble());
			record.setValue("Pidrozdil_id", ui.tableWidget_pokaz_lich_gar_voda->item( i, 4)->text());
			record.setValue("PochLichKub", ui.tableWidget_pokaz_lich_gar_voda->item( i, 5)->text().toDouble());
			record.setValue("KincLichKub", ui.tableWidget_pokaz_lich_gar_voda->item( i, 6)->text().toDouble());
			model->setRecord(model->rowCount()-1, record);
			model->submitAll();
		}
		else{   //Update Record
			model->setFilter("id="+ui.tableWidget_pokaz_lich_gar_voda->item( i, 0)->text());
			model->select();
			if (model->rowCount() == 1){
				record = model->record(0);
				record.setValue("PochLich", ui.tableWidget_pokaz_lich_gar_voda->item( i, 1)->text().toDouble());
				record.setValue("KincLich", ui.tableWidget_pokaz_lich_gar_voda->item( i, 2)->text().toDouble());
				record.setValue("KilkistBilyzny", ui.tableWidget_pokaz_lich_gar_voda->item( i, 3)->text().toDouble());
				record.setValue("PochLichKub", ui.tableWidget_pokaz_lich_gar_voda->item( i, 5)->text().toDouble());
				record.setValue("KincLichKub", ui.tableWidget_pokaz_lich_gar_voda->item( i, 6)->text().toDouble());
				model->setRecord(0, record);
				model->submitAll();
			}
		}
	}
	__execPokazLichGarVodaSelectQuery(ui.spinBox_month_pokaz_lich_gar_voda->value(), ui.spinBox_year_pokaz_lich_gar_voda->value());
	delete model;
	delete query;
}
//------------------------------------------------------------
void tke_MainWindow_org::buttonclickSaveMeteorOpal()
{
	QSqlTableModel *model = new QSqlTableModel();
	QSqlRecord record;
	model->setTable("meteorolog");
	for (int i=0; i<ui.tableWidget_meteor->rowCount(); i++){
		if (ui.tableWidget_meteor->item( i, 0)->text().toInt() ==0 ){  //insert record
			if (ui.tableWidget_meteor->item( i, 1)->text() != "-"){ //Провірка, чи температура не дорівнює значенню "-" (відсутня)
				model->insertRow(model->rowCount());
				record = model->record(model->rowCount()-1);
				record.setValue("id", nextLichylnyk("meteorolog"));
				record.setValue("Date_pokazn", QDate(ui.spinBox_year_meteor_table->value(), 
									ui.spinBox_month_meteor_table->value(), i+1));
				record.setValue("Temper", ui.tableWidget_meteor->item( i, 1)->text().toDouble());
				model->setRecord(model->rowCount()-1, record);
				model->submitAll();
			}
		}
		else{   //Update Record
			model->setFilter("id="+ui.tableWidget_meteor->item( i, 0)->text());
			model->select();
			if (model->rowCount() == 1){
				record = model->record(0);
				record.setValue("Temper", ui.tableWidget_meteor->item( i, 1)->text().toDouble());
				model->setRecord(0, record);
				model->submitAll();
			}
		}
	}
	__execMeteorSelectQuery(ui.spinBox_month_meteor_table->value(), ui.spinBox_year_meteor_table->value());
	
	delete model;
}
//------------------------------------------------------------
void tke_MainWindow_org::pokazLichDateChanged()
{
	int month = date.month();
	int year = date.year();

	if (ui.spinBox_year_pokaz_lich->value() == date.year()){
		month++;
		if (month == 13)
			month=12;
		ui.spinBox_month_pokaz_lich->setMaximum(month);
	}
	else if (ui.spinBox_year_pokaz_lich->value() < date.year())
		ui.spinBox_month_pokaz_lich->setMaximum(12);
	else
		ui.spinBox_month_pokaz_lich->setMaximum(1);
	__execPokazLichSelectQuery(ui.spinBox_month_pokaz_lich->value(), ui.spinBox_year_pokaz_lich->value());
	//Заповнення колонок висвітлення нормативних тарифів для даного місяця
	month = ui.spinBox_month_pokaz_lich->value();
	year = ui.spinBox_year_pokaz_lich->value();
	
	UOrgTaryfInfo orgTaryf(year, month);
	ui.pokaz_lich_label_norm_taryf_gkal->setText ( codec->toUnicode("ГКал.:") + uMToStr2(orgTaryf.vart_g_kal(FinaceTypeAny)) );
}
//------------------------------------------------------------
void tke_MainWindow_org::pokazLichGarVodaDateChanged()
{
	int month = date.month();
	int year = date.year();

	if (ui.spinBox_year_pokaz_lich_gar_voda->value() == date.year()){
		month++;
		if (month == 13)
			month=12;
		ui.spinBox_month_pokaz_lich_gar_voda->setMaximum(month);
	}
	else if (ui.spinBox_year_pokaz_lich_gar_voda->value() < date.year())
		ui.spinBox_month_pokaz_lich_gar_voda->setMaximum(12);
	else
		ui.spinBox_month_pokaz_lich_gar_voda->setMaximum(1);
	__execPokazLichGarVodaSelectQuery(ui.spinBox_month_pokaz_lich_gar_voda->value(), ui.spinBox_year_pokaz_lich_gar_voda->value());
	//Заповнення колонок висвітлення нормативних тарифів для даного місяця
	month = ui.spinBox_month_pokaz_lich_gar_voda->value();
	year = ui.spinBox_year_pokaz_lich_gar_voda->value();
	
	UOrgTaryfInfo orgTaryf(year, month);
	ui.pokaz_lich_label_norm_taryf_gkal_gar_voda->setText( "ГКал.:" + uMToStr2(orgTaryf.vart_g_kal_gar_vody)+"; Куб.ГВ:"+uMToStr2(orgTaryf.vart_kuba_GV) );
}
//------------------------------------------------------------
void tke_MainWindow_org::meteorDateChanged()
{
	int month = date.month();

	if (ui.spinBox_year_meteor_table->value() == date.year()){
		month++;
		if (month == 13)
			month=12;
		ui.spinBox_month_meteor_table->setMaximum(month);
	}
	else if (ui.spinBox_year_meteor_table->value() < date.year())
		ui.spinBox_month_meteor_table->setMaximum(12);
	else
		ui.spinBox_month_meteor_table->setMaximum(1);
	__execMeteorSelectQuery(ui.spinBox_month_meteor_table->value(), ui.spinBox_year_meteor_table->value());
}
//------------------------------------------------------------
void tke_MainWindow_org::dniOpalDateChanged()
{
	int year, month = date.month();

	if (ui.spinBox_year_dni_opal->value() == date.year()){
		month++;
		if (month == 13)
			month=12;
		ui.spinBox_month_dni_opal->setMaximum(month);
	}
	else if (ui.spinBox_year_dni_opal->value() < date.year())
		ui.spinBox_month_dni_opal->setMaximum(12);
	else
		ui.spinBox_month_dni_opal->setMaximum(1);
	//__execDniOpalSelectModel(ui.spinBox_month_dni_opal->value(), ui.spinBox_year_dni_opal->value());
	year = ui.spinBox_year_dni_opal->value();
	month = ui.spinBox_month_dni_opal->value();
	dniOpalQueryModel->setQuery("SELECT pidrozdily.id, pidrozdily.Ugoda, 'N ' & pidrozdily.Ugoda & ' ' & organizacii.Nazva &'/' & pidrozdily.Name AS Pidrozdil_name, \
										NoLichOpal.Num_days_opal, LichOpal.Num_days_opal_lich \
								FROM ((pidrozdily \
								LEFT JOIN (SELECT Pidrozdil, sum(Day_kinc_opal-Day_poch_opal+1) AS Num_days_opal \
											FROM grafik_opal WHERE year(Opal_date)="+QVariant(year).toString()+" and month(Opal_date)="+QVariant(month).toString()+" and Opal_lich=1 \
											GROUP BY Pidrozdil) AS NoLichOpal \
								ON NoLichOpal.Pidrozdil=pidrozdily.id)\
								LEFT JOIN (SELECT Pidrozdil, sum(Day_kinc_opal-Day_poch_opal+1) AS Num_days_opal_lich \
											FROM grafik_opal WHERE year(Opal_date)="+QVariant(year).toString()+" and month(Opal_date)="+QVariant(month).toString()+" and Opal_lich=2 \
											GROUP BY Pidrozdil) AS LichOpal \
								ON LichOpal.Pidrozdil=pidrozdily.id) \
								LEFT JOIN organizacii ON organizacii.Ugoda=pidrozdily.Ugoda WHERE pidrozdily.Opal=1 \
								ORDER BY pidrozdily.Ugoda");
}
//------------------------------------------------------------
void tke_MainWindow_org::dniOpalOpenEditForm(const QModelIndex & index)
{
	formDaysOpal = new tkeFormDaysOpal(0, dniOpalQueryModel, ui.tableView_dni_opal, 
										ui.spinBox_year_dni_opal->value(), ui.spinBox_month_dni_opal->value());
	formDaysOpal->setWindowModality(Qt::ApplicationModal);
	connect(formDaysOpal->dniOpalTableModel(), SIGNAL(dataChanged(const QModelIndex &,const QModelIndex &)), this, SLOT(dniOpalDateChanged()));
	formDaysOpal->show();
}
//------------------------------------------------------------
void tke_MainWindow_org::oplataDateChanged()
{
	int year, month = date.month();

	if (ui.spinBox_year_oplata->value() == date.year()){
		month++;
		if (month == 13)
			month=12;
		ui.spinBox_month_oplata->setMaximum(month);
	}
	else if (ui.spinBox_year_oplata->value() < date.year())
		ui.spinBox_month_oplata->setMaximum(12);
	else
		ui.spinBox_month_oplata->setMaximum(1);
	
	year = ui.spinBox_year_oplata->value();
	month = ui.spinBox_month_oplata->value();
	d_Perevid_Date.setDate(ui.spinBox_year_oplata->value(), ui.spinBox_month_oplata->value(), 1);
	
	modelOplata->setFilter("year(Pererah_date)="+QVariant(year).toString()+" \
							and month(Pererah_date)="+QVariant(month).toString());
	ui.tableView_oplata->resizeColumnsToContents();
}
//------------------------------------------------------------
void tke_MainWindow_org::pokaz_lich_opal_widget_item_changed(QTableWidgetItem * item)
{
	item->setBackgroundColor( Qt::green );
}
//------------------------------------------------------------
void tke_MainWindow_org::meteor_widget_item_changed(QTableWidgetItem * item)
{
	item->setBackgroundColor( Qt::green );
}
//------------------------------------------------------------
//------------------------------------------------------------
//-------------functions-----------------------------------
int tke_MainWindow_org::__insertPidrozdilRecord()
{
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT max(id) FROM pidrozdily");
	query->seek(0);
	int iterId = query->value(0).toInt();
	delete query;
	modelInsertPidrozdily->insertRows(0,1);
	xRecord = modelInsertPidrozdily->record(0);
	xRecord.setValue("id", iterId+1);
	xRecord.setValue("Ugoda", ui.spinBox_Ugoda->value());
	xRecord.setValue("Name", "- - -");
	xRecord.setValue("Opal", 1);
	xRecord.setValue("Gar_voda", 0);
	xRecord.setValue("Ploshcha", 0);
	xRecord.setValue("Obem", 0);
	xRecord.setValue("Tepl_harakt", 0);
	xRecord.setValue("T_vnutr", 0);
	xRecord.setValue("Tepl_navantaj", 0);
	xRecord.setValue("Lich", 0);
	xRecord.setValue("Kotelnya_ID", 0);
	xRecord.setValue("Koef_lich", 1.163);
	
	modelInsertPidrozdily->setRecord(0, xRecord);
	if (!modelInsertPidrozdily->submitAll()){
		QMessageBox::critical(this,
			codec->toUnicode("!!! Невдале додавання запису підрозділу!!!"),
			codec->toUnicode("Програма не змогла додати запис підрозділу до бази даних.\n") 
							+ modelInsertPidrozdily->lastError().text(),
			QMessageBox::Ok,
			QMessageBox::NoButton);
	}
	return iterId+1;
}
//------------------------------------------------------------
void tke_MainWindow_org::__execPokazLichSelectQuery(int monthFiltr, int yearFiltr)
{
	disconnect(ui.tableWidget_pokaz_lich, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(pokaz_lich_opal_widget_item_changed(QTableWidgetItem *)));
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT count(*) FROM pidrozdily WHERE pidrozdily.Lich=1");
	query->seek(0);
	ui.tableWidget_pokaz_lich->setRowCount(query->value(0).toInt());
	query->exec("SELECT pidrozdily.Ugoda & ' ' & organizacii.Nazva, pokaz_lich_opal_organiz.id, \
					pokaz_lich_opal_organiz.Osn_kilk_tepla, pokaz_lich_opal_organiz.Dod_kilk_tepla, pidrozdily.id, pidrozdily.Name \
					FROM ((SELECT * FROM pokaz_lich_opal_organiz WHERE year="+QVariant(yearFiltr).toString()+" and month="+QVariant(monthFiltr).toString()+") AS pokaz_lich_opal_organiz \
					RIGHT JOIN pidrozdily ON pokaz_lich_opal_organiz.Pidrozdil_ID = pidrozdily.id) \
					LEFT JOIN organizacii ON organizacii.Ugoda=pidrozdily.Ugoda  \
					WHERE pidrozdily.Lich=1 ORDER BY pidrozdily.Ugoda, pidrozdily.Name");
	QStringList *strList = new QStringList;
	int pokazLichRow=0;
	while (query->next()){
		*strList << query->value(0).toString()+"/\n-->"+query->value(5).toString();
		ui.tableWidget_pokaz_lich->setItem(pokazLichRow, 0, new QTableWidgetItem(query->value(1).toString()));
		ui.tableWidget_pokaz_lich->setItem(pokazLichRow, 1, new QTableWidgetItem(query->value(2).toString()));
		ui.tableWidget_pokaz_lich->setItem(pokazLichRow, 2, new QTableWidgetItem(query->value(3).toString()));
		if (query->value(1).toInt() == 0){
			ui.tableWidget_pokaz_lich->item(pokazLichRow,1)->setBackgroundColor ( Qt::cyan );
			ui.tableWidget_pokaz_lich->item(pokazLichRow,2)->setBackgroundColor ( Qt::cyan );
		}
		ui.tableWidget_pokaz_lich->setItem(pokazLichRow, 3, new QTableWidgetItem(query->value(4).toString()));
		
		ui.tableWidget_pokaz_lich->item(pokazLichRow,1)->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );
		ui.tableWidget_pokaz_lich->item(pokazLichRow,2)->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );
		
		pokazLichRow++;
	}
	ui.tableWidget_pokaz_lich->setVerticalHeaderLabels(*strList);
	ui.tableWidget_pokaz_lich->resizeRowsToContents();
	delete strList;
	delete query;
	connect(ui.tableWidget_pokaz_lich, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(pokaz_lich_opal_widget_item_changed(QTableWidgetItem *)));
}
//------------------------------------------------------------
void tke_MainWindow_org::__execPokazLichGarVodaSelectQuery(int monthFiltr, int yearFiltr)
{
	disconnect(ui.tableWidget_pokaz_lich_gar_voda, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(pokaz_lich_opal_widget_item_changed(QTableWidgetItem *)));
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT count(*) FROM pidrozdily WHERE pidrozdily.Gar_voda=true");
	query->seek(0);
	ui.tableWidget_pokaz_lich_gar_voda->setRowCount(query->value(0).toInt());
	query->exec("SELECT pidrozdily.Ugoda & ' ' & organizacii.Nazva, pokaz_lich_gar_voda_organiz.id, \
						pokaz_lich_gar_voda_organiz.PochLich, pokaz_lich_gar_voda_organiz.KincLich, \
						pokaz_lich_gar_voda_organiz.KilkistBilyzny, pidrozdily.id, pidrozdily.Name, \
						pokaz_lich_gar_voda_organiz.PochLichKub, pokaz_lich_gar_voda_organiz.KincLichKub\
				FROM ((SELECT * FROM pokaz_lich_gar_voda_organiz \
						WHERE year="+QVariant(yearFiltr).toString()+" \
							and month="+QVariant(monthFiltr).toString()+") AS pokaz_lich_gar_voda_organiz \
				RIGHT JOIN pidrozdily \
					ON pokaz_lich_gar_voda_organiz.Pidrozdil_ID = pidrozdily.id) \
				LEFT JOIN organizacii \
					ON organizacii.Ugoda=pidrozdily.Ugoda  \
				WHERE pidrozdily.Gar_voda=true \
				ORDER BY pidrozdily.Ugoda, pidrozdily.Name");
	QStringList *strList = new QStringList;
	int pokazLichRow=0;
	while (query->next()){
		*strList << query->value(0).toString()+"/\n-->"+query->value(6).toString();
		ui.tableWidget_pokaz_lich_gar_voda->setItem(pokazLichRow, 0, new QTableWidgetItem(query->value(1).toString()));
		ui.tableWidget_pokaz_lich_gar_voda->setItem(pokazLichRow, 1, new QTableWidgetItem(QString::number(query->value(2).toDouble()).replace(".",",")));
		ui.tableWidget_pokaz_lich_gar_voda->setItem(pokazLichRow, 2, new QTableWidgetItem(QString::number(query->value(3).toDouble()).replace(".",",")));
		ui.tableWidget_pokaz_lich_gar_voda->setItem(pokazLichRow, 3, new QTableWidgetItem(QString::number(query->value(4).toDouble()).replace(".",",")));
		ui.tableWidget_pokaz_lich_gar_voda->setItem(pokazLichRow, 5, new QTableWidgetItem(QString::number(query->value(7).toDouble()).replace(".",",")));
		ui.tableWidget_pokaz_lich_gar_voda->setItem(pokazLichRow, 6, new QTableWidgetItem(QString::number(query->value(8).toDouble()).replace(".",",")));
		if (query->value(1).toInt() == 0){
			ui.tableWidget_pokaz_lich_gar_voda->item(pokazLichRow,1)->setBackgroundColor ( Qt::cyan );
			ui.tableWidget_pokaz_lich_gar_voda->item(pokazLichRow,2)->setBackgroundColor ( Qt::cyan );
			ui.tableWidget_pokaz_lich_gar_voda->item(pokazLichRow,3)->setBackgroundColor ( Qt::cyan );
			ui.tableWidget_pokaz_lich_gar_voda->item(pokazLichRow,5)->setBackgroundColor ( Qt::cyan );
			ui.tableWidget_pokaz_lich_gar_voda->item(pokazLichRow,6)->setBackgroundColor ( Qt::cyan );
		}
		ui.tableWidget_pokaz_lich_gar_voda->setItem(pokazLichRow, 4, new QTableWidgetItem(query->value(5).toString()));
		
		ui.tableWidget_pokaz_lich_gar_voda->item(pokazLichRow,1)->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );
		ui.tableWidget_pokaz_lich_gar_voda->item(pokazLichRow,2)->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );
		ui.tableWidget_pokaz_lich_gar_voda->item(pokazLichRow,3)->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );
		ui.tableWidget_pokaz_lich_gar_voda->item(pokazLichRow,5)->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );
		ui.tableWidget_pokaz_lich_gar_voda->item(pokazLichRow,6)->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );
		
		pokazLichRow++;
	}
	ui.tableWidget_pokaz_lich_gar_voda->setVerticalHeaderLabels(*strList);
	ui.tableWidget_pokaz_lich_gar_voda->resizeColumnsToContents();
	ui.tableWidget_pokaz_lich_gar_voda->resizeRowsToContents();
	delete strList;
	delete query;
	connect(ui.tableWidget_pokaz_lich_gar_voda, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(pokaz_lich_opal_widget_item_changed(QTableWidgetItem *)));
}
//------------------------------------------------------------
void tke_MainWindow_org::__execMeteorSelectQuery(int monthFiltr, int yearFiltr)
{
	disconnect(ui.tableWidget_meteor, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(meteor_widget_item_changed(QTableWidgetItem *)));
	short num_days_in_month=days_in_month[monthFiltr-1];
	ui.tableWidget_meteor->setRowCount(num_days_in_month);
	
	for (int i=0; i<num_days_in_month; i++){
		ui.tableWidget_meteor->setItem(i, 0, new QTableWidgetItem("0"));
		ui.tableWidget_meteor->setItem(i, 1, new QTableWidgetItem("-"));
		ui.tableWidget_meteor->item(i,1)->setTextAlignment( Qt::AlignRight );
	}
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT * FROM meteorolog \
					WHERE year(Date_pokazn)="+QVariant(yearFiltr).toString()+" and month(Date_pokazn)="+QVariant(monthFiltr).toString());
	while (query->next()){
		int curRow = query->value(1).toDate().day()-1;
		ui.tableWidget_meteor->item(curRow,0)->setText(query->value(0).toString());
		ui.tableWidget_meteor->item(curRow,1)->setText(uMToStr2(query->value(2).toDouble()));
	}
	ui.tableWidget_meteor->resizeRowsToContents();
	delete query;
	connect(ui.tableWidget_meteor, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(meteor_widget_item_changed(QTableWidgetItem *)));
}
//------------------------------------------------------------
void tke_MainWindow_org::__pererahAllSaldoMonth(short month, short year, UPostupForm *postupForm)
{
	int prevMonth, prevYear;
	prevMonth = month-1;
	if (prevMonth==0){
		prevMonth = 12;
		prevYear = year-1;
	}
	else{
		prevYear = year;
	}
	double prevVhSaldo, prevNarahOpal, prevNarahGarVoda, prevOplata, curVhSaldo;
	QSqlQuery *query = new QSqlQuery;
	QSqlQuery *organizQuery = new QSqlQuery;
	QSqlTableModel * saldoUpdateModel = new QSqlTableModel();
	
	//налаштування форми поступу
	postupForm->showLable(true);
	postupForm->setPostupLabel(codec->toUnicode("Перерахунок вхідних сальдо організацій"));
	query->exec("SELECT count(*) FROM organizacii");
	query->seek(0);
	postupForm->setNumPos(query->value(0).toInt());
	
	organizQuery->exec("SELECT Ugoda FROM organizacii");
	while (organizQuery->next()){
		//Вхідне Сальдо попереднього місяця
		query->exec("SELECT max(Vh_Saldo) FROM saldo \
					WHERE year(CurDate)="+QVariant(prevYear).toString()+" \
					and month(CurDate)="+QVariant(prevMonth).toString()+" \
					and Ugoda_ID="+organizQuery->value(0).toString());
		query->seek(0);
		prevVhSaldo = query->value(0).toDouble();
		
		//Нарахування за попередній місяць (ЦО і гаряча вода)
		query->exec("SELECT sum(operacii.Narah_opal), sum(Narah_voda) \
						FROM operacii, \
							(SELECT id FROM pidrozdily \
									WHERE Ugoda="+organizQuery->value(0).toString()+") AS pidrozdilyOrg \
						WHERE operacii.year="+QVariant(prevYear).toString()+" \
								and operacii.month="+QVariant(prevMonth).toString()+" \
								and operacii.Pidrozdil=pidrozdilyOrg.id");
		query->seek(0);
		prevNarahOpal = query->value(0).toDouble();
		prevNarahGarVoda = query->value(1).toDouble();
		
		//Оплата в попередньому місяці
		query->exec("SELECT sum(Suma) FROM oplata \
						WHERE year(Pererah_date)="+QVariant(prevYear).toString()+" \
						and month(Pererah_date)="+QVariant(prevMonth).toString()+" \
						and Organiz_id="+organizQuery->value(0).toString());
		query->seek(0);
		prevOplata = query->value(0).toDouble();
		
		//Розрахунок вхідного сальдо
		curVhSaldo = prevVhSaldo + prevNarahOpal + prevNarahGarVoda - prevOplata;
		
		//Запис сальдо до бази даних
		saldoUpdateModel->setTable("saldo");
		saldoUpdateModel->setFilter("year(CurDate)="+QVariant(year).toString()+" \
										and month(CurDate)="+QVariant(month).toString()+" \
										and Ugoda_ID="+organizQuery->value(0).toString());
		saldoUpdateModel->select();
		if (saldoUpdateModel->rowCount() == 0){
			saldoUpdateModel->insertRows(0, 1);
			QSqlRecord record = saldoUpdateModel->record(0);
			record.setValue("id", nextLichylnyk("saldo"));
			record.setValue("Ugoda_id", organizQuery->value(0));
			record.setValue("CurDate", QDate(year, month, 1));
			record.setValue("Vh_saldo", curVhSaldo);
			saldoUpdateModel->setRecord(0, record);
			saldoUpdateModel->submitAll();
		}
		else{
			QSqlRecord record = saldoUpdateModel->record(0);
			record.setValue("Vh_saldo", curVhSaldo);
			saldoUpdateModel->setRecord(0, record);
			saldoUpdateModel->submitAll();
		}
		postupForm->inc_pos();
	}
	delete organizQuery;
	delete query;
	delete saldoUpdateModel;
}
//------------------------------------------------------------
bool tke_MainWindow_org::__pererahCurUgodaSaldo(int ugoda, short year, short month)
{
	QSqlQuery saldoQuery;
	QSqlQuery query;
	bool doneOk=true, curOper;
	saldoQuery.exec("SELECT id, CurDate, Vh_Saldo \
					FROM saldo \
					WHERE Ugoda_id="+QVariant(ugoda).toString()+" \
						and CurDate>=cdate('1."+QVariant(month).toString()+"."+QVariant(year).toString()+"') ORDER BY CurDate");
	saldoQuery.seek(0);
	double vhSaldo, narahAll, oplata, vyhSaldo;
	QDate curDate;
	vhSaldo = saldoQuery.value(2).toDouble();
	curDate = saldoQuery.value(1).toDate();
	query.exec("SELECT sum(Narah_opal+Narah_voda) \
				FROM pidrozdily \
				LEFT JOIN operacii \
					ON operacii.Pidrozdil=pidrozdily.id \
				WHERE operacii.Year="+QVariant(curDate.year()).toString()+" \
					and operacii.Month="+QVariant(curDate.month()).toString()+" \
					and pidrozdily.Ugoda="+QVariant(ugoda).toString());
	query.seek(0);
	narahAll = query.value(0).toDouble();
	query.exec("SELECT sum(Suma) \
				FROM oplata \
				WHERE year(Pererah_date)="+QVariant(curDate.year()).toString()+" \
					and month(Pererah_date)="+QVariant(curDate.month()).toString()+" \
					and Organiz_id="+QVariant(ugoda).toString());
	query.seek(0);
	oplata = query.value(0).toDouble();
	vyhSaldo = d_to_m(vhSaldo + narahAll - oplata);
	while (saldoQuery.next()){
		vhSaldo = vyhSaldo;
		curOper = query.exec("UPDATE saldo \
								SET Vh_saldo="+uMToStr2Krapka(vhSaldo)+" \
								WHERE id="+saldoQuery.value(0).toString());
		doneOk = doneOk && curOper;
		if (!curOper){
			qWarning() << codec->toUnicode("Не змінено вхідного сальдо організації - ") + QVariant(ugoda).toString() +
									codec->toUnicode(", за ") + curDate.toString("MM.yyyy") + "\n" +
									codec->toUnicode("помилка - ")+ query.lastError().text() +
									"\nvhidne Saldo - " + uMToStr2(vhSaldo) +codec->toUnicode("грн.");
		}
		curDate = saldoQuery.value(1).toDate();
		query.exec("SELECT sum(Narah_opal+Narah_voda) \
					FROM pidrozdily \
					LEFT JOIN operacii \
						ON operacii.Pidrozdil=pidrozdily.id \
					WHERE operacii.Year="+QVariant(curDate.year()).toString()+" \
						and operacii.Month="+QVariant(curDate.month()).toString()+" \
						and pidrozdily.Ugoda="+QVariant(ugoda).toString());
		query.seek(0);
		narahAll = query.value(0).toDouble();
		query.exec("SELECT sum(Suma) \
					FROM oplata \
					WHERE year(Pererah_date)="+QVariant(curDate.year()).toString()+" \
						and month(Pererah_date)="+QVariant(curDate.month()).toString()+" \
						and Organiz_id="+QVariant(ugoda).toString());
		query.seek(0);
		oplata = query.value(0).toDouble();
		vyhSaldo = d_to_m(vhSaldo + narahAll - oplata);
	}
	return doneOk;
}
//------------------------------------------------------------
bool tke_MainWindow_org::__pererahAllUgodaSaldo(short year, short month)
{
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT count(*) FROM organizacii");
	query->seek(0);
	//налаштування форми поступу
	UPostupForm *postupForm = new UPostupForm(0, query->value(0).toInt());
	postupForm->show();
	bool doneOk=true;
	query->exec("SELECT Ugoda FROM organizacii ORDER BY Ugoda");
	while (query->next()){
		doneOk = doneOk && __pererahCurUgodaSaldo(query->value(0).toInt(), year, month);
		postupForm->inc_pos();
	}
	
	delete postupForm;
	delete query;
	return doneOk;
}
//------------------------------------------------------------
void tke_MainWindow_org::openSqlBrowser()
{
	browser = new Browser(0,0);
	browser->setRoot(IRC);
	browser->show();
	
	connect(this, SIGNAL(mainWindowClosed()), browser, SLOT(close()));
	connect(this, SIGNAL(closeLoginSignal()), browser, SLOT(close()));
}
//------------------------------------------------------------
void tke_MainWindow_org::loadGUISettingsFromIni()
{
	
}
//------------------------------------------------------------
void tke_MainWindow_org::look_ugoda_info( const QModelIndex & index, const QModelIndex & index_ignored )
{
	const QAbstractItemModel *sqlTableModel = index.model();
	if (sqlTableModel){
		int abonPos, curRah = sqlTableModel->data(sqlTableModel->index(index.row(), 1), Qt::DisplayRole).toInt();
		abonPos = abonentsList.indexOf(curRah)+1;
		if (abonPos!=0)
			ui.spinBox_navigator->setValue(abonPos);
		else{
			QMessageBox::critical(
				this,
				codec->toUnicode("!!! Невірна угода !!!"),
				codec->toUnicode("В базі даних немає організації з номером ")+QVariant(curRah).toString()+".",
				QMessageBox::Ok,
				QMessageBox::NoButton,
				QMessageBox::NoButton);
		}
	}
}
//------------------------------------------------------------
void tke_MainWindow_org::activ_column_changes( const QModelIndex & current, const QModelIndex & previous )
{
	if (previous.column() == 1){
		look_ugoda_info(current, current);
	}
}
//------------------------------------------------------------
void tke_MainWindow_org::changeCellContents(int row, int col, double val)
{
	ui.tableWidget_slujb_rah_operacii->item(row,col)->setText(uMToStr2(val));
}
//------------------------------------------------------------
void tke_MainWindow_org::showOperaciiSpecRahMorInfo()
{
	if (ui.checkBox_mor_info_spec_rah->isChecked()){
		ui.tableWidget_slujb_rah_operacii->setColumnHidden ( 2, false);
		ui.tableWidget_slujb_rah_operacii->setColumnHidden ( 4, false);
	}
	else{
		ui.tableWidget_slujb_rah_operacii->setColumnHidden ( 2, true);
		ui.tableWidget_slujb_rah_operacii->setColumnHidden ( 4, true);
	}
}
//------------------------------------------------------------
void tke_MainWindow_org::curDateChanged()
{
	currentSysDate = dateEdit_PotochnaData->date();
	ui.spinBox_year_filtr->setValue(currentSysDate.year());
	ui.spinBox_slujb_rah_year_filtr->setValue(currentSysDate.year());
	ui.dateEdit_narah_pidrozdily->setDate(currentSysDate);
	//Поновлення фільтрів дати
	ui.spinBox_month_pokaz_lich->setValue(currentSysDate.month());
	ui.spinBox_year_pokaz_lich->setValue(currentSysDate.year());
	ui.spinBox_month_meteor_table->setValue(currentSysDate.month());
	ui.spinBox_year_meteor_table->setValue(currentSysDate.year());
	ui.spinBox_month_dni_opal->setValue(currentSysDate.month());
	ui.spinBox_year_dni_opal->setValue(currentSysDate.year());
	ui.spinBox_month_oplata->setValue(currentSysDate.month());
	ui.spinBox_year_oplata->setValue(currentSysDate.year());
	ui.spinBox_month_pokaz_lich_gar_voda->setValue(currentSysDate.month());
	ui.spinBox_year_pokaz_lich_gar_voda->setValue(currentSysDate.year());
	
	if (date.month()==12)
		ui.spinBox_year_pokaz_lich->setMaximum( currentSysDate.year()+1 );
	else
		ui.spinBox_year_pokaz_lich->setMaximum( currentSysDate.year() );
	if (date.month()==12)
		ui.spinBox_year_pokaz_lich_gar_voda->setMaximum( currentSysDate.year()+1 );
	else
		ui.spinBox_year_pokaz_lich_gar_voda->setMaximum( currentSysDate.year() );
	if (date.month()==12)
		ui.spinBox_year_meteor_table->setMaximum( currentSysDate.year()+1 );
	else
		ui.spinBox_year_meteor_table->setMaximum( currentSysDate.year() );
}
//------------------------------------------------------------
void tke_MainWindow_org::setSettingsFromIni()
{
	//Налаштування GUI з іні-файлу
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	dateEdit_PotochnaData->setDate(settings->value("GUI_View/SystemCurDate").toDate());
	curDateChanged();
	delete settings;
}
//------------------------------------------------------------
void tke_MainWindow_org::saveSettingsToIni()
{
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	//Збереження вигляду GUI в іні-файл
	QSettings *settings = new QSettings("./tke.ini", QSettings::IniFormat);
	settings->beginGroup("GUI_View");
	settings->setValue("SystemCurDate",currentSysDate);
	delete settings;
}
//------------------------------------------------------------
void tke_MainWindow_org::populateControlObjectsHash()
{
	controlObjectsHash->clear();
	controlObjectsHash->insert(10010, dateEdit_PotochnaData);
	controlObjectsHash->insert(10020, ui.action_debuger);
	controlObjectsHash->insert(10030, ui.action_sqlBrowser);
	controlObjectsHash->insert(10040, ui.action_taryf_table);
	controlObjectsHash->insert(10050, ui.action_dni_opalennya);
	controlObjectsHash->insert(10060, ui.action_pokaz_lich_table);
	controlObjectsHash->insert(10070, ui.action_pokaz_lich_gar_voda);
	controlObjectsHash->insert(10080, ui.action_meteor_table);
	controlObjectsHash->insert(10090, ui.action_oplata);
	controlObjectsHash->insert(10100, ui.action_calc_tepl_navantaj);
	controlObjectsHash->insert(10110, ui.action_narah_opal);
	controlObjectsHash->insert(10120, ui.action_narah_gar_voda);
	controlObjectsHash->insert(10130, ui.action_calc_saldo_spec_rahunkiv);
	controlObjectsHash->insert(10140, ui.action_pererahunok_saldo_usih_spec_rahunkiv);
	controlObjectsHash->insert(10150, ui.action_pererah_vhidnogo_saldo_potochnoi_ugody);
	controlObjectsHash->insert(10160, ui.action_pererah_vhidnyh_saldo_usih_ugod);
	controlObjectsHash->insert(10170, ui.action_print_vidomist_oplaty_za_CO);
	controlObjectsHash->insert(10180, ui.action_print_narah_gar_voda);
	controlObjectsHash->insert(10190, ui.action_print_vidomist_narahuvannya_i_postuplennya_dohodiv);
	controlObjectsHash->insert(10200, ui.action_print_nakopychuvalna_vvidomist_narahuvannya);
	controlObjectsHash->insert(10210, ui.action_print_vidomist_realizacii);
	controlObjectsHash->insert(10220, ui.action_print_rahunok);
	controlObjectsHash->insert(10230, ui.action_print_vidomist_borjnykiv_organiz);
	controlObjectsHash->insert(10240, ui.tabWidget->widget(1));
	controlObjectsHash->insert(10250, ui.tabWidget->widget(2));
	//UgodyForm
	controlObjectsHash->insert(10260, ui.lineEdit_Nazva);
	controlObjectsHash->insert(10270, ui.lineEdit_Ur_adresa);
	controlObjectsHash->insert(10280, ui.spinBox_Ugoda);
	controlObjectsHash->insert(10290, ui.lineEdit_Edrpou);
	controlObjectsHash->insert(10300, ui.comboBox_Bank);
	controlObjectsHash->insert(10310, ui.lineEdit_Rozr_Rah);
	controlObjectsHash->insert(10320, ui.lineEdit_Vidpovidaln);
	controlObjectsHash->insert(10330, ui.lineEdit_Tel1);
	controlObjectsHash->insert(10340, ui.lineEdit_Tel2);
	controlObjectsHash->insert(10350, ui.comboBox_Finans);
	controlObjectsHash->insert(10360, ui.comboBox_Pryznach);
	controlObjectsHash->insert(10370, ui.dateEdit_date_ugody);
	controlObjectsHash->insert(10380, ui.pushButton_new);
	controlObjectsHash->insert(10390, ui.pushButton_organiz_form_save);
	controlObjectsHash->insert(10400, ui.pushButton_organiz_form_abort);
	controlObjectsHash->insert(10410, ui.tableView_pidrozdily);
	controlObjectsHash->insert(10420, ui.spinBox_year_filtr);
	//SpecrahunkyForm
	controlObjectsHash->insert(10430, ui.spinBox_Rahunok);
	controlObjectsHash->insert(10440, ui.lineEdit_RahunokName);
	controlObjectsHash->insert(10450, ui.comboBox_Rahunok_type);
	controlObjectsHash->insert(10460, ui.checkBox_mor_info_spec_rah);
	controlObjectsHash->insert(10470, ui.pushButton_organiz_form_slujb_rah_save);
	controlObjectsHash->insert(10480, ui.pushButton_organiz_form_slujb_rah_abort);
	controlObjectsHash->insert(10490, ui.pushButton_slujb_rah_new);
	controlObjectsHash->insert(10500, ui.spinBox_slujb_rah_year_filtr);
	//Нарахування по підрозділах
	controlObjectsHash->insert(10510, ui.dateEdit_narah_pidrozdily);
	controlObjectsHash->insert(10520, ui.tableWidget_dni_opal_narah_pidrozdily);
	controlObjectsHash->insert(10530, ui.doubleSpinBox_osnovna_kilkist_tepla);
	controlObjectsHash->insert(10540, ui.doubleSpinBox_dodatkova_kilkist_tepla);
	controlObjectsHash->insert(10550, ui.doubleSpinBox_taryf_za_CO_narah_pidrozdily);
	controlObjectsHash->insert(10560, ui.doubleSpinBox_pochatk_pokazn_lich_narah_pidrozdily);
	controlObjectsHash->insert(10570, ui.doubleSpinBox_kinc_pochatk_pokazn_lich_narah_pidrozdily);
	controlObjectsHash->insert(10580, ui.doubleSpinBox_kilkist_bilyzny_narah_pidrozdily);
	controlObjectsHash->insert(10590, ui.doubleSpinBox_taryf_za_gar_vodu_narah_pidrozdily);
	controlObjectsHash->insert(10600, ui.doubleSpinBox_zag_kilkist_tepla_CO_narah_pidrozdily);
	controlObjectsHash->insert(10610, ui.doubleSpinBox_zag_narah_za_CO_narah_pidrozdily);
	controlObjectsHash->insert(10620, ui.doubleSpinBox_zag_kilkist_tepla_GV_narah_pidrozdily);
	controlObjectsHash->insert(10630, ui.doubleSpinBox_zag_narah_GV_narah_pidrozdily);
	controlObjectsHash->insert(10640, ui.checkBox_avtomatychne_narahuvannya_pidrozdily);
	controlObjectsHash->insert(10650, ui.pushButton_reload_narah_pidrozdily);
	controlObjectsHash->insert(10660, ui.pushButton_do_narah);
	controlObjectsHash->insert(10670, ui.pushButton_print_rahunok);
	controlObjectsHash->insert(10680, ui.pushButton_save_narah_pidrozdily);
	controlObjectsHash->insert(10690, calculatorButton);
	controlObjectsHash->insert(10700, ui.action_saveDatabase);
	//Загальні
	
}
//------------------------------------------------------------
void tke_MainWindow_org::calculatorButton_clicked()
{
	QProcess::startDetached("calc");
}
//------------------------------------------------------------
void tke_MainWindow_org::saveMode()
{
	organizButtonCancelClicked();
	organizButtonSlujbRahCancelClicked();
	
	//Закриття кнопок і вікон
	ui.dockWidget_taryf->setVisible(false);
	ui.dockWidget_pokaz_lich_gar_voda_table->setVisible(false);
	ui.dockWidget_pokaz_lich_table->setVisible(false);
	ui.dockWidget_meteor_table->setVisible(false);
	ui.dockWidget_dni_opal->setVisible(false);
	ui.dockWidget_oplata->setVisible(false);
	
	ui.action_taryf_table->setChecked(false);
	ui.action_dni_opalennya->setChecked(false);
	ui.action_pokaz_lich_table->setChecked(false);
	ui.action_pokaz_lich_gar_voda->setChecked(false);
	ui.action_meteor_table->setChecked(false);
	ui.action_taryf_table->setChecked(false);
	ui.action_oplata->setChecked(false);
	
	//Переведення в неактивний стан усіх віджетів програми
	QHashIterator<int, QObject *> iter(*controlObjectsHash);
	while (iter.hasNext())
		castControls(iter.next().value(), false);
	
	IRC = false;
	emit closeLoginSignal();
}
//------------------------------------------------------------
void tke_MainWindow_org::workMode(int mode)
{
	saveMode();
	QSqlQuery *query = new QSqlQuery();
	query->exec("SELECT id FROM users_priv WHERE id>1000 and ["+QString::number(mode)+"]=true");
	while (query->next())
		castControls(controlObjectsHash->value(query->value(0).toInt()), true);
	
	query->exec("SELECT id FROM users_priv WHERE id=10031 and ["+QString::number(mode)+"]=true");
	if (query->seek(0))
		IRC = true;
	
	delete query;
}
//------------------------------------------------------------
tke_MainWindow_org::~tke_MainWindow_org()
{
	saveSettingsToIni();
	
	qInstallMsgHandler(0);
	
	tke_Dialog_saveDataBase *saveDialog = new tke_Dialog_saveDataBase();
	connect(this, SIGNAL(requestSaveDataBaseWindow()), saveDialog, SLOT(show()));
	
	
	emit mainWindowClosed();
	emit requestSaveDataBaseWindow();
	/*delete orgSelectQuery;
	delete defValuesThread;
	delete specRahSelectQuery;
	delete insertSqlModel;
	delete modelPidrozdily;
	delete modelInsertPidrozdily;
	delete modelOplata;
	delete model_taryf;
	delete dniOpalQueryModel;
	delete palette;
	delete default_palette;*/
}
//------------------------------------------------------------
