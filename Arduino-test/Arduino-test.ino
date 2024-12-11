#include <Wire.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>

#include <SoftwareSerial.h>
#include <dht.h>

#define tx 11
#define rx 12
#define PUMP_PIN 8
#define LIGHT_PIN 7
#define DHT11_PIN 2
#define PHOTOSENSOR_PIN A0
#define MOISTURESENSOR_PIN A1
#define WATERLEVEL_PIN A2

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

#define WET 220
#define DRY 460

#define DARK 80

/* Commands for requesting sensor data from Arduino via Serial*/
enum COMMANDS : uint8_t {
  TEMPERATURE = 1,
  HUMIDITY,
  PHOTOSENSOR,
  WATER_LVL,
  SOIL_MOISTURE,
  PUMP_STATE,
};

/* struct for all the data */
typedef struct {
  float temperature;
  float humidity;
  int16_t photoSensor;
  int16_t waterLevel;
  int16_t soilMoisture;
  bool pumpState;
  bool ledState;
} SensorData;

// init
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

SoftwareSerial mySerial(rx, tx);
dht DHT;
SensorData *currentData = (SensorData *)malloc(sizeof(SensorData));

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(57600);

  // if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3D for 128x64
  //   Serial.println(F("SSD1306 allocation failed"));
  //   for (;;)
  //     ;
  // }

  // delay(2000);
  // display.clearDisplay();
  // Serial.println("Starting..........");

  // display.setTextSize(1);
  // display.setTextColor(WHITE);
  // display.setCursor(0, 10);
  // // Display static text
  // display.println("Hello, world!");
  // display.display();
}

void loop() {
  // put your main code here, to run repeatedly:
  // Poll for commands from connected ESP8266 via Serial
  while (mySerial.available() > 0) {

    // Read command
    Serial.print("Command Received: ");
    byte cmd = mySerial.read();
    Serial.println(cmd);

    // Process command
    switch (cmd) {
      case TEMPERATURE:
        Serial.println("Temperature requested");
        Serial.println("Sending temp data.....");
        getTemperature();
        writeBytes((byte *)&(currentData->temperature), sizeof(float));
        break;
      case HUMIDITY:
        Serial.println("Humidity requested");
        Serial.println("Sending humidity data.....");
        getHumidity();
        writeBytes((byte *)&(currentData->humidity), sizeof(float));
        break;
      case PHOTOSENSOR:
        Serial.println("Photosensor requested");
        Serial.println("Sending photosensor data.....");
        getPhotoSensor();
        writeBytes((byte *)&(currentData->photoSensor), sizeof(int16_t));
        break;
      case WATER_LVL:
        Serial.println("Water Level requested");
        Serial.println("Sending water level data.....");
        getWaterLvl();
        writeBytes((byte *)&(currentData->waterLevel), sizeof(int16_t));
        break;
      case SOIL_MOISTURE:
        Serial.println("Soil Moisture requested");
        Serial.println("Sending soil moisture data.....");
        getMoistureSensor();
        writeBytes((byte *)&(currentData->soilMoisture), sizeof(int16_t));
        break;
      case PUMP_STATE:
        Serial.println("Pump state requested");
        Serial.println("Sending pump state data.....");
        getPumpState();
        writeBytes((byte *)&(currentData->pumpState), sizeof(bool));
        // pump on or off
        break;
      default:
        Serial.println("Unknown Command");
    }
  }
  if (currentData->soilMoisture < WET && currentData->pumpState) {
    digitalWrite(PUMP_PIN, LOW);
    currentData->pumpState = false;
  }
  if (currentData->soilMoisture > DRY && ~currentData->pumpState) {
    digitalWrite(PUMP_PIN, HIGH);
    currentData->pumpState = true;
  }
  if (currentData->photoSensor <= DARK && ~currentData->ledState) {
    digitalWrite(LIGHT_PIN, HIGH);
    currentData->ledState = true;
  }
  if (currentData->photoSensor > DARK && currentData->ledState) {
    digitalWrite(LIGHT_PIN, LOW);
    currentData->ledState = false;
  }
}

void getHumidity() {
  int chk = DHT.read11(DHT11_PIN);
  currentData->humidity = (float)DHT.humidity;
}

void getTemperature() {
  int chk = DHT.read11(DHT11_PIN);
  currentData->temperature = (float)DHT.temperature;
}

void getPhotoSensor() {
  currentData->photoSensor = analogRead(PHOTOSENSOR_PIN);
}

void getMoistureSensor() {
  currentData->soilMoisture = analogRead(MOISTURESENSOR_PIN);
}

void getWaterLvl() {
  currentData->waterLevel = analogRead(WATERLEVEL_PIN);
}

void getPumpState() {
  currentData->pumpState = digitalRead(8);
}
/* function to send data that has more than 1 Byte over Serial */
void writeBytes(byte *toSend, size_t size) {
  for (size_t i = 0; i < size; i++) {
    mySerial.write(*toSend++);
  }
}