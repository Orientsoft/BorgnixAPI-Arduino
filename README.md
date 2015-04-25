Introduce
------------
A client library for the Arduino  that provides support for MQTT.  

Base on PubSubclient, BorgnixAPI for arduino platform

The BorgnixClient extend PubSubClient to support borgnix UUID/token certification.

Function
------------
- **BorgnixClient(host, port, uuid, token, callback, client);**  
Construction function, need callback function to deal the in messages.
>callback(message, length)

- **boolean BorgDevConnect(char* ClientID);**
Connect to Borgnix mqtt server, if success connected, return true.

- **void BorgDevDisconnect();**

- **boolean BorgDevSend(char* payload);**
send any payload to Borgnix server.


Installation
------------

Download the zip and extract to Arduino libary path.




