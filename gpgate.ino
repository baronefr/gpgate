//###################################################
//   gpgate - a gps-driven gate controller
//---------------------------------------------------
//  coded by Francesco Barone, @ github.com/baronefr
//  under Open Access licence
//---------------------------------------------------

#define VER "v1 alpha (nano)" // software version signcode
// note: to use DEBUG prints, define DEBUg level in general.h

#include "general.h"
#include "led.h"
#include "logic.h"

#include "mygate.h"  // the file with your gates & waypoints


#include <RCSwitch.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>


// hardware
RCSwitch rcsw = RCSwitch();
TinyGPSPlus gps;
SoftwareSerial s_gps(PIN_GPS_RX, PIN_GPS_TX);


// system
state_t sys_state = SYS_UNFIX;
bool sys_error = false;
gateid_t sys_current_gate;

// tic toc variables
unsigned long TIC_gps;     // time of last gps update
unsigned long TIC_led;     // time of last led update
unsigned long TIC_status;  // time of last system status change
unsigned long TIC_button;  // keypress time


// button 
//  |  handling  BMAIN_* -> main button
//  |            BAUX_*  -> auxiliary button (if implemented)

boolean BMAIN_active = false;  //if true, main button has been activated
boolean BMAIN_long = false;    //if true, main button has been longpressed
//boolean BAUX_active = false;   //if true, aux button has been activated



// ---------------
//       MAIN
// ---------------

void setup() {

  // hardware setup
  pinMode(PIN_BMAIN, INPUT);

  // led (see class impl.)
  init_led();
  led_set_color(gates[0].color);  // default gate is the first declared
  
  // transmitter
  rcsw.enableTransmit(PIN_RC);
  rcsw.setProtocol(RC_PROTOCOL);
  rcsw.setPulseLength(RC_PULSELEN);
  //rcsw.setRepeatTransmit(15);
  
  // gps
  s_gps.begin(GPS_BAUD);

  // state machine
  sys_state = 10;      // dummy value
  system_update(SYS_UNFIX);
  sys_error = false;
  TIC_gps = millis();

  // usb serial
  #ifdef DEBUG
    Serial.begin(USB_BAUD);
    Serial.println("-- gpgate --"); Serial.print(VER);
    Serial.println(", by Francesco Barone");
    Serial.println(" > debug mode <");
  #endif

  delay(5000);  // wait for GPS module check
  // GPS error handle
  if (millis() > 7000 && !sys_error && gps.charsProcessed() < 10) {
    #ifdef DEBUG
      Serial.println("[ERR] No GPS detected, check wiring");
    #endif
    sys_error = true;
  }
}


void loop() {
  // revert manual mode if timeout
  if((sys_state==SYS_MANUAL) && (millis()-TIC_status>SYS_MANUAL_TIMEOUT))
    system_update(SYS_AUTO);   // exit from manual mode

  if(s_gps.available()) gps.encode(s_gps.read());

  #ifdef USE_LED_ANIMATION
  if(millis()-TIC_led > 1000/LED_FPS)
    {  led_update();  TIC_led = millis(); }
  #endif
  
  button_handler();
  
  // GPS action
  if( millis()-TIC_gps > GPS_WAIT_TIME ) {
    if(gps.location.isValid()) waypoint_handler();
    else if( (sys_state==SYS_FIXED) && (millis()-TIC_gps>GPS_NOT_STABLE_FIX_TIME)) system_update(SYS_UNFIX);
    #ifdef DEBUG
      else { Serial.println("  [gps] still not fixed"); TIC_gps = millis(); }
    #endif
  }
}
