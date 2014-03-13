/****************************************************************************
**
** Created: Roman Mulyk 24.09.2006
**
**
****************************************************************************/

#ifndef TKE_MATH_H
#define TKE_MATH_H

#include <QString>
#include <QVariant>
#include <QDate>
#include <QSqlQuery>

#include <UMath>


typedef enum UOrgFinanceType_
{
	FinaceTypeAny = 0,
	FinanceMiskBudget = 1, // ̳����� ������
	FinanceRyonBudget = 2, // �������� ������
	FinanceOblBudjget = 3, // �������� ������
	FinanceStateBudget = 4, // ��������� ������
	FinanceGosprozrah = 5, // ��������������� ���������
	FinanceNasel = 6 // ���������
} UOrgFinanceType;


struct UNarah_data
{
	int				rah;	//�������
	short			bud;	//id �������
	double 			s;		//����� ��������
	double			zs;		//����� �����
	unsigned char	pd;		//����� ��� (������� ������)
	unsigned char	p;		//������� �����
	unsigned char	pd_2;	//����� ��� (������� ������)_2
	unsigned char	p_2;	//������� �����_2
	double			sp;		//����� �� �����
	double			sp_2;		//����� �� �����
	double			pds;	//����� �� ��'�
	double			pd1;	//����� �� ������
	double			ks;		//��������� �� �����
	int				kp;		//������� �� ��������
	double  		t; 		//����� ��� �������
	short			kimn;	//������� �����
	short			ludy;	//������� �����������
	QDate			nDate;	//���� �����������
	double			normaGV;//���������� ������� ������ ����,��� ����������� ���� ����� �� �����
	double			vykorGV;//�������� ������������ ���� ���������
	double			gkalSt; //ʳ������ ��������� �������� �� �������
	double			kvadrSt;//�������� ��� �������� ��������
	double			vartGkalSt;//������� �������� �������� ������
	double			vartKvadrSt;//������� �������� �������� ������
	double			pilgPlos;//ϳ������ �����
	double			pilgObem;//ϳ������� ��'�� ������ ����
	double			tVGV;//����� ���� ��
	double			tOsGV; //����� �� ����� � �����
	bool 			isLichGV; //� �������� �� ��, �� ����� ������� ��������
	double			dodSumaOnKuby; //ʳ������ ��� ��� ��������� ��
	short			kilkistDnivNoLichAbonGV; //ʳ������ ��� ��� ������������� ��������� ��
	short			kilkistDnivNoGVAbon;
	bool			needDateLimits; //������ ������������ �������� �� ��� ��� �����
	QDate			pilgaBeginDate; //���� ������� ������� �����
	QDate			pilgaEndDate; //���� ��������� ������� �����
	QDate			pilgaBeginDate_2; //���� ������� ������� �����
	QDate			pilgaEndDate_2; //���� ��������� ������� �����
	short			begPilgDayCO; //���������� ���� ������� ����� �� ��
	short			endPilgDayCO; //ʳ������ ���� ������� ����� �� ��
	short			begPilgDayGV; //���������� ���� ������� ����� �� ��
	short			endPilgDayGV; //ʳ������ ���� ������� ����� �� ��
	short			begPilgDayCO_2; //���������� ���� ������� ����� �� ��
	short			endPilgDayCO_2; //ʳ������ ���� ������� ����� �� ��
	short			begPilgDayGV_2; //���������� ���� ������� ����� �� ��
	short			endPilgDayGV_2; //ʳ������ ���� ������� ����� �� ��
	short			pochDayPoslugCO; //���������� ���� ������� ������ �� (����� ������������ ��� ����������� �����)
	short			kincDayPoslugCO; //ʳ������ ���� ������� ������ �� (����� ������������ ��� ����������� �����)
	short			pochDayPoslugGV; //���������� ���� ������� ������ �� (����� ������������ ��� ����������� �����)
	short			kincDayPoslugGV; //ʳ������ ���� ������� ������ �� (����� ������������ ��� ����������� �����)
};

struct UNasTaryfInfo
{
	UNasTaryfInfo(){
		vart_g_kal=0; normat_taryf_kvadr=0; vart_kuba_GV=0; normaKubivGVNaOsobu=0; valid=false;
	};
	double vart_g_kal;
	double normat_taryf_kvadr;
	double vart_kuba_GV;
	double vart_na_lud_in_month_GV;
	double normaKubivGVNaOsobu;
	bool valid;
};

struct UOrgTaryfInfo
{
	UOrgTaryfInfo(){
		m_vart_g_kal = 0;
		m_vart_g_kal_gosp_rozrah = 0;
		vart_g_kal_gar_vody = 0;
		vart_kvadr_CO = 0;
		vart_kuba_GV = 0;
		valid=false;
	};
	UOrgTaryfInfo(int year, int month){
		valid = populate(year, month);
	};
	UOrgTaryfInfo(QDate dt){
		valid = populate(dt.year(), dt.month());
	};
	bool populate(int year, int month);
	bool isValid() {
		return valid;
	}
	double vart_g_kal(UOrgFinanceType eFinaceType);
  public:
    double vart_g_kal_gar_vody;
	double vart_kvadr_CO;
	double vart_kuba_GV;
  private:
    double m_vart_g_kal;
	double m_vart_g_kal_gosp_rozrah;
	bool valid;
};

struct UAbonObPlSubs
{
	bool need;
	double suma;
	UAbonObPlSubs():need(false),suma(0){};
	UAbonObPlSubs(double opSum):need(true),suma(opSum){};
};

//Function declatarions
	double	f_narah(UNarah_data *d);
	double	f_pilga(UNarah_data *d);
	double	f_pilga_2(UNarah_data *d);
	double	f_narah_korektura_ploshchi(UNarah_data *d);
	double	s_pilga_korektura_ploshchi(const UNarah_data *d);
	double	f_pilga_korektura_ploshchi(UNarah_data *d);
	double	f_pilga_2_korektura_ploshchi(UNarah_data *d);
	double	f_narah_GV(UNarah_data *d);
	double	f_read_vykor_GV(UNarah_data *d, bool isNarah=false);
	double	f_pilga_GV(UNarah_data *d);
	double	f_pilga_GV_2(UNarah_data *d);
	double	f_narah_St(UNarah_data *d);
	double	vhSaldoInMonth(int rahunok, short year, short month, bool useCustomVhSaldo, double customVhSaldo=0);
	double	vhSaldoInMonth(int rahunok, short year, short month);
	bool inMDateInterval(QDate curDate, QDate pochDate, QDate kincDate);
	
	double normaSpojyvGVNaOsobu(int year, int month);
	UNasTaryfInfo nasTaryf(short year, short month);
	UOrgTaryfInfo orgTaryf(short year, short month);
	double diuchTaryfInMonth(int bud_id, short year, short month);
	UAbonObPlSubs oplataZgidnoNarahSubs(int rah, short year, short month);
#endif
