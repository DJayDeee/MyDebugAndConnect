/******************************************************************************************************
 * Name : Debug_n_Connect.cpp
 * Rev. : 1.0
 * Date : 2020-May-06
 * By   : Jean-Daniel Lavoie
 * Desc.: Test MyDebug and Myconnect tools.
 * URL  : 
******************************************************************************************************/
#include <Arduino.h>

#include <MyDebugAndConnect.h>                          // https://github.com/DJayDeee/MyDebug
MyConnect   myconnect(0);                               // factoreset_pin 0
MyDebug     mydebug(DEFAULT_BAUDRATE,DEFAULT_LED_PIN);  // Configure Serial baudrate and handle builtin led.

///////////////////////////////////////////////////////////////////////////////////////
// Prepare and configure the uC after power up or reset.
void setup(void){
  mydebug.Init();							            // Initiate Led and serial port.
  mydebug.setBlynkRate(0.3);
  Serial.println(F("SETUP: Power On WiFiManager portal."));
  myconnect.init();
  mydebug.handle(1);
}

///////////////////////////////////////////////////////////////////////////////////////
// Do this forever!?
void loop(void){
  mydebug.handle();
}
