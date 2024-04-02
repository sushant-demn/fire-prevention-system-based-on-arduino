unsigned long prevTime = millis();
const int sensorIn = A3;
int mVperAmp = 185; // use 185 for 5A, 100 for 20A Module and 66 for 30A Module
double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;
int buzzst = 0; // buzzzer intitial state
const int current = A3;
const float max_current = 5.0;
// lowest and highest sensor readings:
const int sensorMin = 0;           //  sensor minimum
const int sensorMax = 1024;        // sensor maximum
#define MQ_PIN (0)                 // define which  analog input channel you are going to use
#define RL_VALUE (5)               // define the load resistance on the board, in kilo ohms
#define RO_CLEAN_AIR_FACTOR (9.83) // RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO,
                                   // which is derived from the  chart in datasheet

/**********************Software Related Macros***********************************/
#define CALIBARAION_SAMPLE_TIMES (50)     // define how many samples you are  going to take in the calibration phase
#define CALIBRATION_SAMPLE_INTERVAL (500) // define the time interal(in milisecond) between each samples in the
                                          // cablibration phase
#define READ_SAMPLE_INTERVAL (50)         // define how many samples you are  going to take in normal operation
#define READ_SAMPLE_TIMES (5)             // define the time interal(in milisecond) between each samples in

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
/*********************Application Related Macros*********************************/
#define GAS_LPG (0)
#define GAS_CO (1)
#define GAS_SMOKE (2)

/****************************Globals**********************************************/
float LPGCurve[3] = {2.3, 0.21, -0.47};   // two points are taken from the curve.
                                          // with these two points,  a line is formed which is "approximately equivalent"
                                          // to  the original curve.
                                          // data  format:{ x, y, slope}; point1: (lg200, 0.21), point2: (lg10000, -0.59)
float COCurve[3] = {2.3, 0.72, -0.34};    // two points are taken from the curve.
                                          // with these two points,  a line is formed which is "approximately equivalent"
                                          // to  the original curve.
                                          // data  format:{ x, y, slope}; point1: (lg200, 0.72), point2: (lg10000,  0.15)
float SmokeCurve[3] = {2.3, 0.53, -0.44}; // two points are taken from the curve.
                                          // with these two points,  a line is formed which is "approximately equivalent"
                                          // to  the original curve.
                                          // data  format:{ x, y, slope}; point1: (lg200, 0.53), point2: (lg10000,  -0.22)
float Ro = 10;                            // Ro is initialized to 10 kilo ohms

