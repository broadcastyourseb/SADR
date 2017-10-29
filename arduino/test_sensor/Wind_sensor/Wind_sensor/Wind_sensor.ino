float WindSpeed,MaxWindSpeed,MiniTime,reedSwitchTime;
volatile unsigned long reedSwitchCount; // cup rotation counter used in interrupt routine
volatile unsigned long ContactBounceTime; // Timer to avoid contact bounce in interrupt routine
unsigned long tempoSerial, TimeStamp;

// Delay between 2 serial printing
#define SERIAL_DELAY 10000

//Comment out if you setup don't include some sensor.
#define USE_WIND_SENSOR   //USE ANEMOMETER type SWITCH. Comment if not.

// Wind sensor
// Activation threshold for wind in km/h
#define WIND_FLAG_TRIGGER 20
// A wind speed of 1.492 MPH (2.4 km/h) causes the switch to close once per second. 
#define WIND_RPM_TO_KMH 25 // Relation between windspeed in km/h and rotation per minute
#define ANEMOMETER_BOUNCE_TIME 15 //Below this value, no count is adding

#ifdef USE_WIND_SENSOR
  #include <math.h>
  #define  ANEMOMETER_PIN 3    // the pin that the pushbutton is attached to
#endif //USE_WIND_SENSOR*/

// This is the function that the interrupt calls to increment the rotation count
#ifdef USE_WIND_SENSOR
// This is the function that the interrupt calls to increment the rotation count
void isr_rotation () {
  TimeStamp = millis();
  if ((TimeStamp - ContactBounceTime) > ANEMOMETER_BOUNCE_TIME ) { // debounce the switch contact.
    reedSwitchCount++;
    if (reedSwitchCount > 1) {
        reedSwitchTime += TimeStamp - ContactBounceTime;
    }
    if ((TimeStamp - ContactBounceTime) < MiniTime) {
        MiniTime = TimeStamp - ContactBounceTime;
    }
  }
  ContactBounceTime = TimeStamp ;
  // Winspeed and MaxWindSpeed in RPM - switch closed 2 times per revolution - SERIAL_DELAY in ms
  //WindSpeed = (reedSwitchCount / 2 * 60) / (reedSwitchTime / 1000);
  WindSpeed = (reedSwitchCount - 1) / reedSwitchTime * 30000;
  if (MiniTime == SERIAL_DELAY || MiniTime == 0) {
    MaxWindSpeed = WindSpeed ;
  } else {
    //MaxWindSpeed = (1 / 2 * 60) / (MiniTime / 1000);
    MaxWindSpeed = 30000 / MiniTime;
  }
  // Winspeed and MaxWindSpeed in Km/h
  WindSpeed = WindSpeed / WIND_RPM_TO_KMH;
  MaxWindSpeed = MaxWindSpeed / WIND_RPM_TO_KMH;
}

#endif //USE_WIND_SENSOR

void setup() {

#ifdef USE_WIND_SENSOR
  // initialize the anemometer as a input:
  pinMode(ANEMOMETER_PIN, INPUT);
  // interrupt when switch contact is closed
  attachInterrupt(digitalPinToInterrupt(ANEMOMETER_PIN), isr_rotation, FALLING);
#endif //USE_WIND_SENSOR*/



  // initialize serial communication:
  Serial.begin(9600);

}

void loop() {

  if ((millis() - tempoSerial) > SERIAL_DELAY ) { // time between 2 serial sending.

    Serial.print(WindSpeed);
    Serial.print(":");
    Serial.println(MaxWindSpeed);
    
    tempoSerial = millis();
    WindSpeed = 0; // Set WindSpeed count to 0 after calculations
    MaxWindSpeed = 0; // Set MaxWindSpeed count to 0 after calculations
    ContactBounceTime = 0; // Set ContactBounceTime count to 0 after calculations
    reedSwitchCount = 0; // Set reedSwitchCount count to 0 after calculations
    reedSwitchTime = 0; // Set reedSwitchTime to 0 afer calculations
    MiniTime = SERIAL_DELAY; // Set MiniTime value to SERIAL_DELAY after calculations
  }
}
