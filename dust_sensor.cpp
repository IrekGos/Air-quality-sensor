// HM3301 dust sensor datasheet:
// https://files.seeedstudio.com/wiki/Grove-Laser_PM2.5_Sensor-HM3301/res/HM-3300%263600_V2.1.pdf

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include "dust_sensor.h"

#define I2C_FREQUENCY 100000
#define SENSOR_I2C_ADDRESS 0x40
#define DATA_LENGTH 29
#define NUMBER_OF_ATTEMPTS 10
#define NUMBER_OF_MEASUREMENTS 5
#define uS_TO_S_FACTOR 1000000

const uint64_t TIME_TO_SLEEP = (uint64_t)uS_TO_S_FACTOR * 30;

const uint8_t mosfet_pin = D7;

void DustSensor::dust_sensor_init() {
  Wire.begin();
  Wire.setClock(I2C_FREQUENCY);
  Wire.setTimeOut(200);
  gpio_hold_dis((gpio_num_t)mosfet_pin);
  pinMode(mosfet_pin, OUTPUT);
  digitalWrite(mosfet_pin, LOW);
}

void DustSensor::sleep_mode(bool mode) {
  if (mode) {
    digitalWrite(mosfet_pin, LOW);
  } else {
    digitalWrite(mosfet_pin, HIGH);
  }
}

void DustSensor::reset() {
  sleep_mode(true);
  light_sleep(0.5 * uS_TO_S_FACTOR);
  sleep_mode(false);
}

void DustSensor::light_sleep(uint64_t time) {
  esp_sleep_enable_timer_wakeup(time);
  esp_light_sleep_start();
}

bool DustSensor::read_sensor_value(uint8_t *temp) {
  uint8_t bytesReceived = Wire.requestFrom(SENSOR_I2C_ADDRESS, DATA_LENGTH);
  if (bytesReceived == DATA_LENGTH) {
    Wire.readBytes(temp, bytesReceived);
  }
  return bytesReceived == DATA_LENGTH;
}

void DustSensor::parse_result(uint8_t *raw_bytes, measurement_result *result) {
  // based on datasheet, chapter "Appendix 2: IIC Communication Protocol"
  result->pm1 = (raw_bytes[4] << 8) | raw_bytes[5];
  result->pm2_5 = (raw_bytes[6] << 8) | raw_bytes[7];
  result->pm10 = (raw_bytes[8] << 8) | raw_bytes[9];
}

bool DustSensor::are_values_correct(measurement_result *result) {
  int bad_value = (1 << 16) - 1;
  return result->pm1 != bad_value && result->pm2_5 != bad_value && result->pm10 != bad_value;
}

void DustSensor::add_result_to_accumulator(measurement_result *result, measurement_result *accumulator) {
  accumulator->pm1 += result->pm1;
  accumulator->pm2_5 += result->pm2_5;
  accumulator->pm10 += result->pm10;
}

void DustSensor::calc_mean_value(measurement_result *accumulator, uint8_t successful_readings) {
  accumulator->pm1 /= successful_readings;
  accumulator->pm2_5 /= successful_readings;
  accumulator->pm10 /= successful_readings;
}

void DustSensor::store_value(JsonDocument &results, measurement_result *accumulator) {
  results["PM1"] = accumulator->pm1;
  results["PM2.5"] = accumulator->pm2_5;
  results["PM10"] = accumulator->pm10;
}

uint8_t DustSensor::measure_air_quality(JsonDocument &results) {
  // from datasheet, chapter "6. Pin definition":
  // "It takes at least 30 seconds for stabilize when restart the fan."
  sleep_mode(false);
  light_sleep(TIME_TO_SLEEP);
  uint8_t buf[DATA_LENGTH];
  measurement_result tmp, accumulator = { 0 };
  uint8_t successful_readings = 0;
  for (uint8_t i = 0; i < NUMBER_OF_ATTEMPTS; i++) {
    if (read_sensor_value(buf)) {
      parse_result(buf, &tmp);
      if (are_values_correct(&tmp)) {
        successful_readings++;
        add_result_to_accumulator(&tmp, &accumulator);
        if (successful_readings == NUMBER_OF_MEASUREMENTS) {
          break;
        }
      } else {
        reset();
      }
    } else {
      reset();
    }
    light_sleep(1 * uS_TO_S_FACTOR);
  }
  sleep_mode(true);
  // hold mosfet_pin state during deep sleep
  gpio_hold_en((gpio_num_t)mosfet_pin);

  if (successful_readings > 0) {
    calc_mean_value(&accumulator, successful_readings);
    store_value(results, &accumulator);
  }
  return successful_readings;
}