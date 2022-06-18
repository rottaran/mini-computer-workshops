#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <vector>
#include <map>
#include <set>

#define Ain1 14
#define Ain2 12
#define Bin1 33
#define Bin2 32
#define SERVO 25
#define MOTORSLEEP 27

#define VELOCITY 240
#define ROTATE 240

static BLEUUID serviceUUID ("6490cc9f-e782-4bf9-a90f-b5e9af5f005c");
static BLEUUID charUUID    ("7a67ab6c-e057-4462-8b40-feb463be3cdf");

// forward declarations
class MyBleButton;
enum ButtonRoles { BTN_LEFT, BTN_RIGHT, BTN_BACK, BTN_FORWARD };

// global variables
std::set<MyBleButton*> buttons; // the linked BLE buttons
std::set<ButtonRoles> freeRoles; // robot actions, currently not assigned to buttons
std::map<BLERemoteCharacteristic*, MyBleButton*> notifications;
bool btnStates[4] = {
  false,
  false,
  false,
  false,
};


class MyBleButton 
  : public BLEClientCallbacks
{
public:
  enum States { CONNECTING, CONNECTED, DISCONNECTED };

  MyBleButton(BLEAdvertisedDevice const& pAdvertisedDevice, ButtonRoles pRole) {
    role = pRole;
    advertisedDevice = pAdvertisedDevice;
    state = CONNECTING;

    Serial.print("button connected: ");
    Serial.println(advertisedDevice.toString().c_str());

    pClient = BLEDevice::createClient();
    pClient->setClientCallbacks(this);
    pClient->connect(&advertisedDevice);
    Serial.println("connect started");

    Serial.print(" id ");
    Serial.println(pClient->getConnId());

    pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      state = DISCONNECTED;
      return;
    }
    Serial.println(" - Found our service");

    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      state = DISCONNECTED;
      return;
    }
    Serial.println(" - Found our characteristic");

    // Read the value of the characteristic.
    if(pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("The characteristic value was: ");
      //Serial.println(value.c_str());
      if (value[0] == 1) Serial.print("1");
      else Serial.print("0");
      Serial.println();
      btnStates[role] = value[0];
    }

    if(pRemoteCharacteristic->canNotify()) {
      notifications.insert(std::make_pair(pRemoteCharacteristic, this));
      pRemoteCharacteristic->registerForNotify(&MyBleButton::notifyCallback);
    } else {
      Serial.println("notify could not be registered :-(");
      state = DISCONNECTED;
    }
  }

  ~MyBleButton() {
    notifications.erase(pRemoteCharacteristic);
    pClient->disconnect();
    delete pClient;
  }

  static void notifyCallback(
    BLERemoteCharacteristic* pBLERemoteCharacteristic,
    uint8_t* pData,
    size_t length,
    bool isNotify) {
      // TODO file bug to get access to the internal pointer to the BLEService ?
      Serial.print("Notify callback from ");
      Serial.print(notifications[pBLERemoteCharacteristic]->pClient->getPeerAddress().toString().c_str());
      //Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
      Serial.print(" of data length ");
      Serial.println(length);
      Serial.print("data: ");
      if (*pData == 1) Serial.print("1");
      else Serial.print("0");
      Serial.println();

      // do something with the new value
      btnStates[notifications[pBLERemoteCharacteristic]->role] = pData[0];
  }

  void onConnect(BLEClient* pclient) {
    Serial.print("connected: ");
    Serial.println(pclient->getPeerAddress().toString().c_str());
    state = CONNECTED;
  }

  void onDisconnect(BLEClient* pclient) {
    Serial.print("disconnected: ");
    Serial.println(pclient->getPeerAddress().toString().c_str());
    state = DISCONNECTED;
  }

  bool isDisconnected() const { return state == DISCONNECTED; }

public:
  ButtonRoles role;

protected:
  BLEAdvertisedDevice advertisedDevice;
  States state;
  BLEClient* pClient;
  BLERemoteService* pRemoteService;
  BLERemoteCharacteristic* pRemoteCharacteristic;
};



class MyAdvertisedDeviceCallbacks
  : public BLEAdvertisedDeviceCallbacks
{
public:
  std::vector<BLEAdvertisedDevice*> found;

  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
      // TODO add locking?
      found.push_back(new BLEAdvertisedDevice(advertisedDevice));
    }
  }
};

MyAdvertisedDeviceCallbacks madvs;

void setup() {
  Serial.begin(115200);

  Serial.println("setting up the motors");
  pinMode(2, OUTPUT);
  digitalWrite(2, 1); // LED
  setupmotor();

  freeRoles.insert(BTN_LEFT);
  freeRoles.insert(BTN_RIGHT);
  freeRoles.insert(BTN_BACK);
  freeRoles.insert(BTN_FORWARD);

  Serial.println("setting up BLE scanning");
  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan();
  //pBLEScan->setAdvertisedDeviceCallbacks(&madvs);
  //pBLEScan->setInterval(100);
  //pBLEScan->setWindow(99);  // less or equal setInterval value
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true); // active scan uses more power, but get results faster

  Serial.println("start BLE scan");
  //pBLEScan->start(5, false);
}

void loop() {
  // put your main code here, to run repeatedly:

  if (freeRoles.size() > 0) {
    BLEScan* pBLEScan = BLEDevice::getScan();
    BLEScanResults foundDevices = pBLEScan->start(5, false); // 5sec scan, not continue

    for (int i=0; i < foundDevices.getCount(); i++) {
      auto advertisedDevice = foundDevices.getDevice(i);
      Serial.print("BLE Advertised Device found: ");
      Serial.println(advertisedDevice.toString().c_str());

      if (!advertisedDevice.haveServiceUUID() || !advertisedDevice.isAdvertisingService(serviceUUID)) continue;
      if (freeRoles.size() == 0) continue;

      auto role = *freeRoles.begin();
      freeRoles.erase(freeRoles.begin());

      auto pb = new MyBleButton(advertisedDevice, role);
      buttons.insert(pb);
    }
  } else {
    // stop scanning if full
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->stop();
  }

  // check for lost connections
  for (auto pb: buttons) {
    if (pb->isDisconnected()) {
      freeRoles.insert(pb->role);
      buttons.erase(pb);
      //pBLEScan->start(0, false); // 1sec scan, restart
    }
  }

  int inputX = 0;
  int inputY = 0;

  if (btnStates[BTN_FORWARD]) {
    inputY += VELOCITY;
  }
  if (btnStates[BTN_BACK]) {
    inputY -= VELOCITY;
  }
  if (btnStates[BTN_RIGHT]) {
    inputX += ROTATE;
  }
  if (btnStates[BTN_LEFT]) {
    inputX -= ROTATE;
  }

  // if (inputX != 0 || inputY != 0) {
  //   Serial.print(inputX);
  //   Serial.print(" ");
  //   Serial.println(inputY);
  // }

  motor(inputY+inputX, inputY-inputX);

  delay(5);
}


void setupmotor() {
  pinMode(MOTORSLEEP, OUTPUT);
  digitalWrite(MOTORSLEEP, HIGH);
  int freq = 120000;
  int resolution = 8;
  ledcSetup(0, freq, resolution);
  ledcAttachPin(Ain1, 0);
  ledcSetup(1, freq, resolution);
  ledcAttachPin(Ain2, 1);
  ledcSetup(2, freq, resolution);
  ledcAttachPin(Bin1, 2);
  ledcSetup(3, freq, resolution);
  ledcAttachPin(Bin2, 3);
}

void motor(int left, int right) {
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
}


