//  FARM DATA RELAY SYSTEM
//
//  ESP-NOW Sensor Example
//  [LOLIN D1 mini]
//  Developed by Timm Bogner (timmbogner@gmail.com) in Urbana, Illinois, USA.
//
//= DEFINES ========================================================================================
//#define DEBUG

//= INCLUDES =======================================================================================
#include "fdrs_node_config.h"
#include <fdrs_node.h>

//= CONSTANTS ======================================================================================

//= VARIABLES ======================================================================================
float data1;
float data2;

//##################################################################################################
//==================================================================================================
//**************************************************************************************************
void setup() {
#ifdef DEBUG
  // Open serial communications and wait for port to open:
  // Serial.begin(115200);
  // while (!Serial) { ; }
  Serial.println(F("ESP-Now:Node:START-UP >>>>>>>>>>>>>>>"));
#endif

  beginFDRS();
}
//**************************************************************************************************
//OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
void loop() {
  data1 = readHum();
  loadFDRS(data1, HUMIDITY_T);
  data2 = readTemp();
  loadFDRS(data2, TEMP_T);
  //  DBG(sendFDRS());
  if (sendFDRS()) {
    DBG("Big Success!");
  } else {
    DBG("Nope, not so much.");
  }
  sleepFDRS(30);  //Sleep time in seconds
}
//OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
//==================================================================================================
float readTemp() {
  return 22.069;
}
//==================================================================================================
float readHum() {
  return random(0, 100);
}
//==================================================================================================