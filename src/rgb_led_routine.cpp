#include <Arduino.h>
#include "rgb_led_routine.h"

// Red, green, and blue pins for PWM control
const int redPin = 5;    // 13 corresponds to GPIO13
const int greenPin = 19; // 12 corresponds to GPIO12
const int bluePin = 21;  // 14 corresponds to GPIO14

// Setting PWM frequency, channels and bit resolution
const int freq = 5000;
const int redChannel = 0;
const int greenChannel = 1;
const int blueChannel = 2;
// Bit resolution 2^8 = 256
const int resolution = 8;

void initPWM() {
  ledcSetup(redChannel, freq, resolution);
  ledcSetup(greenChannel, freq, resolution);
  ledcSetup(blueChannel, freq, resolution);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(redPin, redChannel);
  ledcAttachPin(greenPin, greenChannel);
  ledcAttachPin(bluePin, blueChannel);
}

void setColorRGB(uint8_t red, uint8_t green, uint8_t blue) {
  #ifdef DEBUG
  Serial.println("Set colors:  ");
  Serial.println(red);
  Serial.println(green);
  Serial.println(blue);
  #endif // DEBUG

  ledcWrite(redChannel, red);
  ledcWrite(greenChannel, green);
  ledcWrite(blueChannel, blue);
}

//JSON payload string-to-int type casting
void TrimPayloadToRGB(String payload_, uint8_t* red, uint8_t* green, uint8_t* blue) {
  uint8_t indexA = payload_.indexOf('(');
  uint8_t indexZ = payload_.indexOf(')');
  String numString = payload_.substring(indexA + 1, indexZ);

  #ifdef DEBUG
  Serial.print("Cutted string:  ");
  Serial.println(numString);
  #endif // DEBUG

  // to get RED value from RGB JSON
  uint8_t indexFirstComma = numString.indexOf(',');
  String redString = numString.substring(0, indexFirstComma);
  redString.trim();

  #ifdef DEBUG
  Serial.print("redString:  ");
  Serial.println(redString);
  #endif // DEBUG

  *red = redString.toInt();
  // to get GREEN value from RGB JSON
  numString.remove(0, indexFirstComma + 2);

  #ifdef DEBUG
  Serial.print("GreenString & Blue:  ");
  Serial.println(numString); 68, 60
  #endif // DEBUG

  uint8_t indexSecondComma = numString.indexOf(',');
  String greenString = numString.substring(0, indexSecondComma);

  #ifdef DEBUG
  Serial.print("greenString:  ");
  Serial.println(greenString);
  #endif // DEBUG

  *green = greenString.toInt();
  // to get BLUE value from RGB JSON
  numString.remove(0, indexSecondComma + 2);

  #ifdef DEBUG
  Serial.print("Blue:  ");
  Serial.println(numString);
  #endif // DEBUG

  *blue = numString.toInt();
  numString = "";

  #ifdef DEBUG
  Serial.println("Ints from String RGB:");
  Serial.println(red);
  Serial.println(green);
  Serial.println(blue);
  #endif // DEBUG
}