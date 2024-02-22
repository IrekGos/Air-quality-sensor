import json
import urequests as requests
import time
import machine

from measurements import Measurements
from wifi import WiFi
from dust_sensor import DustSensor

CPU_FREQUENCY = 70          # in kHz, lower frequencies cause problems with WiFi
ONE_HOUR_IN_SECONDS = 3565  # one hour minus time spent measuring and sending data
NUMBER_OF_ATTEMPTS = 10


def send_data(data: dict, credentials: dict) -> bool:
    api_key = credentials["API_KEY"]
    url = "https://api.thingspeak.com/update.json"
    data["api_key"] = api_key
    try:
        response = requests.get(url, json=data)
    except:
        return False
    return response.status_code == 200


if __name__ == "__main__":

    # reduce frequency to save power
    machine.freq(CPU_FREQUENCY * 1000000)

    with open('config.json', 'r') as config_file:
        config = json.load(config_file)

    dust_sensor = DustSensor()
    wifi = WiFi()
    measurements = Measurements()

    while True:
        dust_results = dust_sensor.measure_air_quality()

        # sleep to stabilize voltage before measurements
        time.sleep_ms(10)

        results = {}
        if dust_results != {}:
            results["field1"] = dust_results["PM1"]
            results["field2"] = dust_results["PM2.5"]
            results["field3"] = dust_results["PM10"]
        results["field4"] = measurements.measure_ambient_temperature()
        results["field5"] = measurements.measure_onboard_temperature()
        results["field6"] = measurements.measure_supply_voltage()

        for _ in range(NUMBER_OF_ATTEMPTS):
            wifi.setup(config)
            if wifi.is_connected():
                if send_data(results, config):
                    break
            wifi.turn_off()

        time.sleep(ONE_HOUR_IN_SECONDS)
