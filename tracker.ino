
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

/* wIFI authentication credentials */
const char *ssid = "<your_network_ssid>";
const char *password = "<your_network_password>";

/* host website */
const char *host = "opensky-network.org";
/* SHA 1 fingerprint for opensky-network.org */
const char fingerprint[] PROGMEM = "91 0B ED 3A 74 27 C8 D3 0D 58 69 71 2A 3A 99 BA FB 60 8A FE";
/* use HTTPS */
const int port = 443;
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.display();
  WiFi.mode(WIFI_OFF);
  delay(1000);
  WiFi.mode(WIFI_STA);

  /* connect to your local WIFI */
  WiFi.begin(ssid, password);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("Connected.");
  
  display.setTextSize(2); 
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(WiFi.localIP());
  display.display();
}


void loop() {
  WiFiClientSecure client;
  client.setFingerprint(fingerprint);
  client.setTimeout(15000);

  String json;
  if (client.connect(host, port)) {
    
    /* bbox coordinates */
    String lamax = "", lomin = "", lomax = "", lamin = "";
  
    String request = "/api/states/all?lamin=" + lamin + "&lomin=" + lomin + "&lamax=" + lamax + "&lomax=" + lomax;

    client.print(String("GET ") + request + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");


  }
  while (client.connected() || client.available()) {
    String line = client.readStringUntil('\n');
    if (line.startsWith("{")) json = line;
  }

  DynamicJsonDocument doc(1024);
  auto err = deserializeJson(doc, json);
  if (err) {
    Serial.println(err.c_str());
  }
  else {
    JsonArray states = doc["states"];
    for (JsonVariant state : states) {
      String callcode = state[1];
      Serial.println(callcode);
    }


  }
  delay(200000);
}
