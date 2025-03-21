#include <Servo.h>
#include <Arduino.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>

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

//LEDs for status indication
#define LED_GREEN 10
#define LED_WHITE 11
#define LED_RED 12

// Mode definitions
#define ARMED HIGH
#define DISARMED LOW

//BMP280 object
Adafruit_BMP280 bmp280;

//Servo objects
Servo MotorL, MotorR, MotorA;

//Motors speed variables
int SpeedL, SpeedR, SpeedA;

//Battery voltage variables
int BAT1Sraw, BAT2Sraw;
float BAT1Svoltage, BAT2Svoltage;

//Motherboard PCB temperature
float droneTemperature;

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

void measure_battery_voltage() {
    //Raw values read from ADC
    BAT1Sraw = analogRead(BAT1S); //Measure raw value of 1S battery
    BAT2Sraw = analogRead(BAT2S); //Measure raw value of 2S battery

    //Voltage calculation from raw values
    BAT1Svoltage = (BAT1Sraw / 1023.0) * 4.9;   // 1S
    BAT2Svoltage = (BAT2Sraw / 1023.0) * 4.9;   // 2S
}

void measure_temperature() {
  //Temperature read from BMP280
  droneTemperature = bmp280.readTemperature();
}

void send_measurement_data() {
    //Prepare data to send
    String data = "BAT1S" + String(BAT1Svoltage, 2) +
                  "BAT2S" + String(BAT2Svoltage, 2) +
                  "TEMP" + String(droneTemperature, 2);

    //Sending data via RS485
    digitalWrite(SLAVE_EN, HIGH);
    delay(100);
    Serial.println(data);
    Serial.flush();
    digitalWrite(SLAVE_EN, LOW);
    delay(100);
}

void setup() {

  //Attach defined pins to the motors
  MotorL.attach(MOTOR_L_PIN);
  MotorR.attach(MOTOR_R_PIN);
  MotorA.attach(MOTOR_A_PIN);

  MotorL.write(90);
  MotorR.write(90);
  MotorA.write(90);

  pinMode(SLAVE_EN, OUTPUT);

  //Set the battery sensors as inputs
  pinMode(BAT1S, INPUT);
  pinMode(BAT2S, INPUT);

  //Set the additional sensors as inputs
  //Uncomment if additional Sensor1 are used
  // pinMode(AdditionalSensor1, INPUT);  
  //Uncomment if additional Sensor2 are used
  // pinMode(AdditionalSensor2, INPUT);
  //Uncomment if additional Sensor3 are used
  // pinMode(AdditionalSensor3, INPUT);  
  //Uncomment if additional Sensor4 are used
  // pinMode(AdditionalSensor4, INPUT);  
  
  //Set the LED pins as outputs
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_WHITE, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_WHITE, HIGH);
  digitalWrite(LED_RED, HIGH);

  //Set baud rate for serial communication
  Serial.begin(115200);

  //Wait once for initialization ESCs
  delay(7000);

  //Initialize BMP280 sensor
  if (!bmp280.begin()) {
      droneTemperature = -111.0; // Wartość błędu
      return;
  }
}


void loop() {

  //Measure battery voltage
  measure_battery_voltage();
  //Measure temperature
  measure_temperature();

  //Disconnect if not connected to remote for long enough:
  int loop_counter = 0;
  while (!(Serial.available() > 0)) {
    
  loop_counter++;

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

        //Send the measurement data to the remote
        send_measurement_data();

        //Break out of a loop if any serial data is available.
        digitalWrite(SLAVE_EN, LOW);
        delay(100);

        if (Serial.available() > 0) {
          data = "L90R90A90";
        }
      }   
    }      
  }

  //Decode the motor speeds from the received data
  decode_motor_speeds(data, SpeedL, SpeedR, SpeedA);

  //Invert motors directions
  SpeedR = 180 - SpeedR;
  // SpeedA = 180 - SpeedA;
  // SpeedL = 180 - SpeedL;

  //Write the motor speeds to the motors
  MotorL.write(SpeedL);
  MotorR.write(SpeedR);
  MotorA.write(SpeedA);
  
}
