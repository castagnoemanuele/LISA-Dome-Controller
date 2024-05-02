#ifndef DISPLAYOLED_H
#define DISPLAYOLED_H
#include <Arduino.h> // necessary header file for the "String" class
#include <Adafruit_SSD1306.h> // necessary header file for the "Adafruit_SSD1306" class
#include <Encoder.h>
#include "Telescope.h"

class DisplayOled {
public:
    void displayMessage(String message, Adafruit_SSD1306& display);
    String currentData; // Used to hold the l needUpdate data from server
    String FirstRow; // Used to hold the string that appears in the first display row
    bool secondRow; // indicates if the message should be displayed in the second row
    int rowCounter; // Used to print to the to the correct row
    DisplayOled();
    void initDisplay(Adafruit_SSD1306& display, Encoder& encoder, Telescope& LISA);
    void printWifiStatus(Adafruit_SSD1306& display);
    void printPositionStatus(Adafruit_SSD1306& display, Encoder& encoder, Telescope& LISA);

private:
    // Add private member variables as needed
};



#endif // DISPLAYOLED_H