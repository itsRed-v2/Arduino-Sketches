#ifndef ANIMATION_MANAGER_H
#define ANIMATION_MANAGER_H

#include <FastLED.h>

#include "Animation.h"

#define DATA_PIN 12
#define LED_COUNT 150
#define UPDATE_PERIOD 20

struct AnimationManager {
  uint32_t lastUpdateTime = 0;
  uint32_t transitionDuration = 0;
  uint32_t transitionStart = 0;
  Animation *currentAnimation;
  Animation *newAnimation = nullptr;

  CRGB mainBuffer[LED_COUNT];
  CRGB secondaryBuffer[LED_COUNT];

  AnimationManager(Animation* initialAnimation) {
    currentAnimation = initialAnimation;
  }

  void setupFastLED() {
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(mainBuffer, LED_COUNT);
  }

  void update() {
    uint32_t time = millis();
    if (time - lastUpdateTime < UPDATE_PERIOD || lastUpdateTime > time) return;
    lastUpdateTime = time;

    currentAnimation->render(time, mainBuffer, LED_COUNT);

    if (newAnimation != nullptr) {
      uint32_t transitionElapsed = time - transitionStart;

      if (transitionElapsed > transitionDuration) {
        newAnimation->render(time, mainBuffer, LED_COUNT);
        currentAnimation = newAnimation;
        newAnimation = nullptr;
        transitionStart = 0;
        transitionDuration = 0;
      } else {
        newAnimation->render(time, secondaryBuffer, LED_COUNT);

        float transitionProgress = 
          ((float)transitionElapsed) / ((float)transitionDuration);
        
        for (int i = 0; i < LED_COUNT; i++) {
          for (int j = 0; j < 3; j++) {
            uint8_t color1 = mainBuffer[i][j];
            uint8_t color2 = secondaryBuffer[i][j];
            uint8_t fadedColor = (uint8_t) (color1 * (1 - transitionProgress) + color2 * transitionProgress);
            mainBuffer[i][j] = fadedColor;
          }
        }
      }
    }

    FastLED.show();
  }

  void setAnimation(Animation *animation) {
    currentAnimation = animation;
  }

  void fadeToAnimation(Animation *animation) {
    newAnimation = animation;
    transitionDuration = 500; // 0.5s
    transitionStart = millis();
  }

};

#endif
