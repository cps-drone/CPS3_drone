#ifndef DEFINITIONS_H
#define DEFINITIONS_H

//RS485 communication enable pin (HIGH for transmitting, LOW for receiving)
#define SLAVE_EN 5

//Motor PWMs
#define MOTOR_L_PIN 4
#define MOTOR_R_PIN 7
#define MOTOR_A_PIN 3

//Battery cells voltage measurement
#define BAT1S A0
#define BAT2S A1

//Additional sensors
#define AdditionalSensor1 A2
#define AdditionalSensor2 A3
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

#endif // DEFINITIONS_H