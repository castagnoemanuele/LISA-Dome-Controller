#include <button.h>
#include <Preferences.h>


enum Direction {
  UNKNOWN,
  CW,
  CCW,
};

struct Encoder {
	float currentPosition;
  bool hasChanged;
  int fullRotation; ///number of encoder rotation corresponding to a full rotation of the dome
  int oldSector;
  int newSector;

  Direction direction;
  //bool status[3];
};


/// @brief Handles encoder Data
void checkEncoder(Encoder& encoder1);

/// @brief Counts how many Ticks happen to make a full rotation of the dome
/// @return number of ticks counted
uint8_t countTicksFullRotation (Encoder& encoder1, Button& buttonReset, Button& limitSwitch);

/// @brief call this function to disable all controls and wait for the dome to return to the original position
void resetPosition(Encoder& encoder1, Button& buttonReset, Button& limitSwitch);

/// @brief saves data to the EEPROM to a specified location
/// @param data The data to be saved.
/// @param address The address in the EEPROM where the data will be saved.
/// @param preferences The preferences object.
void saveData (float data, const char *address, Preferences& preferences);





