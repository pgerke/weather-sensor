#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// I need the sea level pressure for altitude calculation. Using 1013.25hPa.
#define SEA_LEVEL_PRESSURE_HPA (1013.25)

const char* readErrorMessage = "An error ocurred reading data from the sensor.";
Adafruit_BME280 bme; // The sensor
float temperature = -1., humidity = -1., pressure = -1., altitude = -1.; // The variables for the measurements.
bool sensor_ok; // Stores the result of the sensor connection test.
const char* ssid = "<YOUR SSID HERE>";  // Enter SSID here
const char* password = "<YOUR WIFI PASSWORD HERE>";  // Enter Password here
WiFiClient wifiClient; // The WiFi Client
ESP8266WebServer server(80); // The web server listening on port 80

void setup() {
  Serial.begin(9600); // Open a serial port
  delay(100);
  setup_sensor(); // Configure the sensor
  setup_wifi(); // Configure the WiFi
  setup_webserver(); // Configure the web server
}

void setup_webserver() {
  server.on("/", handle_OnConnect); // Configure the web server to handle requests on the base URL
  server.on("/json", handle_OnConnectJson); // Configure the web server to handle requests on the JSON route.
  server.onNotFound(handle_NotFound); // Configure the web server to do something if the requested resource was not found.
  server.begin(); //Start the web server
  Serial.println("Web server started");
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
  WiFi.mode( WIFI_STA ); // The MCU shall act as a WiFi consumer
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

bool read_data() {
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F;
  altitude = bme.readAltitude(SEA_LEVEL_PRESSURE_HPA);
  // Check if any reads failed and print an error if so.
  if (isnan(humidity) || isnan(temperature) || isnan(pressure) || isnan(altitude)) {
    return false;
  }
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
  return true;
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

void handle_OnConnect() {
  if (read_data()) {
    String html = "<!DOCTYPE html> <html>\n";
    html += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
    html += "<title>ESP8266 Weather Station</title>\n";
    html += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
    html += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
    html += "p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
    html += "</style>\n";
    html += "</head>\n";
    html += "<body>\n";
    html += "<div id=\"webpage\">\n";
    html += "<h1>ESP8266 Weather Station</h1>\n";
    html += "<p>Temperature: ";
    html += temperature;
    html += "&deg;C</p>";
    html += "<p>Humidity: ";
    html += humidity;
    html += "%</p>";W
    html += "<p>Pressure: ";
    html += pressure;
    html += "hPa</p>";
    html += "<p>Altitude: ";
    html += altitude;
    html += "m</p>";
    html += "</div>\n";
    html += "</body>\n";
    html += "</html>\n";
    server.send(200, "text/html", html);
  } else {
    Serial.println(readErrorMessage);
    server.send(500, "text/plain", readErrorMessage);
  }
}

void handle_OnConnectJson() {
  if (read_data()) {
    String json = "{\"temperature\": ";
    json += temperature;
    json += ", \"humidity\": ";
    json += humidity;
    json += ", \"pressure\": ";
    json += pressure;
    json += ", \"altitude\": ";
    json += altitude;
    json += "}";
    server.send(200, "application/json", json);
  } else {
    Serial.println(readErrorMessage);
    server.send(500, "text/plain", readErrorMessage);
  }
}

void loop() {
  server.handleClient();
}
