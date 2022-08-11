//###################################################
//   gpgate - a gps-driven gate remote controller
//- - - - - - - - - - - - - - - - - - - - - - - - - -
//  coded by Francesco Barone | github.com/baronefr
//  under Open Access licence
//---------------------------------------------------
//   This file defines the logical functions
//  behind gpgate.
//---------------------------------------------------

#ifndef GPGATE_LOGIC_H
#define GPGATE_LOGIC_H

#include "general.h"
#include "led.h"

#include <Arduino.h>
#include <RCSwitch.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <LowPower.h>

extern RCSwitch rcsw;
extern TinyGPSPlus gps;
extern SoftwareSerial s_gps;  // to disable interface before sleep mode (ISSUE)

// tic-toc variables
extern unsigned long TIC_gps;     // time of last gps update
extern unsigned long TIC_led;     // time of last led update
extern unsigned long TIC_status;  // time of last system status change
extern unsigned long TIC_button;  // keypress time

void button_handler();

// logic methods
uint system_update(state_t new_state);
void waypoint_handler();

#endif
