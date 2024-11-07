//Leroy,MameMore,Connor,Nicole,Justin
#include <max30102.h>
#include <stdio.h>

MAX30102_Status_t MAX30102_Init(I2C_HandleTypeDef *hi2c)
{
    __uint8_t data;

    //set bit 6 to 1; this software resets the sensor
    data = 0x40; //0100000

    if (MAX30102_WriteRegister(hi2c,
                               MAX30102_REG_MODE_CONFIG,
                               data) != MAX30102_OK)
    {
        return MAX30102_ERROR;
    }

    do
    {
        if (MAX30102_ReadRegister(hi2c,
                                  MAX30102_REG_MODE_CONFIG,
                                  &data) != MAX30102_OK)
        {
            return MAX30102_ERROR;
        }
    } while (data & 0x40); // bitwise check

    data = 0x40; // enable FIFO almost full interrupt (bit 6)
    if (MAX30102_WriteRegister(hi2c,
                               MAX30102_REG_INTR_ENABLE_1,
                               data) != MAX30102_OK)
    {
        return MAX30102_ERROR;
    }

    /* 18-bit ADC resolution (SPO2_ADC_RGE = 3) */
    /* 50 samples per second (SPO2_SR = 2) */
    /* 411 μs LED pulse width (LED_PW = 3)  */
    /* Combined as 0x63 for the SPO2_CONFIG register */

    data = 0x63; //01100011
    if (MAX30102_WriteRegister(hi2c,
                               MAX30102_REG_SPO2_CONFIG,
                               data) != MAX30102_OK)
    {
        return MAX30102_ERROR;
    }

    // initial LED pulse amplitudes
    data = 0x24;
    if (MAX30102_WriteRegister(hi2c,
                               MAX30102_REG_LED1_PA,
                               data) != MAX30102_OK)
    {
        return MAX30102_ERROR;
    }
    if (MAX30102_WriteRegister(hi2c,
                               MAX30102_REG_LED2_PA,
                               data) != MAX30102_OK)
    {
        return MAX30102_ERROR;
    }

    return MAX30102_OK;
}

MAX30102_Status_t MAX30102_WriteRegister(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t value)
{
    //write a single byte to the specified register
    if (HAL_I2C_Mem_Write(hi2c,
                          MAX30102_I2C_ADDRESS,
                          reg,
                          I2C_MEMADD_SIZE_8BIT,
                          &value,
                          1,
                          HAL_MAX_DELAY) != HAL_OK)
    {
        return MAX30102_ERROR;
    }

    return MAX30102_OK;
}

MAX30102_Status_t MAX30102_ReadRegister(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t *value)
{
    //read a single byte from the specified register
    if (HAL_I2C_Mem_Read(hi2c,
                         MAX30102_I2C_ADDRESS,
                         reg,
                         I2C_MEMADD_SIZE_8BIT,
                         value,
                         1,
                         HAL_MAX_DELAY) != HAL_OK)
    {
        return MAX30102_ERROR;
    }

    return MAX30102_OK;
}

MAX30102_Status_t MAX30102_ReadFIFO(I2C_HandleTypeDef *hi2c, uint32_t *red_data, uint32_t *ir_data) {
    uint8_t fifo_data[6]; 

    // Read 6 bytes from the FIFO data register
    if (HAL_I2C_Mem_Read(hi2c,
                         MAX30102_I2C_ADDRESS,
                         MAX30102_REG_FIFO_DATA,
                         I2C_MEMADD_SIZE_8BIT,
                         fifo_data,
                         6,
                         HAL_MAX_DELAY) != HAL_OK)
    {
        return MAX30102_ERROR;
    }
    
    // Intensity of red LED reflected back into MAX30102 photodetector
    *red_data = ((uint32_t)fifo_data[0] << 16) | ((uint32_t)fifo_data[1] << 8) | fifo_data[2];
    // Intensity of infrared LED reflected back into MAX30102 photodetector
    *ir_data = ((uint32_t)fifo_data[3] << 16) | ((uint32_t)fifo_data[4] << 8) | fifo_data[5]; 

    return MAX30102_OK;
}

MAX30102_Status_t ON(I2C_HandleTypeDef *hi2c) {
    uint8_t data;

    // Read current value of the SHDN bit
    if (MAX30102_ReadRegister(hi2c, MAX30102_REG_MODE_CONFIG, &data) != MAX30102_OK) {
        return MAX30102_ERROR;
    }

    data &= ~0x80; // Wake up sensor by clearing SHDN bit
    if (MAX30102_WriteRegister(hi2c, MAX30102_REG_MODE_CONFIG, data) != MAX30102_OK) {
        return MAX30102_ERROR;
    }



    return MAX30102_OK;
}

MAX30102_Status_t OFF(I2C_HandleTypeDef *hi2c) {
    uint8_t data;

    // Read current value of the SHDN bit
    if (MAX30102_ReadRegister(hi2c, MAX30102_REG_MODE_CONFIG, &data) != MAX30102_OK) {
        return MAX30102_ERROR;
    }

    data |= 0x80; // Put sensor to sleep using SHDN bit
    if (MAX30102_WriteRegister(hi2c, MAX30102_REG_MODE_CONFIG, data) != MAX30102_OK) {
        return MAX30102_ERROR;
    }



    return MAX30102_OK;
}
