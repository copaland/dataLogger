#include "Storage.h"

void loadConfig() {
    if (SPIFFS.exists("/config.json")) {
        File configFile = SPIFFS.open("/config.json", "r");
        if (configFile) {
            StaticJsonDocument<1024> doc;
            DeserializationError error = deserializeJson(doc, configFile);
            
            if (!error) {
                strlcpy(config.wifi_ssid, doc["wifi_ssid"] | "", sizeof(config.wifi_ssid));
                strlcpy(config.wifi_password, doc["wifi_password"] | "", sizeof(config.wifi_password));
                config.log_interval = doc["log_interval"] | DEFAULT_LOG_INTERVAL;
            }
            configFile.close();
        }
    } else {
        // 기본 설정 저장
        strlcpy(config.wifi_ssid, "YourSSID", sizeof(config.wifi_ssid));
        strlcpy(config.wifi_password, "YourPassword", sizeof(config.wifi_password));
        config.log_interval = DEFAULT_LOG_INTERVAL;
        saveConfig();
    }
}

void saveConfig() {
    File configFile = SPIFFS.open("/config.json", "w");
    if (configFile) {
        StaticJsonDocument<1024> doc;
        doc["wifi_ssid"] = config.wifi_ssid;
        doc["wifi_password"] = config.wifi_password;
        doc["log_interval"] = config.log_interval;
        
        serializeJson(doc, configFile);
        configFile.close();
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