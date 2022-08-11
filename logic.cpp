//###################################################
//   gpgate - a gps-driven gate controller
//- - - - - - - - - - - - - - - - - - - - - - - - - -
//  coded by Francesco Barone | github.com/baronefr
//  under Open Access licence
//---------------------------------------------------
//   This file implements the logical functions
//  behind gpgate.
//---------------------------------------------------

#include "logic.h"


// button variables
//  |  handling  BMAIN_* -> main button
boolean BMAIN_active = false;  // if true, main button has been activated
boolean BMAIN_long = false;    // if true, main button has been longpressed
boolean BMAIN_crit = false;    // if true, main button pressed for a very long time...

unsigned long TIC_button;  // keypress time


// -----------------
//  gate operations
// -----------------

void next_gate() {
  // cycle among available gates
  if((sys_current_gate++)==ngates) sys_current_gate=1;
  led_set_color( gates[sys_current_gate-1].color );
}

void rc_send(gateid_t id) {
  // send signal to selected gate
  rcsw.send( gates[id-1].rcc );
  led_rc_signal();
  #ifdef DEBUG
   Serial.print("[rc] signal sent\n");
  #endif
}



// -----------------
//    BUTTON LOGIC
// -----------------

// action for short button pressure
void button_action() {
  // send the signal to current gate
  rc_send(sys_current_gate);
  if(sys_state==SYS_MANUAL) TIC_status=millis();
}

// action for long button pressure
void button_keypress() {
  #ifdef DEBUG
   Serial.println("[btt] long press");
  #endif
  if(sys_state==SYS_FIXED) system_update(SYS_MANUAL);
  else if(sys_state==SYS_MANUAL) TIC_status=millis();
  next_gate();
}

// action for very long button pressure
void button_critical() {
  #ifdef DEBUG
   Serial.println("[btt] critical press");
  #endif
  system_update(SYS_SLEEP);
}

void button_handler() {
  unsigned long this_time = millis();
  
  // handle the button keypress
  if(digitalRead(PIN_BMAIN)==HIGH) {
    
    if(!BMAIN_active)  // register time of activation
      { BMAIN_active=true;  TIC_button = this_time; }
    
    if( this_time-TIC_button > BUTTON_LONG_TIME )
      if(!BMAIN_long) { BMAIN_long = true;   button_keypress(); }
      if(!BMAIN_crit && (this_time-TIC_button > BUTTON_CRIT_TIME))
        {  BMAIN_crit = true; button_critical();  }
    
  } else {
    
    if(BMAIN_active) {
      
      if(BMAIN_long)
        { BMAIN_long=false;    }
      else button_action();
      
      if(BMAIN_crit) BMAIN_crit=false;
      BMAIN_active=false; 
    }
    
  }
  
}

// -----------------
//    SLEEP LOGIC
// -----------------

#ifdef SYS_SLEEP_RESET
void(* my_reboot)(void) = 0;  // reboot the system after wakeup
#endif

void wakeUp() {};  



// -----------------
//    SYSTEM LOGIC
// -----------------

boolean sys_premanual_fixed = false;

uint system_update(state_t new_state) {
  // updates system state
  if(sys_state==new_state) return 0;
  
  switch(new_state) {
    
    #ifdef DEBUG  // no need to do something in particular for these...
    case SYS_BOOT:
      Serial.println("[sys] boot");
      break;
      
    case SYS_UNFIX:
      Serial.println("[sys] switching to UNFIXED state");
      break;
    
    case SYS_FIXED:
      Serial.println("[sys] switching to FIXED state");
      break;
    #endif
    
    case SYS_MANUAL:
      sys_premanual_fixed = (sys_state==SYS_FIXED)?true:false;  // keep previous state in memory
     
      #ifdef DEBUG
        Serial.println("[sys] switching to MANUAL state");
      #endif
      break;

    case SYS_AUTO:
      new_state = sys_premanual_fixed?SYS_FIXED:SYS_MANUAL;
      #ifdef DEBUG
        Serial.println("[sys] reverting manual mode");
      #endif
      break;
      
    case SYS_SLEEP:
      #ifdef DEBUG
        Serial.println("[sys] switching to sleep state");
      #endif
      
      TIC_status = millis();
      led_sleep_mode();
      sys_state = SYS_SLEEP;


      // -----------------------
      // entering SLEEP MODE
      
      s_gps.end();  // [ISSUE] need to disable serial to enter sleep mode
      delay(1000);
    
      // set wakeup pin
      attachInterrupt(0, wakeUp, HIGH);
    
      // power down state (ADC & BOD module disabled)
      LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 


      // -----------------------
      // revert from SLEEP MODE
      
      // Disable external pin interrupt on wake up pin.
      detachInterrupt(0);

      #ifdef SYS_SLEEP_RESET
      my_reboot();
      #endif
    
      s_gps.begin(GPS_BAUD);  // [ISSUE] restore gps serial
      new_state = SYS_UNFIX;
      led_set_color( gates[sys_current_gate-1].color );
      break;
  }

  sys_state = new_state;  // setting the new state
  led_set_animation();    // update led animation according to status
  TIC_status = millis();  // update time marker of status update
  return 1;
}


