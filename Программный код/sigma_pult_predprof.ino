/*
 *   D7  — NRF24 CE
 *   D8  — NRF24 CSN
 *   D13 — NRF24 SCK
 *   D11 — NRF24 MOSI
 *   D12 — NRF24 MISO
 *   3.3V — NRF24 VCC (ОБЯЗАТЕЛЬНО 3.3V!)
 *   GND — Общий минус
 */

#include <SPI.h>
#include <RF24.h>

RF24 radio(7, 8);
const byte address[6] = "00001";

bool radio_ready = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Ожидание команд");
  Serial.println("Доступные команды:");
  Serial.println("  '1' — Горизонтальный скан");
  Serial.println("  '2' — Вертикальный скан");
  Serial.println("  '3' — Диагональ 1");
  Serial.println("  '4' — Диагональ 2");
  Serial.println("  '0' — Стоп + возврат в центр");
  Serial.println("  '5' — Лазер ВКЛ");
  Serial.println("  '6' — Лазер ВЫКЛ");

  if (!radio.begin()) {
    Serial.println("ОШИБКА: NRF24 не обнаружен!");
    Serial.println("Проверьте подключение:");
  }
  
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  
  radio_ready = true;
  Serial.println("Радиомодуль готов. Введите команду:");
}

void loop() {
  if (!radio_ready) return;
  if (Serial.available()) {
    char command = Serial.read();
    if (command == '\n' || command == '\r') return;
    Serial.print("→ Отправка команды '");
    Serial.print(command);
    Serial.print("' в кубсат... ");
    bool ok = radio.write(&command, sizeof(command));   
    if (ok) {
      Serial.println("OK");
    } else {
      Serial.println("ОШИБКА: нет связи с кубсатом!");
      Serial.println("Проверьте:");
      Serial.println("  • Кубсат включён и в радиусе 5 м");
      Serial.println("  • Антенны направлены друг на друга");
      Serial.println("  • Питание кубсата стабильно 5В");
    }
  }
}