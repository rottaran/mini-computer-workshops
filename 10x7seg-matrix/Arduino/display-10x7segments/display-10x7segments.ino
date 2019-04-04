#include <SPI.h>

/** wiring summary: 
 *  
 * Three shift registers 74x595 are attached to ATmega's hardware SPI module.
 * D10 is SS slave select, low active.
 * D11 is MISO master out slave in.
 * D13 is SCK serial clock.
 * As usual, he first byte transmitted goes to the last shift register. 
 * 
 * The first shift register addresses the 8 segments of all digits. It is
 * high active, i.e. a 1 switches on the low side NPN transistor. The shift
 * register's output enable is connected to D9 for PWM-based dimming of the 
 * digits. It is connected to a 10k pullup resistor in order to switch of 
 * all segments while the ATmega is booting.
 * 
 * The second and third shift registers activates one of the 10 digits. It is
 * low active, i.e. a 0 switches on the hight side p-channel MOSFET. Only one
 * digit should be active at any time. The shift register's output enable is
 * connected to ground in order to have these outputs enables all the time.
 * 
 * Note: Output enable on HIGH, i.e. disabled, puts the output pins into high
 * impedance. No current will flow in or out of these pins then. Therefore, you 
 * cannot use a n-channel MOSFET on the low side. It would simply not switch off
 * when the segments output becomes disabled.
 * 
 * A 10 to 22 Ohm resistor in series to each segment's LED is used. The resistor
 * is shared by all 10 digits. It is put in front of the segment's low side 
 * transistor's collector. Beware that these can become hot (2,5V*100mA=250mW) 
 * because all of the segment's current will flow through it. In practice two
 * 1206 SMD resistors were used with 22 Ohm for red and green and 10 Ohm for blue.
 * This could be simplified because the brightness of the digits differs anyway 
 * depending on the color due to the different efficiency.
 * 
 * Four 100uF SMD capacitors between VCC and ground are placed throughout the
 * display board. This reduced voltage variation from 4.2--5.3V to 4.6--5V. Still,
 * for some digits patterns, you can hear some high frequency noise from the 
 * power bank's voltage converter.
 */

/** principle of operation:
 *  
 *  The activated bits of each digit are stored in the "segments" array.
 *  
 *  The PWM pulse width for each digit is stored in the "brightness" array. 
 *  Because output enable is inverted, 255-x gives the actual pulse width.
 *  
 *  Multiplexing across the digits is used (persistence of vision). Just one
 *  digit is active at any time and all digits share the same 8 segment lines.
 *  
 *  The ATmega's Timer1 is used to generate the pulse width modulated
 *  output enable signal for digit dimming. In addition its overflow interrupt 
 *  is used to cycle through the digits.
 *  
 *  Beware that the register for the pulse width (OCR1A) is double buffered. It
 *  becomes effective at the next overflow. Therefore, we have to quickly push the 
 *  new digit and its segments to the shift registers when the overflow interrupt
 *  is triggered. This is achieved by a trick: The new values were already
 *  transmitted at the end of the previous overflow interrupt. We just need to 
 *  tell the shift register to take them over to their outputs. This is achieved by
 *  setting the slave select pin to high. After that, we have at least 256 cycles to
 *  load the next PWM value and preload the next digit into the shift registers.
 *  
 *  The timer is configured in 8-bit fast-PWM mode with non-inverted output.
 *  This gives a resolution of 256 brightness levels. The base frequency is high 
 *  enough to actually use 10-bit PWM. This would be useful because the pulse width, 
 *  the LEDs brightness, and the eye's perception have no linear relation. 
 *    
 *  The clock divider could be set to clk/8, giving 2MHz counting frequency and 
 *  2/256MHz = 7.8kHz switching between the digits. With 10 digits this results in 
 *  a refresh rate of 780Hz. The overflow timer interrupt implementation takes around 64 cycles. This results 
 *  in a processor load around 3%.
 *  
 *  Note: clock divider clk/64 looks still alright without flicker. But the
 *  multiplexing is visible when moving the display quickly or making videos of it.
 *  
 *  Note: 10-bit PWM and clock divider clk/1 gives 15.6kHz digit switching, 
 *  1.5kHz refresh rate, and 6.2% processor load. This would increase the 
 *  effectiveness of the display board's capacitors. 
 *  
 *  Currently, 9-bit clk/1 is used
 */

// brightness for the 10 digits
volatile uint8_t brightness[10];

// the segment bits for the 10 digits
volatile uint8_t segments[10];

void init7seg();
void put7seg(uint8_t pos, char c);
void write7seg(char const* str, int start=0);
void dimming(int8_t pos, uint8_t bright) { brightness[pos] = bright; }
void colorDimming(uint8_t brightRT, uint8_t brightGN, uint8_t brightBL);
void dot7seg(uint8_t pos, bool on=true);

