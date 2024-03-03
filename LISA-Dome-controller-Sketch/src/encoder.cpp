#include <config.h>
#include "encoder.h"
#include <esp32-hal-gpio.h>


/// @brief Handles encoder Data
void checkEncoder(Encoder encoder1){
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