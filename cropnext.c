
#include <LTask.h>
#include <LWiFi.h>
#include <LWiFiClient.h>
#include <LBattery.h>
#include "Barometer.h"
#include <Wire.h>

#define WIFI_AP "iPhone"
#define WIFI_PASSWORD "helloworld1"
#define WIFI_AUTH LWIFI_WPA  // choose from LWIFI_OPEN, LWIFI_WPA, or LWIFI_WEP.

// Ubidots information

#define URL        "things.ubidots.com"
#define TOKEN      "WF1hbsrYltg5xxvwGDowAejTykHigEi5"          // replace with your Ubidots token generated in your profile tab
#define VARIABLEID "55d5f60476254xx22b34eb0441x"
#define VARIABLEID1 "55d5f5e376254xx22ce1e035x8f" //temprature
#define VARIABLEID2 "55d4c74b76254xx2483097x3c35" //dust
#define VARIABLEID3 "55d5fbf77625423xx63264e715" //crow





Barometer myBarometer;
float temperature;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 3000;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;
int counter = 0;

void setup()
{
  LTask.begin();
  LWiFi.begin();
    pinMode(8,INPUT);

  Serial.begin(9600);
  myBarometer.init();


  // keep retrying until connected to AP
  Serial.println("Connecting to AP");
  while (0 == LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD)))
  {
    delay(1000);
  }
}

void loop()
{ 
   temperature = myBarometer.bmp085GetTemperature(myBarometer.bmp085ReadUT()); //Get the temperature, bmp085ReadUT MUST be called first
     duration = pulseIn(8, LOW);
  lowpulseoccupancy = lowpulseoccupancy+duration;
 ratio = lowpulseoccupancy/1000;  // Integer percentage 0=>100
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; 
  save_value(String(analogRead(A0)), String(temperature), String(concentration), String(analogRead(A2)));
  delay(500);
  
}

void save_value(String value, String a, String dust, String crow){
 
  Serial.println("Sending value to Ubidots...");
  LWiFiClient c;
  while (!c.connect(URL, 80))
  {
    Serial.println("Retrying to connect...");
    delay(100);
  }

  String data = "{\"value\":"+ value + "}";
  String thisLength = String(data.length());
  
  // Build HTTP POST request
  c.print("POST /api/v1.6/variables/");
  c.print(VARIABLEID);
  c.print("/values?token=");
  c.print(TOKEN);
  c.println(" HTTP/1.1");
  c.println("Content-Type: application/json");
  c.println("Content-Length: " + thisLength);
  c.print("Host: ");
  c.println(URL);
  c.print("\n" + data);
  c.print(char(26));
  
  data = "{\"value\":"+ a + "}";
  thisLength = String(data.length());
  
  // Build HTTP POST request
  c.print("POST /api/v1.6/variables/");
  c.print(VARIABLEID1);
  c.print("/values?token=");
  c.print(TOKEN);
  c.println(" HTTP/1.1");
  c.println("Content-Type: application/json");
  c.println("Content-Length: " + thisLength);
  c.print("Host: ");
  c.println(URL);
  c.print("\n" + data);
  c.print(char(26));
  //////////////////////////////////////////////////////////
   data = "{\"value\":"+ crow + "}";
  thisLength = String(data.length());
  
  // Build HTTP POST request
  c.print("POST /api/v1.6/variables/");
  c.print(VARIABLEID3);
  c.print("/values?token=");
  c.print(TOKEN);
  c.println(" HTTP/1.1");
  c.println("Content-Type: application/json");
  c.println("Content-Length: " + thisLength);
  c.print("Host: ");
  c.println(URL);
  c.print("\n" + data);
  c.print(char(26));
///////////////////////////////////////////////////////////////////

 data = "{\"value\":"+ dust + "}";
  thisLength = String(data.length());
  
  // Build HTTP POST request
  c.print("POST /api/v1.6/variables/");
  c.print(VARIABLEID2);
  c.print("/values?token=");
  c.print(TOKEN);
  c.println(" HTTP/1.1");
  c.println("Content-Type: application/json");
  c.println("Content-Length: " + thisLength);
  c.print("Host: ");
  c.println(URL);
  c.print("\n" + data);
  c.print(char(26));

//////////////////////////////////////////////////////


  // read server response
  
  while (c){
    Serial.print((char)c.read());
  }
  
  c.stop();
  
}


