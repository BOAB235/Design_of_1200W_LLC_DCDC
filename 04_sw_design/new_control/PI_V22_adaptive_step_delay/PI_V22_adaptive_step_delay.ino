#include "stm32f1xx.h"



//#define _Ena_drv PB10
//#define _Dis_drv PB12
//#define disable_HW_OVP_OUT PB11 // Conflict with timer use PB11
//#define PWM_FANS PA10
//#define  Flip_feedback PB0


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

static inline void __attribute__((always_inline)) reset_flip_flop(){
// reset flip flop
GPIOB->BSRR = (1U << 10);// Ena_drv HIGH 
delayMicroseconds(10); 
GPIOB->BSRR = (1U << 26);// Ena_drv LOW
delayMicroseconds(10); 
GPIOB->BSRR = (1U << 10);// Ena_drv HIGH 
delayMicroseconds(10);
}


static inline uint16_t adc_read(uint8_t ch) {
ADC1->SQR1 = 0;                 // 1 conversion
ADC1->SQR3 = (ch & 0x1F);       // select channel 0..17
ADC1->CR2  |= ADC_CR2_SWSTART;  // start
while (!(ADC1->SR & ADC_SR_EOC));
return ADC1->DR;                // 12-bit (right-aligned)
}

static inline void __attribute__((always_inline)) init_ADC(){
// Clocks
RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_ADC1EN;

// PA0..PA7 analog
GPIOA->CRL = 0x00000000;

// ADC clk = PCLK2/6 (~12 MHz if PCLK2=72 MHz)
RCC->CFGR &= ~RCC_CFGR_ADCPRE;
RCC->CFGR |=  RCC_CFGR_ADCPRE_DIV6;

// Fastest sampling (1.5 cycles). Increase if source impedance is high.
ADC1->SMPR2 = 0x00000000;

// Power on -> reset cal -> calibrate
ADC1->CR2 |= ADC_CR2_ADON;
ADC1->CR2 |= ADC_CR2_RSTCAL; while (ADC1->CR2 & ADC_CR2_RSTCAL);
ADC1->CR2 |= ADC_CR2_CAL;    while (ADC1->CR2 & ADC_CR2_CAL);

// Single conversion via SWSTART (F1 needs EXTTRIG + EXTSEL=111)
ADC1->CR2 &= ~ADC_CR2_CONT;
ADC1->CR2 &= ~ADC_CR2_EXTSEL;
ADC1->CR2 |=  ADC_CR2_EXTSEL;     // set EXTSEL=111 (SWSTART)
ADC1->CR2 |=  ADC_CR2_EXTTRIG;

// Mandatory second ADON after calibration
ADC1->CR2 |= ADC_CR2_ADON;





}


static inline void __attribute__((always_inline)) init_PWM(){
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
TIM1->ARR = 480;    // 150 kHz (edge-aligned): f = 72MHz / (PSC+1) / (ARR+1)
TIM1->CCR2 = 240;    // 50% duty (CHANGED to CCR2)

// PWM mode 1 + preload (CHANGED to OC2M and OC2PE)
TIM1->CCMR1 |= (6 << TIM_CCMR1_OC2M_Pos) | TIM_CCMR1_OC2PE; // OC2M=110 (PWM1), preload
TIM1->CR1  |= TIM_CR1_ARPE;                // ARR preload

// Enable CH2 + CH2N (CHANGED to CC2E and CC2NE)
//TIM1->CCER |= TIM_CCER_CC2E | TIM_CCER_CC2NE;
// Explicitly ensure outputs are disabled to guarantee starting low
TIM1->CCER &= ~(TIM_CCER_CC2E | TIM_CCER_CC2NE);


/*

Dead time resolution:
With Fclk = 72 MHz → 1 step ≈ 13.888 ns
DTG = 4 → 55.6 ns
DTG = 5 → 69.4 ns
DTG = 6 → 83.3 ns
DTG = 7 → 97.2 ns
DTG = 10 → 138.9 ns
DTG = 15 → 208.3 ns
DTG = 20 → 277.8 ns
*/
// Dead-time (~70 ns) + MOE
TIM1->BDTR = (20 << TIM_BDTR_DTG_Pos) | TIM_BDTR_MOE;

// Force update to load preloaded regs initially
TIM1->EGR = TIM_EGR_UG;

// Start counter
TIM1->CR1 |= TIM_CR1_CEN;

// Only overflow/underflow generate UEV (ignore UG for IRQ/DMA)—keeps latching clean
TIM1->CR1 |= TIM_CR1_URS;

// === A1 marker pin: PA1 as push-pull 50 MHz ===
/*RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
GPIOA->CRL &= ~(GPIO_CRL_MODE1 | GPIO_CRL_CNF1);
GPIOA->CRL |= (0b11 << GPIO_CRL_MODE1_Pos); // MODE1=11 (50 MHz), CNF1=00 (PP)
*/
// Clear any pending UIF from the initial UG so first sync waits properly
TIM1->SR &= ~TIM_SR_UIF;







}




