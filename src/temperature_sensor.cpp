#include "Arduino.h"
#include "temperature_sensor.h"
#include <WiFi.h>
#include <MQTT.h>

#include <Adafruit_Sensor.h>
#include "DHT.h"

#include <Adafruit_BME680.h>

unsigned long lastMillis = 0;

extern MQTTClient client;

/*#define BME_SCK 18
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 5*/

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 bme; // I2C
//Adafruit_BME680 bme(BME_CS); // hardware SPI
//Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);

#define DHTPIN 4 // Digital pin connected to the DHT sensor
// Uncomment whatever type you're using!
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);

// publish temperature every 10sec
void DHTReadAndPublish() {
  if (millis() - lastMillis > 5000)
  {
    lastMillis = millis();  // to reset timer

    // float temperature = dht.readTemperature();
    // if (isnan(temperature))
    // {
    //   Serial.println(F("Failed to read from DHT sensor!"));
    //   return;
    // }
    // Serial.print(F("%  Temperature: "));
    // Serial.print(temperature);
    // Serial.print(F("°C "));

    randomSeed(42);   
    float temperature = random(1,10);
    char DHTtemperatureString[8];
    dtostrf(temperature, 1, 2, DHTtemperatureString);

    #ifdef DEBUG
    Serial.print("Temperature: ");
    Serial.println(DHTtemperatureString);
    #endif // DEBUG

    // client.publish("interfer-esp32/temperature", DHTtemperatureString);

    
    bme680_routine();
  }
}

int bme680_setup() {
  Serial.println(F("BME680 async test"));

  if (!bme.begin()) {
    Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
    while (1);
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}

void bme680_set_fake_params() {
  randomSeed(12);   
  bme.temperature = random(27.0, 29.9);
  bme.pressure = random(1000.0, 1010.0);
  bme.humidity = random(30.0, 80.9);
  bme.gas_resistance = random(55.0, 65.9);

  // Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
}
void bme680_publish_params() {
  client.publish("interfer-esp32/temperature", String(bme.temperature));
  client.publish("interfer-esp32/pressure", String(bme.pressure));
  client.publish("interfer-esp32/humidity", String(bme.humidity));
  client.publish("interfer-esp32/gas-resistance", String(bme.gas_resistance));
}

void bme680_routine() {
  // Tell BME680 to begin measurement.
  // unsigned long endTime = bme.beginReading();
  // if (endTime == 0) {
  //   Serial.println(F("Failed to begin reading :("));
  //   return;
  // }
  // Serial.print(F("Reading started at "));
  // Serial.print(millis());
  // Serial.print(F(" and will finish at "));
  // Serial.println(endTime);

  // Serial.println(F("You can do other work during BME680 measurement."));
  // delay(50); // This represents parallel work.
  // // There's no need to delay() until millis() >= endTime: bme.endReading()
  // // takes care of that. It's okay for parallel work to take longer than
  // // BME680's measurement time.

  // // Obtain measurement results from BME680. Note that this operation isn't
  // // instantaneous even if milli() >= endTime due to I2C/SPI latency.
  // if (!bme.endReading()) {
  //   Serial.println(F("Failed to complete reading :("));
  //   return;
  // }

  bme680_set_fake_params();  // UNLESS USING REAL BME680

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
  // Serial.print(bme.gas_resistance / 1000.0);
  Serial.print(bme.gas_resistance);
  Serial.println(F(" KOhms"));

  // Serial.print(F("Approx. Altitude = "));
  // Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  // Serial.println(F(" m"));

  // Serial.println();
  // delay(2000);

  bme680_publish_params();
}