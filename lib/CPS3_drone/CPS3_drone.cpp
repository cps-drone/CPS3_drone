#include "CPS3_drone.h"

/*
    * Function sets transmission mode to the mode,
    * described by passed to function parameter "transmission_mode".
    * Fuction contains small 50 miliseconds delay after modes switch.
*/
void set_CPS3_transmission_mode(cps3_drone_t *CPS3, bool transmission_mode){
    CPS3->Data.master_mode = transmission_mode;
    digitalWrite(SLAVE_EN, CPS3->Data.master_mode);
}

/*
    * Fuction initialises all of the drone variables with 0 (for int/float),
    * with 90 (for motors speed), with empty string (for Strings).
    * It also attaches the proper Arduino pins to the motors, and sets
    * the Master Flag as true (after connecting the Weipu the remote is 
    * waiting for the drone votage data measurement)
*/
void CPS3_drone_init(cps3_drone_t *CPS3){
    // Initialize motors
    CPS3->MotorL.motor.attach(MOTOR_L_PIN);
    CPS3->MotorR.motor.attach(MOTOR_R_PIN);
    CPS3->MotorA.motor.attach(MOTOR_A_PIN);
    // Set initial motor speeds to neutral position (90)
    CPS3->MotorL.speed = 90;    // Neutral position for left motor
    CPS3->MotorR.speed = 90;    // Neutral position for right motor
    CPS3->MotorA.speed = 90;    // Neutral position for vertical motor
    CPS3->MotorL.motor.write(CPS3->MotorL.speed);
    CPS3->MotorR.motor.write(CPS3->MotorR.speed);
    CPS3->MotorA.motor.write(CPS3->MotorA.speed);
    CPS3->LEDs_state = false; // LEDs off by default

    // Initialize battery measurements with 0
    CPS3->Battery.raw_value = 0;
    CPS3->Battery.voltage = 0.0f;
    // Set master mode flag and set the massages as empty
    CPS3->Data.master_mode = true;          // Default to master mode
    CPS3->Data.message_for_transmit = "";   // Set the transmit message as empty
    CPS3->Data.message_received = "";       // Set the received message as empty

    // Master mode pin as output
    pinMode(SLAVE_EN, OUTPUT);
    set_CPS3_transmission_mode(CPS3, DRONE_TRANSMISSION_MASTER);

    pinMode(LED_PIN, OUTPUT); // Set LED pin as output
    digitalWrite(LED_PIN, LOW); // Ensure LEDs are off at startup

    //Set the battery sensors as inputs
    pinMode(BAT, INPUT);

    //Set baud rate for serial communication
    Serial.begin(9600);

    //Wait once for initialization ESCs
    delay(7000);

    //Timer variables init
    CPS3->Data.no_massage_timer_current_time = 0;
    CPS3->Data.no_massage_timer_previous_time = 0;
}

void set_LEDs_state(cps3_drone_t *CPS3) {
    if (CPS3->LEDs_state) {
        digitalWrite(LED_PIN, HIGH); // Turn on LEDs
    } else {
        digitalWrite(LED_PIN, LOW);  // Turn off LEDs
    }
}


/*
    * Function measures the battery raw voltage,
    * and calculates the correct battery voltage.
*/
void get_battery_voltage(cps3_drone_t *CPS3){
    // Read raw values from ADC
    CPS3->Battery.raw_value = analogRead(BAT); // Measure raw value of the battery voltage
    // Calculate voltages from raw values
    CPS3->Battery.voltage = (CPS3->Battery.raw_value / 1023.0) * 4.9 * 2;   // Calculate battery voltage
}

