#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <FirebaseArduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Servo.h>

//Servo
Servo myservo;

const char ssid[] = "hamdan";  //  your network SSID (name)
const char pass[] = "";       // your network password

// NTP Servers:
static const char ntpServerName[] = "us.pool.ntp.org";

const int timeZone = 7;     // Central European Time

WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

time_t getNtpTime();
void digitalClockDisplay();
void printDigits(int digits);
void sendNTPpacket(IPAddress &address);

//DS18B20
#define ONE_WIRE_BUS 2 // Data wire is plugged into port 9 on the Arduino
#define precision 12 // OneWire precision Dallas Sensor

//Relay
#define relay_1 5 //D1
#define relay_2 4 //D2

//Firebase
#define FIREBASE_HOST "verticalgarden-project.firebaseio.com"
#define FIREBASE_AUTH "nPdSyY5tQPBZ6WFtHNRKbGmYbd3YwNyCEj1e0THK"

//DS18B20
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.
DeviceAddress T1, T2; // arrays to hold device addresses

//Soil Moisture
int sensor_tanah = A0;
int output_tanah ;

//Relay
String air_1;
String air_2;

//Servo
String pupuk_1;

void setup()
{
  Serial.begin(115200);
  while (!Serial) ; // Needed for Leonardo only
  delay(250);
  Serial.println("TimeNTP Example");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("IP number assigned by DHCP is ");
  Serial.println(WiFi.localIP());
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
  setSyncInterval(300);

  //Servo
  myservo.attach(0);  // attaches the servo on GIO2 to the servo object

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
  pinMode(relay_1, OUTPUT);
  pinMode(relay_2, OUTPUT);

  // connect to wifi.
  //  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  //  Serial.print("connecting");
  //  while (WiFi.status() != WL_CONNECTED) {
  //    Serial.print(".");
  //    delay(500);
  //  }
  //  Serial.println();
  //  Serial.print("connected: ");
  //  Serial.println(WiFi.localIP());

  //Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.set("pupuk1/servo_1", "off");
}

time_t prevDisplay = 0; // when the digital clock was displayed

void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

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

void loop()
{
  //Servo
  int pos;

  //DS18B20 Mulai
  sensors.requestTemperatures();
  //DS18B20 Bubar

  //sensor tanah mulai
  output_tanah = analogRead(sensor_tanah);
  output_tanah = map(output_tanah, 1023, 350, 0, 100);
  //sensor tanah bubar

  //Relay Mulai
  //  air_1 = Firebase.getString("air/relay_1");
  if  (sensors.getTempC(T1) >= 30) {
    //    Serial.println("air nyala");
    digitalWrite(relay_1, HIGH);
  }
  else {
    //    Serial.println("air tewas");
    digitalWrite(relay_1, LOW);
  }
  //Relay bubar

  //Servo Mulai
  pupuk_1 = Firebase.getString("pupuk1/servo_1");
  if  (pupuk_1 == "on") {
    for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      myservo.write(pos);                 // tell servo to go to position in variable 'pos'
      delay(15);                          // waits 15ms for the servo to reach the position
    }
  }
  //
  if  (pupuk_1 == "off") {
    for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
      myservo.write(pos);                 // tell servo to go to position in variable 'pos'
      delay(15);                          // waits 15ms for the servo to reach the position
    }
  }
  //
  if  (pupuk_1 == "oto") {
    if (month() == 1 ) {
      if (day() == 1) {
        if (hour() == 13) {
          if (minute() == 30) {
            for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
              // in steps of 1 degree
              myservo.write(pos);              // tell servo to go to position in variable 'pos'
              delay(15);                       // waits 15ms for the servo to reach the position
              //              Firebase.set("pupuk/servo_1_status", "on");
            }
            delay (10000);
            for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
              myservo.write(pos);              // tell servo to go to position in variable 'pos'
              delay(15);                       // waits 15ms for the servo to reach the position
              //              Firebase.set("pupuk/servo_1_status", "off");
            }
          }
        }
      }
    }
  }
  //Servo Bubar

  //Firebase Mulai
  StaticJsonBuffer<256> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  //  root["posisi"] = "atas";
  JsonObject& parameter = root.createNestedObject("parameter");
  parameter["node"] = "node-01";
  parameter["suhu"] = sensors.getTempC(T1);
  parameter["kelembaban"] = output_tanah;

  JsonObject& waktu = root.createNestedObject("waktu");
  waktu["hari"] = (day() + String(" ") + month() + String(" ") + year());
  waktu["jam"] = (hour() + String(":") + minute() + String(":") + second());
  root.prettyPrintTo(Serial);

  Firebase.push("vertical_garden", root);
  delay(1000);
  //Firebase Bubar

  //    Serial.println((hour()) + String(":") + (minute()) + String(":") + (second()));
  //  if (timeStatus() != timeNotSet) {
  //    if (now() != prevDisplay) { //update the display only if time has changed
  //      prevDisplay = now();
  //      digitalClockDisplay();
  //    }
  //  }
}

void digitalClockDisplay()
{
  Serial.println(hour() + String(":") + minute() + String(":") + second());
  Serial.println(day() + String(" ") + month() + String(" ") + year());
}

void printDigits(int digits)
{
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");

  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;

      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress & address)
{

  memset(packetBuffer, 0, NTP_PACKET_SIZE);

  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;            // Stratum, or type of clock
  packetBuffer[2] = 6;            // Polling Interval
  packetBuffer[3] = 0xEC;         // Peer Clock Precision

  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;

  Udp.beginPacket(address, 123);   //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
