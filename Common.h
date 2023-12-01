#ifndef _HEADERFILE_COMMON
#define _HEADERFILE_COMMON
//= CONSTANTS ======================================================================================
#define SW_VERSION "2023.12.01"
//------------------------------------------------
//------------------------------------------------
#define ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))
//------------------------------------------------
#ifdef DEBUG
#define TIME_TICK 40L
#else
#define TIME_TICK 10L
#endif

#define SEC 100 * TIME_TICK  // 1 second
//------------------------------------------------
#ifdef DEBUG
	#define debugPrint(x) Serial.print(x) //debug on
	#define debugPrintln(x) Serial.println(x) //debug on
#else
	#define debugPrint(x) {;} //debug off
	#define debugPrintln(x) {;} //debug off
#endif
//------------------------------------------------
//= INCLUDES =======================================================================================

//= VARIABLES ======================================================================================

//------------------------------------------------
struct Measurement {
  unsigned int time_stamp = 0;
  unsigned int voltage_vcc = 0;
  unsigned int voltage_battery = 0;
  unsigned int noise_decibel = 0;
};
//------------------------------------------------

//==================================================================================================
#endif // _HEADERFILE_COMMON
