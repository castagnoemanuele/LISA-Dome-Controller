#include <HardwareSerial.h>
#include <telescope.h>


int Telescope::getTelescopePosition() {
  return telescopePosition;
}

// Function to convert Right Ascension from HH:MM:SS format to degrees
int Telescope::convertRA(char* ra) {
  int hours, minutes, seconds;
  sscanf(ra, "%d:%d:%d", &hours, &minutes, &seconds);

  Serial.print("Hours: ");
  Serial.println(hours);
  Serial.print("Minutes: ");
  Serial.println(minutes);
  Serial.print("Seconds: ");
  Serial.println(seconds);


  // Convert hours, minutes, and seconds to degrees
  int degrees = hours * 15 + minutes * 0.25 + seconds * 0.00416667;
  Serial.print("Degrees: ");
  Serial.println(degrees);
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
    char byte[8] = {0};
    do {
      Serial.println("waiting for telescope position");
      delay(1000);
    } while (Serial1.available() <= 8);
    
    Serial1.readBytes( byte, 8);
    // Check if the received position is correctly formatted
    if (byte[2] == ':' && byte[5] == ':') {
      Serial.println("Telescope position received: ");
      
  
      Serial.println(byte);

      // Convert the data received in degrees
      int degrees = convertRA(byte);

      // Save data to telescope object
      telescopePosition = degrees;
      
      Serial.print("Telescope position saved: ");
      Serial.println(telescopePosition);
    }
    else {
      Serial.println("Telescope position not correctly formatted");
      Serial.println(byte);
    }
  }
}

