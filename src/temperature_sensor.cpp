#include "Arduino.h"
#include <WiFi.h>
#include <MQTT.h>

#include <Adafruit_Sensor.h>
#include "DHT.h"

unsigned long lastMillis = 0;

extern MQTTClient client;

#define DHTPIN 4 // Digital pin connected to the DHT sensor
// Uncomment whatever type you're using!
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);

// publish temperature every 10sec
void DHTReadAndPublish() {
  if (millis() - lastMillis > 10000)
  {
    lastMillis = millis();

    float temperature = dht.readTemperature();
    if (isnan(temperature))
    {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    Serial.print(F("%  Temperature: "));
    Serial.print(temperature);
    Serial.print(F("°C "));

    char DHTtemperatureString[8];
    dtostrf(temperature, 1, 2, DHTtemperatureString);

    #ifdef DEBUG
    Serial.print("Temperature: ");
    Serial.println(DHTtemperatureString);
    #endif // DEBUG

    client.publish("esp32/temperature", DHTtemperatureString);
  }
}