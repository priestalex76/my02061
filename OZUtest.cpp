#include "my02061.h"


extern	HANDLE	hECE0206;
extern	DWORD nOutput;
extern	DWORD tick;
extern	BOOL result;

#pragma pack(1)



void BUF256x32_write(ULONG * paramArray);


///////////////////////////////////////////////////////////////////////
//
//  ���� ������ ��������� ������
//
//////////////////////////////////////////////////////////////////////

BOOL tstOZU()
{
	ULONG   param [256];	//������ �������� ����������

//	���������� ������ ������  0xaaaaaaaa/0x55555555

	int i = 256;
	do{
		i--;
		param[i] = 0x55555555;
		i--;
		param[i] = 0xaaaaaaaa;
	}while(i);

	BUF256x32_write( param);//������ � �������� �����



//	������ ���������� �� ������ � ���������
//		� ��������

	struct {
		UCHAR ParamNumber;	//����� ��������� � �������
		UCHAR Comm;	// = 0x40
		} bufInput47;
	ULONG     param47;	//��� ��������� ���������

	for(i=0; i<256; i++)
	{
		bufInput47.ParamNumber = i;
		bufInput47.Comm = 0x40;
		param47 = 0;

		unsigned int templ = ( i&1)? 0x55555555 : 0xaaaaaaaa;


		do{
			result = DeviceIoControl (
				hECE0206
				,ECE02061_XP_READ_PARAM_1 //������ ��������� ������
				,&bufInput47
				,2                                  
				,&param47 
				,4 
				,&nOutput 
				,NULL  );

		}while(nOutput==0);

		if(param47 != templ)
		{
			printf("ECE02061_XP_READ_PARAM_1 error: result=%x  i=%03d nOutput=%d 0x%08x (0x%08x)\n"
									,result
									, i
									, nOutput
									, param47
									,templ);
			return 1;
		}
	}


//	���������� ������ ������ 0x55555555/0xaaaaaaaa

	i = 256;
	do{
		i--;
		param[i] = 0xaaaaaaaa;
		i--;
		param[i] = 0x55555555;
	}while(i);

	BUF256x32_write( param);//������ � �������� �����



//	������ ���������� �� ������ � ���������
//		� ��������

	for(i=0; i<256; i++)
	{
		bufInput47.ParamNumber = i;
		bufInput47.Comm = 0x40;
		param47 = 0;

		unsigned int templ = ( i&1)? 0xaaaaaaaa : 0x55555555;

		do{
			result = DeviceIoControl (
				hECE0206
				,ECE02061_XP_READ_PARAM_1//������ ��������� ������
				,&bufInput47
				,2                                  
				,&param47 
				,4 
				,&nOutput 
				,NULL  );

		}while(nOutput==0);

		if(param47 != templ)
		{
			printf("ECE02061_XP_READ_PARAM_1 error: result=%x  i=%03d nOutput=%d 0x%08x (0x%08x)\n"
									,result
									, i
									, nOutput
									, param47
									,templ);
			return 1;
		}
	}


	return 0;
}
