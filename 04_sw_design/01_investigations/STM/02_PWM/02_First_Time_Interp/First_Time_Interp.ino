/*  STM32F103C8T6 (CH340) - Timer interrupt sets a flag; loop prints a timestamp.
    Core: Arduino Core STM32 by STMicroelectronics (a.k.a. STM32duino official)
    Board: Generic STM32F1 series -> BluePill F103C8 (or equivalent)
    Serial: CH340 (uses 'Serial')
*/

#include <Arduino.h>

// ---------- Configuration ----------
#define TICK_HZ 200               // Timer interrupt frequency (prints per second). 200 Hz ≈ safe for 115200 baud
#define USE_CPU_CYCLE_COUNTER 0   // 0 = print micros(); 1 = print CPU cycles via DWT->CYCCNT
// ----------------------------------

volatile bool timerFlag = false;
HardwareTimer* tickTimer = nullptr;

void onTick() {
  // Keep ISR minimal: set a flag only.
  timerFlag = true;

  // (Optional) light feedback: toggle the LED at each tick.
  // Uncomment if you want a visible heartbeat.
  // static bool s = false;
  // digitalWrite(LED_BUILTIN, (s = !s));
}

#if USE_CPU_CYCLE_COUNTER
static inline void enableCycleCounter() {
#if defined(CoreDebug) && defined(DWT) && defined(DWT_CTRL_CYCCNTENA_Msk) && defined(CoreDebug_DEMCR_TRCENA_Msk)
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;   // enable tracing
  DWT->CYCCNT = 0;                                  // reset cycle counter
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;              // start cycle counter
#else
  // If these symbols aren’t available on your core/MCU, we’ll fall back to micros()
  // (No runtime error; just be aware cycles won’t print.)
#endif
}
#endif

void setup() {
  // LED is typically PC13 on BluePill; safe to set even if unused.
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  // For CH340 (UART), Serial is available immediately. Small delay helps the monitor attach cleanly.
  delay(200);

#if USE_CPU_CYCLE_COUNTER
  enableCycleCounter();
#endif

  // Use TIM2 as a general-purpose tick source.
  tickTimer = new HardwareTimer(TIM2);
  tickTimer->setOverflow(TICK_HZ, HERTZ_FORMAT); // e.g., 200 Hz
  tickTimer->attachInterrupt(onTick);            // update event callback
  tickTimer->resume();

  Serial.println("Timer started.");
}

void loop() {
  if (timerFlag) {
    timerFlag = false;

#if USE_CPU_CYCLE_COUNTER
  #if defined(DWT) && defined(DWT_CTRL_CYCCNTENA_Msk)
    // Prints raw CPU cycle count (wraps ~59.6s @ 72 MHz)
    uint32_t cycles = DWT->CYCCNT;
    Serial.println(cycles);
  #else
    // Fallback if DWT is not available
    Serial.println(micros());  // wraps ~71.6 minutes
  #endif
#else
    // Default: print microseconds (wraps ~71.6 minutes)
    Serial.println(micros());
#endif
  }
}
