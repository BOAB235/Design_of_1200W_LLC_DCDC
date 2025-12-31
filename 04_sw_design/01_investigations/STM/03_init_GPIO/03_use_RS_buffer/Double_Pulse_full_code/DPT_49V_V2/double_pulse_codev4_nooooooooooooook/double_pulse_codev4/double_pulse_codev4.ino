#include "stm32f1xx.h"

#define CPU_FREQ 72000000UL
#define PA9_SET  (1u << 9)
#define PA9_RST  (1u << (9 + 16))
#define PA9_BIT  (1u << 9)

#define _Ena_drv PB10
#define _Dis_drv PB12



void delay_cycles(uint32_t cycles) {
    while (cycles) {
        cycles--;
        NP;
    }

}

void delay_ns(uint32_t t_ns) {
	// each us have 72 cycle , 72Mhz 
	// each cycle take 13.888 ns 
	uint32_t cycles= (uint32_t)  (t_ns*72)/1000; 
	delay_cycles(cycles) ; 
}

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

void loop() {


//////////// ENABLE DRIVER 
delayMicroseconds(10); 
digitalWrite(_Dis_drv, HIGH); // redundancy
digitalWrite( _Ena_drv, LOW); 
delayMicroseconds(10); 
digitalWrite( _Ena_drv, HIGH); 


/////////// double pulse waveform on / off / on /off

  uint32_t ton1  = 15; // us
  uint32_t toff1 = 2; // us
  uint32_t ton2  = 2; // us
  //uint32_t toff2 = 2; // us

  GPIOA->BSRR = PA9_SET; 
  //delayMicroseconds(ton1); 
  //delay_ns(ton1 *1000);
  delay_cycles(72);

  GPIOA->BSRR = PA9_RST; 
  //delayMicroseconds(toff1);
  //delay_ns(toff1 *1000);
  delay_cycles(72);
   
  GPIOA->BSRR = PA9_SET; 
  //delayMicroseconds(ton2);
  //delay_ns(ton2 *1000);
  delay_cycles(72);

  GPIOA->BSRR = PA9_RST;
  //delayMicroseconds(toff2);

  while(1); // stop
}
