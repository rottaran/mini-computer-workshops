#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>


#define LED_PIN 25
#define BUTTON_PIN 27
#define SLEEP_AFTER 60 // s
#define MIN_LOOP_TIME 5 //ms


BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t value = 0;
uint8_t oldValue = 0;
uint32_t sleep_timer = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "6490cc9f-e782-4bf9-a90f-b5e9af5f005c"
#define CHARACTERISTIC_UUID "7a67ab6c-e057-4462-8b40-feb463be3cdf"


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      digitalWrite(2, HIGH);
      Serial.println("Connect");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      digitalWrite(2, LOW);
      pServer->startAdvertising();
      Serial.println("Disconnect");
    }
};



void setup() {
  Serial.begin(115200);
  print_wakeup_reason();
  
  pinMode(LED_PIN, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
 
  BLEDevice::init("ESP32BUTTON");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  pCharacteristic->setValue((uint8_t*)&value, 1);
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
    value = !digitalRead(BUTTON_PIN);
    digitalWrite(LED_PIN, value);
    
    if (value == 0) {
        sleep_timer ++;  
    } else {
        sleep_timer = 0;
    }
    
    if (deviceConnected) {
        digitalWrite(LED_PIN, value);
        if (value != oldValue) {
            Serial.println("button changed");
            pCharacteristic->setValue((uint8_t*)&value, 1);
            pCharacteristic->notify();
        }

        if (sleep_timer >= SLEEP_AFTER*3*1000/MIN_LOOP_TIME) {
          sleep();
        }
    } else {
        if (sleep_timer >= SLEEP_AFTER*1000/MIN_LOOP_TIME) {
            sleep();
        }
    }

    if (deviceConnected != oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
    }
    
    oldValue = value;
    delay(MIN_LOOP_TIME);
}

void sleep() {
  Serial.println("DEEP SLEEP!");
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_27, 0);
  esp_deep_sleep_start();
}

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason){
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}
