#include "Keypad.h"
#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 53
#define RST_PIN 5
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
byte rowPins[ROWS] = {43, 44, 45, 46};
byte colPins[COLS] = {47, 48, 49};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
char KEY[4] = {'1', '1', '1', '1'}; // default secret key
char attempt[4] = {0, 0, 0, 0};
int z = 0;
const int trigPin = 30;
const int echoPin = 32;
const int alarm = 34;
const int secalarm = 36;
long duration;
int distance;
int turnoff;



void setup()
{
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  pinMode(opendoor, OUTPUT);
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(alarm, OUTPUT);
  pinMode(secalarm, OUTPUT);
  Serial.begin(9600);

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
  int i;
  for ( i = 0; i < 4 ; i++ )
  {
    if (attempt[i] == KEY[i])
    {
      correct++;
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
        z++;
    }
  }
}

void activalarm()
{
  digitalWrite(alarm, HIGH);
  digitalWrite(secalarm, HIGH);
  readKeypad();
}

void loop()
{
  if (turnoff != 1) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(10);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;
    if (distance < 70)
      activalarm();  
  }
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
  String content= "";
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
    delay(3000);
    
  }
 
 else   {
    Serial.println(" Access denied");
    delay(3000);
    
  }
}
