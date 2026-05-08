#include "stm32f1xx.h"

void setup() {
  // --- Enable clocks for GPIOA, GPIOB, AFIO, and TIM1 ---
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN |
                  RCC_APB2ENR_AFIOEN | RCC_APB2ENR_TIM1EN;

  // --- PA9 → TIM1_CH2 (AF Push-Pull, 50 MHz) ---
  GPIOA->CRH &= ~(0xF << ((9 - 8) * 4));
  GPIOA->CRH |=  (0xB << ((9 - 8) * 4));

  // --- PB14 → TIM1_CH2N (AF Push-Pull, 50 MHz) ---
  GPIOB->CRH &= ~(0xF << ((14 - 8) * 4));
  GPIOB->CRH |=  (0xB << ((14 - 8) * 4));

  // --- Timer setup ---
  TIM1->PSC = 0;          // Prescaler = 0 → 72 MHz timer clock
  TIM1->ARR = 719;        // Period = 720 counts → 100 kHz PWM
  TIM1->CCR2 = 360;       // 50% duty cycle

  // --- PWM mode 1 on CH2 + preload enabled ---
  TIM1->CCMR1 |= (6 << TIM_CCMR1_OC2M_Pos) | TIM_CCMR1_OC2PE;

  // --- Enable CH2 and CH2N outputs ---
  TIM1->CCER |= TIM_CCER_CC2E | TIM_CCER_CC2NE;

  // --- Main output enable, no dead time ---
  TIM1->BDTR = TIM_BDTR_MOE;

  // --- Update registers and start timer ---
  TIM1->EGR = TIM_EGR_UG;
  TIM1->CR1 = TIM_CR1_CEN;
}

void loop() {
  // PWM runs automatically on PA9 / PB14
}
