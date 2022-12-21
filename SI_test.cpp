#include "my02061.h"

#pragma pack(1)

#define myDEBUG		1	// ����� �������

typedef  struct {
DWORD  param;	//��� ���������
DWORD  timer;	// ��������� ������� ��� ����������� ���������
UCHAR  error;	//������ ������ ���������
} INPUTPARAM, *pINPUTPARAM;
#define myDEBUG		1	// ����� �������

extern	HANDLE	hECE0206;
extern	DWORD nOutput;
extern	DWORD tick;
extern	BOOL result;

ULONG	outBufULONG[2];


void BUF256x32_write(ULONG * paramArray);
void frequency_printf(UCHAR siFreq,UCHAR soFreq);
void SI_pusk(UCHAR ChanNumber, UCHAR Mode, UCHAR Parity, UCHAR Freq);
void SO_pusk(UCHAR Err_en, UCHAR Parity, UCHAR Freq, UCHAR ArrayDim, UCHAR Delay, UCHAR ArrayNumber);
void SI_clear_array(UCHAR ChanNumber);
void SI_stop(UCHAR ChanNumber);
BOOL	inputParamCodeCheck(UCHAR siChanNumber,unsigned int soArrayDim_int, ULONG * outputParam);
void SO_stop(void);


///////////////////////////////////////////////////////////////////////
//
//  ����  ������� �������
//
//////////////////////////////////////////////////////////////////////
BOOL inputChannelTest()
{
	
	ULONG	param[256];
	DWORD tick1;

	unsigned int	ii;
  	for( ii = 0; ii<256; ii++)
	{
		param[ii] = 0x70000000 + (ii<<16) + (((~ii)<<8)&0xff00) + ii;
	}

	BUF256x32_write( param);

	// ��������� ��������� ������
	UCHAR  soErr_en = 0;	// = 0  - ������ 32-������ ����; =1 ������ 33-������ ����;  = 255 - ������ 31-������ ����
	UCHAR  soParity = 1;	//= 0  - ��� ������������ ����������; = 1 - ������������ ���� ����������
	UCHAR  soFreq;	//������ �� �������� 
						//	= 0    12,5���;
						//	 = 1    50���;
						//	 = 2    100 ���;
	UCHAR  soArrayDim = 0;	//������ ������� (1..255���� ��� 0 ��� ������� �������� 256 ����);
	UCHAR  soDelay = 0;	//��� ������������ � ����������� ������: �������� ����� ��������� = 0, 1, 2, .., 255 (��� �������������  0  10,24  20,48  ..  2611,2��);  ��� ����������� ������ = 0
	UCHAR  soArrayNumber = 0;	// ��� ����������� ������ = 1;                          ��� ������������ ������ - ����������                                    ���������� �������� (2..255);                          ��� ����������� ������ = 0. 


	// ��������� ������� �������
	UCHAR  siChanNumber;	//����� ������ (1..4) 
	UCHAR  siMode;	// = 0  - ������� �����; = 1 - ����� ������������
	UCHAR  siParity = 1;	//= 0  - ��� �������� ����������; = 1 - �������� ����������
	UCHAR  siFreq;	//=0 - ����� �� �������� 36-100���; = 1 - ����� �� �������� 11-14,5 ���

	
	DeviceIoControl (
		hECE0206
		,ECE02061_XP_SET_SHORT_MODE
		,NULL
		,0
		,NULL
		,0
		,&nOutput 
		,NULL  );

	for( siChanNumber =1; siChanNumber<=4; siChanNumber++)
	{
		printf("Channel # %d\n",siChanNumber);

		//	����� ������������/� ��������� 

		for( siMode = 1; siMode <=1; siMode--)
		{
			printf("   %s\n",siMode ? "Self-checking mode" : "Operating mode (with stub)");

			// �������� �� ���� ����������

			for( soFreq = 0; soFreq <=2; soFreq++)
			{
				siFreq = soFreq ? 0 : 1;

				frequency_printf(siFreq, soFreq);

				// ���� ������� �������
				for(UCHAR siChanNumber1 =1; siChanNumber1<=4; siChanNumber1++)
				{


					SI_pusk( siChanNumber1, siMode, siParity, siFreq);
					tick = GetTickCount();
					while (( GetTickCount()- tick)<100) {};
					SI_clear_array(siChanNumber1);
				}

				// ���� ��������� ������

				SO_pusk(soErr_en, soParity, soFreq, soArrayDim, soDelay, soArrayNumber);

				tick = GetTickCount();
				do
				{
					tick1 = GetTickCount();
					printf("%d \r",(tick1 - tick));
				}while ((tick1 - tick)<5000) ;


				if(inputParamCodeCheck( siChanNumber
						,256
						,param))
				{

					SO_stop();
					SI_stop(1);
					SI_stop(2);
					SI_stop(3);
					SI_stop(4);

					return 1;
				}


				SO_stop();
				SI_stop(1);
				SI_stop(2);
				SI_stop(3);
				SI_stop(4);

			}//soFreq 
		}// siMode = 1/0
	}//for( siChanNumber =1; siChanNumber<=4; siChanNumber++)

	return 0;
}