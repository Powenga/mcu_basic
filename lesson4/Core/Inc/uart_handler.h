/*
 * uart_handler.h
 *
 *  Created on: 30 сент. 2026 г.
 *      Author: Dmitrii
 */

#ifndef INC_UART_HANDLER_H_
#define INC_UART_HANDLER_H_

#include "main.h"

void UART_Handler_Init(UART_HandleTypeDef *huart);

void UART_Handler_Process(void);

#endif /* INC_UART_HANDLER_H_ */
