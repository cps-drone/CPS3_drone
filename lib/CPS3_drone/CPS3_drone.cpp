#include "CPS3_drone.h"

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
    CPS3->Data.message_for_transmit = "";   // Set the transmit message as empty
    CPS3->Data.message_received = "";       // Set the received message as empty

    pinMode(SLAVE_EN, OUTPUT); // Master mode pin as output

    pinMode(LED_PIN, OUTPUT); // Set LED pin as output
    digitalWrite(LED_PIN, LOW); // Ensure LEDs are off at startup

    //Set the battery sensors as inputs
    pinMode(BAT, INPUT);

    //Set baud rate for serial communication
    Serial.begin(115200);

    //Wait once for initialization ESCs
    delay(7000);
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
void get_steering(cps3_drone_t *CPS3, gripper_t *gripper) {
    digitalWrite(SLAVE_EN, LOW); // Set master mode
    delay(10);

    char buffer[23]; // Buffer to store the incoming message
    int index = 0;   // Index to track the position in the buffer
    unsigned long startTime = millis(); // Start the timer for timeout

    // Read incoming data with a timeout of 20 ms
    while (millis() - startTime < 20) {
        if (Serial.available() > 0) {
            char incomingByte = Serial.read(); // Read one byte from the serial buffer
            if (index < sizeof(buffer) - 1) {  // Ensure buffer does not overflow
                buffer[index++] = incomingByte;
            }
            if (incomingByte == 'E') { // Stop reading if the message ends with 'E'
                break;
            }
        }
    }
    buffer[index] = '\0'; // Null-terminate the buffer to make it a valid string

    // Process the received message
    String message = String(buffer);
    Serial.print("Received message: ");
    Serial.println(message);
    // Left motor speed
    int L_idx = message.indexOf('L');
    if (L_idx != -1) {
        CPS3->MotorL.speed = message.substring(L_idx + 1).toInt();
    }

    // Right motor speed
    int R_idx = message.indexOf('R');
    if (R_idx != -1) {
        String R_str = message.substring(R_idx + 1);
        for (size_t i = 0; i < R_str.length(); ++i) {
            if (!isDigit(R_str.charAt(i))) {
                R_str = R_str.substring(0, i);
                break;
            }
        }
        CPS3->MotorR.speed = R_str.toInt();
    }

    // Vertical motor speed
    int A_idx = message.indexOf('A');
    if (A_idx != -1) {
        String A_str = message.substring(A_idx + 1);
        for (size_t i = 0; i < A_str.length(); ++i) {
            if (!isDigit(A_str.charAt(i))) {
                A_str = A_str.substring(0, i);
                break;
            }
        }
        CPS3->MotorA.speed = A_str.toInt();
    }

    // LEDs state
    int l_idx = message.indexOf('D');
    if (l_idx != -1) {
        String l_str = message.substring(l_idx + 1);
        for (size_t i = 0; i < l_str.length(); ++i) {
            if (!isDigit(l_str.charAt(i))) {
                l_str = l_str.substring(0, i);
                break;
            }
        }
        CPS3->LEDs_state = l_str.toInt();
    }

    // Gripper steering
    int G_idx = message.indexOf('G');
    if (G_idx != -1) {
        String G_str = message.substring(G_idx + 1);
        for (size_t i = 0; i < G_str.length(); ++i) {
            if (!isDigit(G_str.charAt(i))) {
                G_str = G_str.substring(0, i);
                break;
            }
        }
        gripper->command = G_str.toInt();
        gripper_move(gripper);
    }

    // Write motor speeds to the motor Servo objects
    CPS3->MotorL.motor.write(CPS3->MotorL.speed);
    CPS3->MotorR.motor.write(CPS3->MotorR.speed);
    CPS3->MotorA.motor.write(CPS3->MotorA.speed);

    digitalWrite(SLAVE_EN, HIGH); // Set master mode
    send_measurement_data(CPS3);
}

/*
    * Function sends measured battery voltage to the remote.
    * After the message is sent it sets master mode as SLAVE.
*/
void send_measurement_data(cps3_drone_t *CPS3) {
    digitalWrite(SLAVE_EN, HIGH); // Set RS485 to transmission mode
    delay(10); // Ensure the mode is switched before sending the message

    // Format the message as a String
    String send_message = "V" + String(CPS3->Battery.voltage, 2) + "E"; // Format voltage with 2 decimal places

    // Send the message
    Serial.print(send_message); // Send the formatted String
    Serial.flush(); // Ensure the message is fully transmitted
    delay(10); // Wait to ensure the message is sent

    digitalWrite(SLAVE_EN, LOW); // Set RS485 back to receive mode
}
