#ifndef CPS3_DRONE_H
#define CPS3_DRONE_H

#include <Servo.h>
#include <Wire.h>
#include <Arduino.h>
#include "Definitions.h"
#include <EEPROM.h>
#define EEPROM_MODE_ADDR 0


/*
    * Motor structure
    * Contains a Servo object for controlling the motor and an integer for speed.
    * The Servo.h library is used to create and control the motor objects.
    * Speed is represented as an angle between 0 and 180 degrees.
    * The speed is set to 90 degrees for neutral position.
*/
typedef struct motor_s{
    Servo motor; // Servo object for the motor
    int speed; // Speed of the motor
} motor_t;

/*    
    * Battery structure
    * Contains raw values for 1S and 2S batteries and their corresponding voltages.
    * The raw values are read from the analog pins and converted to voltage.
    * The voltage is calculated based on the ADC resolution (1024 bits) and reference voltage (~4.9V).
*/
typedef struct battery_s{
    int raw_value_1S; // Raw value of 1S battery
    int raw_value_2S; // Raw value of 2S battery
    float voltage_1S; // Voltage of 1S battery
    float voltage_2S; // Voltage of 2S battery
} battery_t;

/*
    * Data structure
    * Contains String for the data to transmit and for data received from the remote.
    * It also has a bool variable for Master mode variable
*/
typedef struct data_s{
    String message_for_transmit; // String for data to transmit
    String message_received; // String for data received from remote
    bool master_mode; // Flag indicating if the drone is in master mode
    long no_massage_timer_current_time;
    long no_massage_timer_previous_time;
} data_t;

/* 
    * This structure contains 3 motor objects 1 battery object, and one data object. Each motor object has Servo object,
    * and its speed. Battery struct object contains raw voltages and calculated voltages of each battery cell. 
    * Data struct object contains two Strings, one for the data to transmit and one for data received from the remote;
    * It also contains boolean Master mode flag.
*/
typedef struct cps3_drone_s{
    motor_t MotorL; // Left motor
    motor_t MotorR; // Right motor
    motor_t MotorA; // Vertical motor
    battery_t Battery; // Battery measurements
    data_t Data;

} cps3_drone_t;

/*
    * Function sets transmission mode to the mode,
    * described by passed to function parameter "transmission_mode".
    * Fuction contains small 50 miliseconds delay after modes switch.
*/
void set_CPS3_transmission_mode(cps3_drone_t *CPS3, bool transmission_mode);

/*
    * Fuction initialises all of the drone variables with 0 (for int/float),
    * with 90 (for motors speed), with empty string (for Strings).
    * It also attaches the proper Arduino pins to the motors, and sets
    * the Master Flag as true (after connecting the Weipu the remote is 
    * waiting for the drone votage data measurement)
*/
void CPS3_drone_init(cps3_drone_t *CPS3);

/*
    * Fuction reads data from the remote. It decodes the data,
    * sets motors speed, and sets the Master mode flag as MASTER.
*/
void get_motor_speeds(cps3_drone_t *CPS3);

/*
    * Function measures the battery cells raw voltages,
    * and calculates the correct battery cells voltages.
*/
void get_battery_voltages(cps3_drone_t *CPS3);

/*
    * Function sends measured 1S and 2S voltages to the remote.
    * After the message is sent it sets master mode as SLAVE.
*/
void send_measurement_data(cps3_drone_t *CPS3);


#endif // CPS3_DRONE_H