#ifndef dust_sensor_h
#define dust_sensor_h

typedef struct {
  uint16_t pm1, pm2_5, pm10;
} measurement_result;

class DustSensor {
private:
  void sleep_mode(bool mode);
  void reset(void);
  void light_sleep(uint64_t time);
  bool read_sensor_value(uint8_t *temp);
  void parse_result(uint8_t *raw_bytes, measurement_result *result);
  bool are_values_correct(measurement_result *result);
  void add_result_to_accumulator(measurement_result *result, measurement_result *accumulator);
  void calc_mean_value(measurement_result *accumulator, uint8_t successful_readings);
  void store_value(JsonDocument &results, measurement_result *accumulator);
public:
  void dust_sensor_init(void);
  uint8_t measure_air_quality(JsonDocument &results);
};

#endif