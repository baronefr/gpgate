//###################################################
//   gpgate - a gps-driven gate controller
//- - - - - - - - - - - - - - - - - - - - - - - - - -
//  coded by Francesco Barone | github.com/baronefr
//  under MIT licence
//---------------------------------------------------
//   This file implements the LED setup.
//---------------------------------------------------

#include "led.h"


// --------------------------------------
//  MY SETUP: a ring of 7 leds
//

// general properties
#define LED_TYPE         WS2812B
#define LED_COLOR_ORDER  GRB
#define LED_NUMBER       7
#define LED_BRIGHTNESS   20
#define LED_ANIMATION_FADE  40   

CRGB leds[LED_NUMBER];  // array of led colors
CRGB tmp_led_register;  // housekeeping led color

color_t led_color;      // current selected color

void (*led_animator)();   // pointer to animation function

void monochrome(unsigned long color) 
  // set the led ring to given color (monochrome pulse)
 { for(int i=0;i<LED_NUMBER-1;i++) leds[i]=color; }

CRGB my_white = CRGB(255,255,255);
 

// function to call at setup() to init the led array
void setup_led() {
  // LED setup
  FastLED.addLeds<LED_TYPE,PIN_LED,LED_COLOR_ORDER>(leds, LED_NUMBER).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(LED_BRIGHTNESS);
}


// function to set leds for sleep mode
void led_sleep_mode() {
  // turn off all leds
  FastLED.clear();
  FastLED.show();
}

// change the color of the leds
void led_set_color(color_t color) {
  led_color = color;
  monochrome(led_color);
}

// led when a signal is sent
void led_rc_signal() {
  leds[LED_NUMBER-1] = my_white;
}


#ifdef USE_LED_ANIMATION

unsigned int led_animation_counter;  // counter for animation update

//  animation patterns
void sinelon() 
 {  // animation 'cycle go and back'
  fadeToBlackBy(leds,LED_NUMBER, LED_ANIMATION_FADE);
  leds[beatsin16(12,0,LED_NUMBER-2)]=led_color;
 }

void spiral() 
 {  // animation 'full cycle'
  if(led_animation_counter>=(LED_NUMBER-1)*(LED_FPS/3)) led_animation_counter=0;
  fadeToBlackBy(leds,LED_NUMBER, LED_ANIMATION_FADE);
  leds[led_animation_counter++/(LED_FPS/3)]=led_color;
 }

void dual()
 {  // animation 'dual alternate'
  fadeToBlackBy(leds,LED_NUMBER, LED_ANIMATION_FADE); led_animation_counter++;
  if(led_animation_counter==LED_FPS/2) leds[1]=leds[0]=leds[5]=led_color;
  else if(led_animation_counter>=LED_FPS) { leds[2]=leds[3]=leds[4]=led_color; led_animation_counter=0; }
 }

void pulse()
 {  // animation 'impulse'
  fadeToBlackBy(leds,LED_NUMBER, LED_ANIMATION_FADE); 
  if(led_animation_counter++>=LED_FPS/2)
   { monochrome(led_color);  led_animation_counter=0; }
 }

 

// animation refresh
void led_set_animation() {
  switch(sys_state)
   {
    case SYS_UNFIX:
        led_animator = sinelon;  break;
    case SYS_FIXED:
        led_animator = spiral;   break;
    case SYS_MANUAL: 
        led_animator = dual;     break;
    case SYS_BOOT:
        led_animator = pulse;    break;
   }
  led_animation_counter=0;
}

// function for periodic callback (animation)
void led_update() {
  // call the current animation (through pointer)
  led_animator();
  
  // if any error, there will be a fixed central red led
  if(sys_error) leds[LED_NUMBER-1] = CRGB(255,0,0);
  
  FastLED.show();
 }


#endif
