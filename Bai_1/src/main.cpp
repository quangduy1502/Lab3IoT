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

void led_blinky(void *p) {
  pinMode(LED_PIN, OUTPUT);
  while (true) {
    digitalWrite(LED_PIN, HIGH); vTaskDelay(pdMS_TO_TICKS(1000));
    digitalWrite(LED_PIN, LOW); vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void setup() {
  Serial.begin(115200);
  data_mutex = xSemaphoreCreateMutex();
  if (data_mutex == NULL) { Serial.println("Mutex creation failed"); return; }
  xTaskCreate(temp_humi_monitor, "DHT20", 4096, NULL, 2, NULL);
  xTaskCreate(task_neopixel, "NeoPixel", 4096, NULL, 2, NULL);
  xTaskCreate(led_blinky, "LED", 2048, NULL, 2, NULL);
}
void loop() { vTaskDelay(pdMS_TO_TICKS(1000)); }
