#ifndef __MOTOR_H
#define __MOTOR_H

#include <stdint.h>
#include "pid.h"

extern volatile int32_t TargetSpeed;
extern volatile int32_t CurrentSpeed1;
extern volatile int32_t CurrentSpeed2;
extern volatile int64_t EncoderCount1;
extern volatile int64_t EncoderCount2;
extern PID_TypeDef Motor1_PID;
extern PID_TypeDef Motor2_PID;

void Motor_Init(void);
void Motor_SetPWM(int32_t pwm);
void Motor_UpdateSpeed(void);
void Motor_Follow_Position(void);

#endif
