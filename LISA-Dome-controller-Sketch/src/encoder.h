#include <config.h>



enum Direction {
  UNKNOWN,
  CW,
  CCW,
};

struct Encoder {
	int currentPosition;
  bool hasChanged;
  int fullRotation; ///number of encoder rotation corresponding to a full rotation of the dome
  int oldSector;
  int newSector;

  Direction direction;
  //bool status[3];
};


/// @brief Handles encoder Data
void checkEncoder(Encoder encoder1);





