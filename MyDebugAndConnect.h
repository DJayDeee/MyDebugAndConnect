/****************************************************************************************
* File :		MyDebugAndConnect.h
* Date :		2018-Oct-10
* By :			Jean-Daniel Lavoie
* Description :	This library configure the serial port and one led for debuging.
*				It also allow to control the led (off, on and toggle).
****************************************************************************************/

#ifndef		_MYDEBUGANDCONNECT_H_
#define		_MYDEBUGANDCONNECT_H_

#include	<FS.h>								// https://github.com/pellepl/spiffs
#include	<ArduinoJson.h>						// https://github.com/bblanchon/ArduinoJson
#include	<ESP8266WiFi.h>						// https://github.com/esp8266/Arduino
#include	<WiFiManager.h>						// https://github.com/tzapu/WiFiManager/tree/development
#include	<Ticker.h>							// Timer manager

#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define	DEFAULT_LED_PIN		2					// Buitlin led for Wemos D1 mini.
#define	DEFAULT_BAUDRATE	74880				// To ensure readability of the bootloader.

void 			Toggle(void);					// Callback Helper to connect led_Toggle() with blynker Ticker.

class MyDebug {
  public:
				MyDebug(const int br, const int pin = DEFAULT_LED_PIN);
	void		Init(void);

	void		led_OFF(void);
	void		led_ON(void);
	void		led_Toggle(void);
		
	void		setBlynkRate(const float seconds);
	void		handle(bool force_actualise = 0);

  private:
  	int			led_pin;
	Ticker		blynker;
	int			baudrate;
	int 		last_wifistate;
};

static	const	IPAddress	DHCPAddress = IPAddress(0,0,0,0);		// IP adress used to force DHCP in STA mode.
struct _staticAddress {										// Container for the IP, GATEWAY, SUBNET address and HOSTNAME.
	IPAddress	local	= IPAddress(192,168,0,4);
	IPAddress	gateway	= IPAddress(192,168,0,1);
	IPAddress	subnet	= IPAddress(255,255,255,0);
	char		hostname[32];
};

#define	MYCONNECT_CONFIG_FILE	"/config_myconnect.json"	// CONFIG_FILE is the name of the Configuration file.

void 						saveConfigCallback(void);		// Callback Helper to save parameters with writeConfigFile().

class MyConnect {
  public:
							MyConnect(const int pin);
	void					init(void);
	void					StartManager(int timeout = 0);	// Use WiFiManager and get MQTT parameters.
	bool					writeConfigFile(const char *filename = MYCONNECT_CONFIG_FILE);
	void					setDebug(const bool _debug) { debug = _debug; }	// Called to enable/disable the debug information over serial.
	void					wifiInfo(void);

  private:

  	int						factoreset_pin;
	_staticAddress			staticAddress;
	bool					debug = true;
	int						connect_timeout = 20;
	int						portal_timeout = 120;

	WiFiManager				wm;
	WiFiManagerParameter 	*p_hostname			= new WiFiManagerParameter("hostname", "Custom hostname :", staticAddress.hostname, 32);
	WiFiManagerParameter 	*p_debug			= new WiFiManagerParameter("debug", "Debug serial printout", "T", 2, "type=\"checkbox\" checked", WFM_LABEL_AFTER);
	WiFiManagerParameter 	*p_connect_timeout	= new WiFiManagerParameter("connection_timeout_delay", "WiFi connection timeout :", "20", 6);
	WiFiManagerParameter 	*p_portal_timeout	= new WiFiManagerParameter("portal_timeout_delay", "WiFi configuration portal timeout :", "120", 6);

	bool					readConfigFile(const char *filename = MYCONNECT_CONFIG_FILE);
	void 					ConfigureManager(void);

	void 					ResetConfigFile(const char *filename = MYCONNECT_CONFIG_FILE); 	// Reset parameters file.
	void					ResetWiFi(void);												// Reset ONLY WiFi credential.
	void					FactoryReset(void);

	template <typename Generic>
	void					DEBUG_MYCONNECT(Generic text) const; // Debug channel enable if debug is set.
	template <typename Generic>
	void					ERROR_MYCONNECT(Generic text) const; // ERROR channel enable if debug is set.
};

extern MyDebug		mydebug;
extern MyConnect	myconnect;

#endif // _MYDEBUGANDCONNECT_H_
