// #include <Arduino.h>

// #define PIN_PXL0       4   // IC2 pin 18 = IO4
// #define PIN_PXL1      10   // IC2 pin 16 = IO10
// #define PIN_PXL2      18   // IC2 pin 26 = IO18
// #define PIN_PXL3       7   // IC2 pin 21 = IO7
// #define PIN_PXL4       6   // IC2 pin 20 = IO6
// #define PIN_PXL5       5   // IC2 pin 19 = IO5

// #define PIN_MODE       3   // IC2 pin  6 = IO3
// #define PIN_INTENSITY  1   // IC2 pin 13 = IO1
// #define PIN_SPEED      0   // IC2 pin 12 = IO0

// #define Enable 8

// // ── PWM config ─────────────────────────────────────────────────────────────
// #define PWM_FREQ  1000
// #define PWM_RES      8

// const uint8_t PIXELS[6] = { PIN_PXL0, PIN_PXL1, PIN_PXL2,
//                              PIN_PXL3, PIN_PXL4, PIN_PXL5 };

// // ── State ──────────────────────────────────────────────────────────────────
// enum Mode { MODE_ALL_ON, MODE_CHASE, MODE_BREATHE, MODE_ALTERNATE, MODE_COUNT };
// const char* modeNames[] = { "All On", "Chase", "Breathe", "Alternate" };

// uint8_t  currentMode = MODE_ALL_ON;
// uint8_t  intensity   = 200;
// uint16_t speedDelay  = 80;

// uint32_t lastModePress      = 0;
// uint32_t lastIntensityPress = 0;
// uint32_t lastSpeedPress     = 0;
// const uint16_t DEBOUNCE_MS  = 200;

// // ── Helpers ────────────────────────────────────────────────────────────────
// void setPixel(uint8_t idx, uint8_t val) {
//   ledcWrite(idx, val);
// }

// void allOff() {
//   for (int i = 0; i < 6; i++) setPixel(i, 0);
// }

// void allOn(uint8_t level) {
//   for (int i = 0; i < 6; i++) setPixel(i, level);
// }

// // ── Mode runners ───────────────────────────────────────────────────────────
// void runAllOn() {
//   allOn(intensity);
// }

// uint8_t  chaseIdx   = 0;
// uint32_t chaseTimer = 0;

// void runChase() {
//   if (millis() - chaseTimer < speedDelay) return;
//   chaseTimer = millis();
//   allOff();
//   setPixel(chaseIdx, intensity);
//   chaseIdx = (chaseIdx + 1) % 6;
// }

// float    breatheAngle = 0;
// uint32_t breatheTimer = 0;

// void runBreathe() {
//   if (millis() - breatheTimer < 20) return;
//   breatheTimer = millis();
//   breatheAngle += 0.02f * (100.0f / speedDelay);
//   if (breatheAngle > 2 * PI) breatheAngle -= 2 * PI;
//   uint8_t level = (uint8_t)((sin(breatheAngle) * 0.5f + 0.5f) * intensity);
//   allOn(level);
// }

// bool     altState = false;
// uint32_t altTimer = 0;

// void runAlternate() {
//   if (millis() - altTimer < (uint32_t)speedDelay * 3) return;
//   altTimer = millis();
//   altState = !altState;
//   for (int i = 0; i < 6; i++)
//     setPixel(i, (i % 2 == (int)altState) ? intensity : 0);
// }

// // ── Buttons ────────────────────────────────────────────────────────────────
// void handleButtons() {
//   uint32_t now = millis();

//   if (!digitalRead(PIN_MODE) && now - lastModePress > DEBOUNCE_MS) {
//     lastModePress = now;
//     currentMode = (currentMode + 1) % MODE_COUNT;
//     allOff();
//     Serial.printf("Mode: %s\n", modeNames[currentMode]);
//   }

//   if (!digitalRead(PIN_INTENSITY) && now - lastIntensityPress > DEBOUNCE_MS) {
//     lastIntensityPress = now;
//     intensity = (intensity < 210) ? intensity + 50 : 50;
//     Serial.printf("Intensity: %d\n", intensity);
//   }

