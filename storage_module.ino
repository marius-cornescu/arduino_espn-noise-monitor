//= DEFINES ========================================================================================
//#define DEBUG_MEM

//------------------------------------------------
#ifdef DEBUG_MEM
#define debugMemPrint(x) Serial.print(x)      //debug on
#define debugMemPrintln(x) Serial.println(x)  //debug on
#else
#define debugMemPrint(x) { ; }  //debug off
#define debugMemPrintln(x) { ; }  //debug off
#endif
//------------------------------------------------

//= INCLUDES =======================================================================================

//= CONSTANTS ======================================================================================

// //------------------------------------------------
// struct Measurement {
//   unsigned int time_stamp = 0;
//   unsigned int voltage_vcc = 0;
//   unsigned int voltage_battery = 0;
//   unsigned int noise_decibel = 0;
// };
// //------------------------------------------------

//= VARIABLES ======================================================================================
#define STORE_SIZE 20
#define STORE_ACTUAL_SIZE 21
Measurement store[STORE_ACTUAL_SIZE] = {
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  // extra space
  { 0, 0, 0, 0 }  // <=== END - POSITION 20
};

byte store_current_index = 0;

//##################################################################################################
//==================================================================================================
//**************************************************************************************************
void memory_Setup() {
  debugPrintln(F("Memory:Setup >>>"));
  //..............................
  //pinMode(VCC_SENSOR_PIN, INPUT);
  debugMemPrint(F("Memory:Size = "));
  debugMemPrintln(STORE_SIZE);
  //..............................
  delay(1 * TIME_TICK);
  debugPrintln(F("Memory:Setup <<<"));
}
//**************************************************************************************************
//==================================================================================================
Measurement *memory_NewMeasurement() {
  if (memory_ShouldFlush()) {
    debugMemPrintln(F("Memory:Reached end of storage / reset to first possition."));
    memory_Flush();
  }
  return &store[store_current_index];
}
//==================================================================================================
void memory_Save(Measurement *measurement) {
  debugMemPrint(F("Memory:Save pos: "));
  debugMemPrintln(store_current_index);
  // TODO:
  store_current_index++;
}
//==================================================================================================
bool memory_ShouldFlush() {
  return store_current_index >= STORE_SIZE;
}
//==================================================================================================
void memory_Flush() {
  debugMemPrintln(F("Memory:Flush"));
  // TODO: reset all the entries back to 0
  store_current_index = 0;
}
//==================================================================================================
Measurement *memory_ReadMeasurement(byte store_read_index) {
  debugMemPrint(F("Memory:ReadMeasurement pos: "));
  debugMemPrintln(store_read_index);
  //
  if (store_read_index > STORE_SIZE) {
    store_read_index = STORE_SIZE;
  }
  return &store[store_read_index];
}
//==================================================================================================
byte memory_Size() {
  return STORE_SIZE;
}
//==================================================================================================
//==================================================================================================