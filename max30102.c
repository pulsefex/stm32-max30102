//Leroy,MameMore,Connor,Nicole,Justin
#include <max30102.h>
#include <stdio.h>

MAX30102_Status_t MAX30102_Init(I2C_HandleTypeDef *hi2c) {
    __uint8_t data;

    //set bit 6 to 1; this software resets the sensor
    data = 0x40; //0100000

    if(MAX30102_WriteRegister(hi2c,
    MAX30102_REG_MODE_CONFIG,
     data) != MAX30102_OK)  {
        return MAX30102_ERROR;
    }