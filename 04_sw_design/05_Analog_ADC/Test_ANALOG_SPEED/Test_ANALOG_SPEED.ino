
#include "stm32f1xx.h"

static inline uint16_t adc_read(uint8_t ch) {
  ADC1->SQR1 = 0;                 // 1 conversion
  ADC1->SQR3 = (ch & 0x1F);       // select channel 0..17
  ADC1->CR2  |= ADC_CR2_SWSTART;  // start
  while (!(ADC1->SR & ADC_SR_EOC));
  return ADC1->DR;                // 12-bit (right-aligned)
}

void setup() {





  Serial.begin(115200);           // use SerialUSB or Serial1 if that's your port
  delay(50);                      // small settle time for Serial Monitor
  pinMode(PB14, OUTPUT_OPEN_DRAIN);
  digitalWrite(PB14, LOW);
  // we can't use PA9 and serial in the same time 
  //PA9 is USART1 TX, and you’re forcing it to OUTPUT_OPEN_DRAIN, LOW.
  //pinMode(PA9, OUTPUT_OPEN_DRAIN);
  //digitalWrite(PA9, LOW);









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

  Serial.println("ADC ready");
}

void loop() {
  uint32_t t0 = micros();

  uint16_t a0 = adc_read(0);
  uint16_t a1 = adc_read(1);
  uint16_t a2 = adc_read(2);
  uint16_t a3 = adc_read(3);
  uint16_t a4 = adc_read(4);
  uint16_t a5 = adc_read(5);
  uint16_t a6 = adc_read(6);
  uint16_t a7 = adc_read(7);

  uint32_t dt = micros() - t0;

  Serial.print("dt(us)="); Serial.print(dt); Serial.print(" | ");
  Serial.print(a0); Serial.print(' ');
  Serial.print(a1); Serial.print(' ');
  Serial.print(a2); Serial.print(' ');
  Serial.print(a3); Serial.print(' ');
  Serial.print(a4); Serial.print(' ');
  Serial.print(a5); Serial.print(' ');
  Serial.print(a6); Serial.print(' ');
  Serial.println(a7);

  delay(200);
}
