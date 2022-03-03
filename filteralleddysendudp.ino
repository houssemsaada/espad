#include "WiFi.h"
#include "WiFiUdp.h"
#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
#include "BluetoothSerial.h"
#include "BLEScan.h"
#include "BLEAdvertisedDevice.h"
#include "BLEBeacon.h"
#include "BLEEddystoneTLM.h"
#include "BLEEddystoneURL.h"
#include "math.h" ;
#include "sys/time.h"
#include "BLEAddress.h"
#include "string"  
// WiFi network name and password:
const char * networkName = "Scheidt & Bachmann";
const char * networkPswd = "GusT_AC3ss%SbM";
//const char * networkName = "BadKarma";
//const char * networkPswd = "houssem1234";
float mesure ;
double distance ; 
//IP address to send UDP data to:
const char * udpAddress = "192.168.162.125";
const int udpPort = 5015;
const char * udpAddressm = "192.168.162.66";
const int udpPortm = 5015;
boolean connected = false;
BLEScan* pBLEScan;
static BLEAddress *pServerAddress;
String ipv;
BLEClient*  pClient;
int scanTime = 5; //In seconds
uint16_t beconUUID = 0xFEAA;
//#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00)>>8) + (((x)&0xFF)<<8))
//The udp library class
WiFiUDP udp;
//BluetoothSerial SerialBT;
BLEAdvertising *pAdvertising;  
void setBeacon() {
  char beacon_data[22];
  uint16_t beconUUID = 0xFEAA;
   BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
   oAdvertisementData.setFlags(0x06); // GENERAL_DISC_MODE 0x02 | BR_EDR_NOT_SUPPORTED 0x04
  oAdvertisementData.setCompleteServices(BLEUUID(beconUUID));

    beacon_data[0] = 0x10;  // Eddystone Frame Type (Eddystone-URL)
    beacon_data[1] = 0x20;  // Beacons TX power at 0m
    beacon_data[2] = 0x02;  // URL Scheme 'http://'
    beacon_data[3] = 'E';  // URL add  1
    beacon_data[4] = 'S';  // URL add  2
    beacon_data[5] = 'P';  // URL add  3
    beacon_data[6] = '1';  // URL add  4
    beacon_data[7] = '1';  // URL add  5
    beacon_data[8] = '1';  // URL add  6
    beacon_data[9] = '1';  // URL add  7
    beacon_data[10] = '1';  // URL add  8
    beacon_data[11] = '1';  // URL add  9
    beacon_data[12] = '1';  // URL add 10
    beacon_data[13] = '1';  // URL add 11
    beacon_data[14] = '1';  // URL add 12
    beacon_data[15] = '1';  // URL add 13
    beacon_data[16] = '1';  // URL add 14
    beacon_data[17] = '1';  // URL add 15
    beacon_data[18] = '1';  // URL add 16
    // beacon_data[19] = '5';  // URL add 17
   
  oAdvertisementData.setServiceData(BLEUUID(beconUUID), std::string(beacon_data, 18));
  
  pAdvertising->setScanResponseData(oAdvertisementData);
}  
 class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 public:

  void printDouble( double val, unsigned int precision){
// prints val with number of decimal places determine by precision
// NOTE: precision is 1 followed by the number of zeros for the desired number of decimial places
// example: printDouble( 3.1415, 100); // prints 3.14 (two decimal places)

    Serial.print (int(val));  //prints the int part
    Serial.print("."); // print the decimal point
    unsigned int frac;
    if(val >= 0)
        frac = (val - int(val)) * precision;
    else
        frac = (int(val)- val ) * precision;
    Serial.println(frac,DEC) ;
} 
void onResult(BLEAdvertisedDevice advertisedDevice) {
     pServerAddress = new BLEAddress(advertisedDevice.getAddress());
      std::string strServiceData = advertisedDevice.getServiceData();
       uint8_t cServiceData[100];
       strServiceData.copy((char *)cServiceData, strServiceData.length(), 0);
if (advertisedDevice.getServiceDataUUID().equals(BLEUUID(beconUUID))==true) {  // found Eddystone UUID   
        Serial.printf("is Eddystone: %d %s length %d\n", advertisedDevice.getServiceDataUUID().bitSize(), advertisedDevice.getServiceDataUUID().toString().c_str(),strServiceData.length());
        if (cServiceData[0]==0x10) {
           BLEEddystoneURL oBeacon = BLEEddystoneURL();
           oBeacon.setData(strServiceData);
           Serial.print("{\"URL\":\"");
           Serial.print(oBeacon.getDecodedURL().c_str());
         
           Serial.print("\",\"RSSI\":");
           Serial.print((int)advertisedDevice.getRSSI());
       
           Serial.print(",\"Mac\":\"");
           Serial.print(pServerAddress->toString().c_str());
            Serial.print("\"}");
         
         //  calcul de distance  
    // for (int i=0; i<20; i++) {
   mesure= pow(10,((-69-float(advertisedDevice.getRSSI()))/30)) ;
   
   // distance +=mesure ; 
   // }
//distance=distance/20 ;
       Serial.print("mesure\n");
    printDouble(mesure,100);

//inet_ntop(WiFi.localIP(),ipv);
 //Serial.print(ipv);
char *s = new char[200];
        //send to udp 
udp.beginPacket(udpAddress,udpPort);
      

strcpy(s,"{\"URL\":\"");
strcat(s,oBeacon.getDecodedURL().c_str());
strcat(s,"\",\"RSSI\":");
strcat(s,String(advertisedDevice.getRSSI()).c_str());
strcat(s,",\"Mac\":\"");
strcat(s,pServerAddress->toString().c_str());
strcat(s,"\",\"IP\":\"");
strcat(s,ipv.c_str());
strcat(s,"\"}");
udp.print(s);
/* 
 udp.print("{\"URL =\":");
 udp.print(oBeacon.getDecodedURL().c_str());
 udp.println("}");

 
 udp.print((int)advertisedDevice.getRSSI());
 udp.println("}");
 udp.print("{\"Mac\":");
 udp.print(pServerAddress->toString().c_str());
 udp.println("}");
*/
    udp.endPacket(); 
    delay(20);
    udp.beginPacket(udpAddressm,udpPortm);
    udp.print(s);
    udp.endPacket(); 
        } 
       
        else {
          for (int i=0;i<strServiceData.length();i++) {
            Serial.printf("[%X]",cServiceData[i]);
          }
        }
        Serial.printf("\n");
}}};

