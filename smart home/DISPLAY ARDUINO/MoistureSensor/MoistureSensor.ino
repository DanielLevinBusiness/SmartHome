   //////////////////////////////////////////////
  //        ARDUINO SOIL MOISTURE DEMO        //
 //                                          //
//           http://www.educ8s.tv           //
/////////////////////////////////////////////


const int alarm = 4;
int sensorPin = A0;  
int sensorValue = 0;  
int percent = 0;

void setup() {
  Serial.begin(9600);
  pinMode(alarm, OUTPUT);
}

void loop() {
  digitalWrite(alarm, LOW); 
  sensorValue = analogRead(sensorPin);
  percent = convertToPercent(sensorValue);
  printValuesToSerial();
  if (percent > 60)
  digitalWrite(alarm, HIGH);
  delay(1000);
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
