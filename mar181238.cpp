// // // // #include <Arduino.h>
// // // // #include <FastLED.h>
// // // // #include <math.h>
// // // // #include <cmath>
// // // // #include <vector>

// // // // using vectorRGB = std::vector<float>;
// // // // // ── Pin assignments ─────────────────────────────────────
// // // // #define DATA_PIN   4
// // // // #define PIN_ENABLE 8
// // // // #define HUE_B 3 //HUE(0-360)
// // // // #define SATURATION_B 0 //HUE(0-360)
// // // // #define INTENSITY_B 1 //HUE(0-360)

// // // // // ── FastLED config ──────────────────────────────────────
// // // // #define NUM_LEDS    20
// // // // #define LED_TYPE    WS2812B
// // // // #define COLOR_ORDER GRB
// // // // float hue_selected = 0;
// // // // float saturation_selected = 000;
// // // // float intensity_selected = 000;
// // // // CRGB leds[NUM_LEDS];
// // // // // 🔴 Change this color

// // // // float deg2rad(float deg) {
// // // //     return deg * M_PI / 180.0;
// // // // }

// // // // // map [0–1] → [0–255]
// // // // float map255(float x) {
// // // //     return x * 255.0;
// // // // }

// // // // // clamp to [0–255]
// // // // float clamp255(float x) {
// // // //     if (x < 0) return 0;
// // // //     if (x > 255) return 255;
// // // //     return x;
// // // // }

// // // // vectorRGB HSI2RGB(float H, float S, float I) {
// // // //     float R, G, B;

// // // //     // 🔥 ADD THIS (your main issue)
// // // //     I = I / 255.0;

// // // //     if (H >= 0 && H < 120) {
// // // //         float Hr = deg2rad(H);

// // // //         B = I * (1 - S);
// // // //         R = I * (1 + (S * cos(Hr)) / cos(deg2rad(60) - Hr));
// // // //         G = 3 * I - (R + B);
// // // //     }
// // // //     else if (H >= 120 && H < 240) {
// // // //         H = H - 120;
// // // //         float Hr = deg2rad(H);

// // // //         R = I * (1 - S);
// // // //         G = I * (1 + (S * cos(Hr)) / cos(deg2rad(60) - Hr));
// // // //         B = 3 * I - (R + G);
// // // //     }
// // // //     else {
// // // //         H = H - 240;
// // // //         float Hr = deg2rad(H);

// // // //         G = I * (1 - S);
// // // //         B = I * (1 + (S * cos(Hr)) / cos(deg2rad(60) - Hr));
// // // //         R = 3 * I - (G + B);
// // // //     }

// // // //     // 🔥 ADD THIS (scale + clamp)
// // // //     return {
// // // //         clamp255(map255(R)),
// // // //         clamp255(map255(G)),
// // // //         clamp255(map255(B))
// // // //     };
// // // // }

// // // // CRGB color = HSI2RGB(hue_selected,saturation_selected,intensity_selected);

// // // // void setup() { 
// // // //   // Enable LED power
// // // //   pinMode(HUE_B, INPUT);
// // // //   pinMode(SATURATION_B, INPUT);
// // // //   pinMode(INTENSITY_B, INPUT);
// // // //   pinMode(PIN_ENABLE, OUTPUT);
// // // //   digitalWrite(PIN_ENABLE, HIGH);

// // // //   FastLED.addLeds<WS2812B, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
// // // //   FastLED.setBrightness(200);
// // // //   Serial.begin(115200);
// // // // }

// // // // void loop() {
// // // //   // HSI2RGB(hue_selected,saturation_selected,intensity_selected);
// // // //   // for(int i =0 ;i<20; i++){
// // // //   //       leds[i] = HSI2RGB(hue_selected,saturation_selected,intensity_selected);
// // // //   //   }
// // // //   // leds[]0 = HSI2RGB(hue_selected,saturation_selected,intensity_selected);
// // // //   leds[0] = CRGB(255, 0, 0);
// // // //   FastLED.show();
// // // //   // delay(1000);

// // // //   // leds[0] = CRGB::Blue;
// // // //   // FastLED.show();
// // // //   // delay(1000);

// // // //   // leds[0] = CRGB::Green;
// // // //   // FastLED.show();
// // // //   // delay(1000);

// // // //   // if(digitalRead(HUE_B) == LOW){
// // // //   //   hue_selected += 10;
// // // //   // }
// // // //   // if(hue_selected > 360) hue_selected = 0;
  
