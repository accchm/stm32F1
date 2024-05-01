#ifndef __MAIN_SYSTEM_H_
#define __MAIN_SYSTEM_H_
#include "stm32f10x.h"                  // Device header
#include "stm32f10x.h"                  // Device header
#include "Key.h"
#include "TIM.h"
#include "PWM.h"
#include "oled.h"
#include "IC.h"
#include "usart.h"
#include "AD.h"
#include <math.h>
#include "Menu.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Delay.h"

/**
 * @brief HardwareDefineGroup
 * 
 */
#define Control_GPIO GPIOB
#define Control GPIO_Pin_1
#define BUZZER_GPIO GPIOA
#define BUZZER GPIO_Pin_12
#define SLEEP_GPIO GPIOB
#define SLEEP GPIO_Pin_0
#define START_GPIO GPIOB
#define START GPIO_Pin_4
#define KEY_GPIO GPIOB
#define KEY1 GPIO_Pin_12
#define KEY2 GPIO_Pin_13
#define KEY3 GPIO_Pin_14
#define KEY4 GPIO_Pin_15
#define MOTOR_PWMDuty 60
#define MOTOR_Stop_Min 200
/**
 * @brief DriverDefineGroup
 * 小车参数
 */
#define M_Left_max 4096
#define M_Left_min 0
float car_kp = 1;
float car_kd = 2;
float ERR_A = 1;
float ERR_B = 1;
typedef struct key_table         //菜单索引结构体
{
	uint8_t current;                   //当前位置索引
	uint8_t up;
	uint8_t down;
	uint8_t enter;                    //进入位置索引
	void (*current_fun)();           //当前索引运行的函数
}key_table;
key_table Table[20]=           //菜单索引表
{
    
	{},
	{1,5,2,6,(*Fun_1)},  
	{2,1,3,7,(*Fun_2)},  
	{3,2,4,8,(*Fun_3)},  
	{4,3,5,9,(*Fun_4)},  
	{5,4,1,10,(*Fun_5)}, 

	{6,6,6,1,(*Fun_6)},  
	{7,7,7,1,(*Fun_7)},  
	{8,8,8,1,(*Fun_8)},  
	{9,9,9,1,(*Fun_9)},  
	{10,10,10,1,(*Fun_10)}
};    

struct AD_DATA_Range{
	uint16_t min;
	uint16_t max;
}AD_DATA_Range[7] = {{0,4096},{0,4096},{0,4096},{0,4096},{0,4096},{0,4096},{0,4096}};
float Err[20], Err_Last;
int32_t sum_value = 0;
const char USART_Change[][5] ={"kp","kd","CB","CA","GO","NG"}; 


void Motor_Init(void);
void Motor(uint16_t Motor_State);
void AD_Get(void);
void CAR_Start(uint8_t CAR_START_STATE);
void CAR_STARTInit(void);
void Key_Function(void);
void Motor_Init(void);





#endif

