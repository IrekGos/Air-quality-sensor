#ifndef measurements_h
#define measurements_h

class Measurements {
private:
  float adc_measure(uint8_t pin);
public:
  void configure(void);
  float measure_internal_temp(void);
  float measure_ambient_temp(void);
  float measure_supply_voltage(void);
};

#endif