void setup() {
 Serial.begin(115200);

Serial.printf("start ESP32 \n");

  // Create the BLE Device
  BLEDevice::init("ESP32");
  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pAdvertising = pServer->getAdvertising();
   setBeacon();
   // Start advertising
  pAdvertising->start();
  Serial.println("Advertizing started...");
//scan section
 Serial.println("Scanning...");
 pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
//Connect to the WiFi network
  connectToWiFi(networkName, networkPswd);
}

void loop() {
   
   BLEScanResults FoundDevices = pBLEScan->start(scanTime);
}
  
void connectToWiFi(const char * ssid, const char * pwd){
  Serial.println("Connecting to WiFi network: " + String(ssid));

  // delete old config
  WiFi.disconnect(true);
  //register event handler
  WiFi.onEvent(WiFiEvent);
  
  //Initiate connection
  WiFi.begin(ssid, pwd);

  Serial.println("Waiting for WIFI connection...");
}

//wifi event handler
void WiFiEvent(WiFiEvent_t event){
    switch(event) {
      case SYSTEM_EVENT_STA_GOT_IP:
          //When connected set 
          Serial.print("WiFi connected! IP address: "); 
          ipv = WiFi.localIP().toString().c_str() ;
          Serial.println(WiFi.localIP());
          //initializes the UDP state
          //This initializes the transfer buffer
          udp.begin(WiFi.localIP(),udpPort);
          connected = true;
          break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
          Serial.println("WiFi lost connection");
          connected = false;
          break;
    }
}
