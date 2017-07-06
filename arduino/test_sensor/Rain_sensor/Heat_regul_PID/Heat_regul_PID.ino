#include <PID_v1.h>

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
double Consigne = 55;
// La température mesurée :
double Temp;
// La valeur PWM 0-255 :
double Mosfet;

// Déclaration de l'objet PID
// Les argument sont les variables de gestion
// puis les gains pour P, I et D
// et enfin,le mode.
//Define the aggressive and conservative Tuning Parameters
double aggKp = 6, aggKi = 3, aggKd = 1;
double consKp = 1, consKi = 0.05, consKd = 0.25;

PID myPID(&Temp, &Mosfet, &Consigne, consKp, consKi, consKd, DIRECT);

// Valeurs de l'exemple fourni
//PID myPID(&Temp, &Mosfet, &Consigne, 2, 5, 1, DIRECT);
// Agressif
//PID myPID(&Temp, &Mosfet, &Consigne, 6, 3, 1, DIRECT);
// Moins agressif
// PID myPID(&Temp, &Mosfet, &Consigne, 1, 0.05, 0.25, DIRECT);

float getTemperature() {
  uint8_t i;
  float average;

  // take N samples in a row, with a slight delay
  for (i = 0; i < NUMSAMPLES; i++) {
    samples[i] = analogRead(THERMISTORPIN);
    //delay(10);
  }

  // average all the samples out
  average = 0;
  for (i = 0; i < NUMSAMPLES; i++) {
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

void setup(void) {
  // broche 11 en sortie
  pinMode(OUTP, OUTPUT);
  // activation port série
  Serial.begin(9600);
  // consigne à 40°C
  //Consigne = 40;
  // contrôle entre 0-255
  myPID.SetOutputLimits(0, 255);
  // on active la régulation PID
  myPID.SetMode(AUTOMATIC);

}

void loop(void) {
  // demande de mesure
  Temp = getTemperature();

  double gap = abs(Consigne - Temp); //distance away from setpoint

  if (gap < 2)
  { //we're close to setpoint, use conservative tuning parameters
    myPID.SetTunings(consKp, consKi, consKd);
  } else {
    //we're far from setpoint, use aggressive tuning parameters
    myPID.SetTunings(aggKp, aggKi, aggKd);
  }

  // calcul PID, la variable Mosfet contient
  // ensuite la valeur calculée
  myPID.Compute();
  // écriture PWM
  analogWrite(OUTP, Mosfet);

  SerialSend();
  // envoi des informations temp:PWM à Processing
}

void SerialSend()
{
  Serial.print("setpoint: "); Serial.print(Consigne); Serial.print(" ");
  Serial.print("input: "); Serial.print(Temp); Serial.print(" ");
  Serial.print("output: ");Serial.print(Mosfet/2); Serial.println(" ");
  //delay(100);

}



