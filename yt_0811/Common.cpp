#include "stdafx.h"
#include "Common.h"
int CalTime(){
	//һ���µ�������������3000000����ʡID��Դ
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	return (sys.wDay - 1) * 24 * 3600 + sys.wHour * 3600 
		+ sys.wMinute * 60 + sys.wSecond;
}