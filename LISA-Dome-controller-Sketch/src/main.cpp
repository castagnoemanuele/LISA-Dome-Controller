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


Preferences preferences;
WebServer server(80);

Telescope LISA;
Encoder encoder = {0,0,false,0,0,0,UNKNOWN};

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //initialization of Oled Screen

String currentData; //Used to hold the l needUpdate data from server
String FirstRow;  //used to hold the string that appears in the first display row




Button bttReset = {BTT_RST, 0, false,"RESET"};
Button bttClockwise = {BTT_CW, 0, false,"MANUAL CW"};
Button bttCounterClockwise = {BTT_CCW, 0, false,"MANUAL CCW"};
Button limitSwitch = {BTT_END, 0, false, "END REACHED"};


bool needUpdate = false;
/// @brief Interrupt Routine handler, sets .pressed attribute according to the reading
/// @return 
void IRAM_ATTR bttClick() {
 bttReset.pressed = digitalRead(BTT_RST);
 bttClockwise.pressed = digitalRead(BTT_CW);
 bttCounterClockwise.pressed = digitalRead(BTT_CCW);
 limitSwitch.pressed = digitalRead(BTT_END);
 needUpdate= true;
}


void IRAM_ATTR encoderTick() {
  encoder.hasChanged=true;
}


bool dataSaveNecessary=false;
hw_timer_t *My_timer = NULL;

/// @brief Interrupt Routine handler, Checks the timer to save the position periodically
/// @return 
void IRAM_ATTR onTimer(){
  dataSaveNecessary=true;
}






/// @brief Simple Function that manages the display with two separate columns of 6 rows plus the yellow first row at the top
/// @param message string to be displayed in the log columns
void displayMessage (String message){
  static int displayCounter = -1; //Used to reset display when full
  static int rowCounter = 17; //Used to print to the second column correctly
  
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
    display.println(WiFi.localIP());
    display.setCursor(0, 8);
    display.println(FirstRow);
    display.println(message);
    display.display(); 
    displayCounter=0;
    rowCounter = 17;
  }
}
void handleRoot() {
  server.send(200, "text/plain", "Ready");
}
void handleGet() {
  if (server.hasArg("data")) {
    String data = server.arg("data");

      if (data.toInt()!=0)
      {
        displayMessage (data);
        int degrees = data.toInt();
      }
      Serial.println("Data: " + data);
    

    //if the data has changed in comparison to the one before and it is a string
    if (currentData!=data){
      currentData = data;
      if (data.toInt()==0)
      {
        displayMessage(data);
      }
    } 
    server.send(200, "text/plain", "Data Received");
  }
}
void handlePost() {
  server.send(200, "text/plain", "Processing Data");
}
void handleUpload() {
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    Serial.println("Receiving data:");
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    Serial.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    server.send(200, "text/plain", "Data: ");
  }
}


