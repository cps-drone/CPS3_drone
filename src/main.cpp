#include "Definitions.h"
#include "CPS3_drone.h"

// CPS3 drone struct object instance
cps3_drone_t CPS3;

void setup() {
  // Initialize the CPS3 struct object instance
  CPS3_drone_init(&CPS3);
}

/*
  * Main loop function
  * It updates the CPS3 timer to avoid lost messages,
  * Then it receives messages with motors speeds and master mode flag,
  * After the message is received it measures the battery voltages and
  * sends the measurements data to the remote.
  * In case when it does not receive any messages from the remote,
  * it sends the battery measurements data to the remote (waiting for feedback).
*/
void loop() {
  /*
    * Updating the timer to avoid,
    * case "lost message" for example when the remote,
    * is turned off by mistake during swimming.
  */ 
  CPS3.Data.no_massage_timer_current_time = millis();
  get_motor_speeds(&CPS3);
  get_battery_voltages(&CPS3);
  send_measurement_data(&CPS3);
}