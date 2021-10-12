#ifndef _APP_PWM_H_
#define _APP_PWM_H_

#include "mm32_device.h"
#include "hal_conf.h"

#define PWM1_PORT 	GPIOA
#define PWM1_Pin   	GPIO_Pin_15

#define LED1_ON()  GPIO_ResetBits(LED1_Port,LED1_Pin)
#define LED1_OFF()  GPIO_SetBits(LED1_Port,LED1_Pin)



#endif



