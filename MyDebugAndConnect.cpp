/****************************************************************************************
* File :		MyDebugAndConnect.cpp
* Date :		2020-Mai-03
* By :			Jean-Daniel Lavoie
* Description :	This library control the serial port and one led for debuging.
*				It also allow to manage the WiFi connection and his parameters.
****************************************************************************************/

#include	<MyDebugAndConnect.h>

///////////////////////////////////////////////////////////////////////////////////////
// Callback Helper to connect led_Toggle() with blynker Ticker.
void ToggleCallback(void) {	mydebug.led_Toggle(); }

///////////////////////////////////////////////////////////////////////////////////////
// MyDebug Constructor.
MyDebug::MyDebug(const int br, const int pin) {
	led_pin = pin;
	baudrate = br;
}

///////////////////////////////////////////////////////////////////////////////////////
// Setup pin, ticker and serial.
void MyDebug::Init(void) {
 	pinMode(led_pin, OUTPUT);
	blynker.attach(0.6, ToggleCallback);

	Serial.begin(baudrate);
	Serial.println(" ");
}

///////////////////////////////////////////////////////////////////////////////////////
// Turn builtin led off.
void MyDebug::led_OFF(void) {
	digitalWrite(led_pin, HIGH);
}

///////////////////////////////////////////////////////////////////////////////////////
// Turn builtin led on.
void MyDebug::led_ON(void) {
	digitalWrite(led_pin, LOW);
}

///////////////////////////////////////////////////////////////////////////////////////
// Toggle builtin led state.
void MyDebug::led_Toggle(void) {
	digitalWrite(led_pin, !digitalRead(led_pin));
}

