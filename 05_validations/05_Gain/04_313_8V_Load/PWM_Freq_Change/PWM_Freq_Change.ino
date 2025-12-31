#include "stm32f1xx.h"



#define _Ena_drv PB10
#define _Dis_drv PB12


/*
Outputs:
TIM1_CH2 → PA9  
TIM1_CH2N → PB14 

Dead time resolution:
With Fclk = 72 MHz → 1 step ≈ 13.888 ns
DTG = 4 → ~55.6 ns
DTG = 5 → ~69.4 ns
DTG = 6 → ~83.3 ns
*/

void setup() {
 // Enable clocks
 RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_TIM1EN;

 // GPIOA PA9 -> AF Push-Pull 50 MHz (PA9 is CRH nibble 1, shift 4)
 GPIOA->CRH &= ~(0xF << 4);
 GPIOA->CRH |= (0xB << 4);

 // GPIOB PB14 -> AF Push-Pull 50 MHz (PB14 is CRH nibble 6, shift 24)
 GPIOB->CRH &= ~(0xF << ((14-8)*4));
 GPIOB->CRH |= (0xB << ((14-8)*4));

 // Timer base config
 TIM1->PSC = 0;
 TIM1->ARR = 719;    // 100 kHz (edge-aligned): f = 72MHz / (PSC+1) / (ARR+1)
 TIM1->CCR2 = 360;    // 50% duty (CHANGED to CCR2)

 // PWM mode 1 + preload (CHANGED to OC2M and OC2PE)
 TIM1->CCMR1 |= (6 << TIM_CCMR1_OC2M_Pos) | TIM_CCMR1_OC2PE; // OC2M=110 (PWM1), preload
 TIM1->CR1  |= TIM_CR1_ARPE;                // ARR preload

 // Enable CH2 + CH2N (CHANGED to CC2E and CC2NE)
 //TIM1->CCER |= TIM_CCER_CC2E | TIM_CCER_CC2NE;
 // Explicitly ensure outputs are disabled to guarantee starting low
TIM1->CCER &= ~(TIM_CCER_CC2E | TIM_CCER_CC2NE);

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
 GPIOA->CRL |= (0b11 << GPIO_CRL_MODE1_Pos); // MODE1=11 (50 MHz), CNF1=00 (PP)

 // Clear any pending UIF from the initial UG so first sync waits properly
 TIM1->SR &= ~TIM_SR_UIF;

// Force PB12 HIGH in output data register BEFORE pinMode changes the mode
// digitalWrite(_Dis_drv, HIGH);   // preload ODR = 1 (float high in open-drain mode)
digitalWrite(_Dis_drv, LOW); 
pinMode(_Dis_drv, OUTPUT_OPEN_DRAIN);
digitalWrite(_Dis_drv, LOW); // redundancy
digitalWrite( _Ena_drv, HIGH); 
pinMode(_Dis_drv, OUTPUT);
digitalWrite( _Ena_drv, HIGH); 




  // initialize serial communication at 9600 bits per second:
  //Serial.begin(115200);

   analogReadResolution(12);
}

static inline void toggle_A1_fast() {
 // Toggle via BSRR (deterministic, no digitalWrite jitter)
 if (GPIOA->ODR & (1u << 1)) {
  GPIOA->BSRR = GPIO_BSRR_BR1; // low
 } else {
  GPIOA->BSRR = GPIO_BSRR_BS1; // high
 }
}

// Function updated to handle CCR2
static inline void set_arr_ccr2_synced(uint16_t arr, uint16_t ccr2) // FUNCTION NAME CHANGED and uses ccr2
{
 // --- Critical ordering to avoid the race ---
 uint32_t prim = __get_PRIMASK(); __disable_irq();
 TIM1->CR1 |= TIM_CR1_UDIS;   // 1) Block update transfers (no latching while we stage)
 TIM1->SR &= ~TIM_SR_UIF;    // 2) Clear UIF *after* blocking updates (so we wait for the next real UEV)
 TIM1->ARR = arr;        // 3) Stage ARR
 TIM1->CCR2 = ccr2;       // 3) Stage CCR2 (e.g., (arr+1)/2 for 50%)
 TIM1->CR1 &= ~TIM_CR1_UDIS;   // 4) Re-enable update transfers
 if (!prim) __enable_irq();

 // 5) Wait for the *next* UEV — this is when ARR/CCR2 actually latch
 while (!(TIM1->SR & TIM_SR_UIF)) { /* wait for latch boundary */ }
 TIM1->SR &= ~TIM_SR_UIF;    // acknowledge

 // 6) Mark the latching instant on A1 (few CPU cycles after boundary)
 toggle_A1_fast();
}

void loop() {


//////////// ENABLE DRIVER 
delayMicroseconds(10); 
digitalWrite(_Dis_drv, HIGH); // redundancy
digitalWrite( _Ena_drv, LOW); 
delayMicroseconds(10); 
digitalWrite( _Ena_drv, HIGH); 


set_arr_ccr2_synced(719, (719 + 1) / 2); // 100 kHz, 50% (FUNCTION CALL CHANGED)
// Enable CH2 + CH2N (CHANGED to CC2E and CC2NE)
TIM1->CCER |= TIM_CCER_CC2E | TIM_CCER_CC2NE;


while(1){
// A6 = Middle point of potensiometer 3.3V / GND : max 4095
int valA6 = analogRead(A6); 
//Serial.println(val);
int Periode  = (int) 1200-(valA6*18)/100;
set_arr_ccr2_synced(Periode, (Periode + 1) / 2); // 100 kHz, 50% (FUNCTION CALL CHANGED)
delay(50); // or use millis() to pace outpu

// In oscilloscope i measured 60khz to 155khz ok 
}

 
 // 100 kHz (edge-aligned): f = 72MHz / (PSC+1) / (ARR+1)
while (1); 
}