//   if (!digitalRead(PIN_SPEED) && now - lastSpeedPress > DEBOUNCE_MS) {
//     lastSpeedPress = now;
//     speedDelay = (speedDelay > 20) ? speedDelay / 2 : 160;
//     Serial.printf("Speed delay: %dms\n", speedDelay);
//   }
// }

// // ── Setup ──────────────────────────────────────────────────────────────────
// void setup() {
//   pinMode(Enable,OUTPUT);
//   Serial.begin(115200);
//   delay(500);  // let serial settle before any output
//   Serial.println("\nBooting...");

//   for (int i = 0; i < 6; i++) {
//     Serial.printf("  Attaching PWM ch %d → GPIO %d\n", i, PIXELS[i]);
//     ledcSetup(i, PWM_FREQ, PWM_RES);
//     ledcAttachPin(PIXELS[i], i);
//     setPixel(i, 0);
//   }

//   pinMode(PIN_MODE,      INPUT_PULLUP);
//   pinMode(PIN_INTENSITY, INPUT_PULLUP);
//   pinMode(PIN_SPEED,     INPUT_PULLUP);

//   Serial.println("Lamp controller ready");
// }

// // ── Loop ───────────────────────────────────────────────────────────────────
// void loop() {
//   digitalWrite(Enable,HIGH);
//   handleButtons();

//   switch (currentMode) {
//     case MODE_ALL_ON:    runAllOn();     break;
//     case MODE_CHASE:     runChase();     break;
//     case MODE_BREATHE:   runBreathe();   break;
//     case MODE_ALTERNATE: runAlternate(); break;
//   }
// }

// ----------------------------------------------------------------------------
// How to use the FastLED library for simple control of WS2812B RGB LEDs
// Two lines are drawn, one Green and one Blue, each 3 LEDs in length
// Each line moves end to end along the strip at slightly different rates
// If the pot is moved high enough from the ground terminal, the green
// line can be moved manually by the pot wiper position along the strip.
//
// This example code is unlicensed and is released into the public domain
//
// Gadget Reboot
// ----------------------------------------------------------------------------




#include <Arduino.h>
#include <FastLED.h>

// ── Pin assignments ────────────────────────────────────────────────────────
#define DATA_PIN       4
#define PIN_ENABLE     8
#define PIN_MODE       3 // Hue
#define PIN_INTENSITY  1 // Saturation
#define PIN_SPEED      0 // Intensity

// ── FastLED config ─────────────────────────────────────────────────────────
#define NUM_LEDS    20
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

// ── State ──────────────────────────────────────────────────────────────────
enum Mode { MODE_SOLID, MODE_CHASE, MODE_BREATHE, MODE_RAINBOW, MODE_ALTERNATE, MODE_COUNT };
const char* modeNames[] = { "Solid", "Chase", "Breathe", "Rainbow", "Alternate" };

uint8_t  currentMode = MODE_SOLID;
uint8_t  intensity   = 200;
uint16_t speedDelay  = 80;

const CRGB COLOURS[] = {
  CRGB(255, 255, 255),  // White
  CRGB(255, 160,  80),  // Warm white
  CRGB::Red,
  CRGB::Green,
  CRGB::Blue,
  CRGB::Cyan,
  CRGB::Magenta,
  CRGB::Orange
};
uint8_t colourIdx = 0;

uint32_t lastModePress      = 0;
uint32_t lastIntensityPress = 0;
uint32_t lastSpeedPress     = 0;
const uint16_t DEBOUNCE_MS  = 200;

// ── Helpers ────────────────────────────────────────────────────────────────
CRGB scaledColour() {
  CRGB c = COLOURS[colourIdx];
  c.nscale8(intensity);
  return c;
}

void allOff() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

// ── Mode runners ───────────────────────────────────────────────────────────
void runSolid() {
  fill_solid(leds, NUM_LEDS, scaledColour());
  FastLED.show();
  delay(20);
}

uint8_t  chaseIdx   = 0;
uint32_t chaseTimer = 0;

