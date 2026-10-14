# Requirements

## Product statement

This device is an orientation tracker and it displays the tilt angle of 1-Axis(Roll or Pitch) on an OLED. 
It is used to study the behaviour of the accelerometer/gyroscope and contains the same principle that is used in Automotive and Robotics to understand the positioning of the object and how the object behaves based on it.
It is built to hold stable tilt angle when bumped and avoid drift when the device is idle.

## Min Viable Product
- Read the Accelerometer and Gyroscope from MPU-6050.
- One Axis (Pitch or Roll)
- Combine the Accelerometer Reading and Gyroscope Readings using Complementary Filter.
- Angle displayed as a number on OLED.
- Accelerometer Offset measured once on a level surface and stored as Constants.
- Gyroscope Bias Calibration at Startup.
- Debugging over UART at 10Hz.


## Stretch
- Calibrations to be stored in FLASH as permanent method.
- Use of Kalman Filter(LKF and EKF), Mahony Filter.
- Porting into a Zephyr Project.


## Future Versions
- Two-Axis(Pitch and Roll) Orientation Tracking.
- Move towards a 9-Axis IMU that supports 3-Axis Accelerometer, 3-Axis Gyroscope, and 3-Axis Magnetometer.
- Graphics(to look into the possibility of making use of STM32H7S78-DK Board with LVGL).


## Out of Scope
- Yaw not possible with Accelerometer and Gyroscope(until 9-Axis IMU used).

## Acceptance Criteria
No             Criterion                  How we will measure
1.        Flat position reads 0°±1      Board placed on a flat surface with our phone and OLED is read. 
2.     30°tilt within ±2° of reference  Board inclined to 30° with a protractor, cross-checked with a phone.
3.    Drift over 2mins stays within ±1° Board will be fixed to the desk and we note the start and end angle.
4.  Testing the Accelerometer with taps We will tap the desk such that it vibrates over which MPU-6050 sits and observe the readings. 