// // // //   // if(digitalRead(SATURATION_B) == LOW){
// // // //   //   saturation_selected += 10;
// // // //   // }
// // // //   // if(saturation_selected > 100) saturation_selected = 0;
  
// // // //   // if(digitalRead(INTENSITY_B) == LOW){
// // // //   //   intensity_selected += 10;
// // // //   // }
// // // //   // if(intensity_selected > 100) intensity_selected = 0;
  

// // // //   // Serial.println(hue_selected);
// // // //   // Serial.println(saturation_selected);
// // // //   // Serial.println(intensity_selected);

// // // //   // FastLED.setBrightness(intensity_selected * (255 / 100));


// // // //   // int hueBtn = digitalRead(HUE_B);
// // // //   // Serial.println("HUE");
// // // //   // Serial.println(hueBtn);
// // // //   // delay(20);
// // // //   // int satBtn = digitalRead(SATURATION_B);
// // // //   //   Serial.println("SATURATION_B");
// // // //   //   Serial.println(satBtn);
// // // //   //   delay(20);
// // // //   //   int intBtn = digitalRead(INTENSITY_B);
// // // //   //   Serial.println("INTENSITY_B");
// // // //   //   Serial.println(intBtn);
// // // //   delay(1000);
  
// // // // }


// // // #include <Arduino.h>
// // // #include <FastLED.h>
// // // #include <math.h>
// // // #include <cmath>
// // // #include <vector>

// // // // ── Pin assignments ─────────────────────────────────────
// // // #define DATA_PIN   4
// // // #define PIN_ENABLE 8
// // // #define HUE_B 3 //HUE(0-360)
// // // #define SATURATION_B 0 //HUE(0-360)
// // // #define INTENSITY_B 1 //HUE(0-360)


// // // #define NUM_LEDS    20
// // // #define LED_TYPE    WS2812B
// // // #define COLOR_ORDER GRB
// // // float hue_selected = 0;
// // // float saturation_selected = 000;
// // // float intensity_selected = 000;
// // // CRGB leds[NUM_LEDS];

// // // using vectorRGB = std::vector<float>;
// // // float deg2rad(float deg);
// // // float map255(float x);
// // // float clamp255(float x); 

// // // float hue = 0.0;
// // // float satura = 0.0;
// // // float inten = 0.0;
// // // vectorRGB HSI2RGB(float H, float S, float I);
// // // auto RGBv = HSI2RGB(hue,satura,inten);
// // // // CRGB color = CRGB(RGBv)
// // // int rr = (int) RGBv[0];
// // // int gg = (int) RGBv[1];
// // // int bb = (int) RGBv[2];








// // // void setup() { 
// // //   // Enable LED power
// // //   pinMode(HUE_B, INPUT);
// // //   pinMode(SATURATION_B, INPUT);
// // //   pinMode(INTENSITY_B, INPUT);
// // //   pinMode(PIN_ENABLE, OUTPUT);
// // //   digitalWrite(PIN_ENABLE, HIGH);

// // //   FastLED.addLeds<WS2812B, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
// // //   FastLED.setBrightness(bb+50);
// // //   Serial.begin(115200);
// // // }

// // // void loop() {

// // //   bool hue_r = digitalRead(HUE_B);
// // //   Serial.print(hue_r);
// // //   Serial.print("_");
// // //   if(!hue_r && hue < 360){
// // //     hue += 1;
// // //   }
// // //   if(hue>360) hue = 0;
// // //   delay(100);
// // //   bool sat_r = digitalRead(SATURATION_B);
// // //   Serial.print(sat_r);
// // //   Serial.print("_");
// // //   if(!sat_r && satura<1){
// // //     satura += 0.1;
// // //   }
// // //   if(satura>1.0) satura = 0.0;
// // //   delay(100);
// // //   bool inten_r = digitalRead(INTENSITY_B);
// // //   Serial.print(inten_r);
// // //   Serial.print("_");
// // //   if(!inten_r && inten<100){
// // //     inten += 10;
// // //   }
// // //   if(inten>100) inten = 0;
// // //   delay(100);
// // //    RGBv = HSI2RGB(hue,satura,inten);
// // //    rr = (int) RGBv[0];
// // //    gg = (int) RGBv[1];
// // //    bb = (int) RGBv[2];

// // //   // RGBv = HSI2RGB(hue,satura,inten);
// // //     FastLED.setBrightness(bb+50);


