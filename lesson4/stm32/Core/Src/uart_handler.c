/*
 * uart_handler.h
 *
 *  Created on: 30 сент. 2026 г.
 *      Author: Dmitrii
 */

#include "uart_handler.h"

static UART_HandleTypeDef *g_huart = NULL; // переменная для хранения какой UART инициирован

static uint8_t rx_byte = 0; // Принимаемый байт
static uint8_t msg_ready = 0; // Флаг для проверки что сообщение принято, в прерывании
static uint8_t cmd_to_process = 0; // Буфер для команды

volatile uint8_t start_transmission = 0; // Флаг прерывания для нажатия кнопки
uint8_t toogleState = 0; // Флаг состояния: false -> 'B', true -> 'b'


/*Обработчик входящих сообщений*/
void UART_ReceiveHandler_Init(UART_HandleTypeDef *huart)
{
    g_huart = huart;
    if (g_huart != NULL)
    {
        // Включаем прерывание на прием первого байта
        HAL_UART_Receive_IT(g_huart, &rx_byte, 1);
    }
}

void UART_ReceiveHandler_Process(void)
{
    if (!msg_ready) {
        return; // Если команды нет, сразу выходим
    }

    msg_ready = 0; // Сбрасываем флаг
    
    // Дублируем в ПК по USART2 (USB ST-LINK) для проверки то, что пришло от arduino
    HAL_UART_Transmit(huart, (uint8_t*)"[Arduino -> STM32 TX]: ", 23, 100);
    HAL_UART_Transmit(huart, &transmitted_data, 1, 100);
    HAL_UART_Transmit(huart, (uint8_t*)"\r\n", 2, 100);
    
     // Выполняем действия в зависимости от команды
    if (cmd_to_process == 'A') {
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
    }
    else if (cmd_to_process == 'a') {
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
    }
}

void UART_Transmit_Process(UART_HandleTypeDef *huart) {
    if(start_transmission == 1) {
      uint8_t transmitted_data = toogleState ? 'b' : 'B';
      toogleState = !toogleState;
      HAL_UART_Transmit(huart, &transmitted_data, sizeof(transmitted_data), 100);
      // Дублируем в ПК по USART2 (USB ST-LINK) для проверки
      HAL_UART_Transmit(huart, (uint8_t*)"[STM32 TX -> Arduino]: ", 23, 100);
      HAL_UART_Transmit(huart, &transmitted_data, 1, 100);
      HAL_UART_Transmit(huart, (uint8_t*)"\r\n", 2, 100);
      start_transmission = 0;
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // Проверяем, что прерывание пришло именно от нашего UART
    if (g_huart != NULL && huart->Instance == g_huart->Instance)
    {
        cmd_to_process = rx_byte;
        msg_ready = 1;

        // Перезапускаем прием
        HAL_UART_Receive_IT(g_huart, &rx_byte, 1);
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == B1_Pin) {
		start_transmission = 1;
	}
}
