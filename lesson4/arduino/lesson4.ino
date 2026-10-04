#include <SoftwareSerial.h>

const int BUTTON_PIN = 4; // Кнопка подключена к D4 и GND
const int LED_PIN = 5;    // Светодиод на D5

// Настройка программного UART: RX = D3, TX = D2
SoftwareSerial stmSerial(3, 2);

bool toggleState = false; // Флаг состояния: false -> 'A', true -> 'a'
int lastButtonState = HIGH;

void setup() {
  // put your setup code here, to run once:
  // Инициализируем аппаратный UART на скорости 9600 бод (для ПК)
  Serial.begin(9600);
  // Программный UART для связи с STM32 (9600 бод)
  stmSerial.begin(9600);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("Arduino UNO готова к обмену UART.");
}

void loop() {
  int currentButtonState = digitalRead(BUTTON_PIN);

  // Фиксация момента нажатия (переход из HIGH в LOW)
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    delay(50); // Программный антидребезг (50 мс)
    
    if (digitalRead(BUTTON_PIN) == LOW) {
      char symbolToSend = (!toggleState) ? 'A' : 'a';
      
      // 1. Отправляем символ в STM32 через SoftwareSerial (пин D2 -> TX)
      stmSerial.print(symbolToSend);
      
      // 2. Выводим дубль в ПК для контроля
      Serial.print("[Arduino TX -> STM32]: ");
      Serial.println(symbolToSend);
      
      // Переключаем флаг
      toggleState = !toggleState;
    }
  }

  lastButtonState = currentButtonState;

  // Проверка входящих данных от STM32
  if (stmSerial.available() > 0) {
    char receivedChar = stmSerial.read();
    
    Serial.print("[STM32 TX -> Arduino]: ");
    Serial.println(receivedChar);

    // Управление местным светодиодом на D5
    if (receivedChar == 'B') {
      digitalWrite(LED_PIN, HIGH);
    } else if (receivedChar == 'b') {
      digitalWrite(LED_PIN, LOW);
    }
  }
}