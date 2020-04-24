/****************************************************************************************
* File :		MyDebugAndConnect.cpp
* Date :		2018-Oct-10
* By :			Jean-Daniel Lavoie
* Description :	This library configure the serial port and one led for debuging.
*				It also allow to control the led (off, on and toggle).
****************************************************************************************/

#include	"MyDebugAndConnect.h"

int	mydebug::led_pin = DEFAULT_LED_PIN;
void Toggle(void) { digitalWrite(mydebug::led_pin, !digitalRead(mydebug::led_pin));	}

// Constructor : 
mydebug::mydebug(const int br, const int pin) {
	led_pin = pin;
	baudrate = br;
}

void mydebug::Init(void) {
 	pinMode(led_pin, OUTPUT);
	blynker.attach(0.6, Toggle);
	
	Serial.begin(baudrate);
	Serial.println(" ");
}

void mydebug::led_OFF(void) {
	digitalWrite(led_pin, HIGH);
}

void mydebug::led_ON(void) {
	digitalWrite(led_pin, LOW);
}

void mydebug::led_Toggle(void) {
	digitalWrite(led_pin, !digitalRead(led_pin));
}
	
void mydebug::setBlynkRate(const float seconds) {
	if(seconds)								// If blynk rate is 0 stop the blynker and let the led on.
		blynker.attach(seconds, Toggle);
	else {
		blynker.detach();
		led_OFF();
	}
}
	
void mydebug::handleWifiBlynk(bool force_actualise)	{
	if ( (WiFi.isConnected() != last_wifistate) | force_actualise ){	// Change led blynk state only if WiFi status change.
		last_wifistate = WiFi.isConnected();
		if (!last_wifistate) {				// Led blynk slowly if WiFi is disconnected.
		  setBlynkRate(1);
		  WiFi.begin();
		} else								// Led OFF solid if WiFi is connected.
		  setBlynkRate(0);
	}
}




/*=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=*/
/*=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=*/
/*=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=|=*/
///////////////////////////////////////////////////////////////////////////////////////
// Constructor :
myconnect::myconnect(const int pin) {
	factoreset_pin = pin;
}


///////////////////////////////////////////////////////////////////////////////////////
// Called to  restore and initialise WiFiManager.
void myconnect::init(void) {	
	pinMode(factoreset_pin, INPUT);
	if(!readConfigFile(CONFIG_FILE))				// Restore MQTT parameters from FileSystem.
		DEBUG_MYCONNECT(F("==>CONTINUE WITHOUT RESTORING SETTINGS<=="));
	
	wm.setSaveConfigCallback(state_queue.push(FORCEWIFIMANAGER););	// Callback to set shouldSaveConfig flag only if you it save button in portal.
//	wm.setShowStaticFields(true);					// force show static ip fields
	wm.setConnectTimeout(20);						// how long to try to connect for before continuing													// setConfigPortalTimeout is ignored in this mode, user is responsible for closing configportal

	DEBUG_MYCONNECT(F("# MYCONNECT initialized #"));
}


