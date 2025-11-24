#include "stm32f10x.h"
#include "Motor.h"
#include "Tracking.h"
#include "InfraredSense.h"
#include "pid.h"

PID_TypeDef PID_Tracking;

void Tracking_Init(void)
{
    PID_Init(&PID_Tracking, 15.0f, 0.0f, 5.0f);
}

// 通过pid控制电机左右速度比
/**
 * 电机控制函数
 * @param status 红外传感器状态
 */
static void Motor_Control(uint8_t status)
{
    const int8_t STABLE_ERROR = 0;
    int8_t error = 0;
    if (status == OFF_TRACK) {
        Motor_SetSpeed(-5, -5); // 后退
        return;
    }

    switch (status) {
    case DEVIATE_TO_LEFT:
        error = -1;
        break;
    case 0x0E: // 补充左偏情况
        error = -2;
        break;
    case DEVIATE_TO_RIGHT:
        error = 1;
        break;
    case 0x07: // 补充右偏情况
        error = 2;
        break;
    case ON_TRACK:
        error = STABLE_ERROR;
        break;
    case CROSSED_LINE:
        error = 0; // 保持原有速度
        break;
    default:
        break;
    }

    float correction = PID_Calculate(&PID_Tracking, 0.0f, (float)error);
    int32_t baseSpeed = 20;                           // 基础速度
    int32_t speed1 = baseSpeed + (int32_t)correction; // 左轮速度
    int32_t speed2 = baseSpeed - (int32_t)correction; // 右轮速度
    Motor_SetSpeed(speed1, speed2);
}

void Tracking_Control(void)
{
    Motor_Control(GetInfraredSenseFlag());
}