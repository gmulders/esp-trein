/*
 * Minimal example of Wi-Fi configuration with fallback AP and form using ESPAsyncWebServer
 * 
 * Libraries needed:
 *   - ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
 *   - AsyncTCP for ESP32 (https://github.com/me-no-dev/AsyncTCP)
 */

#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>
// #include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include "WifiManager.h"
#include "DriverManager.h"

Preferences preferences;
AsyncWebServer server(80);
DNSServer dnsServer;

// Initial configuration
std::vector<DriverConfig> initialConfig = {
    {DriverType::MOTOR_WITH_DIRECTION, "motor1", {5, 6}},
    {DriverType::PIN_ON_OFF, "led1", {7}},
//    {DriverType::FAST_LED_STRIP, "ledStrip1", {8}, 30}
};

WifiManager wifiManager(&dnsServer, &server, &preferences);
DriverManager driverManager(initialConfig);

void setup() {
  Serial.begin(115200);
  delay(1000);


  wifiManager.autoConnect();

  // Start server
  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
}
