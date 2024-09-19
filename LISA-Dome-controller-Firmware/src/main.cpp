#include <WiFi.h>
#include <WebServer.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <encoder.h>
#include <config.h>
#include <WiFiManager.h>
#include <telescope.h>
#include <button.h>
#include <Preferences.h>
#include <displayOled.h>


Preferences preferences;
WebServer server(80);

Telescope LISA;
Encoder encoder = {0,0,false,0,0,0,UNKNOWN};


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //initialization of Oled Screen
DisplayOled displayOled;


Button bttReset = {BTT_RST, 0, false,"RESET"};///onboard button to reset the dome's position
Button bttExternalReset = {BTT_EXT_RST, 0, false,"RESET"}; ///external button to reset the dome's position
Button bttClockwise = {BTT_CW, 0, false,"MANUAL CW"};
Button bttCounterClockwise = {BTT_CCW, 0, false,"MANUAL CCW"};
Button limitSwitch = {BTT_END, 0, false, "END REACHED"};


bool needUpdate = false; //flag to update the display only when the button is pressed
static bool commandsPrinted = false; //bool for user interface commands

/// @brief Interrupt Routine handler, sets .pressed attribute according to the reading
/// @return 
void IRAM_ATTR bttClick() {
  bttReset.pressed = digitalRead(BTT_RST);
  bttExternalReset.pressed = digitalRead(BTT_EXT_RST);
  bttClockwise.pressed = digitalRead(BTT_CW);
  bttCounterClockwise.pressed = digitalRead(BTT_CCW);
  limitSwitch.pressed = !digitalRead(BTT_END); //opposite because it is active when low
  needUpdate= true;
}


void IRAM_ATTR encoderTick() {
  encoder.hasChanged=true;
}


bool dataSaveNecessary=false;
bool telescopeCheckNecessary=false;
hw_timer_t *timerEPROM = NULL;
hw_timer_t *timerTelescope = NULL;

/// @brief Interrupt Routine handler, Checks the timer to save the position periodically
void IRAM_ATTR onTimerEPROM(){
  dataSaveNecessary=true;
}
/// @brief Interrupt Routine handler, Checks the timer to check the telescope position periodically
void IRAM_ATTR onTimerTelescope(){
  telescopeCheckNecessary=true;
}


