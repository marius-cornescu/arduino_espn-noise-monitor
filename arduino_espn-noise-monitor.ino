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
//#define DEBUG_MQTT
//#define DEBUG_BCM
#define DEBUG_MEM

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
#define PUBLISH_TOPIC "home/solar-powered/unit-X/status"
#define SUBSCRIBE_TOPIC "home/solar-powered/unit-X/command/+"
//
#define OFF 0x1
#define ON 0x0

#define PERIOD_READ_VOLTAGE 5 * 60 * SEC  // 5 minutes

//= INCLUDES =======================================================================================
#include "Common.h"

//= CONSTANTS ======================================================================================
const byte LED_INDICATOR_PIN = LED_BUILTIN;  // choose the pin for the LED // D13

//= VARIABLES ======================================================================================
unsigned long lastVoltageMeasurementMillis = 0;

bool publishStateAtStartup = true;

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
  wifi_Setup();
  //
  mqtt_Setup();
  //
  collectMeasurements();
  publishMeasurements();
  publishStateAtStartup = false;
  //
  wifi_LowPower_Sleep();
  //
  digitalWrite(LED_INDICATOR_PIN, OFF);
  //..............................
  debugPrintln(F("START-UP <<<<<<<<<<<<<<<"));
}
//**************************************************************************************************
//OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
void loop() {
  if (shouldPublishMeasurements()) {
    publishMeasurements();
  }

  collectMeasurements();
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
  if (__ShouldMeasureVoltage() || publishStateAtStartup || newMeasurement) {
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
  return response;
}
//==================================================================================================
void publishMeasurements() {
  debugPrintln(F("MAIN: Publish measurements"));
  //
  wifi_LowPower_Wake();
  //
  mqtt_MaintainConnection();
  //
  // TODO: read the data from storage
  //
  if (mqtt_ShouldPublish() || publishStateAtStartup) {
    publishXXXDataToMqtt();
  }
  //
  wifi_LowPower_Sleep();
  //
}
//==================================================================================================
void publishXXXDataToMqtt() {
#ifdef DEBUG
  digitalWrite(LED_INDICATOR_PIN, ON);
#endif
  //
  unsigned int operating_voltage = bcm_ReadOperatingVoltageMilliV();
  unsigned int battery_voltage = bcm_ReadBatteryVoltageMilliV();
  //
  char statusReport[100];
  sprintf(statusReport, "{\"vcc\": %d, \"battery_vcc\": %d }", operating_voltage, battery_voltage);
  mqtt_PublishString(PUBLISH_TOPIC, statusReport);
  //
#ifdef DEBUG
  digitalWrite(LED_INDICATOR_PIN, OFF);
#endif
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