void setup() {
  init7seg();
  colorDimming(64,80,20);
  Serial.begin(57600);
  Serial.println("hello world.");
}

void loop() {
  Serial.println("running...");

// if (1) {
//  write7seg("0123456789");
//  delay(5000);
//  write7seg("ABCDEF");
//  delay(5000);

//  for (int i=0; i<100; i++) {
//    write7seg("");
//    dot7seg(i%10);
//    delay(50);
//  }
//  }
  
//  if (1) {
//  for (int d=1; d<70; d = d+10) {
//    for (int i=-11; i<11; i++) {
//      write7seg("HALLO JANA", -i);
//      delay(d);
//    }
//  }
//  }
//    for (int i=-11; i<=0; i++) {
//      write7seg("HALLO JANA", -i);
//      delay(100);
//    }

  for (int i=0; i<10; i++) dimming(i,0);
  write7seg("HALLO JANA", 0);
  for (int d=0; d<200; d = d+10) {
    for (int i=0; i<10; i++) dimming(i,d);
    delay(50);
  }
  for (int d=0; d<100; d = d+10) {
    for (int i=0; i<10; i++) dimming(i,200-d);
    delay(50);
  }
  colorDimming(64,80,20);
  delay(5000);

// new code

  int waitTime = 1000;

  // inital value is 08
  write7seg("08  008   ", 0);
  delay(waitTime);

  // 08 => 15
  write7seg("080 008   ", 0);
  delay(waitTime);

  write7seg("0808008   ", 0);
  delay(waitTime);

  write7seg("0818008   ", 0);
  delay(waitTime);

  write7seg("0815008   ", 0);
  delay(waitTime);

  write7seg("0815108   ", 0);
  delay(waitTime);

  // valid value is 15
  write7seg("08151  15 ", 0);
  delay(waitTime);


  // loop 
  for (int i=0; i<20; i++){
  
    // 15 => 23
    write7seg("18151  15 ", 0);
    delay(waitTime);
  
    write7seg("15151  15 ", 0);
    delay(waitTime);
  
    write7seg("25151  15 ", 0);
    delay(waitTime);
  
    write7seg("23151  15 ", 0);
    delay(waitTime);
  
    write7seg("23150  15 ", 0);
    delay(waitTime);
  
    // valid value is 23
    write7seg("2315023   ", 0);
    delay(waitTime);
  
  
    // 23 => 42
    write7seg("2325023   ", 0);
    delay(waitTime);
  
    write7seg("2323023   ", 0);
    delay(waitTime);
  
    write7seg("2343023   ", 0);
    delay(waitTime);
  
    write7seg("2342023   ", 0);
    delay(waitTime);
  
    write7seg("2342123   ", 0);
    delay(waitTime);
  
    // valid value is 42
    write7seg("23421  42 ", 0);
    delay(waitTime);
  
  
    // 42 => 08
    write7seg("43421  42 ", 0);
    delay(waitTime);
  
    write7seg("42421  42 ", 0);
    delay(waitTime);
  
    write7seg("02421  42 ", 0);
    delay(waitTime);
  
    write7seg("08421  42 ", 0);
    delay(waitTime);
  
    write7seg("08420  42 ", 0);
    delay(waitTime);
  
    // valid value is 08
    write7seg("0842008   ", 0);
    delay(waitTime);
  
  
    // 08 => 15
    write7seg("0802008   ", 0);
    delay(waitTime);
  
    write7seg("0808008   ", 0);
    delay(waitTime);
  
    write7seg("0818008   ", 0);
    delay(waitTime);
  
    write7seg("0815008   ", 0);
    delay(waitTime);
  
    write7seg("0815108   ", 0);
    delay(waitTime);
  
    // valid value is 15
    write7seg("08151  15 ", 0);
    delay(waitTime);

  }

  // end of new code
  
//  for (int i=0; i<256; i++) {
//    dimming(1, i);
//    delay(1000/256);
//  }
//  for (int i=0; i<256; i++) {
//    dimming(1, 255-i);
//    delay(1000/256);
//  }
}


// WARNING: hard coded values, not configurable, OE depends on hardware Timer1A
constexpr uint8_t PIN_SHIFT_SS = 10; // slave select
constexpr uint8_t PIN_SHIFT_OE = 9; // ~output enable for segments (first 8 bit)

