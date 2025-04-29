#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <FS.h>

// Structure to store configuration settings
struct Config {
    char wifi_ssid[32];
    char wifi_password[32];
    int log_interval;        // Logging interval in minutes
    float alarm_temperature; // Alarm temperature threshold
};

extern Config config;

// 함수 선언
void loadConfig();
void saveConfig();

// 알람 이벤트 저장/로드
void saveAlarmEvents();
void loadAlarmEvents();

// 온도 로그 저장
void saveTemperatureLog(float temperature);

#endif