void setup()
{
  // initialize serial communication @ 9600 baud:
  Serial.begin(9600);

  pinMode(13, OUTPUT); // buzzer at digital pin 13
  pinMode(A3, INPUT);
  pinMode(12, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  digitalWrite(12, LOW);
  digitalWrite(11, LOW);
  digitalWrite(10, LOW);
  Serial.print("Calibrating...\
");
  Ro = MQCalibration(MQ_PIN); // Calibrating the sensor. Please  make sure the sensor is in clean air                                                   //when  you perform the calibration
  Serial.print("Calibration  is done...\
");
  Serial.print("Ro=");
  Serial.print(Ro);
  Serial.print("kohm");
  Serial.print("\
");
pinMode(8, OUTPUT);
digitalWrite(8, HIGH);


}
void loop()
{
  unsigned long current_Time = millis();
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
  // read the sensor on analog A0:
  int sensorReading = analogRead(A1);
  // map the sensor range (four options):
  // ex: 'long  int map(long int, long int, long int, long int, long int)'
  int range = map(sensorReading, sensorMin, sensorMax, 0, 3);
  if (current_Time - prevTime > 5000 && buzzst == 1 && range == 2)
  {
    buzzst = 0;
    digitalWrite(13, buzzst);
  }
  // serial com for esp8266
  if (range == 0 or range == 1)
  {

    Serial.print('!');
  };

  // range value:
  switch (range)
  {
  case 0: // A fire closer than 1.5 feet away.
    Serial.println("** Close  Fire **");
    bz200(current_Time, prevTime);
    break;
  case 1: // A fire between 1-3 feet away.
    Serial.println("**  Distant Fire **");
    bz1000(current_Time, prevTime);
    break;
  case 2: // No fire detected.
    Serial.println("No  Fire");
    break;
  }
  if (current_Time - prevTime > 1)
  {
    Serial.print("LPG:");
    Serial.print(MQGetGasPercentage(MQRead(MQ_PIN) / Ro, GAS_LPG));
    Serial.print("ppm");
    Serial.print("    ");
    Serial.print("CO:");
    Serial.print(MQGetGasPercentage(MQRead(MQ_PIN) / Ro, GAS_CO));
    Serial.print("ppm");
    Serial.print("    ");
    Serial.print("SMOKE:");
    Serial.print(MQGetGasPercentage(MQRead(MQ_PIN) / Ro, GAS_SMOKE));
    Serial.print("ppm");
    Serial.print("\n");
    Serial.print("\n");
  }
  if ((MQGetGasPercentage(MQRead(MQ_PIN) / Ro, GAS_LPG) or MQGetGasPercentage(MQRead(MQ_PIN) / Ro, GAS_CO) or MQGetGasPercentage(MQRead(MQ_PIN) / Ro, GAS_SMOKE)) >= 1)
  {
    bz200(current_Time, prevTime);
    Serial.print('@');
  }
     Voltage = getVPP();
 VRMS = (Voltage/2.0) *0.707; // sq root
 AmpsRMS = (VRMS * 1000)/mVperAmp;
 float Wattage = (220*AmpsRMS)-45.06; //Observed 18-20 Watt when no load was connected, so substracting offset value to get real consumption.
 Serial.print(AmpsRMS);
 Serial.println(" Amps RMS ");
 Serial.print(Wattage); 
 Serial.println(" Watt ");
 if (current_Time - prevTime >= 5000)
 {
  if (Wattage >= 35)
  {
    digitalWrite(12, HIGH);
    digitalWrite(11, HIGH);
    digitalWrite(10, HIGH);
  }
  
 }
 
}
float MQResistanceCalculation(int raw_adc)
{
  return (((float)RL_VALUE * (1023 - raw_adc) / raw_adc));
}
float MQCalibration(int mq_pin)
{
  int i;
  float val = 0;

  for (i = 0; i < CALIBARAION_SAMPLE_TIMES; i++)
  { // take multiple samples
    val += MQResistanceCalculation(analogRead(mq_pin));
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  val = val / CALIBARAION_SAMPLE_TIMES; // calculate the average  value

  val = val / RO_CLEAN_AIR_FACTOR; // divided  by RO_CLEAN_AIR_FACTOR yields the Ro
                                   // according  to the chart in the datasheet

  return val;
}
float MQRead(int mq_pin)
{
  int i;
  float rs = 0;

  for (i = 0; i < READ_SAMPLE_TIMES; i++)
  {
    rs += MQResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);
  }

  rs = rs / READ_SAMPLE_TIMES;

  return rs;
}
int MQGetGasPercentage(float rs_ro_ratio, int gas_id)
{
  if (gas_id == GAS_LPG)
  {
    return MQGetPercentage(rs_ro_ratio, LPGCurve);
  }
  else if (gas_id == GAS_CO)
  {
    return MQGetPercentage(rs_ro_ratio, COCurve);
  }
  else if (gas_id == GAS_SMOKE)
  {
    return MQGetPercentage(rs_ro_ratio, SmokeCurve);
  }

  return 0;
}
int MQGetPercentage(float rs_ro_ratio, float *pcurve)
{
  return (pow(10, (((log(rs_ro_ratio) - pcurve[1]) / pcurve[2]) + pcurve[0])));
}
void bz200(long ctime, long ptime)
{
  if (ctime - ptime > 200)
  {
    buzzst = !buzzst;
    digitalWrite(13, buzzst);
  }
}
void bz1000(long ctime, long ptime)
{
  if (ctime - ptime > 1000)
  {
    buzzst = !buzzst;
    digitalWrite(13, buzzst);
  }
}
float getVPP()
{
  float result;
  
  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1024;          // store min value here
  
   uint32_t start_time = millis();

   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = analogRead(sensorIn);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           /*record the maximum sensor value*/
           minValue = readValue;
       }
    }
   
   // Subtract min from max
   result = ((maxValue - minValue) * 5)/1024.0;
      
   return result;
 }