// // //   leds[0] = CRGB(rr,gg,bb);
// // //   leds[1] = CRGB(rr,gg,bb);
// // //   leds[2] = CRGB(rr,gg,bb);
// // //   FastLED.show();
// // //   Serial.print(".........");
// // //   for(float i: RGBv){

// // //     Serial.print(" ");
// // //     Serial.print(i);
// // //   }
  
// // //   // delay(1000);
  
// // // }






// // // float deg2rad(float deg) {
// // //     return deg * M_PI / 180.0;
// // // }

// // // // map [0–1] → [0–255]
// // // float map255(float x) {
// // //     return x * 255.0;
// // // }

// // // // clamp to [0–255]
// // // float clamp255(float x) {
// // //     if (x < 0) return 0;
// // //     if (x > 255) return 255;
// // //     return x;
// // // }

// // // vectorRGB HSI2RGB(float H, float S, float I) {
// // //     float R, G, B;

// // //     // 🔥 ADD THIS (your main issue)
// // //     I = I / 255.0;
// // //     // I = I / 100.0;

// // //     if (H >= 0 && H < 120) {
// // //         float Hr = deg2rad(H);

// // //         B = I * (1 - S);
// // //         R = I * (1 + (S * cos(Hr)) / cos(deg2rad(60) - Hr));
// // //         G = 3 * I - (R + B);
// // //     }
// // //     else if (H >= 120 && H < 240) {
// // //         H = H - 120;
// // //         float Hr = deg2rad(H);

// // //         R = I * (1 - S);
// // //         G = I * (1 + (S * cos(Hr)) / cos(deg2rad(60) - Hr));
// // //         B = 3 * I - (R + G);
// // //     }
// // //     else {
// // //         H = H - 240;
// // //         float Hr = deg2rad(H);

// // //         G = I * (1 - S);
// // //         B = I * (1 + (S * cos(Hr)) / cos(deg2rad(60) - Hr));
// // //         R = 3 * I - (G + B);
// // //     }

// // //     // 🔥 ADD THIS (scale + clamp)
// // //     return {
// // //         clamp255(map255(R)),
// // //         clamp255(map255(G)),
// // //         clamp255(map255(B))
// // //     };
// // // }




// // // // #include <Adafruit_NeoPixel.h>

// // // // #define NUM_LEDS 3
// // // // #define DATA_PIN 4
// // // // #define EN 8

// // // // Adafruit_NeoPixel strip(NUM_LEDS, DATA_PIN, NEO_GRBW);

// // // // void setup() {
// // // //   pinMode(EN, OUTPUT);
// // // //   digitalWrite(EN, HIGH);  

// // // //   strip.begin();
// // // //   strip.show();  // Initialize all pixels to 'off'
// // // // }

// // // // void loop() {
// // // //   for (int i = 0; i < NUM_LEDS; i++) {
// // // //     strip.setPixelColor(i, strip.Color(0, 255, 0));  // Green
// // // //   }
// // // //   strip.show();
// // // // }







// // #include <Adafruit_NeoPixel.h>
// // #include<vector>

// // #define NUM_LEDS 20
// // #define DATA_PIN 4
// // #define EN 8
// // #define HUE_B 3
// // #define SAT_B 0
// // #define INTEN_B 1
// // int deBounceDelay = 100;
// // int satStep = 0;   // 0 → 10 (11 values including 0)
// // using vectorRGB = std::vector<float>;

// // float deg2rad(float deg);
// // float map255(float x);
// // float clamp255(float x);
// // vectorRGB HSI2RGB(float H, float S, float I);

// // vectorRGB rrggbb;

// // Adafruit_NeoPixel strip(NUM_LEDS, DATA_PIN, NEO_GRBW);



// // float Hue = 0.0;
// // float Sat = 0.0;
// // float Inten = 0.0;
// // auto colors =  strip.Color(255, 0, 0);

// // void rgbValue() {
// //   rrggbb = HSI2RGB(Hue,Sat,Inten*2.25);
// //   float r = rrggbb[0];
// //   float g = rrggbb[1];
// //   float b = rrggbb[2];
// //   colors =  strip.Color(r, g, b);
// // }

// // void setup() {
// //   pinMode(EN, OUTPUT);
// //   digitalWrite(EN, HIGH);  
// //   pinMode(HUE_B, INPUT);
// //   pinMode(SAT_B, INPUT);
// //   pinMode(INTEN_B, INPUT);
// //   Serial.begin(115200);
// //   strip.begin();
// //   strip.show();  // Initialize all pixels to 'off'
// // }

