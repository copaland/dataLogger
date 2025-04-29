#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <WebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <ModbusRTU.h>
#include <HardwareSerial.h>

// Global variables
extern WebServer server;
extern ModbusRTU mb;
extern float currentTemp;
extern bool isAlarmActive;
extern std::vector<struct AlarmEvent> alarmEvents;

// Struct definitions
struct Config {
    char wifi_ssid[32];
    char wifi_password[32];
    int log_interval;  // in seconds
};

struct AlarmEvent {
    time_t timestamp;
    float temperature;
};

// Function declarations
void setupWebServer();
void handleRoot();
void handleGetData();
void handleGetAlarms();
void handleUpdateConfig();
void handleDeleteAlarm();
void loadConfig();
void saveConfig();
void saveAlarmEvents();

#endif