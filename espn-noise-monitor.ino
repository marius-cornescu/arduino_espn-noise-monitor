/*
  PIN CONNECTIONS
  -------------------------------
  -------------------------------
  -------------------------------
*/
//= DEFINES ========================================================================================
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//#define DEBUG
//#define DEBUG_V
//#define DEBUG_BCM
//#define DEBUG_MEM

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define OFF 0x1
#define ON 0x0

#define PERIOD_READ_VOLTAGE 5 * 60 * SEC  // 5 minutes

//= INCLUDES =======================================================================================
#include "Common.h"
#include "fdrs_node_config.h"
#include <fdrs_node.h>

//= CONSTANTS ======================================================================================
const byte LED_INDICATOR_PIN = LED_BUILTIN;  // choose the pin for the LED // D13

//= VARIABLES ======================================================================================
Measurement measurement = { 0, 0, 0, 0 };
bool shouldPublishMeasurements = false;

unsigned long lastVoltageMeasurementMillis = 0;

unsigned long lastNoiseMeasurementDba = 0;

//##################################################################################################
//==================================================================================================
//**************************************************************************************************
void setup() {
#if defined(DEBUG) || defined(DEBUG_V) || defined(DEBUG_BCM) || defined(DEBUG_MEM)
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) { ; }
#endif
  debugPrintln(F("START-UP >>>>>>>>>>>>>>>"));
  //..............................
  // initialize digital pin LED_INDICATOR_PIN as an output.
  pinMode(LED_INDICATOR_PIN, OUTPUT);
  //..............................
  //
  bcm_Setup();
  //
  mic_Setup();
  //
  beginFDRS();
  //
  shouldPublishMeasurements = collectMeasurements();
  //
  digitalWrite(LED_INDICATOR_PIN, OFF);
  //..............................
  debugPrintln(F("START-UP <<<<<<<<<<<<<<<"));
}
//**************************************************************************************************
//OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
void loop() {
  //
  bool was_published = false;
  //
  if (shouldPublishMeasurements) {
    debugPrintln(F("MAIN: Publish measurements"));
    //
#ifdef DEBUG
    digitalWrite(LED_INDICATOR_PIN, ON);
#endif
    //
    was_published = sendFDRS();
    //
#ifdef DEBUG
    digitalWrite(LED_INDICATOR_PIN, OFF);
#endif
  }
  //
  collectMeasurements();
  //
  if (was_published) {
    debugPrintln("Succesfuly published measurements");
    sleepFDRS(5 * 60);  // Sleep time in seconds
  } else {
    debugPrintln("FAILED to publish measurements");
    sleepFDRS(30);  // Sleep time in seconds
  }
  //
}
//OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
//==================================================================================================
bool collectMeasurements() {
  //
  bool newMeasurement = false;
  //
  // TODO: measure something else and if needed, newMeasurement => if we save useful metric, add VCC info, too
  //
  if (__ShouldMeasureVoltage() || newMeasurement) {
    debugPrintln(F("MAIN: Measure voltage"));
    //
    measurement.voltage_vcc = bcm_ReadOperatingVoltageMilliV();
    measurement.voltage_battery = bcm_ReadBatteryVoltageMilliV();
    //
    loadFDRS(measurement.voltage_vcc, VOLTAGE_T);
    loadFDRS(measurement.voltage_battery, VOLTAGE2_T);
    //
    newMeasurement = true;
  }
  //
  if (__ShouldMeasureNoise() || newMeasurement) {
    debugPrintln(F("MAIN: Measure noise"));
    //
    measurement.noise_decibel = 0;//int decibelLevel = mic_GetBecibelLevel();
    //
    loadFDRS(measurement.noise_decibel, NOISE_T);
    //
    newMeasurement = true;
  }
  //
  if (newMeasurement) {
    measurement.time_stamp = millis();
  }
  //
  return newMeasurement;
}
//==================================================================================================
bool __ShouldMeasureVoltage() {
  if (millis() - lastVoltageMeasurementMillis <= PERIOD_READ_VOLTAGE) {
    return false;
  } else {
    lastVoltageMeasurementMillis = millis();
    return true;
  }
}
//==================================================================================================
bool __ShouldMeasureNoise() {
  return true;
}
//==================================================================================================