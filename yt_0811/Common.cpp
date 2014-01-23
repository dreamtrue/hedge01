#include "stdafx.h"
#include "Common.h"
int CalTime(){
	//一个月的总秒数不超过3000000，节省ID资源
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	return (sys.wDay - 1) * 24 * 3600 + sys.wHour * 3600 
		+ sys.wMinute * 60 + sys.wSecond;
}