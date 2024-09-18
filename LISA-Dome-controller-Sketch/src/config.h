
/**
 * @file config.h
 * @brief Configuration file for LISA Dome Controller.
 *
 * This file contains the pin definitions and configuration settings for the LISA Dome Controller.
 * It includes pin assignments for LEDs, buttons, encoders, I2C communication, and the OLED display.
 
 */

#define LED_INTEGRATED 15 ///LED pin
#define S1_PIN 11 ///Selector pin 1 for the relays
#define S2_PIN 12 ///Selector pin 2 for the relays
#define SCL_PIN 39 ///I2C SCL pin
#define SDA_PIN 37 ///I2C SDA pin
#define BTT_CW 18 ///Clockwise button pin
#define BTT_CCW 16 ///Counter Clockwise button pin
#define BTT_RST 33 ///Reset button pin
#define BTT_END 35 ///Limit switch button pin
#define ENCODER1 10 ///Encoder pin
#define ENCODER2 3 ///Encoder pin
#define ENCODER3 5 ///Encoder pin
#define TX_PIN 14 ///Telescope TX pin
#define RX_PIN 13 ///Telescope RX pin


#define SCREEN_WIDTH 128 /// OLED display width, in pixels
#define SCREEN_HEIGHT 64 /// OLED display height, in pixels
#define OLED_RESET -1 /// Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///Oled Screen Address for initialization



