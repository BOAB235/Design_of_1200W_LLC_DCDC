#include "stm32f1xx.h"

#define CPU_FREQ 72000000UL
#define PA9_SET  (1u << 9)
#define PA9_RST  (1u << (9 + 16))
#define PA9_BIT  (1u << 9)

static inline void dwt_start(void) {
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}


void setup(void) {
  // Enable GPIOA clock
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

  // PA9 = General purpose push‑pull output, 50 MHz (CRH bits [7:4])
  GPIOA->CRH &= ~(0xFu << 4);
  GPIOA->CRH |=  (0x3u << 4);   // MODE9=11 (50MHz), CNF9=00 (GP PP)

  // Ensure USART1 doesn't grab PA9 (we don't enable it)
  RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN;

  dwt_start();

  // Quick self-check: force LOW and verify ODR reflects it
  GPIOA->BSRR = PA9_RST;
  if (GPIOA->ODR & PA9_BIT) { for (;;){ /* trap if config failed */ } }
}

void loop(void) {
  uint32_t ton1  = 2; // us
  uint32_t toff1 = 1; // us
  uint32_t ton2  = 1; // us
  uint32_t toff2 = 1; // us

  GPIOA->BSRR = PA9_SET; 
  delayMicroseconds(ton1); 

  GPIOA->BSRR = PA9_RST; 
  delayMicroseconds(toff1);
   
  GPIOA->BSRR = PA9_SET; 
  delayMicroseconds(ton2);

  GPIOA->BSRR = PA9_RST;
  delayMicroseconds(toff2);
}