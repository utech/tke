//------------------------------------------------------------
//
// Implamantation of tke_MainWindow class.
// Created by Roman Mulyk 19.02.2007
//
//------------------------------------------------------------

#ifndef TKE_MAINWINDOW_ORG_H
#define TKE_MAINWINDOW_ORG_H
// QT standart includes
#include <QMainWindow>
#include <QTSql>
#include <QHeaderView>
#include <QList>
#include <QDate>
#include <QVector>
#include <QTextCodec>
#include <QMessageBox>
#include <QSqlError>
#include <QHash>
//#include <QAssistantClient>

#include "ui_tke_mainwindow_org.h"
#include "tke_dialog_edit_pidrozdil.h"
#include "../tkeAbonent/tke_math.h"
#include "utaryf_delegate.h"
#include "upokaz_lich_opal_delegate.h"
#include "udni_opal_query_model_delegate.h"
#include "udiff_func.h"
#include "tke_form_days_opal.h"
#include "tke_form_days_opal_simple.h"
#include <UPostupForm>
#include "../tkeAbonent/tke_dialog_narah.h"
#include "uoplata_delegate.h"
#include "uprint_docs_organiz.h"
#include "../src_other/replaceStrings/UReplaceStringsParser.h"
#include "ucalc_def_values_thread.h"
#include "tke_dialog_print_rahunok.h"
#include "tke_dialog_filtr_borjnykiv_organiz.h"
#include "tke_dialog_date_interval.h"
#include "../tkeAbonent/tke_dialog_login.h"
#include "../tkeAbonent/tke_dialog_change_password.h"
#include "../tkeAbonent/tke_dialog_options.h"
#include "../tkeAbonent/tke_Dialog_saveDataBase.h"
#include "../UTkeGlobal.h"

#include <QtDebug>


class Browser;

