# MyDebugAndConnect
This library configure the serial port and one led for debuging (off, on and toggle).
It also hide custom parameters for WiFiManager.

## Contents
- [Defines and structures](#defines-and-structures)  
- [Functions](#functions)
- [License and credits](#license-and-credits)

### Defines and structures
* For the MyDebug class :
  * DEFAULT_LED_PIN define is used to select the onboard led pin by default (2).
  * DEFAULT_BAUDRATE is serial monitor speed by default (74880).  This speed specially allow to see bootloader informations.
  
* For the MyDebug class :
    * MYCONNECT_CONFIG_FILE	is the json file ("/config_myconnect.json") where the parameters are stored in SPIFFS.
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
* MyDebug(baudrate, led_pin) instanciate class and set BAUDRATE and LED_PIN.
  * Init()		    Begin the serial connection and 0.6 second led blink.
  * led_OFF()		  Turn led off.
  * led_ON()		  Turn led on.
  * led_Toggle()	Toggle led state.
  * setBlynkRate(seconds) Blink the led for selected time.
  * handle()      Called during loop() to blink led slowly (1 second) if wifi isn't connected and OFF when connected.
  
* MyConnect(factory_reset_pin) instanciate class and select the pin who reset the uC when set low during boot sequence.
  * 

### License and credits
- Inspired from [tzapu/WiFiManager](https://github.com/tzapu/WiFiManager/tree/development)
- [Arduino for ESP8266](https://github.com/esp8266/Arduino)
- [SPI Flash File System (SPIFFS)](https://github.com/pellepl/spiffs)
- [C++ JSON library for IoT](https://github.com/bblanchon/ArduinoJson) [https://arduinojson.org](https://arduinojson.org)
- [Ticker library for Arduino](https://github.com/sstaub/Ticker)
