int pinTemp = A1;   //This is where our Output data goes
const int hot = 2;
const int cold = 3;
void setup() {
  Serial.begin(9600);
  pinMode(hot, OUTPUT);
  pinMode(cold, OUTPUT);
  digitalWrite(hot, LOW);
  digitalWrite(cold, LOW);
}
void loop() {
  int temp = analogRead(pinTemp);    //Read the analog pin
  temp = temp * 0.48828125;   // convert output (mv) to readable celcius
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println("C");  //print the temperature status
  delay(500);
  if (temp > 26) 
    digitalWrite(hot, HIGH);
    else 
      digitalWrite(hot, LOW);
  if (temp < 8) 
    digitalWrite(cold, HIGH);
    else
    digitalWrite(cold, LOW);
}
