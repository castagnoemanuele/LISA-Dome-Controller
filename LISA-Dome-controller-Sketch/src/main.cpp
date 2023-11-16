#include <WiFi.h>
#include <WebServer.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>

Preferences preferences;
WebServer server(80);

const char* ssid     = "NETGEAR-Guest";
const char* password = "saibersechiure";

#define LED_INTEGRATED 15
#define S1_PIN 11
#define S2_PIN 12
#define SCL_PIN 39
#define SDA_PIN 37
#define BTT_CW 18
#define BTT_CCW 16
#define BTT_RST 33
#define BTT_END 35 
#define ENCODER1 10
#define ENCODER2 3
#define ENCODER3 5
#define TX 14
#define RX 13


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C //Oled Screen Address for initialization
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //initialization of Oled Screen





String currentData; //Used to hold the l needUpdate data from server
String FirstRow;  //used to hold the string that appears in the first display row



struct Button {
	const uint8_t PIN;
	uint32_t numberKeyPresses;
	bool pressed;
  String message;
};

Button bttReset = {BTT_RST, 0, false,"RESET"};
Button bttClockwise = {BTT_CW, 0, false,"MANUAL CW"};
Button bttCounterClockwise = {BTT_CCW, 0, false,"MANUAL CCW"};
Button limitSwitch = {BTT_END, 0, false, "END REACHED"};

bool needUpdate = false;
/// @brief Interrupt Routine handler, sets .pressed attribute to the reading
/// @return 
void IRAM_ATTR isr() {
 bttReset.pressed = digitalRead(BTT_RST);
 bttClockwise.pressed = digitalRead(BTT_CW);
 bttCounterClockwise.pressed = digitalRead(BTT_CCW);
 limitSwitch.pressed = digitalRead(BTT_END);
 needUpdate= true;
}

enum Direction {
  UNKNOWN,
  CW,
  CCW,
};

struct Encoder {
	int currentPosition;
  bool hasChanged;
  int fullRotation; ///number of encoder rotation corresponding to a full rotation of the dome
  int oldSector;
  int newSector;

  Direction direction;
  //bool status[3];
};

Encoder encoder = {0,false,0,0,UNKNOWN};

void IRAM_ATTR isr2() {
  encoder.hasChanged=true;
}

bool dataSaveNecessary=false;
hw_timer_t *My_timer = NULL;
void IRAM_ATTR onTimer(){
  dataSaveNecessary=true;
}

/// @brief updates the value of the dome's position every 5 minutes, called by a timer
void saveData (){
  preferences.begin("LISA", false); 
  encoder.currentPosition = preferences.putFloat("position", encoder.currentPosition);
  Serial.print("New position saved to EPROM:");
  Serial.print(encoder.currentPosition);
  preferences.end();
}
/// @brief call this function to disable all controls and wait for the dome to return to the original position
void resetPosition(){
   // bttReset.pressed=false;
    while(!limitSwitch.pressed){
      //check if we need a clockwise or counterclockwise rotation to get to the limit switch faster
      if(encoder.currentPosition<0){ 
        digitalWrite(S1_PIN,HIGH);
      } else if (encoder.currentPosition>0){
        digitalWrite(S2_PIN,HIGH);
      }
      delay(100);
    }
    digitalWrite(S1_PIN,LOW);
    digitalWrite(S2_PIN,LOW);
    
    encoder.currentPosition = 0;
}

