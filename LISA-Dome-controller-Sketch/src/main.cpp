#include <WiFi.h>
#include <WebServer.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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





String currentData; //Used to hold the latest data from server
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

/// @brief Interrupt Routine handler, sets .pressed attribute to the reading
/// @return 
void IRAM_ATTR isr() {
  Serial.println("enter isr");
 bttReset.pressed = digitalRead(BTT_RST);
 bttClockwise.pressed = digitalRead(BTT_CW);
 bttCounterClockwise.pressed = digitalRead(BTT_CCW);
 limitSwitch.pressed = digitalRead(BTT_END);
}


/// @brief Simple Function that manages the display with two separate columns of 6 rows plus the yellow first row at the top
/// @param message 
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
    FirstRow = "connected";
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

  attachInterrupt(digitalPinToInterrupt(ENCODER1), isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER2), isr, CHANGE);
}

void loop()
{
  static float currentPosition; //used to hold dome's position in degrees relative to the end of run
  
  
  //when the reset button is pressed, the motor starts spinning clockwise untile the limit switch stops it
  if  (bttReset.pressed) {
    displayMessage(bttReset.message);
   // bttReset.pressed=false;
    while(!limitSwitch.pressed){
      digitalWrite(S1_PIN,HIGH);
      delay(100);
    }
    digitalWrite(S1_PIN,LOW);
    displayMessage ("END of RUN");
    currentPosition = 0;
	}


  if(bttClockwise.pressed){
    displayMessage(bttClockwise.message);
    while (bttClockwise.pressed)
    {
      digitalWrite(S1_PIN, HIGH);
      delay(100);

    }
    digitalWrite(S1_PIN, LOW);
    delay(100);
  }

  if(bttCounterClockwise.pressed){
    displayMessage(bttCounterClockwise.message);
    while (bttCounterClockwise.pressed)
    {
      digitalWrite(S2_PIN, HIGH);
      delay(100);
    }
    digitalWrite(S2_PIN, LOW);
    delay(100);
  }
  /////////////////////////////////////////////////////////////
   
  //server.handleClient(); for future remote control features
}
