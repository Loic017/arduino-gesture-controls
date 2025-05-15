import serial
import time
import numpy as np
import pickle

arduino_port = "/dev/ttyACM0"
baud_rate = 9600
output_file = "sensor_data.csv"

ser = serial.Serial(arduino_port, baud_rate, timeout=1)
time.sleep(2)


cols = [
    "Timestamp",
    "Proximity",
    "Accel_X",
    "Accel_Y",
    "Accel_Z",
    "Gyro_X",
    "Gyro_Y",
    "Gyro_Z",
]

sample_set = []

while True:
    try:
        line = ser.readline().decode("utf-8").strip()
        if line:
            if line == "Start":
                sample = []
            elif line == "Stop":
                print(f"Sample recorded: {np.array(sample).shape}")
                sample_set.append(np.array(sample))
                sample = []
            else:
                data = line.split(",")
                sample.append([time.time()] + [float(x) for x in data])

    except KeyboardInterrupt:
        print("Recording stopped.")
        print("Total samples recorded:", len(sample_set))
        with open("data/action_down_recording.pkl", "wb") as f:
            pickle.dump(sample_set, f)
        break
    except Exception as e:
        print(f"Error: {e}")
