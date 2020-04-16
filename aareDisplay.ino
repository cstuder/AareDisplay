/**
 * Display Aare temperature on a screen
 * 
 * Digit selectors D1-D4 are connected on GPA0-GPA3
 * 
 * Segments are connected on GPB0-GPB8
 */
#include <ESP8266WiFi.h>       // ESP8266 Core WiFi Library
#include <DNSServer.h>         // Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>  // Local WebServer used to serve the configuration portal
#include <WiFiManager.h>       // https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ESP8266HTTPClient.h> // GET client

#include <Wire.h> // I2C bus library

#include <ArduinoJson.h> // Decode JSON responses

// Local config
String API_URL = "http://aare.schwumm.ch/api/current?app=ch.existenz.aaredisplay&version=1.0.0 ";
String API_temperaturkey = "temperature";
int DTime = 4;            // [ms] Display refresh time
float temperature = 0.0;  // [°C] Temperature value cache
long mydelay = 10 * 1000; // [ms] Time between updates from webservice // TODO: Switch to 5 min
long timer = millis();    // Global refresh timer

void setup()
{
  // Setup serial port and I2C bus
  Serial.begin(115200);

  pinMode(16, OUTPUT); // Turn on internal LED
  digitalWrite(16, LOW);

  Wire.pins(5, 4); // Corresponds to D2, D1
  Wire.begin();    // Defaults to address 0

  Wire.beginTransmission(0x20);
  Wire.write(0x00); // A Port
  Wire.write(0x00); // Set entire port to output
  Wire.endTransmission();

  Wire.beginTransmission(0x20);
  Wire.write(0x01); // B Port
  Wire.write(0x00); // Set entire port to output
  Wire.endTransmission();

  Wire.beginTransmission(0x20);
  Wire.write(0x13);      // B Port
  Wire.write(B10000000); // Show dots everywhere
  Wire.endTransmission();

  Wire.beginTransmission(0x20);
  Wire.write(0x12);      // A Port
  Wire.write(B00000000); // Turn on all digits
  Wire.endTransmission();

  // Setup WiFi
  WiFiManager wifiManager;
  wifiManager.autoConnect("AareDisplay");

  // Output WiFi information
  Serial.print("Connected to WiFi: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Using API: ");
  Serial.println(API_URL);
  digitalWrite(16, HIGH); // Turn off connection LED
}

void loop()
{
  if (millis() > timer)
  { // Fetch new value from API
    if (WiFi.status() == WL_CONNECTED)
    {
      HTTPClient http;

      http.begin(API_URL);
      int httpCode = http.GET();

      if (httpCode > 0)
      {
        String payload = http.getString();
        Serial.println(payload);

        // Parse JSOn
        StaticJsonBuffer<500> jsonBuffer;
        JsonObject &root = jsonBuffer.parseObject(payload);
        temperature = root[API_temperaturkey];
        Serial.println(temperature);
      }

      http.end();
    }

    timer = millis() + mydelay;
  }

  showValue(temperature);
}

/**
 * Show float value
 * 
 * Note: This is sometimes off by a hundreth degree...
 * 
 * @param float value
 */
void showValue(float value)
{
  int newValue = (value)*100;
  int hundrets = newValue % 10;
  int tenths = (newValue / 10) % 10;
  int ones = (newValue / 100) % 10;
  int tenners = (newValue / 1000) % 10;

  if (tenners > 0)
  {
    showNumber(4, tenners, false);
  }
  else
  {
    showNothing(4);
  }

  showNumber(3, ones, true);
  showNumber(2, tenths, false);
  showNumber(1, hundrets, false);
}

/**
 * Show number on a single digit
 * 
 * @param int digit (1-4)
 * @param int x
 * @param bool dot
 */
void showNumber(int digit, int x, bool dot)
{
  showPattern(digit, pickNumber(x, dot));
}

/**
 * Show nothing here
 * 
 * @param int digit (1-4)
 */
void showNothing(int digit)
{
  showPattern(digit, 0);
}

/**
 * Show this bit pattern
 * 
 * @param int digit [1-4]
 * @param int pattern
 */
void showPattern(int digit, int pattern)
{
  int digitBits = 0;
  bitSet(digitBits, digit - 1);

  Wire.beginTransmission(0x20);
  Wire.write(0x13);    // B Port
  Wire.write(pattern); // Set pattern
  Wire.endTransmission();

  Wire.beginTransmission(0x20);
  Wire.write(0x12);       // A Port
  Wire.write(~digitBits); // Turn on this digit
  Wire.endTransmission();

  delay(DTime);

  Wire.beginTransmission(0x20);
  Wire.write(0x12); // A Port
  Wire.write(0x0F); // Turn off all digits
  Wire.endTransmission();
}

/**
 * Return pinout for this number, including the dot
 * 
 * @param int x
 * @param bool dot
 * @return int
 */
int pickNumber(int x, bool dot)
{
  byte r = B0;

  switch (x)
  { //.GFEDCBA
  case 1:
    r = B00000110;
    break;
  case 2:
    r = B01011011;
    break;
  case 3:
    r = B01001111;
    break;
  case 4:
    r = B01100110;
    break;
  case 5:
    r = B01101101;
    break;
  case 6:
    r = B01111101;
    break;
  case 7:
    r = B00000111;
    break;
  case 8:
    r = B01111111;
    break;
  case 9:
    r = B01101111;
    break;
  default:
    r = B00111111;
    break;
  }

  if (dot)
  {
    r = r | B10000000;
  }

  return r;
}
