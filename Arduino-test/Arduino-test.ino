#include <SoftwareSerial.h>
#include <dht.h>

#define tx  4
#define rx  5
#define DHT11_PIN A2
#define PHOTOSENSOR_PIN A0
#define MOISTURESENSOR_PIN A1

/* Commands for requesting sensor data from Arduino via Serial*/
enum COMMANDS {
  TEMPERATURE = 1,
  HUMIDITY,
  PHOTOSENSOR,
  WATER_LVL,
  SOIL_MOISTURE,
  PUMP_STATE,
};

enum WETNESS{
  WET = 220,
  SOMEWHAT_WET = 320,
  SOMEWHAT_DRY = 400,
  DRY = 460
};

enum REPORT_MODE : uint8_t{
  PERIODICAL,
  ON_DEMAND
};

/* struct for all the data */
/* should change to a struct of each sensor reading including time, sensor type, raw data, report mode*/
typedef struct {
  float     temperature;
  float     humidity;
  int16_t   photoSensor;
  int16_t   waterLevel;
  int16_t   soilMoisture;
  bool      pumpState;
} SensorData;

typedef struct {
  char *       sensorType;
  byte *       sensingData;
  uint16_t     samplingRate;
  bool         enablePeriodicSampling;
} Sensor;

// init
SoftwareSerial mySerial(rx, tx);
SensorData initData = {34.23, 23.34, 123, 321, 0};
SensorData * currentData = &initData;
dht DHT;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(57600);
  Serial.print("Starting..........");
}

void loop() {
  // put your main code here, to run repeatedly:

  // Poll for commands from connected ESP8266 via Serial
  while(mySerial.available() > 0){
    
    // Read command
    Serial.print("Command Received: ");
    byte cmd = mySerial.read();
    Serial.println(cmd);

    // Process command
    switch(cmd){
      case TEMPERATURE:
        Serial.println("Temperature requested");
        Serial.println("Sending temp data.....");
        writeBytes((byte *)(&currentData->temperature), sizeof(float));
        break;
      case HUMIDITY:
        Serial.println("Humidity requested");
        Serial.println("Sending humidity data.....");
        writeBytes((byte *)(&currentData->humidity), sizeof(float));             
        break;
      case PHOTOSENSOR:
        Serial.println("Photosensor requested");
        Serial.println("Sending photosensor data.....");
        writeBytes((byte *)(&currentData->photoSensor), sizeof(int16_t));             
        break;
      case WATER_LVL:
        Serial.println("Water Level requested");
        Serial.println("Sending water level data.....");
        writeBytes((byte *)(&currentData->waterLevel), sizeof(int16_t));  
        break;
      case SOIL_MOISTURE:
        Serial.println("Soil Moisture requested");
        Serial.println("Sending soil moisture data.....");
        writeBytes((byte *)(&currentData->soilMoisture), sizeof(int16_t));  
        break;
      case PUMP_STATE:
        Serial.println("Pump state requested");
        Serial.println("Sending pump state data.....");
        writeBytes((byte *)(&currentData->pumpState), sizeof(bool));  
        // pump on or off
        break;
      default:
        Serial.println("Unknown Command");
    }
  }
}

void getHumidity(){
  int chk = DHT.read11(DHT11_PIN);
  currentData->humidity = DHT.humidity;
}

void getTemperature(){
  int chk = DHT.read11(DHT11_PIN);
  currentData->temperature = DHT.temperature;
}

void getPhotoSensor(){
  currentData->photoSensor = analogRead(PHOTOSENSOR_PIN);
}

void getMoistureSensor(){
  currentData->soilMoisture = analogRead(MOISTURESENSOR_PIN);
}

/* function to send data that has more than 1 Byte over Serial */
void writeBytes(byte * toSend, size_t size){
  for(size_t i = 0; i < size; i++){
    mySerial.write(*toSend++);
  }
}