/*编码器测速*/

#include "stm32f10x.h" // Device header
#include "Motor.h"

void Encoder_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure); // TIM3 CH1/CH2

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(GPIOB, &GPIO_InitStructure); // TIM4 CH1/CH2

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 0xFFFF;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 0;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);

    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 0x0F;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;

    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
    TIM_ICInit(TIM4, &TIM_ICInitStructure);

    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
    TIM_ICInit(TIM4, &TIM_ICInitStructure);

    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_BothEdge, TIM_ICPolarity_BothEdge);
    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_BothEdge, TIM_ICPolarity_BothEdge);

    TIM_SetCounter(TIM3, 0);
    TIM_SetCounter(TIM4, 0);

    TIM_Cmd(TIM3, ENABLE);
    TIM_Cmd(TIM4, ENABLE);
}

// 获取编码器1计数值并清零
// 在tim1中断中调用，1ms一次
int32_t Encoder1_Get(void)
{
    int16_t Temp;
    Temp = TIM_GetCounter(TIM3);
    TIM_SetCounter(TIM3, 0);
    return Temp;
}
int32_t Encoder2_Get(void)
{
    int16_t Temp;
    Temp = TIM_GetCounter(TIM4);
    TIM_SetCounter(TIM4, 0);
    return Temp;
}

#define SPEED_WINDOW_SIZE 10
static int16_t SpeedBuffer1[SPEED_WINDOW_SIZE] = {0};
static int16_t SpeedBuffer2[SPEED_WINDOW_SIZE] = {0};
static uint8_t BufferIdx = 0;

void Encoder_Tick(void)
{
    // 1. 读取 1ms 内的原始脉冲增量
    int16_t delta1 = Encoder1_Get();
    int16_t delta2 = Encoder2_Get();

    // 2. 累计位置 (使用原始增量，避免积分误差)
    EncoderCount1 += delta1;
    EncoderCount2 += delta2;

    // 3. 更新滑动窗口 (FIFO)
    SpeedBuffer1[BufferIdx] = delta1;
    SpeedBuffer2[BufferIdx] = delta2;
    BufferIdx++;
    if (BufferIdx >= SPEED_WINDOW_SIZE) {
        BufferIdx = 0;
    }

    // 4. 计算窗口总和 (等效于最近 10ms 的脉冲数)
    int32_t sum1 = 0;
    int32_t sum2 = 0;
    for (uint8_t i = 0; i < SPEED_WINDOW_SIZE; i++) {
        sum1 += SpeedBuffer1[i];
        sum2 += SpeedBuffer2[i];
    }

    // 5. 更新全局速度变量
    CurrentSpeed1 = sum1;
    CurrentSpeed2 = sum2;
}
