//###################################################
//   gpgate - a gps-driven gate remote controller
//- - - - - - - - - - - - - - - - - - - - - - - - - -
//  coded by Francesco Barone | github.com/baronefr
//  under Open Access licence
//---------------------------------------------------
//   This file defines the general types and
//  configuration.
//---------------------------------------------------

#ifndef GPGATE_GENERAL_H
#define GPGATE_GENERAL_H

#include <stddef.h>


#define DEBUG    // uncomment to enable debug mode (requires serial monitor)
//#define DEBUG_MAX_TABLE_LINES 10  //if defined, will print a table with all distances from waypoints


// -----------------
//  general defines
// -----------------
typedef unsigned int uint;
typedef char const* label_t;
typedef char const* rcstr_t;
#define T_MAX_DOUBLE 30000.0


// ---------------
//   INTERFACE
// ---------------
#define USB_BAUD 115200  // USB baud rate for config purpose
#define GPS_BAUD   9600  // GPS module baud rate    //57600

#define PIN_BMAIN   A5   //control button pin  //NEED TO BE 2 or 3 for wake up! TODO
//#define PIN_BAUX  A4   //temporary 'change' button pin
#define PIN_GPS_TX  3    //GPS tx-pin
#define PIN_GPS_RX  2    //GPS rx-pin     // from other sketch: RXPin = 2, TXPin = 3;
#define PIN_LED     A2   //LED clock contrl pin
#define PIN_RC      A4   //trasmitter pin      //NOTE: A7 NOT WORKING!


// ----------------
//      SYSTEM
// ----------------
typedef unsigned char state_t;
extern state_t sys_state;
extern bool sys_error;

#define SYS_UNFIX  0     // gps is unfixed
#define SYS_FIXED  1     // gps is fixed
#define SYS_MANUAL 2     // manual mode (when fixed)
#define SYS_AUTO   3     // revert manual mode
#define SYS_SUSP   5     // system suspension
#define SYS_WAKEUP 6     // system wakeup from suspension

#define SYS_SUSP_DISTANCE  10000  // [m]  minimum distance to trigger suspension
#define SYS_MANUAL_TIMEOUT 10000  // [ms] how much time does manual mode persist

#define BUTTON_LONG_TIME   1000   // [ms] time to trigger the long button action
#define BUTTON_CRIT_TIME   5000   // [ms] time to execute critical button keypress action


// ----------------
//       GPS
// ----------------
#define GPS_WAIT_TIME 1000            // refresh rate of GPS when fixed
#define GPS_NOT_STABLE_FIX_TIME 5000  //how much time to wait before saying that fix is no longer stable
#define GPS_STABILITY_THRES 3
//#define GPS_HDOP_THRES 10.0  //HDOP tolerance under which position is considered to be fixed

// ----------------
//       LED
// ----------------
#define USE_LED_ANIMATION       // undefine to disable animations
#define LED_FPS 10
#define LED_ANIMATION_FADE 40        
typedef unsigned long color_t;

// ----------------
//      LOGIC
// ----------------
typedef struct _gate { label_t label; rcstr_t rcc; color_t color; } gate;
typedef unsigned char gateid_t;    // type to use as gate id
typedef struct _waypoint { double lat, lon; gateid_t id; label_t label; } waypoint;

extern gateid_t sys_current_gate;
extern const waypoint wps[];
extern const gate gates[];
extern const size_t nwps;
extern const size_t ngates;

#define GATE_SWITCH_THRESH 3

#endif
