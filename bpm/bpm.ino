#include <FastLED.h>

#define LED_PIN     6
#define NUM_LEDS    150
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

const int pulsePin = 11;                // Pulse Sensor purple wire connected to analog pin X

CRGB leds[NUM_LEDS];

const CRGB normal(32, 255, 32);
const CRGB high(255, 0, 0);
const int highThreshold = 100;
const int brightnessHi = 255;
const int brightnessLo = 16;
const int pulseLength = 250;
const int pulseFactor = 4;

volatile int BPM;                   // used to hold the pulse rate
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // holds the time between beats, the Inter-Beat Interval
volatile boolean Pulse = false;     // true when pulse wave is high, false when it's low
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.

long next;

void setup()
{
  Serial.begin(250000);
  
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  
  BPM = 50;

// Uncomment to connect to pulse sensor.
// Also include interrupt.ino from flora-pulse-neopixels
//  delay(1000);                      // Wait a second
//  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS

  next = millis() + (pulseFactor*60000)/BPM;
}

void loop()
{
  fill_solid(leds, NUM_LEDS, (BPM > highThreshold) ? high : normal);

  long timeUntilPulse = next - millis();

  if (timeUntilPulse > pulseLength) // Pulse not started
  {
    FastLED.setBrightness(brightnessLo);
  }
  else if (timeUntilPulse > -pulseLength)  // Pulse on
  {
    FastLED.setBrightness(brightnessLo + ((brightnessHi - brightnessLo) * (pulseLength - abs(timeUntilPulse)))/pulseLength);
  }
  else  // Pulse complete
  {
    FastLED.setBrightness(brightnessLo);
    next = next + (pulseFactor*60000)/BPM;  // Next pulse is at previous pulse + period.
  }
  FastLED.show();

  if (Serial.available() > 0)
  {
    BPM = Serial.parseInt();
  }
}
