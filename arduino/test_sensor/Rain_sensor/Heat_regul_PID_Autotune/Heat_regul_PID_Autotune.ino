#include <PID_v1.h>
#include <PID_AutoTune_v0.h>


// which analog pin to connect
#define THERMISTORPIN A3       
// resistance at 25 degrees C
#define THERMISTORNOMINAL 100000     
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 25
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 4120
// the value of the 'other' resistor
#define SERIESRESISTOR 92900
// On contrôle avec la broche 11
#define OUTP 11

//Variable pour echantillonnage
int samples[NUMSAMPLES];

// Variables pour PID
// La température souhaitée :
double Consigne;
// La température mesurée :
double Temp;
// La valeur PWM 0-255 :
double Mosfet;

byte ATuneModeRemember=2;
double kp=2,ki=0.5,kd=2;

double kpmodel=1.5, taup=100, theta[50];
double outputStart=5;
double aTuneStep=50, aTuneNoise=1, aTuneStartValue=200;
unsigned int aTuneLookBack=20;

boolean tuning = true;
unsigned long  modelTime, serialTime;

PID myPID(&Temp, &Mosfet, &Consigne, kp,ki,kd, DIRECT);
PID_ATune aTune(&Temp, &Mosfet);

//set to false to connect to the real world
boolean useSimulation = false;

float getTemperature() {
      uint8_t i;
      float average;
     
      // take N samples in a row, with a slight delay
      for (i=0; i< NUMSAMPLES; i++) {
       samples[i] = analogRead(THERMISTORPIN);
       //delay(10);
      }
     
      // average all the samples out
      average = 0;
      for (i=0; i< NUMSAMPLES; i++) {
         average += samples[i];
      }
      average /= NUMSAMPLES;
     
      // convert the value to resistance
      average = 1023 / average - 1;
      average = SERIESRESISTOR / average;
     
      float steinhart;
      steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
      steinhart = log(steinhart);                  // ln(R/Ro)
      steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
      steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
      steinhart = 1.0 / steinhart;                 // Invert
      steinhart -= 273.15;                         // convert to C
     
      return steinhart;
}

void setup()
{
  if(useSimulation)
  {
    for(byte i=0;i<50;i++)
    {
      theta[i]=outputStart;
    }
    modelTime = 0;
  }
  //Setup the pid 
  myPID.SetMode(AUTOMATIC);

  if(tuning)
  {
    tuning=false;
    changeAutoTune();
    tuning=true;
  }
  
  serialTime = 0;
  Serial.begin(115200);

    // broche 11 en sortie
  pinMode(OUTP, OUTPUT);
  // consigne à 40°C
  Consigne = 50;
  // contrôle entre 0-255
  myPID.SetOutputLimits(0, 255);
 
  analogReference(EXTERNAL);

}

void loop()
{

  unsigned long now = millis();

  if(!useSimulation)
  { //pull the input in from the real world
    //input = analogRead(0);
    // demande de mesure
    Temp = getTemperature();
  }
  
  if(tuning)
  {
    byte val = (aTune.Runtime());
    if (val!=0)
    {
      tuning = false;
    }
    if(!tuning)
    { //we're done, set the tuning parameters
      kp = aTune.GetKp();
      ki = aTune.GetKi();
      kd = aTune.GetKd();
      myPID.SetTunings(kp,ki,kd);
      AutoTuneHelper(false);
    }
  }
  else myPID.Compute();
  
  if(useSimulation)
  {
    theta[30]=Mosfet;
    if(now>=modelTime)
    {
      modelTime +=100; 
      DoModel();
    }
  }
  else
  {
     //analogWrite(0,output);
     analogWrite(OUTP, Mosfet);
  }
  
  //send-receive with processing if it's time
  if(millis()>serialTime)
  {
    SerialReceive();
    SerialSend();
    serialTime+=500;
  }
}

void changeAutoTune()
{
 if(!tuning)
  {
    //Set the output to the desired starting frequency.
    Mosfet=aTuneStartValue;
    aTune.SetNoiseBand(aTuneNoise);
    aTune.SetOutputStep(aTuneStep);
    aTune.SetLookbackSec((int)aTuneLookBack);
    AutoTuneHelper(true);
    tuning = true;
  }
  else
  { //cancel autotune
    aTune.Cancel();
    tuning = false;
    AutoTuneHelper(false);
  }
}

void AutoTuneHelper(boolean start)
{
  if(start)
    ATuneModeRemember = myPID.GetMode();
  else
    myPID.SetMode(ATuneModeRemember);
}


void SerialSend()
{
  Serial.print("setpoint: ");Serial.print(Consigne); Serial.print(" ");
  Serial.print("input: ");Serial.print(Temp); Serial.print(" ");
  Serial.print("output: ");Serial.print(Mosfet); Serial.print(" ");
  if(tuning){
    Serial.println("tuning mode");
  } else {
    Serial.print("kp: ");Serial.print(myPID.GetKp());Serial.print(" ");
    Serial.print("ki: ");Serial.print(myPID.GetKi());Serial.print(" ");
    Serial.print("kd: ");Serial.print(myPID.GetKd());Serial.println();
  }
}

void SerialReceive()
{
  if(Serial.available())
  {
   char b = Serial.read(); 
   Serial.flush(); 
   if((b=='1' && !tuning) || (b!='1' && tuning))changeAutoTune();
  }
}

void DoModel()
{
  //cycle the dead time
  for(byte i=0;i<49;i++)
  {
    theta[i] = theta[i+1];
  }
  //compute the input
  //Temp = (kpmodel / taup) *(theta[0]-outputStart) + Temp*(1-1/taup) + ((float)random(-10,10))/100;
  Temp = getTemperature();

}


