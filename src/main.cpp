// Include Library
#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <Preferences.h>
#include <TinyGPSPlus.h>
#include <MQTT.h>
#include <NusabotSimpleTimer.h>

// WidgetMap myMap(V12);
TinyGPSPlus gps;
HardwareSerial SerialGPS(2);

// wifi configuration
const char ssid[] = "Redmi Note 10 Pro";
const char pass[] = "1sampai8";

WiFiClient wifi;
MQTTClient mqtt;
NusabotSimpleTimer timer;

// unsigned int move_index;         // moving index, to be used later
unsigned int move_index = 1; // fixed location for now
static const uint32_t GPSBaud = 9600;

//inisialisasi pin color sensor
const int s2 = 19;
const int s3 = 5;
const int out = 18;
int  Red=0, Blue=0, Green=0, Frequency = 0, Uang = 0; //RGB values
bool statusUang = 0;
bool msg = 0;
//inisialisasi gps variabel
float latitude;
float longitude;


void subscribe(String &topic, String &data){
  if(topic == "kelasiot/led"){
    if(data == "nyala"){
      digitalWrite(27, 1);
    } else {
      digitalWrite(27, 0);
    }
  }
}

void publish() {
  // Membuat payload JSON
  String payload = "{\"lat\":" + String(latitude, 6) + ",\"lon\":" + String(longitude, 6) + "}";
  Serial.print("Publishing message: ");
  Serial.println(payload);

  // Memublikasikan payload ke topik MQTT
  mqtt.publish("kotakamal/gps", (char*) payload.c_str());
}

void connect (){
  Serial.println("Connecting WiFi");
  while(WiFi.status()!= WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected to WiFi");
  Serial.println("==================");
  Serial.println("Connecting to Broker");
  while(!mqtt.connect("broker321")){
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connect to MQTT");
  mqtt.subscribe("kotakamal/#");
}

void checkGPS()
{
  if (gps.charsProcessed() < 4)
  {
    Serial.println(F("No GPS detected: check wiring."));
    Serial.println("GPS OFF");
  }
}

void location()
{
  while (SerialGPS.available() > 0)
  {
    if (gps.encode(SerialGPS.read()))
    {
      if (gps.location.isValid())
      {
        Serial.println("GPS ON");
        latitude = (gps.location.lat()); // Storing the Lat. and Lon.
        longitude = (gps.location.lng());

        Serial.print("LAT:  ");
        Serial.println(latitude, 6); // float to x decimal places
        Serial.print("LONG: ");
        Serial.println(longitude, 6);
      }
      Serial.println();
    }
  }
}
//-------------- *Definition Color Sensor* --------------
int getRed(){
  digitalWrite (s2, LOW);
  digitalWrite (s3, LOW);
  Frequency = pulseIn(out,LOW); /*Get the Red Color Frequency*/
  return Frequency;
}
int getGreen(){
  digitalWrite (s2, HIGH);
  digitalWrite (s3, HIGH);
  Frequency = pulseIn(out,LOW); /*Get the Green Color Frequency*/
  return Frequency;
}
int getBlue(){
  digitalWrite (s2, LOW);
  digitalWrite (s3, HIGH);
  Frequency = pulseIn(out,LOW); /*Get the Blue Color Frequency*/
  return Frequency;
}

void setup()
{
  Serial.begin(115200);
  pinMode(s2,OUTPUT);
  pinMode(s3,OUTPUT);
  pinMode(out,INPUT);
  WiFi.begin(ssid,pass);
  mqtt.begin("broker.emqx.io",wifi);
  SerialGPS.begin(9600, SERIAL_8N1,  4 , 2 );//4 Pin TX(GPS), 2 PIN RX(GPS)
  mqtt.onMessage(subscribe);
  timer.setInterval(1000, publish);
  connect();
  // const char *ssid = "Redmi Note 10 Pro";
  // const char *password = "1sampai8";
}

void loop()
{
  mqtt.loop();
  timer.run();
  if(!mqtt.connected()){
    connect();
  }
  location();

  Red = getRed();
  delay(200);
  Green = getGreen();
  delay(200);
  Blue = getBlue();
  delay(200);

  Serial.print ("Red Freq = ");
  Serial.print (Red);
  Serial.print (" ");
  Serial.print ("Green Freq = ");
  Serial.print (Green);
  Serial.print (" ");
  Serial.print ("Blue Freq = ");
  Serial.print (Blue);


  if(Red>15&&Red<25&&Green>15&&Green<25&&Blue>15&&Blue<25){ //parameter masing-masing uang harus dikalibrasi manual
    Uang = Uang + 20000;
    statusUang = !statusUang;
    Serial.println("20rb"); 

  }else if(Red>15&&Red<25&&Green>15&&Green<25&&Blue>15&&Blue<25){ //parameter masing-masing uang harus dikalibrasi manual
    Uang = Uang + 100000;
    statusUang = !statusUang;
    Serial.println("100rb");

  }else if(Red > 20 && Red <= 26 && Green >= 19 && Green <= 23 && Blue > 14 && Blue <= 19){ //parameter masing-masing uang harus dikalibrasi manual
    Uang = Uang + 50000;
    statusUang = !statusUang;
    Serial.println("50rb");

  }else if(Red>15&&Red<25&&Green>15&&Green<25&&Blue>15&&Blue<25){ //parameter masing-masing uang harus dikalibrasi manual
    Uang = Uang + 2000;
    statusUang = !statusUang;
    Serial.println("2rb");

  }else if(Red>15&&Red<25&&Green>15&&Green<25&&Blue>15&&Blue<25){ //parameter masing-masing uang harus dikalibrasi manual
    Uang = Uang + 5000;
    statusUang = !statusUang;
    Serial.println("5rb");

  }else if(Red>15&&Red<25&&Green>15&&Green<25&&Blue>15&&Blue<25){ //parameter masing-masing uang harus dikalibrasi manual
    Uang = Uang + 10000;
    statusUang = !statusUang;
    Serial.println("10rb");

  }else if(Red>15&&Red<25&&Green>15&&Green<25&&Blue>15&&Blue<25){ //parameter masing-masing uang harus dikalibrasi manual
    Uang = Uang + 1000;
    statusUang = !statusUang;
    Serial.println("1rb");

  }else if (Red > 100 && Green > 100 && Blue >100){
    statusUang = 0;
    msg = 0;
  }
  if (statusUang == 1 && msg == 0) {
    Serial.println(Uang);
    // display.clearDisplay();
    // display.setTextSize(2);  // Draw 2X-scale text
    // display.setTextColor(SSD1306_WHITE);
    // display.setCursor(5, 0);
    // display.println(F("Jazakllahu"));
    // display.setCursor(10, 15);
    // display.println(F("Khairan!"));
    // display.display();  // Show initial text
    // delay(2000);

    // bot.sendMessage(CHAT_ID, "Saldo Infaq Hari Ini :");
    // String nominal = String(Uang);
    // bot.sendMessage(CHAT_ID, nominal);

    msg = !msg;
  }

}