/*
Code: Saiful Hasan.
Contact: Saiful.hasan@outlook.com


Version: 1.01
Date: 09/05/18
*/

//-----------------------Including library------------------------------//

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include<Ticker.h>


//--------------------ISR for implementing watchdog-------------------//
Ticker secondTick;
volatile int watchdogCount=0;
void ISRwatchdog(){


  watchdogCount++;
  if(watchdogCount==60){


    Serial.println();
    Serial.print("The watch dog bites......");
    ESP.restart();
  }
}


//-----------------Defining required pins---------------------------//



#define lockPin 0              // NODE MCU PIN D1
#define drawinglightPin 4      // NODE MCU PIN D2


//-------------------Defining required variables---------------------//

String lockStatus="0";
String drawingStatus="0";

String Status="";
char msg[6];



//-------------------------------WiFi Credentials--------------------//

const char* ssid = "DataSoft_WiFi";
const char* password = "support123";
//const char* mqtt_server = "broker.mqtt-dashboard.com";
const char* mqtt_server = "182.163.112.207";

WiFiClient espClient;
PubSubClient client(espClient);

//------------------------setup-------------------//

void setup() {
  // put your setup code here, to run once:

Serial.begin(115200);
secondTick.attach(1,ISRwatchdog);

pinMode(lockPin,OUTPUT);
pinMode(drawinglightPin,OUTPUT);

digitalWrite(lockPin,HIGH);
digitalWrite(drawinglightPin,HIGH);


setup_wifi();
client.setServer(mqtt_server, 1883);
client.setCallback(callback);

}


//-------------------------loop-------------------//

void loop() 
{

  watchdogCount=0;

 if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
 //---------------------Updating device status-------------------------------//

    
    Status="";
    Status=Status+lockStatus+drawingStatus;
    Status.toCharArray(msg,6);
    delay(250);
  
/*  pirValue=digitalRead(pirPin);
  delay(100);
  Serial.print("pirValue:");
  Serial.println(pirValue);
      
  if(pirValue== HIGH)
  {

        digitalWrite(pirlightPin,LOW);//Light on
        Serial.println("pir light on");
        
    }
  
  if(pirValue==LOW)

  {
        delay(10000);
        digitalWrite(pirlightPin,HIGH);//Light off
        Serial.println("pir light off");
    
  }*/
 }


//------------------setup_wifi---------------------//

void setup_wifi() {
   delay(100);
  // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


//----------------------reconnect()--------------------------//

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if your MQTT broker has clientID,username and password
    //please change following line to    if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
     //once connected to MQTT broker, subscribe command if any

     //----------------------Subscribing to required topics-----------------------//

     
      client.subscribe("sajib/thesis/light");
      Serial.println("Subsribed to topic: sajib/thesis/light");
      client.subscribe("sajib/thesis/lock");
      Serial.println("Subsribed to topic: sajib/thesis/lock");
      client.subscribe("sajib/thesis/reset");
      Serial.println("Subsribed to topic: sajib/thesis/reset");
      
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 6 seconds before retrying
      delay(5000);
    }
  }
} //end reconnect()


//---------------------------Callback funtion-------------------------------------//


void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  
//------------------------user_input for manual load control-----------------//


  if(strcmp(topic, "sajib/thesis/light") == 0){
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    char data=payload[i];
    
  
    if (data=='0')
    {
      digitalWrite(lockPin,LOW);//UNLOCKED
      Serial.println("Door unlocked");
      lockStatus="0";
      }
      if (data=='1')
    {
      digitalWrite(lockPin,HIGH);//Locked
      Serial.println("Door locked");
      lockStatus="1";
      }
      
      if (data=='2')
    {
      digitalWrite(drawinglightPin,LOW);//LIGHT ON
      //digitalWrite(drawingtvPin,LOW);
      
      Serial.println("Light on");
      //Serial.println("drawing tv on");
      drawingStatus="1";
      Serial.print("drawingStatus:");
      Serial.println(drawingStatus);
      }
      
      
      if (data=='3')
    {
      
      digitalWrite(drawinglightPin,HIGH);//LIGHT OFF
      //digitalWrite(drawingtvPin,HIGH);
      drawingStatus="0";
     
      Serial.println("Light off");
      //Serial.println("drawing tv off");
     
      Serial.print("drawingStatus:");
      Serial.println(drawingStatus);
      }
       
  }}


//-------------------------Publishing device status upon request from user app--------------------------//


  if(strcmp(topic, "sajib/thesis/login") == 0){
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    char data2=payload[i];
   
   
   if (data2=='1')
    {

      client.publish("apartment/status/fair",msg);
      Serial.print("Published Status:");
      Serial.println(msg);
    }
            
      }

  }

//----------------------Restarting the board from Engineer's end----------------------------//



  if(strcmp(topic, "sajib/thesis/reset") == 0){
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    char data3=payload[i];
    
  
    if (data3=='1')
    {
      
      Serial.println("Resetting Device.........");
       ESP.restart();
      }}}
       

  
}
