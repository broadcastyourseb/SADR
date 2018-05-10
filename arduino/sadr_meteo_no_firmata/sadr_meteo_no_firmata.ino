/* BROADCASTYOURSEB http://www.sadr.fr

Inspired by :
SADRMETEO FIRMWARE based on INDIDUINOMETEO FIRMWARE.
NACHO MAS 2013. http://indiduino.wordpress.com


IMPORTANT: Customize following values to match your setup
*/
// Delay between 2 serial printing
#define SERIAL_DELAY 10000

//Comment out if you setup don't include some sensor.
#define USE_DHT_SENSOR_INTERNAL   //USE INTERNAL DHT HUMITITY SENSOR. Comment if not.
#define USE_DHT_SENSOR_EXTERNAL   //USE EXTERNAL DHT HUMITITY SENSOR. Comment if not.
#define USE_IR_SENSOR   //USE MELEXIS IR SENSOR. Comment if not.
#define USE_P_SENSOR   //USE BME280 PRESSURE SENSOR. Comment if not.
#define USE_WIND_SENSOR   //USE ANEMOMETER type SWITCH. Comment if not.
#define USE_LIGHT_SENSOR   //USE SOLAR PANEL AS LIGHT SENSOR. Comment if not.
#define USE_RAIN_SENSOR   //USE TELECONTROLLI CAPACITIVE RAIN SENSOR. Comment if not.

//All sensors (T22int=DHT22 INT,T22ext=DHT22 EXT,Tir=MELEXIS and Tp=BME280) include a ambient temperature
//Choose  that sensor, only one, is going to use for main Ambient Temperature:
#define T_MAIN_T22ext
//#define T_MAIN_T22int
//#define T_MAIN_Tir  
//#define T_MAIN_Tp

//DHT Sensor
#define DHT_HUMIDITY_CORRECTION -15

//IR Sensor
//Cloudy sky is warmer that clear sky. Thus sky temperature meassure by IR sensor
//is a good indicator to estimate cloud cover. However IR really meassure the
//temperatura of all the air column above increassing with ambient temperature.
//So it is important include some correction factor:
//From AAG Cloudwatcher formula. Need to improve futher.
//http://www.aagware.eu/aag/cloudwatcher700/WebHelp/index.htm#page=Operational%20Aspects/23-TemperatureFactor-.htm
//Sky temp correction factor. Tsky=Tsky_meassure - Tcorrection
//Formula Tcorrection = (K1 / 100) * (Thr - K2 / 10) + (K3 / 100) * pow((exp (K4 / 1000* Thr)) , (K5 / 100));
#define  K1 33.
#define  K2 0. 
#define  K3 4.
#define  K4 100.
#define  K5 100.

//Clear sky corrected temperature (temp below means 0% clouds)
#define CLOUD_TEMP_CLEAR  -22
//Totally cover sky corrected temperature (temp above means 100% clouds)
#define CLOUD_TEMP_OVERCAST  -10
//Activation treshold for cloudFlag (%)
#define CLOUD_FLAG_PERCENT  30

//Light sensor
// Activation threshold for daylight (between 0 and 5000mV)
#define DAYLIGHT_FLAG_TRIGGER 1

// Wind sensor
// Activation threshold for wind in km/h
#define WIND_FLAG_TRIGGER 20
#define MAX_WIND_FLAG_TRIGGER 50
// A wind speed of 1.492 MPH (2.4 km/h) causes the switch to close once per second. 
#define WIND_RPM_TO_KMH 25 // Relation between windspeed in km/h and rotation per minute
#define ANEMOMETER_BOUNCE_TIME 10 //Below this value, no count is adding

//Rain sensor
//Calibrate this value using known capacitor.
#define IN_STRAY_CAP_TO_GND 31
#define IN_CAP_TO_GND  31
#define MAX_ADC_VALUE 1023
#define RAIN_FLAG_TRIGGER 80 //if above this capacity then rainy is true
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
// Target temperature
#define COLD_TEMPERATURE_TARGET 10
#define HOT_TEMPERATURE_TARGET 55
#define COLD_TEMPERATURE_SECURITY -20 // must be less than COLD_TEMPERATURE_TARGET
#define HOT_TEMPERATURE_SECURITY 60 // must be greater than HOT_TEMPERATURE_TARGET

/*END OFF CUSTOMIZATION. YOU SHOULD NOT NEED TO CHANGE ANYTHING BELOW */

//#include <SPI.h>
#include <Adafruit_Sensor.h>


#ifdef USE_IR_SENSOR
  #include <Adafruit_MLX90614.h>
