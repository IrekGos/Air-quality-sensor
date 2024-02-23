# Air-quality-sensor

Simple IoT system that measures air quality. Based on the RPi Pico W, written in MicroPython. Connected to the ThingSpeak cloud.

## Hardware

- Raspberry Pi Pico W
- Grove - Laser PM2.5 Sensor (HM3301)
- MCP9700A analog temperature sensor

## Default pinout

| Raspberry Pi Pico W | HM3301 |
| ------------------- | ------ |
| 3V3 (OUT)           | VCC    |
| GND                 | GND    |
| GPIO 4              | SDA    |
| GPIO 5              | SCL    |
| GPIO 0              | SET    |
| GPIO 1              | RESET  |

## Setup

- prepare `config.json` file and fill it with your credentials:

```json
{
  "WIFI_SSID": "WiFi SSID",
  "WIFI_PASSWORD": "WiFi password",
  "API_KEY": "cloud api key"
}
```

- upload `config.json` and all python scripts to Pico W. After restarting, the `main.py` script will be executed
