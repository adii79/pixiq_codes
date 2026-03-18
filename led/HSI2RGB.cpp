// #include <iostream>
// #include <cmath>
// #include <vector>

// using vectorRGB = std::vector<float>;

// float deg2rad(float deg) {
//     return deg * M_PI / 180.0;
// }

// vectorRGB HSI2RGB(float H, float S, float I) {
//     float R, G, B;

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

//     return {R, G, B};
// }

// int main() {
//     vectorRGB rgb = HSI2RGB(20, 0.8, 0.8);

//     std::cout << "R: " << rgb[0] << "\n";
//     std::cout << "G: " << rgb[1] << "\n";
//     std::cout << "B: " << rgb[2] << "\n";
// }



#include <iostream>
#include <cmath>
#include <vector>

using vectorRGB = std::vector<float>;

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

int main() {
    vectorRGB rgb = HSI2RGB(184,84.31/100,100);

    std::cout << "R: " << rgb[0] << "\n";
    std::cout << "G: " << rgb[1] << "\n";
    std::cout << "B: " << rgb[2] << "\n";
}