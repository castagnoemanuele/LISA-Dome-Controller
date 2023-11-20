#define ENCODER_1 13
#define ENCODER_2 12
#define ENCODER_3 14

#define LED_INT 2

#define S1 27
#define S2 26
int counter =0;
#define END_PIN 25

void setup() {
  Serial.begin(115200); 
  pinMode (S1, INPUT_PULLDOWN);
  pinMode (S2, INPUT_PULLDOWN);
  pinMode (ENCODER_1, OUTPUT);
  pinMode (ENCODER_2, OUTPUT);
  pinMode (ENCODER_3, OUTPUT);
  pinMode (LED_INT,OUTPUT);
  pinMode (END_PIN,OUTPUT);
}

void loop() {
  counter=0;
  while(digitalRead(S1) == HIGH){
    digitalWrite(ENCODER_1, HIGH);
    digitalWrite(LED_INT, HIGH);
    delay(50);
    digitalWrite(ENCODER_1, LOW);
    digitalWrite(LED_INT, LOW);

    delay(250);

    digitalWrite(ENCODER_2, HIGH);
    digitalWrite(LED_INT, HIGH);
    delay(50);
    digitalWrite(ENCODER_2, LOW);
    digitalWrite(LED_INT, LOW);

    delay(250);
    
    digitalWrite(ENCODER_3, HIGH);
    digitalWrite(LED_INT, HIGH);
    delay(50);
    digitalWrite(ENCODER_3, LOW);
    digitalWrite(LED_INT, LOW);

    delay(250);
    counter++;
    if(counter >= 30){
      digitalWrite(END_PIN, HIGH);
      delay(500);
      digitalWrite(END_PIN, LOW);
      Serial.println("End of run activated");
    }
  }
  while(digitalRead(S2) == HIGH){
    digitalWrite(ENCODER_3, HIGH);
    digitalWrite(LED_INT, HIGH);
    delay(50);
    digitalWrite(ENCODER_3, LOW);
    digitalWrite(LED_INT, LOW);

    delay(250);

    digitalWrite(ENCODER_2, HIGH);
    digitalWrite(LED_INT, HIGH);
    delay(50);
    digitalWrite(ENCODER_2, LOW);
    digitalWrite(LED_INT, LOW);

    delay(250);

    digitalWrite(ENCODER_1, HIGH);
    digitalWrite(LED_INT, HIGH);
    delay(50);
    digitalWrite(ENCODER_1, LOW);
    digitalWrite(LED_INT, LOW);

    delay(250);
    counter++;
    if(counter >= 30){
      digitalWrite(END_PIN, HIGH);
      delay(500);
      digitalWrite(END_PIN, LOW);
      Serial.println("End of run activated");
      
    }
  }


}
