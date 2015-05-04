/*
  Microphone
  A class for handling microphone input.
  Uses Timer1 while running or listening so you can't use it for anything else.  
  Author: Ron Smith, That Aint Working
  Created: April 2015
*/
 
#include "Microphone.h"
#include <Arduino.h>
#include <TimerOne.h>

#define BUF_LEN   10
#define BANDS     3
#define BAND_SIZE 4

_Microphone Microphone;


void mic_input_isr() {
    Microphone.counter++;
}


void mic_check_isr() {
    Microphone.check();
}


const int itod_pin[2] = {2, 3};


_Microphone::_Microphone() : interrupt_pin(-1), on_tone_rising(NULL), on_tone_falling(NULL), on_mic_check(NULL), on_nsr(NULL) {
    buf = new unsigned int[BUF_LEN];
}


void _Microphone::start(int intrpt_pin, unsigned long intrvl, unsigned int nsr) {
    interrupt_pin = intrpt_pin;
    interval = intrvl;
    nsreset = nsr;
    reset();
    set_interrupts();
    Serial.println("Microphone started");
}

void _Microphone::restart() {
    set_interrupts();
    Serial.println("Microphone restarted");
}


void _Microphone::stop() {
    Timer1.detachInterrupt();
    detachInterrupt(interrupt_pin);
    Serial.println("Microphone stopped");
}


void _Microphone::dump_buffer() {
    for (int b = 0; b < BUF_LEN; b++) {
        if (b > 0) Serial.print(", ");
        Serial.print(buf[b]);
    }
    Serial.println();
}


void _Microphone::set_interrupts() {
    Serial.println("Microphone setting interrupts");

    // call the mic_in_isr() function whenever digital pin 2 goes from LOW to HIGH
    //pinMode(itod_pin[interrupt_pin], INPUT_PULLUP);
    attachInterrupt(interrupt_pin, mic_input_isr, FALLING);
  
    // Setup a timer interrupt to handling the microphone data
    Timer1.initialize(interval);
    Timer1.attachInterrupt(mic_check_isr);
}


int updown(int v0, int v1) {
    if (v0 < v1) return 1;
    if (v0 > v1) return -1;
    return 0;
}


void _Microphone::evaluate_tone() {
    int b, i, j, t, h[BANDS], l[BANDS], hs[BANDS-1], ls[BANDS-1];
    dump_buffer();
    for (b = 0; b < BANDS; b++) {
        h[b] = 0;
        l[b] = 2000;
        for (i = BAND_SIZE * b - b; i < BAND_SIZE * (b+1) - b; i++) {
            h[b] = max(h[b], buf[i]);
            l[b] = min(l[b], buf[i]);
        }
        if (b > 0) Serial.print(" / ");
        Serial.print(h[b]);
        Serial.print(", ");
        Serial.print(l[b]);
    }
    Serial.println();
    t = 0;
    for (j = 0; j < BANDS-1; j++) {
        t += updown(h[j], h[j+1]) + updown(l[j], l[j+1]);
    }
    Serial.println(t);
    if (t < 0 && on_tone_falling) (*on_tone_falling)();
    else if (t > 0 && on_tone_rising) (*on_tone_rising)();
}


void _Microphone::check() {
    noInterrupts();
    if (on_mic_check) (*on_mic_check)();
    if (counter > 0) {
        buf[bufidx++] = counter;
        counter = 0;
        nscounter = 0;
        if (bufidx >= BUF_LEN) {
            evaluate_tone();
            reset();
        }
    } else {
        if (++nscounter > nsreset) {
            if (bufidx > 7) { // close enough, pad it out
                for (int b = bufidx; b < BUF_LEN; b++)
                    buf[b] = buf[bufidx];
                bufidx = BUF_LEN;
                evaluate_tone();
            } else if (on_nsr) {
                (*on_nsr)();
            }
            reset();
        }
    }
    interrupts();
}


void _Microphone::reset() {
    counter = 0;
    nscounter = 0;
    for (bufidx = 0; bufidx < BUF_LEN; bufidx++) buf[bufidx] = 0;
    bufidx = 0;
}

