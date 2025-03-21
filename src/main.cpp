#include <Servo.h>
#include <Arduino.h>

#define SLAVE_EN 5

#define MOTOR_L_PIN 4
#define MOTOR_R_PIN 3
#define MOTOR_A_PIN 2

#define BatterySensor A0

// Mode definitions
#define ARMED HIGH
#define DISARMED LOW

Servo MotorL;
Servo MotorR;
Servo MotorA;

int SpeedL = 0;
int SpeedR = 0;
int SpeedA = 0;

float BatteryVoltage = 0.00;


//Function for decoding the required motor speeds from the message sent by the controller 
void decode_motor_speeds(const String& input, int& SpeedL, int& SpeedR, int& SpeedA) {
    int posL = input.indexOf('L');
    int posR = input.indexOf('R');
    int posA = input.indexOf('A');

    if (posL != -1) {
        int nextL = input.substring(posL + 1).toInt();
        SpeedL = nextL;
    }

    if (posR != -1) {
        String subStrR = input.substring(posR + 1);
        for (size_t i = 0; i < subStrR.length(); ++i) {
            if (!isDigit(subStrR.charAt(i))) {
                subStrR = subStrR.substring(0, i);
                break;
            }
        }
        SpeedR = subStrR.toInt();
    }

    if (posA != -1) {
        String subStrA = input.substring(posA + 1);
        for (size_t i = 0; i < subStrA.length(); ++i) {
            if (!isDigit(subStrA.charAt(i))) {
                subStrA = subStrA.substring(0, i);
                break;
            }
        }
        SpeedA = subStrA.toInt();
    }
}

void setup() {

  //Defining servos with their pins
  MotorL.attach(MOTOR_L_PIN);
  MotorR.attach(MOTOR_R_PIN);
  MotorA.attach(MOTOR_A_PIN);

  MotorL.write(90);
  MotorR.write(90);
  MotorA.write(90);

  pinMode(SLAVE_EN, OUTPUT);
  Serial.begin(115200);

  delay(7000);

}


void loop() {

  //Disconnect if not connected to remote for long enough:
  int loop_counter = 0;
  while (!(Serial.available() > 0)) {
    
    loop_counter++;
//    Serial.println(loop_counter);

    if (loop_counter > 10000) {
        MotorL.write(90);
        MotorR.write(90);
        MotorA.write(90);      
    }
  }

  
  // Read the desired motor speeds and write them to motors
  String data;
  digitalWrite(SLAVE_EN, LOW);

  if (Serial.available() > 0) {
    data = Serial.readStringUntil('\n');

    // Drone recieved an "I am disarmed" command
    if (data == "d") {
        
        // Serial.print("Now, it's the moment's I would disarm IRL");    
        MotorL.write(90);
        MotorR.write(90);
        MotorA.write(90);
  
      //Enter a while loop while the drone is disarmed
      while (data == "d") {

         //Check the battery voltage
        BatteryVoltage = map(analogRead(BatterySensor), 0, 1023, 0, 500);
        BatteryVoltage = BatteryVoltage/100*3;

         
        //Communicate the battery voltage to the drone
        digitalWrite(SLAVE_EN, HIGH);
        delay(100);
        
        Serial.print("\n");
        Serial.print(BatteryVoltage);
        Serial.flush();
        

        //Break out of a loop if any serial data is available.
        digitalWrite(SLAVE_EN, LOW);
        delay(100);
        if (Serial.available() > 0) {
          data = "L90R90A90";
        }
      }   
    }      
  }
  decode_motor_speeds(data, SpeedL, SpeedR, SpeedA);

  MotorL.write(SpeedL);
  MotorR.write(SpeedR);
  MotorA.write(SpeedA);
  
}
