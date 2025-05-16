#include <Arduino.h>
#include <Arduino_LSM9DS1.h>
#include <Arduino_APDS9960.h>
#include <state_dict.h>

int dense_model(float *input);

bool recording = false;
int proximity = 250;
int prev_proximity = 250;
float ax, ay, az;
float gx, gy, gz;

static float features[100][8];
static int sample_index = 0;

unsigned long last_action_time = 0; // Track the last time an action was triggered
const unsigned long debounce_delay = 200; // 200 ms debounce delay

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
  }

  // unsigned long current_time = millis();

  if (prev_proximity >= 20 && proximity < 20 && !recording) {
    // Start recording
    recording = true;
    Serial.println("Start");
    sample_index = 0;
    memset(features, 0, sizeof(features));
  } else if (proximity >= 20 && recording) {
    // Stop recording
    recording = false;
    Serial.println("Stop");

    // Perform inference
    if (sample_index >= 100) {
      Serial.print("Sample index size: ");
      Serial.println(sample_index);
      float flattened_features[600];
      for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 8; j++) {
          flattened_features[i * 8 + j] = features[i][j];
        }
      }
      Serial.println("Inference");
      dense_model(flattened_features);
      delay(250);
    // } 
    // else if (sample_index < 50) {
    //   Serial.println("Size of sample is too small.");
    //   memset(features, 0, sizeof(features));
    //   sample_index = 0;
    } else {
      for (int i = sample_index; i < 100; i++) {
        for (int j = 0; j < 8; j++) {
          features[i][j] = 0;
        }
      }
      float flattened_features[600];
      for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 8; j++) {
          flattened_features[i * 8 + j] = features[i][j];
        }
      }
      Serial.println("Inference");
      dense_model(flattened_features);
      delay(250);
    }
  }

  if (recording) {
    // if (sample_index == 100) {
    //   Serial.println("Size of samples reached 100");
    //   recording = false;
    // }
    if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(ax, ay, az);
      if (IMU.gyroscopeAvailable()) {
        IMU.readGyroscope(gx, gy, gz);

        // Collect features in the specified order
        features[sample_index][0] = ax;
        features[sample_index][1] = ay;
        features[sample_index][2] = az;
        features[sample_index][3] = gx;
        features[sample_index][4] = gy;
        features[sample_index][5] = gz;

        sample_index++;
      }
    }
  }

  prev_proximity = proximity;
}

//     if (inference) {
//         // If we have collected 100 samples, process them
//       if (sample_index == 100) {
//         // Flatten the features array to shape 800
//         float flattened_features[800];
//         for (int i = 0; i < 100; i++) {
//           for (int j = 0; j < 8; j++) {
//         flattened_features[i * 8 + j] = features[i][j];
//           }
//         }
//         Serial.println("Inference");
//         dense_model(flattened_features);
//         memset(features, 0, sizeof(features));
//         sample_index = 0;
//         inference = false;

// }
//     } else {
//       // Buffer out the sample to size 800 with 0s
//       for (int i = sample_index; i < 100; i++) {
//         for (int j = 0; j < 8; j++) {
//           features[i][j] = 0;
//         }
//       }
//       float flattened_features[800];
//         for (int i = 0; i < 100; i++) {
//           for (int j = 0; j < 8; j++) {
//         flattened_features[i * 8 + j] = features[i][j];
//           }
//         }
//       Serial.println("Inference");
//       dense_model(flattened_features);
//       memset(features, 0, sizeof(features));
//       sample_index = 0;
//       inference = false;
//     }
// }

// Simple weighted sum etc etc
// Weights are loaded in from .h file
float* dense_layer(float *input, int input_shape, int output_shape, int hidden_units, const float *loaded_weights, const float *loaded_biases) {
  float* weights = new float[hidden_units * input_shape];
  float* biases = new float[hidden_units];
  float* output = new float[output_shape];
  Serial.println("  Set Arrays");

  for (int i = 0; i < hidden_units; i++) {
    biases[i] = loaded_biases[i];

    for (int j = 0; j < input_shape; j++) {
      weights[i * input_shape + j] = loaded_weights[i * input_shape + j];
    }
  }
  Serial.println("  Set Params");

  for (int i = 0; i < hidden_units; i++) {
    output[i] = 0;
    for (int j = 0; j < input_shape; j++) {
      output[i] += input[j] * weights[i * input_shape + j];
    }
    output[i] += biases[i];
  }

  delete[] weights;
  delete[] biases;
  return output;
}

float relu(float input) {
    if (input < 0) {
            input = 0;
    }
    return input;
}

int predicted_class(float *input) {
  int max_idx = 0;
  float max_val = 0;
  for (int i = 0; i < 3; i++) {
    Serial.print(input[i]);
    Serial.print(",");
    if (input[i] > max_val) {
      max_idx = i;
    }
  }
  return max_idx;
}

int dense_model(float *input) {
  Serial.println("Layer 1");
  float* layer_1 = dense_layer(input, 600, 10, 10, layer_1_weights, layer_1_bias);

  Serial.println("ReLU");
  for (int i = 0; i < 10; i++) {
    layer_1[i] = relu(layer_1[i]);
  }

  Serial.println("Layer 2");
  float* layer_2 = dense_layer(layer_1, 10, 3, 3, layer_2_weights, layer_2_bias);

  Serial.println("Output");
  int predicted_idx = predicted_class(layer_2);
  String c = "";

  switch (predicted_idx) {
    case 0:
      c = "background";
      break;
    case 1:
      c = "movement left";
      break;
    case 2:
      c = "movement down";
      break;
    default:
      c = "unknown";
      break;
}

  Serial.println("");
  Serial.println(predicted_idx);
  Serial.println(c);

  return predicted_idx;
}

