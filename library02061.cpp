#include "my02061.h"

#pragma pack(1)

typedef  struct {
DWORD  param;	//��� ���������
DWORD  timer;	// ��������� ������� ��� ����������� ���������
UCHAR  error;	//������ ������ ���������
} INPUTPARAM;

#define myDEBUG		1	// ����� �������

HANDLE	hECE0206;
DWORD nOutput;
DWORD tick;
BOOL result;

#pragma pack(1)


UCHAR  bufOutput51;     // ����������, � ������� ������� ���������� ���
						//������ :   1 - ��������� ����� ������
						//                   0 - ��� ������

//----------------------------------------------------------------------------------------
//
//		���������� ������ BUF256x32 ( 4 ������� ������ 64 �����)
//			paramArray - ������ ������ 256 ����
//

void BUF256x32_write(ULONG * paramArray)
{

struct {
	UCHAR   paramAddress;	//����� ������ ������ BUF256x32
	UCHAR   Comm;	// = 0x80
	ULONG   param [64];	//������ �������� ����������, ����������� (ArrayDim) �� ����� 127
	} outputParam;

	DWORD nbufInputSize41 = sizeof (outputParam);	// ����� �������� ������( � ������).

	outputParam.Comm = 0x80;

	for(int ch =0; ch<4; ch++)
	{
		outputParam.paramAddress =ch*64;
		for(int i = 0; i<64; i++)
			outputParam.param [i] = *(paramArray + ch*64 + i);

		DeviceIoControl (
				hECE0206
				,ECE02061_XP_WRITE_ARRAY_1
				,&outputParam
				,nbufInputSize41
				,NULL
				,0
				,&nOutput
				,NULL );

		tick = GetTickCount();
		while (( GetTickCount()- tick)<300) {};// �������� �������
	}





}

//----------------------------------------------------------------------------------------
//
//		���� �������� ������
//

void SI_pusk(UCHAR ChanNumber, UCHAR Mode, UCHAR Parity, UCHAR Freq)
{
 struct {
		UCHAR  ChanNumber;	//����� ������ (1..4)
		UCHAR  Mode;	// = 0  - ������� �����; = 1 - ����� ������������
		UCHAR  Parity;	//= 0  - ��� �������� ����������; = 1 - �������� ����������
		UCHAR  Freq;	//=0 - ����� �� �������� 36-100���; = 1 - ����� �� �������� 11-14,5 ���
		} bufInput51;

	DWORD	tick1 = GetTickCount();
	while (( GetTickCount()- tick1)<300) {};

	bufInput51.ChanNumber = ChanNumber;
	bufInput51.Mode = Mode;
	bufInput51.Parity = Parity;
	bufInput51.Freq = Freq;

	DeviceIoControl (
		hECE0206
		,ECE02061_XP_SI_PUSK
		,&bufInput51
		,4
		,&bufOutput51
		,1
		,&nOutput
		,NULL  );
}
//----------------------------------------------------------------------------------------
//
//		���� ��������� ������
//

void SO_pusk(UCHAR Err_en, UCHAR Parity, UCHAR Freq, UCHAR ArrayDim, UCHAR Delay, UCHAR ArrayNumber)
{
 struct {
		UCHAR  Err_en;	// = 0  - ������ 32-������ ����; =1 ������ 33-������ ����;  = 255 - ������ 31-������ ����
		UCHAR  Parity;	//= 0  - ��� ������������ ����������; = 1 - ������������ ���� ����������
		UCHAR  Freq;	//������ �� ��������
						//	= 0    12,5���;
						//	 = 1    50���;
						//	 = 2    100 ���;
		UCHAR  ArrayDim;	//������ ������� (1..255���� ��� 0 ��� ������� �������� 256 ����);
		UCHAR  Delay;	//��� ������������ � ����������� ������: �������� ����� ��������� = 0, 1, 2, .., 255 (��� �������������  0  10,24  20,48  ..  2611,2��);  ��� ����������� ������ = 0
		UCHAR  ArrayNumber;	// ��� ����������� ������ = 1;                          ��� ������������ ������ - ����������                                    ���������� �������� (2..255);                          ��� ����������� ������ = 0.
		} bufInput43;


	DWORD	tick1 = GetTickCount();
	while (( GetTickCount()- tick1)<300) {};

	bufInput43.Err_en = Err_en;
	bufInput43.Parity = Parity;
	bufInput43.Freq = Freq;
	bufInput43.ArrayDim = ArrayDim;
	bufInput43.Delay = Delay;
	bufInput43.ArrayNumber = ArrayNumber;

	DeviceIoControl (
		hECE0206
		,ECE02061_XP_SO_PUSK1
		,&bufInput43
		,6
		,NULL
		,0
		,&nOutput
		,NULL  );
}

//----------------------------------------------------------------------------------------
//
//		��������� ������� ������.
//

void SI_clear_array(UCHAR ChanNumber)
{

UCHAR  bufOutput;


	DeviceIoControl (
		hECE0206
		,ECE02061_XP_CLEAR_SI_ARRAY
		,&ChanNumber
		,1
		,&bufOutput
		,1
		,&nOutput
		,NULL  );

}


//----------------------------------------------------------------------------------------
//
//		������� ������.
//

void SI_stop(UCHAR ChanNumber)
{

UCHAR  bufOutput;

	DeviceIoControl (
		hECE0206
		,ECE02061_XP_SI_STOP
		,&ChanNumber
		,1
		,&bufOutput
		,1
		,&nOutput
		,NULL  );

}
//----------------------------------------------------------------------------------------
//
//		������� ������.
//

