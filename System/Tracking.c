#include "stm32f10x.h"
#include "Motor.h"
#include "Tracking.h"
#include "InfraredSense.h"

void Tracking_Control(void)
{
    // 这里可以添加循迹控制逻辑
    // 例如根据传感器输入调整电机速度和方向
    switch (GetInfraredSenseFlag();) {
    case:

        break;

    default:
        break;
    }
}