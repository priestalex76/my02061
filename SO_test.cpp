#include "my02061.h"

#pragma pack(1)

typedef  struct {
DWORD  param;	
DWORD  timer;	
UCHAR  error;	
} INPUTPARAM, *pINPUTPARAM;

#define CHANNEL_PUSK	SI_pusk( siChanNumber, siMode, siParity, siFreq);\
						tick = GetTickCount();\
						while (( GetTickCount()- tick)<100) {};\
						SI_clear_array(siChanNumber);\
						SO_pusk(soErr_en, soParity, soFreq, soArrayDim, soDelay, soArrayNumber);

#define myDEBUG		1	

#define INPUTCHANNELNUMBER		2	// ����� �������� ������

extern	HANDLE	hECE0206;
extern	DWORD nOutput;
extern	DWORD tick;
extern	BOOL result;

extern ULONG	outBufULONG[2];


void BUF256x32_write(ULONG * paramArray);
void SI_pusk(UCHAR ChanNumber, UCHAR Mode, UCHAR Parity, UCHAR Freq);
void SO_pusk(UCHAR Err_en
				, UCHAR Parity
				, UCHAR Freq
				, UCHAR ArrayDim
				, UCHAR Delay
				, UCHAR ArrayNumber);
void SI_clear_array(UCHAR ChanNumber);
void SI_stop(UCHAR ChanNumber);
void SO_stop(void);
void read_array_CC(UCHAR ChanNumber,INPUTPARAM * bufOutput );
BOOL test_period(UCHAR siChanNumber
				, unsigned int soArrayDim_int
				, DWORD minPeriodTemplate
				, DWORD maxPeriodTemplate);
void read_parameter_CC(UCHAR ChanNumber
				, UCHAR ParamNumber
				,INPUTPARAM * bufOutput );
void frequency_printf(UCHAR siFreq,UCHAR soFreq);
BOOL	inputParamCodeCheck(UCHAR siChanNumber
				,unsigned int soArrayDim_int
				, ULONG * outputParam);



