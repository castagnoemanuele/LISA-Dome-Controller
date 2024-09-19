
/**
 * @file displayOled.cpp
 * @brief Implementation of the DisplayOled class for managing the OLED display.
 * 
 * This file contains the implementation of the DisplayOled class, which is responsible for
 * managing the OLED display used in the LISA Dome Controller project. The class provides
 * methods for initializing the display, displaying messages, and updating the display with
 * the current WiFi status and telescope position.
 * 
 * @details
 * The DisplayOled class provides the following functionalities:
 * - Initialization of the OLED display.
 * - Displaying messages in a two-column format.
 * - Updating the display with the current WiFi status.
 * - Updating the display with the current position of the telescope and the target position.
 * 
 * The display is divided into two columns, each with 6 rows, plus a yellow first row at the top.
 * When the first column is full, the messages are displayed in the second column. When both
 * columns are full, the display is cleared and the messages start from the first column again.
 * 
 * The initDisplay method initializes the display and shows a custom logo, followed by the
 * WiFi status and the telescope position. The printWifiStatus method updates the first row
 * of the display with the current WiFi status, and the printPositionStatus method updates
 * the second row with the current position of the telescope and the target position.
 * 
 * @note This file is part of the LISA Dome Controller project.
 */

#include "displayOled.h"
#include <config.h>
#include <logo.h>
#include <Encoder.h>
#include "Telescope.h" 

DisplayOled::DisplayOled() : secondRow(0), rowCounter(0) {
    // Default constructor
}


/// @brief Simple Function that manages the display with two separate columns of 6 rows plus the yellow first row at the top
/// @param message string to be displayed in the log columns
void DisplayOled::displayMessage(String message, Adafruit_SSD1306& display) {
  display.setTextSize(1);
  display.setTextColor(WHITE);

  if (secondRow) { // when we exceed the space on the first column move to the second
    display.setCursor(64, rowCounter + 16);
    display.println(message);
    rowCounter += 8;
  } else {//print on the first column
    display.setCursor(0, rowCounter+16);
    display.println(message);
    rowCounter += 8;
    //check if we need to print on the second column
    if(rowCounter >= 48) {  
      secondRow = 1;
      rowCounter = 0;
    }
  }
   
  if (secondRow && rowCounter == 56) {
    //when the log part is full reset it
    display.fillRect(0,16,128,48,BLACK);
    //print the message just received in the firs column
    display.setCursor(0, 16);
    display.println(message);
    //
    rowCounter = 8;
    secondRow = false;
  }
  
  display.display();
}


/**
 * @brief Initializes the OLED display and shows the initial screen.
 * 
 * This method initializes the OLED display, shows a custom logo, and updates the display
 * with the current WiFi status and the telescope position. It sets up the I2C communication,
 * initializes the display with the Adafruit library, and displays the initial content.
 * 
 * @param display The Adafruit_SSD1306 display object. 
 * @param encoder The Encoder object used to get the current position of the telescope.
 * @param LISA The Telescope object used to get the target position of the telescope.
 * 
 * @note This method must be called before any other display-related methods to ensure
 * the display is properly initialized.
 */
void DisplayOled::initDisplay(Adafruit_SSD1306& display, Encoder& encoder, Telescope& LISA){
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
    display.clearDisplay();
    printWifiStatus(display);//print the wifi status in the first row
    printPositionStatus(display,encoder,LISA);//print the position status in the second row
    display.display();
    
 }
 /// @brief called to update the first row of the display with the current IP address (at the moment not used)
 /// @param display 
 void DisplayOled::printWifiStatus(Adafruit_SSD1306& display){
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("no Wifi");
    display.display();
 }

/// @brief Updates the second row of the display with the current position of the telescope and the target position
/// @param display the display object
/// @param encoder the encoder object
/// @param LISA the telescope object
void DisplayOled::printPositionStatus(Adafruit_SSD1306& display, Encoder& encoder, Telescope& LISA){
  display.fillRect(0, 7, 128, 9, WHITE);//to clear the previous row
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(1, 8);
  display.print("POS ");
  display.print(encoder.currentDegrees);
  display.drawCircle(46, 8, 1, BLACK); //symbol for degrees
  display.setCursor(60, 8);
  display.print("TARGET ");
  display.print(LISA.getTelescopePosition());//change to getTargetPosition
  display.drawCircle(120, 8, 1, BLACK);//symbol for degrees
  display.display();
}
    