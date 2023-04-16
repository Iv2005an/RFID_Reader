#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9  // Пин rfid модуля RST
#define SS_PIN 10  // Пин rfid модуля SS

MFRC522 rfid(SS_PIN, RST_PIN);  // Объект rfid модуля
MFRC522::MIFARE_Key key;        // Объект ключа
MFRC522::StatusCode status;     // Объект статуса

void setup() {
  Serial.begin(115200);                      // Инициализация Serial
  SPI.begin();                               // Инициализация SPI
  rfid.PCD_Init();                           // Инициализация модуля
  rfid.PCD_SetAntennaGain(rfid.RxGain_max);  // Установка усиления антенны
  rfid.PCD_AntennaOff();                     // Выключаем антенну
  rfid.PCD_AntennaOn();                      // Включаем антенну

  for (byte i = 0; i < 6; i++) {  // Наполняем ключ
    key.keyByte[i] = 0xFF;        // Ключ по умолчанию 0xFFFFFFFFFFFF
  }
}

void loop() {
  static uint32_t reboot_timer = millis();  // Таймер перезагрузки
  static uint32_t read_timer = millis();    // Таймер чтения
  if (millis() - reboot_timer >= 1000) {    // Таймер с периодом 1000 мс
    reboot_timer = millis();                // Сброс таймера
    digitalWrite(RST_PIN, HIGH);            // Сбрасываем модуль
    delayMicroseconds(2);                   // Ждем 2 мкс
    digitalWrite(RST_PIN, LOW);             // Отпускаем сброс
    rfid.PCD_Init();                        // Инициализируем заного
  }

  if (!rfid.PICC_IsNewCardPresent()) return;  // Если новая метка не поднесена - вернуться в начало loop
  if (!rfid.PICC_ReadCardSerial()) return;    // Если метка не читается - вернуться в начало loop
  if (millis() - read_timer <= 200) {
    read_timer = millis();  // Сброс таймера
    return;
  }
  for (uint8_t i = 0; i < 4; i++) {  // Обработка UID
    Serial.print("0x");
    Serial.print(rfid.uid.uidByte[i], HEX);
    if (i != 3) Serial.print(" ");
  }
  Serial.println();
  read_timer = millis();  // Сброс таймера
}