void setup()
{
  ////////////////////////SERIAL COMMUNICATION INIT/////////////////////
  Serial.begin(9600); 
  delay(3000);
  Serial1.begin(9600,SERIAL_8N1,TX_PIN,RX_PIN); ///serial port to communicate with telescope
  /////////////////BUTTON/PIN INITIALIZATION////////////////////////////
  pinMode(LED_BUILTIN, OUTPUT);      // set the LED pin mode
  pinMode (bttReset.PIN, INPUT_PULLDOWN);
  pinMode (bttClockwise.PIN, INPUT_PULLDOWN);
  pinMode (bttCounterClockwise.PIN, INPUT_PULLDOWN);
  pinMode (limitSwitch.PIN, INPUT_PULLUP);
  pinMode (S1_PIN, OUTPUT);
  pinMode (S2_PIN,OUTPUT);
  
  attachInterrupt (limitSwitch.PIN, bttClick, CHANGE);
  attachInterrupt (bttReset.PIN, bttClick, CHANGE);
  attachInterrupt (bttClockwise.PIN, bttClick, CHANGE);
  attachInterrupt (bttCounterClockwise.PIN, bttClick, CHANGE);
  ///////////////////SCREEN INITIALIZATION/////////////////////////////////
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
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    // display.println("Connecting to:");
    // display.println(ssid);
    display.display();

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
    
    FirstRow = "Connected! ";
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    server.on("/", handleRoot);
    server.on("/get", HTTP_GET, handleGet);
    server.on("/post", HTTP_POST, handlePost, handleUpload);
    server.begin();
  }
  else{FirstRow="no Wifi";}
  
  ///Print address for the first time
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 8);
  display.println(FirstRow);
  display.print(WiFi.localIP());
  display.display(); 

  /////////////////////////ENCODER PINS INITIALIZATION////////////////////////////////
  pinMode(ENCODER1, INPUT_PULLDOWN);
  pinMode(ENCODER2, INPUT_PULLDOWN);
  pinMode(ENCODER3, INPUT_PULLDOWN); 
  attachInterrupt(digitalPinToInterrupt(ENCODER1), encoderTick, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER2), encoderTick, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER3), encoderTick, RISING);

  ////////////////////////ENCODER STATUS SETUP//////////////////////////////
  preferences.begin("LISA", true); //start preferences in readonly mode
  encoder.currentPosition = preferences.getInt("currentPosition", 0); //read values from EPROM and set them to the encoder variables
  encoder.fullRotation= preferences.getInt("fullRotation",0);
  //check if we have any data on the fullRotation
  if(encoder.fullRotation==0){
    Serial.println("No full rotation data found, counting now");
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

  ////////////////////////////TIMER FOR SAVING DATA/////////////////////////////
  My_timer = timerBegin(0, 8000, true);
  timerAttachInterrupt(My_timer, &onTimer, true);
  timerAlarmWrite(My_timer, 3000000, true); //setting a timer to save encoder data to EPROM every 5 minutes
  timerAlarmEnable(My_timer); //Just Enable the timer
}

void loop()
{
  if (limitSwitch.pressed)
  {
    encoder.currentPosition = 0;
  }
  //when the reset button is pressed, the motor starts spinning clockwise until the limit switch stops it
  if (bttReset.pressed)
  {
    displayMessage(bttReset.message);
    resetPosition(encoder, bttReset, limitSwitch);
    displayMessage("END of RUN");
  }
  //turn on relays according to the button status
  digitalWrite(S1_PIN, bttClockwise.pressed);
  digitalWrite(LED_BUILTIN, bttClockwise.pressed);

  if (bttClockwise.pressed & needUpdate)
  {
    displayMessage(bttClockwise.message);
    needUpdate = false;
  }
  digitalWrite(S2_PIN, bttCounterClockwise.pressed);
  digitalWrite(LED_BUILTIN, bttCounterClockwise.pressed);
  if (bttCounterClockwise.pressed & needUpdate)
  {
    displayMessage(bttCounterClockwise.message);
    needUpdate = false;
  }

  ///////////////////////UPDATE POSITION//////////////////////////////
  if (encoder.hasChanged)
  {
    //when movement is detected, we first check the direction, and then update the position
    checkEncoder(encoder);
    Serial.println("checkEncoder called");
    encoder.hasChanged = false;
    updatePosition(encoder, bttReset, limitSwitch);
  }

  //server.handleClient(); for future remote control features

  //////////////////DATA SAVE//////////////////////////////
  if (dataSaveNecessary)
  {
    saveData(encoder.currentPosition, "currentPosition", preferences); //save the position to the EEPROM every 5 minutes
    dataSaveNecessary = false;
    displayMessage("POS SAVE");
  }
  //////////////////////SERIAL COMM TEST/////////////////////////////
  // while (Serial1.available() > 0) {
  //   char inByte = Serial1.read();
  //   Serial.write(inByte);
  //   Serial1.write(inByte);
  //   displayMessage(String(inByte));
  // }
  ////////////////////////////Serial terminal interface to test functions////////////////////////
  if (Serial.available() > 0)
  {
    char inByte = Serial.read();
    if (inByte == 'p')
    {
      LISA.checkTelescopePosition();
    }
    if(inByte == 'r')
    {
      resetPosition(encoder, bttReset, limitSwitch);
    }
    if (inByte == 'c')
    {
      countTicksFullRotation(encoder, bttReset, limitSwitch, preferences);
    }
    if (inByte == 's')
    {
      saveData(encoder.currentPosition, "currentPosition", preferences);
    }
   
  }
}