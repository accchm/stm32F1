#include "Menu.h"

void Fun_1(void)                    //显示ad采集的原始值
{
    OLED_ShowString(0,0,"PA0:",12);
	OLED_ShowString(0,12,"PA1:",12);
	OLED_ShowString(0,24,"PA2:",12);
	OLED_ShowString(0,36,"PA3:",12);
	OLED_ShowString(64,0,"PA4:",12);
	OLED_ShowString(64,12,"PA5:",12);
	OLED_ShowString(64,24,"PA6:",12);
	if(adc_complete_flag)
	{
		OLED_ShowNum(32,0,AD_Data[0],4,12);
		OLED_ShowNum(32,12,AD_Data[1],4,12);
		OLED_ShowNum(32,24,AD_Data[2],4,12);
		OLED_ShowNum(32,36,AD_Data[3],4,12);
		OLED_ShowNum(96,0,AD_Data[4],4,12);
		OLED_ShowNum(96,12,AD_Data[5],4,12);
		OLED_ShowNum(96,24,AD_Data[6],4,12);
		adc_complete_flag = 0;
	}
	OLED_Refresh_Gram();
}
void Fun_2(void)
{
	
}
void Fun_3(void)
{
}
void Fun_4(void)
{
}
void Fun_5(void)
{
}

void Fun_6(void)
{
}

void Fun_7(void)
{
}
void Fun_8(void)
{
}
void Fun_9(void)
{
}

void Fun_10(void)
{
}
