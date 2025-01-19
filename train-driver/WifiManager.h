#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <DNSServer.h>
#include <Preferences.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

// Change these to your desired fallback AP credentials if Wi-Fi fails
#define AP_SSID "ESP32_Config_AP"
#define AP_PASSWORD "12345678"  // Must be at least 8 chars for WPA2

#define DNS_PORT 53

class WifiManager
{
public:
    WifiManager(DNSServer *dnsServer, AsyncWebServer *server, Preferences *preferences);
    ~WifiManager();

    bool autoConnect();

private:
    DNSServer* _dnsServer;
    AsyncWebServer* _server;
    Preferences* _preferences;

    bool connect(String ssid, String pass);
    void startAP();
    void handleResetWifi(AsyncWebServerRequest* request);
};

struct WiFiNetwork {
  String ssid;
  int rssi;
  String encryption;
  int channel;

  // Constructor
  WiFiNetwork(String s, int r, String e, int c)
      : ssid(s), rssi(r), encryption(e), channel(c) {}
};

#endif // WIFI_MANAGER_H
