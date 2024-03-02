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





