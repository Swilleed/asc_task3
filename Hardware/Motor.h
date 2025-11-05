#ifndef __MOTOR_H
#define __MOTOR_H

#include <stdint.h>
#include "pid.h"

void Motor_Init(void);
void Motor_SetPWM(int32_t pwm);
void Motor_UpdateSpeed(void);
void Motor_Follow_Position(void);

#endif