// // void loop() {
// //   bool hue_read = digitalRead(HUE_B);
// //   if(!hue_read){
// //     if(Hue<361){
// //       Hue += 1;
// //     }
// //     else{
// //       Hue = 0;
// //     }
// //   }
// //   delay(deBounceDelay);
// //  bool sat_read = digitalRead(SAT_B);
// // if (!sat_read) {
// //     if(Sat<1.1){
// //       Sat += 0.11;
// //     }
// //     else{
// //       Sat = 0;
// //     }
// // }
// //    delay(deBounceDelay);  // increase delay for proper button debounce
// //   bool inten_read = digitalRead(INTEN_B);
// //   if(!inten_read){
// //     if(Inten<101){
// //       Inten += 1;
// //     }
// //     else{
// //       Inten = 0;
// //     }
// //   }
// //   delay(deBounceDelay);
// //   Serial.printf(" h:%i s:%i i:%i \n",hue_read,sat_read,inten_read);
// //   rgbValue();
// //   for (int i = 0; i < NUM_LEDS; i++) {
// //     strip.setPixelColor(i, colors);  // Green
// //     strip.setBrightness(Inten*2.25);  // Green
// //   }
// //   strip.show();
// // }


// // // float deg2rad(float deg);
// // // float map255(float x);
// // // float clamp255(float x);
// // // vectorRGB HSI2RGB(float H, float S, float I);

// // float deg2rad(float deg) {
// //     return deg * M_PI / 180.0;
// // }

// // // map [0–1] → [0–255]
// // float map255(float x) {
// //     return x * 255.0;
// // }

// // // clamp to [0–255]
// // float clamp255(float x) {
// //     if (x < 0) return 0;
// //     if (x > 255) return 255;
// //     return x;
// // }

// // vectorRGB HSI2RGB(float H, float S, float I) {
// //     float R, G, B;

// //     // 🔥 ADD THIS (your main issue)
// //     I = I / 255.0;

// //     if (H >= 0 && H < 120) {
// //         float Hr = deg2rad(H);

// //         B = I * (1 - S);
// //         R = I * (1 + (S * cos(Hr)) / cos(deg2rad(60) - Hr));
// //         G = 3 * I - (R + B);
// //     }
// //     else if (H >= 120 && H < 240) {
// //         H = H - 120;
// //         float Hr = deg2rad(H);

// //         R = I * (1 - S);
// //         G = I * (1 + (S * cos(Hr)) / cos(deg2rad(60) - Hr));
// //         B = 3 * I - (R + G);
// //     }
// //     else {
// //         H = H - 240;
// //         float Hr = deg2rad(H);

// //         G = I * (1 - S);
// //         B = I * (1 + (S * cos(Hr)) / cos(deg2rad(60) - Hr));
// //         R = 3 * I - (G + B);
// //     }

// //     // 🔥 ADD THIS (scale + clamp)
// //     return {
// //         clamp255(map255(R)),
// //         clamp255(map255(G)),
// //         clamp255(map255(B))
// //     };
// // }





// #include <Adafruit_NeoPixel.h>
// #include<vector>

// #define NUM_LEDS 20
// #define DATA_PIN 4
// #define EN 8
// #define HUE_B 3
// #define SAT_B 0
// #define INTEN_B 1
// int deBounceDelay = 100;
// int satStep = 0;   // 0 → 10 (11 values including 0)
// using vectorRGB = std::vector<float>;

// float deg2rad(float deg);
// float map255(float x);
// float clamp255(float x);
// vectorRGB HSI2RGB(float H, float S, float I);

// vectorRGB rrggbb;

// Adafruit_NeoPixel strip(NUM_LEDS, DATA_PIN, NEO_GRBW);



// float Hue = 0.0;
// float Sat = 0.0;
// float Inten = 0.0;
// auto colors =  strip.Color(255, 0, 0);

// void rgbValue() {
//   rrggbb = HSI2RGB(Hue,Sat,Inten*2.25);
//   float r = rrggbb[0];
//   float g = rrggbb[1];
//   float b = rrggbb[2];
//   colors =  strip.Color(r, g, b);
// }

// void HSI();

// void setup() {
//   pinMode(EN, OUTPUT);
//   digitalWrite(EN, HIGH);  
//   pinMode(HUE_B, INPUT);
//   pinMode(SAT_B, INPUT);
//   pinMode(INTEN_B, INPUT);
//   Serial.begin(115200);
//   strip.begin();
//   strip.show();  // Initialize all pixels to 'off'
// }

