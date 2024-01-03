#include <Arduino.h>
#include <FastLED.h>
#include <ArduinoJson.h>

#define NUM_LEDS 153
#define LEDS_PER_ALCOVE 51
#define DATA_PIN 4
#define BUILTIN_LED 2

CRGB leds[NUM_LEDS];

void setup() {

  // Initialize built-in LED pin as an output
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
  delay(200);
  digitalWrite(BUILTIN_LED, LOW);

  // Initialize serial communication
  Serial.begin(9600);
    Serial.println("{\"status\": \"ready\"}");


  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);

  // Initialize LED strip with all led off
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB{0, 0, 0};
  }
  FastLED.show();

}


CRGB hexToCRGB(const String& hexColor) {
    // Check if the hexColor string starts with #
    if (hexColor[0] == '#') {
        // Remove the # symbol
        String hexValue = hexColor.substring(1);

        // Parse the hexadecimal string
        unsigned long hex = strtoul(hexValue.c_str(), NULL, 16);

        // Extract RGB components
        uint8_t r = (hex >> 16) & 0xFF;
        uint8_t g = (hex >> 8) & 0xFF;
        uint8_t b = hex & 0xFF;

        // Create and return a CRGB object
        return CRGB{r, g, b};
    } else {
        // Handle invalid input (no # symbol)
        Serial.println("Invalid hex color format. Must start with #.");
        return CRGB{0, 0, 0}; // Default to black
    }
}

void fillRandomColor(CRGB* leds, int numLeds) {
  CRGB randomColor = CRGB(random(0, 255), random(0, 255), random(0, 255));
  for (int i = 0; i < numLeds; i++) {
    leds[i] = randomColor;
  }
}

void fillGradient(CRGB* leds, int numLeds, CRGB color1, CRGB color2) {
  for (int i = 0; i < numLeds; i++) {
    leds[i] = color1.lerp8(color2, i * 255 / numLeds);
  }
}


// User can provide a json with 3 colors in the serial monitor
// ex : {"alcove1": {"colors": ["#FFA500", "#FF0000"]}, "alcove2": {"colors": ["#E10000"]}, "alcove3": {"colors": ["#FF00FF", "#0000FF"]}}
void checkForSerialInputsJson() {
  // Check if there is any serial input
  if (Serial.available() > 0) {
    // Read the input
    String input = Serial.readStringUntil('\n');
  
    // Parse the input as JSON
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, input);

    // Extract the colors for each alcove

    for (int i = 0; i < 3; i++) {

      // Get the alcove
      JsonObject alcove = doc["alcove" + String(i+1)];
      JsonArray colors = alcove["colors"];

      // If there is only one color, fill the alcove with this color
      if (colors.size() == 1) {
        CRGB segment[LEDS_PER_ALCOVE];

        fillGradient(segment, LEDS_PER_ALCOVE, hexToCRGB(colors[0]), hexToCRGB(colors[0]));
        for (int j = 0; j < LEDS_PER_ALCOVE; j++) {
          leds[j + i * LEDS_PER_ALCOVE] = segment[j];
        }
      } else if (colors.size() > 1) {
        // If there are two colors, fill the alcove with a gradient of these colors
        CRGB segment[NUM_LEDS / 3];


        fillGradient(segment, LEDS_PER_ALCOVE, hexToCRGB(colors[0]), hexToCRGB(colors[1]));
        for (int j = 0; j < LEDS_PER_ALCOVE; j++) {
          leds[j + i * LEDS_PER_ALCOVE] = segment[j];
        }
      }

    }
    Serial.println("{\"status\": \"ok\"}");

    // On affiche les leds
    FastLED.show();


  }
}




void loop() {
  /*
  // Create 3 segments of 3 LEDs
  CRGB segment1[NUM_LEDS / 3];
  CRGB segment2[NUM_LEDS / 3];
  CRGB segment3[NUM_LEDS / 3];

  // Assign random color to each segment
  fillRandomColor(segment1, NUM_LEDS / 3);
  fillRandomColor(segment2, NUM_LEDS / 3);
  fillRandomColor(segment3, NUM_LEDS / 3);

  // Copy segments to the main LED strip
  for (int i = 0; i < NUM_LEDS / 3; i++) {
    leds[i] = segment1[i];
    leds[i + NUM_LEDS / 3] = segment2[i];
    leds[i + 2 * NUM_LEDS / 3] = segment3[i];
  }
  */

  /*
  // Rempli le ruban de led avec un dégradé de couleurs aléatoires
  CRGB startColor = CRGB(random(0, 255), random(0, 255), random(0, 255));
  CRGB endColor = CRGB(random(0, 255), random(0, 255), random(0, 255));

  CRGB segment[NUM_LEDS];
  fillGradient(segment, NUM_LEDS, startColor, endColor);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = segment[i];
  }
  */

  // Wait for serial input
  checkForSerialInputsJson();

}