/*
    * Fuction reads data from the remote. It decodes the data,
    * sets motors speed, and sets the Master mode flag as MASTER.
*/
void get_steering(cps3_drone_t *CPS3, gripper_t *gripper){

    if (Serial.available() > 0 && CPS3->Data.master_mode == DRONE_TRANSMISSION_SLAVE) {

        set_CPS3_transmission_mode(CPS3, DRONE_TRANSMISSION_SLAVE);

        CPS3->Data.message_received = Serial.readStringUntil('\n');

        // speed for the left motor
        int L_idx = CPS3->Data.message_received.indexOf('L');
        if (L_idx != -1) {
            CPS3->MotorL.speed = CPS3->Data.message_received.substring(L_idx + 1).toInt();
        }

        // speed for the right motor
        int R_idx = CPS3->Data.message_received.indexOf('R');
        if (R_idx != -1) {
            String R_str = CPS3->Data.message_received.substring(R_idx + 1);
            for (size_t i = 0; i < R_str.length(); ++i) {
                if (!isDigit(R_str.charAt(i))) {
                    R_str = R_str.substring(0, i);
                    break;
                }
            }
            CPS3->MotorR.speed = R_str.toInt();
        }

        // speed for the vertical motor
        int A_idx = CPS3->Data.message_received.indexOf('A');
        if (A_idx != -1) {
            String A_str = CPS3->Data.message_received.substring(A_idx + 1);
            for (size_t i = 0; i < A_str.length(); ++i) {
                if (!isDigit(A_str.charAt(i))) {
                    A_str = A_str.substring(0, i);
                    break;
                }
            }
            CPS3->MotorA.speed = A_str.toInt();
        }

        // master mode transmission flag
        int M_idx = CPS3->Data.message_received.indexOf('M');
        if (M_idx != -1) {
            String M_str = CPS3->Data.message_received.substring(M_idx + 1);
            for (size_t i = 0; i < M_str.length(); ++i) {
                if (!isDigit(M_str.charAt(i))) {
                    M_str = M_str.substring(0, i);
                    break;
                }
            }
            set_CPS3_transmission_mode(CPS3, M_str.toInt());

            CPS3->Data.no_massage_timer_previous_time = CPS3->Data.no_massage_timer_current_time;
        }

        // LEDs state flag
        int l_idx = CPS3->Data.message_received.indexOf('l');
        if (l_idx != -1) {
            String l_str = CPS3->Data.message_received.substring(l_idx + 1);
            for (size_t i = 0; i < l_str.length(); ++i) {
                if (!isDigit(l_str.charAt(i))) {
                    l_str = l_str.substring(0, i);
                    break;
                }
            }
            CPS3->LEDs_state = l_str.toInt();
        }


        // gripper steering
        int G_idx = CPS3->Data.message_received.indexOf('G');
        if (G_idx != -1) {
            String G_str = CPS3->Data.message_received.substring(G_idx + 1);
            for (size_t i = 0; i < G_str.length(); ++i) {
                if (!isDigit(G_str.charAt(i))) {
                    G_str = G_str.substring(0, i);
                    break;
                }
            }
            gripper->command = G_str.toInt();
            gripper_move(gripper);
        }

        // write motor speeds to the motor Servo objects
        CPS3->MotorL.motor.write(CPS3->MotorL.speed);
        CPS3->MotorR.motor.write(CPS3->MotorR.speed);
        CPS3->MotorA.motor.write(CPS3->MotorA.speed);
    }
    /*
        * Handling the case when there is no received messages from remote.
        * For example when the remote is turned off by mistake during swimming.
    */
    else if(!Serial.available() && (CPS3->Data.no_massage_timer_current_time - CPS3->Data.no_massage_timer_previous_time >= NO_MASSAGE_INTERVAL)){
        set_CPS3_transmission_mode(CPS3, DRONE_TRANSMISSION_MASTER);
        CPS3->Data.no_massage_timer_previous_time = CPS3->Data.no_massage_timer_current_time;
        get_battery_voltage(CPS3);
        send_measurement_data(CPS3);
    }
}

/*
    * Function sends measured battery voltage to the remote.
    * After the message is sent it sets master mode as SLAVE.
*/
void send_measurement_data(cps3_drone_t *CPS3){

    if(CPS3->Data.master_mode == DRONE_TRANSMISSION_MASTER){
        CPS3->Data.master_mode = DRONE_TRANSMISSION_SLAVE;
        CPS3->Data.message_for_transmit =   
                "V" + String(CPS3->Battery.voltage, 2) + "," +   // V like battery voltage
                "M" + String(CPS3->Data.master_mode) + ",";         // M like Master mode
                
        Serial.println(CPS3->Data.message_for_transmit);
        Serial.flush();
        set_CPS3_transmission_mode(CPS3, CPS3->Data.master_mode);
    }
}