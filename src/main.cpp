#include <WiFi.h>
#include <MQTT.h>

#include <Adafruit_Sensor.h>
#include "DHT.h"

#include "temperature_sensor.h"
#include "rgb_led_routine.h"

// #define DEBUG // uncomment to see debug output 

const char ssid[] = "YOUR_WIFI_SSID";
const char pass[] = "YOUR_WIFI_PASS";

#define BROKER_URI "broker.emqx.io"

WiFiClient net;
MQTTClient client;
extern DHT dht;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  #ifdef DEBUG
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  #endif // DEBUG

  Serial.println("connecting...");
  while (!client.connect("esp32", "", "")) {
    Serial.print(".");
    client.disconnect();
    delay(2000);
  }

  Serial.println("connected!");

  client.subscribe("esp32");
  client.subscribe("esp32/color");
}

void actionOnMessage(String topic_, String payload_) {
  #ifdef DEBUG
  Serial.print("TOPIC:  ");
  Serial.println(topic_);
  Serial.print("PAYLOAD:  ");
  Serial.println(payload_);
  #endif // DEBUG

  // string payload from "(..., ..., ...)"
  uint8_t red;   // Red value for RGB
  uint8_t green; // Green value for RBG
  uint8_t blue;  // Blue value for RGB

  TrimPayloadToRGB(payload_, &red, &green, &blue);

  setColorRGB(red, green, blue);
}

// callback on incoming message
void messageReceived(String &topic, String &payload) {
  #ifdef DEBUG
  Serial.println("incoming: " + topic + " - " + payload);
  #endif // DEBUG

  actionOnMessage(topic, payload);
}

void setup() {
  Serial.begin(115200);

  dht.begin();

  initPWM();

  WiFi.begin(ssid, pass);

  client.begin(BROKER_URI, net);
  client.onMessage(messageReceived);

  connect();
}

void loop() {
  client.loop();
  delay(10); // <- fixes some issues with WiFi stability
  if (!client.connected()) {
    connect();
  }
  DHTReadAndPublish();
}
