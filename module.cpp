#include "my02061.h"
#include <stdio.h>
#include <dos.h>
#include <windows.h>

#define THREADS_NUMBER 10
#define ITERATIONS_NUMBER 100
#define PAUSE 10 /* ms */

DWORD dwCounter = 0;
#pragma pack(1)
#define myDEBUG		1			// СЂРµР¶РёРј РѕС‚Р»Р°РґРєРё

typedef  struct
{
DWORD  param;					//РєРѕРґ РїР°СЂР°РјРµС‚СЂР°
DWORD  timer;					// РїРѕРєР°Р·Р°РЅРёСЏ С‚Р°Р№РјРµСЂР° РїСЂРё РїРѕСЃС‚СѓРїР»РµРЅРёРё РїР°СЂР°РјРµС‚СЂР°
UCHAR  error;					//РѕС€РёР±РєР° РїСЂРёРµРјР° РїР°СЂР°РјРµС‚СЂР°
} INPUTPARAM, *pINPUTPARAM;
#define myDEBUG		1			// СЂРµР¶РёРј РѕС‚Р»Р°РґРєРё
DWORD WINAPI OutputChanelFunction(CONST LPVOID lpParam);
DWORD WINAPI InputChanelFunction(CONST LPVOID lpParam);
extern	HANDLE	hECE0206;
extern	DWORD nOutput;
extern	DWORD tick;
extern	BOOL result;

void BUF256x32_write(ULONG * paramArray);
void frequency_printf(UCHAR siFreq,UCHAR soFreq);

void SI_pusk(UCHAR ChanNumber, UCHAR Mode, UCHAR Parity, UCHAR Freq);
void SI_stop(UCHAR ChanNumber);
void SI_clear_array(UCHAR ChanNumber);

void SO_pusk(UCHAR Err_en, UCHAR Parity, UCHAR Freq, UCHAR ArrayDim, UCHAR Delay, UCHAR ArrayNumber);
void SO_stop(void);

BOOL inputParamCodeCheck(UCHAR siChanNumber,unsigned int soArrayDim_int, ULONG * outputParam);

