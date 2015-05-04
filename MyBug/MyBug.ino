
/*
  MyBug
  This is the program to control My Bug.
  Final project for edX EE40LX.
  Author: Ron Smith, That Aint Working
  Created: April 2015
*/

#include <stdio.h>

// must include TimerOne in the .ino so it gets added to the compile path so Microphone can use it
// NOTE: this appears to block PWM on pins 9-11, not just 9-10 as the documentation suggests
#include <TimerOne.h>    

#include "Microphone.h"
#include "SoundEffects.h"
#include "Eyes.h"

#define EYE_POWER     17       // physical pin 26 aka A3 but used for digital out
#define LEFT_EYE      A2       // physical pin 25 aka digital 16
#define RIGHT_EYE     A1       // physical pin 24 aka digital 15
#define LOOK_INTERVAL 1000L
#define EYE_DIFF      200

#define MIC_INT       0        // INT0 = digital 2 = physical pin 4
#define MIC_INTERVAL  100000L
#define NS_RESET      3

#define SPEAKER       13       // physical pin 19

#define MOTOR_LEFT    5        // physical pin 11
#define MOTOR_RIGHT   6        // physical pin 12
#define MOTOR_BOTH    56

#define LED_LEFT      18       // physical pin 27 aka A4 but used for digital out
#define LED_RIGHT     10       // physical pin 16

#define LED_YELLOW    LED_LEFT        
#define LED_GREEN     LED_RIGHT

#define HALF_SECOND   500
#define ONE_SECOND    1000
#define TWO_SECONDS   2000


char sbuf[100];

volatile int tone_dir = 0;

Eyes* eyes;
int prev_eyes_avg;
unsigned long next_look;


void tone_rising() {
    Serial.println("Rising!");
    tone_dir = 1;
}


void tone_falling() {
    Serial.println("Falling!");
    tone_dir = -1;
}

void mic_check() {
    Microphone.dump_buffer();
}

void no_sound_recv() {
    Serial.println("Silent");
}

void stop_mic() {
    Microphone.stop();
}

void restart_mic() {
    Microphone.restart();
}

void setup() {
    Serial.begin(9600);
    Serial.println("Begin setup");
    
    pinMode(SPEAKER, OUTPUT);
    pinMode(LED_LEFT, OUTPUT);
    pinMode(LED_RIGHT, OUTPUT);
    
    SoundEffects.init(SPEAKER, &stop_mic, &restart_mic);
    
    digitalWrite(LED_LEFT, HIGH);
    tone(SPEAKER, 500);
    delay(500);
    digitalWrite(LED_LEFT, LOW);
    
    Serial.println("Initializing eyes");
    eyes = new Eyes(EYE_POWER, LEFT_EYE, RIGHT_EYE);
    prev_eyes_avg = eyes->avg();
    next_look = millis() + LOOK_INTERVAL;

    digitalWrite(LED_RIGHT, HIGH);
    tone(SPEAKER, 750);
    delay(500);
    digitalWrite(LED_RIGHT, LOW);
    
    Serial.println("Initializing microphone");
    Microphone.start(MIC_INT, MIC_INTERVAL, NS_RESET);
    Microphone.on_tone_rising = &tone_rising;
    Microphone.on_tone_falling = &tone_falling;
    //Microphone.on_mic_check = &mic_check;
    //Microphone.on_nsr = &no_sound_recv;

    digitalWrite(LED_LEFT, HIGH);
    digitalWrite(LED_RIGHT, HIGH);
    tone(SPEAKER, 1000);
    delay(500);
    noTone(SPEAKER);
    digitalWrite(LED_RIGHT, LOW);
    digitalWrite(LED_LEFT, LOW);
    
    Serial.println("Setup complete");
}

void motor_start(int motor_pin, int speed) {
    analogWrite(motor_pin, 250); // motor needs a little kickstart
    delay(10);
    analogWrite(motor_pin, speed);
    Serial.print(motor_pin == MOTOR_LEFT ? "Left" : "Right");
    Serial.println(" motor on");
}

void drive_motors(unsigned int motor_select, unsigned int duration) {
    // need to stop listening while motor runs -- it detects vibration as sound
    Microphone.stop();
    
    if (motor_select == MOTOR_LEFT || motor_select == MOTOR_BOTH) {
        motor_start(MOTOR_LEFT, 100);
        digitalWrite(LED_LEFT, HIGH);
    }
    if (motor_select == MOTOR_RIGHT || motor_select == MOTOR_BOTH) {
        motor_start(MOTOR_RIGHT, 100);
        digitalWrite(LED_RIGHT, HIGH);
    }
    
    delay(duration);
    
    digitalWrite(LED_LEFT, LOW);
    digitalWrite(LED_RIGHT, LOW);
    analogWrite(MOTOR_LEFT, 0);
    analogWrite(MOTOR_RIGHT, 0);
    Serial.println("Motors off");
    
    Microphone.restart();
}


void check_eyes(unsigned long cur_millis) {
    eyes->look();
    int cur_eyes_avg = eyes->avg();
    if (eyes->left - eyes->right > EYE_DIFF) {
        Serial.println("Right eye dark");
        SoundEffects.uh_oh();
        drive_motors(MOTOR_LEFT, ONE_SECOND);
    } else if (eyes->right - eyes->left > EYE_DIFF) {
        Serial.println("Left eye dark");
        SoundEffects.uh_oh();
        drive_motors(MOTOR_RIGHT, ONE_SECOND);
    } else if (prev_eyes_avg - cur_eyes_avg > EYE_DIFF) {
        Serial.println("Both eyes dark");
        SoundEffects.oh_no();
        for (int x = 0; x < 3; x++) {
            drive_motors(MOTOR_LEFT, HALF_SECOND);
            delay(HALF_SECOND);
            drive_motors(MOTOR_RIGHT, HALF_SECOND);
            delay(HALF_SECOND);
        }
    } else if (cur_eyes_avg - prev_eyes_avg > EYE_DIFF) {
        Serial.println("Both eyes light");
        SoundEffects.ahhh();
    } else if (eyes->left - prev_eyes_avg > EYE_DIFF) {
        Serial.println("Left eye light");
        SoundEffects.ahhh();
    } else if (eyes->right - prev_eyes_avg > EYE_DIFF) {
        Serial.println("Right eye light");
        SoundEffects.ahhh();
    }
    prev_eyes_avg = cur_eyes_avg;
    next_look = cur_millis + LOOK_INTERVAL;
}


void loop() {
    unsigned long cur_millis = millis();

    if (tone_dir > 0) {
        SoundEffects.slide(500, 1000);
        drive_motors(MOTOR_LEFT, TWO_SECONDS);
        tone_dir = 0;
    } else if (tone_dir < 0) {
        SoundEffects.slide(1000, 500);
        drive_motors(MOTOR_RIGHT, TWO_SECONDS);
        tone_dir = 0;
    }

    if (cur_millis >= next_look) {
        check_eyes(cur_millis);
    }
}

