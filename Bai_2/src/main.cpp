#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "global.h"
#include "temp_humi_monitor.h"

#define LED_PIN 48
#define NEO_PIN 45
#define LED_COUNT 1

void task_neopixel(void *pvParameters) {
    Adafruit_NeoPixel strip(
        LED_COUNT,
        NEO_PIN,
        NEO_GRB + NEO_KHZ800
    );

    strip.begin();
    strip.clear();
    strip.show();

    while (1) {
        float temperature = glob_temperature;
        bool ready = sensor_ready;
        bool error = sensor_error;

        uint32_t color = strip.Color(0, 0, 0);

        if (ready && !error) {
            if (temperature < 20.0f) {
                color = strip.Color(0, 0, 255);
            }
            else if (temperature < 28.0f) {
                color = strip.Color(0, 255, 0);
            }
            else {
                color = strip.Color(255, 0, 0);
            }
        }

        strip.setPixelColor(0, color);
        strip.show();

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void task_led_blinky(void *pvParameters) {
    pinMode(LED_PIN, OUTPUT);

    bool ledState = false;
    unsigned long lastChange = millis();

    while (1) {
        float humidity = glob_humidity;
        bool ready = sensor_ready;
        bool error = sensor_error;

        if (!ready || error) {
            digitalWrite(LED_PIN, LOW);
            ledState = false;
            lastChange = millis();
        }
        else {
            unsigned long interval;

            if (humidity < 40.0f) {
                interval = 2000;
            }
            else if (humidity <= 60.0f) {
                interval = 1000;
            }
            else {
                interval = 500;
            }

            if (millis() - lastChange >= interval) {
                ledState = !ledState;

                if (ledState) {
                    digitalWrite(LED_PIN, HIGH);
                }
                else {
                    digitalWrite(LED_PIN, LOW);
                }

                lastChange = millis();
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void setup() {
    Serial.begin(115200);

    xTaskCreate(
        temp_humi_monitor,
        "DHT20",
        4096,
        NULL,
        2,
        NULL
    );

    xTaskCreate(
        task_neopixel,
        "NeoPixel",
        4096,
        NULL,
        2,
        NULL
    );

    xTaskCreate(
        task_led_blinky,
        "LED",
        2048,
        NULL,
        2,
        NULL
    );
}

void loop() {
    vTaskDelay(pdMS_TO_TICKS(1000));
}