///////////////////////////////////////////////////////////////////////
//
//  ����  ��������� ������
//
//////////////////////////////////////////////////////////////////////
BOOL outputChannelTest()
{
	DWORD	minPeriodTemplate;
	DWORD	maxPeriodTemplate;

	DWORD tick1;
	int array_number;
	
	unsigned int	ii;

	// ��������� �������� ������
	UCHAR  siChanNumber = INPUTCHANNELNUMBER;	//����� ������ (1..4) 
	UCHAR  siMode;	// = 0  - ������� �����; = 1 - ����� ������������
	UCHAR  siParity;	//= 0  - ��� �������� ����������; = 1 - �������� ����������
	UCHAR  siFreq;	//=0 - ����� �� �������� 36-100���; = 1 - ����� �� �������� 11-14,5 ���
	// ��������� ��������� ������
	UCHAR  soErr_en;	// = 0  - ������ 32-������ ����; =1 ������ 33-������ ����;  = 255 - ������ 31-������ ����
	UCHAR  soParity;	//= 0  - ��� ������������ ����������; = 1 - ������������ ���� ����������
	UCHAR  soFreq;	//������ �� �������� 
						//	= 0    12,5���;
						//	 = 1    50���;
						//	 = 2    100 ���;
	UCHAR  soArrayDim;	//������ ������� (1..255���� ��� 0 ��� ������� �������� 256 ����);
	UCHAR  soDelay;	//��� ������������ � ����������� ������: �������� ����� ��������� = 0, 1, 2, .., 255 (��� �������������  0  10,24  20,48  ..  2611,2��);  ��� ����������� ������ = 0
	UCHAR  soArrayNumber;	// ��� ����������� ������ = 1;                          ��� ������������ ������ - ����������                                    ���������� �������� (2..255);                          ��� ����������� ������ = 0. 

	ULONG   param [256];	//������ �������� ����������
	
	DeviceIoControl (
		hECE0206
		,ECE02061_XP_SET_LONG_MODE
		,NULL
		,0
		,NULL
		,0
		,&nOutput 
		,NULL  );


	//	���������� ������ ��������� ������

	for( ii = 0; ii<256; ii++)
	{
		param[ii] = 0x70000000 + (ii<<16) + (((~ii)<<8)&0xff00) + ii;
	}

	BUF256x32_write( param);

	siParity = 1;
	soParity = 1;
	soErr_en = 0;

//  ����������� �����

	printf(" SINGLE OUTPUT:\n");
	soArrayNumber = 1;
	soDelay = 0;

	//	����� ������������/� ��������� 

	for( siMode = 1; siMode <=1; siMode--)
	{
		printf("   %s\n",siMode ? "Self-checking mode" : "Operating mode (with stub)");

		// �������� �� ���� ���������� 

		for( soFreq = 0; soFreq <=2; soFreq++)
		{
			siFreq = soFreq ? 0 : 1;

			frequency_printf(siFreq, soFreq);

			if (soFreq)
			{
				minPeriodTemplate = (720/soFreq)-10;
				maxPeriodTemplate = (720/soFreq)+10;
			}
			else
			{
				minPeriodTemplate = 2800;
				maxPeriodTemplate = 2960;
			}

			// �������� ��� ���� �������� �������

			UCHAR	ArrayDimVariants[4] = {1,128,255,0};	

			for(UCHAR ArrayDimVariantsInd = 0; ArrayDimVariantsInd <4; ArrayDimVariantsInd++)
			{
				soArrayDim = ArrayDimVariants[ArrayDimVariantsInd];

				unsigned int soArrayDim_int = soArrayDim ? soArrayDim : 256;
				
				printf("          SO Array Dimension = %3d\n",soArrayDim_int);
			



				SI_pusk( siChanNumber, siMode, siParity, siFreq);
				tick = GetTickCount();
				while (( GetTickCount()- tick)<100) {};
				SI_clear_array(siChanNumber);
				SO_pusk(soErr_en, soParity, soFreq, soArrayDim, soDelay, soArrayNumber);


				tick = GetTickCount();
				while (( GetTickCount()- tick)<3000) {};// �������� �������


				SI_stop(siChanNumber);
				if(inputParamCodeCheck( siChanNumber
										, soArrayDim_int
										, param))
				{

			//		SI_stop(siChanNumber);

					return 1;
				}

				if(test_period( siChanNumber
								,soArrayDim_int
								,minPeriodTemplate
								,maxPeriodTemplate))
				{

		//			SI_stop(siChanNumber);

					return 1;
				}

		//		SI_stop(siChanNumber);

			}//soArrayDim
		}//soFreq 
	}// siMode = 1/0




//  ������������ �����

	UCHAR	ArrayNumberVariants[2] = {2,5};
	for(UCHAR ArrayNumberVariantsInd = 0; ArrayNumberVariantsInd <2; ArrayNumberVariantsInd++)
	{
		soArrayNumber = ArrayNumberVariants[ArrayNumberVariantsInd];
		printf(" MULTIPLE OUTPUT:  SO Array Number = %3d\n",soArrayNumber);


		//	����� ������������/� ��������� 

		for( siMode = 1; siMode <=1; siMode--)
		{
			printf("   %s\n",siMode ? "Self-checking mode" : "Operating mode (with stub)");

			// �������� �� ���� ����������

			for( soFreq = 0; soFreq <=2; soFreq++)
			{
				siFreq = soFreq ? 0 : 1;

				frequency_printf(siFreq, soFreq);

				// �������� ��� ������� ������ 1 �����

				soArrayDim = 1;
				soDelay = 0;
				printf("          SO Array Dimension = 1\n");

					//	���� �������



				CHANNEL_PUSK;


					tick = GetTickCount();
					tick1 = tick;
					while ((tick1 - tick)<1000) 
					{
						tick1 = GetTickCount();
						printf("%d \r",(tick1 - tick));
					};// �������� �������

					INPUTPARAM inputParam[256];
					read_array_CC(siChanNumber, inputParam );

					// ������� ���������� �������� ��������

					array_number = 0;

					for( ii = 0; ii<256; ii++)
						if(param[0] == (inputParam[ii].param&0x7fffffff)) array_number++;

					if(array_number != soArrayNumber)
					{
							printf("          ERROR: input array number = %d\n",array_number);
#ifdef myDEBUG						// ����� ������� 
							for(int f=0;f<256;f++)
							{
									printf("paramN:%3d  inputParam: %08x timer:%08x error:%02x\n"
										,f
										,inputParam[f].param
										,inputParam[f].timer
										,inputParam[f].error);
							}
#endif


							SI_stop(siChanNumber);

							return 1;

					}  

					SI_stop(siChanNumber);

				// �������� ��� ������� ������ 256 ����

					soArrayDim = 0;
					printf("          SO Array Dimension = 256\n");
					soDelay = 0;

					array_number = 0; 
					INPUTPARAM  bufOutput57;

					//	���� �������


					CHANNEL_PUSK;



					// ������� ���������� �������� ��������

					tick = GetTickCount();
					do							// ������� ���������� ������ ������� �������
					{
						read_parameter_CC(siChanNumber
							,255
							,&bufOutput57);
					}while((bufOutput57.param==0)&&(( GetTickCount()- tick)<1000));

					if(bufOutput57.param==0)
					{
						printf("          ERROR: first input array timeout \n");
						SI_stop(siChanNumber);
						return 1;
					}
					DWORD  timerTemp0 = bufOutput57.timer;
					array_number++; 
					
					tick = GetTickCount();
					do
					{
						read_parameter_CC(siChanNumber
							,255
							,&bufOutput57);
						if(bufOutput57.timer !=timerTemp0)
						{
							array_number++; 
							timerTemp0 = bufOutput57.timer;
						}

						tick1 = GetTickCount();
						printf("%d \r",(tick1 - tick));
					}while ((tick1 - tick)<(1000*(unsigned long int)soArrayNumber));// �������� �������

					if(array_number!=soArrayNumber)
					{
						printf("          ERROR: array_number = %d \n",array_number);
						SI_stop(siChanNumber);


						return 1;
					}

					SI_stop(siChanNumber);

			}//soFreq 
		}// siMode = 1/0
	}



	return 0;
}

