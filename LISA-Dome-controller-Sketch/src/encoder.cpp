
#include "config.h"
#include "encoder.h"
#include <Preferences.h>


#include <esp32-hal-gpio.h>
#include "button.h"


/// @brief Handles encoder Data
/**
 * Checks the state of the encoder.
 *
 * @param encoder1 The Encoder object to be checked.
 */
void checkEncoder(Encoder& encoder1){
  if(encoder1.oldSector!=encoder1.newSector){
  encoder1.oldSector=encoder1.newSector;}
  if(digitalRead(ENCODER1)==HIGH){
    encoder1.newSector=1;
  }
  if(digitalRead(ENCODER2)==HIGH){
    encoder1.newSector=2;
  }
  if(digitalRead(ENCODER3)==HIGH){
    encoder1.newSector=3;
  }
  encoder1.hasChanged=false;

  /////////DETECT DIRECTION////////
  switch (encoder1.newSector){
    case 1:
    if (encoder1.oldSector==2){
      encoder1.direction=CCW;
    }
    if (encoder1.oldSector==1){
      encoder1.direction=CW;
    }
    break;
    case 2:
    if (encoder1.oldSector==3){
      encoder1.direction=CCW;
    }
    if (encoder1.oldSector==1){
      encoder1.direction=CW;
    }
    break;
    case 3:
    if (encoder1.oldSector==1){
      encoder1.direction=CCW;
    }
    if (encoder1.oldSector==2){
      encoder1.direction=CW;
    }
    break;
  }
}

/// @brief call this function to disable all controls and wait for the dome to return to the original position
/**
 * Resets the position of the dome.
 *
 * @param encoder1 The encoder object.
 * @param resetButton The button used for resetting the position.
 * @param limitSwitch The limit switch used for detecting the end position.
 */
void resetPosition(Encoder& encoder1, Button& resetButton, Button& limitSwitch){
  resetButton.pressed=false;
   while(!limitSwitch.pressed){
    //check if we need a clockwise or counterclockwise rotation to get to the limit switch faster
    if(encoder1.currentPosition<0){ 
      digitalWrite(S1_PIN,HIGH);
    } else if (encoder1.currentPosition>0){
      digitalWrite(S2_PIN,HIGH);
    }
    delay(100);
   }
   digitalWrite(S1_PIN,LOW);
   digitalWrite(S2_PIN,LOW);
   
   encoder1.currentPosition = 0;
}

/// @brief Counts how many Ticks happen to make a full rotation of the dome
/// @return number of ticks counted 
uint8_t countTicksFullRotation (Encoder& encoder1, Button& resetButton, Button& limitSwitch)
{
  resetPosition(encoder1, resetButton, limitSwitch);
  uint8_t ticks=0;


  return ticks;
}

/// @brief updates the value of the dome's position every 5 minutes, called by a timer
void saveData (float data, const char *address, Preferences& preferences){
  preferences.begin("LISA", false); 
  preferences.putFloat(address, data);
  Serial.print("New position saved to EPROM:");
  Serial.println(data);
  preferences.end();
}
