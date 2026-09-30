/*
 * uart_handler.h
 *
 *  Created on: 30 сент. 2026 г.
 *      Author: Dmitrii
 */

#include "uart_handler.h"

static UART_HandleTypeDef *g_huart = NULL;

static uint8_t rx_byte = 0;
static uint8_t msg_ready = 0;
static uint8_t cmd_to_process = 0;

void UART_Handler_Init(UART_HandleTypeDef *huart)
{
    g_huart = huart;
    if (g_huart != NULL)
    {
        // Включаем прерывание на прием первого байта
        HAL_UART_Receive_IT(g_huart, &rx_byte, 1);
    }
}

void UART_Handler_Process(void)
{
    if (!msg_ready) {
        return; // Если команды нет, сразу выходим, не тратя время процессора
    }

    msg_ready = 0; // Сбрасываем флаг

    // Выполняем действия в зависимости от команды
    if (cmd_to_process == 'A') {
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
    }
    else if (cmd_to_process == 'a') {
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
    }
    else {
        // Мягкое мигание для неизвестной команды
        for (uint8_t var = 0; var < 6; ++var) {
            HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
            HAL_Delay(100);
        }
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
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
