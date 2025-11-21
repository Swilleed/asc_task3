#ifndef __MOTOR_H
#define __MOTOR_H

#include <stdint.h>
#include "pid.h"

extern volatile int32_t TargetSpeed1;
extern volatile int32_t TargetSpeed2;
extern volatile int32_t CurrentSpeed1;
extern volatile int32_t CurrentSpeed2;
extern volatile int64_t EncoderCount1;
extern volatile int64_t EncoderCount2;
extern PID_TypeDef Motor1_PID;
extern PID_TypeDef Motor2_PID;

void Motor_Init(void);
void Motor1_SetPWM(int32_t pwm);
void Motor2_SetPWM(int32_t pwm);
void Motor_SetSpeed(int32_t speed1, int32_t speed2);
void Motor1_UpdateSpeed(void);
void Motor2_UpdateSpeed(void);
void Motor_Follow_Position(void);
void Motor1_ReadCurrentSpeed(int32_t speed);
void Motor2_ReadCurrentSpeed(int32_t speed);
int32_t Motor1_GetCurrentSpeed(void);
int32_t Motor2_GetCurrentSpeed(void);

#endif