static inline void __attribute__((always_inline))  init_digital_pins() {
// **_Ena_drv (PB10)** as output 
GPIOB->CRH &= ~(0xF << 8); 
GPIOB->CRH |= (0x3 << 8);



// Flip_feedback as input 
// 2. Erase PB0 bits, leave PB1-PB7 untouched
GPIOB->CRL &= ~(0xF << 0); 
// 3. Write 0x4 (Floating Input) to PB0
GPIOB->CRL |= (0x4 << 0);

/*
//_Dis_drv (PB12)	OUTPUT OPEN DRIVE
GPIOB->CRH &= ~(0xF << 16); GPIOB->CRH |= (0x7 << 16);
GPIOB->BSRR = (1U << 12);//_Dis_drv  =HIGH = OPEN 
*/


//disable_HW_OVP_OUT (PB11)	OUTPUT
GPIOB->CRH &= ~(0xF << 12); GPIOB->CRH |= (0x3 << 12);
GPIOB->BSRR = (1U << 11);//disable_HW_OVP_OUT (PB11) HEIGH 



/*
// PB1 AS INPUT (COMP FEEDBACK)
// 1. Clear exactly 4 bits for PB1 (bits 4, 5, 6, 7)
// PB0, PB2-PB7 remain exactly as they were.
GPIOB->CRL &= ~(0xF << 4); 
// 2. Set PB1 to Floating Input (0x4)
GPIOB->CRL |= (0x4 << 4);
*/

// PWM_FANS (PA10)	OUTPUT	 
GPIOA->CRH &= ~(0xF << 8); GPIOA->CRH |= (0x3 << 8);
// TURN FAN ON HIGH 
GPIOA->BSRR = (1U << 10);
}


void setup() {
init_PWM();
analogReadResolution(12);
init_ADC();  
init_digital_pins();
reset_flip_flop();
}




// Function updated to handle CCR2
static inline void pwm_periode_dutyc_free_glitch(uint16_t arr, uint16_t ccr2) // FUNCTION NAME CHANGED and uses ccr2
{
// --- Critical ordering to avoid the race ---
uint32_t prim = __get_PRIMASK(); __disable_irq();
TIM1->CR1 |= TIM_CR1_UDIS;   // 1) Block update transfers (no latching while we stage)
TIM1->SR &= ~TIM_SR_UIF;    // 2) Clear UIF *after* blocking updates (so we wait for the next real UEV)
//TIM1->ARR = arr;        // 3) Stage ARR
TIM1->ARR = arr - 1;
TIM1->CCR2 = ccr2;       // 3) Stage CCR2 (e.g., (arr+1)/2 for 50%)
TIM1->CR1 &= ~TIM_CR1_UDIS;   // 4) Re-enable update transfers
if (!prim) __enable_irq();

// 5) Wait for the *next* UEV — this is when ARR/CCR2 actually latch
while (!(TIM1->SR & TIM_SR_UIF)) { /* wait for latch boundary */ }
TIM1->SR &= ~TIM_SR_UIF;    // acknowledge

// 6) Mark the latching instant on A1 (few CPU cycles after boundary)
//toggle_A1_fast();


}






// Global variables

int min_periode = (int) 480; //72E3/150=480 , 150 kHz
int max_periode = (int) 758;  //72E3/95=757.89, 95khz


float per_accumulator = (float)  min_periode; //480.0
unsigned long last_time_us = 0;
const float SLEW_RATE_TICKS_PER_US =0.72; //277.89/386 =0.72, (277 ticks / 386us)


float vout=0;
float vin=0;
int Periode, Periode__; 










