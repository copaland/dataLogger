#include "Storage.h"

// Load configuration from SPIFFS
void loadConfig() {
    if (SPIFFS.exists("/config.json")) {
        File configFile = SPIFFS.open("/config.json", "r");
        if (configFile) {
            StaticJsonDocument<512> doc;
            DeserializationError error = deserializeJson(doc, configFile);

            if (!error) {
                // 설정 값 불러오기
                // 예: logInterval = doc["logInterval"].as<int>() * 60000;
                strlcpy(config.wifi_ssid, doc["wifi_ssid"] | "", sizeof(config.wifi_ssid));
                strlcpy(config.wifi_password, doc["wifi_password"] | "", sizeof(config.wifi_password));
                config.log_interval = doc["log_interval"] | DEFAULT_LOG_INTERVAL;
                config.alarm_temperature = doc["alarm_temperature"] | -150.0; // Default to -150°C
            } else {
                Serial.println("설정 파일 파싱 실패");
            }
            configFile.close();
        }
    } 
 /*   
    else {
        // Default configuration
        strlcpy(config.wifi_ssid, "YourSSID", sizeof(config.wifi_ssid));
        strlcpy(config.wifi_password, "YourPassword", sizeof(config.wifi_password));
        config.log_interval = DEFAULT_LOG_INTERVAL;
        config.alarm_temperature = -150.0; // Default to -150°C
        saveConfig();
    }
    */
}

// Save configuration to SPIFFS
void saveConfig() {
    File configFile = SPIFFS.open("/config.json", "w");
    if (configFile) {
        StaticJsonDocument<512> doc;
        // 설정 값 저장하기
        // 예: doc["logInterval"] = logInterval / 60000;
        doc["wifi_ssid"] = config.wifi_ssid;
        doc["wifi_password"] = config.wifi_password;
        doc["log_interval"] = config.log_interval;
        doc["alarm_temperature"] = config.alarm_temperature;

        serializeJson(doc, configFile);
        configFile.close();
    } else {
        Serial.println("설정 파일 열기 실패");
        return;
    }
}

void saveAlarmEvents() {
    File alarmFile = SPIFFS.open("/alarms.json", "w");
    if (alarmFile) {
        DynamicJsonDocument doc(16384);
        JsonArray array = doc.to<JsonArray>();
        
        for(const auto& event : alarmEvents) {
            JsonObject eventObj = array.createNestedObject();
            eventObj["timestamp"] = event.timestamp;
            eventObj["temperature"] = event.temperature;
        }
        
        serializeJson(doc, alarmFile);
        alarmFile.close();
    }
}

void logData(float temperature) {
    File dataFile = SPIFFS.open("/temperature_log.csv", "a");
    if (dataFile) {
        time_t now = time(nullptr);
        struct tm timeinfo;
        localtime_r(&now, &timeinfo);
        
        char timestamp[25];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &timeinfo);
        
        dataFile.printf("%s,%.1f\n", timestamp, temperature);
        dataFile.close();
    }
}
