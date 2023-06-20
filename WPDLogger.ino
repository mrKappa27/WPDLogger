//https://randomnerdtutorials.com/esp32-data-logging-temperature-to-microsd-card/
//https://www.reddit.com/r/esp8266/comments/kps3gp/05v_analog_pressure_transducer_using_it_on_33v/
//https://www.reddit.com/r/esp32/comments/l3zrv8/5v_water_flow_sensor_on_esp32/
//https://www.reddit.com/r/esp32/comments/t9ggzg/pressure_sensor_010_bar_for_use_on_esp32/

// Libraries for SD card
#include "FS.h"
#include "SD.h"
#include <SPI.h>


// Define deep sleep options
uint64_t uS_TO_S_FACTOR = 1000000;  // Conversion factor for micro seconds to seconds
// Sleep for 1 minute = 60 seconds
uint64_t TIME_TO_SLEEP = 60;
// ADC stabilization reading time (ms)
uint64_t ADC_T_STAB = 50;

// Define CS pin for the SD card module
#define SD_CS 5

// These constants won't change:
const int sensorPin = 2;  // pin that the sensor is attached to
const int ledPin = 25;      // pin that the LED is attached to

//Max pressure value in Bar/100 (5Bar = 500)
const int maxPressure = 550;
//Min pressure value in Bar/100 (0Bar = 0)
const int minPressure = 0;

// Save reading number on RTC memory
RTC_DATA_ATTR int readingID = 0;

String dataMessage;

// variables:
int sensorValue = 0;    // the sensor value
int sensorMin = 300;      // minimum sensor value (0 Bar @ 0.5V -> ~300pt)
int sensorMax = 3795;     // maximum sensor value(5 Bar @ 4.5V -> ~3795pt)

// Pressure Sensor variables
float pressure;

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

void setup() {
  // Start serial communication for debugging purposes
  Serial.begin(115200);

  // Enable Timer wake_up
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  Serial.print("Booting...#");
  Serial.println(readingID);

  // Set analog read resolution to 12bit (0..4096)
  analogReadResolution(12);

  // Initialize SD card
  SD.begin(SD_CS);  
  if(!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    esp_deep_sleep_start(); 
    return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    esp_deep_sleep_start(); 
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    esp_deep_sleep_start(); 
    return;    // init failed
  }

  // If the data.txt file doesn't exist
  // Create a file on the SD card and write the data labels
  File file = SD.open("/data.txt");
  if(!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/data.txt", "Reading ID, Date, Raw value, Pressure (x.xx Bar) \r\n");
  }
  else {
    Serial.println("File already exists");  
  }
  file.close();

  getReadings();
  getTimeStamp();
  logSDCard();
  
  // Increment readingID on every new reading
  readingID++;
  
  // Start deep sleep
  Serial.println("DONE! Going to sleep now.");
  esp_deep_sleep_start(); 
}

void loop() {
  // The ESP32 will be in deep sleep
  // it never reaches the loop()
}

// Function to get pressure
void getReadings(){

  delay(ADC_T_STAB);
  
  // read the sensor:
  sensorValue = analogRead(sensorPin);

  Serial.print("Raw analog:");
  Serial.println(sensorValue);
  
  // in case the sensor value is outside the range seen during calibration
  sensorValue = constrain(sensorValue, sensorMin, sensorMax);

  // apply the calibration to the sensor reading
  pressure = map(sensorValue, sensorMin, sensorMax, minPressure, maxPressure) / 100.0;

  Serial.print("Pressure:");
  Serial.println(pressure);
}

// Function to get date and time from NTPClient or RTC module
void getTimeStamp() {
  //Placeholder
}

// Write the sensor readings on the SD card
void logSDCard() {
  dataMessage = String(readingID) + "," + String(dayStamp) + "," + String(sensorValue) + "," + 
                String(pressure, 2) + "\r\n";
  Serial.print("Save data: ");
  Serial.println(dataMessage);
  appendFile(SD, "/data.txt", dataMessage.c_str());
}

// Write to the SD card file
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// Append data to the SD card file
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}
