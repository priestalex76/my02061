//#include<iostream>
#include "my02061.h"

HANDLE OpenDeviceByIndex(  DWORD Index, PDWORD pError );

extern	HANDLE	hECE0206;

int main(int argc, char* argv[])
{
	DWORD	Error;
	//Подключение к устройству по его номеру
	hECE0206  = OpenDeviceByIndex(0, &Error);

	if (hECE0206 == INVALID_HANDLE_VALUE) //Проверка
	{
		printf("| ERROR opening device: (%0x) \n",Error);
		//std::cout << "ERROR opening device:"s << Error << std::endl;
		return 0;
	}
	//Вывод указателя на подключенное устройство
	printf("| hECE0206 = 0x%0x\n",hECE0206);
	//std::cout << "hECE0206 = "s << hECE0206 << std::endl;

	CloseHandle ( hECE0206 );

	return 0;
}
