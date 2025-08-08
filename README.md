# Air-quality-sensor

Simple IoT system that measures air quality. Based on the ESP32-S3, written in Arduino. Connected to the ThingSpeak cloud.

## Hardware

- XIAO ESP32-S3
- Grove - Laser PM2.5 Sensor (HM3301)
- MCP9700A analog temperature sensor

## Default pinout

| ESP32-S3 | HM3301 |
| -------- | ------ |
| GPIO 5   | SDA    |
| GPIO 6   | SCL    |

| ESP32-S3 | MOSFET |
| -------- | ------ |
| GPIO 44  | Gate   |

## Setup

- prepare `credentials.h` file and fill it with your credentials:

```c
#ifndef credentials_h
#define credentials_h

#define SSID "ssid"
#define PASSWORD "password"
#define API_KEY "api_key"

// Identity: DigiCert Global Root G2
// Verified by: DigiCert Global Root G2
// Expires: 15/01/38
#define THINGSPEAK_CERTIFICATE "-----BEGIN CERTIFICATE-----\n\
MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh\n\
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n\
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n\
MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT\n\
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n\
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG\n\
9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI\n\
2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx\n\
1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ\n\
q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz\n\
tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ\n\
vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo0IwQDAP\n\
BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV\n\
5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY\n\
1Yl9PMWLSn/pvtsrF9+wX3N3KjITOYFnQoQj8kVnNeyIv/iPsGEMNKSuIEyExtv4\n\
NeF22d+mQrvHRAiGfzZ0JFrabA0UWTW98kndth/Jsw1HKj2ZL7tcu7XUIOGZX1NG\n\
Fdtom/DzMNU+MeKNhJ7jitralj41E6Vf8PlwUHBHQRFXGU7Aj64GxJUTFy8bJZ91\n\
8rGOmaFvE7FBcf6IKshPECBV1/MUReXgRPTqh5Uykw7+U0b6LJ3/iyK5S9kJRaTe\n\
pLiaWN0bfVKfjllDiIGknibVb63dDcY3fe0Dkhvld1927jyNxF1WW6LZZm6zNTfl\n\
MrY=\n\
-----END CERTIFICATE-----"

#endif
```

- open Arduino IDE and install `ArduinoJson` library
- add ESP32 board package to your Arduino IDE (instruction [here](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/#software-preparation))
- build code and upload to your board

## Air Quality Index

Polish scale provided by the Chief Inspectorate of Environmental Protection ([source](https://powietrze.gios.gov.pl/pjp/content/health_informations)).

| AQI       | PM2.5 [µg/m3 ] | PM10 [µg/m3 ] |
| --------- | -------------- | ------------- |
| Very good | 0 - 13         | 0 - 20        |
| Good      | 13.1 - 35      | 20.1 - 50     |
| Moderate  | 35.1 - 55      | 50.1 - 80     |
| Fair      | 55.1 - 75      | 80.1 - 110    |
| Poor      | 75.1 - 110     | 110.1 - 150   |
| Very poor | > 110          | > 150         |
