#include "Arduino.h"
#include "temperature_sensor.h"
#include <utils.h>

#include <WiFi.h>
#include <MQTT.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>

#define TIMEOUT 5000
unsigned long lastMillis = 0;

extern MQTTClient client;

/*#define BME_SCK 18
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 5*/

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 bme; // I2C
// Adafruit_BME680 bme(BME_CS); // hardware SPI
// Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);

int bme680_setup()
{
  Serial.println(F("BME680 async test"));

  if (!bme.begin())
  {
    Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
    return 1;
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

  return 0;
}

void bme680_set_fake_params()
{
  randomSeed(12);

  /* NOTE: random() returns long int type, so second usage of it is to add decimals to result */
  bme.temperature = float(random(26, 30) + random(0, 100) / 100.0);
  // Serial.printf("|temperature| median_filter = (%d)\n", median_filter(int(bme.temperature)));
  bme.pressure = float(random(1000, 1010) + random(0, 100) / 100.0) / 10; // kPa
  bme.humidity = median_filter(random(50, 70));
  bme.gas_resistance = float(random(55, 65) + random(0, 100) / 100.0); // kOhms

}

void bme680_publish_params()
{
  client.publish("monitoring-esp32/temperature", String(bme.temperature));
  client.publish("monitoring-esp32/pressure", String(bme.pressure));
  client.publish("monitoring-esp32/humidity", String(bme.humidity));
  client.publish("monitoring-esp32/gas-resistance", String(bme.gas_resistance));
}

void bme680_routine()
{
  if (millis() - lastMillis > TIMEOUT)
  {
    lastMillis = millis();    // to reset timer

    bme680_set_fake_params(); // UNLESS USING REAL BME680

    Serial.print(F("Reading completed at "));
    Serial.println(millis());

    Serial.print(F("Temperature = "));
    Serial.print(bme.temperature);
    Serial.println(F(" *C"));

    Serial.print(F("Pressure = "));
    // Serial.print(bme.pressure / 100.0);
    Serial.print(bme.pressure);
    Serial.println(F(" hPa"));

    Serial.print(F("Humidity = "));
    Serial.print(bme.humidity);
    Serial.println(F(" %"));

    Serial.print(F("Gas = "));
    Serial.print(bme.gas_resistance);
    Serial.println(F(" KOhms"));

    bme680_publish_params();
  }
}