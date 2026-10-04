/*
 * uart_handler.h
 *
 *  Created on: 30 сент. 2026 г.
 *      Author: Dmitrii
 */

#ifndef INC_UART_HANDLER_H_
#define INC_UART_HANDLER_H_

#include "main.h"

void UART_Receive_Handler_Init(UART_HandleTypeDef *main_uart, UART_HandleTypeDef *log_uart);

void UART_Receive_Handler_Process(void);
void UART_Transmit_Process(void);

#endif /* INC_UART_HANDLER_H_ */
