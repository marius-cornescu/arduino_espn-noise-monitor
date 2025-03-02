//= DEFINES ========================================================================================

#define SOUND_SENSOR_PIN A1  // ESP32

//= INCLUDES =======================================================================================
#include "Common.h"
#include <Wire.h>

//= CONSTANTS ======================================================================================
const unsigned int SAMPLE_WINDOW = 100;  // Sample window width in mS (50 mS = 10Hz)
const unsigned int MAX_SAMPLE_COUNT = 100;

const unsigned int MIN_ANALOG_VAL = 0;
const unsigned int MAX_ANALOG_VAL = 4096;

//= VARIABLES ======================================================================================
//uint16_t noiseLevel = 0;

#ifdef DEBUG_MIC
unsigned int minSampleVal = MAX_ANALOG_VAL;
unsigned int maxSampleVal = MIN_ANALOG_VAL;
#endif

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
int mic_GetNoiseLevelAverage(byte sampleSize) {
  if (sampleSize <= 0) {
    sampleSize = 1;
  } else if (sampleSize > MAX_SAMPLE_COUNT) {
    sampleSize = MAX_SAMPLE_COUNT;
  }

  int response = MIN_ANALOG_VAL;

  for (byte i = 0; i < sampleSize; i++) {
    int sample_val = mic_GetNoiseLevel();
    if (sample_val > response) {
      response = sample_val;
    }
    delay(10L);
  }

#ifdef DEBUG_MIC
  debugPrint(F("Loudness:"));
  debugPrint(response);
  debugPrintln(F(""));
#endif

  return response;
}
//==================================================================================================
int mic_GetNoiseLevel() {
  unsigned long startMillis = millis();     // Start of sample window

  unsigned int sample = 0;
  unsigned int signalMin = MAX_ANALOG_VAL;  //maximum value
  unsigned int signalMax = MIN_ANALOG_VAL;  //minimum value

  int peakToPeak = 0;              // peak-to-peak level

  // collect data for the sample window
  while (millis() - startMillis < SAMPLE_WINDOW) {
    sample = analogRead(SOUND_SENSOR_PIN);  //get reading from microphone
    //__recordMinMax_IfValuesChanged(sample);
    //debugPrintln(sample);

    if (sample < MAX_ANALOG_VAL) {
      if (sample > signalMax) {
        signalMax = sample;  // save the max levels
      }
      if (sample < signalMin) {
        signalMin = sample;  // save the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude

  //noiseLevel = map(peakToPeak, MIN_ANALOG_VAL, MAX_ANALOG_VAL, 30, 100);  // calibrate for Noises
  int response = peakToPeak;

  //Serial.println(peakToPeak);

#ifdef DEBUG_MIC
  debugPrint(F("signalMin:"));
  debugPrint(signalMin);
  debugPrint(F(","));
  debugPrint(F("signalMax:"));
  debugPrint(signalMax);
  debugPrint(F(","));
  // debugPrint(F("peakToPeak:"));
  // debugPrint(peakToPeak);
  // debugPrint(F(","));
  debugPrint(F("Loudness:"));
  debugPrint(response);
  debugPrintln(F("dBA"));
#endif

  return response;
}
//==================================================================================================
void __recordMinMax_IfValuesChanged(unsigned int sample) {
#ifdef DEBUG_MIC
  bool shouldPrintData = false;
  //
  if (sample < minSampleVal) {
    minSampleVal = sample;
    shouldPrintData = true;
  }
  //
  if (sample > maxSampleVal) {
    maxSampleVal = sample;
    shouldPrintData = true;
  }
  //
  if (shouldPrintData) {
    debugPrint(F("MIC:|minSampleVal["));
    debugPrint(minSampleVal);
    debugPrint(F("]|maxSampleVal["));
    debugPrint(maxSampleVal);
    debugPrintln(F("]"));
  }
#endif
}
//==================================================================================================