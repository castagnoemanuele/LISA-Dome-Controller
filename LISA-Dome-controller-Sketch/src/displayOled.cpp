#include "displayOled.h"
#include <config.h>
#include <logo.h>
DisplayOled::DisplayOled() : displayCounter(0), rowCounter(0) {
    // Il costruttore inizializza displayCounter e rowCounter a 0
}


/// @brief Simple Function that manages the display with two separate columns of 6 rows plus the yellow first row at the top
/// @param message string to be displayed in the log columns
void DisplayOled::displayMessage (String message, Adafruit_SSD1306& display){
  Serial.println("Displaying message");
  Serial.println(message);
  Serial.println ("Display counter: ");
  Serial.println(displayCounter);
  Serial.println ("Row counter: ");
  Serial.println(rowCounter);
  
  if (displayCounter < 11){
    
    if(displayCounter >= 5){ //when we exceed the space on the first column move to the second
      display.setCursor(64, rowCounter);
      display.println(message);
      display.display();
      displayCounter++;
      rowCounter += 8;
    }
    else{
      display.println(message);
      display.display();
      displayCounter++;
    }
    
  }
  else {
    //when display is full reset it but printing again the address
    display.clearDisplay();
    display.setCursor(0, 0);
    // display.println(WiFi.localIP());
    display.setCursor(0, 8);
    // display.println(FirstRow);
    // display.println(message);
    display.display(); 
    displayCounter=0;
    rowCounter = 17;
  }
}

void DisplayOled::initDisplay(Adafruit_SSD1306& display){
    ////////////////////////OLED INITIALIZATION////////////////////////////
  Wire.begin(SDA_PIN, SCL_PIN); 

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
  {
    Serial.println(F("SSD1306 allocation failed")); 
  } 

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
    display.display();
    delay(100);
    display.clearDisplay();

    //show custom logo
    display.drawBitmap((128 - splash1_width) / 2, ((64 - splash1_height) / 2)+10,
               custom_logo, splash1_width, splash1_height, 1);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("LISA Dome Controller");
    display.display();
    delay(3000); // Pause for 3 seconds to allow serial connection
 }