#include <math.h>

// this constant won't change:
const int  WindSensorPin = 3;    // the pin that the pushbutton is attached to
const int ledPin = 13;       // the pin that the LED is attached to

volatile unsigned long Rotations; // cup rotation counter used in interrupt routine
volatile unsigned long ContactBounceTime; // Timer to avoid contact bounce in interrupt routine

float WindSpeed; // speed miles per hour

// This is the function that the interrupt calls to increment the rotation count
void isr_rotation () {

  if ((millis() - ContactBounceTime) > 15 ) { // debounce the switch contact.
    Rotations++;
    ContactBounceTime = millis();
  }
}

void setup() {

  // initialize the anemometer as a input:
  pinMode(WindSensorPin, INPUT);
    // initialize the LED as an output:
  pinMode(ledPin, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(WindSensorPin), isr_rotation, FALLING);

  // initialize serial communication:
  Serial.begin(9600);

}

void loop() {

  Rotations = 0; // Set Rotations count to 0 ready for calculations
  
  sei(); // Enables interrupts
  delay (3000); // Wait 3 seconds to average
  cli(); // Disable interrupts
  
  // A wind speed of 1.492 MPH (2.4 km/h) causes the switch to close once per second. 
  // V = R / 30 * 2.4 = R * 0.8
  
  WindSpeed = Rotations * 0.75;
  
  Serial.print(Rotations); Serial.print("\t\t");
  Serial.println(WindSpeed);

} 
