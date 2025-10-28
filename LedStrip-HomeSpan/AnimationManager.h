#ifndef ANIMATION_MANAGER_H
#define ANIMATION_MANAGER_H

#include <FastLED.h>

#include "Animation.h"

#define DATA_PIN 12
#define LED_COUNT 150
#define UPDATE_PERIOD 20

CRGB colors[LED_COUNT];

struct AnimationManager {
  uint32_t lastUpdateTime = 0;
  Animation *currentAnimation;

  AnimationManager(Animation* initialAnimation) {
    currentAnimation = initialAnimation;
  }

  void setupFastLED() {
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(colors, LED_COUNT);
  }

  void update() {
    uint32_t time = millis();
    if (time - lastUpdateTime < UPDATE_PERIOD || lastUpdateTime > time) return;
    lastUpdateTime = time;

    currentAnimation->render(time, colors, LED_COUNT);
    FastLED.show();
  }

  void setAnimation(Animation *animation) {
    currentAnimation = animation;
  }

};

#endif