#endif //USE_IR_SENSOR

#if defined(USE_DHT_SENSOR_INTERNAL) || defined(USE_DHT_SENSOR_EXTERNAL)
  #include "DHT.h"
#endif //USE_DHT_SENSOR_INTERNAL

#ifdef USE_DHT_SENSOR_INTERNAL
  #define DHT_INT_PIN 2         // what pin we're connected DHT22 to
  #define DHT_INT_TYPE DHT22   // DHT 22  (AM2302), AM2321
  DHT dhtInt(DHT_INT_PIN, DHT_INT_TYPE);
#endif //USE_DHT_SENSOR_INTERNAL

#ifdef USE_DHT_SENSOR_EXTERNAL
  #define DHT_EXT_PIN 4         // what pin we're connected DHT22 to
  #define DHT_EXT_TYPE DHT22   // DHT 22  (AM2302), AM2321
  DHT dhtExt(DHT_EXT_PIN, DHT_EXT_TYPE);
#endif //USE_DHT_SENSOR_EXTERNAL

#ifdef USE_P_SENSOR
  #include <Adafruit_BME280.h>
  // Define I2C address of the pressure sensor
  #undef BME280_ADDRESS         // Undef BME280_ADDRESS from the BME280 library to easily override I2C address
  #define BME280_ADDRESS (0x76)// address I2C du BME280
  Adafruit_BME280 bme; // I2C
#endif //USE_P_SENSOR*/

#ifdef USE_IR_SENSOR
  // Initialize MLX906 sensor.
  Adafruit_MLX90614 mlx = Adafruit_MLX90614();
#endif //USE_IR_SENSOR*/

#ifdef USE_WIND_SENSOR
  #include <math.h>
  #define  ANEMOMETER_PIN 3    // the pin that the pushbutton is attached to
#endif //USE_WIND_SENSOR*/

#ifdef USE_RAIN_SENSOR
  #include <PID_v1.h>
  #define RAIN_OUT_PIN A2
  #define RAIN_IN_PIN A1
  double Consigne = COLD_TEMPERATURE_TARGET;
  //Variable pour echantillonnage de la sonde de température du capteur de pluie
  int samples[NUMSAMPLES];
  // Variables pour PID
  // Measured temperature
  double TempRain;
  // PWM value 0-255 :
  double Mosfet;

  // Déclaration de l'objet PID
  // Les arguments sont les variables de gestion puis les gains pour P, I et D et enfin,le mode.
  // Define the aggressive and conservative Tuning Parameters
  double aggKp = 43.7, aggKi = 0.323, aggKd = 7.18;
  double consKp = 4.47, consKi = 0.033, consKd = 0.734;
  PID myPID(&TempRain, &Mosfet, &Consigne, consKp, consKi, consKd, DIRECT);
#endif //USE_RAIN_SENSOR*/

float T22int,Hr22int,DewInt,T22ext,Hr22ext,DewExt,Light,Tp,P,T,IR,Clouds,skyT,Tir,WindSpeed,MaxWindSpeed,Capacity,reedSwitchDeltaTime, stringCheck;
int cloudy,dewing,frezzing,windy,rainy,daylight;
volatile unsigned long reedSwitchCount; // cup rotation counter used in interrupt routine
unsigned long tempoSerial, TimeStamp;

#if defined(USE_DHT_SENSOR_INTERNAL) || defined(USE_DHT_SENSOR_EXTERNAL)
// dewPoint function NOAA
// reference: http://wahiduddin.net/calc/density_algorithms.htm 
double dewPoint(double celsius, double humidity)
{
        double AZERO= 373.15/(273.15 + celsius);
        double SUM = -7.90298 * (AZERO-1);
        SUM += 5.02808 * log10(AZERO);
        SUM += -1.3816e-7 * (pow(10, (11.344*(1-1/AZERO)))-1) ;
        SUM += 8.1328e-3 * (pow(10,(-3.49149*(AZERO-1)))-1) ;
        SUM += log10(1013.246);
        double VP = pow(10, SUM-3) * humidity;
        double Tvar = log(VP/0.61078);   // temp var
        return (241.88 * Tvar) / (17.558-Tvar);
}

// delta max = 0.6544 wrt dewPoint()
// 5x faster than dewPoint()
// reference: http://en.wikipedia.org/wiki/Dew_point
double dewPointFast(double celsius, double humidity)
{
        double a = 17.271;
        double b = 237.7;
        double temp = (a * celsius) / (b + celsius) + log(humidity/100);
        double Td = (b * temp) / (a - temp);
        return Td;
}
#endif //USE_DHT_SENSOR_INTERNAL || USE_DHT_SENSOR_EXTERNAL