// void loop() {
//   bool hue_read = digitalRead(HUE_B);
//   if(!hue_read){
//     if(Hue<361){
//       Hue += 1;
//     }
//     else{
//       Hue = 0;
//     }
//   }
//   delay(deBounceDelay);
//  bool sat_read = digitalRead(SAT_B);
// if (!sat_read) {
//     if(Sat<1.1){
//       Sat += 0.12;
//     }
//     else{
//       Sat = 0;
//     }
// }
//    delay(deBounceDelay);  // increase delay for proper button debounce
//   bool inten_read = digitalRead(INTEN_B);
//   if(!inten_read){
//     if(Inten<101){
//       Inten += 10;
//     }
//     else{
//       Inten = 0;
//     }
//   }
//   delay(deBounceDelay);
//   Serial.printf(" h:%i s:%i i:%i \n",hue_read,sat_read,inten_read);
//   rgbValue();
//   for (int i = 0; i < NUM_LEDS; i++) {
//     strip.setPixelColor(i, colors);  // Green
//     strip.setBrightness(Inten*2.25);  // Green
//   }
//   strip.show();
// }


// // float deg2rad(float deg);
// // float map255(float x);
// // float clamp255(float x);
// // vectorRGB HSI2RGB(float H, float S, float I);

// float deg2rad(float deg) {
//     return deg * M_PI / 180.0;
// }

// // map [0–1] → [0–255]
// float map255(float x) {
//     return x * 255.0;
// }

// // clamp to [0–255]
// float clamp255(float x) {
//     if (x < 0) return 0;
//     if (x > 255) return 255;
//     return x;
// }

// vectorRGB HSI2RGB(float H, float S, float I) {
//     float R, G, B;

//     // 🔥 ADD THIS (your main issue)
//     I = I / 255.0;

//     if (H >= 0 && H < 120) {
//         float Hr = deg2rad(H);

//         B = I * (1 - S);
//         R = I * (1 + (S * cos(Hr)) / cos(deg2rad(60) - Hr));
//         G = 3 * I - (R + B);
//     }
//     else if (H >= 120 && H < 240) {
//         H = H - 120;
//         float Hr = deg2rad(H);

//         R = I * (1 - S);
//         G = I * (1 + (S * cos(Hr)) / cos(deg2rad(60) - Hr));
//         B = 3 * I - (R + G);
//     }
//     else {
//         H = H - 240;
//         float Hr = deg2rad(H);

//         G = I * (1 - S);
//         B = I * (1 + (S * cos(Hr)) / cos(deg2rad(60) - Hr));
//         R = 3 * I - (G + B);
//     }

//     // 🔥 ADD THIS (scale + clamp)
//     return {
//         clamp255(map255(R)),
//         clamp255(map255(G)),
//         clamp255(map255(B))
//     };
// }



// void HSI(){

// }









#include <Adafruit_NeoPixel.h>
#include<vector>

#define NUM_LEDS 20
#define DATA_PIN 4
#define EN 8
#define HUE_B 3 // HUE , Red
#define SAT_B 0 // SAT , Green
#define INTEN_B 1 // INTEN , Blue
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
auto colors =  strip.Color(255, 0, 0);

void rgbValue() {
  rrggbb = HSI2RGB(Hue,Sat,Inten*2.25);
  float r = rrggbb[0];
  float g = rrggbb[1];
  float b = rrggbb[2];
  colors =  strip.Color(r, g, b);
}

void HSI();

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

void loop() {
  if(digitalRead(HUE_B) && digitalRead(INTEN_B)){
    Serial.print("this is working ");
  }
  HSI();
}


// float deg2rad(float deg);
// float map255(float x);
// float clamp255(float x);
// vectorRGB HSI2RGB(float H, float S, float I);

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



void HSI(){
 bool hue_read = digitalRead(HUE_B);
  if(!hue_read){
    if(Hue<361){
      Hue += 1;
    }
    else{
      Hue = 0;
    }
  }
  delay(deBounceDelay);
 bool sat_read = digitalRead(SAT_B);
if (!sat_read) {
    if(Sat<1.1){
      Sat += 0.12;
    }
    else{
      Sat = 0;
    }
}
   delay(deBounceDelay);  // increase delay for proper button debounce
  bool inten_read = digitalRead(INTEN_B);
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
    strip.setPixelColor(i, colors);  // Green
    strip.setBrightness(Inten*2.25);  // Green
  }
  strip.show();
}






