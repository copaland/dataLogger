#include "WiFi.h"
#include <cstring> // For strlen

extern Config config; // Ensure config is declared as extern

void setupWiFi() {
    if (strlen(config.wifi_ssid) > 0) {
        WiFi.begin(config.wifi_ssid, config.wifi_password);
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        Serial.println("\nWiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("WiFi SSID is not set!");
    }
}