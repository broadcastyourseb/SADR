const int OUT_PIN = A2;
const int IN_PIN = A1;

//Capacitance between IN_PIN and Ground
//Stray capacitance value will vary from board to board.

//Calibrate this value using known capacitor.
const float IN_STRAY_CAP_TO_GND = 31.00;
const float IN_CAP_TO_GND  = IN_STRAY_CAP_TO_GND;

const int MAX_ADC_VALUE = 1023;

void setup()

{
  pinMode(OUT_PIN, OUTPUT);
  //digitalWrite(OUT_PIN, LOW);  //This is the default state for outputs
  pinMode(IN_PIN, OUTPUT);
  //digitalWrite(IN_PIN, LOW);

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  
}

bool rain(int integration) {
  //Capacitor under test between OUT_PIN and IN_PIN

    float moyenne = 0, capacitance = 0;
  
    for (int i = 0; i < integration; i++) {
      
      //Rising high edge on OUT_PIN
      pinMode(IN_PIN, INPUT);
      digitalWrite(OUT_PIN, HIGH);
      int val = analogRead(IN_PIN);
      digitalWrite(OUT_PIN, LOW);
  
      //Low value capacitor
      //Clear everything for next measurement
      pinMode(IN_PIN, OUTPUT);
  
      //Calculate and print result
      capacitance = (float)val * IN_CAP_TO_GND / (float)(MAX_ADC_VALUE - val);
      moyenne += capacitance;
    }
    moyenne = moyenne / integration;
    Serial.println(moyenne);
    if (moyenne > 100) {
      return true;
    } else {
      return false;
    }
}


void loop()

{
    if (rain(120)) {
      //Serial.println("il pleut, fermer l'abris");
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      digitalWrite(LED_BUILTIN, LOW);
    }
    while (millis() % 1000 != 0)
       ;
}
