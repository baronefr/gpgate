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
void init_led();

// In this file you have to implement some variables & methods
void led_set_color(color_t color);

#ifdef USE_LED_ANIMATION
    void led_update();                // update the led
    extern void led_set_animation();  // a function which updates the animation depending on system status
#endif




#endif
