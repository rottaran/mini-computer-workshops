//#include<Adafruit_NeoPixel.h>
#include<BluetoothSerial.h>
BluetoothSerial SerialBT;

/*Servo C_Servo1;
Servo C_Servo2;
Servo C_Servo3;*/

#define LED 32
#define LED_num 12

//Adafruit_NeoPixel C_LED = Adafruit_NeoPixel(LED_num, LED, NEO_GRB + NEO_KHZ800);


#define LED_BUILTIN 2
#define Ain1 15
#define Ain2 16
//#define SLP 27
#define Bin1 0
#define Bin2 4
#define Servo1 33

int velocity = 255;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booted");

  int freq = 250000;
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

  SerialBT.begin("DENNIS-ROBOZ");

  // not-sleep motor driver
  pinMode(33, OUTPUT);
  digitalWrite(33, HIGH);

  for (int i=0; i<5; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);    
  }
  
  //C_Servo1.attach(Servo1);
  //C_Servo2.attach(Servo2);
  //C_Servo3.attach(Servo3);
}

void loop() 
{
  motor(0, 50);
  delay(5000);
  motor(0, 100); 
  delay(5000);
  motor(0, 170);
  delay(5000);
  motor(0, 255);
  delay(5000);
}


void motor(int left, int right)
{
  if (left < 0) {
    ledcWrite(1, 255);
    ledcWrite(0, 255-(-left));     
  } else {
    ledcWrite(0, 255);
    ledcWrite(1, 255-left);
  }

  if (right < 0) {
    ledcWrite(3, 255);
    ledcWrite(2 , 255-(-right));
  } else {
    ledcWrite(2, 255);
    ledcWrite(3, 255-right);    
  }
}
