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


// -----------------
//   debug tools
// -----------------
//#define DEBUG    // uncomment to enable debug mode (requires serial monitor)
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

#define PIN_BMAIN    2   // control button pin
//      ^^^^^   note:  check if this button can be used to wake up from sleep!
#define PIN_GPS_TX   7   // GPS tx-pin
#define PIN_GPS_RX   6   // GPS rx-pin
#define PIN_LED     A2   // LED clock contrl pin
#define PIN_RC      A4   // trasmitter pin

#define BUTTON_LONG_TIME   1000   // [ms] time to trigger long button action (manual mode)
#define BUTTON_CRIT_TIME   5000   // [ms] time to execute critical button keypress action (sleep mode)



// ----------------
//      SYSTEM
// ----------------
typedef unsigned char state_t;
extern state_t sys_state;
extern bool sys_error;
extern bool sys_sleep;

#define SYS_UNFIX  0     // gps is unfixed
#define SYS_FIXED  1     // gps is fixed
#define SYS_MANUAL 2     // manual mode (when fixed)
#define SYS_AUTO   3     // revert manual mode
#define SYS_SLEEP  5     // system sleep mode

#define SYS_BOOT   8     // boot
#define SYS_VOID   9     // dummy value (no meaning)


#define SYS_MANUAL_TIMEOUT 15000  // [ms] how much time does manual mode persist
#define SYS_AUTOSLEEP_DIST 10000  // [m]  minimum distance to auto-trigger sleep mode (undefine to disable auto-sleep)
#define SYS_SLEEP_RESET           // if defined, the system is reset after waking up from sleep mode


// ----------------
//       GPS
// ----------------
#define GPS_PRELIMINARY_TEST  2500    // [ms] time to wait in setup() to check gps module wiring
#define GPS_WAIT_TIME         1000    // [ms] refresh rate of GPS logic
#define GPS_NOT_STABLE_FIX_TIME 5000  // [ms] time to wait before saying that fix is no longer stable
#define GPS_STABILITY_THRES     3     // how many stable waypoints to wait before considering the fix is stable

//#define GPS_HDOP_THRES 10.0    // HDOP tolerance under which position is considered to be fixed [TODO]


// ----------------
//       LED
// ----------------
#define USE_LED_ANIMATION       // undefine to disable animations
#define LED_FPS 10              // be careful... high FPS may cause problems!
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

#define GATE_SWITCH_THRESH 3   // stable waypoints to wait before switching gate

#endif
