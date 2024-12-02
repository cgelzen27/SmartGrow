#include <SoftwareSerial.h>
#include <dht.h>

#define tx  4
#define rx  5
#define DHT11_PIN 8
#define PHOTOSENSOR_PIN A0
#define MOISTURESENSOR_PIN A1
#define WATERLEVEL_PIN A2

/* Commands for requesting sensor data from Arduino via Serial*/
enum COMMANDS : uint8_t{
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
// typedef struct {
//   float     temperature;
//   float     humidity;
//   int16_t   photoSensor;
//   int16_t   waterLevel;
//   int16_t   soilMoisture;
//   bool      pumpState;
// } SensorData;

typedef struct {
  char *       sensorType;
  byte *       sensingData;
  uint16_t     samplingRate;
  bool         enablePeriodicSampling;
} Sensor;

// init
SoftwareSerial mySerial(rx, tx);
dht DHT;
// SensorData initData = {34.23, 23.34, 123, 321, 0};
// SensorData * currentData = &initData;
Sensor * temperature, * humidity, * photoSensor, * waterLevel, * soilMoisture, * pumpState;

void setup() {
  //initSensorStructure();
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(57600);
  Serial.println("Starting..........");

  pinMode(PHOTOSENSOR_PIN, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  //Serial.println(*(double *)(temperature->sensingData));
  getPhotoSensor();
  Serial.println(*(int *)(photoSensor->sensingData));
  delay(2000);
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
        writeBytes((byte *)(temperature), sizeof(Sensor));
        break;
      case HUMIDITY:
        Serial.println("Humidity requested");
        Serial.println("Sending humidity data.....");
        writeBytes((byte *)(humidity), sizeof(Sensor));             
        break;
      case PHOTOSENSOR:
        Serial.println("Photosensor requested");
        Serial.println("Sending photosensor data.....");
        writeBytes((byte *)(photoSensor), sizeof(Sensor));             
        break;
      case WATER_LVL:
        Serial.println("Water Level requested");
        Serial.println("Sending water level data.....");
        writeBytes((byte *)(waterLevel), sizeof(Sensor));  
        break;
      case SOIL_MOISTURE:
        Serial.println("Soil Moisture requested");
        Serial.println("Sending soil moisture data.....");
        writeBytes((byte *)(soilMoisture), sizeof(Sensor));  
        break;
      case PUMP_STATE:
        Serial.println("Pump state requested");
        Serial.println("Sending pump state data.....");
        writeBytes((byte *)(pumpState), sizeof(Sensor));  
        // pump on or off
        break;
      default:
        Serial.println("Unknown Command");
    }
  }
}

void initSensorStructure(){
  temperature = (Sensor *) malloc(sizeof(Sensor));
  humidity = (Sensor *) malloc(sizeof(Sensor));
  photoSensor = (Sensor *) malloc(sizeof(Sensor));
  waterLevel = (Sensor *) malloc(sizeof(Sensor));
  soilMoisture = (Sensor * )malloc(sizeof(Sensor));
  pumpState = (Sensor *) malloc(sizeof(Sensor));
}

void getHumidity(){
  int chk = DHT.read11(DHT11_PIN);
  Serial.println(DHT.humidity);
  humidity->sensingData = (byte *) &(DHT.humidity);
}

void getTemperature(){
  int chk = DHT.read11(DHT11_PIN);
  Serial.println(DHT.temperature);
  temperature->sensingData = (byte *) &(DHT.temperature);
}

void getPhotoSensor(){
  static int phData;
  phData = analogRead(PHOTOSENSOR_PIN);

  photoSensor->sensingData = (byte *) &phData;
}

void getMoistureSensor(){  
  static int soilData;
  soilData = analogRead(MOISTURESENSOR_PIN);

  soilMoisture->sensingData = (byte *) &soilData;
}

void getWaterLvl(){
  static int waterData;
  waterData = analogRead(WATERLEVEL_PIN);

  waterLevel->sensingData = (byte *) &waterData;
}
/* function to send data that has more than 1 Byte over Serial */
void writeBytes(byte * toSend, size_t size){
  for(size_t i = 0; i < size; i++){
    mySerial.write(*toSend++);
  }
}