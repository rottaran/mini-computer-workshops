/*
PID-Windtunnel: Ein einfacher PID-Regler für Vertikal-Windtunnel.
Copyright (C) 2018  Randolf Rotta

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
    
#include "FastLED.h"

// Pinbelegung
constexpr byte PIN_LEDS = 6;
constexpr byte PIN_POTI = A0;
constexpr byte PIN_FAN = 9;
constexpr byte PIN_TRIGGER = 7;
constexpr byte PIN_ECHO = 8;

// weitere Konstanten
constexpr byte NUM_LEDS = 60;
constexpr int16_t uss_unten = 5300; // Abstand vom Ultraschallsensor bis Ball in 0-Punkt, in micro-Sekunden
constexpr byte led_unten = 3; // LED für Null-Punkt
constexpr byte led_oben = 58; // LED für Endanschlag, korrigieren falls Abweichung im oberen Bereich
constexpr uint16_t zeitschritt = 25; // Milli-Sekunden, Abstand zwischen den Ultraschall-Messungen

//======== PID Parametersätze ============================================================================================
#if 1   // P ohne offset
constexpr float offset = 0;
float kP = 1.0;
float kI = 0.0000;
float kD = 000.00;
#elif 1 // P mit offset
constexpr float offset = 118;
float kP = 0.1;
float kI = 0.0000;
float kD = 000.00;
#elif 1 // PI
constexpr float offset = 0;
float kP = 0.05;
float kI = 0.00001;
float kD = 000.00;
#else // PID
constexpr float offset = 0;
float kP = 3.0;
float kI = 0.0005;
float kD = 600.00;
#endif 
//========================================================================================================================

// Zustand des PID-Reglers
int16_t sollwert = 0; // Führungsgröße: z.B. mit Poti einstellbar 
int16_t istwert = 0; // Regelgröße: mit Ultraschall oder LoF gemessene Distanz
int16_t steuergroesse = 0; // was an den lüfter geht
int32_t abweichung = 0; // Regeldifferenz
int32_t abweichung_alt = 0; // vom vorherigen Zeitschritt für die Ableitung
float integral = offset;
//int32_t istwert_alt = 0; //  vom vorherigen Zeitschritt für die Ableitung
CRGB leds[NUM_LEDS]; // Zustand der LED-Leiste


void setup() {
  Serial.begin(115200);
  Serial.println("PID-Windtunnel  Copyright (C) 2018  Randolf Rotta\n"
    "This program comes with ABSOLUTELY NO WARRANTY. "
    "This is free software, and you are welcome to redistribute it "
    "under certain conditions; see the code for details.");

  FastLED.addLeds<WS2812B, PIN_LEDS, GRB>(leds, NUM_LEDS);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();

  // Potentiometer für die Wunschhöhe
  pinMode(PIN_POTI, INPUT);

  // Lüfter
  pinMode(PIN_FAN, OUTPUT);
  //  PWM konfigurieren: non-inverted, 10bit fast PWM, no prescaling, 15.625kHz
  OCR1A = 0;
  ICR1 = 0xFFFF;
  TCCR1A = (TCCR1A & 0b00111100) | 0b10000011;
  TCCR1B = (TCCR1B & 0b11100000) | 0b00001001;

  // Ultraschall Entfernungssensor
  pinMode(PIN_TRIGGER, OUTPUT);   // Trigger
  digitalWrite(PIN_TRIGGER, LOW);
  pinMode(PIN_ECHO, INPUT);    // Echo
}


void loop() {
  //delay(zeitschritt);
  static uint32_t next_time = millis();
  if (int(next_time - millis()) > 0) return;
  next_time += zeitschritt;

  uint32_t start_time = millis();

  // Poti auslesen
  sollwert = analogRead(A0);

  // Ultraschallsensor abfragen
  noInterrupts();
  digitalWrite(7, HIGH); //Trigger Impuls 10 us
  delayMicroseconds(10);
  digitalWrite(7, LOW);
  // Echo-Zeit messen, maximum = 1000*1000*2/340 ist ca. 6000
  int32_t uss_zeit = pulseIn(8, HIGH, 6000); 
  interrupts();
  // auf selben Groessenbereich wie Poti bringen, unten ist 0
  istwert = map(uss_zeit, 0, uss_unten, 1023, 0);

  //==== PID-Regler ========================================================================================================
  abweichung = sollwert - istwert;
  float ableitung = float(abweichung-abweichung_alt)/zeitschritt;
  abweichung_alt = abweichung;
  integral = constrain(integral + kI*zeitschritt*abweichung, -500, 2000);
  steuergroesse = kP*abweichung + integral + kD*ableitung;
  //========================================================================================================================
  
  //float ableitung = float(istwert_alt-istwert)/zeitschritt;
  //istwert_alt = istwert;

  // Lüfter ansteuern
  OCR1A = constrain(steuergroesse,0,1023); //analogWrite(9, steuergroesse);

  // Ausgabe auf LED Leiste
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  leds[constrain(map(steuergroesse,0,1023,led_unten,led_oben),0,NUM_LEDS-1)].b = 60; 
  leds[constrain(map(sollwert,0,1023,led_unten,led_oben),0,NUM_LEDS-1)].r = 255;
  leds[constrain(map(istwert,0,1023,led_unten,led_oben),0,NUM_LEDS-1)].g = 255; 
  FastLED.show();

  // Ausgabe auf Serielle Leitung
//  Serial.print(uss_zeit);
  Serial.print("\t");
  Serial.print(steuergroesse);
  Serial.print("\t");
  Serial.print(sollwert);
  Serial.print("\t");
//  Serial.print(abweichung);
  Serial.print("\t");
  Serial.print(istwert);
//  Serial.print("\t");
//  Serial.print(abweichung);
  Serial.print("\t");
  Serial.print(integral);
//  Serial.print("\t");
//  Serial.print(kD*ableitung);
//  uint32_t end_time = millis();
//  Serial.print("\t");
//  Serial.print(end_time -start_time);
//  Serial.print("\t");
//  Serial.print(start_time);
//  Serial.print("\t");
//  Serial.print(next_time);
  Serial.println();  
}

