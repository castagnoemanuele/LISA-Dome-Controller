/**
 * @file encoder.cpp
 * @brief This file contains the implementation of functions to handle the encoder data, update the position of the dome, reset the position, count ticks for a full rotation, convert ticks to degrees, and save data to EEPROM.
 * 
 * The functions included are:
 * - checkEncoder(Encoder& encoder1)
 * - updatePosition(Encoder& encoder1)
 * - resetPosition(Encoder& encoder1, Button& resetButton, Button& limitSwitch)
 * - countTicksFullRotation(Encoder& encoder1, Button& resetButton, Button& limitSwitch, Preferences& preferences)
 * - convertTicksToDegrees(int ticks, Encoder& encoder1)
 * - saveData(int data, const char *address, Preferences& preferences)
 * 
 * These functions are used to manage the state and position of the encoder, update the dome's position, reset the dome's position, count the number of ticks for a full rotation, convert ticks to degrees, and save data to EEPROM.
 * 
 * 
 */
#include "config.h"
#include "encoder.h"
#include <Preferences.h>


#include <esp32-hal-gpio.h>
#include "button.h"


/// @brief Handles encoder Data
/**
 * Checks the state of the encoder by comparing which sector is active relative to the sector that
 * was active in the previous iteration. This way it also detects the direction of rotation.
 * @param encoder1 The Encoder object to be checked.
 */
void checkEncoder(Encoder& encoder1){
  if(encoder1.oldSector!=encoder1.newSector){
  //save the old sector
  encoder1.oldSector=encoder1.newSector;}
  //find the new sector
  if(digitalRead(ENCODER1)==LOW){
    encoder1.newSector=1;
  }
  if(digitalRead(ENCODER2)==LOW){
    encoder1.newSector=2;
  }
  if(digitalRead(ENCODER3)==LOW){
    encoder1.newSector=3;
  }

  encoder1.hasChanged=false;

  /////////DETECT DIRECTION////////
  //we do this by comparing the old sector with the new sector
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

/// @brief Updates the position of the dome according to the rotation of the encoder
/// @param encoder1 the encoder object
/// @param resetButton the button object
/// @param limitSwitch the button object
void updatePosition(Encoder& encoder1){
  if(encoder1.newSector!=encoder1.oldSector){
      Serial.print(encoder1.oldSector);
      Serial.print("->");
      Serial.println(encoder1.newSector);
      /////////////UPDATE POSITION////////////////
      if (encoder1.direction==CW){
        encoder1.currentPosition++;
      }
      else if (encoder1.direction==CCW){
        encoder1.currentPosition--;
      }
      Serial.print("Current encoder position in ticks: ");
      Serial.print(encoder1.currentPosition);
      
      //Convert data to degrees 
      encoder1.currentDegrees = convertTicksToDegrees(encoder1.currentPosition, encoder1);

      Serial.print("  In degrees: ");
      Serial.println(encoder1.currentDegrees);
      Serial.println("");

      
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
  
  if (encoder1.currentPosition==0){
    Serial.println("Dome is already in position 0.");
    return;
  }
  
  Serial.println("Resetting position to 0.");
  
  while(!limitSwitch.pressed){
    //check if we need a clockwise or counterclockwise rotation to get to the limit switch faster
    if(encoder1.currentPosition<0){ 
      digitalWrite(S1_PIN,HIGH);
    } else if (encoder1.currentPosition>0){
      digitalWrite(S2_PIN,HIGH);
    }
    delay(100);
  }
  //when we reach position zero the motor gets shut off
  digitalWrite(S1_PIN,LOW);
  digitalWrite(S2_PIN,LOW);
  Serial.println("Dome has returned to position 0.");
  //Update current position information
  encoder1.currentPosition = 0;
  encoder1.currentDegrees = convertTicksToDegrees(encoder1.currentPosition, encoder1);

}

/// @brief Counts how many Ticks happen to make a full rotation of the dome and saves the data to the EEPROM
/// @return number of ticks counted 
long countTicksFullRotation (Encoder& encoder1, Button& resetButton, Button& limitSwitch, Preferences& preferences)
{
  //bring the dome to the starting position
  resetPosition(encoder1, resetButton, limitSwitch);
  long ticks=0;
  Serial.println("Starting rotation to count fullRot.");
  digitalWrite(S1_PIN,HIGH);//initiate rotation
  delay(1000); //wait for the dome to start rotating
  while (limitSwitch.pressed==false){
    //count how many updates happen to the encoder
    if(encoder1.hasChanged){
      checkEncoder(encoder1);
      if(encoder1.oldSector != encoder1.newSector){
        ticks++;
        Serial.println(ticks);
      }
      encoder1.hasChanged=false;
    }
  }
  //Turn off the motor after the dome has made a full rotation
  digitalWrite(S1_PIN,LOW);
  Serial.print("Ticks counted:");
  Serial.println(ticks);
  saveData(ticks, "fullRotation", preferences);
  
  return ticks;
}
/// @brief Convert a number of ticks with a porportion to the equivalent in degrees keepin
/// @param ticks number of ticks to be converted
/// @param encoder1 the encoder object, used to get the full rotation data
/// @return the number of tick converted to degrees, considering the full rotation data
int convertTicksToDegrees(int ticks, Encoder& encoder1){
  int degrees;
  if (encoder1.fullRotation==0){
    Serial.println("Full rotation data not found.");
    return 0;
  }
  if(ticks<0){
    return ((ticks*360)/(encoder1.fullRotation )) + 360;
  }
  return (ticks*360)/(encoder1.fullRotation);
}

/// @brief to save data in a certain EPROM addres, called by a timer interrupt or a function
/**
 * Saves data to the EEPROM in a specified location.
 *
 * @param data The data to be saved.
 * @param address The address in the EEPROM where the data will be saved.
 * @param preferences The preferences object.
 */
void saveData (int data, const char *address, Preferences& preferences){
  preferences.begin("LISA", false); 
  //may correct in future, but for now only two addresses are working
  const char *addressChar = "fullRotation";
  if (address == addressChar){
    preferences.putInt("fullRotation", data);
    Serial.println("Full rotation data saved.");
  }
  const char *addressChar2 = "currentPosition";
  if (address == addressChar2){
    preferences.putInt("currentPosition", data);
    Serial.println("Current position data saved.");
  }
 
  
  preferences.end();
}
