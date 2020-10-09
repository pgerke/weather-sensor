#include <LiquidCrystal.h>
#include <Adafruit_BME280.h>

// I need the sea level pressure for altitude calculation. Using 1013.25hPa.
#define SEA_LEVEL_PRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // The sensor
float temperature = -1., humidity = -1., pressure = -1., altitude = -1.; // The variables for the measurements.
bool even_loop = true; // The display is too small to show all values at once, so I'll show them in an alternating way.
bool sensor_ok; // Stores the result of the sensor connection testJa, gena
LiquidCrystal lcd(7,8,9,10,11,12); // Define the LC display and the pins that I'm using

void setup() {
  lcd.begin(16, 2); // Initialize the display to use two lines with 16 characters each.
  sensor_ok = bme.begin(0x76); // Connect to the sensor using I2C

  // Check if I can connect to the sensor
  if (sensor_ok) {
    lcd.setCursor(0, 0); // Set the cursor to the first char of the first line.
    lcd.print("Sensor is ok!");  
    lcd.setCursor(0, 1);
    lcd.print("Stand by...");  
  } else {
    // ...and give an error message of not.
    lcd.setCursor(0, 0);
    lcd.print("Sensor not found!");  
    lcd.setCursor(0, 1);
    lcd.print("Check wiring...");  
  }
}

void loop() {
  // If the connection test failed, it makes no sense to do a measurement.
  if (!sensor_ok) {
    delay(3000);
    return;
  }

  // Toggle the loop indicator
  even_loop = !even_loop;
  // Read data from the sensor
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F;
  altitude = bme.readAltitude(SEA_LEVEL_PRESSURE_HPA);

  // Check if any reads failed and print an error if so.
  if (isnan(humidity) || isnan(temperature) || isnan(pressure) || isnan(altitude)) {
    lcd.setCursor(0, 0);
    lcd.print("Reading failed!");  
    lcd.setCursor(0, 1);
    lcd.print("Retrying...");  
    delay(3000);
    return;
  }

  if (even_loop) { // Print temperature and humidity on an even loop
    lcd.setCursor(0, 0);
    lcd.print("Temp.:  ");
    lcd.print(temperature);
    lcd.print(" C ");
    lcd.setCursor(0, 1);
    lcd.print("rel.H:  ");
    lcd.print(humidity);
    lcd.print("%  ");
  } else { // Print pressure and altitude on an odd loop
    lcd.setCursor(0, 0);
    lcd.print("Pres.: ");
    lcd.print(pressure);
    lcd.print("hPa");
    lcd.setCursor(0, 1);
    lcd.print("Alti.: ");
    lcd.print(altitude);
    lcd.print("m "); 
  }
  delay(3000); // wait for 3s before restarting the loop
}
