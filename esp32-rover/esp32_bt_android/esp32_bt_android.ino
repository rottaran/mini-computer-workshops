//#include<Adafruit_NeoPixel.h>
#include<BluetoothSerial.h>
BluetoothSerial SerialBT;

/*Servo C_Servo1;
  Servo C_Servo2;
  Servo C_Servo3;*/

//#define LED 32
//#define LED_num 12

//Adafruit_NeoPixel C_LED = Adafruit_NeoPixel(LED_num, LED, NEO_GRB + NEO_KHZ800);


#define LED_BUILTIN 2
#define Ain1 15
#define Ain2 0
#define Bin1 4
#define Bin2 16
#define SLP 17

int velocity = 128;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booted");

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

  SerialBT.begin("MYNAME");

  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }

  // not-sleep motor driver
  pinMode(SLP, OUTPUT);
  digitalWrite(SLP, HIGH);
}

void loop()
{
  digitalWrite(LED_BUILTIN, SerialBT.hasClient());
  if (!SerialBT.hasClient()) motor(0, 0);
  if (SerialBT.available() > 0)
  {
    char bt = SerialBT.read();
    Serial.println(bt);

    switch (bt) {
      case '0': velocity = 0; break;
      case '1': velocity = 50; break;
      case '2': velocity = 75; break;
      case '3': velocity = 100; break;
      case '4': velocity = 125; break;
      case '5': velocity = 133; break;
      case '6': velocity = 140; break;
      case '7': velocity = 175; break;
      case '8': velocity = 200; break;
      case '9': velocity = 225; break;
      case 'q': velocity = 255; break;
      case 'V': /* tone on */ break;
      case 'v': /* tone off */ break;
      case 'F': motor(velocity, velocity); break; // forward
      case 'B': motor(-velocity, -velocity); break; // backward
      case 'S': motor(0, 0); break; // stop
      case 'L': motor(velocity, -velocity); break; // left
      case 'R': motor(-velocity, velocity); break; // right
      // more keys for LR RL etc...
      case 'G': motor(velocity, 0); break; // forward left
      case 'I': motor(0, velocity); break; // forward right
      case 'H': motor(-velocity, 0); break; // back left
      case 'J': motor(0, -velocity); break; // back right
    }

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
