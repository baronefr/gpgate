//###################################################
//   gpgate - a gps-driven gate controller
//---------------------------------------------------
//  coded by Francesco Barone, @ github.com/baronefr
//  under Open Access licence
//---------------------------------------------------

#define VER "v1 alpha (nano)" //software version signcode
#define DEBUG 1    // if 1 -> activates debug mode
// note: debug mode requires serial monitor


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

  // led
  init_led();        // see class impl.
  led_set_color(gates[0].color);
  
  // transmitter
  rcsw.enableTransmit(PIN_RF);
  rcsw.setProtocol(RF_PROTOCOL);
  rcsw.setPulseLength(RF_PULSELEN);
  //rcsw.setRepeatTransmit(15);
  
  // gps
  s_gps.begin(GPSBAUD);

  // state machine
  sys_state = 10;
  system_update(SYS_UNFIX);
  sys_error = false;

  // usb serial
  #if DEBUG
    Serial.begin(USB_BAUD);
    Serial.println("-- gpgate --"); Serial.print(VER);
    Serial.println(", created by Francesco Barone");
    Serial.println(" > debug mode <");
  #endif
}


void loop() {
  // revert manual mode if timeout
  if((sys_state==SYS_MANUAL) && (millis()-TIC_status>SYS_MANUAL_TIMEOUT)) 
    system_update(SYS_AUTO);   // exit from manual mode

  // execute button handler
  button_handler();

  // alternates LED update & GPS signal verification
  if(millis()-TIC_led > 1000/LED_FPS) led_update();
  else if(s_gps.available()) gps.encode(s_gps.read());
  
  // GPS action
  if( millis()-TIC_gps > GPS_WAIT_TIME ) {
    if(gps.location.isValid()) waypoint_handler();
    else if( (sys_state==SYS_FIXED) && (millis()-TIC_gps>GPS_NOT_STABLE_FIX_TIME)) system_update(SYS_UNFIX);
  }
  
  // GPS error handle
  if ( !sys_error && millis() > 5000 && gps.charsProcessed() < 10 ) {
    #if DEBUG
      Serial.println(F("[ERR] No GPS detected, check wiring"));
    #endif
    sys_error = true;
  }

}
