void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  digitalWrite(12, LOW);
  digitalWrite(11, LOW);
  digitalWrite(10, LOW);
}

void loop() {

  if (Serial.available() >= 1) {
    char a = Serial.read();
    if (a == '1') {
      digitalWrite(12, LOW);
    }
    if (a == '0') {
      digitalWrite(12, HIGH);
    }
    if (a == '2') {
      digitalWrite(11, HIGH);
    }
    if (a == '3') {
      digitalWrite(11, LOW);
    }
    if (a == '4') {
      digitalWrite(10, HIGH);
    }
    if (a == '5') {
      digitalWrite(10, LOW);
    }
  }
}
