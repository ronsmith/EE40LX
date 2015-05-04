/*
  Microphone
  A class for handling microphone input.
  Uses Timer1 while running or listening so you can't use it for anything else.
  Author: Ron Smith, That Aint Working
  Created: April 2015
*/

#ifndef Microphone_h_
#define Microphone_h_

typedef void (*feedback_func_p)();

class _Microphone {
    
public:

    _Microphone();
    
    // start listening in the backgrund using interrupts, 
    // check frequency count every interval, 
    // reset for nosound every nsr checks.
    void start(int intrpt_pin, unsigned long intrvl, unsigned int nsr);
    
    void stop();
    
    void restart();
        
    void dump_buffer(); // to Serial
    
    feedback_func_p on_tone_rising;      // a rising tone received
    feedback_func_p on_tone_falling;     // a falling tone received
    feedback_func_p on_mic_check;        // starting a microphone check
    feedback_func_p on_nsr;              // starting a no sound reset

private:
    int interrupt_pin;
    unsigned long interval;
    int nscounter;
    int nsreset;
    
    volatile unsigned int counter;
    volatile unsigned int* buf;
    volatile int bufidx;

    void set_interrupts();
    void evaluate_tone();
    void check();
    void reset();  

    friend void mic_input_isr();
    friend void mic_check_isr();
};

extern _Microphone Microphone;

#endif // Microphone_h_

