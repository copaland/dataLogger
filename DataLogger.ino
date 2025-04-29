#include <WiFi.h>
#include <time.h>
#include "WebServer.h"
#include "Storage.h"
#include <ModbusRTU.h>

// Pin definitions
#define LED_PIN 2
#define BUZZER_PIN 7
#define MODBUS_RW_PIN 4
// Modbus 설정
#define MODBUS_RX_PIN 16  // ESP32의 RX 핀
#define MODBUS_TX_PIN 17  // ESP32의 TX 핀
#define MODBUS_ID 1       // Slave ID
#define TEMPERATURE_REGISTER 0  // 온도 레지스터 주소
#define MODBUS_BAUDRATE 9600   // 통신 속도

// Constants
#define ALARM_TEMP -150.0
#define DEFAULT_LOG_INTERVAL 600

// Global variables
ModbusRTU mb;
// Modbus 통신을 위한 Serial2 사용
HardwareSerial ModbusSerial(2);  // UART2 사용
float currentTemp = 0.0;
bool isAlarmActive = false;
std::vector<AlarmEvent> alarmEvents;
Config config;

// WiFi and NTP settings
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 32400;  // UTC+9 (Seoul)
const int daylightOffset_sec = 0;

void setup() {
    Serial.begin(115200);
    // Serial2 초기화
    ModbusSerial.begin(MODBUS_BAUDRATE, SERIAL_8N1, MODBUS_RX_PIN, MODBUS_TX_PIN);    
    // Initialize pins
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(MODBUS_RW_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    
    // Initialize SPIFFS and load configuration
    if(!SPIFFS.begin(true)) {
        Serial.println("SPIFFS initialization failed!");
        return;
    }
    loadConfig();
    
    // Initialize ModbusRTU
    ModbusSerial.begin(9600, SERIAL_8N1, 16, 17);  // RX=16, TX=17
    mb.begin(&ModbusSerial);
    mb.master();

    // Connect to WiFi
    WiFi.begin(config.wifi_ssid, config.wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    // Setup NTP
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    
    // Setup web server
    setupWebServer();
}

void loop() {
    static unsigned long lastRead = 0;
    static unsigned long lastLog = 0;
    const unsigned long READ_INTERVAL = 1000;  // Read every second
    
    server.handleClient();
    
    // Read temperature
    if (millis() - lastRead >= READ_INTERVAL) {
        lastRead = millis();
        readTemperature();
        checkAlarm();
        
        // Log data if interval has passed
        if (millis() - lastLog >= (config.log_interval * 1000)) {
            logData(currentTemp);
            lastLog = millis();
        }
    }
    delay(1);
}

void readTemperature() {
    uint16_t result;
    if(mb.readHreg(MODBUS_ID, TEMPERATURE_REGISTER, &result, 1)) {
        return (float)((int16_t)result / 10.0);  // 값을 실제 온도로 변환 (예: 235 -> 23.5°C)
    }
    return -999.9;  // 에러 시 반환값
}

void checkAlarm() {
    if (currentTemp > ALARM_TEMP && !isAlarmActive) {
        isAlarmActive = true;
        triggerAlarm();
        addAlarmEvent();
    } else if (currentTemp <= ALARM_TEMP) {
        isAlarmActive = false;
        digitalWrite(BUZZER_PIN, LOW);
        digitalWrite(LED_PIN, LOW);
    }
}

void triggerAlarm() {
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
}

void addAlarmEvent() {
    AlarmEvent event;
    event.timestamp = time(nullptr);
    event.temperature = currentTemp;
    alarmEvents.push_back(event);
    saveAlarmEvents();
}