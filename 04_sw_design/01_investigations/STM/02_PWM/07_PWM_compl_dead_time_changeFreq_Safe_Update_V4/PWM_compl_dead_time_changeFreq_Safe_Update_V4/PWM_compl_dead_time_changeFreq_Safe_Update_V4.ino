#include "stm32f1xx.h"
/*
Outputs:
TIM1_CH1  → PA8
TIM1_CH1N → PB13

Dead time resolution:
With Fclk = 72 MHz → 1 step ≈ 13.888 ns
DTG = 4 → ~55.6 ns
DTG = 5 → ~69.4 ns
DTG = 6 → ~83.3 ns
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
  TIM1->PSC  = 0;
  TIM1->ARR  = 719;       // 100 kHz (edge-aligned): f = 72MHz / (PSC+1) / (ARR+1)
  TIM1->CCR1 = 360;       // 50% duty

  // PWM mode 1 + preload
  TIM1->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos) | TIM_CCMR1_OC1PE; // OC1M=110 (PWM1), preload
  TIM1->CR1   |= TIM_CR1_ARPE;                                // ARR preload

  // Enable CH1 + CH1N
  TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC1NE;

  // Dead-time (~70 ns) + MOE
  TIM1->BDTR = (5 << TIM_BDTR_DTG_Pos) | TIM_BDTR_MOE;

  // Force update to load preloaded regs initially
  TIM1->EGR = TIM_EGR_UG;

  // Start counter
  TIM1->CR1 |= TIM_CR1_CEN;

  // Only overflow/underflow generate UEV (ignore UG for IRQ/DMA)—keeps latching clean
  TIM1->CR1 |= TIM_CR1_URS;

  // === A1 marker pin: PA1 as push-pull 50 MHz ===
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  GPIOA->CRL &= ~(GPIO_CRL_MODE1 | GPIO_CRL_CNF1);
  GPIOA->CRL |=  (0b11 << GPIO_CRL_MODE1_Pos);  // MODE1=11 (50 MHz), CNF1=00 (PP)

  // Clear any pending UIF from the initial UG so first sync waits properly
  TIM1->SR &= ~TIM_SR_UIF;
}

static inline void toggle_A1_fast() {
  // Toggle via BSRR (deterministic, no digitalWrite jitter)
  if (GPIOA->ODR & (1u << 1)) {
    GPIOA->BSRR = GPIO_BSRR_BR1;  // low
  } else {
    GPIOA->BSRR = GPIO_BSRR_BS1;  // high
  }
}

static inline void set_arr_ccr1_synced(uint16_t arr, uint16_t ccr1)
{
  // --- Critical ordering to avoid the race ---
  uint32_t prim = __get_PRIMASK(); __disable_irq();
  TIM1->CR1 |= TIM_CR1_UDIS;      // 1) Block update transfers (no latching while we stage)
  TIM1->SR  &= ~TIM_SR_UIF;       // 2) Clear UIF *after* blocking updates (so we wait for the next real UEV)
  TIM1->ARR  = arr;               // 3) Stage ARR
  TIM1->CCR1 = ccr1;              // 3) Stage CCR1 (e.g., (arr+1)/2 for 50%)
  TIM1->CR1 &= ~TIM_CR1_UDIS;     // 4) Re-enable update transfers
  if (!prim) __enable_irq();

  // 5) Wait for the *next* UEV — this is when ARR/CCR1 actually latch
  while (!(TIM1->SR & TIM_SR_UIF)) { /* wait for latch boundary */ }
  TIM1->SR &= ~TIM_SR_UIF;        // acknowledge

  // 6) Mark the latching instant on A1 (few CPU cycles after boundary)
  toggle_A1_fast();
}

void loop() {
  set_arr_ccr1_synced(719, (719 + 1) / 2);  // 100 kHz, 50%
  delayMicroseconds(25);

  set_arr_ccr1_synced(359, (359 + 1) / 2);  // 200 kHz, 50%
  delayMicroseconds(37);
}
