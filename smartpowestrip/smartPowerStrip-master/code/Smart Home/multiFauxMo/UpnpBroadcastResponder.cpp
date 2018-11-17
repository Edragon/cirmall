#include "UpnpBroadcastResponder.h"
#include "Switch.h"
#include <functional>
 
// Multicast declarations
char 				packetBuffer[512];   
IPAddress			ipMulti(239, 255, 255, 250);
const unsigned int 	portMulti = 1900;


#define MAX_SWITCHES 14
Switch 	switches[MAX_SWITCHES] = {};
int 	numOfSwitchs 		   = 0;

//#define numOfSwitchs (sizeof(switches)/sizeof(Switch)) //array size  
 
/*------------------------------------------------
	Constructor and destructor
--------------------------------------------------*/ 
UpnpBroadcastResponder::UpnpBroadcastResponder(){
    
}

UpnpBroadcastResponder::~UpnpBroadcastResponder(){/*nothing to destruct*/}
 
 
 /*------------------------------------------------
	Set-up functions
--------------------------------------------------*/ 
 void UpnpBroadcastResponder::addDevice(Switch& device) {
  Serial.print("Adding switch : ");  Serial.print(device.getAlexaInvokeName());
  Serial.print(" index : ");		 Serial.println(numOfSwitchs);
  
  switches[numOfSwitchs] = device;		// adds the address of the Switch device object to an array
  numOfSwitchs++;
}
 
/*------------------------------------------------
	Responses
--------------------------------------------------*/ 
bool UpnpBroadcastResponder::beginUdpMulticast(){
  boolean state = false;  
  Serial.println("Begin multicast ..");
  
  if(UDP.beginMulticast(WiFi.localIP(), ipMulti, portMulti)) {
    Serial.print("Udp multicast server started at ");
    Serial.print(ipMulti);    Serial.print(":");    Serial.println(portMulti);

    state = true;
  }
  else{
    Serial.println("Connection failed");
  }
  
  return state;
}



void UpnpBroadcastResponder::serverLoop(){
  int packetSize = UDP.parsePacket();
  if (packetSize <= 0)    return;
  
  IPAddress 	senderIP 	= UDP.remoteIP();
  unsigned int 	senderPort 	= UDP.remotePort();  
  UDP.read(packetBuffer, packetSize);  
  String request			= String((char *)packetBuffer);

  if(request.indexOf('M-SEARCH') > 0) {
      if((request.indexOf("urn:Belkin:device:**") > 0) || (request.indexOf("ssdp:all") > 0) || (request.indexOf("upnp:rootdevice") > 0)) {
        Serial.println("Got UDP Belkin Request..");
      
        for(int n = 0; n < numOfSwitchs; n++) {
            Switch &sw = switches[n];

            if (&sw != NULL) {
              sw.respondToSearch(senderIP, senderPort);              
            }
        }
      }
  }
}

