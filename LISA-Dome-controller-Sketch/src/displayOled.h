#ifndef DISPLAYOLED_H
#define DISPLAYOLED_H
#include <Arduino.h> // necessary header file for the "String" class
#include <Adafruit_SSD1306.h> // necessary header file for the "Adafruit_SSD1306" class

class DisplayOled {
public:
    void displayMessage(String message, Adafruit_SSD1306& display);
    String currentData; // Used to hold the l needUpdate data from server
    String FirstRow; // Used to hold the string that appears in the first display row
    int displayCounter; // Used to reset display when full
    int rowCounter; // Used to print to the second column correctly
    DisplayOled();
    void initDisplay(Adafruit_SSD1306& display);

private:
    // Add private member variables as needed
};



#endif // DISPLAYOLED_H