// GPS
#include <TinyGPS++.h>
TinyGPSPlus gps;

// GYRO
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
Adafruit_MPU6050 mpu;

// WIFI
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
ESP8266WiFiMulti WiFiMulti;
String deviceID = "1";
String serverAddress = "http://martorenzo.click/project/tree/server/";   // put slash at end
String wifiSSID = "TREEWIFI";
String wifiPass = "12345678";
bool isWifiConnected = false;

// INPUT
int batt100 = D5;
int batt50 = D6;
int batt25 = D7;
int flameSensor = A0;

// VAR
String gyroX = "0";
String gyroY = "0";
String gyroZ = "0";
String gpsSensorLat = "0";
String gpsSensorLang = "0";
String battVal = "25";
String flameVal = "0";


// START
// ==============================
void setup(void)
{
  // Serial
  Serial.begin(9600);

  // GYRO
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // WIFI
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(wifiSSID.c_str(), wifiPass.c_str());
  while (WiFiMulti.run() != WL_CONNECTED)
  {
    Serial.println("WIFI - CONNECTING TO " + wifiSSID);
  }

  // INPUT
  pinMode(batt100, INPUT_PULLUP);
  pinMode(batt50, INPUT_PULLUP);
  pinMode(batt25, INPUT_PULLUP);

  delay(100);
}


// LOOP
// ==============================
void loop()
{
  // RESET
  gyroX = "0";
  gyroY = "0";
  gyroZ = "0";
  gpsSensorLat = "0";
  gpsSensorLang = "0";
  battVal = "25";
  flameVal = "0";

  // GPS
  while (Serial.available() > 0){
    gps.encode(Serial.read());
    if (gps.location.isUpdated()){
      gpsSensorLat = String(gps.location.lat());
      gpsSensorLang = String(gps.location.lng());
    }
  }

  // GYRO
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  gyroX = String(g.gyro.x);
  gyroY = String(g.gyro.y);
  gyroZ = String(g.gyro.z);

  // BATTERY
  if (!batt25)
  {
    battVal = "25";
  }
  if (!batt50)
  {
    battVal = "50";
  }
  if (!batt100)
  {
    battVal = "100";
  }

  // FLAME
  flameVal = String(analogRead(flameSensor));

  Serial.println(gyroX);
  Serial.println(gyroY);
  Serial.println(gyroZ);
  Serial.println(gpsSensorLat);
  Serial.println(gpsSensorLang);
  Serial.println(battVal);
  Serial.println(flameVal);
  Serial.println("reseted===================================================");

  // SEND
  WiFiClient client;
  HTTPClient http;
  String serverUrl = serverAddress + "api.php?mode=devset&devgyx=" + gyroX + "&devgyy=" + gyroY + "&devgyz=" + gyroZ + "&devlat=" + gpsSensorLat + "&devlong=" + gpsSensorLang + "&devfire=" + flameVal + "&devbat=" + battVal + "&devid=" + deviceID;
  if (http.begin(client, serverUrl)) 
  {
    int httpCode = http.GET();
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();
        Serial.println(payload);
      }
    }
  }

  http.end();

  //
  delay(250);
}