void setup()
{
  ////////////////////////SERIAL COMMUNICATION INIT/////////////////////
  Serial.begin(9600); 
  Serial1.begin(9600,SERIAL_8N1,TX_PIN,RX_PIN); ///serial port to communicate with telescope
  /////////////////BUTTON/PIN INITIALIZATION////////////////////////////
  pinMode(LED_BUILTIN, OUTPUT);      // set the LED pin mode
  pinMode (bttReset.PIN, INPUT_PULLDOWN);
  pinMode (bttExternalReset.PIN, INPUT_PULLDOWN);
  pinMode (bttClockwise.PIN, INPUT_PULLDOWN);
  pinMode (bttCounterClockwise.PIN, INPUT_PULLDOWN);
  pinMode (limitSwitch.PIN, INPUT_PULLUP);
  pinMode (S1_PIN, OUTPUT);
  pinMode (S2_PIN,OUTPUT);
  digitalWrite(S1_PIN, LOW);
  digitalWrite(S2_PIN, LOW);
  
  attachInterrupt (limitSwitch.PIN, bttClick, FALLING);
  attachInterrupt (bttReset.PIN, bttClick, CHANGE);
  attachInterrupt (bttClockwise.PIN, bttClick, CHANGE);
  attachInterrupt (bttCounterClockwise.PIN, bttClick, CHANGE);
  attachInterrupt (bttExternalReset.PIN, bttClick, CHANGE);
  /////////////////////////ENCODER PINS INITIALIZATION////////////////////////////////
  pinMode(ENCODER1, INPUT_PULLUP);
  pinMode(ENCODER2, INPUT_PULLUP);
  pinMode(ENCODER3, INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(ENCODER1), encoderTick, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENCODER2), encoderTick, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENCODER3), encoderTick, FALLING);

  ////////////////////////OLED INITIALIZATION//////////////////////////
  displayOled.initDisplay(display, encoder, LISA);

    ////////////////////////////WIFI INITIALIZATION//////////////////////////
    // start by connecting to a WiFi network
    //WiFiManager wifiManager; //REMOVED AFTER INITIAL SETUP
    //wifiManager.resetSettings(); //TO CLEAN SETTINGS
    //wifiManager.autoConnect("AutoConnectAP");
    

    ///Wait for Connection while also printing on the display
    // for (int i = 0; i<12;i++)
    // {
    //   if (WiFi.status() != WL_CONNECTED) {
    //     delay(500);
    //     Serial.print(".");
    //     displayMessage("......");
    //   }
    // }

  if (WiFi.status() == WL_CONNECTED) 
  {
    //Blink 3 times to signal wifi connection
    for (int i=0; i<3; i++){
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
       digitalWrite(LED_BUILTIN, LOW);
      delay(100);
    }
    
    
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
  }
  else{displayOled.FirstRow="no Wifi";}
  
  ////////////////////////ENCODER STATUS SETUP//////////////////////////////
  preferences.begin("LISA", true); //start preferences in readonly mode
  encoder.currentPosition = preferences.getInt("currentPosition", 0); //read values from EPROM and set them to the encoder variables
  encoder.fullRotation= preferences.getInt("fullRotation",0);
  //check if we have any data on the fullRotation
  if(encoder.fullRotation==0){
    Serial.println("No full rotation data found, counting now");
    displayOled.displayMessage("COUNT TICKS",display);
    countTicksFullRotation(encoder,bttReset,limitSwitch,preferences);
  }
  Serial.print("Current position: ");
  Serial.println(encoder.currentPosition);
  encoder.currentDegrees = convertTicksToDegrees(encoder.currentPosition, encoder);
  Serial.print("     In degrees: ");
  Serial.println(encoder.currentDegrees);
  Serial.print("Number of encoder clicks for a full rotation: ");
  Serial.println(encoder.fullRotation);
  preferences.end();

  displayOled.printPositionStatus(display, encoder, LISA); //refresh position with the new data from the EPROM

  ////////////////////////////TIMER FOR SAVING DATA/////////////////////////////
  timerEPROM = timerBegin(0, 8000, true);
  timerAttachInterrupt(timerEPROM, &onTimerEPROM, true);
  timerAlarmWrite(timerEPROM, 3000000, true); //setting a timer to save encoder data to EPROM every 5 minutes
  timerAlarmEnable(timerEPROM); //Just Enable the timer

  //////////////////////////TIMER FOR TELESCOPE CHECK//////////////////////////
  timerTelescope = timerBegin(1, 8000, true);
  timerAttachInterrupt(timerTelescope, &onTimerTelescope, true);
  timerAlarmWrite(timerTelescope, 100000, true); //setting a timer to check the telescope position every 10 seconds
  //timerAlarmEnable(timerTelescope); //Just Enable the timer

  

}

