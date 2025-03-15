#include <driver/temperature_sensor.h>
#include <Arduino.h>
#include "measurements.h"

// from MCP9700A analog thermometer datasheet (chapter "DC ELECTRICAL CHARACTERISTICS")
#define TEMP_COEF 0.01  // Temperature Coefficient
#define VOLT_AT_0 0.5   // Sensor Output Voltage at 0°C
#define NUMBER_OF_SAMPLES 10
#define TEMP_CALIBRATION_COEF 2
#define SUPPLY_CALIBRATION_COEF 0.15

const uint8_t ambient_temp_pin = A8;
const uint8_t supply_voltage_pin = A9;

temperature_sensor_handle_t temp_sensor = NULL;
const temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(-10, 80);

void Measurements::configure() {
  pinMode(ambient_temp_pin, INPUT);
  pinMode(supply_voltage_pin, INPUT);
  temperature_sensor_install(&temp_sensor_config, &temp_sensor);
  temperature_sensor_enable(temp_sensor);
}

float Measurements::adc_measure(uint8_t pin) {
  float sum = 0;
  for (uint8_t i = 0; i < NUMBER_OF_SAMPLES; i++) {
    sum += analogReadMilliVolts(pin)* 0.001;
    delay(10);
  }
  float average = sum / NUMBER_OF_SAMPLES;
  return average;
}

float Measurements::measure_internal_temp() {
  float tsens_value;
  temperature_sensor_get_celsius(temp_sensor, &tsens_value);
  return roundf(tsens_value * 10.0) / 10.0;
}

float Measurements::measure_ambient_temp() {
  float analogVolts = adc_measure(ambient_temp_pin);
  float temp = (analogVolts - VOLT_AT_0) / TEMP_COEF;
  return roundf(temp * 10.0) / 10.0 + TEMP_CALIBRATION_COEF;
}

float Measurements::measure_supply_voltage() {
  float analogVolts = adc_measure(supply_voltage_pin);
  //  * 2 due to voltage devider
  return roundf(analogVolts * 2 * 100.0) / 100.0 + SUPPLY_CALIBRATION_COEF;
}