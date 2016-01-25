MySensors 2.0 and HomeAssistant 11.1 MQTT communication

This is the update for the automation home system with MySensors and HomeAssistant using MQTT protocol.
News:
 - updated versions of MySensors 2.0 and HomeAssistant 11.1;
 - for MQTT 3.1.1 external broker is needed, for example mosquitto (on the previous version the MQTT broker was included into the gateway);
 - the gateway allow sensors.
 
My project include multisensor-nodes and also multisensor-Gateway with several examples sketches, each of them tested and working.

 
The Gateway

The Gateway main components are:
 - Arduino UNO or Mega;
 - Ethernet Shield W5100;
 - NRF24+ radio module;
 - sensors as needed.
The connection with NRF24L+ radio module must be made as described on MySensors site at the Ethernet Gateway build section for the UNO board. For the Mega board, the connection is detailed into the sketch, the colors are the same as described on MySensors site.
The sketch for this gateway is based on GatewayW5100MQTTClient from MySensors. I have modified them to get DHCP address from the router and to add sensors there. Also the radio connection is modified for Arduino Mega.
Before compile and upload this sketch you have to:
  - modify the "byte TCP_MAC[]" to have unique MAC address inside your network;
  - update the MQTT broker address at: #define MY_CONTROLLER_IP_ADDRESS;
  - update the MQTT broker port #define MY_PORT 1883 (usually no need to change here);
  - check the sensors into the sketch, add or delete sensors as needed; take care about CHILD_IDs and PINs;
  - update the OneWire address for each device (for DS18BS temperature sensor).

  
Router

No changes are requested on the router side for this version of MySensors. If you wold like, you can assign a static IP address for your MQTT Gateway based on the MAC defined in the MQTTGateway.ino, for example 192.168.1.101. You can check the right assignament by pinging the gateway:
   ping 192.168.1.101


Sensors

For the sensors the main components are:
 - Arduino Nano; (UNO or Mega may be used too);
 - NRF24L+ module;
 - sensors as needed.
I have started to built single sensor modules, tested each of them individually, then combine several sensors into single node module and test them again.
The starting point is the "Sensors build area" from MySensors site. 
Start with "Connection the radio", this part is exactly the same as presented on the site.
Then choose the sensor type according to your needs, find the relevant sketch into my repositories and uploade it to the Arduino. Before upload, please check and edit (if necessary) the following:
  - node ID from #define NODE_ID; they must be unique for each node, the value could be between 1 and 254;
  - sensor PIN from #define sensor_PIN; check the available free PINs for your Arduino board;
  - update interval for the sensor: tUpdate in ms; it is usefull also to check if the node is alive by checking the last time update on the controller;
  - if not connect to the MQTT, try to upgrade the mosquitto to the latest version from http://mosquitto.org/ 
Several Linux distribution are not using by default the last version of mosquitto but a older version that support only MQTT 3.1 and not 3.1.1:
sudo apt-add-repository ppa:mosquitto-dev/mosquitto-ppa
sudo apt-get update
Then install the latest version of mosquitto from Software Manager (take care, there may be also the old version available).

Special remark for my gas/smoke detection: my programm take the lowest measured value of the sensor and keep it as zero  refference. It send to the controller the difference between the actual reading from the sensor pin and this refference. You have to decide yourself the treshhold for alarm activation (e.g 20 units).

If you need any other sensors combination into one module, you can start from individual sensor sketch and add them into one single sketch .ino file, I hope my examples are clear enough to do this very easy.
If you reach the maximum size of the Arduino Nano memory, you have to disable DEBUG by comment the line with "//" (explained also on MySensors site). If still run out of memory, you have to replace the Arduino Nano with UNO (small improvement)or even Mega (BIG improvement).

If you go for Arduino Mega, the radio connection is the same as mentioned into the Arduino Mega Gateway, please look carefully at PINs assignament.


HomeAssistant

If you use the HomeAssistant controller as I did, please install the software as presented on their site at "Getting started" section, then start the server with
$ hass --open-ui 
afterward stop it with Ctrl-C. In this moment you should have a default configuration.yaml file into your directory HOME/.homeassistant. If you do not see any .homeassistant dir, in the file browser go to HOME dir then press Ctrl-H.
You have to edit this configuration.yaml file and add your sensors there. I have one example of the configuration.yaml file that works with most of my single sensors and with several combined sensors too(for some of sensors examples small adjustements may have to be performed to match the sensor topic area: node ID, sensor ID, payload).
At mqtt broker section you must also edit the IP address of your MQTT broker, in my example it is 192.169.1.101.

When you will add other sensors or switches, just copy the code from one of the examples, then adjust them for matching the nodeID and the sensorID and they are ready to use.


Learned from experience

While working and performed tests on this project, I encountered several issues I would like to mention here to pay attention on them:
  - the payload for binary sensors must be letters or start with letters, not just numbers, for example ON / OFF, not 0 / 1 otherwise they don't work! (may be this was solved in the latest version, didn't check yet);
  - when starting HomeAssistant, it set all relays to OFF; I think it is possible to change that, I am looking for a solution;
  - the NRF24L+ does not offer large area radio coverage, but enough for one appartment; they are some radio improvement advices on the net; I used a monopol antenna, practically it is a 31mm wire that replace the on-board antenna.
  

What I have used

They are differences between platforms and software versions, so I mention here my setup that works for me:
  - sensor network: MySensors 2.0;
  - Controller: HomeAssistant v.11.1 installed on LinuxMint 17.3, python 3.4 (default on LinuxMint), pip3;
  - mosquitto upgraded to 1.4.7 for MQTT 3.1.1;
  - Gateway: Arduino Uno, Ethernet Shield W5100 and NRF24L+ (tested also with Arduino Mega 2560), sensors;
  - nodes: Arduino Nano, NRF24L+ and sensors;
  - Arduino IDE 1.6.5;
  - tested sensors: switches, digital input, analog input, relays, IR receiver, PIR, MQ-2, MQ-5, DHT11, DHT22, DS18B20, water leak (digital), sound (digital), vibration (digital), Light KY018 (analog), distance HC-SR04.

  
Refferences

Most part of this project is inspired from the following sites:
http://www.mysensors.org/
https://home-assistant.io/
https://www.arduino.cc/


I hope my work presented here will help other people to quickly implement their own projects and save their time.





