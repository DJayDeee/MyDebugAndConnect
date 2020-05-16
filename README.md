# MyDebugAndConnect
This library configure the serial port and one led for debuging (off, on and toggle).
It also hide custom parameters for WiFiManager.

## Contents
- [Defines and structures](#defines-and-structures)  
- [Functions](#functions)
- [License and credits](#license-and-credits)

### Defines and structures
* For the MyDebug class :
  * DEFAULT_BAUDRATE is serial monitor speed by default (74880).  This speed specially allow to see bootloader informations.
  
* For the MyDebug class :
    * MYCONNECT_CONFIG_FILE is the json file ("/config_myconnect.json") where the parameters are stored in SPIFFS.
      Exemple of what is inside CONFIG_FILE :
  ```bash
  {
    "local": "192.168.0.4",
    "gateway": "192.168.0.1",
    "subnet": "255.255.255.0",
    "hostname": "",
    "debug": true,
    "connect_timeout": 20,
    "portal_timeout": 120
  }
  ```
  * DHCPAddress is used to switch from static IP to DHCP address (0.0.0.0).  Does not work whit stable release 2.4.0 [Arduino for ESP8266](https://github.com/esp8266/Arduino) see issue [https://github.com/esp8266/Arduino/pull/4145](https://github.com/esp8266/Arduino/pull/4145).
  * staticAdress structure contain the LOCAL, GATEWAY, SUBNET adress and hostname :
  ```bash
  struct _staticAddress {
	  IPAddress	local	= IPAddress(192,168,0,4);
	  IPAddress	gateway	= IPAddress(192,168,0,1);
	  IPAddress	subnet	= IPAddress(255,255,255,0);
	  char		hostname[32];
  };
  ```

### Functions
* Toggle() is a Callback helper fonction for blynker Ticker.
* saveConfigCallback() is a Callback helper fonction to save parameters to MYCONNECT_CONFIG_FILE.

* MyDebug(baudrate, led_pin) instanciate class and set BAUDRATE and LED_PIN.
  * Init() begin the serial connection and 0.6 second led blink.
  * led_OFF() turn led off.
  * led_ON() turn led on.
  * led_Toggle() toggle led state.
  * setBlynkRate(seconds) blink the led for selected time.
  * handle() is called during loop() to blink led slowly (1 second) if wifi isn't connected and OFF when connected.
  
* MyConnect(factory_reset_pin) instanciate class and select the pin who reset the uC when set low during boot sequence.
  * init() restore the parameters from MYCONNECT_CONFIG_FILE, configure the WiFiManager,try to connect to the last WiFi and display the wifiInfo().  If the connection failed (the connection timeout is reached) the WiFiManager is started unit it reach the portal timeout.
  * StartManager(timeout) start WiFiManager configuration portal with the selected timeout in second.
  * writeConfigFile(filename) save the WiFiManager parameters to the selected filename.
  * setDebug(debug) enable or disable the debug information over serial.
  * wifiInfo(void) display the connection result, the IP adress and the hostname of the uC.

### License and credits
- Inspired from [tzapu/WiFiManager](https://github.com/tzapu/WiFiManager/tree/development)
- [Arduino for ESP8266](https://github.com/esp8266/Arduino)
- [C++ JSON library for IoT](https://github.com/bblanchon/ArduinoJson) [https://arduinojson.org](https://arduinojson.org)
- Ticker by Ivan Grokhtokov
