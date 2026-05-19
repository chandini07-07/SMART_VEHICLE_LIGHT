#include <Wire.h> 
#include <Adafruit_Sensor.h> 
#include <Adafruit_MPU6050.h> 
#include <BluetoothSerial.h> 
 
#define FRONT_LED_PIN 15 
#define REAR_LED_PIN 2 
#define LIGHT_SENSOR_PIN 34  // ADC pin for LDR 
#define SPEED_SENSOR_PIN 4   // Digital pin for speed sensor 
#define BLUETOOTH_NAME "SmartBikeLight" 
 
// Create objects for sensors 
Adafruit_MPU6050 mpu; 
BluetoothSerial SerialBT; 
 
float ambientLightLevel = 0; int bikeSpeed = 0; int prevSpeed = 0; unsigned long lastSpeedCheck = 0; 
 
void setup() { 
  // Initialize Serial Monitor for debugging 
  Serial.begin(115200); 
   
  // Initialize LED pins 
  pinMode(FRONT_LED_PIN, OUTPUT);   pinMode(REAR_LED_PIN, OUTPUT); 
   
  // Initialize Bluetooth 
  SerialBT.begin(BLUETOOTH_NAME); 
  Serial.println("Bluetooth Initialized."); 
 
  // Initialize Light Sensor (LDR) 
  pinMode(LIGHT_SENSOR_PIN, INPUT); 
   
  // Initialize Speed Sensor (Reed Switch) 
  pinMode(SPEED_SENSOR_PIN, INPUT_PULLUP); 
   
  // Initialize MPU6050 (Accelerometer/Gyroscope)   Wire.begin();   if (!mpu.begin()) { 
    Serial.println("Could not find a valid MPU6050 sensor, check wiring."); 
          while (1); 
  } 
  Serial.println("MPU6050 Initialized."); 
 
  // Set LED light to an initial mode  digitalWrite(FRONT_LED_PIN, LOW); // Turn off front light initially   digitalWrite(REAR_LED_PIN, LOW);  // Turn off rear light initially 
} 
 
void loop() { 
  // Read ambient light level using LDR (Light Sensor)   ambientLightLevel = analogRead(LIGHT_SENSOR_PIN); 
// Read speed sensor data (calculate bike speed)   bikeSpeed = readSpeed(); 
   
  // Read accelerometer data (motion detection) 
  readMotion(); 
   
  // Adjust light intensity based on ambient light 
  adjustLightIntensity(); 
   
  // Send data to Bluetooth app (real-time data)   sendBluetoothData(); 
 
  delay(100);  // Wait a little before the next loop iteration 
} 
 
int readSpeed() {         static unsigned long lastMillis = 0;         static unsigned long pulseCount = 0; 
        static unsigned long lastPulseTime = 0; 
 
  if (digitalRead(SPEED_SENSOR_PIN) == LOW) {  // If magnet is near the sensor 
            pulseCount++;             lastPulseTime = millis(); 
  } 
 
 if (millis() - lastMillis >= 1000) {  // Every second, calculate speed    lastMillis = millis(); 
    // Assuming 1 pulse = 1 rotation and wheel circumference is known (e.g., 2.1 meters) 
    float speed = (pulseCount * 2.1) / (millis() - lastPulseTime) * 1000; // meters per second 
        pulseCount = 0;  // Reset pulse count         return int(speed * 3.6);  // Convert to km/h 
  }   return 0; 
} 
 
void readMotion() {     sensors_event_t a, g, temp; 
     mpu.getEvent(&a, &g, &temp); 
 
  // Detect sudden changes or motion to control emergency blinking or turning  if (abs(a.acceleration.x) > 9.0 || abs(a.acceleration.y) > 9.0) { 
// Example: Sudden movement (could be a fall or sudden stop)     digitalWrite(REAR_LED_PIN, HIGH);  // Emergency blink 
                  delay(500); 
                  digitalWrite(REAR_LED_PIN, LOW); 
           } 
} 
 
void adjustLightIntensity() { 
  // Adjust front light brightness based on ambient light level (simple threshold)   if (ambientLightLevel < 500) {  // Low light, turn on full brightness     analogWrite(FRONT_LED_PIN, 255);  // Max brightness 
  } else {  // Bright light, dim the front light 
    analogWrite(FRONT_LED_PIN, 50);  // Dimmed light 
} 
 
// Adjust rear light based on bike speed  if (bikeSpeed > 20) {  // If speed > 20 km/h, blink rear light             digitalWrite(REAR_LED_PIN, HIGH); 
             delay(200); 
             digitalWrite(REAR_LED_PIN, LOW); 
              delay(200); 
  } else { 
    Digital Write (REAR_LED_PIN, HIGH); // Steady light for low speed 
     } 
} 
 
void send Bluetooth Data () { 
    // Send speed, light level, and battery status to Bluetooth      String data = "Speed: " + String (bike Speed) + " km/h\n";       data += "Ambient Light Level: " + String (ambient Light Level) + "\n"; 
   
   // Send data to Bluetooth Serial 
      Serial BT. Print ln(data); 
} 
