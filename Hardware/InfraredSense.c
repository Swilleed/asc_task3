#include "stm32f10x.h"
#include "InfraredSense.h"
uint8_t InfraredSenseFlag = 0;

void InfraredSense_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_1 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

static uint8_t InfraredSense_Read(void)
{
    uint8_t status = 0;

    // 左接b13
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 1) {
        status |= INFRARED_LEFT;
    }
    else {
        status &= ~INFRARED_LEFT;
    }
    // 中左接b1
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 1) {
        status |= INFRARED_MIDDLELEFT;
    }
    else {
        status &= ~INFRARED_MIDDLELEFT;
    }
    // 中右接b11
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 1) {
        status |= INFRARED_MIDDLERIGHT;
    }
    else {
        status &= ~INFRARED_MIDDLERIGHT;
    }
    // 右接b10
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 1) {
        status |= INFRARED_RIGHT;
    }
    else {
        status &= ~INFRARED_RIGHT;
    }

    return status;
}

static uint8_t Digital_filter(void)
{
    uint8_t raw = InfraredSense_Read();
    static uint8_t history[4] = {0};
    static uint8_t index = 0;
    history[index] = raw;
    index = (index + 1) % 4;

    uint8_t filtered = 0x0F;
    for (uint8_t i = 0; i < 4; i++) {
        filtered &= history[i];
    }
    return filtered;
}

void InfraredSensor_Tick(void)
{
    InfraredSenseFlag = Digital_filter();
}

uint8_t GetInfraredSenseFlag(void)
{
    return InfraredSenseFlag;
}
