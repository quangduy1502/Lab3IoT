#include "temp_humi_monitor.h"

#include <Wire.h>
#include <math.h>

DHT20 dht20;
LiquidCrystal_I2C lcd(33, 16, 2);

void displayData(float temperature, float humidity, bool error) {
    lcd.setCursor(0, 0);
    lcd.print("                ");

    lcd.setCursor(0, 1);
    lcd.print("                ");

    lcd.setCursor(0, 0);

    if (error) {
        lcd.print("Sensor error");
        Serial.println("Failed to read DHT20!");
        return;
    }

    lcd.print("Temp: ");
    lcd.print(temperature, 1);
    lcd.print(" C");

    lcd.setCursor(0, 1);
    lcd.print("Humi: ");
    lcd.print(humidity, 1);
    lcd.print(" %");

    Serial.print("Temperature: ");
    Serial.print(temperature, 1);
    Serial.print(" C  Humidity: ");
    Serial.print(humidity, 1);
    Serial.println(" %");
}

void temp_humi_monitor(void *pvParameters) {
    const float TEMP_RANGE_THRESHOLD = 1.0f; // Covers Max error from -10°C to 62°C
    const float HUMI_RANGE_THRESHOLD = 7.0f; // Covers worst-case Max error across 0-100% RH (use 4.0f for 30-80% RH)
    const uint32_t SAMPLE_INTERVAL_MS = 1100; // DHT20 library  requires >= 1000ms between reads

    dht20.begin(11, 12);
    lcd.begin();
    lcd.backlight();

    vTaskDelay(pdMS_TO_TICKS(SAMPLE_INTERVAL_MS));

    while (1) {
        float sumTemperature = 0;
        float sumHumidity = 0;

        float minTemperature = 1000;
        float maxTemperature = -1000;
        float minHumidity = 1000;
        float maxHumidity = -1000;

        bool valid = true;

        for (int i = 0; i < 5; i++) {
            int status = dht20.read();
            float temperature = dht20.getTemperature();
            float humidity = dht20.getHumidity();

            if (status != DHT20_OK || isnan(temperature) || isnan(humidity)) {
                valid = false;
            } else {
                sumTemperature += temperature;
                sumHumidity += humidity;

                if (temperature < minTemperature) minTemperature = temperature;
                if (temperature > maxTemperature) maxTemperature = temperature;
                if (humidity < minHumidity) minHumidity = humidity;
                if (humidity > maxHumidity) maxHumidity = humidity;
            }

            vTaskDelay(pdMS_TO_TICKS(SAMPLE_INTERVAL_MS));
        }

        if (!valid) {
            glob_temperature = 0;
            glob_humidity = 0;
            sensor_error = true;
            sensor_ready = true;
            displayData(0, 0, true);
        } else if ((maxTemperature - minTemperature < TEMP_RANGE_THRESHOLD) &&
                   (maxHumidity - minHumidity < HUMI_RANGE_THRESHOLD)) {
            float averageTemperature = sumTemperature / 5.0f;
            float averageHumidity = sumHumidity / 5.0f;

            glob_temperature = averageTemperature;
            glob_humidity = averageHumidity;
            sensor_error = false;
            sensor_ready = true;

            displayData(averageTemperature, averageHumidity, false);
        } else {
            Serial.println("Unstable samples. Keeping previous data.");
        }
    }
}