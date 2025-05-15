import torch
import pickle
import torch.nn as nn
import torch.nn.functional as F
import torch.utils.data as data
import numpy as np
import os
from sklearn.preprocessing import StandardScaler
import serial
import time

labels = ["background_recording", "action_left_recording", "action_down_recording"]


class SimpleNN(nn.Module):
    def __init__(self, input_size):
        super(SimpleNN, self).__init__()
        self.fc1 = nn.Linear(input_size, 10)
        self.fc2 = nn.Linear(10, 3)

    def forward(self, x):
        x = F.relu(self.fc1(x))
        x = self.fc2(x)
        return x


model = SimpleNN(input_size=800)
model.load_state_dict(torch.load("../models/simple_nn_model.pth"))

scaler = StandardScaler()
with open("../models/scaler.pkl", "rb") as f:
    scaler = pickle.load(f)

arduino_port = "/dev/ttyACM0"
baud_rate = 9600
output_file = "sensor_data.csv"

ser = serial.Serial(arduino_port, baud_rate, timeout=1)
time.sleep(2)


def crop_sample(data, size=(100, 8)):
    if data.shape[0] < size[0]:
        new_data = np.zeros((size[0], size[1]))
        new_data[: data.shape[0], : data.shape[1]] = data
    else:
        new_data = data[: size[0], : size[1]]
    return new_data


sample = []
while True:
    try:
        line = ser.readline().decode("utf-8").strip()
        if line:
            if line == "Start":
                sample = []
            elif line == "Stop":
                print(f"Sample recorded: {np.array(sample).shape}")
                sample_set = np.array(sample)
                print(f"Sample shape before cropping: {sample_set.shape}")
                sample_set = crop_sample(sample_set)
                print(f"Sample shape after cropping: {sample_set.shape}")
                sample_set = sample_set.reshape(
                    sample_set.shape[0] * sample_set.shape[1]
                )
                print(f"Sample shape after reshaping: {sample_set.shape}")
                sample_set = np.expand_dims(sample_set, axis=0)
                sample_set = scaler.transform(sample_set)

                sample_set = torch.tensor(sample_set, dtype=torch.float32)
                with torch.no_grad():
                    output = model(sample_set)
                    _, predicted = torch.max(output, 1)
                    print(f"Predicted class: {labels[predicted.item()]}")

                sample = []
            else:
                data = line.split(",")
                sample.append([time.time()] + [float(x) for x in data])

    except KeyboardInterrupt:
        break
