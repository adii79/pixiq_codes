#include <Arduino.h>
#include <FastLED.h>
#include <math.h>

// ── Pin assignments ─────────────────────────────────────
#define DATA_PIN   4
#define PIN_ENABLE 8
#define HUE_B 3 //HUE(0-360)
#define SATURATION_B 0 //HUE(0-360)
#define INTENSITY_B 1 //HUE(0-360)

// ── FastLED config ──────────────────────────────────────
#define NUM_LEDS    20
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
float hue_selected = 255;
float saturation_selected = 100;
float intensity_selected = 100;
CRGB leds[NUM_LEDS];
// 🔴 Change this color

CRGB HSI2RGB(float H, float S, float I) {
    float r, g, b;

    H = fmod(H, 360); // wrap hue
    H = H * PI / 180; // convert to radians

    if (H < 2 * PI / 3) {
        b = I * (1 - S);
        r = I * (1 + (S * cos(H) / cos(PI/3 - H)));
        g = 3 * I - (r + b);
    } 
    else if (H < 4 * PI / 3) {
        H = H - 2 * PI / 3;
        r = I * (1 - S);
        g = I * (1 + (S * cos(H) / cos(PI/3 - H)));
        b = 3 * I - (r + g);
    } 
    else {
        H = H - 4 * PI / 3;
        g = I * (1 - S);
        b = I * (1 + (S * cos(H) / cos(PI/3 - H)));
        r = 3 * I - (g + b);
    }

    // clamp & convert to 0–255
    return CRGB(
        constrain(r * 255, 0, 255),
        constrain(g * 255, 0, 255),
        constrain(b * 255, 0, 255)
    );
}


CRGB color = HSI2RGB(hue_selected,saturation_selected,intensity_selected);

void setup() { 
  // Enable LED power
  pinMode(HUE_B, INPUT_PULLDOWN);
  pinMode(SATURATION_B, INPUT_PULLDOWN);
  pinMode(INTENSITY_B, INPUT_PULLDOWN);
  pinMode(PIN_ENABLE, OUTPUT);
  digitalWrite(PIN_ENABLE, HIGH);

  FastLED.addLeds<WS2812B, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(255);
  // leds[0] = CRGB::Red;
  // leds[0] = CRGB::Green;
  Serial.begin(115200);
}

void loop() {
    for(auto x: leds){
        leds[x] = HSI2RGB(hue_selected,saturation_selected,intensity_selected);
    }
  FastLED.show();
  delay(1000);

  // leds[0] = CRGB::Blue;
  // FastLED.show();
  // delay(1000);

  // leds[0] = CRGB::Green;
  // FastLED.show();
  // delay(1000);

  // digitalRead(HUE_B);
  // if(digitalRead(HUE_B) == LOW && hue_selected < 331){
  //   hue_selected += 12;
  // }
  // else {
  //   hue_selected ;
  // }
  // leds[0] = CRGB::HTMLColorCode(hue_selected);
  // FastLED.show();
  // delay(1000);

  int hueBtn = digitalRead(HUE_B);
  Serial.println("HUE");
  Serial.println(hueBtn);
  delay(20);
  int satBtn = digitalRead(SATURATION_B);
    Serial.println("SATURATION_B");
    Serial.println(satBtn);
    delay(20);
    int intBtn = digitalRead(INTENSITY_B);
    Serial.println("INTENSITY_B");
    Serial.println(intBtn);
  delay(20);
  
}