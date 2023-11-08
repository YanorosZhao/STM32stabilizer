#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Servos.h"
#include "MPU6050.h"

extern int16_t AccX,AccY;
int16_t AX,AY;

int main(void)
{
	OLED_Init();
	Servos_PWM_Init();
	MPU6050_Init();

	OLED_ShowChar(1, 1, 'X');
	OLED_ShowChar(2, 1, 'Y');

	
	while (1)
	{
		AX = AccX>>2;
		AY = AccY>>2;
		int16_t xDU=((AX-6425)/-40.5);
		int16_t yDU=((AY-6000)/-40);
		MPU6050_GetData();
		OLED_ShowSignedNum(1, 2, AX, 7);
		OLED_ShowSignedNum(2, 2, AY, 7);
		OLED_ShowNum(3,1,xDU,8);
		OLED_ShowNum(4,1,yDU,8);
		Servos_x_angle((uint16_t)xDU);//A2是X轴，沿面包板长边中线，通道三
		Servos_y_angle((uint16_t)yDU);//A0是Y轴，沿面包板短边中线，通道一
		
	}
}
