// which analog pin to connect
#define THERMISTORPIN A3       
// resistance at 25 degrees C
#define THERMISTORNOMINAL 100000     
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 100
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 4120
// the value of the 'other' resistor
#define SERIESRESISTOR 92900
// On contrôle avec la broche 11
#define OUTP 11
// on définit les valeurs basses et hautes des consignes de régul 
#define LOW_TARGET 10
#define HIGH_TARGET 200
     
int samples[NUMSAMPLES], start;

float getTemperature() {
    uint8_t i;
    float average;

    // take N samples in a row, with a slight delay
    for (i = 0; i < NUMSAMPLES; i++) {
    samples[i] = analogRead(THERMISTORPIN);
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
  Serial.begin(9600);
  analogWrite(OUTP, LOW_TARGET);
  start = millis();
}
 
void loop(void) {

  Serial.print(millis());
  Serial.print(";");
  Serial.print(getTemperature());
  Serial.print(";");
  Serial.println(LOW_TARGET);
  
  if (millis() - start > 60000) {
    while(1) {
      analogWrite(OUTP, HIGH_TARGET);
      Serial.print(millis());
      Serial.print(";");
      Serial.print(getTemperature());
      Serial.print(";");
      Serial.println(HIGH_TARGET);
    }
  }
  
  delay(100);
}
