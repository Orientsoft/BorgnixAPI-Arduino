Introduce
------------
A client library for the Arduino  that provides support for MQTT.  

Base on PubSubclient, BorgnixAPI for arduino platform

The BorgnixClient extend PubSubClient to support borgnix UUID/token certification.

Function
------------
- **BorgnixClient(host, port, uuid, token, callback, client);**  
Construction function, need callback function to deal the in messages.
  callback(message, length)

- **boolean BorgDevConnect(char* ClientID);**
Connect to Borgnix mqtt server, if success connected, return true.

- **void BorgDevDisconnect();**

- **boolean BorgDevSend(char* payload);**
send any payload to Borgnix server.

Example
------------
`code()`

    Markup :  `code()`

```javascript
    var specificLanguage_code = 
    {
        "data": {
            "lookedUpPlatform": 1,
            "query": "Kasabian+Test+Transmission",
            "lookedUpItem": {
                "name": "Test Transmission",
                "artist": "Kasabian",
                "album": "Kasabian",
                "picture": null,
                "link": "http://open.spotify.com/track/5jhJur5n4fasblLSCOcrTp"
            }
        }
    }
```

    Markup : ```javascript
             ```
             
`code()`

    Markup :  `code()`
    ```C++
    //Borgnix Arduino CC3000 wifi example
    // http://www.borgnix.com
    // Arduino JSON library
    // https://github.com/bblanchon/ArduinoJson
  
    #include <Adafruit_CC3000.h>
    #include <ccspi.h>
    #include <SPI.h>
    #include <BorgnixClient.h>
    #include <ArduinoJson.h>
  
  
    ......
    //register Borgnix, get UUID/token from http://dev.borgnix.com
    #define UUID   "6a758a40-c39c-11e4-ac01-b5dcf150caf3"
    #define TOKEN  "595590183827a285474c768dcbff07ea2f780cd9"
  
    //get wifi or ethernet client
    Adafruit_CC3000_Client client = Adafruit_CC3000_Client();
    BorgnixClient borgnixclient("voyager.orientsoft.cn", 11883, UUID,TOKEN, callback, client);
  
  
    void callback (char* topic, byte* payload, unsigned int length) {
  
      // Write the code
  
    }
  
    void setup(void)
    {
      .....
      // Build network connection
      if(borgnixclient.BorgDevConnect(clientId)){
        Serial.println(F("Borgnix Connected"));
      }
      
    }
  
    void loop(void) {
     
  
      borgnixclient.loop();
      delay(1000);
    }

```

    Markup : ```C++
    ```
Installation
------------

Download the zip and extract to Arduino libary path.




