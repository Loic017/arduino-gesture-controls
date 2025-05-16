# Arduino Gesture Recognition
Gesture recognition on Arduino Nano 33 BLE Sense. Goal: Train and deploy initial PyTorch model, then implement model from scratch on Arduino.

## Current State
- Arduino accelerometer and gyroscope data collected for 2 gestures and background.
    - Tilt down
    - Tilt left
- Simple dense model trained and tested on PyTorch.
- Implementation of 2 layer dense model in C++ on the Arduino. (Does not need deployment with TFLite etc)

![alt text](image.png)

## How it works
1. Arduino proximity detects when figure is on arduino -> Triggers recording of accelerometer and gyroscope data
2. Once proximity detects finger is off of the arduino -> Stops recording data -> Computes inference

## To Do
- [ ] Deploy model to Arduino (from PyTorch)
- [x] Write model from scratch for Arduino (Move weights from PyTorch model)
    - [x] Test model
    - [ ] Can I optimize it in anyway?
- [ ] Make gestures trigger events
- [ ] Add more gestures
- [ ] Better method for size of sample

## Notes
- Need more (and more varied) background data -> Consistently thinks background/nothing is moving down


## My Takeaways/Practice Areas
- Working with Arduino
- Edge deployment
- C++ and writing ML in C++