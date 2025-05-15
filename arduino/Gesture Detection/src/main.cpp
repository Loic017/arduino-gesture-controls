#include <Arduino.h>
#include <Arduino_LSM9DS1.h>
#include <Arduino_APDS9960.h>
#include <state_dict.h>

bool recording = false;
int proximity = 250;
float ax, ay, az;
float gx, gy, gz;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Started");

  if (!APDS.begin()) {
    Serial.println("Failed to initialize APDS9960!");
    while (1);
  }

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println(" Hz");
}

void loop() {
  if (APDS.proximityAvailable()) {
    proximity = APDS.readProximity();
    // Serial.println(proximity);
  }

  // Start/Stop logic based on proximity threshold
  if (proximity < 2 && !recording) {
    recording = true;
    Serial.println("Start");
  } else if (proximity > 2 && recording) {
    recording = false;
    Serial.println("Stop");
  }

  if (recording) {
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(ax, ay, az);
    if (IMU.gyroscopeAvailable()) {
      IMU.readGyroscope(gx, gy, gz);
      // Send data as CSV: proximity, accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z
      Serial.print(proximity); Serial.print(",");
      Serial.print(ax); Serial.print(",");
      Serial.print(ay); Serial.print(",");
      Serial.print(az); Serial.print(",");
      Serial.print(gx); Serial.print(",");
      Serial.print(gy); Serial.print(",");
      Serial.println(gz);
    }
  }
}
}

float* dense_layer(float *input, int input_shape, int output_shape, int hidden_units, float *loaded_weights, float *loaded_biases) {
  float weights[hidden_units][input_shape];
  float biases[hidden_units];
  float output[output_shape];

  for (int i = 0; i < hidden_units; i++) {
    biases[i] = loaded_biases[i];

    for (int j = 0; j < input_shape; j++) {
      weights[i][j] = loaded_weights[i * input_shape + j];
    }
  }

  for (int i = 0; i < hidden_units; i++) {
    output[i] = 0;
    for (int j = 0; j < input_shape; j++) {
      output[i] += input[j] * weights[i][j];
    }
    output[i] += biases[i];
  }

  return output;
}

float* relu(float *input, int input_shape, int hidden_units) {
    for (int i = 0; i < 800; i++) {
        if (input[i] < 0) {
            input[i] = 0;
        }
    }
    return input;
}

int dense_model(float *input) {
  int input_shape = 800;
  int output_shape = 3;
  int hidden_units = 10;

  return predicted_class;
}