/// @brief Handles encoder Data
void checkEncoder(){
  if(encoder.oldSector!=encoder.newSector){
  encoder.oldSector=encoder.newSector;}
  if(digitalRead(ENCODER1)==HIGH){
    encoder.newSector=1;
  }
  if(digitalRead(ENCODER2)==HIGH){
    encoder.newSector=2;
  }
  if(digitalRead(ENCODER3)==HIGH){
    encoder.newSector=3;
  }
  encoder.hasChanged=false;

  /////////DETECT DIRECTION////////
  switch (encoder.newSector){
    case 1:
    if (encoder.oldSector==2){
      encoder.direction=CCW;
    }
    if (encoder.oldSector==1){
      encoder.direction=CW;
    }
    break;
    case 2:
    if (encoder.oldSector==3){
      encoder.direction=CCW;
    }
    if (encoder.oldSector==1){
      encoder.direction=CW;
    }
    break;
    case 3:
    if (encoder.oldSector==1){
      encoder.direction=CCW;
    }
    if (encoder.oldSector==2){
      encoder.direction=CW;
    }
    break;
  }

  
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
  Serial.begin(115200); 
  /////////////////BUTTON/PIN INITIALIZATION////////////////////////////
  pinMode(LED_BUILTIN, OUTPUT);      // set the LED pin mode
  pinMode (bttReset.PIN, INPUT_PULLDOWN);
  pinMode (bttClockwise.PIN, INPUT_PULLDOWN);
  pinMode (bttCounterClockwise.PIN, INPUT_PULLDOWN);
  pinMode (limitSwitch.PIN, INPUT_PULLUP);
  pinMode (S1_PIN, OUTPUT);
  pinMode (S2_PIN,OUTPUT);
  
  attachInterrupt (limitSwitch.PIN, isr, CHANGE);
  attachInterrupt (bttReset.PIN, isr, CHANGE);
  attachInterrupt (bttClockwise.PIN, isr, CHANGE);
  attachInterrupt (bttCounterClockwise.PIN, isr, CHANGE);
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
    display.println("Connecting to:");
    display.println(ssid);
    display.display();

    ////////////////////////////WIFI INITIALIZATION//////////////////////////
    // start by connecting to a WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    for (int i = 0; i<12;i++)
    {
      if (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        displayMessage("......");
      }
    }
    
  if (WiFi.status() == WL_CONNECTED) 
  {
    //Blink 3 times to signal wifi connection
    for (int i=0; i<3; i++){
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
       digitalWrite(LED_BUILTIN, LOW);
      delay(100);
    }
    
    FirstRow = "Connected" + (String)WiFi.localIP();
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
  display.display(); 

  /////////////////////////ENCODER INITIALIZATION////////////////////////////////
  pinMode(ENCODER1, INPUT_PULLDOWN);
  pinMode(ENCODER2, INPUT_PULLDOWN);
  pinMode(ENCODER3, INPUT_PULLDOWN); 
  attachInterrupt(digitalPinToInterrupt(ENCODER1), isr2, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER2), isr2, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER3), isr2, RISING);

  ////////////////////////READ OLD VALUE FROM EPROM//////////////////////////////
  preferences.begin("LISA", true); //start preferences in readonly mode
  encoder.currentPosition = preferences.getFloat("position", 0);
  encoder.fullRotation= preferences.getInt("fullRotation",0);
  Serial.print("Current position: ");
  Serial.println(encoder.currentPosition);
  Serial.print("Number of encoder clicks for a full rotation: ");
  Serial.println(encoder.fullRotation);
  preferences.end();

  ////////////////////////////TIMER FOR SAVING DATA/////////////////////////////
  My_timer = timerBegin(0, 8000, true);
  timerAttachInterrupt(My_timer, &onTimer, true);
  timerAlarmWrite(My_timer, 3000000, true); //setting a timer to save encoder data to EPROM every 5 minutes
  timerAlarmEnable(My_timer); //Just Enable

}

void loop()
{
  if(limitSwitch.pressed){
    encoder.currentPosition=0;
  }
  //when the reset button is pressed, the motor starts spinning clockwise untile the limit switch stops it
  if  (bttReset.pressed) {
    displayMessage(bttReset.message);
    resetPosition();
    displayMessage ("END of RUN");
	}

  digitalWrite(S1_PIN, bttClockwise.pressed);
  digitalWrite(LED_BUILTIN, bttClockwise.pressed);

  if(bttClockwise.pressed & needUpdate){
    displayMessage(bttClockwise.message);
   needUpdate = false;
  }

  digitalWrite(S2_PIN, bttCounterClockwise.pressed);
  digitalWrite(LED_BUILTIN, bttCounterClockwise.pressed);
  if(bttCounterClockwise.pressed & needUpdate){
    displayMessage(bttCounterClockwise.message);
    needUpdate = false;
  }
  
  /////////////////////////////////////////////////////////////
   if(encoder.hasChanged){
    checkEncoder();
    if(encoder.newSector!=encoder.oldSector){
    displayMessage((String)encoder.oldSector);
    displayMessage("         ");
    FirstRow=(String)encoder.currentPosition;
    //////////////UPDATE POSITION////////////////
    if (encoder.direction==CW){
      encoder.currentPosition++;
    }
    else if (encoder.direction==CCW){
      encoder.currentPosition--;
    }
      Serial.println(encoder.currentPosition);
    }
   }
  //server.handleClient(); for future remote control features
  ////////////////DATA SAVE//////////////////////////////
  if(dataSaveNecessary){
    saveData();
    dataSaveNecessary=false;
    displayMessage("POS SAVE");
  }
  
}
