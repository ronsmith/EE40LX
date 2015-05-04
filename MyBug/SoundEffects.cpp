/*
  SoundEffects
  A class for generating simple sound effects.
  Author: Ron Smith, That Aint Working
  Created: April 2015
*/


#include "SoundEffects.h"
#include <Arduino.h>

_SoundEffects SoundEffects;
 
void _SoundEffects::init(int speaker_pin) {
    this->speaker_pin = speaker_pin;
}


void _SoundEffects::init(int speaker_pin, ba_func_p before_func, ba_func_p after_func) {
    this->speaker_pin = speaker_pin;
    this->before_func = before_func;
    this->after_func = after_func;    
}


void _SoundEffects::uh_oh() {
    Serial.println("SoundEffects::uh_oh");
}


void _SoundEffects::oh_no() {
    Serial.println("SoundEffects::oh_no");
}


void _SoundEffects::ahhh() {
    Serial.println("SoundEffects::ahhh");
}


void _SoundEffects::slide(int from_pitch, int to_pitch) {
    if (before_func) (*before_func)();
    int d = (from_pitch > to_pitch ? -1 : 1);
    Serial.print("SoundEffects::slide ");
    Serial.println(d < 0 ? "down" : "up");
    for (int x = from_pitch; x != to_pitch ; x += d) {
        tone(speaker_pin, x);
        delay(1);
    }
    delay(10);
    noTone(speaker_pin);
    if (after_func) (*after_func)();
}


void _SoundEffects::two_tone(int tone1, int tone2) {
    if (before_func) (*before_func)();
    Serial.println("SoundEffects::two_tone");
    tone(speaker_pin, tone1);
    delay(200);
    tone(speaker_pin, tone2);
    delay(200);
    noTone(speaker_pin);
    if (after_func) (*after_func)();
}


void _SoundEffects::sequence(int len, int tones[], int durations[]) {
    Serial.println("SoundEffects::sequence");
}


