//= DEFINES ========================================================================================
//#define DEBUG_BCM

#define VCC_SENSOR_PIN A0  // ESP3288

#define R1 235.0   // 100 kilo-ohm
#define R2 150.0  // 300 kilo-ohm

//= INCLUDES =======================================================================================

//= CONSTANTS ======================================================================================
const float OPERATING_VOLTAGE = 3.3;
const float RESISTOR_FACTOR = (R1 + R2) / R2;
const float MILLI_VOLT = 1000.0;

//= VARIABLES ======================================================================================
int adc_value = 0;
float adc_voltage = 0.0;
float in_voltage = 0.0;

//##################################################################################################
//==================================================================================================
//**************************************************************************************************
void bcm_Setup() {
  debugPrintln(F("Bcm:Setup >>>"));
  //..............................
  //pinMode(VCC_SENSOR_PIN, INPUT);
  //..............................
  delay(1 * TIME_TICK);
  debugPrintln(F("Bcm:Setup <<<"));
}
//**************************************************************************************************
//==================================================================================================
unsigned int bcm_ReadOperatingVoltageMilliV() {
  return (unsigned int)(OPERATING_VOLTAGE * MILLI_VOLT);
}
//==================================================================================================
unsigned int bcm_ReadBatteryVoltageMilliV() {
  adc_value = analogRead(VCC_SENSOR_PIN);
  adc_voltage = (adc_value * OPERATING_VOLTAGE * MILLI_VOLT) / 1024.0;
  float battery_vcc = adc_voltage * RESISTOR_FACTOR;

#ifdef DEBUG_BCM
  debugPrint(F("BCM:RESISTOR_FACTOR = "));
  debugPrint(RESISTOR_FACTOR);
  debugPrint(F(" | adc_value = "));
  debugPrint(adc_value);
  debugPrint(F(" | adc_voltage = "));
  debugPrint(adc_voltage);
  debugPrint(F(" | battery_vcc = "));
  debugPrintln(battery_vcc);
#endif

  return battery_vcc;
}
//==================================================================================================
// long _readVcc() {
//   long result;
//   // Read 1.1V reference against AVcc
// #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
//   ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
// #elif defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
//   ADMUX = _BV(MUX5) | _BV(MUX0);
// #elif defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
//   ADMUX = _BV(MUX3) | _BV(MUX2);
// #else
//   ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
// #endif
//   delay(2);             // Wait for Vref to settle
//   ADCSRA |= _BV(ADSC);  // Convert
//   while (bit_is_set(ADCSRA, ADSC))
//     ;
//   result = ADCL;
//   result |= ADCH << 8;
//   result = 1126400L / result;  // Calculate Vcc (in mV); 1126400 = 1.1*1024*1000
//   return result;
// }
//==================================================================================================