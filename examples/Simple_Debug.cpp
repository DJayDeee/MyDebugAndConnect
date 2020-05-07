/******************************************************************************************************
 * Name : Simple_Debug.cpp
 * Rev. : 1.0
 * Date : 2020-May-06
 * By   : Jean-Daniel Lavoie
 * Desc.: Test MyDebug tool minimum.
 * URL  : 
*//**AVAILABLE MACHINE STATES*************************************************************************/
#include <Arduino.h>
#include <MyDebugAndConnect.h>                      // https://github.com/DJayDeee/MyDebug
MyDebug mydebug(DEFAULT_BAUDRATE,DEFAULT_LED_PIN);	// Configure Serial baudrate and handle builtin led.


///////////////////////////////////////////////////////////////////////////////////////
// Prepare and configure the uC after power up or reset.
void setup(void){
  mydebug.Init();							                        	// Initiate Led and serial port.
  mydebug.setBlynkRate(0.3);
  Serial.println(F("Blink at 0.3 sec."));
  delay(20);
}

///////////////////////////////////////////////////////////////////////////////////////
// Do this forever!?
void loop(void){
  mydebug.led_Toggle();
}
