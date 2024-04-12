#include <HardwareSerial.h>




/// @brief first ask, then print telescope position and save it
void checkTelescopePosition() {
  if(Serial1.availableForWrite()){
    //Ask the telescope for the Right Ascension
    Serial1.println(":GR#");
    //Listen on the Serial1 for the answer and save it
    char byte[8];
    Serial1.readBytesUntil('#',byte,8);

    //missing code for conversion and saving of the telescope position
  }
}

// int covertRA   FUNC TO obtain degrees

