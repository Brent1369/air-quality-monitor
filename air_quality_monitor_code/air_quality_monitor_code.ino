
#include <Arduino.h>
#include "s8_uart.h"
#include <SoftwareSerial.h>
//#include "PMS.h"
/* BEGIN CONFIGURATION */
#define DEBUG_BAUDRATE 9600
#include <PMserial.h>

#define S8_RX_PIN 22        // Rx pin which the S8 Tx pin is attached to (change if it is needed)
#define S8_TX_PIN 23         // Tx pin which the S8 Rx pin is attached to (change if it is needed)
SoftwareSerial S8_serial(S8_RX_PIN, S8_TX_PIN);
S8_UART *sensor_S8;
S8_sensor sensor;

#define pms5003_RX_PIN 0        // Rx pin which the S8 Tx pin is attached to (change if it is needed)
#define pms5003_TX_PIN 2         // Tx pin which the S8 Rx pin is attached to (change if it is needed)
SerialPM  pms5003_serial(PMS5003, pms5003_RX_PIN, pms5003_TX_PIN);
//PMS pms(pms5003_serial);
//PMS::DATA data;


void setup() {
  
  // Configure serial port, we need it for debug
  Serial.begin(DEBUG_BAUDRATE);

  // Wait port is open or timeout
  int i = 0;
  while (!Serial && i < 50) {
    delay(10);
    i++;
  }
  // First message, we are alive
  Serial.println("");
  Serial.println("Init");

  // Initialize S8 sensor
  S8_serial.begin(S8_BAUDRATE);
  sensor_S8 = new S8_UART(S8_serial);

  //pms5003_serial.begin(S8_BAUDRATE);
pms5003_serial.init();


  // Check if S8 is available
  sensor_S8->get_firmware_version(sensor.firm_version);
  int len = strlen(sensor.firm_version);
  if (len == 0) {
      Serial.println("SenseAir S8 CO2 sensor not found!");
      while (1) { delay(1); };
  }

  // Show basic S8 sensor info
  Serial.println(">>> SenseAir S8 NDIR CO2 sensor <<<");
  printf("Firmware version: %s\n", sensor.firm_version);
  sensor.sensor_id = sensor_S8->get_sensor_ID();
  Serial.print("Sensor ID: 0x"); printIntToHex(sensor.sensor_id, 4); Serial.println("");

  Serial.println("Setup done!");
  Serial.flush();
}


void loop() {
  
  //printf("Millis: %lu\n", millis());

  // Get CO2 measure
  sensor.co2 = sensor_S8->get_co2();
  printf("CO2 value = %d ppm\n", sensor.co2);

  //Serial.printf("/*%u*/\n", sensor.co2);   // Format to use with Serial Studio program

  // Compare with PWM output
  //sensor.pwm_output = sensor_S8->get_PWM_output();
  //printf("PWM output = %0.0f ppm\n", (sensor.pwm_output / 16383.0) * 2000.0);

  // Wait 5 second for next measure
  delay(5000);

 // pms.read(data);
 //   Serial.print("PM 1.0 (ug/m3): ");
  //  Serial.println(data.PM_AE_UG_1_0);

 //   Serial.print("PM 2.5 (ug/m3): ");
 //   Serial.println(data.PM_AE_UG_2_5);

 //   Serial.print("PM 10.0 (ug/m3): ");
 //   Serial.println(data.PM_AE_UG_10_0);

  //  Serial.println();

    pms5003_serial.read();

    Serial.print(F("PM1.0 "));
    Serial.print(pms5003_serial.pm01);
    Serial.print(F(", "));
    Serial.print(F("PM2.5 "));
    Serial.print(pms5003_serial.pm25);
    Serial.print(F(", "));
    Serial.print(F("PM10 "));
    Serial.print(pms5003_serial.pm10);
    Serial.println(F(" [ug/m3]"));
  
}
