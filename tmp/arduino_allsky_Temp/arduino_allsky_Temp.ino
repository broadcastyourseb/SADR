/*
  ReadAnalogVoltage
  Reads an analog input on pin 0, converts it to voltage, and prints the result to the serial monitor.
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.
 
  + Example testing sketch for various DHT humidity/temperature sensors
 */

#include <DHT.h>
/*#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_BME280.h>*/
#include <math.h>

#define DHTPININT 2     // what digital pin we're connected to
#define DHTPINEXT 3     // what digital pin we're connected to

#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
/*#define BME280_ADDRESS 0x76 // address I2C du BME280

#define WindSensorPin (2) // The pin location of the anemometer sensor*/
#define tempo (1)     // temps d'integration de la mesure du vent en s

/*volatile unsigned long Rotations; // cup rotation counter used in interrupt routine
volatile unsigned long ContactBounceTime; // Timer to avoid contact bounce in interrupt routine

float WindSpeed; // speed km/h*/

// Initialize DHT sensor.
DHT dhtInt(DHTPININT, DHTTYPE);
DHT dhtExt(DHTPINEXT, DHTTYPE);

/*// Initialize MLX906 sensor.
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// Initialize BME280
Adafruit_BME280 bme;*/

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  /*pinMode(WindSensorPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(WindSensorPin), rotation, FALLING);*/
  
  dhtInt.begin();
  dhtExt.begin();

  /*if (!mlx.begin()) {
    Serial.println("Could not find a valid MLX906 sensor, check wiring!");
    while (1);
  }
  
  if (!bme.begin(BME280_ADDRESS)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }*/
}

// the loop routine runs over and over again forever:
void loop() {
  
  /*// Set Rotations count to 0 ready for calculations
  Rotations = 0;

  sei(); // Enables interrupts*/
  delay (tempo * 1000); // Wait 3 seconds to average
  /*cli(); // Disable interrupts

  // A wind speed of 1.492 MPH (2.4 km/h) causes the switch to close once per second. 
  // V = R * 2.4

  WindSpeed = Rotations * 2.4 / tempo;*/

  // read the input on analog pin 0:
  int sensorValueA0 = analogRead(A0);

  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValueA0 * (5.0 / 1023.0);
  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float hInt = dhtInt.readHumidity();
  // Read temperature as Celsius (the default)
  float tInt = dhtInt.readTemperature();
  
    // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float hExt = dhtExt.readHumidity();
  // Read temperature as Celsius (the default)
  float tExt = dhtExt.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(hInt) || isnan(tInt)) {
    Serial.println("Failed to read from DHT sensor Internal!");
    return;
  }
    // Check if any reads failed and exit early (to try again).
  if (isnan(hExt) || isnan(tExt)) {
    Serial.println("Failed to read from DHT sensor External!");
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hicInt = dhtInt.computeHeatIndex(tInt, hInt, false);

  // Compute heat index in Celsius (isFahreheit = false)
  float hicExt = dhtExt.computeHeatIndex(tExt, hExt, false);

  Serial.println();

  // print out the value you readfrom DHT22 Int:
  Serial.print("DHT22 Int -- Humidity: ");
  Serial.print(hInt);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(tInt);
  Serial.print(" *C\t");
  Serial.print("Heat index: ");
  Serial.print(hicInt);
  Serial.print(" *C\t");
  Serial.println();
  
  // print out the value you readfrom DHT22 Ext:
  Serial.print("DHT22 Ext -- Humidity: ");
  Serial.print(hExt);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(tExt);
  Serial.print(" *C\t");
  Serial.print("Heat index: ");
  Serial.print(hicExt);
  Serial.print(" *C\t");
  Serial.println();

  // print out the value you readfrom solar panel:
  Serial.print("Solar panel -- Voltage : ");
  Serial.print(voltage);
  Serial.print(" V\t ");
  Serial.println();

  /*// print out the value you readfrom MLX90614:
  Serial.print("IR probe -- Ambient = ");
  Serial.print(mlx.readAmbientTempC()); 
  Serial.print("*C\tObject = ");
  Serial.print(mlx.readObjectTempC());
  Serial.print("*C");
  Serial.println();

  // print out the value you readfrom BME280:
  Serial.print("BME280 -- Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.print(" *C\t");
  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.print(" hPa\t");
  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.print(" %\t");
  Serial.println();

  // print out the windspeed and switch count
  Serial.print("Nombre de rotation = ");
  Serial.print(Rotations); Serial.print("\t\t");
  Serial.print("Vitesse du vent = ");
  Serial.print(WindSpeed);
  Serial.print(" km/h\t");
  Serial.println();*/
}

/*// This is the function that the interrupt calls to increment the rotation count
void rotation () {

  if ((millis() - ContactBounceTime) > 15 ) { // debounce the switch contact.
    Rotations++;
    ContactBounceTime = millis();
  }

} */
