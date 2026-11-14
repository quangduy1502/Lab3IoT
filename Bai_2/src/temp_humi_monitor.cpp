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
    dht20.begin(11, 12);

    lcd.begin();
    lcd.backlight();

    vTaskDelay(pdMS_TO_TICKS(1100));

    while (1) {
        int status = dht20.read();

        float temperature = dht20.getTemperature();
        float humidity = dht20.getHumidity();

        bool error = false;

        if (status != DHT20_OK ||
            isnan(temperature) ||
            isnan(humidity)) {
            error = true;

            temperature = 0;
            humidity = 0;
        }

        glob_temperature = temperature;
        glob_humidity = humidity;

        sensor_error = error;
        sensor_ready = true;

        displayData(temperature, humidity, error);

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}