void loop()
{
  if (limitSwitch.pressed)
  {
    if(encoder.currentPosition!=0){
      Serial.println("End of run reached");
    }
    encoder.currentPosition = 0;
    encoder.currentDegrees = 0;
  }
  ////////////////////////////MANUAL CONTROL///////////////////////////
  //when the reset button is pressed, the motor starts spinning clockwise until the limit switch stops it
  if (bttReset.pressed || bttExternalReset.pressed)
  {
    displayOled.displayMessage(bttReset.message,display);
    resetPosition(encoder, bttReset, limitSwitch);
    displayOled.displayMessage("END of RUN",display);
    displayOled.printPositionStatus(display, encoder, LISA);
  }
  //turn on relays according to the button status
  digitalWrite(S1_PIN, bttClockwise.pressed);
  digitalWrite(LED_BUILTIN, bttClockwise.pressed);

  if (bttClockwise.pressed & needUpdate)
  {
    displayOled.displayMessage(bttClockwise.message,display);
    needUpdate = false;
  }
  digitalWrite(S2_PIN, bttCounterClockwise.pressed);
  digitalWrite(LED_BUILTIN, bttCounterClockwise.pressed);
  if (bttCounterClockwise.pressed & needUpdate)
  {
    displayOled.displayMessage(bttCounterClockwise.message,display);
    needUpdate = false;
  }
  ///////////////////////UPDATE POSITION//////////////////////////////
  if (encoder.hasChanged)
  {
    //when movement is detected, we first check the direction, and then update the position
    checkEncoder(encoder);
    encoder.hasChanged = false;
    updatePosition(encoder);
    displayOled.printPositionStatus(display, encoder, LISA);
  }
  //server.handleClient(); for future remote control features
  //////////////////DATA SAVE//////////////////////////////
  if (dataSaveNecessary)
  {
    saveData(encoder.currentPosition, "currentPosition", preferences); //save the position to the EEPROM every 5 minutes
    dataSaveNecessary = false;
    displayOled.displayMessage("POS SAVE",display);
  }
  ///////////////////////TELESCOPE CHECK////////////////////////
  if (telescopeCheckNecessary)
  {
    displayOled.displayMessage("TEL CHECK",display);
    //LISA.checkTelescopePosition();
    Serial.println("Checking telescope position");
    telescopeCheckNecessary = false;
  }
  //////////////////////SERIAL COMM TEST/////////////////////////////
  // while (Serial1.available() > 0) {
  //   char inByte = Serial1.read();
  //   Serial.write(inByte);
  //   Serial1.write(inByte);
  //   displayMessage(String(inByte));
  // }



  
  ////////////////////////////Serial terminal interface to test functions////////////////////////
  if (!commandsPrinted) {
    Serial.println("\nAvailable commands:");
    Serial.println("p: check telescope position");
    Serial.println("r: reset position");
    Serial.println("c: count ticks for a full rotation");
    Serial.println("s: save data to EEPROM");
    Serial.println("t: print position data on serial");
    Serial.println("z: force update of position data on display");
    Serial.println("w: display message test");
    Serial.println("g: save full rotation data to EEPROM");
    commandsPrinted = true;
  }
  if (Serial.available() > 0)
  {
    char inByte = Serial.read();
    if (inByte == 'p')
    {
      LISA.checkTelescopePosition();
      displayOled.printPositionStatus(display, encoder, LISA);
    }
    if(inByte == 'r')
    {
      resetPosition(encoder, bttReset, limitSwitch);
    }
    if (inByte == 'c')
    {
      countTicksFullRotation(encoder, bttReset, limitSwitch, preferences);
    }
    if (inByte == 's')//force save data to EEPROM
    {
      saveData(encoder.currentPosition, "currentPosition", preferences);
    }
    if (inByte == 't')//Print all position data
    {
      Serial.println("Encoder current position: ");
      Serial.println(encoder.currentPosition);
      Serial.println("Encoder Current degrees: ");
      Serial.println(encoder.currentDegrees);
      Serial.println("Encoder full rotation ticks: ");
      Serial.println(encoder.fullRotation);

      Serial.println("Telescope position: ");
      Serial.println(LISA.getTelescopePosition());
    }
    if (inByte == 'z')
    {
      Serial.println("Updating display position");
      displayOled.printPositionStatus(display, encoder, LISA);
    }
    if(inByte == 'w')
    {
      displayOled.displayMessage("test",display);
      displayOled.displayMessage("COUNT TICKS",display);
    }
    if (inByte== 'g'){
      saveData(360,"fullRotation",preferences);
    }
    else
    {
      Serial.println("Command not recognized");
      commandsPrinted = false;
    }
  }
}