#include "HomeSpan.h"

#include "DEV_Ledstrip.h"
#include "AnimationManager.h"

RainbowLedstrip* rainbowService;
ColorLedstrip* colorService;

Animations::BeatingRed defaultAnimation = Animations::BeatingRed();
AnimationManager animationManager { defaultAnimation };

bool isRainbowEnabled() {
  return rainbowService->on->getVal<bool>();
}

void onRainbowDisable() {
  colorService->refreshLeds();
}

void setup() {
  // Serial.begin(115200);

  homeSpan.setStatusPin(2);
  homeSpan.setControlPin(5, PushButton::TRIGGER_ON_LOW);
  homeSpan.begin(Category::Lighting, "WS2812B Ledstrip");

  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify();

    colorService = new ColorLedstrip(&animationManager);
    rainbowService = new RainbowLedstrip(&animationManager);

  colorService->isRainbowEnabled = isRainbowEnabled;
  rainbowService->onDisable = onRainbowDisable;

  animationManager.setupFastLED();
}

void loop() {
  homeSpan.poll();
  animationManager.update();
}
