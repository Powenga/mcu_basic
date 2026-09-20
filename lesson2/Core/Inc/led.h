#ifndef INC_LED_H
#define INC_LED_H

#include "main.h"

void init_led(GPIO_TypeDef* port, uint16_t pin);
void toggle_led(GPIO_TypeDef* port, uint16_t pin);

#endif
