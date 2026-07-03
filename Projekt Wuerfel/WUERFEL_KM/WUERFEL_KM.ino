#include <Adafruit_NeoPixel.h>

// === Пины ===
#define DATA_PIN  8   // DATA (DS) для 74HC595
#define CLOCK_PIN 9   // CLOCK (SHCP) для 74HC595
#define LATCH_PIN 10  // STCP (latch) для 74HC595
#define LED_PIN   6   // Пин для цепочки WS2812B (7 штук)

// === Настройки NeoPixel ===
#define NUM_LEDS 7
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// === Паттерны для 7-сегментного индикатора (перевёрнутые вверх ногами) ===
// Биты: bit0=a, bit1=b, bit2=c, bit3=d, bit4=e, bit5=f, bit6=g, bit7=dp (не используем)
byte segPatterns[7] = {
  0,          // 0 — не используется
  B00110000,  // 1 (перевёрнутая)
  B01011011,  // 2 (перевёрнутая)
  B01111001,  // 3 (перевёрнутая)
  B01110100,  // 4 (перевёрнутая)
  B01101101,  // 5 (перевёрнутая)
  B01101111   // 6 (перевёрнутая)
};

void setup() {
  // Пины сдвигового регистра
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  
  // Входные пины (D2, D3, D4) — читаем как двоичный код
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);

  // Инициализация NeoPixel
  strip.begin();
  strip.setBrightness(80);
  strip.show(); // всё выключаем
}

void loop() {
  // Читаем сигналы с D2, D3, D4 (D2 — младший бит)
  int d2 = digitalRead(2);
  int d3 = digitalRead(3);
  int d4 = digitalRead(4);
  
  // Преобразуем в число 0–7
  int value = (d4 << 2) | (d3 << 1) | d2;

  // Если значение от 1 до 6 — выводим цифру и управляем светодиодами
  if (value >= 1 && value <= 6) {
    // === Вывод на 7-сегментный индикатор через 74HC595 ===
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, segPatterns[value]);
    digitalWrite(LATCH_PIN, HIGH);

    // === Управление NeoPixel (7 штук) ===
    updateNeoPixels(value);
  } 
  else {
    // Если значение не 1–6 — гасим всё
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0); // пустой паттерн
    digitalWrite(LATCH_PIN, HIGH);
    
    strip.clear();
    strip.show();
  }

  delay(50); // небольшая задержка для стабильности чтения
}

// === Функция управления 7 RGB-светодиодами ===
void updateNeoPixels(int num) {
  uint32_t color;
  
  // Выбираем цвет в зависимости от цифры
  switch (num) {
    case 1: color = strip.Color(128, 0, 0);     break; // Красный
    case 2: color = strip.Color(128, 128, 0);   break; // Жёлтый
    case 3: color = strip.Color(0, 128, 0);     break; // Зелёный
    case 4: color = strip.Color(0, 128, 128);   break; // Светло-голубой (циан)
    case 5: color = strip.Color(0, 0, 128);     break; // Синий
    case 6: color = strip.Color(128, 0, 128);   break; // Фиолетовый
    default: return;
  }

  // Гасим все светодиоды перед установкой новых
  strip.clear();

  // Зажигаем нужные RGB (нумерация: индекс 0 = RGB1, индекс 3 = RGB4 и т.д.)
  switch (num) {
    case 1:
      strip.setPixelColor(3, color);           // только RGB 4
      break;
    case 2:
      strip.setPixelColor(2, color);           // RGB 3 + 5
      strip.setPixelColor(4, color);
      break;
    case 3:
      strip.setPixelColor(2, color);           // RGB 3 + 4 + 5
      strip.setPixelColor(3, color);
      strip.setPixelColor(4, color);
      break;
    case 4:
      strip.setPixelColor(0, color);           // RGB 1 + 3 + 5 + 7
      strip.setPixelColor(2, color);
      strip.setPixelColor(4, color);
      strip.setPixelColor(6, color);
      break;
    case 5:
      strip.setPixelColor(0, color);           // RGB 1 + 3 + 4 + 5 + 7
      strip.setPixelColor(2, color);
      strip.setPixelColor(3, color);
      strip.setPixelColor(4, color);
      strip.setPixelColor(6, color);
      break;
    case 6:
      strip.setPixelColor(0, color);           // RGB 1 + 2 + 3 + 5 + 6 + 7
      strip.setPixelColor(1, color);
      strip.setPixelColor(2, color);
      strip.setPixelColor(4, color);
      strip.setPixelColor(5, color);
      strip.setPixelColor(6, color);
      break;
  }

  strip.show(); // применяем изменения
}