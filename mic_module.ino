//= DEFINES ========================================================================================

#define SOUND_SENSOR_PIN A1  // ESP32

//= INCLUDES =======================================================================================
#include <Wire.h>

//= CONSTANTS ======================================================================================
const int SAMPLE_WINDOW = 100;  // Sample window width in mS (50 mS = 10Hz)

//= VARIABLES ======================================================================================
int decibelLevel = 0;

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
long mic_GetBecibelLevel() {
  unsigned int sample = 0;
  unsigned long startMillis = millis();  // Start of sample window
  unsigned int peakToPeak = 0;           // peak-to-peak level

  unsigned int signalMax = 0;     //minimum value
  unsigned int signalMin = 4095;  //maximum value

  // collect data for 50 mS
  while (millis() - startMillis < SAMPLE_WINDOW) {
    sample = analogRead(SOUND_SENSOR_PIN);  //get reading from microphone

    if (sample < 4095) {
      if (sample > signalMax) {
        signalMax = sample;  // save just the max levels
      } else if (sample < signalMin) {
        signalMin = sample;  // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin;        // max - min = peak-peak amplitude
  double volts = (peakToPeak * OPERATING_VOLTAGE) / 4.095;  // convert to volts

  long db = map(peakToPeak, 1, 1000, 30, 100);  // calibrate for deciBels

#ifdef DEBUG_MIC
    Serial.print("MIC:|voltage = [");
    Serial.print(volts);
    Serial.print("]|peakToPeak = [");
    Serial.print(peakToPeak);
    Serial.print("]");
    Serial.println();
#endif

#ifdef DEBUG
  Serial.print("Loudness: ");
  Serial.print(db);
  Serial.print("dB");
  Serial.println();
#endif

  decibelLevel = db;
  return db;
}
//==================================================================================================