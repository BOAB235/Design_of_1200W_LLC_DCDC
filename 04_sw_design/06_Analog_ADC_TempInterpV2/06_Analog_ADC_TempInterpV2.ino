
#include "stm32f1xx.h"

static inline uint16_t adc_read(uint8_t ch) {
  ADC1->SQR1 = 0;                 // 1 conversion
  ADC1->SQR3 = (ch & 0x1F);       // select channel 0..17
  ADC1->CR2  |= ADC_CR2_SWSTART;  // start
  while (!(ADC1->SR & ADC_SR_EOC));
  return ADC1->DR;                // 12-bit (right-aligned)
}

void init_ADC(){
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





//////////////////////////// TEMP FUNCTION /////////////////////
const int TABLE_SIZE = 15;

const float fac[TABLE_SIZE] = {
  0.53021892, 0.7085381, 0.96818072, 1.38331188,
  1.93285179, 2.73708445, 4.08904919, 6.24657131,
  9.97772497, 15.24231371, 26.6183813, 44.06236682,
  67.31117763, 87.96500807, 189.44444201
};

const float temp[TABLE_SIZE] = {
  125.0, 112.03703704, 100.07122507, 85.61253561,
  74.14529915, 62.67806268, 50.21367521, 37.74928775,
  24.78632479, 13.81766382, 0.35612536, -11.11111111,
  -20.08547009, -25.56980057, -40.02849003
};
float interpolate1D(float x) {

  // Clamp below range
  if (x <= fac[0]) {
    return temp[0];
  }

  // Clamp above range
  if (x >= fac[TABLE_SIZE - 1]) {
    return temp[TABLE_SIZE - 1];
  }

  // Find interval
  for (int i = 0; i < TABLE_SIZE - 1; i++) {
    if (x >= fac[i] && x <= fac[i + 1]) {

      float x0 = fac[i];
      float x1 = fac[i + 1];
      float y0 = temp[i];
      float y1 = temp[i + 1];

      // Linear interpolation formula
      return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
    }
  }

  return 0; // Should never reach here
}
float temp_secondary(){
uint16_t a6 = adc_read(6);
int ADCMAX = 4096; 
float vcc= 3.328;
float R_2 = 4.99; 
float A6V, RthS;  
A6V= vcc*a6/ADCMAX ; 
RthS= R_2/(vcc/A6V-1); 
return interpolate1D(RthS);
}
float temp_primary(){
uint16_t a6 = adc_read(2);
int ADCMAX = 4096; 
float vcc= 3.328;
float R_2 = 35.5987 ; //35.1294; 
float A2V, RthS;  
A2V= vcc*a6/ADCMAX ; 
RthS= R_2/(vcc/A2V-1); 
return interpolate1D(RthS);
}
/////////////////////////////////////////////////////////





///////////////////////////////////////////////////////
float  temp_power_derating(float Temp){
float T0_der=60;
float T1_der=90;
float P_nom= 1200;
if (Temp < T0_der) return P_nom; 
if (Temp >=  T0_der && Temp <=  T1_der){
 return  P_nom*(1-(D11-T0_der)/(T1_der-T0_der)); 
}
if (Temp > T1_der) return 0; 
}
///////////////////////////////////////////////////////

void setup() {





  Serial.begin(115200);           // use SerialUSB or Serial1 if that's your port
  delay(50);                      // small settle time for Serial Monitor
  pinMode(PB14, OUTPUT_OPEN_DRAIN);
  digitalWrite(PB14, LOW);
  // we can't use PA9 and serial in the same time 
  //PA9 is USART1 TX, and you’re forcing it to OUTPUT_OPEN_DRAIN, LOW.
  //pinMode(PA9, OUTPUT_OPEN_DRAIN);
  //digitalWrite(PA9, LOW);


  init_ADC(); 
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
  float vout =0.03081*a1+ -0.09761; 
  Serial.print(vout); Serial.print(' ');
  Serial.print(a2); Serial.print(' ');
  Serial.print(a3); Serial.print(' ');
  Serial.print(a4); Serial.print(' ');
  Serial.print(a5); Serial.print(' ');
  Serial.print(a6); Serial.print(' ');
  Serial.print(a7); Serial.print(' ');
  float vin= 0.3212 * a7 - 7.2289;
  Serial.print(vin); Serial.print(' ');
  float tempS= temp_secondary(); 
  Serial.print(tempS); Serial.print(' ');


  float tempP=temp_primary(); 
  Serial.print(tempP); Serial.print(' ');
  // Max temp
  if (tempP> tempS) tempS = tempP; 
  float power_derating =temp_power_derating( tempS);
  Serial.print(power_derating); Serial.print(' ');

  Serial.println();

  delay(200);
}
