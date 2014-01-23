#include "calendar.h"
//最后交易日计算
WORD ifFinalDay(WORD t_year,WORD t_month){
	WORD t_monthDays = 0;
	WORD t_weekDay = 0;
	t_monthDays = MonthDays( t_year,t_month);
	int totalFriday = 0;//统计一共多少个星期五，第三个星期五是最后交易日
	for(WORD i = 1;i <= t_monthDays;i++){
		t_weekDay = WeekDay(t_year,t_month,i);
		if(t_weekDay ==  5){
			totalFriday++;
		}
		if(totalFriday >= 3){
			//大于3的最近一个交易日,因为如遇节假日是顺延的
			if(isTradeDay(t_year,t_month,i)){
				return i;
			};
		}
	}
	return t_monthDays;//没有找到最后交易日,将最后交易日认为t_monthDays号,这样当月最后倒数第二天切换合约
}
WORD A50FinalDay(WORD t_year,WORD t_month){
	WORD t_monthDays = 0;
	t_monthDays = MonthDays( t_year,t_month);
	int total  = 0;//倒数开始的交易天数统计
	for(WORD i = t_monthDays;i >= 1;i--){
		if(isTradeDay(t_year,t_month,i)){
			total++;
		};
		if(total == 2){
			return i;//最后一个交易日
		}
	}
	return t_monthDays;//没有找到最后交易日,将最后交易日认为t_monthDays号,这样当月最后倒数第二天切换合约
}
bool isTradeDay(WORD t_year,WORD t_month,WORD t_day){
	WORD t_weekday = 0;
	t_weekday = WeekDay(t_year,t_month,t_day);
	//交易日历	
	if(t_year ==  2013){
		if(t_weekday == 6 || t_weekday == 0){
			return false;
		}
		else if(t_month == 9 && t_day >= 19 && t_day <= 21){
			return false;
		}
		else if(t_month == 10 && t_day >= 1 && t_day <= 7)
		{
			return false;
		}
		else{
			//交易日
			return true;
		}
	}
	else{
		//下一年的留待本年后重新更新交易日
		return false;
	}
}
WORD MonthDays(WORD year, WORD month)//根据输入的年号和月份，返回该月的天数    
{
	switch(month)
	{
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:  return 31;
	case 4:
	case 6:
	case 9:
	case 11:  return 30;
	case 2:   if(year%4==0 && year%100!=0 || year%400==0)
				  return 29; 
			  else
				  return 28;
	default: return 0;  
	}
}
WORD WeekDay(WORD year,WORD month,WORD day)
{
	long sum;
	WORD i;
	//计算天数
	sum=((year-1)*365+(year-1)/4+(year-1)/400-(year-1)/100);
	for(i=1;i<month;i++)
	{
		sum+=MonthDays(year,i);
	}
	sum+=day;
	return (WORD)(sum%7);
}