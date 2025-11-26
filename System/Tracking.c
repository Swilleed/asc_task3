#include "stm32f10x.h"
#include "Motor.h"
#include "Tracking.h"
#include "InfraredSense.h"
#include "pid.h"

PID_TypeDef PID_Tracking;

void Tracking_Init(void)
{
    PID_Init(&PID_Tracking, 10.0f, 0.0f, 3.8f);
}

/**
 * 电机控制函数
 * @param status 红外传感器状态
 */
void Tracking_Control(uint8_t status, int32_t baseSpeed)
{
    const int8_t STABLE_ERROR = 0;
    static float coefficient1;
    static float coefficient2;

    int8_t error = 0;
    if (status == OFF_TRACK) {
        Motor_SetSpeed(-15, -15); // 后退
        return;
    }

    switch (status) {
    case DEVIATE_TO_LEFT:
        error = -1;
        break;
    case 0x0E: // 补充左偏情况
        error = -2;
        break;
    case 0x08: // 补充中左偏情况
        error = -1;
        break;
    case DEVIATE_TO_RIGHT:
        error = 1;
        break;
    case 0x07: // 补充右偏情况
        error = 2;
        break;
    case 0x02: // 补充中右偏情况
        error = 1;
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

    if (error < 0) {
        if (baseSpeed == 30) {
            coefficient1 = 1.8f;
            coefficient2 = 1.8f;
        }
        else if (baseSpeed == 40) {
            coefficient1 = 1.7f;
            coefficient2 = 0.5f;
        }
        else if (baseSpeed == 60) {
            coefficient1 = 1.8f;
            coefficient2 = 1.1f;
        }
    }
    else if (error > 0) {
        if (baseSpeed == 30) {
            coefficient1 = 1.8f;
            coefficient2 = 1.8f;
        }
        else if (baseSpeed == 40) {
            coefficient1 = 0.5f;
            coefficient2 = 1.7f;
        }
        else if (baseSpeed == 60) {
            coefficient1 = 1.1f;
            coefficient2 = 1.8f;
        }
    }

    float correction = PID_Calculate(&PID_Tracking, 0.0f, (float)error);
    int32_t speed1 = baseSpeed + coefficient1 * (int32_t)correction; // 左轮速度
    int32_t speed2 = baseSpeed - coefficient2 * (int32_t)correction; // 右轮速度
    Motor_SetSpeed(speed1, speed2);
}