#ifdef USE_IR_SENSOR
//From AAG Cloudwatcher formula. Need to improve futher.
//http://www.aagware.eu/aag/cloudwatcher700/WebHelp/index.htm#page=Operational%20Aspects/23-TemperatureFactor-.htm
//https://azug.minpet.unibas.ch/wikiobsvermes/index.php/AAG_cloud_sensor#Snow_on_the_sky_temperature_sensor
//Td  = (K1 / 100) * (Ta - K2 / 10) + (K3 / 100) * (Exp (K4 / 1000*Ta)) ^ (K5 / 100)
//Tsky=Ts – Td 
double skyTemp() {
  //Constant defined above
  double Td = (K1 / 100.) * (T - K2 / 10) + (K3 / 100.) * pow((exp (K4 / 1000.* T)) , (K5 / 100.));
  double Tsky=IR-Td; // in °C
  return Tsky; //in °C
}

double cloudIndex() {
   double Tcloudy= CLOUD_TEMP_OVERCAST,Tclear=CLOUD_TEMP_CLEAR;
   double Tsky=skyTemp();
   double Index;
   if (Tsky<Tclear) Tsky=Tclear;
   if (Tsky>Tcloudy) Tsky=Tcloudy;
   Index=(Tsky-Tclear)*100/(Tcloudy-Tclear);
   return Index;
}
#endif //USE_IR_SENSOR

#ifdef USE_WIND_SENSOR
// This is the function that the interrupt calls to increment the rotation count
void isr_rotation () {
    // debounce the switch contact.
    reedSwitchDeltaTime = millis() - TimeStamp;
    if (reedSwitchDeltaTime > ANEMOMETER_BOUNCE_TIME ) {
        reedSwitchCount++;
        TimeStamp = millis();

        //switch closed 2 times per revolution - SERIAL_DELAY in ms
        // Winspeed and MaxWindSpeed in RPM
        WindSpeed = (reedSwitchCount-1) * 30000 / SERIAL_DELAY;

        // Winspeed and MaxWindSpeed in Km/h
        WindSpeed = WindSpeed / WIND_RPM_TO_KMH;
        if (MaxWindSpeed < (30000 / reedSwitchDeltaTime) / WIND_RPM_TO_KMH) {
           //MaxWindSpeed = (1 / 2 * 60) / (reedSwitchDeltaTime / 1000);
            MaxWindSpeed = (30000 / reedSwitchDeltaTime)/ WIND_RPM_TO_KMH;
        }
    }
}
#endif //USE_WIND_SENSOR

#ifdef USE_RAIN_SENSOR
float rain(int integration) {
  //Capacitor under test between RAIN_OUT_PIN and RAIN_IN_PIN

    float moyenne = 0, capacitance = 0;
  
    for (int i = 0; i < integration; i++) {
      
      //Rising high edge on RAIN_OUT_PIN
      pinMode(RAIN_IN_PIN, INPUT);
      digitalWrite(RAIN_OUT_PIN, HIGH);
      int val = analogRead(RAIN_IN_PIN);
      digitalWrite(RAIN_OUT_PIN, LOW);
  
      //Clear everything for next measurement
      pinMode(RAIN_IN_PIN, OUTPUT);
      delay(5);
  
      //Calculate and print result
      if (val == MAX_ADC_VALUE) { // Avoid a division by 0
        capacitance = 300;
      } else {
        capacitance = (float)val * IN_CAP_TO_GND / (float)(MAX_ADC_VALUE - val);
      }
      // Max capacitance is 300. It's more handy.
      if (capacitance > 300) {
        capacitance = 300; 
      }
      moyenne += capacitance;
    }
    moyenne = moyenne / integration;
    return moyenne;
}

