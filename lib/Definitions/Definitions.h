#ifndef DEFINITIONS_H
#define DEFINITIONS_H

//RS485 communication enable pin (HIGH for transmitting, LOW for receiving)
#define SLAVE_EN 5

//Motor PWMs
#define MOTOR_L_PIN 4
#define MOTOR_R_PIN 7
#define MOTOR_A_PIN 3

//Battery cells voltage measurement
#define BAT A1

//Additional sensors
#define LED_PIN A4
// #define AdditionalSensor2 A3 <- reserved for gripper
#define AdditionalSensor3 A5
#define AdditionalSensor4 A6

//Speed Modes definitions
#define ARMED HIGH
#define DISARMED LOW

//RS485 transmition modes
#define DRONE_TRANSMISSION_MASTER HIGH
#define DRONE_TRANSMISSION_SLAVE LOW

//Timer for no message received
#define NO_MASSAGE_INTERVAL 2000

//Gripper pin definition
#define GRIPPER_PIN A3

#endif // DEFINITIONS_H