// lowest and highest sensor readings:
const int sensorMin = 0;    //  sensor minimum
const int sensorMax = 1024; // sensor maximum

void setup()
{
  // initialize serial communication @ 9600 baud:
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
    char fire = '@';
}
void loop()
{
  // read the sensor on analog A0:
  int sensorReading = analogRead(A1);
  // map the sensor range (four options):
  // ex: 'long  int map(long int, long int, long int, long int, long int)'
  int range = map(sensorReading, sensorMin, sensorMax, 0, 3);
  char fire = '@';

  // range value:
  switch (range)
  {
  case 0: // A fire closer than 1.5 feet away. 
    Serial.println("** Close  Fire **");
    digitalWrite(LED_BUILTIN, HIGH); // turn the BUZZER on (HIGH is the voltage level)
    Serial.print(fire);
    delay(200);                      // wait for a second
    digitalWrite(LED_BUILTIN, LOW);  // turn the BUZZER off by making the voltage LOW
    delay(200);                      // wait for a second
    break;
  case 1: // A fire between 1-3 feet away.
    Serial.println("**  Distant Fire **");
    digitalWrite(LED_BUILTIN, HIGH); // turn the BUZZER on (HIGH is the voltage level)
    Serial.print(fire);
    delay(1000);                     // wait for a second
    digitalWrite(LED_BUILTIN, LOW);  // turn the BUZZER off by making the voltage LOW
    delay(1000);                     // wait for a second
    break;
  case 2: // No fire detected.
    Serial.println("No  Fire");
    break;
  }
  delay(1); // delay between reads
}