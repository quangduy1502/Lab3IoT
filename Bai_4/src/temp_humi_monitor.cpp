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
    float sumT = 0, sumH = 0, minT = 1000, maxT = -1000, minH = 1000, maxH = -1000;
    bool valid = true;
    for (int i = 0; i < 5; ++i) {
      int status = dht20.read();
      float t = dht20.getTemperature(), h = dht20.getHumidity();
      if (status != DHT20_OK || isnan(t) || isnan(h)) valid = false;
      else {
        sumT += t; sumH += h;
        if (t < minT) minT = t; if (t > maxT) maxT = t;
        if (h < minH) minH = h; if (h > maxH) maxH = h;
      }
      // Driver enforces at least 1 s between actual sensor readings.
      vTaskDelay(pdMS_TO_TICKS(1100));
    }
    if (!valid) { setSensorData(0, 0, true); displayData(0, 0, true); }
    else if (maxT - minT < 1.0f && maxH - minH < 3.0f) {
      float t = sumT / 5.0f, h = sumH / 5.0f;
      setSensorData(t, h, false); displayData(t, h, false);
    } else { Serial.println("Unstable samples: retaining previous values"); }
  }
}
