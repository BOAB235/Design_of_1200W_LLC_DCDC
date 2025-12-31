#include <Arduino.h>

#define FREQ       10000      // 10 kHz
#define N_SAMPLES  15

volatile bool trigger = false;
uint32_t t[N_SAMPLES];
uint16_t idx = 0;

HardwareTimer *timer;

void onTick() {
  trigger = true;  // ISR only sets flag
}

void setup() {
  Serial.begin(115200);
  delay(100);

  timer = new HardwareTimer(TIM2);
  timer->setOverflow(FREQ, HERTZ_FORMAT); // 10 kHz
  timer->attachInterrupt(onTick);
  timer->resume();
}

void loop() {
  if (trigger) {
    trigger = false;

    if (idx < N_SAMPLES) 
    {
      t[idx++] = micros();
    }
    else 
    {
        timer->pause();  // stop interrupt
        for (uint16_t i = 1; i < N_SAMPLES; i++) 
        {
          Serial.println(t[i] - t[i - 1]);
        }
    }
  }
}
