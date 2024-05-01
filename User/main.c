#include "Main_System.h"

/*
PB8,PB9  ���1��TIM4CH3��CH4��
PB10��PB11  ���2��TIM2Ch3��CH4����Ҫ��ӳ�䣩
PA12   ������
PA1~PA7  ��вɼ���TIM4��
PB1  �����TIM3CH4��
PB0  SLEEP    ������������ֹͣ�� 0ֹͣ��1����
PA9TX��PA10  RX��USART1��
PB6 SCL PB7 SDA ��OLED��
PB12~15  ����
PB4  ���ɹ�(��Ҫ��ӳ��)
*/

int main(void)
{	
	OLED_Init();        //SCL:PB6��SDA:PB7
	PWM_Init();            //TIM3 PB1  ���
	AD_Init();         //TIM4   PA0~6    
	Motor_Init();
//	Timer1_Init();
	Serial_Init(115200);
	while(1)
	{
		Key_Function();
		AD_Get();
	}
}
void CAR_STARTInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = START;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(START_GPIO,&GPIO_InitStructure);
}
void CAR_Start(uint8_t CAR_START_STATE)
{
	if(!GPIO_ReadInputDataBit(START_GPIO,START))
	{
		Motor(CAR_START_STATE);
	}
}
void USART1_IRQHandler()        //�����ı����ݰ�
{
	static uint16_t pRxPacket = 0;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		USART_RX_DATA = USART_ReceiveData(USART1);
		if(USART_RXStage == USART_RXStage_Start && USART_RX_Flag == 0)
		{
			if(USART_RX_DATA == '$')
			{
				USART_RXStage = USART_RXStage_Get;
				pRxPacket = 0;
			}
		}
		else if(USART_RXStage == USART_RXStage_Get)
		{
			if(USART_RX_DATA == '\r')
			{
				USART_RXStage = USART_RXStage_Stop;
			}
			else
			{
				USART_RX_BufStr[pRxPacket] = USART_RX_DATA;
				pRxPacket++;
			}
		}
		else if(USART_RXStage == USART_RXStage_Stop)
		{
			USART_RXStage = USART_RXStage_Start;
			if(USART_RX_DATA == '\n')          //������յ�������β
			{
				USART_RX_BufStr[pRxPacket] = '\0';
				USART_RX_Flag = 1;
			}
		}
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);			//���USART1��RXNE��־λ
	}
}
void DMA1_Channel1_IRQHandler(void)                                 //DMAת���������жϣ���������
{	
	if (DMA_GetFlagStatus(DMA1_FLAG_TC1))
	{
		DMA_ClearITPendingBit(DMA1_FLAG_TC1);
		adc_complete_flag = 1;                     //DMAת����ɱ�־
	}
}
void AD_Get(void)
{
	static uint16_t i = 0;
	int16_t j, temp;
	if(adc_complete_flag)
	{
		AD_Data[0] =(uint16_t)(99*(AD_Data[0]-AD_DATA_Range[0].min)/(AD_DATA_Range[0].max-AD_DATA_Range[0].min)+1);
		AD_Data[1] =(uint16_t)(99*(AD_Data[1]-AD_DATA_Range[1].min)/(AD_DATA_Range[1].max-AD_DATA_Range[1].min)+1);
		AD_Data[2] =(uint16_t)(99*(AD_Data[2]-AD_DATA_Range[2].min)/(AD_DATA_Range[2].max-AD_DATA_Range[2].min)+1);
		AD_Data[3] =(uint16_t)(99*(AD_Data[3]-AD_DATA_Range[3].min)/(AD_DATA_Range[3].max-AD_DATA_Range[3].min)+1);
		AD_Data[4] =(uint16_t)(99*(AD_Data[4]-AD_DATA_Range[4].min)/(AD_DATA_Range[4].max-AD_DATA_Range[4].min)+1);
		AD_Data[5] =(uint16_t)(99*(AD_Data[5]-AD_DATA_Range[5].min)/(AD_DATA_Range[5].max-AD_DATA_Range[5].min)+1);
		AD_Data[6] =(uint16_t)(99*(AD_Data[6]-AD_DATA_Range[6].min)/(AD_DATA_Range[6].max-AD_DATA_Range[6].min)+1);               //��һ��

		if(AD_Data[0] + AD_Data[1] + AD_Data[2] + AD_Data[3] + AD_Data[4] + AD_Data[5] + AD_Data[6] <= MOTOR_Stop_Min)
		{
			Motor(0);                             //Խ��ֹͣ
		}
		Err[i] = (float)((AD_Data[0] - AD_Data[6]) + (AD_Data[2] - AD_Data[4])) / ((AD_Data[0] + AD_Data[6]) + abs((AD_Data[2] - AD_Data[4])));
		i++;
		OLED_ShowNum(32,48,Err[i],4,12);
		adc_complete_flag = 0;
	}
	if(i >= 5)                      //�˲�
	{
		for(j=0;j<4;j++)  														 //ð������
		{
			for(i=0;i<4-j;i++)  
			{
				if(Err[i]>Err[i+1])  			
				{
					temp = Err[i]; 
					Err[i] = Err[i+1]; 
					Err[i+1] = temp; 
				} 
			}
		}
		for(i=1;i<4;i++)
		{
			sum_value += Err[i];
		}
		sum_value = sum_value/3;
		sum_value = car_kp * sum_value + car_kd * (sum_value - Err_Last);
		TIM_SetCompare1(TIM3,110 + sum_value);                             //���ƶ��
		Err_Last = sum_value;
		if(sum_value < 0)
		{
			OLED_ShowChar(90,36,'-',12,1);
			OLED_ShowNum(96,36,-1 * sum_value,4,12);
		}
		else
		{
			OLED_ShowChar(90,36,' ',12,1);
			OLED_ShowNum(96,36,sum_value,4,12);
		}
		OLED_Refresh_Gram();
		i = 0;
	}
}
void Motor(uint16_t Motor_State)
{
	GPIO_WriteBit(SLEEP_GPIO,SLEEP,Motor_State?Bit_SET:Bit_RESET);
}
void Motor_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);           // TIM2ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);                //����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);                //����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_PartialRemap2_TIM2, ENABLE);


	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = SLEEP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SLEEP_GPIO,&GPIO_InitStructure);
	GPIO_WriteBit(SLEEP_GPIO,SLEEP,Bit_RESET);

	//���һ
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                                  //�����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;                           //PB1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	TIM_InternalClockConfig(TIM4);          //ʹ���ڲ�ʱ��     
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;        //��Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;         //��ʱģʽ ���ϼ�ʱ
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;                                //�Զ���װֵ   ÿ��һ����Ϊ10k��֮һ��    ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;                      //��Ƶ7200��Ƶ�ʾ�Ϊ10k      PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;                //�߼���ʱ��ר��
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM4,TIM_FLAG_Update);                                  //�����־λ
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);                              //��ʼ���ṹ��
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                     //ģʽΪPWM2,�����෴
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;                    //��ƽ����ת
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;            //ʹ��
	TIM_OCInitStructure.TIM_Pulse = MOTOR_PWMDuty;                                      //CCR��ֵ
	TIM_OC3Init(TIM4,&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;                    //��ƽ����ת
	TIM_OC4Init(TIM4,&TIM_OCInitStructure);
	
	TIM_Cmd(TIM4,ENABLE);                      //��ʱ��ʹ��

	//���2

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                                  //�����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;                           //PB1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	TIM_InternalClockConfig(TIM2);          //ʹ���ڲ�ʱ��     
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;        //��Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;         //��ʱģʽ ���ϼ�ʱ
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;                                //�Զ���װֵ   ÿ��һ����Ϊ10k��֮һ��    ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;                      //��Ƶ7200��Ƶ�ʾ�Ϊ10k      PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;                //�߼���ʱ��ר��
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);                                  //�����־λ
	
	TIM_OCStructInit(&TIM_OCInitStructure);                              //��ʼ���ṹ��
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                     //ģʽΪPWM2,�����෴
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;                    //��ƽ����ת
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;            //ʹ��
	TIM_OCInitStructure.TIM_Pulse = MOTOR_PWMDuty;                                      //CCR��ֵ
	TIM_OC3Init(TIM2,&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;                    //��ƽ����ת
	TIM_OC4Init(TIM2,&TIM_OCInitStructure);
	
	TIM_Cmd(TIM2,ENABLE);                      //��ʱ��ʹ��
	
}
void Key_Function(void)
{
	static uint8_t motor_flag = 0, Keynum = 0;

	Keynum = Keynum_Get();
	switch (Keynum)
	{
		case 1:
		{
			motor_flag = !motor_flag;
			Motor(motor_flag);
			Keynum = 0;
			break;
		}
		case 2:
		{

			break;
		}
		case 3:
		{
			
			break;
		}
		case 4:
		{
			
			break;
		}
	}
}
void BUZZER_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = BUZZER;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BUZZER_GPIO,&GPIO_InitStructure);
}
void BUZZER_State(uint8_t BUZZER_state)
{
	GPIO_WriteBit(BUZZER_GPIO,BUZZER,BUZZER_state?Bit_SET:Bit_RESET);
}
void USART_DATA_Dispose(void)
{
	if(strncmp(USART_RX_BufStr,USART_Change[0],2))          //kp
	{

	}
	else if (strncmp(USART_RX_BufStr,USART_Change[1],2))    //kd
	{

	}
	else if (strncmp(USART_RX_BufStr,USART_Change[2],2))    //cb
	{

	}
	else if (strncmp(USART_RX_BufStr,USART_Change[3],2))    //ca
	{

	}
	else if (strncmp(USART_RX_BufStr,USART_Change[4],2))    //GO
	{
		Motor(1);
	}
	else if (strncmp(USART_RX_BufStr,USART_Change[5],2))    //STOP
	{
		Motor(0);
	}
}

