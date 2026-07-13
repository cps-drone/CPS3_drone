#include "Gripper.h"


void gripper_init(gripper_t *gripper){
    gripper->motor.attach(GRIPPER_PIN);
    gripper->direction = 90; // Set the direction to the neutral position
    gripper->motor.write(gripper->direction);
    gripper->counterOpen = 0;
    gripper->counterClose = 0;
}

void gripper_move(gripper_t *gripper){
    
    switch (gripper->command) // Use the new command member
    {
    case 234:
        gripper->counterClose++;
        gripper->counterOpen = 0;
        if(gripper->counterClose > 0){
            gripper->direction = 0; // Set direction to closed position
        }
        break;
    
    case 567:
        gripper->direction = 90; // Set direction to neutral position
        gripper->counterClose = 0;
        gripper->counterOpen = 0;
        break;

    case 890:
        gripper->counterOpen++;
        gripper->counterClose = 0;
        if(gripper->counterOpen > 0){
            gripper->direction = 180; // Set direction to closed position
        }
        break;

    default:
        break;
    }

    gripper->motor.write(gripper->direction);
}