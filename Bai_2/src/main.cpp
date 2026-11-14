#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "temp_humi_monitor.h"
#include "global.h"
#define NEO_PIN 45
#define LED_PIN 48
#define LED_COUNT 1

void task_neopixel(void *p) {
  Adafruit_NeoPixel strip(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);
  strip.begin(); strip.clear(); strip.show();
  bool on = false;
  while (true) {
    float t, h; bool error;
    bool ready = getSensorData(t, h, error);
    uint32_t color = strip.Color(0, 0, 0);
    if (ready && !error) {
      if (t < 20.0f) color = strip.Color(0, 0, 255);
      else if (t < 28.0f) color = strip.Color(0, 255, 0);
      else color = strip.Color(255, 0, 0);
    }
    strip.setPixelColor(0, color); strip.show();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

// LED toggles at each specified interval. Recheck error every 100 ms.
void task_led_blinky(void *p) {
  pinMode(LED_PIN, OUTPUT);
  bool state = false;
  uint32_t lastChange = millis();
  while (true) {
    float t, h; bool error;
    bool ready = getSensorData(t, h, error);
    if (!ready || error) {
      digitalWrite(LED_PIN, LOW); state = false; lastChange = millis();
    } else {
      uint32_t interval = h < 40.0f ? 2000 : (h <= 60.0f ? 1000 : 500);
      if (millis() - lastChange >= interval) {
        state = !state; digitalWrite(LED_PIN, state ? HIGH : LOW);
        lastChange = millis();
      }
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void setup() {
  Serial.begin(115200);
  data_mutex = xSemaphoreCreateMutex();
  if (data_mutex == NULL) { Serial.println("Mutex creation failed"); return; }
  xTaskCreate(temp_humi_monitor, "DHT20", 4096, NULL, 2, NULL);
  xTaskCreate(task_neopixel, "NeoPixel", 4096, NULL, 2, NULL);
  xTaskCreate(task_led_blinky, "LED", 2048, NULL, 2, NULL);
}
void loop() { vTaskDelay(pdMS_TO_TICKS(1000)); }
