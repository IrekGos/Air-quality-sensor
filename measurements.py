# MCP9700A analog thermometer datasheet:
# https://ww1.microchip.com/downloads/aemDocuments/documents/MSLD/ProductDocuments/DataSheets/MCP9700-Family-Data-Sheet-DS20001942.pdf
#
# Raspberry Pi Pico W datasheet:
# https://datasheets.raspberrypi.com/picow/pico-w-datasheet.pdf

import machine

CONVERSION_FACTOR = 3.3 / (65535)
NUMBER_OF_SAMPLES = 5

# from MCP9700A analog thermometer datasheet (chapter "DC ELECTRICAL CHARACTERISTICS")
TEMP_COEF = 0.01    # Temperature Coefficient
VOLT_AT_0 = 0.5     # Sensor Output Voltage at 0°C


class Measurements:
    def __enable_supply_voltage_measure(self) -> None:
        # from Raspberry Pi Pico W datasheet (chapter 2.1):
        # GPIO29    OP/IP wireless SPI CLK/ADC mode (ADC3) to measure VSYS/3
        # GPIO25    OP wireless SPI CS - when high also enables GPIO29 ADC pin to read VSYS
        machine.Pin(25, mode=machine.Pin.OUT, value=1)
        machine.Pin(29, mode=machine.Pin.IN, pull=None)

    def __adc_measure(self, pin: int, f: function) -> float:
        sum = 0
        adc_pin = machine.ADC(pin)
        for _ in range(NUMBER_OF_SAMPLES):
            adc_value = adc_pin.read_u16()
            adc_voltage = adc_value * CONVERSION_FACTOR
            measured_value = f(adc_voltage)
            sum += measured_value
        average = sum / NUMBER_OF_SAMPLES
        return round(average, 1)

    def measure_supply_voltage(self) -> float:
        self.__enable_supply_voltage_measure()
        # * 3 due to the internal voltage divider
        return self.__adc_measure(3, lambda x: x * 3)

    def measure_onboard_temperature(self) -> float:
        # The temperature sensor measures the Vbe voltage of a biased bipolar
        # diode, connected to the fifth ADC channel. Typically, Vbe = 0.706V
        # at 27 degrees C, with a slope of -1.721mV (0.001721) per degree.
        return self.__adc_measure(4, lambda x: 27 - (x - 0.706)/0.001721)

    def measure_ambient_temperature(self) -> float:
        # formula from equation 4.1 in MCP9700A datasheet
        return self.__adc_measure(2, lambda x: (x - VOLT_AT_0) / TEMP_COEF)
