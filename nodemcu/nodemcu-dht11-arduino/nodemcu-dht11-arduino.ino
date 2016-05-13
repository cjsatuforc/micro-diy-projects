/**
 * Setup:
 *  GPIO04 - LDR Power
 *  GPIO05 - LED
 *  GPIO14 - DHT Data
 * 
 * for deep sleep,
 *  GPIO16 - RST
 *  GPIO00 - Pull Up - 4.7K - VCC (not really needed)
 *  GPIO15 - Pull Down - 4.7K GND (not really needed)
 */

#include <DHT.h>
#include <ESP8266WiFi.h>

extern "C" {
  #include "user_interface.h"
}

#define DHT_PIN 14
#define DHT_TYPE DHT11

#define ADC_PIN A0
#define LED_PIN 5

#define ADC_POWER_PIN 4
#define ADC_SAMPLES 100

#define READINGS_DELAY 5*60*1000

// Fill in your details here
const char* MY_SSID = "...";
const char* MY_PWD = "...";

const String THINGSPEAK_CHANNEL_ID = "...";
const String THINGSPEAK_API_KEY = "...";

DHT dht(DHT_PIN, DHT_TYPE);

struct READING {
  boolean valid;
  float t;
  float h;
  int light;
};

void setup() {
  Serial.begin(9600);

  pinMode(ADC_POWER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  Serial.println("Started...");
  connectWifi();
}

void loop() {
  READING reading = readData();

  Serial.print("T: ");
  Serial.print(reading.t);
  Serial.print(" H: ");
  Serial.print(reading.h);
  Serial.print(" L: ");
  Serial.println(reading.light);

  sendData(reading);

  system_deep_sleep_set_option(0);
  system_deep_sleep(READINGS_DELAY * 1000);  
}

void connectWifi()
{
  Serial.print("Connecting to ");
  Serial.println(MY_SSID);
  WiFi.begin(MY_SSID, MY_PWD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
}

READING readData() {
  READING reading;

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    reading.valid = false;
  }
  else {
    reading.valid = true;
    reading.h = h;
    reading.t = t;
  }

  reading.light = readLightLevel(ADC_POWER_PIN, ADC_SAMPLES);

  return reading;
}

int readLightLevel(int powerPin, int samples) {
  digitalWrite(powerPin, HIGH);
  int total = 0;
  for (int i = 0; i < samples; i++) {
    total += analogRead(ADC_PIN);
  }
  digitalWrite(powerPin, LOW);
  return total / samples;
}

void sendData(READING reading) {
  if (reading.valid == false) {
    Serial.println("Discarding reading since it's not valid.");
    return;
  }

  WiFiClient client;

  if (client.connect("api.thingspeak.com", 80)) { // use ip 184.106.153.149 or api.thingspeak.com
    Serial.println("connected to ThingSpeak");

    String postStr = "api_key=" + THINGSPEAK_API_KEY;
    postStr += "&field1=";
    postStr += String(reading.t);
    postStr += "&field2=";
    postStr += String(reading.h);
    postStr += "&field3=";
    postStr += String(reading.light);
    postStr += "\r\n\r\n";

    String location = "POST /update HTTP/1.1\n";
    Serial.println(location);
    Serial.println(postStr);

    client.print(location);
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    client.flush();

    // Read all the lines of the reply from server and print them to Serial
    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }

    Serial.println();
    Serial.println("closing connection");
  }//end if

  client.stop();

  digitalWrite(LED_PIN, HIGH);
  delay(50);
  digitalWrite(LED_PIN, LOW);
}
