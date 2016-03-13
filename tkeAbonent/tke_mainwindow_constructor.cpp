//------------------------------------------------------------
//
// Implamantation of tke_MainWindow class.
// Created by Roman Mulyk 08.08.2006
//
//------------------------------------------------------------

#include <UStandardDelegate>
#include "tke_mainwindow.h"
#include "ubudynky_delegate.h"
#include "unormat_taryf_delegate.h"
#include "udiuch_taryf_delegate.h"
#include "usubs_delegate.h"
#include "ukvyt_delegate.h"
#include "uspravy_subs_delegate.h"
#include "ukorektura_delegate.h"
#include "usplata_zaborg_delegate.h"
#include "upilgovyky_delegate.h"
#include "uoperacii_delegate.h"
#include "ukorektura_s_delegate.h"
#include "usettings.h"
#include "tke_dialog_filtr_borjnykiv.h"
#include "../tkeSqlBrowser/src/browser.h"

//------------------------------------------------------------
tke_MainWindow::tke_MainWindow(QWidget *parent)
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
	calculatorButton = new QPushButton(QIcon(QString::fromUtf8(":/pictures/pictures/calculator.png")), "Калькулятор",this);
	calculatorButton->setIconSize(QSize(12,12));
	statusBar()->addPermanentWidget(calculatorButton);
	ui.comboBox_Poshuk_rahunok->lineEdit()->setAlignment(Qt::AlignRight);
	ui.comboBox_Poshuk->lineEdit()->setAlignment(Qt::AlignRight);
	ui.tabWidget_monthlyOperacii->setCurrentIndex(0);
	
	loadGUISettingsFromIni();
	
    //assistant = NULL;
	
	setSettingsFromIni();
	curDateChanged();

	setAttribute(Qt::WA_DeleteOnClose);  //Знищення об'єкта при закритті форми
	// initialize object wariables
	ui.line->setFrameShadow(QFrame::Plain);
	ui.line_2->setFrameShadow(QFrame::Plain);
	ui.line_3->setFrameShadow(QFrame::Plain);
	ui.line_4->setFrameShadow(QFrame::Plain);
	ui.line_5->setFrameShadow(QFrame::Plain);
	
	palette = new QPalette();   palette->setColor ( QPalette::Active, QPalette::WindowText, Qt::red );
	default_palette = new QPalette();  *default_palette = ui.line->palette();
	pilgy_hash = new QHash<QString, int>();
	controlObjectsHash = new QHash<int, QObject *>;
	codec = QTextCodec::codecForName("CP1251");
	date = QDate::currentDate();
	record_edited = 0;
	new_record = 0;
	//ініціалізація площ для пільговиків
	d.pds = 10.5;	// площа на сім'ю
	d.pd1 = 21;		// площа на одного пільговика
	
	dateEdit_PotochnaData->setMaximumDate(date.addMonths(2));
	
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
	
	//default values variables initializing
	d_Vulycia = "";							// Вулиця
	d_Koef = 1.163;							// Коефіцієнт переведення
	d_Kotelnya_ID = 1;						// Ідентифікатор котельні
	d_City = codec->toUnicode("Самбір");	// Назва населеного пункту
	d_Sprava = 0;							// Справа
	d_Ugoda = 0;							// Угода
	d_Kategoriya_pilgy_ID = 0;				// Ідентифікатор пільги
	d_Pachka = 0;							// № пачки
	
	//initialize month names in ukrainian language
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
	
	//  set all dock windows close;
		ui.dockWidget_budynky->setVisible( false );
		ui.dockWidget_pilgy->setVisible( false );
		ui.dockWidget_normat_taryf->setVisible( false );
		ui.dockWidget_diuch_taryf->setVisible( false );
		ui.dockWidget_korektura->setVisible( false );
		ui.dockWidget_korektura_s->setVisible( false );
		ui.dockWidget_subs->setVisible( false );
		ui.dockWidget_kvyt->setVisible( false );
		ui.dockWidget_spravy_subs->setVisible( false );
		ui.dockWidget_splata_zaborg->setVisible( false );
		ui.dockWidget_kotelni->setVisible( false );
		ui.dockWidget_pilgovyky->setVisible( false );
		ui.dockWidget_opSubs->setVisible( false );
		ui.dockWidget_narahPereved->setVisible( false );
		
		//Налаштування елементів управління
		QLineEdit *lEdit = ui.comboBox_Poshuk_rahunok->lineEdit();
		lEdit->setInputMask( "000000" );
		
	// set operacci table
		for (int col=0; col<ui.tableWidget_rozrahunky->columnCount(); col++){
			for (int row=0; row<ui.tableWidget_rozrahunky->rowCount(); row++){
				QTableWidgetItem *tableWidgetItem = new QTableWidgetItem();
				tableWidgetItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
				ui.tableWidget_rozrahunky->setItem(row, col, tableWidgetItem );
			}
			ui.tableWidget_rozrahunky->setColumnWidth(col,60);
		}
		
		initBeznadiynaDebitorskaZaborgTable();
	
              //set table budynky
			    model_budynky = new QSqlRelationalTableModel(this);
			    ui.tableView_budynky->setAlternatingRowColors(true);
				ui.tableView_budynky->setColumnHidden(5, true);
			  // end of set table budynky
			  //-------------------------------
			  //set table pilgy
			    model_pilgy = new QSqlRelationalTableModel(this);
				ui.tableView_pilgy->setAlternatingRowColors(true);
			  // end of set tablepilgy	
			  //-------------------------------
			  //set table normat_taryf
			    model_normat_taryf = new QSqlRelationalTableModel(this);
				ui.tableView_normat_taryf->setAlternatingRowColors(true);
			  // end of set table normat_taryf
			  //-------------------------------
			  //set table korektura (Коректура по площі)
				ui.spinBox_month_korektura->setValue(currentSysDate.month());
				ui.spinBox_month_korektura->setMaximum(date.month());
				ui.spinBox_year_korektura->setValue(currentSysDate.year());
				ui.spinBox_year_korektura->setMaximum(date.year());
			    model_korektura = new QSqlRelationalTableModel(this);
				ui.tableView_korektura->setAlternatingRowColors(true);
			  // end of set table korectyra
			  
			  //set table korektura_s (Коректура по cумі)
				ui.spinBox_month_korektura_s->setValue(currentSysDate.month());
				ui.spinBox_month_korektura_s->setMaximum(date.month());
				ui.spinBox_year_korektura_s->setValue(currentSysDate.year());
				ui.spinBox_year_korektura_s->setMaximum(date.year());
			    model_korektura_s = new QSqlRelationalTableModel(this);
				ui.tableView_korektura_s->setAlternatingRowColors(true);
			  // end of set table korectyra
			  
			  //set table subs
				ui.spinBox_month_subs->setValue(currentSysDate.month());
				ui.spinBox_month_subs->setMaximum(date.month());
				ui.spinBox_year_subs->setValue(currentSysDate.year());
				ui.spinBox_year_subs->setMaximum(date.year());
				model_subs = new QSqlRelationalTableModel(this);
				ui.tableView_subs->setAlternatingRowColors(true);
			  // end of set table subs
			  
			  //set table kvyt
				ui.spinBox_kvyt_opl_month->setValue(currentSysDate.month());
				ui.spinBox_kvyt_opl_month->setMaximum(date.month());
				ui.spinBox_kvyt_opl_year->setValue(currentSysDate.year());
				ui.spinBox_kvyt_opl_year->setMaximum(date.year());
				ui.spinBox_kvyt_pereved_month->setValue(currentSysDate.month());
				ui.spinBox_kvyt_pereved_month->setMaximum(date.month());
				ui.spinBox_kvyt_pereved_year->setValue(currentSysDate.year());
				ui.spinBox_kvyt_pereved_year->setMaximum(date.year());
				model_kvytancii = new QSqlRelationalTableModel(this);
				ui.tableView_kvyt->setAlternatingRowColors(true);
			  // end of set table kvyt
			  
			  //set table spravy_subs
				model_spravy_subs = new QSqlRelationalTableModel(this);
				ui.tableView_spravy_subs->setAlternatingRowColors(true);
			  // end of set table spravy_subs
			  
			  //set table splata zaborgovanosti
				model_splata_zaborg = new QSqlRelationalTableModel(this);
				ui.tableView_splata_zaborg->setAlternatingRowColors(true);
			  // end of set table splata zaborgovanosti
			  
			  //set table kotelni
			    model_kotelni = new QSqlRelationalTableModel(this);
				ui.tableView_kotelni->setAlternatingRowColors(true);
			  // end of set table kotelni
			  
			  //set table pilgovyky
				populate_pilgovyky_filtr_comboboxes();
				model_pilgovyky = new QSqlRelationalTableModel(this);
				ui.tableView_pilgovyky->setAlternatingRowColors(true);
			  // end of set table pilgovyky
			  
			  //initialize pointer variables
				abonent_select_query = new QSqlQuery();
				abonent_select_query->prepare("SELECT id, Rahunok, Prizv, Imya, Batk, Dilnycia, Budynok_ID, Kvartyra, \
													Ploshcha, Zaiva_ploshcha, Pilga_ID, Pilga_for, tel, identyf, kimn, \
													ludy, opal, Pilga_ID_2, Pilga_for_2, Pilgy_zdani, GarVoda, \
													BankRahunok, UsePilgyDateLimits, PochPilgaDate, KincPilgaDate, PochPilga_2Date, \
													KincPilga_2Date, NarahStoyak, SufiksKvart, Prymitka, SamovVidednCO \
													FROM abonent \
													ORDER BY Prizv, Imya, Batk");
				abonent_temp_query = new QSqlQuery();
				abonent_update_model = new QSqlTableModel();
				abonent_insert_model = new QSqlTableModel();
				diuch_taryf_query = new QSqlQuery();
				diuch_taryf_update_model = new QSqlTableModel();
				narah_update_model = new QSqlTableModel();
				korektura_update_model = new QSqlTableModel();
				narah_select_query = new QSqlQuery();
				narah_temp_query = new QSqlQuery();
			  //prepare first view record
				populate_comboboxes_abonent();
				exec_abonent_select_query();
				ui.spinBox_navigator->setValue( 1 ); // 1 - bo pershyi zapys
				enable_navigation_buttons(1); // 1 - bo pershyi zapys
				populate_abonent_form(1); // 1 - bo pershyi zapys
				
			  //set table diuch_taryf + dock widget diuch taryf
				if (date.month()==12)
					ui.spinBox_year_diuch_taryf->setMaximum( date.year()+1 );
				else
					ui.spinBox_year_diuch_taryf->setMaximum( date.year() );
				ui.spinBox_month_diuch_taryf->setValue( currentSysDate.month() );
				ui.spinBox_year_diuch_taryf->setValue( currentSysDate.year() );
				//ui.tableWidget_diuch_taryf->setItemDelegate (new UDiuch_taryf_Delegate(ui.tableWidget_diuch_taryf) );
				
				ui.tableWidget_diuch_taryf->setItemDelegateForColumn(2, new USpinBoxDelegate(0,31,ui.tableWidget_diuch_taryf));
                ui.tableWidget_diuch_taryf->setItemDelegateForColumn(3, new UDoubleSpinBoxDelegate(0,999999,3,ui.tableWidget_diuch_taryf));
                ui.tableWidget_diuch_taryf->setItemDelegateForColumn(4, new UDoubleSpinBoxDelegate(0,999999,3,ui.tableWidget_diuch_taryf));
                ui.tableWidget_diuch_taryf->setItemDelegateForColumn(5, new UDoubleSpinBoxDelegate(0,999999,2,ui.tableWidget_diuch_taryf));
                ui.tableWidget_diuch_taryf->setItemDelegateForColumn(7, new UDoubleSpinBoxDelegate(0,999999,3,ui.tableWidget_diuch_taryf));
				
				ui.tableWidget_diuch_taryf->resizeColumnsToContents();
				if (ui.tableWidget_diuch_taryf->columnWidth(5) < 60)
					ui.tableWidget_diuch_taryf->setColumnWidth(5, 60);
				if (ui.tableWidget_diuch_taryf->columnWidth(7) < 60)
					ui.tableWidget_diuch_taryf->setColumnWidth(7, 60);
				ui.tableWidget_diuch_taryf->hideColumn(0);
				ui.tableWidget_diuch_taryf->hideColumn(1);
				ui.tableWidget_diuch_taryf->hideColumn(6);
				
					//Контекстне меню
				ui.tableWidget_diuch_taryf->addAction(ui.action_diuch_taryf_select_all);
				ui.tableWidget_diuch_taryf->addAction(ui.action_diuch_taryf_calc_taryf);
				ui.tableWidget_diuch_taryf->addAction(ui.action_diuch_taryf_save_taryf);
				ui.tableWidget_diuch_taryf->addAction(ui.action_diuch_taryf_obedn_budynky);
				ui.tableWidget_diuch_taryf->setContextMenuPolicy(Qt::ActionsContextMenu);

			  // end of set table diuch_taryf	
				
			  //set table rozrahunky
			    //ui.tableWidget_rozrahunky->resizeColumnsToContents();
				ui.tableWidget_rozrahunky->resizeRowsToContents();
				ui.tableWidget_rozrahunky->verticalHeader()->resize(10, ui.tableWidget_rozrahunky->verticalHeader()->size().height());
				ui.tableWidget_rozrahunky->verticalHeader()->setDefaultAlignment ( Qt::AlignRight );
		
		// --- CONNECTIONS ---
		connect(ui.action_close_program, SIGNAL(activated()), qApp, SLOT(closeAllWindows())); //zakryvannya programy
		connect(ui.action_extSearch, SIGNAL(activated()), this, SLOT(action_extSearch_activated()));
		connect(ui.action_structPostachGV, SIGNAL(activated()), this, SLOT(action_structPostachGV_activated()));
		connect(ui.action_calcObsyagTeplEnerg, SIGNAL(activated()), this, SLOT(action_calcObsyagTeplEnerg_activated()));
		
		connect(ui.action_budynky, SIGNAL(toggled(bool)), this, SLOT(action_budynky_activated())); //vvimk-vymk ui.dockWidget_budynky
		connect(ui.action_pilgy, SIGNAL(toggled(bool)), this, SLOT(action_pilgy_activated())); // vviml-vymkn ui.dockWidget_pilgy
		connect(ui.action_normat_taryf, SIGNAL(toggled(bool)), this, SLOT(action_normat_taryf_activated())); // vviml-vymkn ui.dockWidget_normat_taryf
		connect(ui.action_diuchyi_taryf, SIGNAL(toggled(bool)), this, SLOT(action_diuchyi_taryf_activated())); // vviml-vymkn ui.dockWidget_normat_taryf
		connect(ui.action_pokaz_lich_GV, SIGNAL(activated()), this, SLOT(action_pokaz_lich_GV_activated()));
		connect(ui.action_korektura, SIGNAL(toggled(bool)), this, SLOT(action_korektura_activated())); // vviml-vymkn ui.dockWidget_korectura
		connect(ui.action_korektura_s, SIGNAL(toggled(bool)), this, SLOT(action_korektura_s_activated())); // vviml-vymkn ui.dockWidget_korectura_s
		connect(ui.action_narah, SIGNAL(activated()), this, SLOT(action_narah_activated())); // zapusk dialogu provedennya narahuvannya
		connect(ui.action_narah_GV, SIGNAL(activated()), this, SLOT(action_narah_GV_activated()));
		connect(ui.action_vidomist_pilg, SIGNAL(activated()), this, SLOT(action_print_vidomist_pilg())); // zapusk dialogu druku zved vidomosti pilg
		connect(ui.action_vidomist_pilgovykiv_kategor, SIGNAL(activated()), this, SLOT(action_print_vidomist_pilg_kategor())); // zapusk dialogu druku vidomosti pilg po kategor
		connect(ui.action_zvit_pilg_kategor, SIGNAL(activated()), this, SLOT(action_print_zvit_pilg_kategor())); // 
		connect(ui.action_subs, SIGNAL(toggled(bool)), this, SLOT(action_subs_activated())); // vvimk-vymk ui.dockWidget_subs
		connect(ui.action_kvytancii, SIGNAL(toggled(bool)), this, SLOT(action_kvytancii_activated())); // vvimk-vymk ui.dockWidget_kvytancii
		connect(ui.action_spravy_subs, SIGNAL(toggled(bool)), this, SLOT(action_spravy_subs_activated())); // vvimk-vymk ui.dockWidget_spravy_subs
        connect(ui.action_zaborg, SIGNAL(toggled(bool)), this, SLOT(action_zaborg_activated())); // vvimk-vymk ui.dockWidget_splata_zaborg
		connect(ui.action_kotelni, SIGNAL(toggled(bool)), this, SLOT(action_kotelni_activated())); // vvimk-vymk ui.dockWidget_kotelni
		connect(ui.action_pilgovyky, SIGNAL(toggled(bool)), this, SLOT(action_pilgovyky_activated())); // vvimk-vymk ui.dockWidget_kotelni
		connect(ui.action_opSubs, SIGNAL(toggled(bool)), this, SLOT(action_opSubs_activated()));
		connect(ui.action_narahPereved, SIGNAL(toggled(bool)), this, SLOT(action_narahPereved_activated()));
        connect(ui.action_fill_zapyt_subs, SIGNAL(activated()), this, SLOT(action_fill_zapyt_subs_activated()));

		connect(ui.action_print_vidomist_narah, SIGNAL(activated()), this, SLOT(action_print_vidomist_narah_activated())); // друк відомості нарахування за центральне опалення
		connect(ui.action_print_dovidka_oderj_subs, SIGNAL(activated()), this, SLOT(action_print_dovidka_oderj_subs_activated())); // друк довідки про суму одержаних субсидій
		connect(ui.action_print_kvytancii, SIGNAL(activated()), this, SLOT(action_print_kvytancii_activated())); // друк квитанцій
		connect(ui.action_printCustomKvyt, SIGNAL(activated()), this, SLOT(action_printCustomKvyt_activated()));
		
		connect(ui.action_print_kvytancii_for_abonent, SIGNAL(activated()), this, SLOT(action_print_kvytancii_for_abonent_activated())); // друк квитанції для абонента
		connect(ui.action_registracion, SIGNAL(activated()), this, SLOT(action_registracion_activated())); // реєстрація користувача
		connect(ui.action_close_login, SIGNAL(activated()), this, SLOT(action_close_login_activated())); // вихід користувача з сеансу
		connect(ui.action_pererahunok_saldo, SIGNAL(activated()), this, SLOT(action_pererahunok_saldo_activated())); // перерахунок вхідних сальдо для поточного абонента
		connect(ui.action_pererahunok_saldo_all, SIGNAL(activated()), this, SLOT(action_pererahunok_saldo_all_activated())); // перерахунок вхідних сальдо для усіх абонентів
		connect(ui.action_insert_dbf_kvyt_oshchadbank, SIGNAL(activated()), this, SLOT(action_insert_dbf_kvyt_oshchadbank_activated())); // втягування квитанцій з dbf-ок Ощадбанку 
		connect(ui.action_set_data_sources, SIGNAL(activated()), this, SLOT(action_set_data_sources_activated())); // налаштування джерел даних програми 
		connect(ui.action_print_borjnyky, SIGNAL(activated()), this, SLOT(action_print_borjnyky_activated())); // налаштування джерел даних програми 
        connect(ui.action_print_pereplata, SIGNAL(activated()), this, SLOT(action_print_pereplata_activated())); // налаштування джерел даних програми
        connect(ui.action_debuger_form, SIGNAL(activated()), this, SLOT(action_debuger_form_activated())); // налаштування джерел даних програми
		connect(ui.action_sqlBrowser,SIGNAL(activated()),this,SLOT(action_sqlBrowser_activated()));
		connect(ui.actionPosada,SIGNAL(activated()),this,SLOT(action_posada_activated()));
		connect(ui.action_assistent,SIGNAL(activated()),this,SLOT(action_assistent_activated()));
		connect(ui.action_about,SIGNAL(activated()),this,SLOT(action_about_activated()));
		connect(ui.action_edit_users,SIGNAL(activated()),this,SLOT(action_edit_users_activated()));
		connect(ui.action_print_povidomlennya_dlya_borjnykiv,SIGNAL(activated()),this,SLOT(action_print_povidomlennya_dlya_borjnykiv_activated()));
		connect(ui.action_print_povidomlennya_dlya_pilgovykiv,SIGNAL(activated()),this,SLOT(action_print_povidomlennya_dlya_pilgovykiv_activated()));
		connect(ui.action_clear_vidomosti_pro_zdani_dokumenty_napilgy,SIGNAL(activated()),this,SLOT(action_clear_vidomosti_pro_zdani_dokumenty_napilgy_activated()));
		
		connect(ui.action_print_rahunok_abonenta, SIGNAL(activated()), this, SLOT(action_print_rahunok_abonenta_activated())); // друк відомості користувача
		connect(ui.action_print_forma_akt_vytrat_teploenergii, SIGNAL(activated()), this, SLOT(action_print_forma_akt_vytrat_teploenergii_activated())); // друк форми Акту витрат теплоенергії
		connect(ui.action_save_vidomist_pilg_to_file, SIGNAL(activated()), this, SLOT(action_save_vidomist_pilg_to_file_activated()));
		
		connect(ui.action_archiv_vidomosti_kvytanciy_ob, SIGNAL(activated()), this, SLOT(action_archiv_vidomosti_kvytanciy_ob_activated()));
		connect(ui.action_insert_subs_dbf, SIGNAL(activated()), this, SLOT(action_insert_subs_dbf_activated()));
		connect(ui.action_print_vidomist_borjnykiv_riznyc, SIGNAL(activated()), this, SLOT(action_print_vidomist_borjnykiv_riznyc_activated()));
		connect(ui.action_print_wrong_subs, SIGNAL(activated()), this, SLOT(action_print_wrong_subs()));
		connect(ui.action_vidomist_narah_oplaty_za_CO_dla_viddilu_subs, SIGNAL(activated()), this, SLOT(action_vidomist_narah_oplaty_za_CO_dla_viddilu_subs_activated()));
		connect(ui.action_print_dovidnyk_abonentiv, SIGNAL(activated()), this, SLOT(action_print_dovidnyk_abonentiv_activated()));
		connect(ui.action_dovidnykAbonentivVRozriziNadannyaPoslug, SIGNAL(activated()), this, SLOT(action_dovidnykAbonentivVRozriziNadannyaPoslug_activated()));
		connect(ui.action_print_vidomist_zdanyh_dokumentiv_na_pilgy,SIGNAL(activated()),this,SLOT(action_print_vidomist_zdanyh_dokumentiv_na_pilgy_activated()));
		
		connect(ui.action_open_insert_narah_subs_form, SIGNAL(activated()), this, SLOT(action_open_insert_narah_subs_form_activated()));
		connect(ui.action_open_insert_donarah_subs_form, SIGNAL(activated()), this, SLOT(action_open_insert_donarah_subs_form_activated()));

		connect(ui.action_print_dovidka_u_subs, SIGNAL(activated()), this, SLOT(action_print_dovidka_u_subs_activated()));
		connect(ui.action_print_dovidka_u_subs_2, SIGNAL(activated()), this, SLOT(action_print_dovidka_u_subs_2_activated()));
		
		connect(ui.action_print_vidomost_narahuvannya_dlya_abon_z_lich, SIGNAL(activated()), this, SLOT(action_print_vidomost_narahuvannya_dlya_abon_z_lich_activated()));
		connect(ui.action_print_info_pro_spojyvannya, SIGNAL(activated()), this, SLOT(action_print_info_pro_spojyvannya_activated()));
		connect(ui.action_CheckDataBaseIsCorrect, SIGNAL(activated()), this, SLOT(action_CheckDataBaseIsCorrect_activated()));
		connect(ui.action_Options, SIGNAL(activated()), this, SLOT(action_Options_activated()));
		connect(ui.action_insert_electronic_kvytancii_UNI, SIGNAL(activated()), this, SLOT(action_insert_electronic_kvytancii_UNI_activated()));
		connect(ui.action_insert_electronic_subs_UNI, SIGNAL(activated()), this, SLOT(action_insert_electronic_subs_UNI_activated()));
		connect(ui.action_print_zvit_forma_n_2_pilga_2007, SIGNAL(activated()), this, SLOT(action_print_zvit_forma_n_2_pilga_2007_activated()));
		connect(ui.action_zagalna_vidomist_narahuvannya_i_postuplennya, SIGNAL(activated()), this, SLOT(action_zagalna_vidomist_narahuvannya_i_postuplennya_activated()));
		connect(ui.action_open_insert_kvyt_form, SIGNAL(activated()), this, SLOT(action_open_insert_kvyt_form_activated()));
		connect(ui.action_export_f2_v2007, SIGNAL(activated()), this, SLOT(action_export_f2_v2007_activated()));
		
		connect(ui.action_print_vidnoshennya_pro_vidshkoduvannya_pilg_CO, SIGNAL(activated()), this, SLOT(action_print_vidnoshennya_pro_vidshkoduvannya_pilg_CO_activated()));
		connect(ui.action_print_vidomist_pererahunku_pilg_CO, SIGNAL(activated()), this, SLOT(action_print_vidomist_pererahunku_pilg_CO_activated()));
		connect(ui.action_print_vidnoshennya_pro_vidshkoduvannya_pilg_GV, SIGNAL(activated()), this, SLOT(action_print_vidnoshennya_pro_vidshkoduvannya_pilg_GV_activated()));
		connect(ui.action_print_vidomist_pererahunku_pilg_GV, SIGNAL(activated()), this, SLOT(action_print_vidomist_pererahunku_pilg_GV_activated()));
		
		connect(ui.action_printVidomistOtrymannyaPoslugPoBudynkah, SIGNAL(activated()), this, SLOT(action_printVidomistOtrymannyaPoslugPoBudynkah_activated()));
		connect(ui.action_printVidomistOtrymannyaPoslugPoKotelnyah, SIGNAL(activated()), this, SLOT(action_printVidomistOtrymannyaPoslugPoKotelnyah_activated()));
		connect(ui.action_printVidomistSpojyvannyaCOZaRik, SIGNAL(activated()), this, SLOT(action_printVidomistSpojyvannyaCOZaRik_activated()));
		connect(ui.action_printVidomistSpojyvannyaGVZaRik, SIGNAL(activated()), this, SLOT(action_printVidomistSpojyvannyaGVZaRik_activated()));
		
		connect(ui.action_print_vidomist_samovilnoVidkluchAbon, SIGNAL(activated()), this, SLOT(action_print_vidomist_samovilnoVidkluchAbon_activated()));
		connect(ui.action_printNadannyaPoslugPoBudTaKot, SIGNAL(activated()), this, SLOT(action_printNadannyaPoslugPoBudTaKot_activated()));
		connect(ui.action_printDebetoveKredytoveSaldo,SIGNAL(activated()),this,SLOT(action_printDebetoveKredytoveSaldo_activated()));
		connect(ui.action_printKvytanciiBeznaiynogoBorgu,SIGNAL(activated()),this,SLOT(action_printKvytanciiBeznaiynogoBorgu_activated()));
		connect(ui.action_printKvytanciiBeznaiynogoBorguPotochnAbonent,SIGNAL(activated()),this,SLOT(action_printKvytanciiBeznaiynogoBorguPotochnAbonent_activated()));
		
		connect(ui.action_printVidomistBorjnykivOtrymSubs,SIGNAL(activated()),this,SLOT(action_printVidomistBorjnykivOtrymSubs_activated()));
		connect(ui.action_print_zaborg_za_periody,SIGNAL(activated()),this,SLOT(action_print_zaborg_za_periody_activated()));
		
		connect(ui.action_save_data_base_toArchive, SIGNAL(activated()), this, SLOT(action_save_data_base_toArchive_activated()));
		connect(ui.action_pokazn_opal_stoyak, SIGNAL(activated()), this, SLOT(action_pokazn_opal_stoyak_activated()));
		connect(ui.action_narah_ST, SIGNAL(activated()), this, SLOT(action_narah_ST_activated()));
		connect(ui.action_znyattya_narahuv_vid_vidkluchenyh, SIGNAL(activated()), this, SLOT(action_znyattya_narahuv_vid_vidkluchenyh_activated()));
		connect(ui.action_postDataBaseForAuthurAnalize, SIGNAL(activated()), this, SLOT(action_postDataBaseForAuthurAnalize_activated()));
		connect(ui.action_fastPilgovykyDocInsert, SIGNAL(activated()), this, SLOT(action_fastPilgovykyDocInsert_activated()));
		connect(ui.action_export_vidomist_narahuvannya_po_lich_dbf, SIGNAL(activated()), this, SLOT(action_export_vidomist_narahuvannya_po_lich_dbf_activated()));
		connect(ui.action_insertOpSubsFromFile, SIGNAL(activated()), this, SLOT(action_insertOpSubsFromFile_activated()));
		
		connect(ui.action_calcDebutorskaZaborgVhSaldo, SIGNAL(activated()), this, SLOT(action_calcDebitorskaZaborgVhSaldo_activated()));
		connect(ui.action_fillSubsDbfSpojyvannyaCO, SIGNAL(activated()), this, SLOT(action_fillSubsDbfSpojyvannyaCO_activated()));
		
		connect(calculatorButton, SIGNAL(clicked ()), this, SLOT(calculatorButton_clicked()));
		
		connect(ui.pushButton_add_budynky, SIGNAL(clicked ()), this, SLOT(buttonclick_add_budynky())); // dodavannya budynkiv
		connect(ui.pushButton_del_budynky, SIGNAL(clicked ()), this, SLOT(buttonclick_del_budynky())); // vydalennya budynkiv
		connect(model_budynky, SIGNAL(primeInsert ( int, QSqlRecord &)), this, SLOT(budynky_prime_insert( int, QSqlRecord &))); //data changed in model_kvytancii
		connect(ui.pushButton_add_pilgy, SIGNAL(clicked ()), this, SLOT(buttonclick_add_pilgy())); // dodavannya pilg
		connect(ui.pushButton_del_pilgy, SIGNAL(clicked ()), this, SLOT(buttonclick_del_pilgy())); // vydalennya pilg		
		connect(ui.pushButton_add_normat_taryf, SIGNAL(clicked ()), this, SLOT(buttonclick_add_normat_taryf())); // dodavannya normat taryf
		connect(ui.pushButton_del_normat_taryf, SIGNAL(clicked ()), this, SLOT(buttonclick_del_normat_taryf())); // vydalennya normat taryf

				//abonent form connectors
		connect(ui.spinBox_navigator, SIGNAL(valueChanged (int)), this, SLOT(populate_abonent_form(int))); //  populate abonent form after spinbox_navigator val. ch.
		connect(ui.comboBox_Poshuk, SIGNAL(currentIndexChanged (int)), ui.spinBox_navigator, SLOT(setValue(int))); //spinbox_navigator val. ch. after comboBox_Poshuk val.ch.
		connect(ui.comboBox_Poshuk_rahunok, SIGNAL(currentIndexChanged (int)), ui.spinBox_navigator, SLOT(setValue(int))); //spinbox_navigator val. ch. after comboBox_Poshuk_rahunok val.ch.
		connect(ui.comboBox_Poshuk_rahunok, SIGNAL(currentIndexChanged (int)), this, SLOT(abonent_filtr_combobox_index_changed(int))); //spinbox_navigator val. ch. after comboBox_Poshuk_rahunok val.ch.
		connect(ui.toolButton_extPoshuk, SIGNAL(clicked ()), this, SLOT(action_extSearch_activated()));
		connect(ui.pushButton_first, SIGNAL(clicked ()), this, SLOT(abonent_form_first())); //first button --> spinBox_navigator
		connect(ui.pushButton_previous, SIGNAL(clicked ()), this, SLOT(abonent_form_previous())); //previous button --> spinBox_navigator
		connect(ui.pushButton_next, SIGNAL(clicked ()), this, SLOT(abonent_form_next())); //next button --> spinBox_navigator
		connect(ui.pushButton_last, SIGNAL(clicked ()), this, SLOT(abonent_form_last())); //last button --> spinBox_navigator
		connect(ui.pushButton_new, SIGNAL(clicked ()), this, SLOT(abonent_form_new())); //new button --> new Record
		connect(ui.spinBox_navigator, SIGNAL(valueChanged (int)), this, SLOT(enable_navigation_buttons(int))); //enable/disable navigation buttons
		connect(ui.spinBox_year_filtr, SIGNAL(valueChanged (int)), this, SLOT(populate_operacii_table())); //year filtr changed
		connect(ui.spinBox_year_filtr, SIGNAL(valueChanged (int)), this, SLOT(populateBeznadiynaDebitorskaZaborgTable())); //year filtr changed
		connect(ui.spinBox_year_filtr, SIGNAL(valueChanged (int)), this, SLOT(populate_subs_spl_zaborg_labels())); ////year filtr changed
		connect(ui.pushButton_abonent_form_save, SIGNAL(clicked ()), this, SLOT(pushButton_abonent_form_save_clicked())); //save button --> Save Record
		connect(ui.pushButton_abonent_form_abort, SIGNAL(clicked ()), this, SLOT(pushButton_abonent_form_abort_clicked())); //abort button --> Abort Editing Record
		
				//diuch taryf connectors
		connect(ui.spinBox_month_diuch_taryf, SIGNAL(valueChanged (int)), this, SLOT(diuch_taryf_date_changed())); //month changed
		connect(ui.spinBox_year_diuch_taryf, SIGNAL(valueChanged (int)), this, SLOT(diuch_taryf_date_changed())); //year changed
		connect(ui.pushButton_diuch_taryf_save, SIGNAL(clicked ()), this, SLOT(diuch_taryf_save())); //save diuch taryf
		connect(ui.pushButton_diuch_taryf_calc, SIGNAL(clicked ()), this, SLOT(diuch_taryf_calc())); //calc diuch taryf
		connect(ui.pushButton_diuch_taryf_print, SIGNAL(clicked ()), this, SLOT(diuch_taryf_print())); //calc diuch taryf
		connect(ui.tableWidget_diuch_taryf, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(diuch_taryf_widget_item_changed(QTableWidgetItem *))); //item changed --->>> itemhighlighting
		connect(ui.action_diuch_taryf_obedn_budynky, SIGNAL(activated()), this, SLOT(action_diuch_taryf_obedn_budynky_activated()));
		connect(ui.action_diuch_taryf_select_all, SIGNAL(activated()), ui.tableWidget_diuch_taryf, SLOT(selectAll()));
		connect(ui.action_diuch_taryf_calc_taryf, SIGNAL(activated()), this, SLOT(diuch_taryf_calc()));
		connect(ui.action_diuch_taryf_save_taryf, SIGNAL(activated()), this, SLOT(diuch_taryf_save()));
		
				//subs connectors
		connect(ui.spinBox_month_subs, SIGNAL(valueChanged (int)), this, SLOT(subs_date_changed())); //month changed
		connect(ui.spinBox_year_subs, SIGNAL(valueChanged (int)), this, SLOT(subs_date_changed())); //year changed
		connect(ui.pushButton_add_subs, SIGNAL(clicked ()), this, SLOT(buttonclick_add_subs())); // dodavannya subs
		connect(ui.pushButton_del_subs, SIGNAL(clicked ()), this, SLOT(buttonclick_del_subs())); // vydalennya subs
		connect(ui.pushButton_insert_subs_form, SIGNAL(clicked ()), this, SLOT(buttonclick_open_insert_subs_form())); // внесенння донарахування субсидій з допомогою форми
		connect(ui.pushButton_insert_subs_narah_form, SIGNAL(clicked ()), this, SLOT(buttonclick_open_insert_subs_narah_form())); // внесенння нарахування субсидій з допомогою форми
		connect(model_spravy_subs, SIGNAL(primeInsert ( int, QSqlRecord &)), this, SLOT(spravy_subs_prime_insert( int, QSqlRecord &))); //data changed in model_spravy_subs

				//korectura connectors
		connect(ui.spinBox_month_korektura, SIGNAL(valueChanged (int)), this, SLOT(korektura_date_changed())); //month changed
		connect(ui.spinBox_year_korektura, SIGNAL(valueChanged (int)), this, SLOT(korektura_date_changed())); //year changed
		connect(ui.pushButton_add_korektura, SIGNAL(clicked ()), this, SLOT(buttonclick_add_korektura())); // dodavannya korektura
		connect(ui.pushButton_del_korektura, SIGNAL(clicked ()), this, SLOT(buttonclick_del_korektura())); // vydalennya korektura
				//korectura_s connectors
		connect(ui.spinBox_month_korektura_s, SIGNAL(valueChanged (int)), this, SLOT(korektura_s_date_changed())); //month changed
		connect(ui.spinBox_year_korektura_s, SIGNAL(valueChanged (int)), this, SLOT(korektura_s_date_changed())); //year changed
		connect(ui.pushButton_add_korektura_s, SIGNAL(clicked ()), this, SLOT(buttonclick_add_korektura_s())); // dodavannya korektura_s
		connect(ui.pushButton_del_korektura_s, SIGNAL(clicked ()), this, SLOT(buttonclick_del_korektura_s())); // vydalennya korektura_s
				//kvytancii connectors
		connect(ui.pushButton_add_kvyt, SIGNAL(clicked ()), this, SLOT(buttonclick_add_kvyt())); // dodavannya kvyt
		connect(ui.pushButton_del_kvyt, SIGNAL(clicked ()), this, SLOT(buttonclick_del_kvyt())); // vydalennya kvyt
		connect(ui.pushButton_new_pack, SIGNAL(clicked ()), this, SLOT(buttonclick_new_pack_kvyt())); // Наступний пакет
		connect(ui.pushButton_insert_kvyt_form, SIGNAL(clicked ()), this, SLOT(buttonclick_insert_kvyt_form())); // Внесення квитанцій з форми
		connect(ui.spinBox_kvyt_opl_month, SIGNAL(valueChanged (int)), this, SLOT(kvyt_filtr_changed())); //month changed
		connect(ui.spinBox_kvyt_opl_year, SIGNAL(valueChanged (int)), this, SLOT(kvyt_filtr_changed())); //year changed
		connect(ui.spinBox_kvyt_pereved_month, SIGNAL(valueChanged (int)), this, SLOT(kvyt_filtr_changed())); //month changed
		connect(ui.spinBox_kvyt_pereved_year, SIGNAL(valueChanged (int)), this, SLOT(kvyt_filtr_changed())); //year changed
		connect(ui.spinBox_kvyt_pachka, SIGNAL(valueChanged (int)), this, SLOT(kvyt_filtr_changed())); //month changed
		connect(ui.groupBox_kvyt_oplata, SIGNAL(toggled (bool)), this, SLOT(kvyt_filtr_changed())); //oplata group box enabled
		connect(ui.groupBox_kvyt_pereved, SIGNAL(toggled (bool)), this, SLOT(kvyt_filtr_changed())); //perevedennya group box enabled
		connect(ui.groupBox_kvyt_pachka, SIGNAL(toggled (bool)), this, SLOT(kvyt_filtr_changed())); //pachka group box enabled
		connect(ui.spinBox_kvyt_opl_month, SIGNAL(valueChanged (int)), this, SLOT(kvyt_control_sum_changed())); //month changed
		connect(ui.spinBox_kvyt_opl_year, SIGNAL(valueChanged (int)), this, SLOT(kvyt_control_sum_changed())); //year changed
		connect(ui.spinBox_kvyt_pereved_month, SIGNAL(valueChanged (int)), this, SLOT(kvyt_control_sum_changed())); //month changed
		connect(ui.spinBox_kvyt_pereved_year, SIGNAL(valueChanged (int)), this, SLOT(kvyt_control_sum_changed())); //year changed
		connect(ui.spinBox_kvyt_pachka, SIGNAL(valueChanged (int)), this, SLOT(kvyt_control_sum_changed())); //month changed
		connect(ui.groupBox_kvyt_oplata, SIGNAL(toggled (bool)), this, SLOT(kvyt_control_sum_changed())); //oplata group box enabled
		connect(ui.groupBox_kvyt_pereved, SIGNAL(toggled (bool)), this, SLOT(kvyt_control_sum_changed())); //perevedennya group box enabled
		connect(ui.groupBox_kvyt_pachka, SIGNAL(toggled (bool)), this, SLOT(kvyt_control_sum_changed())); //pachka group box enabled
		connect(model_kvytancii, SIGNAL(primeInsert(int, QSqlRecord &)), this, SLOT(kvytancii_prime_insert( int, QSqlRecord &))); //data changed in model_kvytancii
		connect(model_kvytancii, SIGNAL(dataChanged(const QModelIndex &,const QModelIndex &)), this, SLOT(kvyt_control_sum_changed()));
		connect(model_kvytancii, SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(kvyt_control_sum_changed()));
		connect(model_kvytancii, SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(kvyt_control_sum_changed()));
				//spravy_subs connectors
		connect(ui.pushButton_add_spravy_subs, SIGNAL(clicked ()), this, SLOT(buttonclick_add_spravy_subs())); // dodavannya spravy_subs
		connect(ui.pushButton_del_spravy_subs, SIGNAL(clicked ()), this, SLOT(buttonclick_del_spravy_subs())); // vydalennya spravy_subs
		connect(ui.radioButton_spravy_subs_sort_ruh, SIGNAL(toggled (bool)), this, SLOT(spravy_subs_order_changed(bool))); // zmina filtra (radiobutton)
				//splata_zaborgovanosti connectors
		connect(ui.pushButton_add_splata_zaborg, SIGNAL(clicked ()), this, SLOT(buttonclick_add_splata_zaborg())); // dodavannya spravy splaty zaborg
		connect(ui.pushButton_del_splata_zaborg, SIGNAL(clicked ()), this, SLOT(buttonclick_del_splata_zaborg())); // vydalennya spravy splaty zaborg
		connect(ui.groupBox_spravy_splata_zaborg, SIGNAL(toggled (bool)), this, SLOT(splata_zaborg_order_changed(bool))); // zmina filtra (radiobutton)
		connect(model_splata_zaborg, SIGNAL(primeInsert ( int, QSqlRecord &)), this, SLOT(splata_zaborg_prime_insert( int, QSqlRecord &))); //data changed in model_splata_zaborg
		
				//kotelni connectors
		connect(ui.pushButton_add_kotelni, SIGNAL(clicked ()), this, SLOT(buttonclick_add_kotelni())); // dodavannya spravy splaty zaborg
		connect(ui.pushButton_del_kotelni, SIGNAL(clicked ()), this, SLOT(buttonclick_del_kotelni())); // vydalennya spravy splaty zaborg
		connect(model_kotelni, SIGNAL(primeInsert ( int, QSqlRecord &)), this, SLOT(kotelni_prime_insert( int, QSqlRecord &))); //data changed in model_kotelni
				//pilgovyky connectors
		connect(ui.pushButton_add_pilgovyky, SIGNAL(clicked ()), this, SLOT(buttonclick_add_pilgovyky())); // dodavannya spravy splaty zaborg
		connect(ui.pushButton_del_pilgovyky, SIGNAL(clicked ()), this, SLOT(buttonclick_del_pilgovyky())); // vydalennya spravy splaty zaborg
		connect(ui.groupBox_filtr_kategor_pilgovyky, SIGNAL(toggled (bool)), this, SLOT(pilgovyky_filtr_changed())); //groupBox_filtr_kategor_pilgovyky group box enabled
		connect(ui.comboBox_filtr_pigovyky_kategoriya, SIGNAL(currentIndexChanged(int)), this, SLOT(pilgovyky_filtr_changed())); //kategoriya dlya pilgy changed
		connect(model_pilgovyky, SIGNAL(dataChanged(const QModelIndex &,const QModelIndex &)), this, SLOT(pilgovyky_control_sum_changed())); //data changed in model_pilgovyky
		connect(model_pilgovyky, SIGNAL(primeInsert ( int, QSqlRecord &)), this, SLOT(pilgovyky_prime_insert( int, QSqlRecord &))); //data changed in model_pilgovyky
		connect(ui.groupBox_filtr_kategor_pilgovyky, SIGNAL(toggled (bool)), this, SLOT(pilgovyky_control_sum_changed())); //groupBox_filtr_kategor_pilgovyky group box enabled
		connect(ui.comboBox_filtr_pigovyky_kategoriya, SIGNAL(currentIndexChanged(int)), this, SLOT(pilgovyky_control_sum_changed())); //kategoriya dlya pilgy changed
		
		//default values part connections
			//budynky table default values connectors 
		connect(model_budynky, SIGNAL(beforeInsert(QSqlRecord &)), this, SLOT(before_change_default(QSqlRecord &))); //beforeInsert
		connect(model_budynky, SIGNAL(beforeUpdate(int, QSqlRecord &)), this, SLOT(before_change_default(int, QSqlRecord &))); //beforeInsert
		connect(model_kotelni, SIGNAL(beforeInsert(QSqlRecord &)), this, SLOT(before_change_default(QSqlRecord &))); //beforeInsert
		connect(model_kotelni, SIGNAL(beforeUpdate(int, QSqlRecord &)), this, SLOT(before_change_default(int, QSqlRecord &))); //beforeInsert

		connect(model_spravy_subs, SIGNAL(beforeInsert(QSqlRecord &)), this, SLOT(before_change_default(QSqlRecord &))); //beforeInsert
		connect(model_spravy_subs, SIGNAL(beforeUpdate(int, QSqlRecord &)), this, SLOT(before_change_default(int, QSqlRecord &))); //beforeInsert
		connect(model_splata_zaborg, SIGNAL(beforeInsert(QSqlRecord &)), this, SLOT(before_change_default(QSqlRecord &))); //beforeInsert
		connect(model_splata_zaborg, SIGNAL(beforeUpdate(int, QSqlRecord &)), this, SLOT(before_change_default(int, QSqlRecord &))); //beforeInsert
		connect(model_pilgovyky, SIGNAL(beforeInsert(QSqlRecord &)), this, SLOT(before_change_default(QSqlRecord &))); //beforeInsert
		connect(model_pilgovyky, SIGNAL(beforeUpdate(int, QSqlRecord &)), this, SLOT(before_change_default(int, QSqlRecord &))); //beforeInsert
		connect(model_kvytancii, SIGNAL(beforeInsert(QSqlRecord &)), this, SLOT(before_change_default(QSqlRecord &))); //beforeInsert
		connect(model_kvytancii, SIGNAL(beforeUpdate(int, QSqlRecord &)), this, SLOT(before_change_default(int, QSqlRecord &))); //beforeInsert
		
		connect(dateEdit_PotochnaData, SIGNAL(dateChanged(const QDate &)), this, SLOT(curDateChanged()));
		
		connect(ui.toolButton_lichCOInfo, SIGNAL(clicked ()), this, SLOT(showAbonLichCO()));
		connect(ui.toolButton_lichGVInfo, SIGNAL(clicked ()), this, SLOT(showAbonLichGV()));
		
		// --- CONNECTIONS END ---
		
		//after connectors operators
		diuch_taryf_date_changed();
		
		populateControlObjectsHash();
		action_close_login_activated(); //По замовчуванню форма не активна
		
		setOrganizSpacificSettings();
		
#ifdef TKE_DEMO
		setTkeDbfSettings();
		show();
#endif
		action_registracion_activated();
		
		//Виправити
		//ui.action_postDataBaseForAuthurAnalize->setVisible(false);
		ui.action_CheckDataBaseIsCorrect->setVisible(false);
		//ui.action_print_vidomist_samovilnoVidkluchAbon->setVisible(false);
		if (organizID() != 1)
			ui.action_calcObsyagTeplEnerg->setVisible(false);
}
//------------------------------------------------------------