void SO_stop(void)
{
	DeviceIoControl (
		hECE0206
		,ECE02061_XP_SO_STOP1
		,NULL
		,0
		,NULL
		,0
		,&nOutput
		,NULL  );

}

//----------------------------------------------------------------------------------------
//
//		������ ������ ������ �� �������� ����.
//

void read_array_CC(UCHAR ChanNumber,INPUTPARAM * bufOutput )
{
	ULONG kop;

	switch(ChanNumber)
	{
	case 1:
		kop = ECE02061_XP_READ_ARRAY_CC1;
		break;
	case 2:
		kop = ECE02061_XP_READ_ARRAY_CC2;
		break;
	case 3:
		kop = ECE02061_XP_READ_ARRAY_CC3;
		break;
	case 4:
		kop = ECE02061_XP_READ_ARRAY_CC4;
		break;
	}


	DeviceIoControl (
		hECE0206
		,kop
		,NULL
		,0
		,bufOutput
		,2304
		,&nOutput
		,NULL  );

}


//----------------------------------------------------------------------------------------
//
//		������ �������� ��������� �� �������� ����.
//

void read_parameter_CC(UCHAR ChanNumber, UCHAR ParamNumber,INPUTPARAM * bufOutput )
{
	ULONG kop;

	switch(ChanNumber)
	{
	case 1:
		kop = ECE02061_XP_READ_PARAM_CC1;
		break;
	case 2:
		kop = ECE02061_XP_READ_PARAM_CC2;
		break;
	case 3:
		kop = ECE02061_XP_READ_PARAM_CC3;
		break;
	case 4:
		kop = ECE02061_XP_READ_PARAM_CC4;
		break;
	}


	DeviceIoControl (
		hECE0206
		,kop
		,&ParamNumber
		,1
		,bufOutput
		,9
		,&nOutput
		,NULL  );

}




//----------------------------------------------------------------------------------------
//
// ������ ������� �������
//

void frequency_printf(UCHAR siFreq,UCHAR soFreq)
{

				if(siFreq)
					printf("      frequency: si = Slow");
				else
					printf("      frequency: si = Fast");


				if(soFreq==0)
					printf(" so = 12,5KHz\n");
				else
				{
					if(soFreq==1)
						printf(" so = 50KHz\n");
					else
						printf(" so = 100KHz\n");
				}

}
//------------------------------------------------------------------------------------------------
//
// ��������� �������� ���������� � �������� ��������
//

BOOL	inputParamCodeCheck(UCHAR siChanNumber,unsigned int soArrayDim_int, ULONG * outputParam)
{

	INPUTPARAM inputParam[256];

	read_array_CC(siChanNumber, inputParam );

	unsigned int paramN = 0;

	for( ; paramN<soArrayDim_int; paramN++)
		if((outputParam[paramN] != (inputParam[paramN].param&0x7fffffff))||(inputParam[paramN].error!=0x0f ))
		{
			printf("paramN: %3d  outputParam: %08x  inputParam: %08x  error: %02x\n"
				,paramN
				,outputParam[paramN]
				,inputParam[paramN].param
				,inputParam[paramN].error);

#ifdef myDEBUG
			// ����� �������

			struct {
				UCHAR ParamNumber;	//����� ��������� � �������
				UCHAR Comm;	// = 0x40
				} bufInput47;

			ULONG     param47;	//��� ��������� ���������
			printf("\n");
			for(unsigned int f=0;f<soArrayDim_int;f++)
			{
				bufInput47.ParamNumber = f;
				bufInput47.Comm = 0x40;
				param47 = 0;
				do
				{
					result = DeviceIoControl (
							hECE0206
							,ECE02061_XP_READ_PARAM_1
							,&bufInput47
							,2
							,&param47
							,4
							,&nOutput
							,NULL  );
				}while(nOutput==0);

				printf("paramN:%3d outputParam:%08x inputParam:%08x timer:%08x error:%02x\n"
										,f
										,param47
										,inputParam[f].param
										,inputParam[f].timer
										,inputParam[f].error);
			}
#endif
				return 1;
		}
				return 0;
}


//----------------------------------------------------------------------------------------
//
// �������� ������� ����������� ������� ����������
//

BOOL test_period(UCHAR siChanNumber
				, unsigned int soArrayDim_int
				, DWORD minPeriodTemplate
				, DWORD maxPeriodTemplate)
{
	INPUTPARAM inputParam[256];

	read_array_CC(siChanNumber, inputParam );

	DWORD  timerTemp0 = inputParam[0].timer;

	DWORD temp0 = (timerTemp0>>8)*1024+(timerTemp0&0xff)*4;

				for(unsigned int ii = 1; ii<soArrayDim_int; ii++)
				{
					DWORD  timerTemp1 = inputParam[ii].timer;
					DWORD temp1 = (timerTemp1>>8)*1024+(timerTemp1&0xff)*4;
					DWORD deltatemp = (temp1-temp0);

					if(( deltatemp < minPeriodTemplate)||( deltatemp > maxPeriodTemplate))
					{
						printf("          ERROR:paramN:%3d   input parameter period = %d PeriodTemplate = %d - %d\n"
							,ii
							,deltatemp
							,minPeriodTemplate
							,maxPeriodTemplate);

#ifdef myDEBUG						// ����� �������
							for(unsigned int f=0;f<soArrayDim_int;f++)
							{
									printf("paramN:%3d  inputParam: %08x timer:%08x error:%02x\n"
										,f
										,inputParam[f].param
										,inputParam[f].timer
										,inputParam[f].error);
							}
#endif
								return 1;
					}

					temp0 = temp1;
				}


	return 0;
}
