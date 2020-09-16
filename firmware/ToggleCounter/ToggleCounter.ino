/*
   8-Bit Counter

   Converts the input of 8 toggle switches arranged to represent
   binary code into a decimal format displayed in a 7-segment display.

   Abort startup sequence by actuating any toggle switch.

   MCU: Arduino Nano with Atmega168p

   After 3 minutes of no activity  begin audible chirp.

   Due to pin limitations, toggle0 uses an analog pin with
   a pull-up resitor to detect the state via ADC.

   Created: 09-11-2020
   Reuben Strangelove

   Device uses 2x AA batteries.
   Consumes 107ma all LEDs on, 28ma all LEDs off.
*/

#include <TM1637Display.h> // https://github.com/avishorp/TM1637

#define PIN_TM1637_CLK 11
#define PIN_TM1637_DIO 10

#define PIN_BUZZER 9

#define PIN_LED_0 13
#define PIN_LED_1 A0
#define PIN_LED_2 A1
#define PIN_LED_3 A2
#define PIN_LED_4 A3
#define PIN_LED_5 A4
#define PIN_LED_6 A5
#define PIN_LED_7 12

#define PIN_TOGGLE_0 A7
#define PIN_TOGGLE_1 8
#define PIN_TOGGLE_2 7
#define PIN_TOGGLE_3 6
#define PIN_TOGGLE_4 5
#define PIN_TOGGLE_5 4
#define PIN_TOGGLE_6 3
#define PIN_TOGGLE_7 2

TM1637Display display(PIN_TM1637_CLK, PIN_TM1637_DIO);

const unsigned long timeOutDelay = 180000;  // milliseconds
const unsigned long chirpDelay = 20000;      // milliseconds

byte number;
byte oldNumber;
unsigned long timeOutMillis;
unsigned long chirpMillis;

void setup()
{
  // Set LED pins as ouputs.
  pinMode(PIN_LED_0, OUTPUT);
  pinMode(PIN_LED_1, OUTPUT);
  pinMode(PIN_LED_2, OUTPUT);
  pinMode(PIN_LED_3, OUTPUT);
  pinMode(PIN_LED_4, OUTPUT);
  pinMode(PIN_LED_5, OUTPUT);
  pinMode(PIN_LED_6, OUTPUT);
  pinMode(PIN_LED_7, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  // Set toggle switch pins as inputs.
  pinMode(PIN_TOGGLE_0, INPUT);
  pinMode(PIN_TOGGLE_1, INPUT);
  pinMode(PIN_TOGGLE_2, INPUT);
  pinMode(PIN_TOGGLE_3, INPUT);
  pinMode(PIN_TOGGLE_4, INPUT);
  pinMode(PIN_TOGGLE_5, INPUT);
  pinMode(PIN_TOGGLE_6, INPUT);
  pinMode(PIN_TOGGLE_7, INPUT);

  // Enable internal pullups.
  digitalWrite(PIN_TOGGLE_1, HIGH);
  digitalWrite(PIN_TOGGLE_2, HIGH);
  digitalWrite(PIN_TOGGLE_3, HIGH);
  digitalWrite(PIN_TOGGLE_4, HIGH);
  digitalWrite(PIN_TOGGLE_5, HIGH);
  digitalWrite(PIN_TOGGLE_6, HIGH);
  digitalWrite(PIN_TOGGLE_7, HIGH);

  display.setBrightness(0xff);

  StartUpSequence();
}

void loop()
{
  // Chirp if no activity is detected on the toggles.
  if ((timeOutMillis + timeOutDelay) < millis())
  {
    if ((chirpMillis + chirpDelay) < millis())
    {
      digitalWrite(PIN_BUZZER, HIGH);
      delay(30);
      digitalWrite(PIN_BUZZER, LOW);
      chirpMillis = millis();
    }
  }

  // Turn on LEDs when toggles are actuated.
  digitalWrite(PIN_LED_0, analogRead(PIN_TOGGLE_0) > 100);
  digitalWrite(PIN_LED_1, digitalRead(PIN_TOGGLE_1));
  digitalWrite(PIN_LED_2, digitalRead(PIN_TOGGLE_2));
  digitalWrite(PIN_LED_3, digitalRead(PIN_TOGGLE_3));
  digitalWrite(PIN_LED_4, digitalRead(PIN_TOGGLE_4));
  digitalWrite(PIN_LED_5, digitalRead(PIN_TOGGLE_5));
  digitalWrite(PIN_LED_6, digitalRead(PIN_TOGGLE_6));
  digitalWrite(PIN_LED_7, digitalRead(PIN_TOGGLE_7));

  oldNumber = number;
  number = DecodeToggleSwitchesIntoNumber();

  // If there is activity on the toggle switches reset timeout.
  if (oldNumber != number)
  {
    timeOutMillis = millis();
  }

  display.showNumberDec(number, false);
}

byte DecodeToggleSwitchesIntoNumber()
{
  byte number;

  // Map toggle switches to bits of the byte to display.
  bitWrite(number, 0, analogRead(PIN_TOGGLE_0) > 100);
  bitWrite(number, 1, digitalRead(PIN_TOGGLE_1));
  bitWrite(number, 2, digitalRead(PIN_TOGGLE_2));
  bitWrite(number, 3, digitalRead(PIN_TOGGLE_3));
  bitWrite(number, 4, digitalRead(PIN_TOGGLE_4));
  bitWrite(number, 5, digitalRead(PIN_TOGGLE_5));
  bitWrite(number, 6, digitalRead(PIN_TOGGLE_6));
  bitWrite(number, 7, digitalRead(PIN_TOGGLE_7));

  return number;
}

// Automatically count up when powered on.
// Break upon user input.
void StartUpSequence()
{
  byte startupNumber = DecodeToggleSwitchesIntoNumber();

  for (int number = 0; number < 256; number++)
  {
    // Break startup sequence upon user input.
    if (startupNumber != DecodeToggleSwitchesIntoNumber())
    {
      return;
    }

    digitalWrite(PIN_LED_0, bitRead(number, 0));
    digitalWrite(PIN_LED_1, bitRead(number, 1));
    digitalWrite(PIN_LED_2, bitRead(number, 2));
    digitalWrite(PIN_LED_3, bitRead(number, 3));
    digitalWrite(PIN_LED_4, bitRead(number, 4));
    digitalWrite(PIN_LED_5, bitRead(number, 5));
    digitalWrite(PIN_LED_6, bitRead(number, 6));
    digitalWrite(PIN_LED_7, bitRead(number, 7));

    display.showNumberDec(number, false);

    delay(50);
  }

  // Pause at end of count sequence for effect.
  unsigned long curMillis = millis();
  while ((curMillis + 2000) > millis())
  {
    if (startupNumber != DecodeToggleSwitchesIntoNumber())
    {
      return;
    }
  }
}
