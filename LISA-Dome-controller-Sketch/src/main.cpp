#include <WiFi.h>
#include <WebServer.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

WebServer server(80);

const char* ssid     = "";
const char* password = "";

#define LED_INTEGRATED 15
#define S1_PIN 3
#define S2_PIN 4
#define SCL_PIN 35
#define SDA_PIN 33
#define BTT_CW 16
#define BTT_CCW 18
#define BTT_RST 37
#define BTT_END 39 



#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C //Oled Screen Address for initialization
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //initialization of Oled Screen


int displayCounter = -1; //Used to reset display when full
int rowCounter = 17; //Used to print to the second column correctly

String currentData; //Used to hold the latest data from server
String FirstRow;  //used to hold the string that appears in the first display row

////////////////////////TEST ZONE//////////////////////////////////
struct Button {
	const uint8_t PIN;
	uint32_t numberKeyPresses;
	bool pressed;
  String message;
};

Button bttReset = {BTT_RST, 0, false,"RESET POSITION"};
Button bttClockwise = {BTT_CW, 0, false,"MANUAL CW"};
Button bttCounterClockwise = {BTT_CCW, 0, false,"MANUAL CCW"};

void IRAM_ATTR isrRST() {
 bttReset.pressed = true;
}
///////////////////////////////////////////////////////////////////

//Simple Function that manages the display with two separate columns of 6 rows plus the yellow first row at the top
void displayMessage (String message){
  
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
  pinMode(LED_BUILTIN, OUTPUT);      // set the LED pin mode
  pinMode (bttReset.PIN, INPUT_PULLUP);

  attachInterrupt (bttReset.PIN, isrRST, CHANGE);
  Wire.begin(SDA_PIN, SCL_PIN); 

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
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
    FirstRow = "Wifi connected";
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
  
  //Print address for the first time
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 8);
  display.println(FirstRow);
  display.display(); 
}

void loop()
{
  //server.handleClient();
  if  (bttReset.pressed) {
    bttReset.pressed = false;
    displayMessage(bttReset.message);
	}

}

