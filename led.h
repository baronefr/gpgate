//###################################################
//   gpgate - a gps-driven gate remote controller
//- - - - - - - - - - - - - - - - - - - - - - - - - -
//  coded by Francesco Barone | github.com/baronefr
//  under Open Access licence
//---------------------------------------------------
//   This file defines the LED interface.
//---------------------------------------------------

#ifndef GPGATE_LED_H
#define GPGATE_LED_H


#include "general.h"
#include <FastLED.h>

// method to call in setup()
void setup_led();


// default methods
void led_set_color(color_t color);
void led_rc_signal();
void led_sleep_mode();


// animation methods
#ifdef USE_LED_ANIMATION
  void led_update();             // update the led (periodic callback)
  void led_set_animation();      // set the animation depending on system status
#else
  #undef LED_FPS
#endif




#endif
