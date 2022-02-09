/*************************************************************
  WARNING!
    It's very tricky to get it working. Please read this article:
    http://help.blynk.cc/hardware-and-libraries/arduino/esp8266-with-at-firmware

  This example shows how value can be pushed from Arduino to
  the Blynk App.

  NOTE:
  BlynkTimer provides SimpleTimer functionality:
    http://playground.arduino.cc/Code/SimpleTimer

  App project setup:
    Value Display widget attached to Virtual Pin V5
 *************************************************************/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID           "TMPLxxxxxx"
#define BLYNK_DEVICE_NAME           "Device"
#define BLYNK_AUTH_TOKEN            "K7u9FjpbHoJXjgZSU9cYlLXhkXUNTFmt"


// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial


#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Redmi";
char pass[] = "islam2000";

// Hardware Serial on Mega, Leonardo, Micro...
//#define EspSerial Serial1

// or Software Serial on Uno, Nano...
#include <SoftwareSerial.h>
SoftwareSerial EspSerial(2, 3); // RX, TX

// Your ESP8266 baud rate:
#define ESP8266_BAUD 9600

ESP8266 wifi(&EspSerial);

BlynkTimer timer;

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, millis() / 1000);
}
void sendupTime()
{

  float temp =   analogRead(A0);
  temp = temp *(5000/1024);
  temp /= 10;
  Blynk.virtualWrite(V6,temp);
   Blynk.virtualWrite(V0,temp);
  
   if(digitalRead(4) == 1)
  {
    //Blynk.notify("Someone Entered The Room!");

    Blynk.virtualWrite(V1,"Someone Entered!");
    delay(1000);
    Blynk.virtualWrite(V1," ");
    
  }
  if(analogRead(A1)>200)
  {
    //Blynk.notify("Smoke Alarm !");
    Blynk.virtualWrite(V1,"Smoke Alarm!");
    delay(1000);
    Blynk.virtualWrite(V1," ");
  }
}

void setup()
{
  // Debug console
  Serial.begin(9600);
  //pinMode(4,INPUT_PULLUP);
   //pinMode(A1,INPUT);
  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  //Blynk.begin(auth,ssid, pass);
  // You can also specify server:
  Blynk.begin(auth, wifi, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, wifi, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(1000L, sendupTime);
}

void loop()
{

  Blynk.run();
  timer.run(); // Initiates BlynkTimer


 

}
