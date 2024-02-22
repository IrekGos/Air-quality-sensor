# HM3301 dust sensor datasheet:
# https://files.seeedstudio.com/wiki/Grove-Laser_PM2.5_Sensor-HM3301/res/HM-3300%263600_V2.1.pdf

import machine
import time

I2C_FREQUENCY = 100000
SENSOR_I2C_ADDRESS = 0x40
DATA_LENGTH = 29
NUMBER_OF_ATTEMPTS = 10


class DustSensor:
    def __init__(self) -> None:
        self._i2c = machine.I2C(0, scl=machine.Pin(
            5), sda=machine.Pin(4), freq=I2C_FREQUENCY, timeout=100000)
        self._sleep_mode_pin = machine.Pin(0, mode=machine.Pin.OUT, value=0)
        self._reset_pin = machine.Pin(1, mode=machine.Pin.OUT, value=1)

    def __sleep_mode(self, mode: bool) -> None:
        if mode:
            self._sleep_mode_pin.low()
        else:
            self._sleep_mode_pin.high()

    def __reset(self) -> None:
        self._reset_pin.low()
        time.sleep_ms(100)
        self._reset_pin.high()
        time.sleep_ms(100)

    def __read_sensor_value(self) -> bytes:
        return self._i2c.readfrom(SENSOR_I2C_ADDRESS, DATA_LENGTH)

    def __parse_result(self, data: bytes) -> dict:
        dust_results = {}
        # based on datasheet, chapter "Appendix 2: IIC Communication Protocol"
        dust_results["PM1"] = (data[4] << 8) | data[5]
        dust_results["PM2.5"] = (data[6] << 8) | data[7]
        dust_results["PM10"] = (data[8] << 8) | data[9]
        return dust_results

    def measure_air_quality(self) -> dict:
        # from datasheet, chapter "6. Pin definition":
        # "It takes at least 30 seconds for stabilize when restart the fan."
        self.__sleep_mode(False)
        time.sleep(30)

        results = {}
        for _ in range(NUMBER_OF_ATTEMPTS):
            try:
                data = self.__read_sensor_value()
                results = self.__parse_result(data)
                bad_value = 1 << 16 - 1
                correct = all(value != bad_value for value in results.values())
                if correct:
                    break
                else:
                    results = {}
                    self.__reset()
            except:
                self.__reset()

        self.__sleep_mode(True)
        return results
