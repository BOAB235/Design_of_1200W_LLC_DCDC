#include "stm32f1xx.h"

// --- PA9 macros ---
#define PA9_SET   (1u << 9)
#define PA9_RST   (1u << (9 + 16))

// --- PB14 macros ---
#define PB14_SET  (1u << 14)
#define PB14_RST  (1u << (14 + 16))


#define _Ena_drv PB10
#define _Dis_drv PB12

void setup() {
// Force PB12 HIGH in output data register BEFORE pinMode changes the mode
// digitalWrite(_Dis_drv, HIGH);     // preload ODR = 1 (float high in open-drain mode)
digitalWrite(_Dis_drv, LOW); 
pinMode(_Dis_drv, OUTPUT_OPEN_DRAIN);
digitalWrite(_Dis_drv, LOW); // redundancy
digitalWrite( _Ena_drv, HIGH); 
pinMode(_Dis_drv, OUTPUT);
digitalWrite( _Ena_drv, HIGH); 




/////// PA9 & PB14 as Fast output ///////////
  // Enable GPIOA + GPIOB clocks
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;

  // --- PA9 = 50 MHz push-pull output ---
  GPIOA->CRH &= ~(0xF << 4);   // clear config bits
  GPIOA->CRH |=  (0x3 << 4);   // MODE9=11 (50MHz), CNF9=00
  GPIOA->BSRR = PA9_RST;       // start LOW

  // --- PB14 = 50 MHz push-pull output ---
  GPIOB->CRH &= ~(0xF << 24);  // PB14 config bits
  GPIOB->CRH |=  (0x3 << 24);  // MODE14=11, CNF14=00
  GPIOB->BSRR = PB14_RST;      // start LOW



}




__attribute__((always_inline)) 
static inline void delayNs(uint32_t tns)
{
    /*
    target => oscilo
    6000ns => 5986ns 
    1000ns => 985.3ns
    */

    uint32_t iterations = (tns * 72) / (6.08*1000);

    for (uint32_t i = 0; i < iterations; i++) {
        __asm__ __volatile__("nop" ::: "memory");
    }
}


void loop() {


//////////// ENABLE DRIVER 
delayMicroseconds(10); 
digitalWrite(_Dis_drv, HIGH); // redundancy
digitalWrite( _Ena_drv, LOW); 
delayMicroseconds(10); 
digitalWrite( _Ena_drv, HIGH); 


/////////// double pulse waveform on / off / on /off

  uint32_t ton1  = 10e3; // us
  uint32_t toff1 = 2e3; // us
  uint32_t ton2  = 1e3; // us
  //uint32_t toff2 = 2; // us

  GPIOA->BSRR = PA9_SET; 
  delayNs(ton1);
  GPIOA->BSRR = PA9_RST; 
  delayNs(toff1);
  GPIOA->BSRR = PA9_SET; 
  delayNs(ton2);
  GPIOA->BSRR = PA9_RST; 
  while(1); 

  
  delayNs(500);
  GPIOB->BSRR = PB14_SET; 
  delayNs(500); 
  GPIOB->BSRR = PB14_RST; 

  GPIOA->BSRR = PA9_SET; 
  delayNs(500);
  GPIOA->BSRR = PA9_RST; 
  delayNs(500); 

  while(1);
  delayNs(toff1);
   
  GPIOA->BSRR = PA9_SET; 
  delayNs(ton2);

  GPIOA->BSRR = PA9_RST;


  while(1); // stop
}
