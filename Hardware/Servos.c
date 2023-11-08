#include "stm32f10x.h"                  // Device header

void Servos_PWM_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0 | GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	TIM_InternalClockConfig(TIM2);//TIM2是通用定时器，挂在APB1（36MHz）上，
	//选择内部时钟,但看时钟树链接到定时器的还是经过了*2所以还是72MHz
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;//1分频
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;//计数模式选择，向上计数
	TIM_TimeBaseInitStruct.TIM_Period=2000-1;//设定ARR自动重装值。也是定时周期参数between 0x0000 and 0xFFFF
	TIM_TimeBaseInitStruct.TIM_Prescaler=720-1;//时基单元中的PSC，between 0x0000 and 0xFFFF
												//36MHz(APB2)  ->  *2  ->  TIM2  ->  /PSC  ->  /ARR  ->PWM周期
												//72MHz/720/2000 = 50hz = 20ms
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;//0代表正常，1代表2个周期才进入中断

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
	
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;//极性为高代表极性不反转，正常模式
	TIM_OCInitStructure.TIM_Pulse=150;//ARR=2000，CCR=150，此时占空比对舵机SG90是角度初始的意思
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);

	TIM_OC3Init(TIM2, &TIM_OCInitStructure);//结构体的配置都一样，但是另外赋能给通道三，用PA2,控制另一个舵机

	TIM_Cmd(TIM2, ENABLE);

}

void Servos_x_angle(uint16_t ang)//ang
{
	TIM_SetCompare3(TIM2,ang);//改变TIM_Pulse的值，50 <= Pulse <= 250 ,1通道
	
}

void Servos_y_angle(uint16_t ang)
{
	TIM_SetCompare1(TIM2,ang);//改变TIM_Pulse的值，50 <= Pulse <= 250 ,3通道
}



