/*
  Eyes
  A class for handling two photoresistors, each in a voltage divider circuit so they return 0-5V.
  Author: Ron Smith, That Aint Working
  Created: April 2015
*/

#ifndef Eyes_h_
#define Eyes_h_

class Eyes {
    
public:
    int left;
    int right;
    
    Eyes(int ppin, int lpin, int rpin) : power_pin(ppin), left_pin(lpin), right_pin(rpin), left(0), right(0) {
        pinMode(ppin, OUTPUT);
        pinMode(lpin, INPUT);
        pinMode(rpin, INPUT);
    }
    
    void look() {
        digitalWrite(power_pin, HIGH);    // turn photocell power on
        delayMicroseconds(20);            // wait for values to settle
        left = analogRead(left_pin);      
        right = analogRead(right_pin);
        digitalWrite(power_pin, LOW);     // turn photocell power off
    }
    
    int avg() {
        return (left + right) / 2;
    }
    
private:
    int power_pin;
    int left_pin;
    int right_pin;
};

#endif // Eyes_h_

