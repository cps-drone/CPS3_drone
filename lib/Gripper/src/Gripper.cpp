#include "Gripper.h"


void gripper_init(gripper_t *gripper){
    gripper->motor.attach(GRIPPER_PIN);
    gripper->direction = 90; // Set the direction to the neutral position
    gripper->motor.write(gripper->direction);
}

void gripper_move(gripper_t *gripper){
    
    switch (gripper->command) // Use the new command member
    {
    case 0:
         gripper->direction = 0; // Set direction to closed position
        break;
    
    case 1:
        gripper->direction = 90; // Set direction to neutral position
        break;

    case 2:
        gripper->direction = 180; // Set direction to opened position
        break;

    default:
        break;
    }

    gripper->motor.write(gripper->direction);
}