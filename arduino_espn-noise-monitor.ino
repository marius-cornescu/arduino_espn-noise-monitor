/*
  PIN CONNECTIONS
  -------------------------------
  -------------------------------
  -------------------------------
*/
//= DEFINES ========================================================================================
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define DEBUG
//#define DEBUG_V
//#define DEBUG_BCM
#define DEBUG_MEM

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
unsigned long lastVoltageMeasurementMillis = 0;

//##################################################################################################
//==================================================================================================
//**************************************************************************************************
void setup() {
#if defined(DEBUG) || defined(DEBUG_V) || defined(DEBUG_MQTT) || defined(DEBUG_BCM) || defined(DEBUG_MEM)
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
  memory_Setup();
  //
  bcm_Setup();
  //
  beginFDRS();
  //
  collectMeasurements();
  publishMeasurements();
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
  if (shouldPublishMeasurements()) {
    was_published = publishMeasurements();
  }
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
void collectMeasurements() {
  //
  bool newMeasurement = false;
  Measurement* measurement = memory_NewMeasurement();
  //
  // TODO: measure something else and if needed, newMeasurement => if we save useful metric, add VCC info, too
  //
  if (__ShouldMeasureVoltage() || newMeasurement) {
    debugPrintln(F("MAIN: Measure voltage"));
    //
    measurement->voltage_vcc = bcm_ReadOperatingVoltageMilliV();
    measurement->voltage_battery = bcm_ReadBatteryVoltageMilliV();
    //
    newMeasurement = true;
  }
  //
  if (newMeasurement) {
    measurement->time_stamp = millis();
    memory_Save(measurement);
  }
}
//==================================================================================================
bool shouldPublishMeasurements() {
  bool response = false;
  response = response || memory_ShouldFlush();
  //return response;
  return true;
}
//==================================================================================================
bool publishMeasurements() {
  debugPrintln(F("MAIN: Publish measurements"));
  //
  // TODO: read the data from storage
  //
#ifdef DEBUG
  digitalWrite(LED_INDICATOR_PIN, ON);
#endif
  //
  unsigned int operating_voltage = bcm_ReadOperatingVoltageMilliV();
  unsigned int battery_voltage = bcm_ReadBatteryVoltageMilliV();
  //
  loadFDRS(operating_voltage, VOLTAGE_T);
  loadFDRS(battery_voltage, VOLTAGE2_T);
  //
  bool was_published = sendFDRS();
  //
#ifdef DEBUG
  digitalWrite(LED_INDICATOR_PIN, OFF);
#endif
  //
  return was_published;
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