#include "WebServer.h"

WebServer server(80);

void setupWebServer() {
    if(!SPIFFS.begin(true)) {
        Serial.println("SPIFFS initialization failed!");
        return;
    }

    server.on("/", HTTP_GET, handleRoot);
    server.on("/getData", HTTP_GET, handleGetData);
    server.on("/getAlarms", HTTP_GET, handleGetAlarms);
    server.on("/updateConfig", HTTP_POST, handleUpdateConfig);
    server.on("/deleteAlarm", HTTP_POST, handleDeleteAlarm);
    
    server.enableCORS(true);
    server.begin();
    Serial.println("HTTP server started");
}

void handleRoot() {
    File file = SPIFFS.open("/index.html", "r");
    if(!file) {
        server.send(404, "text/plain", "File not found");
        return;
    }
    server.streamFile(file, "text/html");
    file.close();
}

void handleGetData() {
    StaticJsonDocument<200> doc;
    doc["temperature"] = currentTemp;
    
    struct tm timeinfo;
    if(getLocalTime(&timeinfo)) {
        char timeStringBuff[25];
        strftime(timeStringBuff, sizeof(timeStringBuff), "%Y-%m-%d %H:%M:%S", &timeinfo);
        doc["timestamp"] = String(timeStringBuff);
    }

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void handleGetAlarms() {
    DynamicJsonDocument doc(4096);
    JsonArray array = doc.to<JsonArray>();
    
    for(const auto& event : alarmEvents) {
        JsonObject eventObj = array.createNestedObject();
        eventObj["timestamp"] = event.timestamp;
        eventObj["temperature"] = event.temperature;
    }

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void handleUpdateConfig() {
    if (server.hasArg("plain")) {
        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, server.arg("plain"));

        if (error) {
            server.send(400, "text/plain", "Invalid JSON");
            return;
        }

        // Update configuration
        strlcpy(config.wifi_ssid, doc["wifi_ssid"] | "", sizeof(config.wifi_ssid));
        strlcpy(config.wifi_password, doc["wifi_password"] | "", sizeof(config.wifi_password));
        config.log_interval = doc["log_interval"] | 10;
        config.alarm_temperature = doc["alarm_temperature"] | -150.0;

        saveConfig();
        server.send(200, "application/json", "{\"success\":true}");
    } else {
        server.send(400, "text/plain", "No data received");
    }
}

void handleDeleteAlarm() {
    if (server.hasArg("plain")) {
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, server.arg("plain"));
        
        if (error) {
            server.send(400, "text/plain", "Invalid JSON");
            return;
        }

        time_t timestamp = doc["timestamp"];
        auto it = std::remove_if(alarmEvents.begin(), alarmEvents.end(),
            [timestamp](const AlarmEvent& event) {
                return event.timestamp == timestamp;
            });
        
        if (it != alarmEvents.end()) {
            alarmEvents.erase(it, alarmEvents.end());
            saveAlarmEvents();
        }

        server.send(200, "application/json", "{\"success\":true}");
    }
}
