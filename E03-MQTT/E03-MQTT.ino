#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <PubSubClient.h>
#include <Ticker.h>

// I need the sea level pressure for altitude calculation. Using 1013.25hPa.
#define SEA_LEVEL_PRESSURE_HPA (1013.25)

const char* readErrorMessage = "An error ocurred reading data from the sensor.";
Adafruit_BME280 bme; // The sensor
float temperature = -1., humidity = -1., pressure = -1., altitude = -1.; // The variables for the measurements.
bool sensor_ok; // Stores the result of the sensor connection test.
const char* ssid = "<YOUR SSID HERE>";  // Enter SSID here
const char* password = "<YOUR WIFI PASSWORD HERE>";  // Enter Password here
const char* mqtt_broker = "<YOUR BROKER ADDRESS>"; // The MQTT Broker IP address
const char* mqtt_client_id = "MQTT Test Sensor 1"; // The MQTT client ID
WiFiClient wifiClient; // The WiFi Client
PubSubClient pubSubClient(wifiClient); // The MQTT client
Ticker ticker;

void setup() {
  Serial.begin(9600); // Open a serial port
  delay(100);
  setup_sensor(); // Configure the sensor
  setup_wifi(); // Configure the WiFi
  pubSubClient.setServer(mqtt_broker, 1883); // Configure the MQTT connection
  ticker.attach(30, read_data);
}

void setup_sensor() {
  sensor_ok = bme.begin(0x76); // Connect to the sensor using I2C
  delay(100);
  // Check if I can connect to the sensor
  if (sensor_ok) {
    Serial.println("Sensor is ok!");
  } else {
    Serial.println("Sensor not found, check wiring and reset MCU.");
  }
}

void setup_wifi() {
  WiFi.mode( WIFI_STA ); // The MCU shall act as a WiFi station
  delay(100);
  Serial.println();
  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected...");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());
}

void read_data() {
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F;
  altitude = bme.readAltitude(SEA_LEVEL_PRESSURE_HPA);
  // Check if any reads failed and print an error if so.
  if (isnan(humidity) || isnan(temperature) || isnan(pressure) || isnan(altitude)) {
    return;
  }

  // Publish to MQTT
  pubSubClient.publish("/sensor/temperature", String(temperature).c_str());
  pubSubClient.publish("/sensor/humidity", String(humidity).c_str());
  pubSubClient.publish("/sensor/pressure", String(pressure).c_str());
  pubSubClient.publish("/sensor/altitude", String(altitude).c_str());
  
  // Print to serial
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");
  Serial.print("Relative humidity: ");
  Serial.print(humidity);
  Serial.println("%");
  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.println("hPa");
  Serial.print("Altitude: ");
  Serial.print(altitude);
  Serial.println("m");
}

void reconnect() {
  while (!pubSubClient.connected()) {
    Serial.print("Reconnecting... ");
    if (!pubSubClient.connect(mqtt_client_id)) {
      Serial.print("failed, return code ");
      Serial.print(pubSubClient.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    } else {
      Serial.println(" Done!");
    }
  }
}

void loop() {
  // Check if the client is still connected and try to reconnect if not
  if (!pubSubClient.connected()) {
    reconnect();
  }
  
  pubSubClient.loop();
}
