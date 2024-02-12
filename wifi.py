import network
import time

NUMBER_OF_ATTEMPTS = 10


class WiFi:
    def __turn_on(self) -> None:
        self.__wlan = network.WLAN(network.STA_IF)
        self.__wlan.active(True)

    def __connect(self, credentials: dict) -> None:
        self.__wlan.connect(
            credentials["WIFI_SSID"], credentials["WIFI_PASSWORD"])

    def __wait_for_connection(self) -> None:
        for _ in range(NUMBER_OF_ATTEMPTS):
            if self.__wlan.status() != network.STAT_CONNECTING:
                break
            time.sleep(1)

    def setup(self, credentials: dict) -> None:
        self.__turn_on()
        self.__connect(credentials)
        self.__wait_for_connection()

    def turn_off(self) -> None:
        self.__wlan.disconnect()
        self.__wlan.active(False)
        self.__wlan.deinit()
        self.__wlan = None

    def is_connected(self) -> bool:
        return self.__wlan.isconnected()