void colorDimming(uint8_t brightRT, uint8_t brightGN, uint8_t brightBL) {
  dimming(0, brightRT);
  dimming(1, brightRT);
  dimming(2, brightGN);
  dimming(3, brightGN);
  dimming(4, brightBL);
  dimming(5, brightRT);
  dimming(6, brightRT);
  dimming(7, brightGN);
  dimming(8, brightGN);
  dimming(9, brightBL);
}

void init7seg() {
  colorDimming(0,0,0);
  for (int i=0; i<10; i++) segments[i] = 0;

  // init SPI and switch everything off
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2); 
  pinMode(PIN_SHIFT_OE, OUTPUT);
  pinMode(PIN_SHIFT_SS, OUTPUT);
  digitalWrite(PIN_SHIFT_SS, HIGH); // SS disable
  digitalWrite(PIN_SHIFT_OE, HIGH); // LEDs off

  // configure PWM dimming and multiplex timer on Timer1
  // PWM should be sufficiently faster than the digit multiplexing,
  // otherwise we see fancy mixing effects
  // pin 9 has 31.25 kHz bae frequency for PWM, 0x01: divider 1, 0x02: use divisor 8 for 3.9 kHz PWM, 0x02: divisor 64
  TCCR1A = _BV(COM1A1)  + _BV(WGM11); // enable pin 9
  //+ _BV(COM1A0)+ _BV(WGM10)
  TCCR1B = _BV(WGM12) + 0x1; // fast-pwm 9bit, clock divider clk/1
  TIMSK1 |= _BV(TOIE1); // enable overflow irq
}

ISR(TIMER1_OVF_vect) {
  static uint8_t current_digit = 0;

  // activate the new shift reg. values
  PORTB |= _BV(PB2); //digitalWrite(PIN_SHIFT_SS, HIGH); 

  // prepare for the next digit
  current_digit++;
  if (current_digit == 10) current_digit = 0;
  uint16_t dmask = ~(1<<current_digit);

  // OCR1A is buffered, becomes active on next overflow
  OCR1A = 511-brightness[current_digit]<<1; // analogWrite(PIN_SHIFT_OE, ...)

  PORTB &= ~_BV(PB2); // digitalWrite(PIN_SHIFT_SS, LOW); // let the shift reg
  SPI.transfer((dmask >> 8) & 0xFF); // digit selector
  SPI.transfer(dmask & 0xFF); // digit selector
  SPI.transfer(segments[current_digit]); // current digit's segments
//  SPDR = (dmask >> 8) & 0xFF;
//  while (!(SPSR & _BV(SPIF)));
//  SPDR = dmask & 0xFF;
//  while (!(SPSR & _BV(SPIF)));
//  SPDR = segments[current_digit];
//  while (!(SPSR & _BV(SPIF))); // no need to wait, because long time until next IRQ
}

/** segment wiring
 *    --A6--
 *   |      |
 *  F7      B5
 *   |      |
 *    --G8--  
 *   |      |
 *  E4      C2
 *   |      |
 *    --D3--   DP1
 */

//  segments[0] = 0x01;
//  segments[1] = 0x02;
//  segments[2] = 0x04;
//  segments[3] = 0x08;
//  segments[4] = 0x10;
//  segments[5] = 0x20;
//  segments[6] = 0x40;
//  segments[7] = 0x80;
//  segments[8] = 0x00;
//  segments[9] = 0x00;

const uint8_t digitmap[] = {
// 87654321  
  B01111110, // 0
  B00010010, // 1
  B10111100, // 2
  B10110110, // 3
  B11010010, // 4
  B11100110, // 5
  B11101110, // 6
  B00110010, // 7
  B11111110, // 8
  B11110110, // 9
  B11111010, // A
  B11001110, // B
  B10001100, // C
  B10011110, // D
  B11101100, // E
  B11101000, // F
};

uint8_t transcode(char c) {
  if (c >= '0' && c <= '9') return digitmap[c-'0'];
  if (c >= 'A' && c <= 'F') return digitmap[c-'A'+10];
  if (c == 'H') return B11011010;
  if (c == 'L') return B01001100;
  if (c == 'O') return B01111110;
  if (c == 'J') return B00010110;
  if (c == 'N') return B01111010;
  if (c == ' ') return B00000000;
  return B00000001;
}

void write7seg(char const* str, int start) {
  uint8_t pos=0;
  if (start<0) {
    while (str[pos] != 0 && start<0) { pos++; start++; }
    start=0;
  }
  for (int i=0; i<start && i<10; i++) segments[i] = 0;
  while (str[pos] != 0 && start<10) {
    segments[start] = transcode(str[pos]);
    pos++;
    start++;
  }
  for (; start<10; start++) segments[start] = 0;
}

void dot7seg(uint8_t pos, bool on) {
  if (on) segments[pos] |= B00000001;
  else segments[pos] &= ~B00000001;
}