///////////////////////////////////////////////////////////////////////////////////////
// Change led blink speed.
void MyDebug::setBlynkRate(const float seconds) {
// If blynk rate is 0 stop the blynker and let the led off.
	if(seconds)
		blynker.attach(seconds, ToggleCallback);
	else {
		blynker.detach();
		led_OFF();
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Blynk slowly if not connected or turn led OFF when connected.  ONLY WHEN THE WIFI STATE CHANGE.
void MyDebug::handle(bool force_actualise)	{
// Change led blynk state only if WiFi status change.
	if ( (WiFi.isConnected() != last_wifistate) | force_actualise ) {
		last_wifistate = WiFi.isConnected();
// Led blynk slowly if WiFi is disconnected.  Led OFF solid if WiFi is connected.
		if (!last_wifistate) {
		  setBlynkRate(1);
		  WiFi.begin();
		} else
		  setBlynkRate(0);
	}
//	ArduinoOTA.handle();

}

/*=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=*/
/*=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=*/
/*=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=*/
/*=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=*/

///////////////////////////////////////////////////////////////////////////////////////
// Callback Helper to save parameters with writeConfigFile().
void saveConfigCallback(void) {
	myconnect.writeConfigFile(MYCONNECT_CONFIG_FILE);
}

///////////////////////////////////////////////////////////////////////////////////////
// MyConnect Constructor.
MyConnect::MyConnect(const int pin) {
	factoreset_pin = pin;
}

///////////////////////////////////////////////////////////////////////////////////////
// First initialisation of the WiFi.
void MyConnect::init(void) {
// Factory reset pin to ground?
	pinMode(factoreset_pin, INPUT);
	if(digitalRead(factoreset_pin) == false)
		FactoryReset();

// Restore parameters from MYCONNECT_CONFIG_FILE.
	if(readConfigFile(MYCONNECT_CONFIG_FILE) == false)
		ERROR_MYCONNECT(F("CONTINUE WITHOUT RESTORING SETTINGS."));

// 
	ConfigureManager();
	if(wm.autoConnect() == false)
		ERROR_MYCONNECT(F("Failed to connect or hit timeout"));
	else
		wifiInfo();

//	ArduinoOTA.setHostname(staticAddress.hostname);
//	ArduinoOTA.begin();
	DEBUG_MYCONNECT(F("INITIALIZE successfully"));
}

///////////////////////////////////////////////////////////////////////////////////////
// Restore parameters from the SPIFF filename.
// Return true if success.
bool MyConnect::readConfigFile(const char *filename) {
	DynamicJsonDocument doc(512);
	char *buf2 = new char[18];
// Print the directory and filename of the configuration.
	String sfilename = filename;
	DEBUG_MYCONNECT("LOAD configuration from file : " + sfilename);
// Open or create the SPIFFS file.
	if(!SPIFFS.begin()) {
		ERROR_MYCONNECT(F("Unable to MOUNT spiffs."));
		return false;
	}
	if(!SPIFFS.exists(filename)) {
		ERROR_MYCONNECT("Unable to FIND : " + sfilename);
		SPIFFS.end();
		return false;
	}
	File file = SPIFFS.open(filename, "r");
	if(!file) {
		ERROR_MYCONNECT("Unable to OPEN : " + sfilename);
		SPIFFS.end();
		return false;
	}

// Allocate and store contents of file in a Document.
	DeserializationError error = deserializeJson(doc, file);
// Closing file and unmount SPIFlashFileSystem.
	file.close();
	SPIFFS.end();
	if (error) {
		String error_string = error.c_str();
		ERROR_MYCONNECT("Failed to deserialize file : " + error_string);
    	return false;
	}
// Display the content of the JSON doc.
	serializeJsonPretty(doc, Serial);
	Serial.println("");

// Parse all parameters and override local variables.
	if(doc.containsKey("local")) {
		strcpy(buf2, doc["local"]);
		staticAddress.local.fromString(buf2);
	}
	if(doc.containsKey("gateway")) {
		strcpy(buf2, doc["gateway"]);
		staticAddress.gateway.fromString(buf2);
	}
	if(doc.containsKey("subnet")) {
		strcpy(buf2, doc["subnet"]);
		staticAddress.subnet.fromString(buf2);
	}
	if(doc.containsKey("hostname"))
		strcpy(staticAddress.hostname, doc["hostname"]);
	if(doc.containsKey("debug"))
		debug = doc["debug"];
	if(doc.containsKey("connect_timeout"))
		connect_timeout = doc["connect_timeout"];
	if(doc.containsKey("portal_timeout"))
		portal_timeout = doc["portal_timeout"];

	DEBUG_MYCONNECT(F("Configuration RESTORED."));
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////
// Save parameters to filename in the FileSystem
// Return true if success.
bool MyConnect::writeConfigFile(const char *filename) {
	DynamicJsonDocument doc(512);
// Print the directory and filename of the configuration.
	String sfilename = filename;
	DEBUG_MYCONNECT("SAVE configuration to file : " + sfilename);
// Open or create the SPIFFS file.
	if(!SPIFFS.begin()) {
		ERROR_MYCONNECT(F("Unable to MOUNT spiffs."));
		return false;
	}
	File file = SPIFFS.open(filename, "w");
	if(!file) {
		ERROR_MYCONNECT("Unable to OPEN : " + sfilename);
		SPIFFS.end();
		return false;
	}

// Get the IP adress we are currently using.
	staticAddress.local 	= WiFi.localIP();
	staticAddress.gateway	= WiFi.gatewayIP();
	staticAddress.subnet	= WiFi.subnetMask();	
// Get advanced parameters from WiFiManager.
	strcpy(staticAddress.hostname, const_cast<char*>(p_hostname->getValue()));
	myconnect.debug = (strncmp(p_debug->getValue(), "T", 1) == 0);
	myconnect.connect_timeout = atoi(p_connect_timeout->getValue());
	myconnect.portal_timeout = atoi(p_portal_timeout->getValue());

// Create a JSON document and pass it the parameters.
	doc["local"]	= staticAddress.local.toString();
	doc["gateway"]	= staticAddress.gateway.toString();
	doc["subnet"]	= staticAddress.subnet.toString();
	doc["hostname"]	= staticAddress.hostname;
	doc["debug"]	= debug;
	doc["connect_timeout"]	= connect_timeout;
	doc["portal_timeout"]	= portal_timeout;
// If debug is enable display the content of the JSON document.
	if(debug) {
		serializeJsonPretty(doc, Serial);
		Serial.println("");
	}

// Save the JSON document to the file SPIFFS.
	size_t size = serializeJson(doc, file);
// Closing file and unmount SPIFlashFileSystem.
	file.close();
	SPIFFS.end();
	if (size == 0) {
		ERROR_MYCONNECT("Failed to write to file");
		return false;
	}

	DEBUG_MYCONNECT(F("Configuration SAVED."));
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////
// Configure WiFiManager.
void MyConnect::ConfigureManager(void) {
	char _connect_timeout[6];
	char _portal_timeout[6];

// Use defined hostname.
	WiFi.hostname(staticAddress.hostname);
	p_hostname->setValue(staticAddress.hostname, 32);

// Create a checkbox for the debug boolean input field.
	if(debug)
		p_debug->setValue("T", 2);
	else
		p_debug->setValue("", 2);

// Gater the timeout for connection retry.
	sprintf(_connect_timeout, "%d", connect_timeout);
	p_connect_timeout->setValue(_connect_timeout, 6);

// Gater the timeout before portal close by itself.
	sprintf(_portal_timeout, "%d", portal_timeout);
	p_portal_timeout->setValue(_portal_timeout, 6);

	wm.addParameter(p_hostname);
	wm.addParameter(p_debug);
	wm.addParameter(p_connect_timeout);
	wm.addParameter(p_portal_timeout);

// Apply the debug, save, static IP and timeout parameters to the portal.
	std::vector<const char *> menu = {"wifi","info","sep","erase","restart","sep","exit"};
	wm.setMenu(menu);								// custom menu via vector
	wm.setClass("invert");							//set dark theme
	wm.setDebugOutput(debug);
	wm.setSaveConfigCallback(saveConfigCallback);
	wm.setSTAStaticIPConfig(staticAddress.local, staticAddress.gateway, staticAddress.subnet);
	wm.setConnectTimeout(connect_timeout);
	wm.setConfigPortalTimeout(portal_timeout);
	wm.setCountry("US");

}

///////////////////////////////////////////////////////////////////////////////////////
// Open WiFi credentials portal.
void MyConnect::StartManager(int timeout) {
	wm.setConfigPortalTimeout(timeout);
	if(!wm.startConfigPortal())
		ERROR_MYCONNECT(F("Failed to connect OR hit timeout OR closed without modification"));
	else
		wifiInfo();
}

///////////////////////////////////////////////////////////////////////////////////////
// Display WiFi status, hostname and IP address.
void MyConnect::wifiInfo(void) {
	switch(WiFi.waitForConnectResult()) {
		case WL_CONNECTED:		DEBUG_MYCONNECT(F("WL_CONNECTED (Successful connection is established)."));		break;
		case WL_IDLE_STATUS:	DEBUG_MYCONNECT(F("WL_IDLE_STATUS (Changing between statuses)."));				return;
		case WL_NO_SSID_AVAIL:	DEBUG_MYCONNECT(F("WL_NO_SSID_AVAIL (Configured SSID cannot be reached)."));	return;
		case WL_CONNECT_FAILED:	DEBUG_MYCONNECT(F("WL_CONNECT_FAILED (Password is incorrect?)."));				return;
		case WL_DISCONNECTED:	DEBUG_MYCONNECT(F("WL_DISCONNECTED (Not configured in station mode)."));		return;
		default:				DEBUG_MYCONNECT(F("UNKNOW (42)"));												return;
	}

	DEBUG_MYCONNECT("Local HostName is " + WiFi.hostname());
	DEBUG_MYCONNECT("Local IP is " + WiFi.localIP().toString());
}

///////////////////////////////////////////////////////////////////////////////////////
// Reset (DELETE) parameters file.
void MyConnect::ResetConfigFile(const char *filename) {
	DEBUG_MYCONNECT(F("ERASING ConfigFile."));
	String sfilename = filename;
	if(SPIFFS.begin()) {
		if(SPIFFS.exists(filename)) {
			DEBUG_MYCONNECT("DELETING parameters file : " + sfilename);
			SPIFFS.remove(filename);
		}
		SPIFFS.end();
	} else
		ERROR_MYCONNECT(F("Unable to MOUNT spiffs to remove ConfigFile."));
}

///////////////////////////////////////////////////////////////////////////////////////
// Reset the WiFi credentials.
void MyConnect::ResetWiFi(void) {
	DEBUG_MYCONNECT(F("ERASING WiFi credentials and restart."));
	wm.resetSettings();
	ESP.restart();
    while(true);
}

///////////////////////////////////////////////////////////////////////////////////////
// Reset the WiFi credentials.
void MyConnect::FactoryReset(void) {
	DEBUG_MYCONNECT(F("FULL FACTORY RESET ACTIVATED!"));
	ResetConfigFile(MYCONNECT_CONFIG_FILE);
	ResetWiFi();
}

///////////////////////////////////////////////////////////////////////////////////////
// Channel to print the debug.  Print text only if debug is active.
template <typename Generic>
void MyConnect::DEBUG_MYCONNECT(Generic text) const{
	if (debug) {
		Serial.print("*DEBUG_MYCONNECT: ");
		Serial.println(text);
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Channel to print the ERROR.
template <typename Generic>
void MyConnect::ERROR_MYCONNECT(Generic text) const{
	Serial.print("!!ERROR_MYCONNECT: ");
	Serial.println(text);
}
