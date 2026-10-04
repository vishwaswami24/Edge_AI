#include <YOUR_PROJECT_NAME_inferencing.h> // Replace with your exported library name
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

// Buffer to hold raw sensor data for one inference window
float features[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];

void setup() {
    Serial.begin(115200);
    
    if (!mpu.begin()) {
        Serial.println("Failed to find MPU6050 chip");
        while (1) { delay(10); }
    }
    
    mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
    mpu.setFilterBandwidth(MPU6050_BAND_44_HZ);
    
    Serial.println("Edge AI Predictive Maintenance Node Started.");
}

void loop() {
    // 1. Fill the feature buffer with sensor data
    for (int i = 0; i < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; i += 3) {
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);
        
        features[i + 0] = a.acceleration.x;
        features[i + 1] = a.acceleration.y;
        features[i + 2] = a.acceleration.z;
        
        // Wait exactly the interval expected by the ML model
        delay(1000 / EI_CLASSIFIER_FREQUENCY); 
    }

    // 2. Wrap buffer in a signal structure
    signal_t signal;
    int err = numpy::signal_from_buffer(features, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
    if (err != 0) {
        Serial.println("Failed to create signal from buffer");
        return;
    }

    // 3. Run the classifier
    ei_impulse_result_t result = { 0 };
    err = run_classifier(&signal, &result, false);
    if (err != EI_IMPULSE_OK) {
        Serial.printf("ERR: Failed to run classifier (%d)\n", err);
        return;
    }

    // 4. Print predictions
    Serial.println("Predictions:");
    for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        Serial.printf("  %s: %.5f\n", result.classification[i].label, result.classification[i].value);
        
        // Optional: Trigger physical alert (e.g., LED or Relay) if anomaly is > 80%
        if (strcmp(result.classification[i].label, "anomaly") == 0 && result.classification[i].value > 0.80) {
            Serial.println("WARNING: Mechanical Anomaly Detected! Maintenance Required.");
        }
    }
    Serial.println("---------------------------------");
}