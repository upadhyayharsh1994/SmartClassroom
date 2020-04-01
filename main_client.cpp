#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <SPI.h>
#include <LoRa.h>
#include "pins_arduino.h"

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "1366e33f-7ddb-4891-a3d2-03abe2a703c1"
#define CHARACTERISTIC_UUID "8b5e9d07-b509-4d68-b983-becfea66f9c4"

BLECharacteristic *pCharacteristic;
BLEServer *pServer;
BLEService *pService;
BLEAdvertising *pAdvertising;
int x = 0;
void setup() {
  pinMode(34, INPUT_PULLUP);  
  pinMode(35, INPUT_PULLUP);
  pinMode(26, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("Starting BLE work!");
  Serial.println(x);
  BLEDevice::init("92 Akash_Shah");


  pServer = BLEDevice::createServer();
  pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );


  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");


SPI.begin(LORA_SCK,LORA_MISO,LORA_MOSI,LORA_CS);
 LoRa.setSPI(SPI);
 LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);
  Serial.println("LoRa");
  LoRa.available();
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
    if(digitalRead(34) == HIGH)
    {
      Serial.println("1 Done");
    LoRa.beginPacket();
    LoRa.print("92 Akash_Shah-A");
    LoRa.endPacket();
    }
    else if(digitalRead(35) == HIGH)
    {
        Serial.println("2 Done");
      LoRa.beginPacket();
      LoRa.print("92 Akash_Shah-B");
      LoRa.endPacket();
    }
    else if(digitalRead(26) == HIGH)
    {
        Serial.println("3 Done");
      LoRa.beginPacket();
      LoRa.print("92 Akash_Shah-C");
      LoRa.endPacket();
    }
}