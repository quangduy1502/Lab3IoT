#ifndef GLOBAL_H
#define GLOBAL_H
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
extern float glob_temperature, glob_humidity;
extern bool sensor_ready, sensor_error;
extern SemaphoreHandle_t data_mutex;
void setSensorData(float t, float h, bool error);
bool getSensorData(float &t, float &h, bool &error);
#endif
