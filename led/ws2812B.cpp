// final code for the rgb adn HSI

#include <Adafruit_NeoPixel.h>
#include<vector>

#define NUM_LEDS 20
#define DATA_PIN 4
#define EN 8
#define HUE_B 3 // HUE , Red
#define SAT_B 0 // SAT , Green
#define INTEN_B 1 // INTEN , Blue

bool timingStarted = false;
unsigned long pressStartTime = 0;

bool modeHSI = true; 

int deBounceDelay = 100;
int satStep = 0;   // 0 → 10 (11 values including 0)
using vectorRGB = std::vector<float>;

float deg2rad(float deg);
float map255(float x);
float clamp255(float x);
vectorRGB HSI2RGB(float H, float S, float I);

vectorRGB rrggbb;

Adafruit_NeoPixel strip(NUM_LEDS, DATA_PIN, NEO_GRBW);



float Hue = 0.0;
float Sat = 0.0;
float Inten = 0.0;
int Red = 0;
int Green = 0;
int Blue = 0;
auto colors =  strip.Color(255, 0, 0);

void rgbValue() {
  rrggbb = HSI2RGB(Hue,Sat,Inten*2.25);
  float r = rrggbb[0];
  float g = rrggbb[1];
  float b = rrggbb[2];
  colors =  strip.Color(r, g, b);
}

void HSI(bool hue, bool saturation, bool intensity);
void RGB(bool hue, bool saturation, bool intensity);

void setup() {
  pinMode(EN, OUTPUT);
  digitalWrite(EN, HIGH);  
  pinMode(HUE_B, INPUT);
  pinMode(SAT_B, INPUT);
  pinMode(INTEN_B, INPUT);
  Serial.begin(115200);
  strip.begin();
  strip.show();  // Initialize all pixels to 'off'
}

// bool modeHSI = true;   // true = HSI mode, false = RGB mode

void loop() {
  bool HR = digitalRead(HUE_B);
  bool SG = digitalRead(SAT_B);
  bool IB = digitalRead(INTEN_B);

  // Detect long press (HUE + INTEN)
  if (!HR && !IB) {

    if (!timingStarted) {
      pressStartTime = millis();
      timingStarted = true;
    }

    if (millis() - pressStartTime >= 3000) {
      modeHSI = !modeHSI;   // 🔁 TOGGLE MODE
      timingStarted = false;

      delay(300); // small debounce so it doesn't toggle multiple times
    }

  } else {
    timingStarted = false;
  }

  // 🔁 Stay in selected mode
  if (modeHSI) {
    HSI(HR, SG, IB);
    Serial.print("HSI");
  } else {
    RGB(HR, SG, IB);
    Serial.print("RGB");
  }
}

// void loop() {
//   bool HR = digitalRead(HUE_B);
//   bool SG = digitalRead(SAT_B);
//   bool IB = digitalRead(INTEN_B);
//   HSI(HR,SG,IB);
//   if (!HR && !IB) {

//     if (!timingStarted) {
//       pressStartTime = millis();  // start timer
//       timingStarted = true;
//     }

//     // Check if 3 seconds passed
//     if (millis() - pressStartTime >= 3000) {
//       RGB(HR,SG,IB);
//       timingStarted = false;  // prevent repeated printing
//     }

//   } else {
//     // Reset if any button released
//     timingStarted = false;
//   }
// }


float deg2rad(float deg) {
    return deg * M_PI / 180.0;
}

// map [0–1] → [0–255]
float map255(float x) {
    return x * 255.0;
}

// clamp to [0–255]
float clamp255(float x) {
    if (x < 0) return 0;
    if (x > 255) return 255;
    return x;
}

vectorRGB HSI2RGB(float H, float S, float I) {
    float R, G, B;

    // 🔥 ADD THIS (your main issue)
    I = I / 255.0;

    if (H >= 0 && H < 120) {
        float Hr = deg2rad(H);

        B = I * (1 - S);
        R = I * (1 + (S * cos(Hr)) / cos(deg2rad(60) - Hr));
        G = 3 * I - (R + B);
    }
    else if (H >= 120 && H < 240) {
        H = H - 120;
        float Hr = deg2rad(H);

        R = I * (1 - S);
        G = I * (1 + (S * cos(Hr)) / cos(deg2rad(60) - Hr));
        B = 3 * I - (R + G);
    }
    else {
        H = H - 240;
        float Hr = deg2rad(H);

        G = I * (1 - S);
        B = I * (1 + (S * cos(Hr)) / cos(deg2rad(60) - Hr));
        R = 3 * I - (G + B);
    }

    // 🔥 ADD THIS (scale + clamp)
    return {
        clamp255(map255(R)),
        clamp255(map255(G)),
        clamp255(map255(B))
    };
}



void HSI(bool hue, bool saturation, bool intensity){
 bool hue_read = hue;
  if(!hue_read){ 
    if(Hue<361){
      Hue += 1;
    }
    else{
      Hue = 0;
    }
  }
  delay(deBounceDelay);
 bool sat_read = saturation;
if (!sat_read) {
    if(Sat<1.1){
      Sat += 0.12;
    }
    else{
      Sat = 0;
    }
}
   delay(deBounceDelay);  // increase delay for proper button debounce
  bool inten_read = intensity;
  if(!inten_read){
    if(Inten<101){
      Inten += 10;
    }
    else{
      Inten = 0;
    }
  }
  delay(deBounceDelay);
  Serial.printf(" h:%i s:%i i:%i \n",hue_read,sat_read,inten_read);
  rgbValue();
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, colors);  
    strip.setBrightness(Inten*2.25);  
  }
  strip.show();
}


void RGB(bool hue, bool saturation, bool intensity){
  Serial.print("started;");
 bool hue_read = hue;
  if(!hue_read){ 
    if(Red<255){
      Red += 10;
    }
    else{
      Red = 0;
    }
  }
  delay(deBounceDelay);
 bool sat_read = saturation;
if (!sat_read) {
    if(Green<255){
      Green += 10;
    }
    else{
      Green = 0;
    }
}
   delay(deBounceDelay);  // increase delay for proper button debounce
  bool inten_read = intensity;
  if(!inten_read){
    if(Blue<255){
      Blue += 10;
    }
    else{
      Blue = 0;
    }
  }
  delay(deBounceDelay);
  
  Serial.printf(" h:%i s:%i i:%i \n",Red,Green,Blue);
  rgbValue();
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(Red,Green,Blue));  
    strip.setBrightness(200);  
  }
  strip.show();
}






