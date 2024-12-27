#ifndef wifi_h
#define wifi_h

class Wifi {
public:
  void connect(void);
  bool is_connected(void);
  void turn_off(void);
};

#endif