/****************************************************************************************
* File :		MyDebugAndConnect.h
* Date :		2018-Oct-10
* By :			Jean-Daniel Lavoie
* Description :	This library configure the serial port and one led for debuging.
*				It also allow to control the led (off, on and toggle).
****************************************************************************************/

#ifndef		_MYDEBUGANDCONNECT_H_
#define		_MYDEBUGANDCONNECT_H_

#include	<FS.h>							//https://github.com/pellepl/spiffs
#include	<ArduinoJson.h>					//https://github.com/bblanchon/ArduinoJson
#define	CONFIG_FILE	"/config_0.json"		// CONFIG_FILE is the name of the Configuration file.
#include	<ESP8266WiFi.h>					//https://github.com/esp8266/Arduino
#include	<WiFiManager.h>					//https://github.com/tzapu/WiFiManager
#include	<Ticker.h>						// Timer manager

#define	DEFAULT_LED_PIN		2
#define	DEFAULT_BAUDRATE	74880

class mydebug {
  public:
				mydebug(const int br = DEFAULT_BAUDRATE, const int pin = DEFAULT_LED_PIN);
	void		Init(void);

	void		led_OFF(void);
	void		led_ON(void);
	void		led_Toggle(void);
		
	void		setBlynkRate(const float seconds);
	void		handleWifiBlynk(bool force_actualise = 0);
	
	static int	led_pin;
  private:
	Ticker		blynker;
	int			baudrate;
	int 		last_wifistate;

	template <typename Generic>					// Debug channel enable if debug is set.
	void		DEBUG_MYDEBUG(Generic text) const;
};

class myconnect {
  public:
							myconnect(const int pin = 0);
	void					init(void);
	void					StartManager(int timeout = 0);// Use WiFiManager and get MQTT parameters.
	void					handleSaveConfig(void);

	static bool				shouldSaveConfig;		// Flag for saving data.

  private:
  	int						factoreset_pin;
	void 					ResetWiFi(void); 		// Reset only WiFi credential.

 	WiFiManager				wifiManager.setConnectTimeout(20);
;
	
	bool					readConfigFile(const char *filename = CONFIG_FILE);	// Restore configuration from the FileSystem.
	bool					writeConfigFile(const char *filename = CONFIG_FILE);// Save WiFi configuration to the FileSystem.

	template <typename Generic>					// Debug channel enable if debug is set.
	void					DEBUG_MYCONNECT(Generic text) const;

};

#endif // _MYDEBUGANDCONNECT_H_
