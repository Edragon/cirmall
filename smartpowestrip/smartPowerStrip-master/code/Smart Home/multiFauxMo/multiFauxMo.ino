#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>
#include "switch.h"
#include "UpnpBroadcastResponder.h"

// Function prototypes
boolean connectWifi();
bool 	output1On();
bool 	output1Off();
bool	output2On();
bool 	output2Off();
bool	output3On();
bool	output3Off();

const char* ssid 		= "9F35FE";
const char* password 	= "55463423";
const int 	outputPin1 	= 12;
const int 	outputPin2 	= 13;
const int 	outputPin3	= 16;

UpnpBroadcastResponder 	upnpBroadcastResponder;
Switch 					*itemOne 	= NULL;
Switch 					*itemTwo	= NULL;
Switch					*itemThree	= NULL;

boolean wifiConnected = false;
 /*------------------------------------------------
	Set-up function. Runs once
--------------------------------------------------*/ 
void setup()
{
  Serial.begin(9600);
  pinMode(outputPin1, OUTPUT);
  pinMode(outputPin2, OUTPUT);
  pinMode(outputPin3, OUTPUT);

  wifiConnected = connectWifi();
  
  if(wifiConnected){
    upnpBroadcastResponder.beginUdpMulticast();

    itemOne 	= new Switch("item one", 	80, output1On, output1Off);
    itemTwo 	= new Switch("item two", 	81, output2On, output2Off);
	itemThree	= new Switch("item three",  82, output3On, output3Off);

    upnpBroadcastResponder.addDevice(*itemOne);
    upnpBroadcastResponder.addDevice(*itemTwo);
	upnpBroadcastResponder.addDevice(*itemThree);
  }
}

 /*------------------------------------------------
	Main loop
--------------------------------------------------*/ 
void loop()
{
	 if(wifiConnected){
      upnpBroadcastResponder.serverLoop();
      
      itemOne->serverLoop();
	  itemTwo->serverLoop();
	  itemThree->serverLoop();
	 }
}


/*------------------------------------------------
	Device-control Functions
--------------------------------------------------*/ 

bool output1On() {
    Serial.println("Switch 1 turn on ...");
    digitalWrite(outputPin1, HIGH);
	return true;
}

bool output1Off() {
    Serial.println("Switch 1 turn off ...");
    digitalWrite(outputPin1, LOW);
	return false;
}

bool output2On() {
    Serial.println("Switch 2 turn on ...");
    digitalWrite(outputPin2, HIGH);
	return true;
}

bool output2Off() {
  Serial.println("Switch 2 turn off ...");
  digitalWrite(outputPin2, LOW);
  return false;
}

bool output3On() {
    Serial.println("Switch 3 turn on ...");
    digitalWrite(outputPin3, HIGH);
	return true;
}

bool output3Off() {
  Serial.println("Switch 3 turn off ...");
  digitalWrite(outputPin3, LOW);
  return false;
}

/*------------------------------------------------
	Set-up functions
--------------------------------------------------*/ 
boolean connectWifi(){
  boolean 	state 			= true;
  int 		connectAttemps 	= 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("\nConnecting to WiFi...");
  
  while (WiFi.status() != WL_CONNECTED && connectAttemps < 10) {
    delay(500);
    Serial.print(".");
	connectAttemps++;
  }
  if(connectAttemps >= 10)	state = false;
  
  if (state){
    Serial.print("\nConnected to ");    Serial.println(ssid);
    Serial.print("IP address: ");    	Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("\nConnection failed.");
	// TODO: turn LED ON. Then restart device after some time
  }
  
  return state;
}
