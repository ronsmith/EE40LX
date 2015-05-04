/*
  SoundEffects
  A class for generating simple sound effects.
  Author: Ron Smith, That Aint Working
  Created: April 2015
*/
 
#ifndef SoundEffects_h_
#define SoundEffects_h_

typedef void (*ba_func_p)();

class _SoundEffects {
    public:
        _SoundEffects() : speaker_pin(-1), before_func(0), after_func(0) { }
        
        void init(int speaker_pin);
        void init(int speaker_pin, ba_func_p before_func, ba_func_p after_func);
        
        void uh_oh();
        void oh_no();
        void ahhh();
        void slide(int from_pitch, int to_pitch);
        void two_tone(int tone1, int tone2);
        void sequence(int len, int tones[], int durations[]);
        
    private:
        int speaker_pin;
        ba_func_p before_func;
        ba_func_p after_func;
};

extern _SoundEffects SoundEffects;

#endif // SoundEffects_h_
