void setupWiFi() {
  if (strlen(config.wifi_ssid) > 0) {
    WiFi.begin(config.wifi_ssid, config.wifi_password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      Serial.print(".");
      attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nConnected to WiFi");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("\nFailed to connect to WiFi");
    }
  }
}

void sendPushNotification() {
  // Implement push notification logic here
  // You might want to use services like Firebase Cloud Messaging, IFTTT, or custom implementation
}