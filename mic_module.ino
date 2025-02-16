//= DEFINES ========================================================================================

#define SOUND_SENSOR_PIN A1  // ESP32

//= INCLUDES =======================================================================================
#include "Common.h"
#include <Wire.h>

//= CONSTANTS ======================================================================================
const float OPERATING_VOLTAGE = 3.3;
const int SAMPLE_WINDOW = 100;  // Sample window width in mS (50 mS = 10Hz)

const unsigned long ANALOG_RESOLUTION = 4095;
const unsigned long MIN_ANALOG_VAL = 0;
const unsigned long MAX_ANALOG_VAL = 4095;

const unsigned long MIN_DB_VAL = 0;
const unsigned long MAX_DB_VAL = 4095;

//= VARIABLES ======================================================================================
unsigned int decibelLevel = 0;

unsigned long minLevelDB = MAX_ANALOG_VAL;
unsigned long maxLevelDB = MIN_ANALOG_VAL;

//##################################################################################################
//==================================================================================================
//**************************************************************************************************
void mic_Setup() {
  debugPrintln(F("MIC:Setup >>>"));
  //..............................
  pinMode(SOUND_SENSOR_PIN, INPUT);  // Set the sound signal pin as input
  analogSetPinAttenuation(SOUND_SENSOR_PIN, ADC_11db);
  //..............................
  delay(1 * TIME_TICK);
  debugPrintln(F("MIC:Setup <<<"));
}
//**************************************************************************************************
//==================================================================================================
unsigned int mic_GetBecibelLevelAverage(byte sampleSize) {
  if (sampleSize == 0) {
    sampleSize = 1;
  } else if (sampleSize > 10) {
    sampleSize = 10;
  }

  unsigned int db = mic_GetBecibelLevel(sampleSize);

#ifdef DEBUG_MIC
  debugPrint(F("Loudness:"));
  debugPrint(db);
  debugPrintln(F(""));
#endif

  return db;
}
//==================================================================================================
unsigned int mic_GetBecibelLevel(byte sampleSize) {
  unsigned long sample = 0;

  unsigned long startMillis = millis();      // Start of sample window
  unsigned long peakToPeak = 0;              // peak-to-peak level
  unsigned long signalMin = MAX_ANALOG_VAL;  //maximum value
  unsigned long signalMax = MIN_ANALOG_VAL;  //minimum value

  // collect data for 50 mS
  while (millis() - startMillis < SAMPLE_WINDOW * sampleSize) {
    sample = analogRead(SOUND_SENSOR_PIN);  //get reading from microphone

    __recordMinMax_IfValuesChanged(sample);

    if (sample < MAX_ANALOG_VAL) {
      if (sample > signalMax) {
        signalMax = sample;  // save just the max levels
      } else if (sample < signalMin) {
        signalMin = sample;  // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude

  unsigned int db = map(peakToPeak, MIN_ANALOG_VAL, MAX_ANALOG_VAL, 30, 100);  // calibrate for deciBels

#ifdef DEBUG_MIC
  debugPrint(F("peakToPeak:"));
  debugPrint(peakToPeak);
  debugPrint(F(","));
  debugPrint(F("Loudness:"));
  debugPrint(db);
  debugPrintln(F(""));
#endif

  decibelLevel = db;
  return db;
}
//==================================================================================================
void __recordMinMax_IfValuesChanged(unsigned long sample) {
  bool shouldPrintData = false;
  //
  if (sample < minLevelDB) {
    minLevelDB = sample;
    shouldPrintData = true;
  }
  //
  if (sample > maxLevelDB) {
    maxLevelDB = sample;
    shouldPrintData = true;
  }
  //
  if (shouldPrintData) {
#ifdef DEBUG_MIC
    debugPrint(F("MIC:|minLevelDB["));
    debugPrint(minLevelDB);
    debugPrint(F("]|maxLevelDB["));
    debugPrint(maxLevelDB);
    debugPrintln(F("]"));
#endif
  }
}
//==================================================================================================