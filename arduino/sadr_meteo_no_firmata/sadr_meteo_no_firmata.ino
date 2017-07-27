/* BROADCASTYOURSEB http://www.sadr.fr

Inspired by :
SADRMETEO FIRMWARE based on INDIDUINOMETEO FIRMWARE.
NACHO MAS 2013. http://indiduino.wordpress.com


IMPORTANT: Customize following values to match your setup
*/

//Comment out if you setup don't include some sensor.
#define USE_DHT_SENSOR_INTERNAL   //USE INTERNAL DHT HUMITITY SENSOR. Comment if not.
#define USE_DHT_SENSOR_EXTERNAL   //USE EXTERNAL DHT HUMITITY SENSOR. Comment if not.
#define USE_IR_SENSOR   //USE MELEXIS IR SENSOR. Comment if not.
#define USE_P_SENSOR   //USE BME280 PRESSURE SENSOR. Comment if not.
#define USE_WIND_SENSOR   //USE ANEMOMETER type SWITCH. Comment if not.
#define USE_LIGHT_SENSOR   //USE SOLAR PANEL AS LIGHT SENSOR. Comment if not.
#define USE_DHT_RAIN_SENSOR   //USE TELECONTROLLI CAPACITIVE RAIN SENSOR. Comment if not.

//All sensors (T22int=DHT22 INT,T22ext=DHT22 EXT,Tir=MELEXIS and Tp=BME280) include a ambient temperature
//Choose  that sensor, only one, is going to use for main Ambient Temperature:
//#define T_MAIN_T22ext
//#define T_MAIN_T22int
#define T_MAIN_Tir  
//#define T_MAIN_Tp

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
#define CLOUD_TEMP_CLEAR  -8 
//Totally cover sky corrected temperature (temp above means 100% clouds)
#define CLOUD_TEMP_OVERCAST  0  
//Activation treshold for cloudFlag (%)
#define CLOUD_FLAG_PERCENT  30

// Activation threshold for daylight (between 0 and 1024)
#define DAYLIGHT_FLAG_TRIGGER 5

// Activation threshold for wind in km/h
#define WIND_FLAG_TRIGGER 10
// Rotation count period to estimate the wind speed in millisecond
#define WIND_MEAN_TIME 3000

//Rain sensor
//Calibrate this value using known capacitor.
#define IN_STRAY_CAP_TO_GND 31
#define IN_CAP_TO_GND  31
#define MAX_ADC_VALUE 1023
#define RAIN_FLAG_TRIGGER 120 //if above this capacity then rainy

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
    #define RAIN_OUT_PIN = A2;
    #define RAIN_IN_PIN = A1;
#endif //USE_RAIN_SENSOR*/

float T22int,Hr22int,DewInt,T22ext,Hr22ext,DewExt,Light,Tp,P,T,IR,Clouds,skyT,Tir,WindSpeed,Capacity;
int cloudy,dewing,frezzing,windy,rainy,daylight;
volatile unsigned long Rotations; // cup rotation counter used in interrupt routine
volatile unsigned long ContactBounceTime; // Timer to avoid contact bounce in interrupt routine

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
        double T = log(VP/0.61078);   // temp var
        return (241.88 * T) / (17.558-T);
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

  if ((millis() - ContactBounceTime) > 15 ) { // debounce the switch contact.
    Rotations++;
    ContactBounceTime = millis();
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
  
      //Low value capacitor
      //Clear everything for next measurement
      pinMode(RAIN_IN_PIN, OUTPUT);
  
      //Calculate and print result
      capacitance = (float)val * IN_CAP_TO_GND / (float)(MAX_ADC_VALUE - val);
      moyenne += capacitance;
    }
    moyenne = moyenne / integration;
    return moyenne;
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
#else
    //set IR sensor fail flag
    digitalWrite(PIN_TO_DIGITAL(13), HIGH);
#endif //USE_IR_SENSOR  

#ifdef USE_P_SENSOR
    Tp=bme.readTemperature();
    P=bme.readPressure()/ 100; // in hPa
#else
    //set P sensor fail flag
    digitalWrite(PIN_TO_DIGITAL(13), HIGH);     
#endif //USE_P_SENSOR  */

#ifdef USE_DHT_SENSOR_INTERNAL
    Hr22int=dhtInt.readHumidity();  
    T22int=dhtInt.readTemperature();
    // Check if any reads failed and exit early (to try again).
    /*if (isnan(Hr22int) || isnan(T22int)) {
      //set HR sensor fail flag
      digitalWrite(PIN_TO_DIGITAL(13), HIGH); 
    } else {
      //OK.clear HR sensor fail flag       
      digitalWrite(PIN_TO_DIGITAL(13), LOW); 
    }*/

    DewInt=dewPoint(T22int,Hr22int);
    if (T22int<=DewInt+2) { 
       dewing=1;
    } else {
       dewing=0;
    }
#endif //USE_DHT_SENSOR_INTERNAL

#ifdef USE_DHT_SENSOR_EXTERNAL
    Hr22ext=dhtExt.readHumidity();  
    T22ext=dhtExt.readTemperature();
    // Check if any reads failed and exit early (to try again).
    /*if (isnan(Hr22ext) || isnan(T22ext)) {
      //set HR sensor fail flag
      digitalWrite(PIN_TO_DIGITAL(13), HIGH); 
    } else {
      //OK.clear HR sensor fail flag       
      digitalWrite(PIN_TO_DIGITAL(13), LOW); 
    }*/

#endif //USE_DHT_SENSOR_EXTERNAL    

#ifdef USE_LIGHT_SENSOR
    Light=analogRead(0);
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
    Capacity = rain(120);
    if (Capacity > RAIN_FLAG_TRIGGER) {
      rainy=1;
    } else {
      rainy=0;
    }
#endif //USE_RAIN_SENSOR*/

#ifdef USE_WIND_SENSOR
  Rotations = 0; // Set Rotations count to 0 ready for calculations
  
  sei(); // Enables interrupts
  delay (WIND_MEAN_TIME); // Wait x seconds to average
  cli(); // Disable interrupts
  
  // A wind speed of 1.492 MPH (2.4 km/h) causes the switch to close once per second. 
  // V = R / 3 * 2.4 = R * 0.8
  
  WindSpeed = Rotations * 0.75;
  
  if (WindSpeed > WIND_FLAG_TRIGGER) {
        windy=1;
    } else {
        windy=0;
    }
#endif //USE_WIND_SENSOR*/
}

/*==============================================================================
 * METEOSTATION FUNCTIONS
 *============================================================================*/
String outputChain() {
  return "toto";
}

/*==============================================================================
 * SETUP()
 *============================================================================*/
void setup() {
  setupMeteoStation();
}

/*==============================================================================
 * LOOP()
 *============================================================================*/
void loop() {
  runMeteoStation();
  Serial.print(outputChain());

}
