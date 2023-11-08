#include "stm32f10x.h"                  // Device header


#define MPU6050_ADDRESS		0xD0

int16_t AccX,AccY;

void MPU6050_WaitEvent( uint32_t I2C_EVENT)//硬件i2c需要检查标志位看动作是否完成
{
	uint32_t Times;
	Times = 10000;//循环一次就减一次，到0推出
	while (I2C_CheckEvent(I2C2, I2C_EVENT) != SUCCESS)
	{
		Times --;
		if (Times == 0)
		{
			break;
		}
	}
}


void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)//向寄存器地址写8位数，MPU6050寄存器都是8位的
{
	I2C_GenerateSTART(I2C2, ENABLE);
	MPU6050_WaitEvent( I2C_EVENT_MASTER_MODE_SELECT);
	
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Transmitter);//I2C发送模式
	MPU6050_WaitEvent( I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	
	I2C_SendData(I2C2, RegAddress);
	MPU6050_WaitEvent( I2C_EVENT_MASTER_BYTE_TRANSMITTING);
	
	I2C_SendData(I2C2, Data);
	MPU6050_WaitEvent( I2C_EVENT_MASTER_BYTE_TRANSMITTED);
	
	I2C_GenerateSTOP(I2C2, ENABLE);
}


void MPU6050_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	I2C_InitTypeDef I2C_InitStruct;
	I2C_InitStruct.I2C_Ack=I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress=I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_ClockSpeed=200000;//200khz,要小于400khz
	I2C_InitStruct.I2C_DutyCycle=I2C_DutyCycle_16_9;
	I2C_InitStruct.I2C_Mode=I2C_Mode_I2C;
	I2C_InitStruct.I2C_OwnAddress1=0x00;

	I2C_Init(I2C2, &I2C_InitStruct);
	
	
	MPU6050_WriteReg(0x6b, 0x00);//PWR_MGMT_1寄存器--0x6b
	MPU6050_WriteReg(0x6c, 0x00);//PWR_MGMT_2--0x6c
	MPU6050_WriteReg(0x19, 0x09);//SMPLRT_DIV采样分频--0x19，09表示10分频
	MPU6050_WriteReg(0x1a, 0x06);//CONFIG--0x1a，0000 0110，后三位表示低通滤波
	MPU6050_WriteReg(0x1c, 0x00);//ACCEL_CONFIG--0x1c，没有自测，没有高通滤波，+-2g
	
}




uint8_t MPU6050_ReadReg(uint8_t RegAddress)//向寄存器读数据
{
	uint8_t Data;
	
	I2C_GenerateSTART(I2C2, ENABLE);
	MPU6050_WaitEvent( I2C_EVENT_MASTER_MODE_SELECT);
	
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Transmitter);//这里还是写的时序，所以还是发送模式
	MPU6050_WaitEvent( I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	
	I2C_SendData(I2C2, RegAddress);
	MPU6050_WaitEvent( I2C_EVENT_MASTER_BYTE_TRANSMITTED);
	
	I2C_GenerateSTART(I2C2, ENABLE);									//重新开始
	MPU6050_WaitEvent( I2C_EVENT_MASTER_MODE_SELECT);
	
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Receiver);//这里是读的模式
	MPU6050_WaitEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);
	
	I2C_AcknowledgeConfig(I2C2, DISABLE);//不给应答，在还没接收数据时就要给dis应答，因为i2c很快
	I2C_GenerateSTOP(I2C2, ENABLE);
	
	MPU6050_WaitEvent( I2C_EVENT_MASTER_BYTE_RECEIVED);
	Data = I2C_ReceiveData(I2C2);
	
	I2C_AcknowledgeConfig(I2C2, ENABLE);
	
	return Data;
}

void MPU6050_GetData(void)
						
{
	uint8_t XDataH, XDataL,YDataH,YDataL;
	
	XDataH = MPU6050_ReadReg(0x3b);
	XDataL = MPU6050_ReadReg(0x3c);
	AccX = (XDataH << 8) | XDataL;
	
	YDataH = MPU6050_ReadReg(0x3d);
	YDataL = MPU6050_ReadReg(0x3e);
	AccY = (YDataH << 8) | YDataL;
}