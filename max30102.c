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
}

do {
    if(MAX30102_ReadRegister(hi2c,
    MAX30102_REG_MODE_CONFIG,
    &data)!=MAX30102_OK) {
        return MAX30102_ERROR;
    }
}   while(data & 0x40); //bitwise check

data = 0x40; //enable FIFO almost full interrupt (bit 6)
if (MAX30102_WriteRegister(hi2c,
    MAX30102_REG_INTR_ENABLE_1,
    data) != MAX30102_OK) {
    return MAX30102_ERROR;
}


/* 18-bit ADC resolution (SPO2_ADC_RGE = 3) */
/* 50 samples per second (SPO2_SR = 2) */
/* 411 μs LED pulse width (LED_PW = 3)  */
/* Combined as 0x63 for the SPO2_CONFIG register */

data = 0x63; //01100011
if (MAX30102_WriteRegister(hi2c,
MAX30102_REG_SPO2_CONFIG,
data) != MAX30102_OK) {
    return MAX30102_ERROR;
}