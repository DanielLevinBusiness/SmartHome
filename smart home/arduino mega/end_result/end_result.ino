#include "Keypad.h"
#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 53
#define RST_PIN 5
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
const int opendoor = 4;
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
char KEY[4] = {'1', '1', '1', '1'}; // default secret key
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
int pinTemp = A1;   //This is where our Output data goes
const int hot = 2;
const int cold = 3;                // choose the pin for the LED
int inputPin = 7;               // choose the input pin (for PIR sensor)
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status

void setup()
{
  Serial.begin(9600);      // declare LED as output
  pinMode(inputPin, INPUT);     // declare sensor as input
  pinMode(hot, OUTPUT);
  pinMode(cold, OUTPUT);
  digitalWrite(hot, LOW);
  digitalWrite(cold, LOW);
  pinMode(moisalarm, OUTPUT);
  // put your setup code here, to run once:
  lcd.init();                      // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(1, 0);  // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  pinMode(opendoor, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(alarm, OUTPUT);
  pinMode(secalarm, OUTPUT);

}


void correctKEY() // do this if the correct KEY is entered
{
  Serial.println(" KEY ACCEPTED...");
  digitalWrite(alarm, LOW);
  digitalWrite(secalarm, LOW);
  turnoff = 1;
}
void incorrectKEY() // do this if an incorrect KEY is entered
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
    incorrectKEY();
  }
  if (alarmon == 4)
  {
    turnoff = 0;
  }
  for (int zz = 0; zz < 4; zz++) // clear previous key input
  {
    attempt[zz] = 0;
  }
}
void readKeypad()
{
  char key = keypad.getKey();
  if (key != NO_KEY)
  {
    switch (key)
    {
      case '*':
        z = 0;
        break;
      case '#':
        delay(100); // added debounce
        checkKEY();
        break;
      default:
        attempt[z] = key;
        Serial.println(key);
        z++;
    }
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
void loop()
{
  readKeypad();
  if (turnoff != 1) {
    val = digitalRead(inputPin);  // read input value
    if (val == HIGH) {            // check if the input is HIGH
      digitalWrite(alarm, HIGH);
      digitalWrite(secalarm, HIGH);  // turn LED ON
      if (pirState == LOW) {
        // we have just turned on
        Serial.println("Motion detected!");
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
    if (distance < 0) {
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
    digitalWrite(moisalarm, LOW);
    sensorValue = analogRead(sensorPin);
    percent = ( 100 - ( (sensorValue / 1023.00) * 100 ));
    printValuesToSerial();
    lcd.setCursor(1, 1);
    lcd.print("MOIST IS : ");
    lcd.print(percent);
    lcd.print("%");
    if (percent > 60)
      digitalWrite(moisalarm, HIGH);
    digitalWrite(opendoor, LOW);
    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent())
    {
      return;
    }
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
    {
      return;
    }
    //Show UID on serial monitor
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
      Serial.println();
      digitalWrite(opendoor, HIGH);


    }

    else   {
      Serial.println(" Access denied");
      delay(3000);
    }

  }
}
