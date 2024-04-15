
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

/// @brief Updates the position of the dome according to the rotation of the encoder
/// @param encoder1 the encoder object
/// @param resetButton the button object
/// @param limitSwitch the button object
void updatePosition(Encoder& encoder1, Button& resetButton, Button& limitSwitch){
  if(encoder1.newSector!=encoder1.oldSector){
      Serial.println(encoder1.newSector);
      /////////////UPDATE POSITION////////////////
      if (encoder1.direction==CW){
        encoder1.currentPosition++;
      }
      else if (encoder1.direction==CCW){
        encoder1.currentPosition--;
      }
      Serial.println(encoder1.currentPosition);
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

/// @brief Counts how many Ticks happen to make a full rotation of the dome and saves the data to the EEPROM
/// @return number of ticks counted 
uint8_t countTicksFullRotation (Encoder& encoder1, Button& resetButton, Button& limitSwitch, Preferences& preferences)
{
  //bring the dome to the starting position
  resetPosition(encoder1, resetButton, limitSwitch);
  uint8_t ticks=0;
  digitalWrite(S1_PIN,HIGH);//initiate rotation
  delay(500); //wait for the dome to start rotating
  while (limitSwitch.pressed==false){
    //count how many updates happen to the encoder
    if(encoder1.hasChanged){
      ticks++;
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

int convertTicksToDegrees(int ticks, Encoder& encoder1){
  int degrees;
  if(ticks<0){
    return -(ticks*360)/encoder1.fullRotation + 360;
  }
  return (ticks*360)/encoder1.fullRotation;
}

/// @brief updates the value of the dome's position every 5 minutes, called by a timer
void saveData (float data, const char *address, Preferences& preferences){
  preferences.begin("LISA", false); 
  preferences.putFloat(address, data);
  Serial.print("New position saved to EPROM:");
  Serial.println(data);
  preferences.end();
}