/*************************************************************************************************/
//РљР»Р°СЃСЃ РІС…РѕРґРЅРѕРіРѕ РєР°РЅР°Р»Р°
class InputChanel
{
private:
	 /*
	siChanNumber
	РќРѕРјРµСЂ РєР°РЅР°Р»Р° (1..4)
	----------------------------------
	siMode
	0  - СЂР°Р±РѕС‡РёР№ СЂРµР¶РёРј;
	1 - СЂРµР¶РёРј СЃР°РјРѕРєРѕРЅС‚СЂРѕР»СЏ.
	----------------------------------
	siParity
	0  - Р±РµР· РєРѕРЅС‚СЂРѕР»СЏ РЅРµС‡РµС‚РЅРѕСЃС‚Рё;
	1 - РєРѕРЅС‚СЂРѕР»СЊ РЅРµС‡РµС‚РЅРѕСЃС‚Рё.
	----------------------------------
	siFreq
	0 - РїСЂРёРµРј РЅР° С‡Р°СЃС‚РѕС‚Р°С…  36-100РљР“С†;
	1 - РїСЂРёРµРј РЅР° С‡Р°СЃС‚РѕС‚Р°С… 11-14,5 РљР“С†.
	*/
	UCHAR  siChanNumber;
	UCHAR  siMode;
	UCHAR  siParity;
	UCHAR  siFreq;

public:
	 INPUTPARAM  bufOutput[256];
	 InputChanel()
	 {
		//РљРѕРЅСЃС‚СЂСѓРєС‚РѕСЂ РєР»Р°СЃСЃР°
		 siChanNumber=1;
		 siMode=0;
		 siParity=0;
		 siFreq=1;
	 }
	void setInputChanelMode(UCHAR siChanNumberGet, UCHAR siModeGet, UCHAR siParityGet, UCHAR siFreqGet)
	{
		//РР·РјРµРЅРµРЅРёРµ СЂРµР¶РёРјР° СЂР°Р±РѕС‚С‹ РІС…РѕРґРЅРѕРіРѕ РєР°РЅР°Р»Р°
		siChanNumber=siChanNumberGet;
		siMode=siModeGet;
		siParity=siParityGet;
		siFreq=siFreqGet;
	}
	void openInputChanelNumber(int numberChanel)
	{
		//Р—Р°РїСѓСЃРє РІС…РѕРґРЅРѕРіРѕ РєР°РЅР°Р»Р°
		SI_pusk( numberChanel, siMode, siParity, siFreq);
	}
	void startAllChanel()
	{
		for(UCHAR siChanNumber1 =1; siChanNumber1<=4; siChanNumber1++) // РїСѓСЃРє РІС…РѕРґРЅС‹С… РєР°РЅР°Р»РѕРІ
		{
			openInputChanelNumber(siChanNumber1);
			tick = GetTickCount();
			while (( GetTickCount()- tick)<100) {};
			clearArray(siChanNumber1);
		}
	}
	void clearArray(int numberChanel)
	{
		//РћС‡РёСЃС‚РєР° РјР°СЃСЃРёРІР° Р±СѓС„РµСЂР° РїРѕ РЅРѕРјРµСЂСѓ РєР°РЅР°Р»Р°
		SI_clear_array(numberChanel);
	}
	void ReadBuffer()
	{
		//Р§С‚РµРЅРёРµ РІС…РѕРґРЅРѕРіРѕ РєР°РЅР°Р»Р°
		//Р’РѕР·РІСЂР°С‰Р°РµС‚ Р±СѓС„РµСЂ РєР°РЅР°Р»Р° РІ СЃРІРѕР№СЃС‚РІРѕ bufOutput
		DeviceIoControl (hECE0206,ECE02061_XP_READ_ARRAY_AP1,NULL,0,&bufOutput,2304,&nOutput,NULL);
	}
	void StopAllChanel()
	{
		for(int i=1; i<=4; i++)
		{
			SI_stop(i);
		}
	}
	void StopChanel(int NumberOfChanel)
	{
		SI_stop(NumberOfChanel);
	}
};
/*************************************************************************************************/
//РљР»Р°СЃСЃ РІС‹С…РѕРґРЅРѕРіРѕ РєР°РЅР°Р»Р°
class OutputChanel
{
private:
	/*
	soErr_en
	0 - РІС‹РґР°С‡Р° 32-Р±РёС‚РЅС‹С… СЃР»РѕРІ;
	1 - РІС‹РґР°С‡Р° 33-Р±РёС‚РЅС‹С… СЃР»РѕРІ;
	255 - РІС‹РґР°С‡Р° 31-Р±РёС‚РЅС‹С… СЃР»РѕРІ.
	----------------------------------
	soParity
	0  - Р±РµР· С„РѕСЂРјРёСЂРѕРІР°РЅРёСЏ РЅРµС‡РµС‚РЅРѕСЃС‚Рё;
	1 - С„РѕСЂРјРёСЂРѕРІР°РЅРёРµ Р±РёС‚Р° РЅРµС‡РµС‚РЅРѕСЃС‚Рё
	----------------------------------
	soFreq
	Р’С‹РґР°С‡Р° РЅР° С‡Р°СЃС‚РѕС‚Р°С…:
		0 - 12,5РљР“С†;
		1 - 50РљР“С†;
		2 - 100 РљР“С†;
	----------------------------------
	soArrayDim
	Р Р°Р·РјРµСЂ РјР°СЃСЃРёРІР° (1..255СЃР»РѕРІ РёР»Рё 0 РґР»СЏ РјР°СЃСЃРёРІР° СЂР°Р·РјРµСЂРѕРј 256 СЃР»РѕРІ);
	----------------------------------
	soDelay
	Р”Р»СЏ РјРЅРѕРіРѕРєСЂР°С‚РЅРѕР№ Рё С†РёРєР»РёС‡РµСЃРєРѕР№ РІС‹РґР°С‡Рё: РёРЅС‚РµСЂРІР°Р» РјРµР¶РґСѓ РјР°СЃСЃРёРІР°РјРё = 0, 1, 2, .., 255 (С‡С‚Рѕ СЃРѕРѕС‚РІРµС‚СЃС‚РІСѓРµС‚  0  10,24  20,48  ..  2611,2РјСЃ);
	Р”Р»СЏ РѕРґРЅРѕРєСЂР°С‚РЅРѕР№ РІС‹РґР°С‡Рё = 0
	----------------------------------
	soArrayNumber
	РґР»СЏ РѕРґРЅРѕРєСЂР°С‚РЅРѕР№ РІС‹РґР°С‡Рё = 1;
	РґР»СЏ РјРЅРѕРіРѕРєСЂР°С‚РЅРѕР№ РІС‹РґР°С‡Рё - РєРѕР»РёС‡РµСЃС‚РІРѕ РІС‹РґР°РІР°РµРјС‹С… РјР°СЃСЃРёРІРѕРІ (2..255);
	РґР»СЏ С†РёРєР»РёС‡РµСЃРєРѕР№ РІС‹РґР°С‡Рё = 0.
	*/
	UCHAR  soErr_en;
	UCHAR  soParity;
	UCHAR  soFreq;
	UCHAR  soArrayDim;
	UCHAR  soDelay;
	UCHAR  soArrayNumber;

public:
	INPUTPARAM  bufOutput[256]; //Р‘СѓС„РµСЂ РєР°РЅР°Р»Р°
	OutputChanel()
	{
		//РљРѕРЅСЃС‚СЂСѓРєС‚РѕСЂ РєР»Р°СЃСЃР°
		soErr_en=0;
		soParity=0;
		soFreq=0;
		soArrayDim=0;
		soDelay=0;
		soArrayNumber=0;
	}
	void SetOutputChanelMode(UCHAR  soErr_enGet, UCHAR  soParityGet, UCHAR  soFreqGet, UCHAR  soArrayDimGet,UCHAR soDelayGet, UCHAR soArrayNumberGet)
	{
		//РЈСЃС‚Р°РЅРѕРІРєР° СЂРµР¶РёРјР° СЂР°Р±РѕС‚С‹ РєР°РЅР°Р»Р°
		soErr_en=soErr_enGet;
		soParity=soParityGet;
		soFreq=soFreqGet;
		soArrayDim=soArrayDim;
		soDelay=soDelayGet;
		soArrayNumber=soArrayNumberGet;
	}
	void openInputChanel()
	{
		//РћС‚РєСЂС‹С‚РёРµ РєР°РЅР°Р»Р°
		SO_pusk(soErr_en, soParity, soFreq, soArrayDim, soDelay, soArrayNumber);
	}
	void WriteBuffer(ULONG param[256])
	{
	//РњРµС‚РѕРґ Р·Р°РїРёСЃРё РґР°РЅРЅС‹С… РІ Р±СѓС„РµСЂ РІС‹С…РѕРґРЅРѕРіРѕ РєР°РЅР°Р»Р°
	//Р—Р°РїРёСЃС‹РІР°РµС‚ РґР°РЅРЅС‹Рµ:
	//1. Р’ Р°С‚СЂРёР±СѓС‚ bufOutput
	//2. Р¤РёР·РёС‡РµСЃРєРё РЅР° СѓСЃС‚СЂРѕР№СЃС‚РІРѕ
	//Р’ РєР°С‡РµСЃС‚РІРµ Р°СЂРіСѓРјРµРЅС‚РѕРІ РїСЂРёРЅРёРјР°РµС‚ РјР°СЃСЃРёРІ РґР°РЅРЅС‹С… param[256];
		for (int i=0; i<256; i++)
		{
			bufOutput[i].param=param[i];
		}
		BUF256x32_write(param);
	}
	void ReadBuffer()
	{
	//РњРµС‚РѕРґ С‡С‚РµРЅРёСЏ РґР°РЅРЅС‹С… РІ Р±СѓС„РµСЂРµ РІС‹С…РѕРґРЅРѕРіРѕ РєР°РЅР°Р»Р°
	//Р’РѕР·РІСЂР°С‰Р°РµС‚ РјР°СЃСЃРёРІ РґР°РЅРЅС‹С… РІ Р°С‚СЂРёР±СѓС‚ bufOutput
		struct
		{
		UCHAR ParamNumber;
		UCHAR Comm;
		} bufInput47;
		ULONG param47;
		for(int i=0; i<256; i++)
		{
			bufInput47.ParamNumber = i;
			bufInput47.Comm = 0x40;
			param47 = 0;
			unsigned int templ = ( i&1)? 0x55555555 : 0xaaaaaaaa;
			do
			{
				result = DeviceIoControl (hECE0206,ECE02061_XP_READ_PARAM_1,&bufInput47,2,&param47,4,&nOutput,NULL);
			}
			while(nOutput==0);
			bufOutput[i].param=param47;
		}
	}
	void StopChanel()
	{
		SO_stop();
	}
	double Percent(InputChanel chanel)
	{
	//Р¤СѓРЅРєС†РёСЏ СЂР°СЃС‡РµС‚Р° РїСЂРѕС†РµРЅС‚Р° РїСЂР°РІРёР»СЊРЅРѕ РїРµСЂРµРґР°РЅРЅС‹С… РїР°СЂР°РјРµС‚СЂРѕРІ РјРµР¶РґСѓ РІС‹С…РѕРґРЅС‹Рј Рё РІС…РѕРґРЅС‹Рј Р±СѓС„РµСЂР°РјРё
	//Р’ РєР°С‡РµСЃС‚РІРµ Р°СЂРіСѓРјРµРЅС‚РѕРІ РїСЂРёРЅРёРјР°РµС‚ РґР°РЅРЅС‹Рµ РІС…РѕРґРЅРѕРіРѕ РєР°РЅР°Р»Р° InputChanel
		int rightAnswer=0;
		for(int i=0; i<256; i++)
		{
			if(bufOutput[i].param==chanel.bufOutput[i].param)
			{
				rightAnswer++;
			}
		}
		double RightPercent=((double)rightAnswer/256)*100;
		return RightPercent;
	}
};
/*************************************************************************************************/
//РџРµСЂРµРіСЂСѓР·РєР° РѕРїРµСЂР°С‚РѕСЂР° СЃСЂР°РІРЅРµРЅРёСЏ
bool operator ==(const InputChanel& left, const OutputChanel& right)
{
	//РЎСЂР°РІРЅРёРІР°РµС‚ Р·РЅР°С‡РµРЅРёСЏ Р±СѓС„РµСЂР° РІС…РѕРґРЅРѕРіРѕ Рё РІС‹С…РѕРґРЅРѕРіРѕ РєР°РЅР°Р»Р°
	bool result=false;
	int CountCorrectBuffer=0;
	for (int i=0; i<256; i++)
	{
		if(right.bufOutput[i].param==left.bufOutput[i].param)
		{
			CountCorrectBuffer++;
		}
	}
	if(CountCorrectBuffer==256)
	{
		result=true;
	}
	return result;
}
/*************************************************************************************************/
    InputChanel inputChanel;			//Р’С…РѕРґРЅРѕР№ РєР°РЅР°Р»
	OutputChanel outputChanel;			//Р’С‹С…РѕРґРЅРѕР№ РєР°РЅР°Р»
