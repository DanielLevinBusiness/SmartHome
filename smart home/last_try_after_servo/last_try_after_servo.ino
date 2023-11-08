#include "Keypad.h"
int a=0;
#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 53
#define RST_PIN 5
#include <Wire.h>
#include <Servo.h>
Servo servo_test;
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
const byte ROWS = 4; // four rows
const byte COLS = 3; // three columns
char keys[ROWS][COLS] =
{
  {'1', '2', '3' },
  {'4', '5', '6' },
  {'7', '8', '9' },
  {'*', '0', '#' }
};
byte rowPins[ROWS] = {46, 47, 48, 49};
byte colPins[COLS] = {43, 44, 45};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
char KEY[4] = {'1', '1', '1', '1'};
char TURNONKEY[4] = {'5', '5', '5', '5'};
char attempt[4] = {0, 0, 0, 0};
int z = 0;
const int trigPin = 30;
const int echoPin = 32;
const int alarm = 34;
const int secalarm = 36;
long duration;
int distance;
int turnoff;
const int moisalarm = 10;
int sensorPin = A0;
int sensorValue = 0;
int percent = 0;
int pinTemp = A4;
const int hot = 2;
const int cold = 3;
int inputPin = 7;
int pirState = LOW;
int val = 0;
int angle = 70;
int dooropen = 0;


void setup()
{
  Serial.begin(9600);
  servo_test.attach(9);
  pinMode(inputPin, INPUT);
  pinMode(hot, OUTPUT);
  pinMode(cold, OUTPUT);
  digitalWrite(hot, LOW);
  digitalWrite(cold, LOW);
  pinMode(moisalarm, OUTPUT);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(1, 0);
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(alarm, OUTPUT);
  pinMode(secalarm, OUTPUT);
  servo_test.write(70);
  turnoff = 0;
  
}
void correctKEY()
{
  Serial.println(" KEY ACCEPTED...");
  digitalWrite(alarm, LOW);
  digitalWrite(secalarm, LOW);
  turnoff = 1;
}
void incorrectKEY()
{
  Serial.println("KEY REJECTED!");
}
void checkKEY()
{
  int correct = 0;
  int alarmon = 0;
  int i;
  for ( i = 0; i < 4 ; i++ )
  {
    if (attempt[i] == KEY[i])
    {
      correct++;
    }
    if (attempt[i] == TURNONKEY[i])
    {
      alarmon++;
    }
  }
  if (correct == 4)
  {
    correctKEY();
  }
  else
  {
    if (alarmon == 4) {
      turnoff = 0;
      Serial.println("Alarm ON");
    }
    else
      incorrectKEY();
  }
  for (int zz = 0; zz < 4; zz++) // clear previous key input
  {
    attempt[zz] = 0;
  }
}
void readKeypad()
{
  char key = keypad.getKey();
  if (key != NO_KEY) {
    for (int i = 0; i < 4; i++) {
      char key = keypad.getKey();
      if (key != NO_KEY) {
        Serial.println(key);
        attempt[i] = key;
      } else {
        i--;
      }
    }
    checkKEY();
  }
}
int convertToPercent(int value)
{
  int percentValue = 0;
  percentValue = map(value, 1023, 465, 0, 100);
  return percentValue;
}
void printValuesToSerial()
{
  Serial.print("\n\nAnalog Value: ");
  Serial.print(sensorValue);
  Serial.print("\nPercent: ");
  Serial.print(percent);
  Serial.print("%");
}
void security()
{
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "1B BA CE 83") //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Authorized access");
    dooropen = 1;
  }
  else   {
    Serial.println(" Access denied");
  }
}
void door(int dooropen) {

  if (dooropen == 1 && a==0) {
    a=1;
    for (angle = 70; angle > 0; angle -= 1)
    {
      servo_test.write(angle);
      delay(15);
    }
    delay(5000);
    for (angle = 0; angle <= 70; angle += 5)
    {
      servo_test.write(angle);              //command to rotate the servo to the specified angle
      delay(15);
    }
    dooropen = 0;
  }
}
void loop()
{
  readKeypad();
  door(dooropen);
  if (turnoff != 1) {
    security();
    val = digitalRead(inputPin);  // read input value
    if (val == HIGH) {            // check if the input is HIGH
      // turn LED ON
      if (pirState == LOW) {
        // we have just turned on
        Serial.println("Motion detected!");
        digitalWrite(alarm, HIGH);
        digitalWrite(secalarm, HIGH);
        // We only want to print on the output change, not state
        pirState = HIGH;
      }

    } else {
      // turn LED OFF
      if (pirState == HIGH) {
        // we have just turned of
        Serial.println("Motion ended!");
        // We only want to print on the output change, not state
        pirState = LOW;
      }
    }
    digitalWrite(trigPin, LOW);
    delayMicroseconds(10);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;
    if (distance < 5) {
      digitalWrite(alarm, HIGH);
      digitalWrite(secalarm, HIGH);
    }
  }
  else {
    int temp = analogRead(pinTemp);
    temp = temp * 0.48828125;
    lcd.setCursor(1, 0);
    lcd.print("TEMP IS : ");
    lcd.print(temp);
    lcd.print((char)223);
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println("C");  //print the temperature status
    if (temp > 26)
      digitalWrite(hot, HIGH);
    else
      digitalWrite(hot, LOW);
    if (temp < 8)
      digitalWrite(cold, HIGH);
    else
      digitalWrite(cold, LOW);
    percent = map(analogRead(sensorPin), 0, 1023, 100, 0);
    printValuesToSerial();
    lcd.setCursor(1, 1);
    lcd.print("MOIST IS : ");
    lcd.print(percent);
    lcd.print("%");
    if (percent > 60)
      digitalWrite(moisalarm, HIGH);
    else
      digitalWrite(moisalarm, LOW);


    // Look for new cards

  }
}
