#include "global.h"
float glob_temperature = 0, glob_humidity = 0;
bool sensor_ready = false, sensor_error = false;
SemaphoreHandle_t data_mutex = NULL;
void setSensorData(float t, float h, bool error) {
  xSemaphoreTake(data_mutex, portMAX_DELAY);
  glob_temperature = t; glob_humidity = h;
  sensor_error = error; sensor_ready = true;
  xSemaphoreGive(data_mutex);
}
bool getSensorData(float &t, float &h, bool &error) {
  xSemaphoreTake(data_mutex, portMAX_DELAY);
  t = glob_temperature; h = glob_humidity;
  error = sensor_error; bool ready = sensor_ready;
  xSemaphoreGive(data_mutex);
  return ready;
}
