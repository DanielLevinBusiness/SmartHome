#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

const int moisalarm = 10;
int sensorPin = A0;
int sensorValue = 0;
int percent = 0;

int pinTemp = A1;   //This is where our Output data goes
const int hot = 2;
const int cold = 3;
double temp;
int sensorInput;

void setup() {
  Serial.begin(9600);
  pinMode(hot, OUTPUT);
  pinMode(cold, OUTPUT);
  digitalWrite(hot, LOW);
  digitalWrite(cold, LOW);
  Serial.begin(9600);
  pinMode(moisalarm, OUTPUT);
  // put your setup code here, to run once:
  lcd.init();                      // initialize the lcd
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(1, 0);

}

void loop() {
  sensorInput = analogRead(pinTemp);    //Read the analog pin
  temp = (double)sensorInput / 1024;   //find percentage of input reading
  temp = temp * 5;                     //multiply by 5V to get voltage
  temp = temp - 0.5;                   //Subtract the offset
  temp = temp * 100;    // convert output (mv) to readable celcius
  lcd.setCursor(1, 0);
  lcd.print("TEMP IS : ");
  lcd.print(temp);
  lcd.print((char)223);
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println("C");
  delay(5000);//print the temperature status
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
}


void printValuesToSerial()
{
  Serial.print("\n\nAnalog Value: ");
  Serial.print(sensorValue);
  Serial.print("\nPercent: ");
  Serial.print(percent);
  Serial.print("%");

}
