#include "led.h"

void init_led(GPIO_TypeDef *port, uint16_t pin) {

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_InitStruct.Pin = pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
}

void toggle_led(GPIO_TypeDef *port, uint16_t pin) {
	HAL_GPIO_TogglePin(port, pin);
}
