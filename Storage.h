#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "WebServer.h"

void loadConfig();
void saveConfig();
void saveAlarmEvents();
void logData(float temperature);

#endif