/*
    Basic frequency counter
*/

volatile unsigned long mic_counter;
unsigned long next_check;
const unsigned long INTERVAL = 1000L;

void setup() {
    Serial.begin(9600);
    pinMode(P1_7, INPUT_PULLDOWN);
    attachInterrupt(P1_7, mic_input_isr, RISING);
    mic_counter = 0;
    next_check = millis() + INTERVAL;
}

void mic_input_isr() {
    mic_counter++;
}

void loop() {
    if (millis() >= next_check) {
        Serial.println(mic_counter);
        mic_counter = 0;
        next_check = millis() + INTERVAL;
    }
}
