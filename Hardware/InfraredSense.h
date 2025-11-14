#ifndef INFRAREDSENSE_H
#define INFRAREDSENSE_H

#define INFRARED_LEFT 0x08
#define INFRARED_MIDDLELEFT 0x04
#define INFRARED_MIDDLERIGHT 0x02
#define INFRARED_RIGHT 0x01

#define DEVIATE_TO_LEFT 0x0C
#define DEVIATE_TO_RIGHT 0x03
#define ON_TRACK 0x06
#define OFF_TRACK 0x00
#define CROSSED_LINE 0x0F

void InfraredSense_Init(void);
void InfraredSensor_Tick(void);
uint8_t GetInfraredSenseFlag(void);

#endif // INFRAREDSENSE_H
