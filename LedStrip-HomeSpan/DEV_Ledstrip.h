#include <FastLED.h>
#include "AnimationManager.h"

struct ColorLedstrip : Service::LightBulb {
  SpanCharacteristic *on;
  SpanCharacteristic *hue;
  SpanCharacteristic *saturation;
  SpanCharacteristic *brightness;
  AnimationManager *animationManager;
  Animations::StaticColor staticColor { CRGB{0, 0, 0} };
  bool(*isRainbowEnabled)();

  ColorLedstrip(AnimationManager *animation_manager) : Service::LightBulb() {
    on = new Characteristic::On(0);
    hue = new Characteristic::Hue(29);
    saturation = new Characteristic::Saturation(91);
    brightness = new Characteristic::Brightness(100);
    new Characteristic::ConfiguredName("Color");
    animationManager = animation_manager;
  }

  void sendToStrip(bool isOn, float h, float s, float v) {
    if (isOn) {
      staticColor.color = hsvToRgb(
        static_cast<uint16_t>(h),
        static_cast<uint8_t>(s * 255 / 100),
        static_cast<uint8_t>(v * 255 / 100)
      );
    } else {
      staticColor.color = CRGB{ 0, 0, 0 };
    }

    animationManager->fadeToAnimation(staticColor);
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
  AnimationManager *animationManager;
  Animations::Rainbow rainbow {};

  RainbowLedstrip(AnimationManager *animation_manager) : Service::LightBulb() {
    on = new Characteristic::On(0);
    new Characteristic::ConfiguredName("Rainbow");
    animationManager = animation_manager;
  }

  bool update() {
    enabled = on->getNewVal<bool>();

    if (enabled) {
      animationManager->fadeToAnimation(rainbow);
    } else {
      onDisable();
    }

    return true;
  }

};
