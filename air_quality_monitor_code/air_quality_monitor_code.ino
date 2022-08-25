
#include <Arduino.h>
#include "s8_uart.h"
#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>


#define DEBUG_BAUDRATE 9600


#define TFT_CS 32
#define TFT_DC  26
#define TFT_RST 27

#define TFT_MOSI 25  // Data out
#define TFT_SCLK 33  // Clock out

#define pms5003_RX_PIN 19       // Rx pin which the S8 Tx pin is attached to (change if it is needed)
#define pms5003_TX_PIN 18         // Tx pin which the S8 Rx pin is attached to (change if it is needed)


#define S8_RX_PIN 22        // Rx pin which the S8 Tx pin is attached to (change if it is needed)
#define S8_TX_PIN 23         // Tx pin which the S8 Rx pin is attached to (change if it is needed)

SoftwareSerial S8_serial(S8_RX_PIN, S8_TX_PIN);
S8_UART *sensor_S8;
S8_sensor sensor;


SoftwareSerial  pmsSerial( pms5003_RX_PIN, pms5003_TX_PIN);

struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};

struct pms5003data data;

float p = 3.1415926;
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

//boolean readPMSdata(Stream *s); 
//void TextLCD(char *text, uint16_t color, uint16_t x, uint16_t y);

void setup() {
   tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
  // Configure serial port, we need it for debug
  Serial.begin(9600);

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
pmsSerial.begin(9600);


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


    Serial.println(F("Initialized"));

  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);

  // large block of text
  tft.fillScreen(ST77XX_BLACK);
   tft.setRotation(1);
   tft.setTextSize(1);
}


void loop() {
  

  char buf [100]; 
  //printf("Millis: %lu\n", millis());

  // Get CO2 measure
  sensor.co2 = sensor_S8->get_co2();
 // printf("CO2 value: %d ppm\n", sensor.co2);
  memset(buf, ' ', 100);
  sprintf (buf, "CO2: %d ppm", sensor.co2);
  buf[strlen(buf)]=' ';
  
  //tft.fillScreen(ST77XX_BLACK);
  TextLCD(buf, ST77XX_WHITE, 0, 10);
  //Serial.printf("/*%u*/\n", sensor.co2);   // Format to use with Serial Studio program

  // Compare with PWM output
  //sensor.pwm_output = sensor_S8->get_PWM_output();
  //printf("PWM output = %0.0f ppm\n", (sensor.pwm_output / 16383.0) * 2000.0);

  // Wait 5 second for next measure
  
  if (readPMSdata(&pmsSerial)) {
    // reading data was successful!
   // Serial.println();
   // Serial.println("---------------------------------------");
   // Serial.println("Concentration Units (standard)");
   // Serial.print("PM 1.0: "); Serial.print(data.pm10_standard);

    memset(buf, ' ', 100);
    sprintf (buf, "PM1.0: %d ug/m3", data.pm10_standard);
    buf[strlen(buf)]=' ';
    TextLCD(buf, ST77XX_WHITE, 0, 30);
    
    //Serial.print("\t\tPM 2.5: "); Serial.print(data.pm25_standard);

    memset(buf, ' ', 100);
    sprintf (buf, "PM2.5: %d ug/m3", data.pm25_standard);
    buf[strlen(buf)]=' ';
    TextLCD(buf, ST77XX_WHITE, 0, 50);
    
    //Serial.print("\t\tPM10: "); Serial.println(data.pm100_standard);

    memset(buf, ' ', 100);
    sprintf (buf, "PM10 : %d ug/m3", data.pm100_standard);
    buf[strlen(buf)]=' ';
    TextLCD(buf, ST77XX_WHITE, 0, 70);
    
    /*Serial.println("---------------------------------------");
    Serial.println("Concentration Units (environmental)");
    Serial.print("PM 1.0: "); Serial.print(data.pm10_env);
    Serial.print("\t\tPM 2.5: "); Serial.print(data.pm25_env);
    Serial.print("\t\tPM 10: "); Serial.println(data.pm100_env);
    Serial.println("---------------------------------------");
    Serial.print("Particles > 0.3um / 0.1L air:"); Serial.println(data.particles_03um);
    Serial.print("Particles > 0.5um / 0.1L air:"); Serial.println(data.particles_05um);
    Serial.print("Particles > 1.0um / 0.1L air:"); Serial.println(data.particles_10um);
    Serial.print("Particles > 2.5um / 0.1L air:"); Serial.println(data.particles_25um);
    Serial.print("Particles > 5.0um / 0.1L air:"); Serial.println(data.particles_50um);
    Serial.print("Particles > 10.0 um / 0.1L air:"); Serial.println(data.particles_100um);
    Serial.println("---------------------------------------");*/
  }

  delay(100);
  
}


void TextLCD(char *text, uint16_t color, uint16_t x, uint16_t y)  {
  tft.setCursor(x, y);
  tft.setTextColor(color, ST77XX_BLACK);
  tft.setTextWrap(false);
  tft.print(text);

}

boolean readPMSdata(Stream *s) {
  if (! s->available()) {
    return false;
  }
  
  // Read a byte at a time until we get to the special '0x42' start-byte
  if (s->peek() != 0x42) {
    s->read();
    return false;
  }
 
  // Now read all 32 bytes
  if (s->available() < 32) {
    return false;
  }
    
  uint8_t buffer[32];    
  uint16_t sum = 0;
  s->readBytes(buffer, 32);
 
  // get checksum ready
  for (uint8_t i=0; i<30; i++) {
    sum += buffer[i];
  }
 
  /* debugging
  for (uint8_t i=2; i<32; i++) {
    Serial.print("0x"); Serial.print(buffer[i], HEX); Serial.print(", ");
  }
  Serial.println();
  */
  
  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i=0; i<15; i++) {
    buffer_u16[i] = buffer[2 + i*2 + 1];
    buffer_u16[i] += (buffer[2 + i*2] << 8);
  }
 
  // put it into a nice struct :)
  memcpy((void *)&data, (void *)buffer_u16, 30);
 
  if (sum != data.checksum) {
    Serial.println("Checksum failure");
    return false;
  }
  // success!
  return true;
}
