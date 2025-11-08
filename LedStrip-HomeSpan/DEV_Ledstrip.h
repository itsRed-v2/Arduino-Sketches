#include <FastLED.h>
#include <HomeSpan.h>

#include "StateMachine.h"
#include <FastLED.h>

struct ColorLedstrip : Service::LightBulb {
    SpanCharacteristic *on;
    SpanCharacteristic *hue;
    SpanCharacteristic *saturation;
    SpanCharacteristic *brightness;
    StateMachine &stateMachine;

    ColorLedstrip(StateMachine &stateMachine) : Service::LightBulb(), stateMachine(stateMachine) {
        on = new Characteristic::On(0);
        hue = new Characteristic::Hue(29);
        saturation = new Characteristic::Saturation(91);
        brightness = new Characteristic::Brightness(100);
        new Characteristic::ConfiguredName("Color");
    }

    bool update() {
        bool isOn = on->getNewVal<bool>();
        float h = hue->getNewVal<float>();
        float s = saturation->getNewVal<float>();
        float v = brightness->getNewVal<float>();

        if (isOn) {
            stateMachine.setStaticColor(hsvToRgb(
                static_cast<uint16_t>(h),
                static_cast<uint8_t>(s * 255 / 100),
                static_cast<uint8_t>(v * 255 / 100)
            ));
        } else {
            stateMachine.setStaticColor(CRGB{ 0, 0, 0 });
        }

        return true;
    }

};

struct RainbowLedstrip : Service::LightBulb {
    SpanCharacteristic* on;
    StateMachine &stateMachine;

    RainbowLedstrip(StateMachine &stateMachine) : Service::LightBulb(), stateMachine(stateMachine) {
        on = new Characteristic::On(0);
        new Characteristic::ConfiguredName("Rainbow");
    }

    bool update() {
        bool enabled = on->getNewVal<bool>();
        stateMachine.enableRainbow(enabled);
        return true;
    }

};
