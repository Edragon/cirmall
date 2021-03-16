#ifndef __SPEED_CONTROL_H
#define __SPEED_CONTROL_H

#include "stm8s.h"


#define DUTY_FULL      100
#define DUTY_H         100
#define DUTY_L         50
#define DUTY_ZERO      0

#define SPEED_HIGHVSP  12
#define SPEED_LOWVSP   10
#define SPEED_ZEROVSP  0

#ifdef __SPEED_CONTROL_C
  u8 Vfan=0;
#else
  extern u8 Vfan;
#endif

void Driver_PWM(void);
void Process_Speed_Control(void);

#endif