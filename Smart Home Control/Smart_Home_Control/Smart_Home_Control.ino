#define DEBUG true
#include <SoftwareSerial.h>
SoftwareSerial esp8266(2,3);
#define serialCommunicationSpeed 9600  
#include <LiquidCrystal_I2C.h>
#include <Wire.h> 
LiquidCrystal_I2C lcd(0x27,20,4); 

int buzzer = 5;
int temp = A0;
int smoke = A1;
int pir = 4;
int lamp = 11;
void setup()
{
  Serial.begin(serialCommunicationSpeed);    ///////For Serial monitor 
  esp8266.begin(serialCommunicationSpeed); ///////ESP Baud rate
  pinMode(lamp,OUTPUT);    /////used if connecting a LED to pin 11
  pinMode(pir,INPUT);
  pinMode(temp,INPUT);
  pinMode(smoke,INPUT);
  pinMode(buzzer,OUTPUT);
  digitalWrite(11,HIGH);

  sendData("AT+RST\r\n",2000,DEBUG);                                          // reset module
  sendData("AT+CWMODE=2\r\n",1000,DEBUG);                                    // configure as access point
  sendData("AT+CIFSR\r\n",1000,DEBUG);                                      // get ip address
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG);                                  // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG);                           // turn on server on port 80
  
  digitalWrite(11,LOW);
  lcd.init();                                                           // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(2,0);
  lcd.print("Smart home");
  lcd.setCursor(4,1);
  lcd.print("Control");
}


float sensetemp() ///////function to sense temperature.
 {
  int val = analogRead(temp);
  float mv = val *(5000/1024); 
  float celcius = mv/10;
  return(celcius);
 }
int smokesense()
{
  int SMOKEVAL = analogRead(smoke);
  //Serial.println(SMOKEVAL);
  //digitalWrite(6,HIGH);
  return(SMOKEVAL);
}
int PIRsense()
{
  int PIRVAL = digitalRead(pir);
  return(PIRVAL);
}
 
int connectionId;
void loop()
{
      String IncomingString="";
      boolean StringReady = false;          
  if(esp8266.available())
  {
 
    /////////////////////Recieving from web browser to toggle led
    if(esp8266.find("+IPD,"))
    {
      delay(1000);
      connectionId = esp8266.read()-48;
      IncomingString=esp8266.readString();
      StringReady= true;
      /////////////////////Recieving from web browser to toggle led
      if (StringReady)
      {
        Serial.println("Received String: " + IncomingString);
      
        if (IncomingString.indexOf("ON") != -1) {
          digitalWrite(11,HIGH);
         }
     
        if (IncomingString.indexOf("OFF") != -1) {
          digitalWrite(11,LOW);
         }
      }
   
    /////////////////////Sending data to browser
      String webpage = "<h1> Smart Home Control Panel</h1> ";
      webpage+="<button style=\"margin: 10px;\" id=\"111\" class=\"button\">LAMP ON</button>";
      webpage+="<br>";
      webpage+="<button style=\"margin:10px;\" id=\"110\" class=\"button\">LAMP OFF</button>";
      espsend(webpage); 
    
     if(sensetemp() < 100)
     {
      
        String add1="<h4>Temperature=</h4>";
        String two =  String(sensetemp(), 3);
        add1+= two;
        add1+="&#x2103";   //////////Hex code for degree celcius
        espsend(add1);
        lcd.clear();
        lcd.setCursor(1,0);
        lcd.print("Temp = ");
        lcd.print(two);
        lcd.print((char)223);
        lcd.print('C');
        delay(2000);
        lcd.clear();
      
     }
    
     else
     {
      String c="<br>";
      c+="<h4>Temperature sensor is not conneted</h4>";
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("No connection");
      espsend(c);                                     
     } 
     
     if(smokesense() >= 200)
     {
       String add1="<script>alert(\"SMOKE ALARM!\");</script>";
       lcd.setCursor(2,0);
       lcd.print("SMOKE ALARM !");
       tone(buzzer,1000);
       delay(2000);
       noTone(buzzer);
       tone(buzzer,1000);
       delay(2000);
       noTone(buzzer);
       espsend(add1);
     }
     if(PIRsense() == 1)
     {
       String add1="<script>alert(\"SOMEONE ENTERED THE ROOM!\");</script>";
       lcd.setCursor(1,0);
       lcd.print("SOMEONE ENTERED");
       lcd.setCursor(4,1);
       lcd.print("THE ROOM!");
       espsend(add1);
     }
     String closeCommand = "AT+CIPCLOSE=";  ////////////////close the socket connection////esp command 
     closeCommand+=connectionId; // append connection id
     closeCommand+="\r\n";
     sendData(closeCommand,3000,DEBUG);
    }
  }
}
 
  //////////////////////////////sends data from ESP to webpage///////////////////////////
 
 void espsend(String d)
         {
             String cipSend = " AT+CIPSEND=";
             cipSend += connectionId; 
             cipSend += ",";
             cipSend +=d.length();
             cipSend +="\r\n";
             sendData(cipSend,1000,DEBUG);
             sendData(d,1000,DEBUG); 
         }

//////////////gets the data from esp and displays in serial monitor///////////////////////         
String sendData(String command, const int timeout, boolean debug)
            {
                String response = "";
                esp8266.print(command);
                long int time = millis();
                while( (time+timeout) > millis())
                {
                   while(esp8266.available())
                      {
                         char c = esp8266.read(); // read the next character.
                         response+=c;
                      }  
                }
                
                if(debug)
                     {
                     Serial.print(response); //displays the esp response messages in arduino Serial monitor
                     }
                return response;
            }
