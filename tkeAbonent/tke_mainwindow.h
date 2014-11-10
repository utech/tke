//------------------------------------------------------------
//
// Implamantation of tke_MainWindow class.
// Created by Roman Mulyk 08.08.2006
//
//------------------------------------------------------------

#ifndef TKE_MAINWINDOW_H
#define TKE_MAINWINDOW_H
// QT standart includes
#include <QMainWindow>
#include <QSqlRelationalTableModel>
#include <QSqlTableModel>
#include <QTableView>
#include <QTableWidget>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTableWidgetItem>
#include <QTextCodec>
#include <QModelIndex>
#include <QSqlRelationalDelegate>
#include <QHash>
#include <QSize>
#include <QHeaderView>
#include <QPalette>
#include <QDate>
#include <QTextCursor>
#include <QTextBlockFormat>
#include <QStringList>
#include <QVector>
#include <QTextTable>
#include <QTextTableCell>
#include <QTextCharFormat>
#include <QList>
#include <QFileDialog>
//#include <QAssistantClient>
#include <QSqlError>
#include <QSettings>
#include <QProcess>
#include <QDateTime>

#include <UPostupForm>
#include <UPrintForm>
#include <UTableWidgetSqlExt>

#include "tke_math.h"
#include "uprint_docs.h"
#include "db_own.h"
#include "../tkeOrganizacii/udiff_func.h"
#include "UTkeGlobal.h"
#include "UAbonentFunctions.h"
#include "uexport_info_pilgovyky.h"

#include "ui_tke_mainwindow.h"

#include <QtDebug>

class Browser;