// -----------------
//   WAYPOINT LOGIC
// -----------------
unsigned char stable_counter = 0;
unsigned char printedlines = 0;

uint switch_logic(waypoint wp, double* dist); // defined below

void waypoint_handler() {
  
  double dist;    // distance used in the loop
  double min_dist=T_MAX_DOUBLE;  unsigned char min_id = 0;  // to store the minima

  if(sys_state == SYS_UNFIX)
   {
    if(millis()-TIC_gps < (GPS_STABILITY_THRES-0.5)*GPS_WAIT_TIME) stable_counter++; 
    else stable_counter = 0;
    if(stable_counter==GPS_STABILITY_THRES) system_update(SYS_FIXED);
   }
  TIC_gps=millis();

  #ifdef DEBUG
  #ifdef DEBUG_MAX_TABLE_LINES
   if(printedlines > DEBUG_MAX_TABLE_LINES) // prints intestation row
    {
     Serial.println();
     for(int i=0; i<nwps; i++) 
      { Serial.print(wps[i].id); Serial.print("\t"); }
     Serial.println(""); printedlines=0;
    }
  #endif
  #endif
  
  for(unsigned char i=0; i<nwps; i++)    // computing distances to find minima
   {
    dist = TinyGPSPlus::distanceBetween(gps.location.lat(), gps.location.lng(), wps[i].lat, wps[i].lon);
    if(dist < min_dist) { min_id = i; min_dist=dist; }
    #ifdef DEBUG
    #ifdef DEBUG_MAX_TABLE_LINES
     Serial.print(dist, 3);  Serial.print("  "); 
    #endif
    #endif
   } 
  
  #ifdef DEBUG
  #ifdef DEBUG_MAX_TABLE_LINES
   Serial.print(" -> min ");  Serial.println(wps[min_id].id);  printedlines++;
   #else
   Serial.print("  [gps] closest WP is ");  Serial.print(wps[min_id].label);
   Serial.print(" -> gate ");        Serial.println(gates[wps[min_id].id-1].label);
  #endif
  
  #endif
  
  switch_logic( wps[min_id], &dist);
}


// -----------------
//    SWITCH LOGIC
// -----------------

gateid_t switch_candidate = 0;
unsigned char switch_counter = 0;

#ifdef SYS_AUTOSLEEP_DIST
const size_t SLEEP_MARKER = ngates + 1;
#endif

uint switch_logic(waypoint wp, double* dist) {
  // manage a change of current gate
  // args:  candidate waypoint & distance

  gateid_t cand_gate;  

  if(*dist < SYS_AUTOSLEEP_DIST) cand_gate = wp.id; // retrieve gate id from struct
  #ifdef SYS_AUTOSLEEP_DIST
  else cand_gate = SLEEP_MARKER;  // codify sleep (if auto-sleep is enabled)
  #endif
  
  if(cand_gate == switch_candidate) switch_counter++;
  else { switch_counter = 0;  switch_candidate = cand_gate; return 0; }

  if(switch_counter < GATE_SWITCH_THRESH) return 0; // wait...
  else switch_counter = GATE_SWITCH_THRESH;  // ... until threshold counter
  
  if(sys_state==SYS_MANUAL)   return 0;  // in manual mode don't do anything
  else if(cand_gate==sys_current_gate) return 0;  // no need to change if currently in right gate!

  #ifdef SYS_AUTOSLEEP_DIST
  else if(cand_gate==SLEEP_MARKER)  system_update(SYS_SLEEP);
  #endif
  
  else {
    sys_current_gate = cand_gate;
    #ifdef DEBUG
      Serial.print("[log] Gate switched to G#");  Serial.println(gates[cand_gate-1].label);
    #endif
    led_set_color( gates[cand_gate-1].color );
  }
  
  return 1;
}
