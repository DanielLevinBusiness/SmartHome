const int trigPin = 30;
const int echoPin = 32;
const int alarm = 34;
const int secalarm = 36;
long duration;
int distance;
void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(alarm, OUTPUT);
  pinMode(secalarm, OUTPUT);
  Serial.begin(9600);
}
void loop() {
  digitalWrite(alarm, LOW);
  digitalWrite(secalarm, LOW);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(10);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  if (distance < 70) {
    tone(alarm, 1000);
    tone(secalarm, 1000);
    delay(1000);
    noTone(alarm);
    noTone(secalarm);
    delay(500);
  }
}
