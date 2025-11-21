#include "stm32f10x.h"
#include "InfraredSense.h"
uint8_t InfraredSenseFlag = 0;

void InfraredSense_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3; // Assuming PA0 and PA1 are used for
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // Input Pull-Down
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

static uint8_t InfraredSense_Read(void)
{
    uint8_t status = 0;

    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == 1) {
        status |= INFRARED_LEFT;
    }
    else {
        status &= ~INFRARED_LEFT;
    }
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 1) {
        status |= INFRARED_MIDDLELEFT;
    }
    else {
        status &= ~INFRARED_MIDDLELEFT;
    }
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 1) {
        status |= INFRARED_MIDDLERIGHT;
    }
    else {
        status &= ~INFRARED_MIDDLERIGHT;
    }
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1) {
        status |= INFRARED_RIGHT;
    }
    else {
        status &= ~INFRARED_RIGHT;
    }

    return status;
}

void Digital_filter(void)
{
    uint8_t raw = InfraredSense_Read();
    static uint8_t history[4] = {0};
    static uint8_t index = 0;
    history[index] = raw;
    index = (index + 1) % 4;

    uint8_t filtered = 0x0F; // Assume all sensors are off
    for (uint8_t i = 0; i < 4; i++) {
        filtered &= history[i];
    }
    InfraredSenseFlag = filtered;
}

void InfraredSensor_Tick(void)
{
    Digital_filter();
}

uint8_t GetInfraredSenseFlag(void)
{
    return InfraredSenseFlag;
}
