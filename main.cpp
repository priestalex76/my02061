#include "my02061.h"


HANDLE OpenDeviceByIndex(  DWORD Index, PDWORD pError );
//BOOL tstOZU( void);
//BOOL outputChannelTest( void);
//BOOL inputChannelTest( void);
//void SI_stop(UCHAR ChanNumber);
//void SO_stop(void);
//void StartProcess();

extern	HANDLE	hECE0206;



int main(int argc, char* argv[])
{
	//������� ������
	/*
	��������� ������� � �������� �����
	1. ������ ��������� ������� ������ ������
	2. ��������� ������ ���������� � �������� �����
	3. ��������� ������ � �������� ������
	*/
	DWORD	Error;
	//����������� � ���������� �� ��� ������
	hECE0206  = OpenDeviceByIndex(0, &Error);


	if (hECE0206 == INVALID_HANDLE_VALUE) //��������
	{
		printf("ERROR opening device: (%0x) \n",Error);
		return 0;
	}
	//��������� ����� � ���
	/*				SO_stop(); //�������� �����
					SI_stop(1); //������� ����� 1
					SI_stop(2); //������� ����� 2
					SI_stop(3); //������� ����� 3
					SI_stop(4); //������� ����� 4
	*/
	//����� ��������� �� ������������ ����������
	printf("hECE0206 = 0x%0x\n",hECE0206);

	//���������� ������������
	//printf("\nOZU test\n");
	//���� ���
	/*if( tstOZU()) //������������ ������� ������ � ������ ��������� ������
	{
		printf("OZU test ERROR\n");

		return 0;
	}
	printf("OK\n");*/


/*
	printf("\nOutput Channel (SO) Test \n");
	//���� ��������� ������
	if( outputChannelTest())
	{
		printf("OutputChannelTest ERROR\n");
		return 0;
	}

	printf("\r            \r");
	printf("OK\n");*/
/*
	printf("\nInput Channels (SI) Test \n");
	//���� �������� ������
	if( inputChannelTest())
	{
		printf("InputChannelTest ERROR\n");
		return 0;
	}

	printf("\r            \r");
	printf("OK\n");*/
	//�������� ������ � ����������
	//StartProcess();
	CloseHandle ( hECE0206 );

	return 0;
}
