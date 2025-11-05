#include "stm32f10x.h" // Device header
#include "Motor.h"
#include "PWM.h"
#include "Encoder.h"
#include "pid.h"
#include "OLED.h"

extern volatile int32_t TargetSpeed;
extern volatile int32_t CurrentSpeed1;
extern volatile int32_t CurrentSpeed2;
extern volatile int64_t EncoderCount1;
extern volatile int64_t EncoderCount2;
extern PID_TypeDef Motor1_PID;
extern PID_TypeDef Motor2_PID;

#define MOTOR_PWM_MAX 99

// 电机初始化
void Motor_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOB, GPIO_Pin_12);
    GPIO_SetBits(GPIOB, GPIO_Pin_13);

    PWM_Init();
}

// 对电机1设定驱动力
void Motor_SetPWM(int32_t pwm)
{
    int32_t magnitude = pwm;

    if (magnitude >= 0) {
        GPIO_ResetBits(GPIOB, GPIO_Pin_12);
        GPIO_SetBits(GPIOB, GPIO_Pin_13);
    }
    else {
        GPIO_SetBits(GPIOB, GPIO_Pin_12);
        GPIO_ResetBits(GPIOB, GPIO_Pin_13);
        magnitude = -magnitude;
    }

    if (magnitude > MOTOR_PWM_MAX) {
        magnitude = MOTOR_PWM_MAX;
    }

    PWM_SetCompare3((uint16_t)magnitude);
}

// 更新电机速度控制
void Motor_UpdateSpeed(void)
{
    float output = PID_Calculate(&Motor1_PID, (float)TargetSpeed, (float)CurrentSpeed1);
    Motor1_PID.Output = output;
    Motor_SetPWM((int32_t)Motor1_PID.Output);
}

// 电机位置跟随控制
void Motor_Follow_Position(void)
{
    float target = (float)EncoderCount2;
    float actual = (float)EncoderCount1;
    float error = target - actual;

    // 限制误差范围
    if (error > 500.0f) {
        error = 500.0f;
    }
    else if (error < -500.0f) {
        error = -500.0f;
    }

    target = actual + error;

    float output = PID_Calculate(&Motor2_PID, target, actual);
    Motor2_PID.Output = output;

    // 死区处理
    if (output > -1.5f && output < 1.5f) {
        output = 0.0f;
    }

    Motor_SetPWM((int32_t)output);
}