void runChase() {
  if (millis() - chaseTimer < speedDelay) return;
  chaseTimer = millis();
  fadeToBlackBy(leds, NUM_LEDS, 80);
  leds[chaseIdx] = scaledColour();
  FastLED.show();
  chaseIdx = (chaseIdx + 1) % NUM_LEDS;
}

float    breatheAngle = 0;
uint32_t breatheTimer = 0;

void runBreathe() {
  if (millis() - breatheTimer < 20) return;
  breatheTimer = millis();
  breatheAngle += 0.02f * (100.0f / speedDelay);
  if (breatheAngle > 2 * PI) breatheAngle -= 2 * PI;
  uint8_t level = (uint8_t)((sinf(breatheAngle) * 0.5f + 0.5f) * intensity);
  CRGB c = COLOURS[colourIdx];
  c.nscale8(level);
  fill_solid(leds, NUM_LEDS, c);
  FastLED.show();
}

uint8_t  rainbowHue   = 0;
uint32_t rainbowTimer = 0;

void runRainbow() {
  if (millis() - rainbowTimer < speedDelay) return;
  rainbowTimer = millis();
  fill_rainbow(leds, NUM_LEDS, rainbowHue, 255 / NUM_LEDS);
  FastLED.setBrightness(intensity);
  FastLED.show();
  rainbowHue++;
}

bool     altState = false;
uint32_t altTimer = 0;

void runAlternate() {
  if (millis() - altTimer < (uint32_t)speedDelay * 3) return;
  altTimer = millis();
  altState = !altState;
  for (int i = 0; i < NUM_LEDS; i++)
    leds[i] = (i % 2 == (int)altState) ? scaledColour() : CRGB::Black;
  FastLED.show();
}

// ── Buttons ────────────────────────────────────────────────────────────────
void handleButtons() {
  uint32_t now = millis();

  if (!digitalRead(PIN_MODE) && now - lastModePress > DEBOUNCE_MS) {
    lastModePress = now;
    currentMode = (currentMode + 1) % MODE_COUNT;
    allOff();
    Serial.printf("Mode: %s\n", modeNames[currentMode]);
  }

  if (!digitalRead(PIN_INTENSITY) && now - lastIntensityPress > DEBOUNCE_MS) {
    lastIntensityPress = now;
    if (currentMode == MODE_RAINBOW) {
      intensity = (intensity < 210) ? intensity + 50 : 50;
      FastLED.setBrightness(intensity);
      Serial.printf("Intensity: %d\n", intensity);
    } else {
      colourIdx = (colourIdx + 1) % (sizeof(COLOURS) / sizeof(COLOURS[0]));
      Serial.printf("Colour idx: %d\n", colourIdx);
    }
  }

  if (!digitalRead(PIN_SPEED) && now - lastSpeedPress > DEBOUNCE_MS) {
    lastSpeedPress = now;
    speedDelay = (speedDelay > 20) ? speedDelay / 2 : 160;
    Serial.printf("Speed delay: %dms\n", speedDelay);
  }
}

// ── Setup ──────────────────────────────────────────────────────────────────
void setup() {
  // Enable pin HIGH — powers the WS2812B strip via the LDO/switch on the board
  pinMode(PIN_ENABLE, OUTPUT);
  digitalWrite(PIN_ENABLE, HIGH);

  Serial.begin(115200);
  delay(500);
  Serial.println("\nBooting lamp controller (WS2812B)...");

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS)
         .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(intensity);
  allOff();

  pinMode(PIN_MODE,      INPUT_PULLUP);
  pinMode(PIN_INTENSITY, INPUT_PULLUP);
  pinMode(PIN_SPEED,     INPUT_PULLUP);

  Serial.printf("Ready — %d LEDs on GPIO %d, Enable on GPIO %d\n",
                NUM_LEDS, DATA_PIN, PIN_ENABLE);
}

// ── Loop ───────────────────────────────────────────────────────────────────
void loop() {
  handleButtons();

  switch (currentMode) {
    case MODE_SOLID:     runSolid();     break;
    case MODE_CHASE:     runChase();     break;
    case MODE_BREATHE:   runBreathe();   break;
    case MODE_RAINBOW:   runRainbow();   break;
    case MODE_ALTERNATE: runAlternate(); break;
  }
}