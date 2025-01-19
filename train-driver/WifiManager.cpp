#include "WifiManager.h"
#include <vector>

WifiManager::WifiManager(DNSServer* dnsServer, AsyncWebServer* server, Preferences* preferences) : _dnsServer(dnsServer), _server(server), _preferences(preferences) {};
WifiManager::~WifiManager() {}

bool WifiManager::autoConnect() {
  // Initialize Preferences
  _preferences->begin("wifi", false);

  // Retrieve saved SSID and Password
  String pass = _preferences->getString("pass", "");
  String ssid = _preferences->getString("ssid", "");
  return connect(ssid, pass);
}

void WifiManager::handleResetWifi(AsyncWebServerRequest* request) {
    _preferences->putString("ssid", "");
    _preferences->putString("pass", "");

    // Respond to client
    request->send(200, "text/plain", "WiFi credentials have been reset");

    // Give some time to send the response to client
    delay(2000);
    ESP.restart();
}

bool WifiManager::connect(String ssid, String pass) {
  // If we have credentials, attempt to connect
  if (ssid.length() > 0) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());

    Serial.print("Connecting to ");
    Serial.println(ssid);

    // Wait up to ~10 seconds to connect
    unsigned long startAttemptTime = millis();
    const unsigned long timeout = 10000;
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
      delay(500);
      Serial.print(".");
    }
    Serial.println();

    // Check if connected
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Wi-Fi connected!");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      // Since we are connected to a known wifi we do not need to create an AP.

      _server->on("/reset-wifi", HTTP_GET, std::bind(&WifiManager::handleResetWifi, this, std::placeholders::_1));

      return true;
    }
  }
  // If we fail to connect, or no credentials saved, start AP mode
  Serial.println("Failed to connect. Starting AP...");
  startAP();
  return false;
}

void WifiManager::startAP() {

  // Get the list of APs
  int numNetworks = WiFi.scanNetworks();

  std::vector<WiFiNetwork> networks;

  // Populate the vector with scan results
  for (int i = 0; i < numNetworks; i++) {
    String ssid = WiFi.SSID(i);
    int rssi = WiFi.RSSI(i);
    String encryption = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "Open" : "Secure";
    int channel = WiFi.channel(i);

    networks.push_back(WiFiNetwork(ssid, rssi, encryption, channel));

    Serial.print("Found SSID: ");
    Serial.println(ssid);
  }

  WiFi.scanDelete();

  // Start the Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  Serial.println("Started AP for configuration");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  _dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
  _dnsServer->start(DNS_PORT, F("*"), WiFi.softAPIP());

  // Serve a simple HTML form to enter new SSID and Password
  _server->on("/", HTTP_GET, [networks](AsyncWebServerRequest *request){
    // Basic HTML form
    String html = "<!DOCTYPE html><html><head>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1'/></head><body>";
    html += "<h2>Configure Wi-Fi</h2>";
    html += "<form action='/submit' method='POST'>";
    html += "SSID: <select name='ssid'>";

    for(const WiFiNetwork& network : networks) {
      html += "<option value='";
      html += network.ssid;
      html += "'>";
      html += network.ssid;
      html += "</option>";
      Serial.println(network.ssid);
    }

    html += "</select>";
    html += "<br><br>";
    html += "Password: <input type='password' name='pass'><br><br>";
    html += "<input type='submit' value='Save'>";
    html += "</form></body></html>";

    request->send(200, "text/html", html);
  });

  // Handler for form submission
  _server->on("/submit", HTTP_POST, [this](AsyncWebServerRequest *request){
    if (request->hasParam("ssid", true) && request->hasParam("pass", true)) {
      String newSsid = request->getParam("ssid", true)->value();
      String newPass = request->getParam("pass", true)->value();

      // Save to Preferences
      _preferences->putString("ssid", newSsid);
      _preferences->putString("pass", newPass);

      // Respond to client
      request->send(200, "text/plain", "Credentials saved. Rebooting...");

      // Give some time to send the response to client
      delay(1000);
      ESP.restart();
    } else {
      request->send(400, "text/plain", "Invalid form submission");
    }
  });

  _server->onNotFound([](AsyncWebServerRequest *request) {
    request->redirect("http://192.168.4.1/");  // Redirect to the AP IP address
  });
}

