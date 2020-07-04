#include <avr/eeprom.h>
/*
 * Helpfull infrmation
 * https://alexgyver.ru/lessons/pinout/
 * https://alexgyver.ru/lessons/interrupts/
 * https://alexgyver.ru/lessons/eeprom/
 */
 
// Select pins for watermeters. It pins have to have interraptions. There are 2 and 3 in Arduino NANO. Interuption num is 0 and 1
#define CNTR_H_PIN 2
#define CNTR_H_INTR 0
#define CNTR_C_PIN 3
#define CNTR_C_INTR 1



/*
 * Eeeprom adres structure
 */
#define EEPROM_H_ADDR 0
#define EEPROM_C_ADDR 4

#define MTR_DBNC 500



volatile unsigned int hot_cntr=0;
volatile unsigned int cold_cntr=0;
volatile uint32_t debounce_h=0;
volatile uint32_t debounce_c=0;

unsigned int main_cntr=0;
volatile unsigned int last_hot_cntr=0;
volatile unsigned int last_cold_cntr=0;

void hotCntrTick () {
  if (millis() - debounce_h >= MTR_DBNC) {
    debounce_h = millis();
    if (!digitalRead(CNTR_H_PIN)) 
      hot_cntr++;
  }
}
void coldCntrTick () {
  if (millis() - debounce_c >= MTR_DBNC) {
    debounce_c = millis();
    if (!digitalRead(CNTR_C_PIN))
      cold_cntr++;
  }
}

void setup() {
  Serial.begin(9600);
  hot_cntr = eeprom_read_word(EEPROM_H_ADDR);
  cold_cntr = eeprom_read_word(EEPROM_C_ADDR);
  pinMode(CNTR_H_PIN, INPUT_PULLUP);
  attachInterrupt(CNTR_H_INTR, hotCntrTick, CHANGE);
  pinMode(CNTR_C_PIN, INPUT_PULLUP);
  attachInterrupt(CNTR_C_INTR, coldCntrTick, CHANGE);
  Serial.print("Start");
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(500);                       // wait for a second
  main_cntr++;
  if (main_cntr%5==0 && (hot_cntr!=last_hot_cntr||cold_cntr!=last_cold_cntr )) {
    Serial.print("*");
    eeprom_write_word(EEPROM_H_ADDR, hot_cntr);
    eeprom_write_word(EEPROM_C_ADDR, cold_cntr);
    last_hot_cntr=hot_cntr;
    last_cold_cntr=cold_cntr;
  }
  Serial.print("Hot: ");
  Serial.print(hot_cntr);
  Serial.print(" Cold: ");
  Serial.print(cold_cntr);
  Serial.println();

  if (Serial.available() > 0) {
    if (Serial.find("reset"))
      Serial.println("RESET meters");
      hot_cntr=0;
      cold_cntr=0;
  }

}
