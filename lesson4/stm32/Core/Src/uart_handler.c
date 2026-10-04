/*
 * uart_handler.c
 *
 *  Created on: 30 сент. 2026 г.
 *      Author: Dmitrii
 */

#include "uart_handler.h"

static UART_HandleTypeDef *main_huart = NULL; // UART для взаимодействия
static UART_HandleTypeDef *log_huart = NULL; // UART для отладки

static uint8_t rx_byte = 0; // Принимаемый байт
static volatile uint8_t msg_ready = 0; // Флаг для проверки что сообщение принято, в прерывании
static uint8_t cmd_to_process = 0; // Буфер для команды

static volatile uint8_t start_transmission = 0; // Флаг прерывания для нажатия кнопки
uint8_t toogleState = 0; // Флаг состояния: false -> 'B', true -> 'b'


static void UART_Transmit_Log(uint8_t *data, uint16_t Size);
static void UART_Receive_Log(uint8_t *data, uint16_t Size);

/*Обработчик входящих сообщений*/
void UART_Receive_Handler_Init(UART_HandleTypeDef *main_uart, UART_HandleTypeDef *log_uart)
{
    main_huart = main_uart;  
    if (main_huart != NULL)
    {
        // Включаем прерывание на прием первого байта
        HAL_UART_Receive_IT(main_huart, &rx_byte, 1);
    }

    log_huart = log_uart;
}

void UART_Receive_Handler_Process(void)
{
    if (!msg_ready) {
        return; // Если команды нет, сразу выходим
    }

    msg_ready = 0; // Сбрасываем флаг
    
    // Дублируем в ПК по USART2 (USB ST-LINK) для проверки то, что пришло от arduino
    UART_Receive_Log(&cmd_to_process, sizeof(cmd_to_process));
    
     // Выполняем действия в зависимости от команды
    if (cmd_to_process == 'A') {
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
    }
    else if (cmd_to_process == 'a') {
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
    }
}

void UART_Transmit_Process() {
    if(start_transmission == 1) {
        uint8_t transmitted_data = toogleState ? 'b' : 'B';
        toogleState = !toogleState;
        HAL_UART_Transmit(main_huart, &transmitted_data, sizeof(transmitted_data), 100);
        // Дублируем в ПК по USART2 (USB ST-LINK) для проверки
        UART_Transmit_Log(&transmitted_data, sizeof(transmitted_data));
        start_transmission = 0;
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // Проверяем, что прерывание пришло именно от нашего UART
    if (main_huart != NULL && huart->Instance == main_huart->Instance)
    {
        cmd_to_process = rx_byte;
        msg_ready = 1;

        // Перезапускаем прием
        HAL_UART_Receive_IT(main_huart, &rx_byte, 1);
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == B1_Pin) {
		start_transmission = 1;
	}
}

static void UART_Transmit_Log(uint8_t *data, uint16_t Size) {
    if(log_huart == NULL) {
        return;
    }
      HAL_UART_Transmit(log_huart, (uint8_t*)"[STM32 TX -> Arduino]: ", 23, 100);
      HAL_UART_Transmit(log_huart, data, Size, 100);
      HAL_UART_Transmit(log_huart, (uint8_t*)"\r\n", 2, 100);
}

static void UART_Receive_Log(uint8_t *data, uint16_t Size) {
    if(log_huart == NULL) {
        return;
    }
    HAL_UART_Transmit(log_huart, (uint8_t*)"[Arduino TX -> STM32]: ", 23, 100);
    HAL_UART_Transmit(log_huart, data, Size, 100);
    HAL_UART_Transmit(log_huart, (uint8_t*)"\r\n", 2, 100);
}
