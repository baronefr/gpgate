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


// -----------------
//  gate operations
// -----------------

void next_gate() {
  // cycle among available gates
  if((sys_current_gate++)==ngates) sys_current_gate=1;
  led_set_color( gates[sys_current_gate].color );
}

void send_rf(gateid_t id) {
  // send signal to selected gate
  rcsw.send( gates[id].rf );
  Serial.print("[rf] signal sent\n");
}

// -----------------
//    BUTTON LOGIC
// -----------------
void long_press_action() {
  BMAIN_long = true; Serial.println("[but] long press action!");
  if(sys_state==SYS_FIXED) system_update(SYS_MANUAL);
  else if(sys_state==SYS_MANUAL) TIC_status=millis();
  next_gate();
}

void short_press_action() {
   send_rf(sys_current_gate);
   if(sys_state==SYS_MANUAL) TIC_status=millis();
}

void button_handler() {
  // button operations
  if(digitalRead(PIN_BMAIN)==HIGH) {
    if(!BMAIN_active)
       { BMAIN_active=true;  TIC_button = millis(); }
    if( (millis()-TIC_button > BUTTON_LONG_TIME)&&(!BMAIN_long) ) long_press_action(); 
  } else {
    if(BMAIN_active)
     { if(BMAIN_long) BMAIN_long=false; else short_press_action();
       BMAIN_active=false; }
  }
  
 }



// -----------------
//    SYSTEM LOGIC
// -----------------

boolean sys_premanual_fixed = false;

uint system_update(state_t new_state) {
  // updates system state
  if(sys_state==new_state) return 0;
  
  switch(new_state) {
    #if DEBUG
    case SYS_UNFIX:
      Serial.println("[sys] switching to UNFIXED state");
      break;
    
    case SYS_FIXED:
      Serial.println("[sys] switching to FIXED state");
      break;
    #endif
    
    case SYS_MANUAL:
      sys_premanual_fixed = (sys_state==SYS_FIXED)?true:false;  // keep previous state in memory
     
      #if DEBUG
        Serial.println("[sys] switching to MANUAL state");
      #endif
      break;

    case SYS_AUTO:
      new_state = sys_premanual_fixed?SYS_FIXED:SYS_MANUAL;
      #if DEBUG
        Serial.println("[sys] Reverting manual mode");
      #endif
      break;
      
    case SYS_SUSP:
      #if DEBUG
        Serial.println("[sys] switching to SUSPENSION state");
      #endif
      /*tick_susp = millis();               TODO
      attachInterrupt(digitalPinToInterrupt(2),wakeup_animation,HIGH);
      delay(100);
      set_sleep_mode(SLEEP_MODE_PWR_DOWN);
      sleep_enable();
      sleep_mode();     //enters sleep

      //... restart from here...

      sleep_disable();
      sys_state=SYS_UNFIX;*/
      break;
  }

  sys_state = new_state;  // setting the new state
  led_set_animation();    // update led animation according to status
  TIC_status = millis();  // update time marker of status update
  return 1;
}


// -----------------
//    SLEEP LOGIC
// -----------------
unsigned int suspension_handler(double* dist) { }


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
  
  #if DEBUG
   if(printedlines > DEBUG_MAX_LINES) //prints intestation row
    {
     Serial.println();
     for(int i=0; i<nwps; i++) 
      { Serial.print(wps[i].id); Serial.print("\t"); }
     Serial.println(""); printedlines=0;
    }
  #endif
  
  for(unsigned char i=0; i<nwps; i++)    // computing distances to find minima
   {
    dist = TinyGPSPlus::distanceBetween(gps.location.lat(), gps.location.lng(), wps[i].lat, wps[i].lon);
    if(dist < min_dist) { min_id = i; min_dist=dist; }
    #if DEBUG
     Serial.print(dist, 3);  Serial.print("  "); 
    #endif
   } 
  
  #if DEBUG
   Serial.print(" -> min ");  Serial.println(wps[min_id].id);  printedlines++;
  #endif

  switch_logic( wps[min_id], &dist);
}


// -----------------
//    SWITCH LOGIC
// -----------------

gateid_t switch_candidate = 0;
unsigned char switch_counter = 0;

const size_t SUSP_MARKER = ngates + 1;

uint switch_logic(waypoint wp, double* dist) { // candidate waypoint & distance
  // decide change of current gate

  gateid_t cand_gate;  

  if(*dist > SYS_SUSP_DISTANCE) cand_gate = SUSP_MARKER;  // codify suspension
  else cand_gate = wp.id;   // retrieve gate id from struct
  
  if(cand_gate == switch_candidate) switch_counter++;
  else { switch_counter = 0;  switch_candidate = cand_gate; return 0; }

  if(switch_counter < GATE_SWITCH_THRESH) return 0; // wait...
  else switch_counter = GATE_SWITCH_THRESH;  // ... until threshold counter
  
  if(sys_state==SYS_MANUAL)   return 0;  // in manual mode don't do anything
  else if(cand_gate==sys_current_gate) return 0;  // no need to change if currently in right gate!
  else if(cand_gate==SUSP_MARKER) {
    system_update(SYS_SUSP);
  }
  else {
    sys_current_gate = cand_gate;
    #if DEBUG
      Serial.print("[log] Switched to G");  Serial.println(cand_gate);
    #endif
    led_set_color( gates[cand_gate].color );
  }
  
  return 1;
}
