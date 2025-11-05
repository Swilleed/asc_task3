#include "stm32f10x.h"
#include "pid.h"

extern float kp;
extern float ki;
extern float kd;

void PID_Init(PID_TypeDef *pid)
{
    pid->Err = 0.0f;
    pid->Err_Last = 0.0f;
    pid->Err_Prev = 0.0f;
    pid->Integral = 0.0f;
    pid->Output = 0.0f;
    pid->OutputMax = 99.0f; // 与 PWM 占空比范围一致
    pid->OutputMin = -99.0f;
    pid->Kp = kp; // 使用外部定义的全局变量
    pid->Ki = ki;
    pid->Kd = kd;
}

// PID计算函数
// 参数: target: 目标值
// 参数: actual: 实际值
// 输出: output
float PID_Calculate(PID_TypeDef *pid, float target, float actual)
{
    pid->TargetValue = target;
    pid->ActualValue = actual;

    // 计算当前误差
    pid->Err = pid->TargetValue - pid->ActualValue;

    // 积分项计算
    pid->Integral += pid->Err;

    // 微分项计算
    // 累加
    float derivative = pid->Err - pid->Err_Last;

    // PID输出计算
    pid->Output = (pid->Kp * pid->Err) + (pid->Ki * pid->Integral) + (pid->Kd * derivative);

    // 输出限幅
    if (pid->Output > pid->OutputMax)
        pid->Output = pid->OutputMax;
    else if (pid->Output < pid->OutputMin)
        pid->Output = pid->OutputMin;

    // 更新误差历史
    pid->Err_Prev = pid->Err_Last;
    pid->Err_Last = pid->Err;

    return pid->Output;
}
