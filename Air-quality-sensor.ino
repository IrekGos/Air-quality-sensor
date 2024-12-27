#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <esp_system.h>
#include "measurements.h"
#include "dust_sensor.h"
#include "wifi.h"
#include "credentials.h"

#define DEBUG_MODE false

#define NUMBER_OF_ATTEMPTS 10
#define uS_TO_S_FACTOR 1000000

const uint64_t TIME_TO_SLEEP = (uint64_t)uS_TO_S_FACTOR * 60 * 60;
const char* URL = "https://api.thingspeak.com/update.json";

HTTPClient https;
Measurements measurements;
DustSensor dustsensor;
Wifi wifi;

void signal_start() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);
}

bool send_data(JsonDocument data) {
  data["api_key"] = API_KEY;
  String requestBody;
  serializeJson(data, requestBody);
  int httpsResponseCode = https.POST(requestBody);
  if (DEBUG_MODE)
    Serial.printf("HTTP Response Code: %d\n", httpsResponseCode);
  return httpsResponseCode == HTTP_CODE_OK;
}

void setup() {
  signal_start();
  if (DEBUG_MODE) {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    delay(100);
    Serial.println("Started");
  }

  if (!https.begin(URL, THINGSPEAK_CERTIFICATE)) {
    esp_restart();
  }
  https.addHeader("Content-Type", "application/json");

  measurements.configure();
  dustsensor.dust_sensor_init();
}

void deep_sleep(uint64_t time) {
  esp_sleep_enable_timer_wakeup(time);
  esp_deep_sleep_start();
}

void loop() {
  JsonDocument dust_results, results;
  uint8_t successful_readings = dustsensor.measure_air_quality(dust_results);
  if (successful_readings) {
    results["field1"] = dust_results["PM1"];
    results["field2"] = dust_results["PM2.5"];
    results["field3"] = dust_results["PM10"];
    results["field7"] = successful_readings;
  }

  float internal_temp = measurements.measure_internal_temp();
  float ambient_temp = measurements.measure_ambient_temp();
  float supply_voltage = measurements.measure_supply_voltage();

  results["field4"] = ambient_temp;
  results["field5"] = internal_temp;
  results["field6"] = supply_voltage;

  if (DEBUG_MODE) {
    delay(500);
    Serial.println("results:");
    serializeJson(results, Serial);
    Serial.println("");
    Serial.flush();
  }

  for (uint8_t i = 0; i < NUMBER_OF_ATTEMPTS; i++) {
    wifi.connect();
    if (DEBUG_MODE)
      Serial.printf("is WiFi connected: %d\n", wifi.is_connected());
    if (wifi.is_connected()) {
      if (send_data(results)) {
        wifi.turn_off();
        break;
      }
    }
    wifi.turn_off();
  }

  deep_sleep(TIME_TO_SLEEP);
}
