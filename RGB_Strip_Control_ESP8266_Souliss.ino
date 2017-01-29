/**************************************************************************
    Souliss - RGB LED Strip control from an ESP8266
    
    This is a RGB LED Strip Driver/Controller driven by the Souliss Framework.  
    
    Load this code on ESP8266 board using the porting of the Arduino core
    for this platform.
	
	The 3 output control 3 IRLZ44N Mosfet.
	In the actual configuration the channel are configured in this way:
	Red   channel = GPIO14
	Green channel = GPIO4
	Blue  channel = GPIO5
	
	Code is not yet optimised for any energy saving and will always be active.
	DHCP is used to get the IP address. Use serial output (76800) to get the actual IP adress.
        
***************************************************************************/

// Let the IDE point to the Souliss framework
#include "SoulissFramework.h"

// Configure the framework
#include "bconf/MCU_ESP8266.h"              // Load the code directly on the ESP8266
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node

// Include framework code and libraries
#include <ESP8266WiFi.h>
#include "Souliss.h"

// **** Define the WiFi name and password ****
#define WIFICONF_INSKETCH
#define WiFi_SSID               "YourWifiName"
#define WiFi_Password           "YourWifiPassword"    

// Include framework code and libraries
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "Souliss.h"

// **** Define here the right pin for your ESP module **** 
#define	OUTPUTPIN			BUILTIN_LED

#define LEDCONTROL          0               // This is the memory slot for the logic that handle the light
#define LEDRED              1               // This is the memory slot for the logic that handle the light
#define LEDGREEN            2               // This is the memory slot for the logic that handle the light
#define LEDBLUE             3               // This is the memory slot for the logic that handle the light


void setup()
{   
    delay(10);
    Serial.begin(76800);
    delay(20);
    Serial.println("Going to initialize");
    Initialize();
    Serial.println("Initialisation finished");

	// Connect to the WiFi network and get an address from DHCP
    GetIPAddress();  
	//Uncomment the next line to define a fixed IP address (consult Soulis documentation for more info)	
 // SetIPAddress(ip_address, subnet_mask, ip_gateway);                           
    SetAsGateway(myvNet_dhcp);       // Set this node as gateway for SoulissApp  
    Serial.println("Received IP address: ");
    Serial.println(WiFi.localIP());
	// This is the vNet address for this node, used to communicate with other
	// nodes in your Souliss network
    SetAddress(0xAB01, 0xFF00, 0x0000);
    SetAsPeerNode(0xAB02, 1);
    Serial.println("Node set as peer");
    
    Set_T16(LEDCONTROL);
	
    
	// Define outputs pins
    
    pinMode(4, OUTPUT);                 // Blue LED output
    pinMode(5, OUTPUT);                 // Green LED output
    pinMode(14, OUTPUT);                // Red LED output
}
// Following variable are used to save souliss set LED value. Souliss use 8 bit PWM output (0-255),
// whereas ESP8266 have 10 bit PWM output (0-1023). The variable are used to make the conversion.
int ledBlue = 0;
int ledGreen = 0;
int ledRed = 0;
void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        FAST_10ms() {   // We process the logic and relevant input and output every 10 milliseconds
            Logic_T16(LEDCONTROL);
            ledBlue = mOutput(LEDBLUE);
            ledGreen = mOutput(LEDGREEN);
            ledRed = mOutput(LEDRED);
            if (ledBlue != 0) {
              analogWrite(4, (ledBlue+1)*4); 
            } else {
              analogWrite(4, 0); 
            }
            if (ledGreen != 0) {
              analogWrite(5, (ledGreen+1)*4); 
            } else {
              analogWrite(5, 0); 
            }
            if (ledRed != 0) {
              analogWrite(14, (ledRed+1)*4); 
            } else {
              analogWrite(14, 0); 
            }
            ProcessCommunication();
        } 

        // Here we handle here the communication with Android
        FAST_GatewayComms();                                        
    }
    EXECUTESLOW() { 
        UPDATESLOW();

        SLOW_10s() {                
            // Timer associated to the LED logic control
            Timer_T16(LEDCONTROL); 
			// Test phase output to ensure device is running
			Serial.println("Device is still running ");			
        }     

    } 
} 
