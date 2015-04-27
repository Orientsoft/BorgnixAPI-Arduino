//Borgnix Arduino CC3000 wifi example
// http://www.borgnix.com
// wangz
// Arduino JSON library
// https://github.com/bblanchon/ArduinoJson

#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <BorgnixClient.h>
#include <ArduinoJson.h>


// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!

// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10

// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIVIDER);

#define WLAN_SSID       "SSID"
#define WLAN_PASS       "wpa_password"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2
  
//register Borgnix, get UUID/token from http://dev.borgnix.com
#define UUID   "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
#define TOKEN  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"


Adafruit_CC3000_Client client = Adafruit_CC3000_Client();
BorgnixClient borgnixclient("z.borgnix.com", 1883, UUID,TOKEN, callback, client);


void callback (char* topic, byte* payload, unsigned int length) {


  //Decode Borgnix payload example "{"sensor":"LED","data":[13,1]}"
  StaticJsonBuffer<200> jsonBuffer;
  
  JsonObject& root = jsonBuffer.parseObject((char *)payload);

  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }
  
  const char* sensor = root["sensor"];
  int pin = root["data"][0];
  int ep = root["data"][1];


  if (!strcmp(sensor,"LED")) {
     Serial.println(F("LED control"));
     if (ep == 1) 
       digitalWrite(pin, HIGH);   // turn the LED on (HIGH is the voltage level)
     if (ep == 0)
       digitalWrite(pin, LOW);
  } 
  
  borgnixclient.BorgDevSend("{\"result\":\"OK\"}");

}

void setup(void)
{
  Serial.begin(115200);
  Serial.println(F("Hello, CC3000!\n"));

  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);

  // If you want to set the aref to something other than 5v
  analogReference(EXTERNAL);
  
  Serial.println(F("\nInitialising the CC3000 ..."));
  if (!cc3000.begin()) {
    Serial.println(F("Unable to initialise the CC3000! Check your wiring?"));
    for(;;);
  }

  uint16_t firmware = checkFirmwareVersion();
  if (firmware < 0x113) {
    Serial.println(F("Wrong firmware version!"));
    for(;;);
  }
  
  displayMACAddress();
  
  Serial.println(F("\nDeleting old connection profiles"));
  if (!cc3000.deleteProfiles()) {
    Serial.println(F("Failed!"));
    while(1);
  }

  /* Attempt to connect to an access point */
  char *ssid = WLAN_SSID;             /* Max 32 chars */
  Serial.print(F("\nAttempting to connect to ")); Serial.println(ssid);
  
  /* NOTE: Secure connections are not available in 'Tiny' mode! */
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while(1);
  }
   
  Serial.println(F("Connected!"));
  
  /* Wait for DHCP to complete */
  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP()) {
    delay(100); // ToDo: Insert a DHCP timeout!
  }

  /* Display the IP address DNS, Gateway, etc. */  
  while (!displayConnectionDetails()) {
    delay(1000);
  }
   
  //Attention! clientId can NOT exceed 8 byte. It will cause buffer overflow 
  String clientIdStr = "Borg_" + String(random(1000));
  int clientId_len = clientIdStr.length() + 1;
  char clientId[clientId_len];
  clientIdStr.toCharArray(clientId, clientId_len);    

  if(borgnixclient.BorgDevConnect(clientId)){
    Serial.println(F("Borgnix Connected."));
  }
  
}

void loop(void) {
 
  // are we still connected?
  if (!borgnixclient.connected()) {
      borgnixclient.loop();
      delay(1000);
   }

}


/**************************************************************************/
/*!
    @brief  Tries to read the CC3000's internal firmware patch ID
*/
/**************************************************************************/
uint16_t checkFirmwareVersion(void)
{
  uint8_t major, minor;
  uint16_t version;
  
#ifndef CC3000_TINY_DRIVER  
  if(!cc3000.getFirmwareVersion(&major, &minor))
  {
    Serial.println(F("Unable to retrieve the firmware version!\r\n"));
    version = 0;
  }
  else
  {
    Serial.print(F("Firmware V. : "));
    Serial.print(major); Serial.print(F(".")); Serial.println(minor);
    version = major; version <<= 8; version |= minor;
  }
#endif
  return version;
}

/**************************************************************************/
/*!
    @brief  Tries to read the 6-byte MAC address of the CC3000 module
*/
/**************************************************************************/
void displayMACAddress(void)
{
  uint8_t macAddress[6];
  
  if(!cc3000.getMacAddress(macAddress))
  {
    Serial.println(F("Unable to retrieve MAC Address!\r\n"));
  }
  else
  {
    Serial.print(F("MAC Address : "));
    cc3000.printHex((byte*)&macAddress, 6);
  }
}


/**************************************************************************/
/*!
    @brief  Tries to read the IP address and other connection details
*/
/**************************************************************************/
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}
