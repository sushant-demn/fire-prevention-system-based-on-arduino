#define BLYNK_TEMPLATE_ID "TMPL3_DUbj0Wq"
#define BLYNK_TEMPLATE_NAME "Fire safety"
#define BLYNK_AUTH_TOKEN "9DyJ1y4fcaFn1DhQN0CJUpVW_V_vQ3UC"
#define BLYNK_PRINT Serial
// #define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SPI.h>
// #include <BlynkEdgent.h>

char ssid[] = "Airtel_Sus";
char pass[] = "sus@0011";
BLYNK_WRITE(V2)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  // Serial.print("V1 Slider value is: ");
  Serial.println(pinValue);
}
BLYNK_WRITE(V3)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  // Serial.print("V1 Slider value is: ");
  Serial.println(pinValue);
}
BLYNK_WRITE(V4)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  // Serial.print("V1 Slider value is: ");
  Serial.println(pinValue);
}

void setup()
{
  Blynk.virtualWrite(V0, 1);
  Blynk.virtualWrite(V1, 1);
  Blynk.virtualWrite(V2, 0);
  Blynk.virtualWrite(V3, 2);
  Blynk.virtualWrite(V4, 4);
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}
void loop()
{
  Blynk.run();
  if(Serial.available()>=1){
    char a = Serial.read();
    if(a == '!'){
      Blynk.logEvent("fire", "fire detected");
      Blynk.virtualWrite(V0, 1);
      delay(15000); 
    }}
  if(Serial.available()>=1){
    char a = Serial.read();
  if(a == '@'){
      Blynk.virtualWrite(V1, 1);
      Blynk.logEvent("smoke2", "smoke detected");
      delay(15000); 
    }
  }

  delay(1000);
}

