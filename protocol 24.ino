#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SinricPro.h>
#include <SinricProWindowAC.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <ir_Voltas.h>

#define WIFI_SSID         "Jio_O_1108_4G"
#define WIFI_PASS         "12345678"
#define APP_KEY           "84bddb98-6666-4f5f-9073-00882ab8e60f"
#define APP_SECRET        "84dafc93-32d4-488e-8598-d2f803707531-5d343841-eb8a-488a-867c-acc89409dfaa"
#define AC_DEVICE_ID      "6a347e666ba33a80b97c6e83"

const uint16_t kRecvPin = 14;
const uint16_t kIrLed = 4;

IRrecv irrecv(kRecvPin, 1024, 50, true);
decode_results results;
IRVoltas ac(kIrLed);

unsigned long ecoTimerStart = 0;
bool ecoTimerActive = false;
const unsigned long ONE_HOUR = 3600000;

bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("Google says turn AC %s\n", state ? "on" : "off");
  if (state) {
    ac.on();
  } else {
    ac.off();
    ecoTimerActive = false;
  }
  ac.send();
  return true;
}

bool onTargetTemperature(const String &deviceId, float &temperature) {
  int targetTemp = (int)temperature;
  Serial.printf("Google set temp to %d\n", targetTemp);

  if (targetTemp < 24) {
    if (!ecoTimerActive) {
      Serial.println("below 24, starting eco timer");
      ecoTimerActive = true;
    }
    ecoTimerStart = millis();
  } else {
    Serial.println("temp is fine, timer cleared");
    ecoTimerActive = false;
  }

  ac.setTemp(targetTemp);
  ac.on();
  ac.send();
  return true;
}

void setup() {
  Serial.begin(115200);
  irrecv.enableIRIn();

  ac.begin();
  ac.on();
  ac.setFan(kVoltasFanAuto);
  ac.setMode(kVoltasCool);
  ac.setTemp(24);

  Serial.print("connecting to wifi");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nconnected");

  SinricProWindowAC &myAC = SinricPro[AC_DEVICE_ID];
  myAC.onPowerState(onPowerState);
  myAC.onTargetTemperature(onTargetTemperature);

  SinricPro.onConnected([](){ Serial.println("connected to sinric"); });
  SinricPro.begin(APP_KEY, APP_SECRET);
}

void loop() {
  SinricPro.handle();

  if (irrecv.decode(&results)) {
    if (results.decode_type == VOLTAS) {
      int capturedTemp = results.state[3];
      Serial.print("caught remote signal, temp ");
      Serial.println(capturedTemp);

      if (capturedTemp < 24) {
        if (!ecoTimerActive) ecoTimerActive = true;
        ecoTimerStart = millis();
      } else {
        ecoTimerActive = false;
      }
    }
    irrecv.resume();
  }

  if (ecoTimerActive && (millis() - ecoTimerStart >= ONE_HOUR)) {
    Serial.println("hour's up, forcing AC back to 24");

    ecoTimerActive = false;

    ac.setTemp(24);
    ac.on();
    ac.send();

    SinricProWindowAC &myAC = SinricPro[AC_DEVICE_ID];
    myAC.sendTargetTemperatureEvent(24.0);
  }
}
