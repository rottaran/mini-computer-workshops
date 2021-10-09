#include <BluetoothSerial.h>
BluetoothSerial SerialBT;

//#include <Servo.h>
//Servo myservo;

#define LED_BUILTIN 2
#define Ain1 14
#define Ain2 12
#define Bin1 33
#define Bin2 32
#define SLP 27
#define SERVO 25

int velocity = 200;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting...");
  
  // let the motor driver sleep
  pinMode(SLP, OUTPUT);
  digitalWrite(SLP, LOW); 

  // configure the 4 Pulse Width Modulation channels
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

  // init the servo
  //myservo.attach(SERVO, 4, 0, 255);  // pin 32, pwm channel 4
  //myservo.write(150);

  pinMode(LED_BUILTIN, OUTPUT);
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }

  // start bluetooth classic
  SerialBT.begin("rover-1");
}

void loop()
{
  // check bluetooth connection
  if (!SerialBT.hasClient()) {
    motor(0, 0);
    digitalWrite(SLP, LOW);
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    digitalWrite(SLP, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
  }
  
  if (SerialBT.available() > 0)
  {
    char bt = SerialBT.read();
    Serial.println(bt);

    switch (bt) {
      case '0': velocity = 150; break;
      case '1': velocity = 160; break;
      case '2': velocity = 170; break;
      case '3': velocity = 180; break;
      case '4': velocity = 190; break;
      case '5': velocity = 200; break;
      case '6': velocity = 210; break;
      case '7': velocity = 220; break;
      case '8': velocity = 230; break;
      case '9': velocity = 240; break;
      case 'q': velocity = 255; break;
//      case 'W': myservo.write(150); break;
//      case 'w': myservo.write(150); break;
      case 'V': 
      for (int i=85; i>=10; i--) { 
//        myservo.write(i);
        delay(10);
      } 
      break;
      case 'v':
      for (int i=10; i<=85; i++) { 
//        myservo.write(i);
        delay(20);
      } 
      break;
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
}
