void setup() {
  // Enable GPIOB clock
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

  // Preload ODR HIGH before switching to output
  GPIOB->BSRR = (1 << 12);  // Set PB12 high in ODR

  // Configure PB12 as open-drain output
  // STM32F1 uses CRH for pins 8..15
  // MODE12 = 0b11 (Output, 50 MHz)
  // CNF12  = 0b01 (Open-drain)
  GPIOB->CRH &= ~(0xF << 16);        // Clear bits for PB12
  GPIOB->CRH |=  ((0b11 | (0b01 << 2)) << 16); // MODE=11, CNF=01

  // Now PB12 is open-drain and released HIGH (external pull-up will pull it up)
  delayMicroseconds(1);

  // Pull LOW
  GPIOB->BSRR = (1 << (12 + 16)); // Reset PB12 (drive low)
  delayMicroseconds(1);
  // Pull up 
  GPIOB->BSRR = (1 << 12);  // Set PB12 high in ODR


  while (1) ; // Stop
}

void loop() {}