// Classes
  class tke_MainWindow_org : public QMainWindow
    {
        Q_OBJECT

    public:
        tke_MainWindow_org(QWidget *parent = 0);
		
		int 	__insertPidrozdilRecord(); //Повертає id внесеного підрозділу
		void 	__execPokazLichSelectQuery(int monthFiltr, int yearFiltr);
		void 	__execPokazLichGarVodaSelectQuery(int monthFiltr, int yearFiltr);
		void	__execMeteorSelectQuery(int monthFiltr, int yearFiltr);
		void	__pererahAllSaldoMonth(short month, short year, UPostupForm *postupForm);
		bool	__pererahCurUgodaSaldo(int ugoda, short year, short month);
		bool    __pererahAllUgodaSaldo(short year, short month);
		
		void setSettingsFromIni();
		void saveSettingsToIni();
		void populateControlObjectsHash();
		
		~tke_MainWindow_org();
    public slots:
    	void openSqlBrowser();
		void execOrganizSelectQuery();
		void populateOrganizComboboxes();
		void organizConnectEditres();
		void organizDisConnectEditres();
		void organizFormRecordEditing();
		void organizFormRecordEndEditing();
		void organizFormUpdateRecord();
		void organizFormInsertRecord();
		void organizFormFirst();
		void organizFormPrevious();
		void organizFormNext();
		void organizFormLast();
		void organizFormNew();
		void organizButtonSaveClicked();
		void organizButtonCancelClicked();
		void organizPopulateOperaciiTable();
		void organizBeforeChangeDefaultOplata(QSqlRecord & record);
		void clearOperaciiTable();
		void organizFiltrComboboxIndexChanged(int index);
		void enableNavigationButtons(int index);
		void populateOrganizForm( int seek_query );
		
		void execOrganizSlujbRahSelectQuery();
		void organizSlujbRahConnectEditres();
		void organizSlujbRahDisConnectEditres();
		void organizSlujbRahFormRecordEditing();
		void organizSlujbRahFormRecordEndEditing();
		void organizSlujbRahFormUpdateRecord();
		void organizSlujbRahFormInsertRecord();
		void organizSlujbRahFormFirst();
		void organizSlujbRahFormPrevious();
		void organizSlujbRahFormNext();
		void organizSlujbRahFormLast();
		void organizSlujbRahFormNew();
		void populateOrganizSlujbRahForm( int seek_query );
		void enableSlujbRahNavigationButtons(int index);
		void clearSlujbRahOperaciiTable();
		void organizSlujbRahPopulateOperaciiTable();
		void organizButtonSlujbRahSaveClicked();
		void organizButtonSlujbRahCancelClicked();
		void organizSlujbRahPopulateFiltrComboboxIndexChanged(int index);
		void organizSlujbRahUpdateData(QTableWidgetItem * item);
		void organizSlujbRahOperaciiTableCalcPidsumky();
		
		void populateNarahPidrozdilyTable();
		void clearNarahPidrozdilyForm();
		void populateNarahPidrozdilyForm(int pidrozdilId, QDate curDate);
		void showPidrozdilInfo();
		void showNarahPidrozdilInfo();
		void showDniOpalennyaForm();
		void populateTableKilkistDnivOpal();
		void dniOpalTableChanged();
		void opalLichZnachChanged();
		void opalGVLichZnachChanged();
		void narahZnachChanged();
		void narahNarahPidrozdily();
		void printRahunokNarahPidrozdily();
		void saveNarahPidrozdilyZnach();
		void reloadNarahPidrozdilInfo();

		void action_registracion_activated();
		void action_close_login_activated();
		void actionEditPidrozdil_activated();
		void actionAddPidrozdil_activated();
		void actionDelPidrozdil_activated();
		void action_taryf_table_activated();
		void action_pokaz_lich_table_activated();
		void action_pokaz_lich_gar_voda_activated();
		void action_meteor_table_activated();
		void action_dni_opalennya_activated();
		void action_oplata_activated();
		void action_debuger_activated();
		void action_calc_tepl_navantaj_activated();
		void action_narah_opal_activated();
		void action_narah_gar_voda_activated();
		void action_pererah_vhidnogo_saldo_potochnoi_ugody_activated();
		void action_pererah_vhidnyh_saldo_usih_ugod_activated();
		
		void action_print_narah_gar_voda_activated();
		void action_print_vidomist_narahuvannya_i_postuplennya_dohodiv_activated();
		void action_print_nakopychuvalna_vidomist_narahuvannya_activated();
		void action_print_vidomist_oplaty_za_CO_activated();
		void action_calc_saldo_spec_rahunkiv_activated();
		void action_pererahunok_saldo_usih_spec_rahunkiv_activated();
		void action_print_vidomist_realizacii_activated();
		void action_print_rahunok_activated();
        void action_print_act_vykon_robit_activated();
		void action_print_vidomist_borjnykiv_organiz_activated();
		void action_print_podatkova_nakladna_activated();
		
		void action_Options_activated();
        void action_bank_rahunky_activated();

		void action_saveDatabase_activated();
		void action_assistent_activated();
		void action_about_activated();
		
		void buttonclick_add_taryf();
		void buttonclick_del_taryf();
		void buttonclick_add_oplata();
		void buttonclick_del_oplata();
		void buttonclickSavePokazLichOpal();
		void buttonclickSavePokazLichGarVodaOpal();
		void buttonclickSaveMeteorOpal();
		void pokazLichDateChanged();
		void pokazLichGarVodaDateChanged();
		void meteorDateChanged();
		void dniOpalDateChanged();
		void dniOpalOpenEditForm(const QModelIndex & index);
		void oplataDateChanged();
		void pokaz_lich_opal_widget_item_changed(QTableWidgetItem * item);
		void meteor_widget_item_changed(QTableWidgetItem * item);
		void loadGUISettingsFromIni();
		void look_ugoda_info( const QModelIndex & index, const QModelIndex & index_ignored );
		void activ_column_changes( const QModelIndex & current, const QModelIndex & previous );
		void changeCellContents(int row, int col, double val);
		void showOperaciiSpecRahMorInfo();
		
		void curDateChanged();
		void calculatorButton_clicked();
		
		void saveMode();
		void workMode(int mode);
	
	signals:
		void requestDebugWindow();
		void closeLoginSignal();
		void mainWindowClosed();
		void requestSaveDataBaseWindow();
	
    private:
		Ui::tke_MainWindow_organizacii ui;
		QDateEdit *dateEdit_PotochnaData;
		QPushButton *calculatorButton;
		QHash<int, QObject *> *controlObjectsHash;
		
        //QAssistantClient *assistant;
		
		Browser* browser;
		bool IRC;
		QSqlQuery * orgSelectQuery;
		QSqlQuery * specRahSelectQuery;
		QSqlRecord xRecord;
		QSqlTableModel * insertSqlModel;
		QSqlTableModel * modelInsertPidrozdily;
		QSqlTableModel * modelOplata;
		QSqlQueryModel * modelPidrozdily;
		QSqlRelationalTableModel * model_taryf;
		QSqlQueryModel * dniOpalQueryModel;
		
		QList<int> finansList;  //Ліст фінансування організації
		QList<int> pryznachList; //Ліст призначення організації
		QList<int> bankyList;
		QList<int> slujbRahunkyTypeList;
		
		QPalette *palette, *default_palette;
		
		QTextCodec *codec;
		int record_edited; //0-record not edited, 1 - record edited
		int record_slujbRah_edited; //0-record not edited, 1 - record edited
		int new_record; // 1 - new record< 0-not;
		int new_slujbRah_record; // 1 - new record< 0-not;
		unsigned char days_in_month[12];
		bool daysOpalCh, opalLichCh, opalGVLichCh;
		
		QDate date;
		QDate currentSysDate;
		
		QVector<int> abonentsList;
		QVector<int> slujbRahList;
		
		tkeDialogEditPidrozdil *dialogEditPidrozdil;
		tkeFormDaysOpal *formDaysOpal;
		
		UPrintDocsOrganiz *printDocsOrganiz;
		
		UCalcDefValuesThread *defValuesThread;
		
		QString modelQueryStr; //Стрічка запиту для таблиці підрозділи форми угод
		
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
};

#endif
