import serial
import csv
import time

# Configuration
SERIAL_PORT = 'COM3'  # Change to your ESP32 port (e.g., /dev/ttyUSB0 on Linux)
BAUD_RATE = 115200
OUTPUT_FILE = 'motor_vibration_data.csv'
RECORD_SECONDS = 60

try:
    with serial.Serial(SERIAL_PORT, BAUD_RATE) as ser:
        print(f"Connected to {SERIAL_PORT}. Recording for {RECORD_SECONDS} seconds...")

        with open(OUTPUT_FILE, mode='w', newline='') as file:
            writer = csv.writer(file)

            start_time = time.time()
            while (time.time() - start_time) < RECORD_SECONDS:
                if ser.in_waiting > 0:
                    line = ser.readline().decode('utf-8').strip()
                    if line:
                        writer.writerow(line.split(','))

        print(f"Data saved to {OUTPUT_FILE}")
except Exception as e:
    print(f"Error: {e}")