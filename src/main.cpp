#include <Arduino.h>
#include <homepage.h>
#include <WiFiManager.h>
/* #ifdef ESP32
#include <WiFi.h>
#include <SPIFFS.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include "ESPAsyncWebServer.h" */

// WiFi credentials
const char* ssid = "YourWiFiSSID";
const char* password = "YourWiFiPassword";
bool isFilling = false;
bool isPurging = false;

// RELAY PINS 
const int LIQUID_PIN = 5;  // D1 ON ESP8266
const int GAS_PIN = 4;  // D2 on ESP8266
const int FILL_BUTTON_PIN = 14;  // D5 on ESP8266
const float MAX_FILL_PRESSURE = 0.52; //The pressure at desired fill volume
// Constants
const int PRESSURE_SENSOR_PIN = A0;  // ADC pin on ESP8266
const float V_REF = 3.3;    // ESP8266 ADC reference voltage
const float V_MIN = 0.5;    // Minimum voltage output of the sensor
const float V_MAX = 3.3;    // Maximum voltage output of the sensor
const float P_MIN = 0.0;    // Minimum pressure in MPa
const float P_MAX = 0.35;    // Maximum pressure in MPa
const int ADC_RESOLUTION = 1024;  // ESP8266 has 10-bit ADC (0-1023)
const float MPA_TO_PSI = 145.038; // Conversion factor
const int PRESSURE_SENSOR_OFFSET = 171;  // ESP8266 has 10-bit ADC (0-1023)



// Variables
float rawVoltage = 0.0;
float pressure = 0.0;
float calibrationFactor = 1.0;
unsigned long currentMillis = 0;

/* AsyncWebServer server(80); */

String processor(const String& var){
  if(var == "PRESSURE"){
    return String(pressure * MPA_TO_PSI, 2);
  }
  return String();
}

float map(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float getCurrentSensorPressure() {  
  // Convert ADC reading to voltage (0-3.3V range)
  rawVoltage = ((float)(analogRead(PRESSURE_SENSOR_PIN) - PRESSURE_SENSOR_OFFSET)) * V_REF / ADC_RESOLUTION;
  
  // Map the 0-3.3V range to the sensor's 0.5-4.5V range
  float sensorVoltage = map(rawVoltage, 0, V_REF, V_MIN, V_MAX);
  // Calculate pressure
  pressure = map(sensorVoltage, V_MIN, V_MAX, P_MIN, P_MAX);
  float pressurePSI = pressure * MPA_TO_PSI * calibrationFactor;

  return pressurePSI;
}

void performFill() {
  while (getCurrentSensorPressure() >= MAX_FILL_PRESSURE)
  {
    if(!isFilling) {
      Serial.println("Start Filling!!!");
      digitalWrite(LIQUID_PIN, HIGH);
      isFilling = true;
    } else {
      Serial.println("filling....");
    }
  }
  
  if(isFilling) {
    Serial.println("Filling Complete!!!!");
    digitalWrite(LIQUID_PIN, LOW);
    isFilling = false;
  }
  
}

void performPurge() {
  isPurging = true;
  digitalWrite(GAS_PIN, HIGH);
  delay(1000);
  digitalWrite(GAS_PIN, LOW);
  isPurging = false;
}

void purge() {
  if (!isFilling && !isPurging) {
    performPurge();
    isPurging = false;
  }
}

void fill() {
  purge();
  if (!isFilling) {
    performFill();
  }
}



/* void saveCalibrationFactor() {
  File file = SPIFFS.open("/calibration.txt", FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  file.println(calibrationFactor);
  file.close();
}

void loadCalibrationFactor() {
  File file = SPIFFS.open("/calibration.txt", FILE_READ);
  if (!file) {
    Serial.println("No calibration file found");
    return;
  }
  String factorStr = file.readStringUntil('\n');
  calibrationFactor = factorStr.toFloat();
  file.close();
  Serial.println("Loaded calibration factor: " + String(calibrationFactor));
} */

void setup() {
  Serial.begin(115200);
  
  //SET RELAY PIN MODEs
  pinMode(LIQUID_PIN, OUTPUT);
  pinMode(GAS_PIN, OUTPUT);
  pinMode(FILL_BUTTON_PIN, INPUT_PULLUP);
  
  /* // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An error occurred while mounting SPIFFS");
    return;
  }

  // Load calibration factor from file
  loadCalibrationFactor();
 */
 /*  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println(WiFi.localIP()); */

  /* // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", homepage_html, processor);
  });

  // Route to get current pressure reading
  server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(pressure * MPA_TO_PSI, 2));
  });

  // Route for calibration
  server.on("/calibrate", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("factor")) {
      calibrationFactor = request->getParam("factor")->value().toFloat();
       saveCalibrationFactor();
      request->send(200, "text/plain", "Calibration factor updated");
    } else {
      request->send(400, "text/plain", "Missing calibration factor");
    }
  });

  server.begin(); */

 /*  analogReadResolution(12); */
}

void loop() {
  currentMillis = millis();

  if (!isFilling && !isPurging && digitalRead(FILL_BUTTON_PIN) == LOW ) {
    Serial.println("Fill initiated by button press");
    //fill();
  }
  int rawValue = analogRead(PRESSURE_SENSOR_PIN);
  
  // Convert ADC reading to voltage (0-3.3V range)
  rawVoltage = ((float)(rawValue- PRESSURE_SENSOR_OFFSET)) * V_REF / ADC_RESOLUTION;
  
  // Map the 0-3.3V range to the sensor's 0.5-4.5V range
  float sensorVoltage = map(rawVoltage, 0, V_REF, V_MIN, V_MAX);
  
  // Calculate pressure
  pressure = map(sensorVoltage, V_MIN, V_MAX, P_MIN, P_MAX);
  
  float pressurePSI = pressure * MPA_TO_PSI * calibrationFactor;
  
  Serial.print("Raw ADC: ");
  Serial.print(rawValue);
  Serial.print(", Offset Raw ADC: ");
  Serial.print(rawValue - PRESSURE_SENSOR_OFFSET);
  Serial.print(", ESP8266 Voltage: ");
  Serial.print(rawVoltage, 3);
  Serial.print("V, Mapped Sensor Voltage: ");
  Serial.print(sensorVoltage, 3);
  Serial.print("V, Pressure: ");
  Serial.print(pressurePSI, 2);
  Serial.println(" PSI");
  
  delay(1000);
}
