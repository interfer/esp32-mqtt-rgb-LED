#include <WiFi.h>
#include <MQTT.h>

#include <Adafruit_Sensor.h>
#include "DHT.h"

#include <Adafruit_BME680.h>

#include "temperature_sensor.h"
#include "rgb_led_routine.h"

// #define DEBUG // uncomment to see debug output 

const char ssid[] = "HomeNet";
const char pass[] = "88888888";

#define BROKER_URI "broker.emqx.io"

WiFiClient net;
MQTTClient client;
extern DHT dht;


#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5        /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void sleep_setup() {
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  print_wakeup_reason();

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
  " Seconds");

  Serial.println("Going to sleep now");
  delay(1000);
  Serial.flush(); 
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

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
  while (!client.connect("interfer-esp32", "", "")) {
    Serial.print(".");
    client.disconnect();
    delay(2000);
  }

  Serial.println("connected!");

  client.subscribe("interfer-esp32");
  client.subscribe("interfer-esp32/color");
}

void actionOnMessage(String topic_, String payload_) {
  #ifdef DEBUG
  Serial.print("TOPIC:  ");
  Serial.println(topic_);
  Serial.print("PAYLOAD:  ");
  Serial.println(payload_);
  #endif // DEBUG

  // string payload from "(..., ..., ...)"
  // uint8_t red;   // Red value for RGB
  // uint8_t green; // Green value for RBG
  // uint8_t blue;  // Blue value for RGB

  // TrimPayloadToRGB(payload_, &red, &green, &blue);

  // setColorRGB(red, green, blue);
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

  // dht.begin();

  // initPWM();

  WiFi.begin(ssid, pass);

  client.begin(BROKER_URI, net);
  client.onMessage(messageReceived);

  connect();
  // sleep_setup();
}

void loop() {
  client.loop();
  delay(10); // <- fixes some issues with WiFi stability
  if (!client.connected()) {
    connect();
  }
  DHTReadAndPublish();
  // bme680_routine();
}




  /*
  Next we decide what all peripherals to shut down/keep on
  By default, ESP32 will automatically power down the peripherals
  not needed by the wakeup source, but if you want to be a poweruser
  this is for you. Read in detail at the API docs
  http://esp-idf.readthedocs.io/en/latest/api-reference/system/deep_sleep.html
  Left the line commented as an example of how to configure peripherals.
  The line below turns off all RTC peripherals in deep sleep.
  */
  //esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  //Serial.println("Configured all RTC Peripherals to be powered down in sleep");

  /*
  Now that we have setup a wake cause and if needed setup the
  peripherals state in deep sleep, we can now start going to
  deep sleep.
  In the case that no wake up sources were provided but deep
  sleep was started, it will sleep forever unless hardware
  reset occurs.
  */