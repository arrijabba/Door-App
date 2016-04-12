/*
Eaton Door Monitoring and Release

  Program that reads if a door bell has been pressed. If it has been pressed,
  sends a signal through a Chat Server to an iPhone app, written by Chen Su, to 
  notify a user that someone is at the door. The user then can send an open or 
  do not open signal through the chat server to the arduino which will close a relay
  to complete a circuit.
  
 Chat Server

 A simple server that distributes any incoming messages to all
 connected clients.  To use telnet to  your device's IP address and type.
 You can see the client's input in the serial monitor as well.

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.


 Circuit:
 * WiFi shield attached

 Chat Server
 created 18 Dec 2009
 by David A. Mellis
 modified 31 May 2012
 by Tom Igoe
 
 button pressing and doorBell reading/writing
 added by Matt Mayrosh and Chen Su
 11/9/2015
 modified
 11/30/2015

modified 4/12/2016 by arri m.
 */

#include <SPI.h>
#include <WiFi.h>

char ssid[] = ""; //  your network SSID (name)
char pass[] = "";    // your network password (use for WPA, or use as key for WEP)

int keyIndex = 0;            // your network key Index number (needed only for WEP)
int buttonState1 = 0;        // initial value of button1 (doorBell) is 0
const int doorBell = 6;      // set pin 6 as doorBell into the arduino (pin 9 was reserved)
const int relayLed = 8;      // set pin 8 as relay input
byte buttonState2 = -1;
unsigned long oldMilli = 0;


int status = WL_IDLE_STATUS;

WiFiServer server(80);

boolean alreadyConnected = false; // whether or not the client was connected previously

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  //Set Pin Modes
  pinMode (doorBell, INPUT);
  pinMode (relayLed, OUTPUT);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if ( fv != "1.1.0" )
    Serial.println("Please upgrade the firmware");

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  
  //you're connected now, so print out the data
  Serial.print("You're connected to the network");
  printWifiStatus();
  //printWifiData();

  // start the server:
  server.begin();
}

void loop() {
  // wait for a new client:
  WiFiClient client = server.available();
  // tell iPhone that the door bell has been pressed
  buttonState1 = digitalRead(doorBell);

  //force 5 seconds in between rings. delay helps to keep ios read buffer clear
  if (buttonState1 == HIGH && (millis()-oldMilli > 5000UL)){
    
    client.flush();
    client.write ("RING");
    oldMilli = millis();
    Serial.println ("RING");
  }
  else
  {
    //wait
  }
  

  // when the client sends the first byte, say hello:
  if (client) {
    if (!alreadyConnected) {
      // clead out the input buffer:
      client.flush();
      Serial.println("We have a new client");
      client.println("Connected");
      
      alreadyConnected = true;
      

    }
    
    if (client.available() > 0) {
      // read the bytes incoming from the client:
      buttonState2 = client.read();
      
      // echo the bytes to the serial monitor as well:
      Serial.write(buttonState2);
      }
      
     //set button state to close relay or keep open
      if (buttonState2 == 0) {
        digitalWrite (relayLed,HIGH);
        delay(10000);
        digitalWrite (relayLed, LOW);
        buttonState2 = -1;
        
      }
    }

  } 
  // Reset arduino if the server is dead.
 //if (server.status() != 1) {
   //digitalWrite(2, HIGH); // Reset Arduino
 //}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println("dBm");
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  Serial.print(bssid[5], HEX);
  Serial.print(":");
  Serial.print(bssid[4], HEX);
  Serial.print(":");
  Serial.print(bssid[3], HEX);
  Serial.print(":");
  Serial.print(bssid[2], HEX);
  Serial.print(":");
  Serial.print(bssid[1], HEX);
  Serial.print(":");
  Serial.println(bssid[0], HEX);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}
