#include "stm32f1xx.h"

#define CPU_FREQ 72000000UL
#define PA9_SET  (1u << 9)
#define PA9_RST  (1u << (9 + 16))
#define PA9_BIT  (1u << 9)

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




/////// PA9 as Fast output ///////////
  // Enable GPIOA clock
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  // PA9 = General purpose push‑pull output, 50 MHz (CRH bits [7:4])
  GPIOA->CRH &= ~(0xFu << 4);
  GPIOA->CRH |=  (0x3u << 4);   // MODE9=11 (50MHz), CNF9=00 (GP PP)
  // Ensure USART1 doesn't grab PA9 (we don't enable it)
  RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN;
  // Force LOW 
  GPIOA->BSRR = PA9_RST;



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

  uint32_t ton1  = 7; // us
  uint32_t toff1 = 2; // us
  uint32_t ton2  = 1; // us
  //uint32_t toff2 = 2; // us






  GPIOA->BSRR = PA9_SET; 
  //delayMicroseconds(ton1); 

delayNs(1000);

  GPIOA->BSRR = PA9_RST; 
  while (1);

}
