#include <Adafruit_MLX90614.h>

// Initialize MLX906 sensor.
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup() {
  // put your setup code here, to run once:

  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  if (!mlx.begin()) {
    Serial.println("Could not find a valid MLX906 sensor, check wiring!");
    while (1);
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:

  
  // print out the value you readfrom MLX90614:
  Serial.print("IR probe -- Ambient = ");
  Serial.print(mlx.readAmbientTempC()); 
  Serial.print("*C\tObject = ");
  Serial.print(mlx.readObjectTempC());
  Serial.print("*C");
  Serial.println();
  delay(1000);

}
