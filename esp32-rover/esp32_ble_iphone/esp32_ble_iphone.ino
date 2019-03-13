/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
   Has a characteristic of: 6E400002-B5A3-F393-E0A9-E50E24DCCA9E - used for receiving data with "WRITE"
   Has a characteristic of: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E - used to send data with  "NOTIFY"

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   In this example rxValue is the data received (only accessible inside that function).
   And txValue is the data to be sent, in this example just a byte incremented every second.
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define LED_BUILTIN 2
#define Ain1 15
#define Ain2 16
#define Bin1 0
#define Bin2 4

int velocity = 255;
void motor(int left, int right);

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
//                              6E400001 B5A3 F393 E0A9 E50E24DCCA9E
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      for (int i = 0; i < rxValue.length(); i++) {
        if (rxValue[i] == 0) rxValue[i] = 'S';
        Serial.print(rxValue[i]);
        switch (rxValue[i]) {
//          case '0': velocity = 0; break;
//          case '1': velocity = 50; break;
//          case '2': velocity = 75; break;
//          case '3': velocity = 100; break;
//          case '4': velocity = 125; break;
//          case '5': velocity = 133; break;
//          case '6': velocity = 140; break;
//          case '7': velocity = 175; break;
//          case '8': velocity = 200; break;
//          case '9': velocity = 225; break;
//          case 'q': velocity = 255; break;
          case 'V': /* tone on */ break;
          case 'v': /* tone off */ break;
          case 'A': motor(velocity, velocity); break; // forward
          case 'C': motor(-velocity, -velocity); break; // backward
          case 'S': motor(0, 0); break; // stop
          case 'D': motor(velocity, -velocity); break; // left
          case 'B': motor(-velocity, velocity); break; // right
          // more keys for LR RL etc...
          case 'G': motor(velocity, 0); break; // forward left
          case 'I': motor(0, velocity); break; // forward right
          case 'H': motor(-velocity, 0); break; // back left
          case 'J': motor(0, -velocity); break; // back right
        }
      }
    }
};


void setup() {
  Serial.begin(115200);

  // Create the BLE Device
  BLEDevice::init("MYNAME");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID_TX,
                        BLECharacteristic::PROPERTY_NOTIFY
                      );

  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_RX,
      BLECharacteristic::PROPERTY_WRITE
                                          );

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");

  int freq = 150000;
  int resolution = 8;
  ledcSetup(0, freq, resolution);
  ledcAttachPin(Ain1, 0);
  ledcSetup(1, freq, resolution);
  ledcAttachPin(Ain2, 1);
  ledcSetup(2, freq, resolution);
  ledcAttachPin(Bin1, 2);
  ledcSetup(3, freq, resolution);
  ledcAttachPin(Bin2, 3);

  pinMode(LED_BUILTIN, OUTPUT);

  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
}

void loop() {

  if (deviceConnected) {
    pTxCharacteristic->setValue(&txValue, 1);
    pTxCharacteristic->notify();
    txValue++;
    delay(10); // bluetooth stack will go into congestion, if too many packets are sent
  }

  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }
}

void motor(int left, int right)
{
  if (left == 0) {
    ledcWrite(1, 255);
    ledcWrite(0, 255);
  } else if (left < 0) {
    ledcWrite(1, -left);
    ledcWrite(0, 0);
  } else {
    ledcWrite(1, 0);
    ledcWrite(0, left);
  }

  if (right == 0) {
    ledcWrite(2, 255);
    ledcWrite(3, 255);
  } else if (right < 0) {
    ledcWrite(2, 0);
    ledcWrite(3, -right);
  } else {
    ledcWrite(2, right);
    ledcWrite(3, 0);
  }
  //  if (left < 0) {
  //    ledcWrite(1, 255);
  //    ledcWrite(0, 255-(-left));
  //  } else {
  //    ledcWrite(0, 255);
  //    ledcWrite(1, 255-left);
  //  }
  //
  //  if (right < 0) {
  //    ledcWrite(3, 255);
  //    ledcWrite(2 , 255-(-right));
  //  } else {
  //    ledcWrite(2, 255);
  //    ledcWrite(3, 255-right);
  //  }
}
