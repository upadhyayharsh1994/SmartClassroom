#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <string>
#include <vector>
#include <set>
#include <iterator>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include "pins_arduino.h"
#include <NTPClient.h>
#include<SPI.h>
#include<LoRa.h>

using namespace std;
int scanTime = 5; 
BLEScan* pBLEScan;
 BLEClient*  pClient ;
static BLERemoteCharacteristic* pRemoteCharacteristic;
BLERemoteService* pRemoteService;
static BLEUUID serviceUUID("1366e33f-7ddb-4891-a3d2-03abe2a703c1");
static BLEUUID    charUUID("8b5e9d07-b509-4d68-b983-becfea66f9c4");
const char* ssid = "LAPTOP-P4HHO4P1";
const char* password = "Abcd1233";
const char* awsEndpoint = "a1y1zh2yxjgx9j-ats.iot.us-west-2.amazonaws.com";
const char* topic = "arn:aws:iot:us-west-xxx:thing/Node1";
const char* topic2 = "arn:aws:iot:us-west-xxx:thing/Node2";
const char* subscribeTopic = topic;
const char* subscribeTopic2 = topic2;
const char* certificate_pem_crt = \

"-----BEGIN CERTIFICATE-----\n" \
"-----END CERTIFICATE-----\n";

// xxxxxxxxxx-private.pem.key
const char* private_pem_key = \

"-----BEGIN RSA PRIVATE KEY-----\n" \

"-----END RSA PRIVATE KEY-----\n";

// This key should be fine as is. It is just the root certificate.
const char* rootCA = \
"-----BEGIN CERTIFICATE-----\n" \

"-----END CERTIFICATE-----\n";

WiFiClientSecure wiFiClient;
static BLEAdvertisedDevice* myDevice;

void msgReceived(char* topic, byte* payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient);
void pubSubCheckConnect();
set<string> student_data;
std::map<string,string> answer;
void onReceive(int packetSize);


void setup() {
  Serial.begin(9600);
  Serial.println("Scanning...");
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); 
  pBLEScan->setActiveScan(true); 
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99); 
  delay(50);
  Serial.printf("SDK version: %s\n", ESP.getSdkVersion());

  Serial.print("Connecting to "); 
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();
  Serial.print(", WiFi connected, IP address: "); 
  Serial.println(WiFi.localIP());

  wiFiClient.setCACert(rootCA);
  wiFiClient.setCertificate(certificate_pem_crt);
  wiFiClient.setPrivateKey(private_pem_key);


  SPI.begin(LORA_SCK,LORA_MISO,LORA_MOSI,LORA_CS);
 LoRa.setSPI(SPI);
 LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);
  Serial.println("LoRa Receiver");
    if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.onReceive(onReceive);
  LoRa.receive();
}




void loop() {
  // put your main code here, to run repeatedly:
  pubSubCheckConnect();
  set<string> tempData;
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  for(int i=0;i<foundDevices.getCount();i++)
  {
      String x = foundDevices.getDevice(i).toString().c_str();
      if(isDigit(x.substring(x.indexOf(':')+2,x.indexOf(','))[0]))
      {
        tempData.insert(x.substring(x.indexOf(':')+2,x.indexOf(',')).c_str());
      }
  }
  if(tempData.size() > student_data.size())
  {
    for(int i=0;i<foundDevices.getCount();i++)
    {
      String x = foundDevices.getDevice(i).toString().c_str();
      String realData = x.substring(x.indexOf(':')+2,x.indexOf(','));
      if(tempData.find(realData.c_str()) != tempData.end() && student_data.find(realData.c_str()) == student_data.end())
      {
        StaticJsonBuffer<300> JSONbuffer;
        JsonObject& JSONencoder = JSONbuffer.createObject();
        JSONencoder["key_value"] = realData.substring(0,realData.indexOf(" "));
        JSONencoder["name"] = realData.substring(realData.indexOf(" ") + 1);
        char JSONmessageBuffer[100];
        JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
        boolean rc = pubSubClient.publish(topic,JSONmessageBuffer);
        student_data.insert(realData.c_str());
        Serial.print(realData.c_str());
        Serial.println(" came to class!!");
      }
    }
  }
  else if(tempData.size() < student_data.size())
  {
      for(string f:student_data)
      {
        if(tempData.find(f) == tempData.end())
        {
            String x = f.c_str();
            StaticJsonBuffer<300> JSONbuffer;
            JsonObject& JSONencoder = JSONbuffer.createObject();
            JSONencoder["key_value"] = x.substring(0,x.indexOf(" "));
            char JSONmessageBuffer[100];
            JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
            boolean rc = pubSubClient.publish(topic2,JSONmessageBuffer);
            student_data.erase(f);
            answer.erase(f);
            Serial.print(f.c_str());
            Serial.println(" left the class!!");
        }
      }
  }
  else
  {
    Serial.println("----------------------------------");
    if(student_data.size() > 0)
    {
    if(answer.size() == 0)
    {
      Serial.println("No answers from students!!");
    }
    else
    {
    std::map<string, string>::iterator it;
    for ( it = answer.begin(); it != answer.end(); it++ )
    {
        string name = it->first;
        string val = it->second;
        Serial.print(name.c_str());
        Serial.print(" answered ");
        Serial.println(val.c_str());
    }
    }
    }
    else
    {
      Serial.println("No students present");
    }
    Serial.println("----------------------------------");
  }
  pBLEScan->clearResults();
  pClient = NULL;
  delay(5000);
}


void msgReceived(char* topic, byte* payload, unsigned int length) {
  // Serial.print("Message received on "); 
  // Serial.print(topic); 
  // Serial.print(": ");
  // for (int i = 0; i < length; i++) {
  //   Serial.print((char) payload[i]);
  // }
  // Serial.println();
}

void pubSubCheckConnect() {
  if (!pubSubClient.connected()) {
    Serial.print("PubSubClient connecting to: "); 
    Serial.print(awsEndpoint);
    while (!pubSubClient.connected()) {
      Serial.print(".");
      pubSubClient.connect("ESPthingXXXX");
      delay(1000);
    }
    Serial.println(" connected");
    pubSubClient.subscribe(subscribeTopic);
  }
  pubSubClient.loop();
}

void onReceive(int packetSize) {

  String sname;
  for (int i = 0; i < packetSize; i++) {
    sname.concat((char)LoRa.read());
  }

  if(isDigit(sname.substring(0,2)[0] ) && isDigit(sname.substring(0,2)[1] ) && student_data.find(sname.substring(0,sname.indexOf('-')).c_str()) != student_data.end())
  {
    answer[sname.substring(0,sname.indexOf('-')).c_str()] = sname.substring(sname.indexOf('-')+1).c_str();
  }


}
