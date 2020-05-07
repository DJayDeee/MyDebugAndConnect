/******************************************************************************************************
 * Name : Simple_Connect.cpp
 * Rev. : 1.0
 * Date : 2020-May-06
 * By   : Jean-Daniel Lavoie
 * Desc.: WiFiManager + Saved Parameters file
 * URL  : 
******************************************************************************************************/
#include <Arduino.h>

#include <MyDebugAndConnect.h>    // https://github.com/DJayDeee/MyDebug
MyConnect myconnect(0);           // factoreset_pin 0

///////////////////////////////////////////////////////////////////////////////////////
// Prepare and configure the uC after power up or reset.
void setup(void){
  myconnect.init();
}

///////////////////////////////////////////////////////////////////////////////////////
// Do this forever!?
void loop(void){
}
