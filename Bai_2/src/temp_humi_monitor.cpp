#include "temp_humi_monitor.h"
#include <Wire.h>
#include <math.h>
DHT20 dht20;
LiquidCrystal_I2C lcd(33, 16, 2);

void displayData(float t, float h, bool error) {
  lcd.setCursor(0, 0); lcd.print("                ");
  lcd.setCursor(0, 1); lcd.print("                ");
  lcd.setCursor(0, 0);
  if (error) { lcd.print("Sensor error"); Serial.println("Sensor error"); return; }
  lcd.print("Temp: "); lcd.print(t, 1); lcd.print(" C");
  lcd.setCursor(0, 1); lcd.print("Humi: "); lcd.print(h, 1); lcd.print(" %");
  Serial.printf("Temperature: %.1f C  Humidity: %.1f %%\n", t, h);
}
void temp_humi_monitor(void *p) {
  dht20.begin(11, 12); // initializes Wire with YoloUNO SDA and SCL
  lcd.begin(); lcd.backlight();
  // The library rejects reads in the first second after startup.
  vTaskDelay(pdMS_TO_TICKS(1100));
  while (true) {
    int status = dht20.read();
    float t = dht20.getTemperature(), h = dht20.getHumidity();
    bool error = status != DHT20_OK || isnan(t) || isnan(h);
    if (error) { t = 0; h = 0; }
    setSensorData(t, h, error); displayData(t, h, error);
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}
