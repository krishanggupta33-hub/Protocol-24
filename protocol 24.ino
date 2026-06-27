#include <WiFi.h>
#include "SinricPro.h"
#include "SinricProAirConditioner.h"

#define WIFI_SSID         "YOUR_WIFI_NAME"
#define WIFI_PASS         "YOUR_WIFI_PASSWORD"
#define APP_KEY           "YOUR_SINRIC_APP_KEY"
#define APP_SECRET        "YOUR_SINRIC_APP_SECRET"
#define AC_DEVICE_ID      "6a347e666ba33a80b97c6e83"

unsigned long ecoTimerStart = 0;       
bool ecoTimerActive = false;           
const unsigned long ONE_HOUR = 3600000;

const int IR_SEND_PIN = 2;

bool globalPowerState = false;
float globalTemperature = 24.0;

bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("Power state changed to %s\n", state ? "on" : "off");
  globalPowerState = state;
  
  if (state) {

  } else {

  }
  return true; 
}

bool onTargetTemperature(const String &deviceId, float &temperature) {
  Serial.printf("Target temperature set to: %.1f\n", temperature);
  globalTemperature = temperature;

  if (temperature < 24.0) {
    Serial.println("Warning: Temp below 24. 1-Hour Eco Timer started.");
    ecoTimerActive = true;       
    ecoTimerStart = millis();    
  } else {
    ecoTimerActive = false;      
    Serial.println("Eco mode bypassed. Timer cancelled.");
  }

  return true;
}

void setup() {
  Serial.begin(115200);

  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  WiFi.setSleep(false);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected!");

  SinricProAirConditioner &myAc = SinricPro[AC_DEVICE_ID];
  myAc.onPowerState(onPowerState);
  myAc.onTargetTemperature(onTargetTemperature);

  SinricPro.onConnected([](){ Serial.println("Connected to Sinric Pro Cloud."); });
  SinricPro.onDisconnected([](){ Serial.println("Disconnected from Cloud."); });
  SinricPro.begin(APP_KEY, APP_SECRET);
}

void loop() {
  SinricPro.handle();

  if (ecoTimerActive && (millis() - ecoTimerStart >= ONE_HOUR)) {
    Serial.println("1 hour elapsed! Reverting AC to 24°C Eco-Mode.");
    
    ecoTimerActive = false; 
    globalTemperature = 24.0;

    SinricProAirConditioner &myAc = SinricPro[AC_DEVICE_ID];
    myAc.sendTargetTemperatureEvent(24.0); 
  }
}
