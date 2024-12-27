#include <WiFi.h>
#include "wifi.h"
#include "credentials.h"

#define NUMBER_OF_ATTEMPTS 20

void Wifi::connect() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  for (uint8_t i = 0; i < NUMBER_OF_ATTEMPTS; i++) {
    if (WiFi.status() == WL_CONNECTED) {
      break;
    }
    delay(1000);
  }
}

bool Wifi::is_connected() {
  return WiFi.status() == WL_CONNECTED;
}

void Wifi::turn_off() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}