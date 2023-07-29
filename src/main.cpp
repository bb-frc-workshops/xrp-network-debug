#include <Arduino.h>
#include <WebServer.h>
// #include <WebSockets4WebServer.h>

#include <WebSocketsServer.h>
#include <WiFi.h>

#define BLINK_TIME_MS 500
#define STAT_PRINT_TIME_MS 5000

// WebServer webServer(3300);
// WebSockets4WebServer wsServer;

WebSocketsServer webSocketsServer(3300);

unsigned long _wsMessageCount = 0;

// void handleIndexRoute() {
//   webServer.send(200, "text/plain", "Oh hi");
// }

void handleWSEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[NET:WS] [%u] Disconnected\n", num);
      break;
    case WStype_CONNECTED: {
        // IPAddress ip = wsServer.remoteIP(num);
        IPAddress ip = webSocketsServer.remoteIP(num);
        Serial.printf("[NET:WS] [%u] Connection from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;
    case WStype_TEXT: {
      _wsMessageCount++;
        // StaticJsonDocument<512> jsonDoc;
        // DeserializationError error = deserializeJson(jsonDoc, payload);
        // if (error) {
        //   Serial.println(error.f_str());
        //   break;
        // }

        // _wsMessageCount++;
        // wpilibws::processWSMessage(jsonDoc);
      }
      break;
  }
}

unsigned long _lastBlinkTime;
bool _blinkState;
void checkAndBlink() {
  if (millis() - _lastBlinkTime > BLINK_TIME_MS) {
    _blinkState = !_blinkState;
    digitalWrite(LED_BUILTIN, _blinkState ? HIGH : LOW);
    _lastBlinkTime = millis();
  }
}

unsigned long _lastStatusPrintTime;
void checkAndPrintStatus() {
  if (millis() - _lastStatusPrintTime > STAT_PRINT_TIME_MS) {
    // int numConnectedClients = wsServer.connectedClients();
    int numConnectedClients = webSocketsServer.connectedClients();
    int usedHeap = rp2040.getUsedHeap();
    Serial.printf("[STAT] t:%u c:%d h:%d msg:%u\n", millis(), numConnectedClients, usedHeap, _wsMessageCount);
    _lastStatusPrintTime = millis();
  }
}

void setup() {
  Serial.begin(115200);

  delay(2000);

  // Busy loop if there-s no WiFi hardware
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("[NET] No WiFi module");
    while (true);
  }

  WiFi.setHostname("XRP");
  bool result = WiFi.softAP("XRP", "0123456789");
  if (result) {
    Serial.println("[NET] WiFi AP Ready");
  }
  else {
    Serial.println("[NET] AP setup FAILED");
    while (true);
  }

  // Set up HTTP server routes
  // Serial.println("[NET] Setting up Web Server routes");
  // webServer.on("/", handleIndexRoute);

  // // Set up WS routing
  // Serial.println("[NET] Setting up WebSocket routing");
  // webServer.addHook(wsServer.hookForWebserver("/wpilibws", handleWSEvent));

  // Serial.println("[NET] Starting Web Server on port 3300");
  // webServer.begin();

  webSocketsServer.onEvent(handleWSEvent);
  webSocketsServer.begin();

  Serial.printf("[NET] SSID: %s\n", WiFi.SSID().c_str());
  Serial.printf("[NET] IP: %s\n", WiFi.localIP().toString().c_str());

  _blinkState = true;
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  _lastBlinkTime = millis();
  _lastStatusPrintTime = millis();
}

void loop() {
  // webServer.handleClient();

  Serial.println("===== WS - Loop - START ======");
  // wsServer.loop();
  webSocketsServer.loop();
  Serial.println("===== WS - Loop - END ======");

  checkAndBlink();
  checkAndPrintStatus();

  // delay(5);
}