void iniCH()
{
		outputChanel.SetOutputChanelMode(255,1,1,0,0,0);
	//outputChanel.SetOutputChanelMode(0,1,1,0,0,0);
	inputChanel.setInputChanelMode(1,1,1,0);
	inputChanel.startAllChanel();
}
DWORD WINAPI ThreadProc(CONST LPVOID lpParam) {
  CONST HANDLE hMutex = (CONST HANDLE)lpParam;
  DWORD i;
  for(i = 0; i < ITERATIONS_NUMBER; i++) {
    WaitForSingleObject(hMutex, INFINITE);
    dwCounter++;
    ReleaseMutex(hMutex);
    Sleep(PAUSE);
  }
  ExitThread(0);
}

void StartProcess()
{
	TCHAR szMessage[256];
  DWORD dwTemp, i;
  HANDLE hThreads[THREADS_NUMBER];
  CONST HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
  CONST HANDLE hMutex = CreateMutex(NULL, FALSE, NULL);

   for(i = 0; i < THREADS_NUMBER; i++) {
    hThreads[i] = CreateThread(NULL, 0, &ThreadProc, hMutex, 0, NULL);

  }

  HANDLE InputChanel;
  HANDLE OutputChanel;





	iniCH(); //РРЅРёС†РёР°Р»РёР·Р°С†РёСЏ
	//outCH(); //Р—Р°РїСѓСЃРє РІС‹С…РѕРґРЅРѕРіРѕ РєР°РЅР°Р»Р°
	//inputCH(); //Р—Р°РїСѓСЃРє Рё РїСЂРёРµРј РІС…РѕРґРЅРѕРіРѕ РєР°РЅР°Р»Р°

   InputChanel = CreateThread(NULL, 0, &OutputChanelFunction, hMutex, 0, NULL);
   OutputChanel= CreateThread(NULL, 0, &InputChanelFunction, hMutex, 0, NULL);

   WaitForSingleObject(InputChanel,INFINITE);
   WaitForSingleObject(OutputChanel,INFINITE);


	/*РћСЃС‚Р°РЅРѕРІРєР° Рё Р·Р°РєСЂС‹С‚РёРµ РІСЃРµС… РєР°РЅР°Р»РѕРІ*/
	inputChanel.StopAllChanel();
	outputChanel.StopChanel();
}
DWORD WINAPI OutputChanelFunction(CONST LPVOID lpParam)
{
	printf("OpenOutputChanel Thread\n");
	ULONG	param[256];					//Р›РѕРєР°Р»СЊРЅС‹Р№ Р±СѓС„РµСЂ
	for(int j=1; j<4; j++)
	{
		Sleep(2000);
		for( int i = 0; i<256; i++)			//Р—Р°РїРёСЃС‹РІР°РµРј РёРЅС„РѕСЂРјР°С†РёСЋ РІ Р»РѕРєР°Р»СЊРЅС‹Р№ Р±СѓС„РµСЂ РІС‹С…РѕРґРЅРѕРіРѕ РєР°РЅР°Р»
		{
			param[i] =i*j;
		}
		outputChanel.WriteBuffer(param);
		DeviceIoControl (hECE0206,ECE02061_XP_SET_SHORT_MODE,NULL,0,NULL,0,&nOutput ,NULL);

		outputChanel.openInputChanel();		//РџСѓСЃРє РІС‹С…РѕРґРЅРѕРіРѕ РєР°РЅР°Р»Р°
		Sleep(1000);
	}
	ExitThread(0);
}
DWORD WINAPI InputChanelFunction(CONST LPVOID lpParam)
{
	printf("OpenInputChanel Thread\n");
	FILE *output=fopen("out.txt","w");	//Р¤Р°Р№Р» РІС‹РІРѕРґР° РёРЅС„РѕСЂРјР°С†РёРё
	ULONG	param[256];					//Р›РѕРєР°Р»СЊРЅС‹Р№ Р±СѓС„РµСЂ
	for(int j=1; j<4; j++)
	{
		Sleep(5000);
		for( int i = 0; i<256; i++)			//Р—Р°РїРёСЃС‹РІР°РµРј РёРЅС„РѕСЂРјР°С†РёСЋ РІ Р»РѕРєР°Р»СЊРЅС‹Р№ Р±СѓС„РµСЂ РІС‹С…РѕРґРЅРѕРіРѕ РєР°РЅР°Р»
		{
			param[i] =i*j;
		}


		inputChanel.ReadBuffer();			//Р§С‚РµРЅРёРµ Р±СѓС„РµСЂР° РІС…РѕРґРЅРѕРіРѕ РєР°РЅР°Р»Р°
		Sleep(1000);

		//Р’С‹РІРѕРґ РёРЅС„РѕСЂРјР°С†РёРё РІ С„Р°Р№Р»
		fprintf(output,"РџСЂРѕС†РµРЅС‚ РїСЂР°РІРёР»СЊРЅРѕРіРѕ РїРѕР»СѓС‡РµРЅРёСЏ: %f РёС‚РµСЂР°С†РёРё %d \n\n",outputChanel.Percent(inputChanel),j);
		for(int i=0; i<256; i++)
		{
			fprintf(output,"in=%d, out=%d, time=%d\n",param[i],inputChanel.bufOutput[i].param,inputChanel.bufOutput[i].timer);
		}
		fprintf(output,"\n\n\n");
	}
	fclose(output);
	ExitThread(0);
}