// Classes
  class tke_MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        tke_MainWindow(QWidget *parent = 0);
		
		void setOrganizSpacificSettings();
		void clear_operacii_table();
		void calc_operacii_table();
		void ___pererah_saldo(int month, int year, bool all = false);
		void setSettingsFromIni();
		void saveSettingsToIni();
		void populateControlObjectsHash();
		 
		void initBeznadiynaDebitorskaZaborgTable();
		void clearBeznadiynaDebitorskaZaborgTable();
		void calcBeznadiynaDebitorskaZaborgTable();
		void calcDebVhSaldoInMonth(short year, short month, UPostupForm * pForm);
		
		~tke_MainWindow();
    public slots:
		void exec_abonent_select_query();
		void exec_diuch_taryf_select_query(short month_filtr, short year_filtr);
		void populate_comboboxes_abonent();
		void populate_abonent_form( int seek_query );
		void populate_operacii_table();
		void tableWidget_rozrahunky_cellDoubleClicked(int row,int col);
		void populate_subs_spl_zaborg_labels();
		void populate_pilgovyky_filtr_comboboxes();
		void enable_navigation_buttons(int index);
		void diuch_taryf_date_changed();
		void subs_date_changed();
		void korektura_date_changed();
		void korektura_s_date_changed();
		void kvyt_filtr_changed();
		void pilgovyky_filtr_changed();
		void spravy_subs_order_changed(bool b_val);
		void splata_zaborg_order_changed(bool b_val);
		void kvyt_control_sum_changed();
		void pilgovyky_control_sum_changed();
		void checkBox_Opal_state_changed();
		void checkBox_Zdano_Pilgy_state_changed();
		void checkBox_GarVoda_state_changed();
		void checkBox_narahStoyak_state_changed();
		void checkBox_IsSamovilnyk_state_changed();
		void abonent_form_connect_editres();
		void abonent_form_disconnect_editres();
		void abonent_form_first();
		void abonent_form_previous();
		void abonent_form_next();
		void abonent_form_last();
		void abonent_form_new();
		void abonent_form_record_editing();
		void abonent_form_record_end_editing();
		void abonent_form_update_record();
		void abonent_form_insert_record();
		void abonent_filtr_combobox_index_changed(int new_index);
		void abonent_doubleSpinBox_Ploshcha_valueChanged();
		void pushButton_abonent_form_save_clicked();
		void pushButton_abonent_form_abort_clicked();
		void diuch_taryf_widget_item_changed(QTableWidgetItem *item);
		void diuch_taryf_save();
		void diuch_taryf_calc();
		void diuch_taryf_print();
		
		void beznadiynaDebitorskaZaborgTableWidgetItemChanged(QTableWidgetItem *);
		void populateBeznadiynaDebitorskaZaborgTable();
		void showBeznDebitZabordFlag();
		
		void action_registracion_activated();
		void action_extSearch_activated();
		void action_structPostachGV_activated();
		void action_calcObsyagTeplEnerg_activated();
		void action_close_login_activated();
        void action_budynky_activated();
		void action_pilgy_activated();
		void action_normat_taryf_activated();
		void action_diuchyi_taryf_activated();
		void action_pokaz_lich_GV_activated();
		void action_korektura_activated();
		void action_korektura_s_activated();
		void action_narah_activated();
		void action_narah_GV_activated();
		void action_print_vidomist_pilg();
		void action_print_vidomist_pilg_kategor();
		void action_print_zvit_pilg_kategor();
		void action_print_vidomist_narah_activated();
		void action_print_dovidka_oderj_subs_activated();
		void action_print_kvytancii_activated();
		void action_print_kvytancii_for_abonent_activated();
		void action_printCustomKvyt_activated();
		void action_print_forma_akt_vytrat_teploenergii_activated();
		void action_subs_activated();
		void action_kvytancii_activated();
		void action_spravy_subs_activated();
		void action_zaborg_activated();
		void action_kotelni_activated();
		void action_pilgovyky_activated();
		void action_opSubs_activated();
		void action_narahPereved_activated();
		void action_pererahunok_saldo_activated();
		void action_pererahunok_saldo_all_activated();
		void action_print_rahunok_abonenta_activated();
		void action_save_vidomist_pilg_to_file_activated();
		void action_insert_dbf_kvyt_oshchadbank_activated();
		void action_set_data_sources_activated();
		void action_print_borjnyky_activated();
		void action_debuger_form_activated();
		void action_archiv_vidomosti_kvytanciy_ob_activated();
		void action_insert_subs_dbf_activated();
		void action_print_vidomist_borjnykiv_riznyc_activated();
		void action_print_wrong_subs();
		void action_print_dovidnyk_abonentiv_activated();
		void action_dovidnykAbonentivVRozriziNadannyaPoslug_activated();
		void action_diuch_taryf_obedn_budynky_activated();
		void action_sqlBrowser_activated();
		void action_posada_activated();
		void action_assistent_activated();
		void action_about_activated();
		void action_edit_users_activated();
		void action_print_povidomlennya_dlya_borjnykiv_activated();
		void action_print_povidomlennya_dlya_pilgovykiv_activated();
		void action_vidomist_narah_oplaty_za_CO_dla_viddilu_subs_activated();
		void action_open_insert_narah_subs_form_activated();
		void action_open_insert_donarah_subs_form_activated();
		void action_clear_vidomosti_pro_zdani_dokumenty_napilgy_activated();
		void action_print_vidomist_zdanyh_dokumentiv_na_pilgy_activated();
		
		void action_print_dovidka_u_subs_activated();
		void action_print_dovidka_u_subs_2_activated();
		void action_print_vidomost_narahuvannya_dlya_abon_z_lich_activated();
		void action_print_info_pro_spojyvannya_activated();
		void action_CheckDataBaseIsCorrect_activated();
		void action_Options_activated();
		void action_insert_electronic_kvytancii_UNI_activated();
		void action_insert_electronic_subs_UNI_activated();
		void action_print_zvit_forma_n_2_pilga_2007_activated();
		void action_zagalna_vidomist_narahuvannya_i_postuplennya_activated();
		void action_open_insert_kvyt_form_activated();
		void action_export_f2_v2007_activated();
		void action_print_vidnoshennya_pro_vidshkoduvannya_pilg_CO_activated();
		void action_print_vidomist_pererahunku_pilg_CO_activated();
		void action_print_vidnoshennya_pro_vidshkoduvannya_pilg_GV_activated();
		void action_print_vidomist_pererahunku_pilg_GV_activated();
		
		void action_printVidomistOtrymannyaPoslugPoBudynkah_activated();
		void action_printVidomistOtrymannyaPoslugPoKotelnyah_activated();
		void action_printVidomistSpojyvannyaCOZaRik_activated();
		void action_printVidomistSpojyvannyaGVZaRik_activated();
		void action_print_vidomist_samovilnoVidkluchAbon_activated();
		void action_printNadannyaPoslugPoBudTaKot_activated();
		void action_printDebetoveKredytoveSaldo_activated();
		void action_printKvytanciiBeznaiynogoBorgu_activated();
		void action_printKvytanciiBeznaiynogoBorguPotochnAbonent_activated();
		
		void action_printVidomistBorjnykivOtrymSubs_activated();
		void action_print_zaborg_za_periody_activated();
		
		void curDateChanged();
		
		void action_save_data_base_toArchive_activated();
		void action_pokazn_opal_stoyak_activated();
		void action_narah_ST_activated();
		void action_znyattya_narahuv_vid_vidkluchenyh_activated();
		void action_postDataBaseForAuthurAnalize_activated();
		void action_fastPilgovykyDocInsert_activated();
		void action_export_vidomist_narahuvannya_po_lich_dbf_activated();
		void action_insertOpSubsFromFile_activated();
		void action_fillSubsDbfSpojyvannyaCO_activated();
		
		void action_calcDebitorskaZaborgVhSaldo_activated();
		
		void calculatorButton_clicked();
		
		void populateLichPokaznAbon();
		void showAbonLichCO();
		void showAbonLichGV();
		
		void buttonclick_add_budynky();
		void buttonclick_del_budynky();
		void buttonclick_add_pilgy();
		void buttonclick_del_pilgy();
		void buttonclick_add_normat_taryf();
		void buttonclick_del_normat_taryf();
		void buttonclick_add_korektura();
		void buttonclick_del_korektura();
		void buttonclick_add_korektura_s();
		void buttonclick_del_korektura_s();
		void buttonclick_add_subs();
		void buttonclick_del_subs();
		void buttonclick_open_insert_subs_form();
		void buttonclick_open_insert_subs_narah_form();
		void buttonclick_add_kvyt();
		void buttonclick_del_kvyt();
		void buttonclick_new_pack_kvyt();
		void buttonclick_insert_kvyt_form();
		void buttonclick_add_spravy_subs();
		void buttonclick_del_spravy_subs();
		void buttonclick_add_splata_zaborg();
		void buttonclick_del_splata_zaborg();
		void buttonclick_add_kotelni();
		void buttonclick_del_kotelni();
		void buttonclick_add_pilgovyky();
		void buttonclick_del_pilgovyky();
		void pushButton_addOpString_clicked();
		void pushButton_delOpString_clicked();
		void setOpDateFilter();
		void pushButton_narahPereved_addString_clicked();
		void pushButton_narahPereved_delString_clicked();
		void setNarahPerevedDateFilter();
		void pilgovyky_prime_insert( int row, QSqlRecord & record);
		void budynky_prime_insert( int row, QSqlRecord & record);
		void kvytancii_prime_insert( int row, QSqlRecord & record);
		void splata_zaborg_prime_insert( int row, QSqlRecord & record);
		void kotelni_prime_insert( int row, QSqlRecord & record);
		void spravy_subs_prime_insert( int row, QSqlRecord & record);
		//default values in tables slots
		void before_change_default(QSqlRecord & record);
		void before_change_default(int row, QSqlRecord & record);
		//permishion
		void saveMode();
		void workMode(int mode);
		void loadGUISettingsFromIni();
		
		void look_abonent_info( const QModelIndex &  index,  const QModelIndex &  index_ignored = QModelIndex());
		void activ_column_changes( const QModelIndex & current, const QModelIndex & previous );
		void show_rahunok(int rah);
		void showDebugWindow();
	signals:
		void requestDebugWindow();
		void closeLoginSignal();
		void mainWindowClosed();
		void requestSaveDataBaseWindow();
	
    private:
		Ui::tke_MainWindow ui;
	    QDateEdit *dateEdit_PotochnaData;
		QPushButton *calculatorButton;
		QHash<int, QObject *> *controlObjectsHash;
		
		QList<int> pilgyList;
		QList<int> taryf_ID;
		QList<double> taryf;
		QList<int> pilga_ID;
		QList<double> pilga_vids;
		QList<double> pilga_ploshcha;
		QVector<int> abonentsList;
		QDate date;
		QDate currentSysDate;
		int record_edited; //0-record not edited, 1 - record edited
		int new_record; // 1 - new record< 0-not;
		unsigned char days_in_month[12];
		UNarah_data d;
		QStringList monthlist;
		
		//default values variables
		QString d_Vulycia;				// Вулиця
		double	d_Koef;					// Коефіцієнт переведення
		int		d_Kotelnya_ID;			// Ідентифікатор котельні
		QString	d_City;					// Назва населеного пункту
		int		d_Year;					// Рік
		int		d_Month;				// Місяць
		int		d_Sprava;				// Справа
		int		d_Ugoda;				// Угода
		int		d_Month_poch;			// Місяць початку
		int		d_Year_poch;			// Рік початку
		int		d_Month_end;			// Місяць кінця
		int		d_Year_end;				// Рік кінця
		int		d_Kategoriya_pilgy_ID;	// Ідентифікатор пільги
		QDate	d_Splata_date;			// Дата оплати
		QDate	d_Perevid_Date;			// Дата переведення
		int		d_Pachka;				// № пачки
		
		bool IRC;
		
        //QAssistantClient *assistant;
		
		QSqlQuery *abonent_select_query;
		QSqlQuery *abonent_temp_query;
		QSqlQuery *narah_select_query;
		QSqlQuery *narah_temp_query;
		QSqlQuery *t_filtr_select_query; //видаляється відразу після використання
		QSqlTableModel *abonent_update_model;
		QSqlTableModel *abonent_insert_model;
		QSqlTableModel *narah_update_model;
		QSqlTableModel *korektura_update_model;
		QSqlTableModel *diuch_taryf_update_model;
		QSqlTableModel *posadaModel;
		QSqlQuery *diuch_taryf_query;
		QSqlRelationalTableModel *model_budynky;
		QSqlRelationalTableModel *model_pilgy;
		QSqlRelationalTableModel *model_normat_taryf;
		QSqlRelationalTableModel *model_korektura;
		QSqlRelationalTableModel *model_subs;
		QSqlRelationalTableModel *model_kvytancii;
		QSqlRelationalTableModel *model_spravy_subs;
		QSqlRelationalTableModel *model_splata_zaborg;
		QSqlRelationalTableModel *model_kotelni;
		QSqlRelationalTableModel *model_pilgovyky;
		QSqlRelationalTableModel *model_korektura_s;
		QTextCodec *codec;
		QHash<QString, int> *pilgy_hash;
		QPalette *palette, *default_palette;
		
		UTableWidgetSqlExt *opTwExt;
		UTableWidgetSqlExt *perevedTwExt;
    };
#endif