float getTemperature() {
  uint8_t i;
  float average;

  // take N samples in a row, with a slight delay
  //for (i = 0; i < NUMSAMPLES; i++) {
  //  samples[i] = analogRead(THERMISTORPIN);
  //}

  // average all the samples out
  average = 0;
  for (i = 0; i < NUMSAMPLES; i++) {
    //average += samples[i];
    average += analogRead(THERMISTORPIN);   
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

void regulHeat(double Temperature, double Target) {

  double gap = abs(Target - Temperature); //distance away from setpoint

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
}
#endif //USE_RAIN_SENSOR

// This is the function called to initialize each activate sensor
void setupMeteoStation(){

  // Start serial
  Serial.begin(9600);

#ifdef USE_P_SENSOR
  bme.begin(BME280_ADDRESS);
#endif //USE_P_SENSOR

#ifdef USE_DHT_SENSOR_INTERNAL
  dhtInt.begin();
#endif //USE_DHT_SENSOR_INTERNAL

#ifdef USE_DHT_SENSOR_EXTERNAL
  dhtExt.begin();
#endif //USE_DHT_SENSOR_EXTERNAL

#ifdef USE_IR_SENSOR
   mlx.begin();
#endif //USE_IR_SENSOR*/

#ifdef USE_WIND_SENSOR
  // initialize the anemometer as a input:
  pinMode(ANEMOMETER_PIN, INPUT);
  // interrupt when switch contact is closed
  attachInterrupt(digitalPinToInterrupt(ANEMOMETER_PIN), isr_rotation, FALLING);
#endif //USE_WIND_SENSOR*/

#ifdef USE_RAIN_SENSOR
  pinMode(RAIN_OUT_PIN, OUTPUT);
  pinMode(RAIN_IN_PIN, OUTPUT);
    // broche 11 en sortie
  pinMode(OUTP, OUTPUT);
  // contrôle entre 0-255
  myPID.SetOutputLimits(0, 255);
  // on active la régulation PID
  myPID.SetMode(AUTOMATIC);
#endif //USE_RAIN_SENSOR*/
}

/*==============================================================================
 * METEOSTATION FUNCTIONS
 *============================================================================*/
void runMeteoStation() {
  
#ifdef USE_IR_SENSOR  
    
    IR = mlx.readObjectTempC(); // Sky temperature in °C

    Tir = mlx.readAmbientTempC(); // Ambiant temperature in °C
    Clouds=cloudIndex();
    skyT=skyTemp();
    if (Clouds >CLOUD_FLAG_PERCENT) {
      cloudy=1;
    } else {
      cloudy=0;
    }
#endif //USE_IR_SENSOR  

#ifdef USE_P_SENSOR
    Tp=bme.readTemperature();
    P=bme.readPressure()/ 100; // in hPa     
#endif //USE_P_SENSOR  */

#ifdef USE_DHT_SENSOR_INTERNAL
    Hr22int=dhtInt.readHumidity();
    Hr22int+=DHT_HUMIDITY_CORRECTION;  
    T22int=dhtInt.readTemperature();
#endif //USE_DHT_SENSOR_INTERNAL

#ifdef USE_DHT_SENSOR_EXTERNAL
    Hr22ext=dhtExt.readHumidity();
    Hr22ext+=DHT_HUMIDITY_CORRECTION;
    T22ext=dhtExt.readTemperature();
#endif //USE_DHT_SENSOR_EXTERNAL    

#ifdef USE_LIGHT_SENSOR
    Light=analogRead(0)*4.88;
    if (Light > DAYLIGHT_FLAG_TRIGGER) {
        daylight=1;
    } else {
        daylight=0;
    }
#endif //USE_LIGHT_SENSOR

#if defined T_MAIN_T22ext
    T=T22ext;
#elif defined T_MAIN_T22int
    T=T22int;
#elif defined T_MAIN_Tir  
    T=Tir;
#elif defined T_MAIN_Tp
    T=Tp;
#endif  //T_MAIN  

if (T <=2) {
  frezzing=1;
} else {
  frezzing=0;
}

#ifdef USE_DHT_SENSOR_EXTERNAL
    DewExt=dewPoint(T,Hr22ext);
    if (T<=DewExt+2) { 
       dewing=1;
    } else {
       dewing=0;
    }
#endif //USE_DHT_SENSOR_EXTERNAL

#ifdef USE_RAIN_SENSOR
    //Asking for temperature of the rain sensor
    TempRain = getTemperature();
    Capacity = rain(NUMSAMPLES);
    if (Capacity > RAIN_FLAG_TRIGGER) {
      rainy=1;
    } else {
      rainy=0;
    }
#endif //USE_RAIN_SENSOR*/

#ifdef USE_WIND_SENSOR
  if ((WindSpeed > WIND_FLAG_TRIGGER) | (MaxWindSpeed > MAX_WIND_FLAG_TRIGGER)) {
        windy=1;
    } else {
        windy=0;
    }
#endif //USE_WIND_SENSOR*/
}

/*==============================================================================
 * CHECK OUPUT SERIAL
 *============================================================================*/
bool outputCheck() {

    // Initialize the check variable to zero
    stringCheck = true;
    // Sum output variable
    float checksum = stringCheck = T + frezzing + Light + daylight + T22int + Hr22int + T22ext + Hr22ext + DewExt +
    dewing + Tir + IR + skyT + Clouds + cloudy + P + Tp + WindSpeed + MaxWindSpeed + windy + Capacity +
    rainy + Consigne + TempRain + Mosfet ;

    // If everything is ok, function return TRUE
    if (isnan(checksum) || isinf(checksum)) {
        stringCheck = false ;
    }
    //Serial.println(checksum);
    return stringCheck;
}

/*==============================================================================
 * OUPUT SERIAL
 *============================================================================*/
void outputChain() {
    // send sensor values:
    Serial.print("N:");
    //Generic value
    Serial.print(T);
    Serial.print(":");
    Serial.print(frezzing);
    Serial.print(":");
    
#ifdef USE_LIGHT_SENSOR
    Serial.print(Light);
    Serial.print(":");
    Serial.print(daylight);
    Serial.print(":");
#endif //USE_LIGHT_SENSOR*/

#ifdef  USE_DHT_SENSOR_INTERNAL
    Serial.print(T22int);
    Serial.print(":");
    Serial.print(Hr22int);
    Serial.print(":");
#endif //USE_DHT_SENSOR_INTERNAL*/

#ifdef USE_DHT_SENSOR_EXTERNAL
    Serial.print(T22ext);
    Serial.print(":");
    Serial.print(Hr22ext);
    Serial.print(":");
    Serial.print(DewExt);
    Serial.print(":");
    Serial.print(dewing);
    Serial.print(":");
#endif //USE_DHT_SENSOR_EXTERNAL*/

#ifdef USE_IR_SENSOR
    Serial.print(Tir);
    Serial.print(":");
    Serial.print(IR);
    Serial.print(":");
    Serial.print(skyT);
    Serial.print(":");
    Serial.print(Clouds);
    Serial.print(":");
    Serial.print(cloudy);
    Serial.print(":");
#endif //USE_IR_SENSOR*/

#ifdef USE_P_SENSOR
    Serial.print(P);
    Serial.print(":");
    Serial.print(Tp);
    Serial.print(":");
#endif //USE_P_SENSOR*/

#ifdef USE_WIND_SENSOR
    Serial.print(WindSpeed);
    Serial.print(":");
    Serial.print(MaxWindSpeed);
    Serial.print(":");
    Serial.print(windy);
    Serial.print(":");
#endif //USE_WIND_SENSOR*/

#ifdef USE_RAIN_SENSOR
    Serial.print(Capacity);
    Serial.print(":");
    Serial.print(rainy);
    Serial.print(":");
    Serial.print(Consigne);
    Serial.print(":");
    Serial.print(TempRain);
    Serial.print(":");
    Serial.println(Mosfet);
#endif //USE_DHT_RAIN_SENSOR*/
}

/*==============================================================================
 * SETUP()
 *============================================================================*/
void setup() {
  setupMeteoStation();
  delay(5000);

  TimeStamp = millis();
}

/*==============================================================================
 * LOOP()
 *============================================================================*/
void loop() {
   
  #ifdef USE_RAIN_SENSOR
  if (rainy == 1) {
     Consigne = HOT_TEMPERATURE_TARGET;
  } else {
     if (!isnan(T22int)) {
        int newConsigne = ((T22int / 5) + 2 );
        newConsigne *= 5;
        if (newConsigne > COLD_TEMPERATURE_TARGET && newConsigne < HOT_TEMPERATURE_TARGET) {
            Consigne = max(COLD_TEMPERATURE_TARGET , newConsigne);
        }
     }
  }

  if ((TempRain <= HOT_TEMPERATURE_SECURITY) | (TempRain >= COLD_TEMPERATURE_SECURITY)) {
    regulHeat(TempRain, Consigne);
  } else {
    if (rainy == 1) {
        Mosfet = 100;
    } else {
        Mosfet = 60;
    }
    analogWrite(OUTP, Mosfet);
  }
  #endif //USE_RAIN_SENSOR*/
  
  if ((millis() - tempoSerial) > SERIAL_DELAY ) { // time between 2 serial sending.
    runMeteoStation();
    if (outputCheck()) {
        outputChain();
    } else {
        Serial.print("error:");
        outputChain();
    }
    tempoSerial = millis();
    WindSpeed = 0; // Set WindSpeed count to 0 after calculations
    MaxWindSpeed = 0; // Set MaxWindSpeed count to 0 after calculations
    reedSwitchCount = 0; // Set reedSwitchCount count to 0 after calculations
    reedSwitchDeltaTime = 0; // Set reedSwitchDeltaTime value to 0 after calculations
  }
}
