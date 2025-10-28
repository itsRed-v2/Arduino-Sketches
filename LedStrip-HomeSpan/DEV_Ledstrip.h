#include <FastLED.h>

#define DATA_PIN 12
#define LED_COUNT 150
#define UPDATE_PERIOD 20

CRGB colors[LED_COUNT];

void setupFastLED() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(colors, LED_COUNT);
}

// Converts a color from HSV to RGB.
// h is hue, as a number between 0 and 360.
// s is the saturation, as a number between 0 and 255.
// v is the value, as a number between 0 and 255.
CRGB hsvToRgb(uint16_t h, uint8_t s, uint8_t v)
{
    uint8_t f = (h % 60) * 255 / 60;
    uint8_t p = (255 - s) * (uint16_t)v / 255;
    uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t r = 0, g = 0, b = 0;
    switch((h / 60) % 6){
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
    return { r, g, b };
}

struct ColorLedstrip : Service::LightBulb {
  SpanCharacteristic *on;
  SpanCharacteristic *hue;
  SpanCharacteristic *saturation;
  SpanCharacteristic *brightness;
  bool(*isRainbowEnabled)();

  ColorLedstrip() : Service::LightBulb() {
    on = new Characteristic::On(0);
    hue = new Characteristic::Hue(29);
    saturation = new Characteristic::Saturation(91);
    brightness = new Characteristic::Brightness(100);
    new Characteristic::ConfiguredName("Color");
  }

  void sendToStrip(bool isOn, float h, float s, float v) {
    CRGB col;
    if (isOn) {
      col = hsvToRgb(
        static_cast<uint16_t>(h),
        static_cast<uint8_t>(s * 255 / 100),
        static_cast<uint8_t>(v * 255 / 100)
      );
    } else {
      col = CRGB{ 0, 0, 0 };
    }
    for (uint16_t i = 0; i < LED_COUNT; i++) {
      colors[i] = col;
    }

    FastLED.show();
  }

  void refreshLeds() {
    bool isOn = on->getVal<bool>();
    float h = hue->getVal<float>();
    float s = saturation->getVal<float>();
    float v = brightness->getVal<float>();

    sendToStrip(isOn, h, s, v);
  }

  bool update() {
    if (isRainbowEnabled()) {
      return true;
    }

    bool isOn = on->getVal<bool>();
    if (on->updated()) {
      isOn = on->getNewVal<bool>();
    }

    float h = hue->getVal<float>();
    float s = saturation->getVal<float>();
    float v = brightness->getVal<float>();
    if (hue->updated())
      h = hue->getNewVal<float>();
    if (saturation->updated())
      s = saturation->getNewVal<float>();
    if (brightness->updated())
      v = brightness->getNewVal<float>();

    sendToStrip(isOn, h, s, v);

    return true;
  }

};

struct RainbowLedstrip : Service::LightBulb {
  SpanCharacteristic* on;
  bool enabled = false;
  uint32_t lastUpdateTime = 0;
  void(*onDisable)();

  RainbowLedstrip() : Service::LightBulb() {
    on = new Characteristic::On(0);
    new Characteristic::ConfiguredName("Rainbow");
  }

  bool update() {
    enabled = on->getNewVal<bool>();

    if (!enabled) {
      onDisable();
    }

    return true;
  }

  void loop() {
    if (!enabled) return;

    uint32_t time = millis();
    if (time - lastUpdateTime < UPDATE_PERIOD || lastUpdateTime > time) return;

    lastUpdateTime = time;

    for(uint16_t i = 0; i < LED_COUNT; i++)
    {
      float value = (time >> 7) + (i << 0);
      value = value * 359 / 256;
      colors[i] = hsvToRgb(((uint32_t) value) % 360, 255, 255);
    }

    FastLED.show();
  }
};
