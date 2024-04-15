#include <HardwareSerial.h>
#include <telescope.h>





// Function to convert Right Ascension from HH:MM:SS format to degrees
int Telescope::convertRA(char* ra) {
  int hours, minutes, seconds;
  sscanf(ra, "%d:%d:%d", &hours, &minutes, &seconds);

  // Convert hours, minutes, and seconds to degrees
  int degrees = hours * 15 + minutes * 0.25 + seconds * 0.00416667;

  return degrees;
}

/// @brief first ask, then print telescope position and save it
void Telescope::checkTelescopePosition() {
  if (Serial1.availableForWrite()) {
    // Ask the telescope for the Right Ascension
    Serial1.println(":GR#");
    //Clean the buffer
    while (Serial1.available() > 0) {
      Serial1.read();
    }
    // Listen on the Serial1 for the answer and save it
    char byte[8];
    do {
      Serial.println("waiting for telescope position");
      delay(1000);
    } while (Serial1.available() <= 8);
    // TODO: Add a check for not correctly formatted answers

    Serial1.readBytesUntil('#', byte, 8);
    // Check if the received position is correctly formatted
    if (strlen(byte) == 7 && byte[2] == ':' && byte[5] == ':') {
      Serial.println("Telescope position received: ");
      Serial.println(byte);

      // Convert the data received in degrees
      int degrees = convertRA(byte);

      // Save data to telescope object
      telescopePosition = degrees;
    }
    else {
      Serial.println("Telescope position not correctly formatted");
    }
  }
}

