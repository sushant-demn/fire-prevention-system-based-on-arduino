// lowest and highest sensor readings:
const int sensorMin = 0;    //  sensor minimum
const int sensorMax = 1024; // sensor maximum

#define         MQ_PIN                       (0)     //define which  analog input channel you are going to use
#define         RL_VALUE                     (5)     //define the load resistance on the board, in kilo ohms
#define         RO_CLEAN_AIR_FACTOR          (9.83)  //RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO,
                                                     //which is derived from the  chart in datasheet
 
/**********************Software Related Macros***********************************/
#define         CALIBARAION_SAMPLE_TIMES     (50)    //define how many samples you are  going to take in the calibration phase
#define         CALIBRATION_SAMPLE_INTERVAL  (500)   //define the time interal(in milisecond) between each samples in the
                                                     //cablibration phase
#define         READ_SAMPLE_INTERVAL         (50)    //define how many samples you are  going to take in normal operation
#define         READ_SAMPLE_TIMES            (5)     //define the time interal(in milisecond) between each samples in 

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7  = 2;
// LiquidCrystal lcd(rs, en, d4, d5, d6, d7);                                                     //normal  operation
 
/*********************Application Related Macros*********************************/
#define         GAS_LPG                      (0)
#define         GAS_CO                       (1)
#define         GAS_SMOKE                    (2)
 
/****************************Globals**********************************************/
float           LPGCurve[3]  =  {2.3,0.21,-0.47};   //two points are taken from the curve.  
                                                    //with these two points,  a line is formed which is "approximately equivalent"
                                                    //to  the original curve. 
                                                    //data  format:{ x, y, slope}; point1: (lg200, 0.21), point2: (lg10000, -0.59) 
float           COCurve[3]  =  {2.3,0.72,-0.34};    //two points are taken from the curve.  
                                                    //with these two points,  a line is formed which is "approximately equivalent" 
                                                    //to  the original curve.
                                                    //data  format:{ x, y, slope}; point1: (lg200, 0.72), point2: (lg10000,  0.15) 
float           SmokeCurve[3] ={2.3,0.53,-0.44};    //two points are taken from the curve.  
                                                    //with these two points,  a line is formed which is "approximately equivalent" 
                                                    //to  the original curve.
                                                    //data  format:{ x, y, slope}; point1: (lg200, 0.53), point2: (lg10000,  -0.22)                                                     
float           Ro           =  10;                 //Ro is initialized to 10 kilo ohms

void setup()
{
  // initialize serial communication @ 9600 baud:
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
    Serial.print("Calibrating...\
");                
  Ro = MQCalibration(MQ_PIN);                       //Calibrating the sensor. Please  make sure the sensor is in clean air                                                   //when  you perform the calibration                    
  Serial.print("Calibration  is done...\
"); 
  Serial.print("Ro=");
  Serial.print(Ro);
  Serial.print("kohm");
  Serial.print("\
");
}
void loop()
{
  // read the sensor on analog A0:
  int sensorReading = analogRead(A1);
  // map the sensor range (four options):
  // ex: 'long  int map(long int, long int, long int, long int, long int)'
  int range = map(sensorReading, sensorMin, sensorMax, 0, 3);
  // range value:
  switch (range)
  {
  case 0: // A fire closer than 1.5 feet away.
    Serial.println("** Close  Fire **");
    digitalWrite(LED_BUILTIN, HIGH); // turn the BUZZER on (HIGH is the voltage level)
    delay(200);                      // wait for a second
    digitalWrite(LED_BUILTIN, LOW);  // turn the BUZZER off by making the voltage LOW
    delay(200);                      // wait for a second
    break;
  case 1: // A fire between 1-3 feet away.
    Serial.println("**  Distant Fire **");
    digitalWrite(LED_BUILTIN, HIGH); // turn the BUZZER on (HIGH is the voltage level)
    delay(1000);                     // wait for a second
    digitalWrite(LED_BUILTIN, LOW);  // turn the BUZZER off by making the voltage LOW
    delay(1000);                     // wait for a second
    break;
  case 2: // No fire detected.
    Serial.println("No  Fire");
    break;
  }
  delay(1); // delay between reads
     Serial.print("LPG:"); 
   Serial.print(MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_LPG)  );
   Serial.print( "ppm" );
   Serial.print("    ");   
   Serial.print("CO:");  
   Serial.print(MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_CO) );
   Serial.print(  "ppm" );
   Serial.print("    ");   
   Serial.print("SMOKE:"); 
   Serial.print(MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_SMOKE) );
   Serial.print(  "ppm" );
   Serial.print("\n");
   Serial.print("\
");
if ((MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_LPG) or MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_CO) or MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_SMOKE))>=1)
{
    digitalWrite(LED_BUILTIN, HIGH); // turn the BUZZER on (HIGH is the voltage level)
    delay(200);                      // wait for a second
    digitalWrite(LED_BUILTIN, LOW);  // turn the BUZZER off by making the voltage LOW
    delay(200); 
}
}
float MQResistanceCalculation(int raw_adc)
{
  return ( ((float)RL_VALUE*(1023-raw_adc)/raw_adc));
}
float MQCalibration(int mq_pin)
{
  int i;
  float val=0;
 
  for (i=0;i<CALIBARAION_SAMPLE_TIMES;i++) {            //take multiple samples
    val += MQResistanceCalculation(analogRead(mq_pin));
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  val = val/CALIBARAION_SAMPLE_TIMES;                   //calculate the average  value
 
  val = val/RO_CLEAN_AIR_FACTOR;                        //divided  by RO_CLEAN_AIR_FACTOR yields the Ro 
                                                        //according  to the chart in the datasheet 
 
  return val; 
}
float MQRead(int mq_pin)
{
  int i;
  float rs=0;
 
  for (i=0;i<READ_SAMPLE_TIMES;i++)  {
    rs += MQResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);
  }
 
  rs = rs/READ_SAMPLE_TIMES;
 
  return rs;  
}
int MQGetGasPercentage(float rs_ro_ratio, int gas_id)
{
  if ( gas_id  == GAS_LPG ) {
     return MQGetPercentage(rs_ro_ratio,LPGCurve);
  } else  if ( gas_id == GAS_CO ) {
     return MQGetPercentage(rs_ro_ratio,COCurve);
  } else if ( gas_id == GAS_SMOKE ) {
     return MQGetPercentage(rs_ro_ratio,SmokeCurve);
  }    
 
  return 0;
}
int  MQGetPercentage(float rs_ro_ratio, float *pcurve)
{
  return (pow(10,(  ((log(rs_ro_ratio)-pcurve[1])/pcurve[2]) + pcurve[0])));
}

 
