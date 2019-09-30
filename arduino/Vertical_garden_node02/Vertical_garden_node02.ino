#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

//NTP
const long utcOffsetInSeconds = 25200;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

//DS18B20
#define ONE_WIRE_BUS 2 // Data wire is plugged into port 9 on the Arduino
#define precision 12 // OneWire precision Dallas Sensor

//Relay
//#define relay_1 5
//#define relay_2 4

//Firebase
#define FIREBASE_HOST "vertical-garden-project.firebaseio.com"
#define FIREBASE_AUTH "FIHGNRi2tFbyezMZrSFjGbErax28MdTDA839h6wY"
#define WIFI_SSID "Wifi Belakang"
#define WIFI_PASSWORD "bismillahaja"

//DS18B20
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.
DeviceAddress T1, T2; // arrays to hold device addresses

//Soil Moisture
int sensor_tanah = A0;
int output_tanah ;

//Relay
//String air_1;
//String air_2;

//NTP
char daysOfTheWeek[7][12] = {"Ahad", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};

void setup(void)
{
  Serial.begin(115200); //Start serial port

  //DS18B20
  Serial.println("Dallas Temperature IC Control Library");
  sensors.begin();
  Serial.print("Found: ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" Devices.");
  if (!sensors.getAddress(T1, 0)) Serial.println("Not Found Sensor 1");
  if (!sensors.getAddress(T2, 1)) Serial.println("Not Found Sensor 2");
  sensors.setResolution(T1, precision);
  sensors.setResolution(T2, precision);

  //Relay
//  pinMode(relay_1, OUTPUT);
//  pinMode(relay_2, OUTPUT);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  //Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
//  Firebase.setInt("air/relay_1", 0);
//  Firebase.setInt("air/relay_2", 0);

  //NTP
  timeClient.begin();
}
// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print(tempC);
}

void printData(DeviceAddress deviceAddress)
{
  printTemperature(deviceAddress);
  Serial.println();
}

void loop(void)
{
  //NTP
  timeClient.update();
  //  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  //  Serial.print(", ");
  //  Serial.print(timeClient.getHours());
  //  Serial.print(":");
  //  Serial.print(timeClient.getMinutes());
  //  Serial.print(":");
  //  Serial.println(timeClient.getSeconds());
  //  Serial.println(timeClient.getFormattedTime());

  //DS18B20 Mulai
  sensors.requestTemperatures();
  //DS18B20 Bubar

  //sensor tanah mulai
  output_tanah = analogRead(sensor_tanah);
  output_tanah = map(output_tanah, 1023, 350, 0, 100);
  //sensor tanah bubar

  //Relay Mulai
//  air_1 = Firebase.getString("air/relay_1");
//  if  (air_1 != "0") {
//    Serial.println("air nyala");
//    digitalWrite(relay_1, HIGH);
//  }
//  else {
//    Serial.println("air tewas");
//    digitalWrite(relay_1, LOW);
//  }
  //Relay bubar

  //Firebase Mulai
  StaticJsonBuffer<256> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  //  root["posisi"] = "atas";
  JsonObject& parameter = root.createNestedObject("parameter");
  parameter["node"] = "node-02";
  parameter["suhu"] = sensors.getTempC(T1);
  parameter["kelembaban"] = output_tanah;

  JsonObject& waktu = root.createNestedObject("waktu");
  waktu["hari"] = daysOfTheWeek[timeClient.getDay()];
  waktu["jam"] = timeClient.getFormattedTime();
  root.prettyPrintTo(Serial);

  Firebase.push("vertical_garden", root);
  delay(5000);
  //Firebase Bubar
}
