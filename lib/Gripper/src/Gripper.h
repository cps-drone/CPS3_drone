#ifndef GRIPPER_H
#define GRIPPER_H

#include "Servo.h"
#include "Arduino.h"
#include "Definitions.h"


typedef struct gripper_s{
    Servo motor; // Servo object for the gripper
    /*
     *  0 - jaws closing
     *  90 - jaws in neutral position
     *  180 - jaws opening
     */
    int direction; // Position of the gripper (0-180 degrees)
    int command; // Command for the gripper (0 for closing, 1 for neutral, 2 for opening)
} gripper_t;

/*
 * Function initializes the gripper by attaching the servo to the specified pin and setting the initial direction to neutral (90 degrees).
 * It also sets the servo to the neutral position.
 */
void gripper_init(gripper_t *gripper);

/*
 * Function moves the gripper in the specified direction.
 * The direction parameter can be:
 *  - 0 for closing the jaws
 *  - 90 for neutral position
 *  - 180 for opening the jaws
 * The function writes the corresponding angle to the servo to move the gripper.
 */
void gripper_move(gripper_t *gripper);

#endif // GRIPPER_H