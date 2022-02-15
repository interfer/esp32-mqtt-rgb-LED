#include <WiFi.h>
#include <MQTT.h>

#define RED_PIN 5
#define GREEN_PIN 19
#define BLUE_PIN 21

const char ssid[] = "HomeSweetHome";
const char pass[] = "Mart1rosjana13";

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

uint8_t red;   // Red value for RGB
uint8_t green; // Green value for RBG
uint8_t blue;  // Blue value for RGB

void connect()
{
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("\nconnecting...");
  while (!client.connect("esp32", "", ""))
  {
    Serial.print(".");
    client.disconnect();
    delay(2000);
  }

  Serial.println("\nconnected!");

  client.subscribe("esp32");
  client.subscribe("esp32/color");
}
struct StringParser
{
  void reset()
  {
    from = to = -1;
  }
  bool update(String &s, char div = ',')
  {
    if (to == s.length())
      return 0;
    from = to + 1;
    to = s.indexOf(div, from);
    if (to < 0)
      to = s.length();
    str = s.substring(from, to);
    return 1;
  }
  String str;
  int from = -1;
  int to = -1;
};

void setColorRGB()
{
  digitalWrite(RED_PIN, red);
  digitalWrite(GREEN_PIN, green);
  digitalWrite(BLUE_PIN, blue);
}

void actionOnMessage(String topic_, String payload_)
{
  Serial.print("TOPIC:  ");
  Serial.println(topic_);
  Serial.print("PAYLOAD:  ");
  Serial.println(payload_);
  if (payload_ == "on")
  {
    Serial.println("LED IS ON");
  }
  if (payload_ == "off")
  {
    Serial.println("LED IS OFF");
  }
  // to "(..., ..., ...)"

  uint8_t indexA = payload_.indexOf('(');
  uint8_t indexZ = payload_.indexOf(')');
  String numString = payload_.substring(indexA + 1, indexZ);
  Serial.print("Cutted string:  ");
  Serial.println(numString);

  // to get RED value from RGB JSON
  uint8_t indexFirstComma = numString.indexOf(',');
  String redString = numString.substring(0, indexFirstComma);
  redString.trim();
  Serial.print("redString:  ");
  Serial.println(redString); // here add toInt()
  red = redString.toInt();
  // to get GREEN value from RGB JSON
  numString.remove(0, indexFirstComma + 2);
  Serial.print("GreenString & Blue:  ");
  Serial.println(numString); // here add toInt() 68, 60
  uint8_t indexSecondComma = numString.indexOf(',');
  String greenString = numString.substring(0, indexSecondComma);
  // greenString.trim();
  Serial.print("greenString:  ");
  Serial.println(greenString); // here add toInt()
  green = greenString.toInt();
  // to get BLUE value from RGB JSON
  numString.remove(0, indexSecondComma + 2);
  Serial.print("Blue:  ");
  Serial.println(numString); //
  blue = numString.toInt();
  // greenString.trim();
  numString = "";

  Serial.println("Colors RGB:");
  Serial.println(red);
  Serial.println(green);
  Serial.println(blue);
  
  // and let's change the color due to RBG palette from MQTT
  setColorRGB();
}

void messageReceived(String &topic, String &payload)
{
  Serial.println("incoming: " + topic + " - " + payload);
  actionOnMessage(topic, payload);
}

void setup()
{
  Serial.begin(115200);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  WiFi.begin(ssid, pass);

  client.begin("broker.emqx.io", net);
  client.onMessage(messageReceived);

  connect();
}

void loop()
{
  client.loop();
  delay(10); // <- fixes some issues with WiFi stability
  if (!client.connected())
  {
    connect();
  }
  // publish a message roughly every second.
  // if (millis() - lastMillis > 10000)
  // {
  //   lastMillis = millis();
  //   client.publish("esp32", "on");
  // }
}