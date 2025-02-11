/*
  PIN CONNECTIONS
  -------------------------------
  -------------------------------
  -------------------------------

*/
//= DEFINES ========================================================================================
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define DEBUG
//#define DEBUG_MIC
//#define DEBUG_BCM

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define OFF 0x1
#define ON 0x0

#define VCC_SENSOR_PIN A0
#define SOUND_SENSOR_PIN A1

#ifdef DEBUG
#define PERIOD_MEASUREMENT 1
#else
#define PERIOD_MEASUREMENT 1 * 60
#endif

#define PERIOD_READ_VOLTAGE 1 * 60
//#define PERIOD_READ_VOLTAGE 5 * 60  // 5 minutes
#define PERIOD_READ_VOLTAGE_SEC PERIOD_READ_VOLTAGE *SEC  // 5 minutes

//= INCLUDES =======================================================================================
#include "Common.h"
#include "fdrs_node_config.h"
#include <fdrs_node.h>
#include "driver/temperature_sensor.h"

//= CONSTANTS ======================================================================================

//= VARIABLES ======================================================================================
temperature_sensor_handle_t temp_handle = NULL;

Measurement measurement = { 0, 0, 0, 0, 0 };

unsigned long lastVoltageMeasurementMillis = 0;
unsigned long lastNoiseMeasurementDba = 0;

//##################################################################################################
//==================================================================================================
//**************************************************************************************************
void setup() {
#if defined(DEBUG) || defined(DEBUG_V) || defined(DEBUG_BCM)
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) { ; }
  delay(5 * SEC);
#endif
  debugPrintln(F("START-UP >>>>>>>>>>>>>>>"));
  //..............................
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, ON);
  //..............................
  //
  bcm_Setup();
  //
  mic_Setup();
  //
  esp32_Setup_TempSensor();
  //
  beginFDRS();
  //
  digitalWrite(LED_BUILTIN, OFF);
  //..............................
  debugPrintln(F("START-UP <<<<<<<<<<<<<<<"));
}
//**************************************************************************************************
//OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
void loop() {
  debugPrintln(F("##############################"));
  //
  bool shouldPublishMeasurements = collectMeasurements();
  //
  if (shouldPublishMeasurements) {
    debugPrintln(F("MAIN: Publish measurements"));
    //
    digitalWrite(LED_BUILTIN, ON);
    //
    bool was_published = sendFDRS();
    //
    if (was_published) {
      debugPrintln("Succesfuly published measurements");
    } else {
      DBG("FAILED");
      //debugPrintln("FAILED to publish measurements");
    }
    //
    digitalWrite(LED_BUILTIN, OFF);
  }
  //
  sleepFDRS(PERIOD_MEASUREMENT);
}
//OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
//==================================================================================================
bool collectMeasurements() {
  debugPrintln(F("MAIN: Collect measurements"));
  //
  bool newMeasurement = false;
  //
  // TODO: measure something else and if needed, newMeasurement => if we save useful metric, add VCC info, too
  //
  if (__ShouldMeasureNoise() || newMeasurement) {
    debugPrintln(F("MAIN: Measure noise"));
    //
    measurement.noise_decibel = mic_GetBecibelLevel();
    //
    loadFDRS(measurement.noise_decibel, NOISE_T);
    //
    newMeasurement = true;
  }
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
  if (__ShouldMeasureTemperature() || newMeasurement) {
    debugPrintln(F("MAIN: Temperature"));
    //
    measurement.temperature = 0;
    esp32_Read_TempSensor(&measurement.temperature);
    //
    loadFDRS(measurement.temperature, TEMP_T);
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
bool __ShouldMeasureNoise() {
  return true;
}
//==================================================================================================
bool __ShouldMeasureVoltage() {
  if (millis() - lastVoltageMeasurementMillis <= PERIOD_READ_VOLTAGE_SEC) {
    return false;
  } else {
    lastVoltageMeasurementMillis = millis();
    return true;
  }
}
//==================================================================================================
bool __ShouldMeasureTemperature() {
  return false;
}
//==================================================================================================
void esp32_Setup_TempSensor() {
  temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(-10, 40);
  temperature_sensor_install(&temp_sensor_config, &temp_handle);
}
//==================================================================================================
void esp32_Read_TempSensor(float *celsius) {
  temperature_sensor_enable(temp_handle);
  temperature_sensor_get_celsius(temp_handle, celsius);
  temperature_sensor_disable(temp_handle);
}
//==================================================================================================
