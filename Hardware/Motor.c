#include "stm32f10x.h" // Device header
#include "Motor.h"
#include "PWM.h"
#include "Encoder.h"
#include "pid.h"
#include "OLED.h"

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
void Motor1_SetPWM(int32_t pwm)
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

void Motor2_SetPWM(int32_t pwm)
{
    int32_t magnitude = pwm;

    if (magnitude >= 0) {
        GPIO_ResetBits(GPIOB, GPIO_Pin_14);
        GPIO_SetBits(GPIOB, GPIO_Pin_15);
    }
    else {
        GPIO_SetBits(GPIOB, GPIO_Pin_14);
        GPIO_ResetBits(GPIOB, GPIO_Pin_15);
        magnitude = -magnitude;
    }

    if (magnitude > MOTOR_PWM_MAX) {
        magnitude = MOTOR_PWM_MAX;
    }

    PWM_SetCompare4((uint16_t)magnitude);
}

void Motor_SetSpeed(int32_t speed1, int32_t speed2)
{
    TargetSpeed1 = speed1;
    TargetSpeed2 = speed2;
}

// 更新电机速度控制
void Motor1_UpdateSpeed(void)
{
    float output = PID_Calculate(&Motor1_PID, (float)TargetSpeed1, (float)CurrentSpeed1);
    Motor1_PID.Output = output;
    Motor1_SetPWM((int32_t)Motor1_PID.Output);
}

void Motor2_UpdateSpeed(void)
{
    float output = PID_Calculate(&Motor2_PID, (float)TargetSpeed2, (float)CurrentSpeed2);
    Motor2_PID.Output = output;
    Motor2_SetPWM((int32_t)Motor2_PID.Output);
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

    Motor1_SetPWM((int32_t)output);
}

int32_t Motor1_GetCurrentSpeed(void)
{
    return CurrentSpeed1;
}
int32_t Motor2_GetCurrentSpeed(void)
{
    return CurrentSpeed2;
}

void Motor1_ReadCurrentSpeed(int32_t speed)
{
    CurrentSpeed1 = speed;
}

void Motor2_ReadCurrentSpeed(int32_t speed)
{
    CurrentSpeed2 = speed;
}