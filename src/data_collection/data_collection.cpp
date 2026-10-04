#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

// Set sampling frequency (e.g., 100 Hz = 10ms delay)
const int SAMPLE_DELAY_MS = 10; 

void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10); 

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) { delay(10); }
  }
  
  // Optimize settings for vibration detection
  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
  mpu.setFilterBandwidth(MPU6050_BAND_44_HZ);
  
  // Print CSV Header for Edge Impulse Data Forwarder
  Serial.println("accX,accY,accZ");
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Print data in comma-separated format
  Serial.print(a.acceleration.x);
  Serial.print(",");
  Serial.print(a.acceleration.y);
  Serial.print(",");
  Serial.println(a.acceleration.z);

  delay(SAMPLE_DELAY_MS);
}