///////////////////////////////////////////////////////////////////////////////////////
// Called to restore MQTT parameters from the SPIFF CONFIG_FILE.
// Return true if success.
bool myconnect::readConfigFile(const char *filename) {
	Serial.println(F("*<--===| Restoring config file |===-->"));

	if(!SPIFFS.begin()){
		Serial.println(F("==>ERROR<==  Unable to mount SPI Flash File System."));
		return false;
	}
	if(!SPIFFS.exists(filename)) {
		Serial.printf("==>ERROR<==  Configuration file \"%s\" does not exist.\n", filename);
		SPIFFS.end();
		return false;
	}
	File f = SPIFFS.open(filename, "r");
	if(!f) {
		Serial.printf("==>ERROR<==  Unable to open \"%s\" to read configuration.\n", filename);
		SPIFFS.end();
		return false;
	}
	size_t size = f.size();								// Allocate and store contents of CONFIG_FILE in a buffer.
	std::unique_ptr<char[]> buf(new char[size]);
	f.readBytes(buf.get(), size);
	f.close();											// Closing file and unmount SPIFlashFileSystem.
	SPIFFS.end();

	DynamicJsonBuffer jsonBuffer;						// Using dynamic JSON buffer.
	JsonObject& json = jsonBuffer.parseObject(buf.get()); // Parse JSON string.
	if(!json.success()) {
		Serial.println(F("==>ERROR<==  JSON parseObject() failed."));
		return false;
	}
				
	if(json.containsKey(String(F("server"))))			// Parse all parameters and override local variables.
		strcpy(MQTT_credential.server, json[String(F("server"))]);
	if(json.containsKey(String(F("port"))))
		MQTT_credential.port = json[String(F("port"))];
	if(json.containsKey(String(F("username"))))
		strcpy(MQTT_credential.username, json[String(F("username"))]);
	if(json.containsKey(String(F("password"))))
		strcpy(MQTT_credential.password, json[String(F("password"))]);
	if(json.containsKey(String(F("clientID"))))
		strcpy(MQTT_credential.clientID, json[String(F("clientID"))]);
	if(json.containsKey(String(F("topic"))))
		strcpy(MQTT_credential.topic, json[String(F("topic"))]);

	Serial.printf("Configuration from file \"%s\" :\n", filename); // Print what will be restore from the CONFIG_FILE.
	json.prettyPrintTo(Serial);
	Serial.println("***RESTORED***");
	DEBUG_MYCONNECT("");
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////
// Called to save MQTT parameters to CONFIG_FILE in the FileSystem
// Return true if success.
bool myconnect::writeConfigFile(const char *filename) {
	Serial.println(F("<--===| Saving config file |===-->"));

	if(!SPIFFS.begin()){
		Serial.println(F("==>ERROR<==  Unable to mount SPI Flash File System."));
		return false;
	}
	File f = SPIFFS.open(filename, "w");
	if(!f) {
		Serial.printf("==>ERROR<==  Failed to open \"%s\" for writing configuration.\n", filename);
		SPIFFS.end();
		return false;
	}

	DynamicJsonBuffer jsonBuffer;						// Using dynamic JSON buffer which is not the recommended memory model, but anyway, See https://github.com/bblanchon/ArduinoJson/wiki/Memory%20model
	JsonObject& json = jsonBuffer.createObject();		// Create JSON string.
	json[String(F("server"))]	= MQTT_credential.server;
	json[String(F("port"))]		= MQTT_credential.port;
	json[String(F("clientID"))]	= MQTT_credential.clientID;
	json[String(F("username"))]	= MQTT_credential.username;
	json[String(F("password"))]	= MQTT_credential.password;
	json[String(F("topic"))]	= MQTT_credential.topic;
	
	json.printTo(f);									// Write data to file, close it and unmount SPIFlashFileSystem.
	f.close();
	SPIFFS.end();

	Serial.printf("Configuration file \"%s\" :\n", filename);
	json.prettyPrintTo(Serial);
	Serial.println("***SAVED***");
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////
// Called to configure and save the WiFi credentials and MQTT parameters.
void myconnect::StartManager(int timeout) {
	wm.setConfigPortalTimeout(timeout);				// auto close configportal after n seconds
	if(!wm.startConfigPortal())						// auto generated AP name from chipid
		DEBUG_MYCONNECT(F("Failed to connect or hit timeout"));
}

///////////////////////////////////////////////////////////////////////////////////////
// Called to reset the WiFi credentials.
void myconnect::ResetWiFi(void) {
	DEBUG_MYCONNECT(F("!ERASING WiFi parameters and restart!"));
	wm.resetSettings();
	state_queue.push(RESTART);
}


///////////////////////////////////////////////////////////////////////////////////////
// Channel to print the debug.
template <typename Generic>
void myconnect::DEBUG_MYCONNECT(Generic text) const{
	Serial.print("*DEBUG_MYCONNECT: ");
	Serial.println(text);
}