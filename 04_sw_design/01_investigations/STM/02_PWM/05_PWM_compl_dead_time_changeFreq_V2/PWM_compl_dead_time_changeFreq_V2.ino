#include "stm32f1xx.h"
/*
Outputs:
TIM1_CH1 → PA8
TIM1_CH1N → PB13

Dead time resolution:
With Fclk = 72 MHz → 1 step = 13.8 ns
DTG = 4 → ~55 ns
DTG = 5 → ~69 ns
DTG = 6 → ~83 ns
*/

void setup() {
  // Enable clocks
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_TIM1EN;

  // GPIOA PA8 -> AF Push-Pull 50 MHz
  GPIOA->CRH &= ~(0xF << 0);
  GPIOA->CRH |=  (0xB << 0);

  // GPIOB PB13 -> AF Push-Pull 50 MHz
  GPIOB->CRH &= ~(0xF << ((13-8)*4));
  GPIOB->CRH |=  (0xB << ((13-8)*4));

  // Timer base config
  TIM1->PSC = 0;
  TIM1->ARR = 719;        // 100 kHz (edge-aligned)
  TIM1->CCR1 = 360;       // 50% duty

  // PWM mode 1 + preload
  TIM1->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos) | TIM_CCMR1_OC1PE;
  TIM1->CR1   |= TIM_CR1_ARPE;

  // Enable CH1 + CH1N
  TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC1NE;

  // Deadtime (~70 ns)
  TIM1->BDTR = (5 << TIM_BDTR_DTG_Pos) | TIM_BDTR_MOE;

  // Force update to load preloaded registers
  TIM1->EGR = TIM_EGR_UG;

  // Start counter
  TIM1->CR1 |= TIM_CR1_CEN;

  // Trigger PIN 
  pinMode(A1, OUTPUT);   // Set A1 as output
}
static uint16_t arr;
void loop() {
    
    arr = 719;
    TIM1->ARR = arr;                // Preloaded, updates at next UEV
    TIM1->CCR1 = (arr + 1) / 2;     // Keep 50% duty
    digitalWrite(A1, LOW);
    delayMicroseconds(25);
    arr = 359;
    TIM1->ARR = arr;                // Preloaded, updates at next UEV
    TIM1->CCR1 = (arr + 1) / 2;     // Keep 50% duty
    digitalWrite(A1, HIGH);
    delayMicroseconds(37);
  
}
