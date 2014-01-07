#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

void printAlarmInfo(DeviceAddress deviceAddress);
void printCurrentTemp(DeviceAddress deviceAddress);
void printAddress(DeviceAddress deviceAddress);
void printTemp(DeviceAddress deviceAddress);

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
DeviceAddress insideThermometer, outsideThermometer;

// function that will be called when an alarm condition exists during DallasTemperatures::processAlarms();
void newAlarmHandler(uint8_t* deviceAddress)
{
  Serial.println("Alarm Handler Start"); 
  printAlarmInfo(deviceAddress);
  printTemp(deviceAddress);
  Serial.println();
  Serial.println("Alarm Handler Finish");
}

void printCurrentTemp(DeviceAddress deviceAddress)
{
  printAddress(deviceAddress);
  printTemp(deviceAddress);
  Serial.println();
}

void printAddress(DeviceAddress deviceAddress)
{
  Serial.print("Address: ");
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
  Serial.print(" ");
}

void printTemp(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  if (tempC != DEVICE_DISCONNECTED)
  {
    Serial.print("Current Temp F: ");
    Serial.print(DallasTemperature::toFahrenheit(tempC));
  }
  else Serial.print("DEVICE DISCONNECTED");
  Serial.print(" ");
}

void printAlarmInfo(DeviceAddress deviceAddress)
{
  char temp;
  printAddress(deviceAddress);
  temp = sensors.getHighAlarmTemp(deviceAddress);
  Serial.print("High Alarm: ");
  Serial.print(DallasTemperature::toFahrenheit(temp), DEC);
  Serial.print("F");
  Serial.print(" Low Alarm: ");
  temp = sensors.getLowAlarmTemp(deviceAddress);
  Serial.print(DallasTemperature::toFahrenheit(temp), DEC);
  Serial.print("F");
  Serial.print(" ");
}

void setup(void)
{
  // start serial port
  Serial.begin(115200);
  Serial.println("Dallas Temperature IC Control Library Demo");

  // Start up the library
  sensors.begin();
  
  // locate devices on the bus
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // search for devices on the bus and assign based on an index
  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0");
    
  sensors.setResolution(insideThermometer, 12);  

  // set alarm ranges
  //Serial.println("Setting alarm temps...");
  //sensors.setHighAlarmTemp(insideThermometer, 25);
  //sensors.setLowAlarmTemp(insideThermometer, 23);
  
  //Serial.print("New insideThermometer ");
  //printAlarmInfo(insideThermometer);
  //Serial.println();

  // attach alarm handler
  //sensors.setAlarmHandler(&newAlarmHandler);

}

void loop(void)
{ 
  // ask the devices to measure the temperature
  sensors.requestTemperatures();
  
  // just print out the current temperature
  printCurrentTemp(insideThermometer);
  
  
  delay(300);
}

