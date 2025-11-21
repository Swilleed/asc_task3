#include "stm32f10x.h" // Device header
#include "Delay.h"
#include "OLED.h"
#include "Motor.h"
#include "Key.h"
#include "Encoder.h"
#include "Timer.h"
#include "pid.h"
#include "Serial.h"
#include "Menu.h"
#include "Tracking.h"
#include "InfraredSense.h"

volatile uint8_t isRunning = 0; // 0-菜单模式, 1-循迹运行模式

volatile int32_t TargetSpeed1 = 20;
volatile int32_t TargetSpeed2 = 50;
volatile int32_t CurrentSpeed1, CurrentSpeed2;
volatile int64_t EncoderCount1 = 0;
volatile int64_t EncoderCount2 = 0;
volatile float kp = 0.8f;
volatile float ki = 0.1f;
volatile float kd = 0.2f;

PID_TypeDef Motor1_PID;
PID_TypeDef Motor2_PID;
volatile uint8_t SpeedReportFlag = 0;

int main(void)
{
    OLED_Init();
    Motor_Init();
    Key_Init();
    Encoder_Init();
    Timer_Init();
    Serial_Init();
    Tracking_Init();

    PID_Init(&Motor1_PID, kp, ki, kd);
    PID_Init(&Motor2_PID, kp, ki, kd);

    InitMenu();

    while (1) {
        if (isRunning == 0) {
            DisplayMenu();
        }
        else {
            OLED_ShowString(1, 1, "Running...     ");
            OLED_ShowString(2, 1, "               ");
            OLED_ShowString(3, 1, "               ");
            OLED_ShowString(4, 1, "               ");

            // 运行模式下，按任意键停止
            if (Key_Check(KEY_0, KEY_SINGLE) || Key_Check(KEY_1, KEY_SINGLE)) {
                isRunning = 0;
                Motor_SetSpeed(0, 0);
            }
        }

        OLED_ShowSignedNum(3, 1, CurrentSpeed1, 4);
        OLED_ShowSignedNum(3, 8, CurrentSpeed2, 4);

        // 处理串口命令
        int16_t requestedSpeed;
        if (Serial_TryParseTarget(&requestedSpeed)) {
            TargetSpeed1 = requestedSpeed;
            Serial_Printf("@ACK:%d\r\n", TargetSpeed1);
        }

        if (SpeedReportFlag) {
            SpeedReportFlag = 0;
            Serial_Printf("@CUR:%ld\r\n", (long)CurrentSpeed1);
        }
    }
}

void TIM1_UP_IRQHandler(void)
{
    Key_Tick();

    if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET) {
        // Motor1_ReadCurrentSpeed(Encoder1_Get());
        // Motor2_ReadCurrentSpeed(Encoder2_Get());
        CurrentSpeed1 = Encoder1_Get();
        CurrentSpeed2 = Encoder2_Get();
        EncoderCount1 += Motor1_GetCurrentSpeed();
        EncoderCount2 += Motor2_GetCurrentSpeed();
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);

        if (isRunning) {
            Tracking_Control();   // 计算目标速度
            Motor1_UpdateSpeed(); // 执行速度闭环
            Motor2_UpdateSpeed();
        }
        else {
            Motor1_SetPWM(0);
            Motor2_SetPWM(0);
        }

        SpeedReportFlag = 1;
    }

    InfraredSensor_Tick();
}