static inline void __attribute__((always_inline))  soft_start_protocol(){




//================== wait validat voltage to start ======
int Vin_Min_to_start = 60;
float Vinold = -100;
float Vindiff=100; 
delay (10);
while(vin<Vin_Min_to_start || Vindiff >10  )
{//START AT 100V min 
//the loop will exit only when vin >= 60 AND Vindiff <= 10
vin=0.3212 * adc_read(7) - 7.2289;
Vindiff = vin -Vinold;
Vinold = vin ; 
if (Vindiff < 0) Vindiff =-Vindiff ; // Abs
delay (1);
};
delay (10);
//=======================================================





//Periode = min_periode;//= 480 ticks = 150kHz 
Periode =288; //72Mhz/250khz=288
pwm_periode_dutyc_free_glitch(Periode, Periode / 2);

// Charge the boostrap capa 
// 1. Enable PA9 (Main channel / Low-Side)
TIM1->CCER |= TIM_CCER_CC2E;  
// 2. Disable PB14 (Complementary channel / High-Side)
TIM1->CCER &= ~TIM_CCER_CC2NE;
delayMicroseconds(100);




/*
Dead time resolution:
With Fclk = 72 MHz → 1 step ≈ 13.888 ns
DTG = 4 → 55.6 ns
DTG = 50 → 694.4 ns
DTG = 10 → 138.9 ns
*/
TIM1->BDTR = (50<< TIM_BDTR_DTG_Pos) | TIM_BDTR_MOE;


//Periode = 686;//105kHz 
//pwm_periode_dutyc_free_glitch(Periode, Periode / 3);
// Enable CH2 + CH2N (CHANGED to CC2E and CC2NE)
TIM1->CCER |= TIM_CCER_CC2E | TIM_CCER_CC2NE;
//delayMicroseconds(100);
while (Periode < 480){ // (480-288)=192x 10u = 1.92ms
//72e6/150e3=480
delayMicroseconds(10);
pwm_periode_dutyc_free_glitch(Periode, Periode / 2);
Periode = Periode +2;
}


// Change dead time to 140ns 
TIM1->BDTR = (10 << TIM_BDTR_DTG_Pos) | TIM_BDTR_MOE;
delayMicroseconds(100);




}







static inline void __attribute__((always_inline)) activated_HWP_VOUT(){
// Enable the Vout HW OVP: MAKE OVP_INPUT (PB11) AS INPUT
GPIOB->CRH &= ~(0xF << 12); GPIOB->CRH |= (0x4 << 12);
}


void loop() {

  soft_start_protocol();
  activated_HWP_VOUT();
  unsigned long step_delay;
  float error ; 

  // ── Tuning ──────────────────────────────────────────────
  const float vout_target     = 48.0f;
  const float DEADBAND        = 0.15f;  // [V]  no step inside this window
  const int   TICK_STEP       = 2;     // [ticks] always 2, ZVS safe
  const unsigned long DLY_FAR  =  20;  // [µs] |error| > 2.0V
  const unsigned long DLY_MID  = 100;  // [µs] |error| > 0.5V
  const unsigned long DLY_NEAR = 400;  // [µs] |error| > 0.1V  ← key for stability
  // ────────────────────────────────────────────────────────

  unsigned long step_timer = micros();

  while (1) {

    // ── FAST PATH: always runs, no blocking ──────────────
    vout = 0.03081f * adc_read(1) - 0.09761f;
    error = vout_target - vout;
    if (error < 0) error = -error;  // ABS function 
    // Determine step delay from current error zone
    if      (error > 1.0f) step_delay = DLY_FAR;
    else if (error > 0.3f) step_delay = DLY_MID;
    else                   step_delay = DLY_NEAR;

    // ── SLOW PATH: step only when timer expired + outside deadband ──
    if (error > DEADBAND && (micros() - step_timer) >= step_delay) {

      step_timer = micros();  // reset timer BEFORE pwm call (pwm blocks ~7µs)

      if (vout < vout_target) {
        Periode += TICK_STEP; // Vout too low  → lower freq → more gain
      } else {
        Periode -= TICK_STEP; // Vout too high → raise freq → less gain
      }

      // Safety clamp
      if (Periode > max_periode) Periode = max_periode;
      if (Periode < min_periode) Periode = min_periode;

      // Force even value (half-bridge symmetry)
      Periode = (Periode / 2) * 2;

      // Apply — blocks until UEV boundary (~7µs), called ONLY here
      pwm_periode_dutyc_free_glitch(Periode, Periode / 2);
    }

  }
}


