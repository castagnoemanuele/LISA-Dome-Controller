/// @file encoder.h
/// @brief This file contains the implementation of the Encoder class methods, including functions to check the encoder position,
/// update the dome position, count the number of ticks for a full rotation, and save data to the EEPROM.


#include <button.h>
#include <Preferences.h>

#ifndef ENCODER_H
#define ENCODER_H

enum Direction {
  UNKNOWN,
  CW,
  CCW,
};

struct Encoder {
	int currentPosition; ///current position in ticks
  int currentDegrees; ///current position in degrees
  bool hasChanged; ///flag to check if the encoder has changed
  int fullRotation; ///number of encoder rotation corresponding to a full rotation of the dome
  int oldSector; ///the sector where the dome was before the last movement
  int newSector; ///the sector where the dome is after the last movement

  Direction direction;
  //bool status[3];
};


/// @brief Handles encoder Data
void checkEncoder(Encoder& encoder1);
/// @brief Updates the position of the dome according to the rotation of the encoder
/// @param encoder1 the encoder object
/// @param buttonReset the button object
/// @param limitSwitch the button object
void updatePosition(Encoder& encoder1);

/// @brief Counts how many Ticks happen to make a full rotation of the dome and saves the data to the EEPROM
/// @return number of ticks counted
long countTicksFullRotation (Encoder& encoder1, Button& buttonReset, Button& limitSwitch, Preferences& preferences);

/// @brief call this function to disable all controls and wait for the dome to return to the original position
void resetPosition(Encoder& encoder1, Button& buttonReset, Button& limitSwitch);

/// @brief saves data to the EEPROM to a specified location
/// @param data The data to be saved.
/// @param address The address in the EEPROM where the data will be saved.
/// @param preferences The preferences object.
void saveData (int data, const char *address, Preferences& preferences);

int convertTicksToDegrees(int ticks, Encoder& encoder1);


#endif // ENCODER_H

