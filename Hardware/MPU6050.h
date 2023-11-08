#ifndef __MPU6050_H
#define __MPU6050_H

void MPU6050_WaitEvent( uint32_t I2C_EVENT);
void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data);
void MPU6050_Init(void);
uint8_t MPU6050_ReadReg(uint8_t RegAddress);
void MPU6050_GetData(void);


#endif
