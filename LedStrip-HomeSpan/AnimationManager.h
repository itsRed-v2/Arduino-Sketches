#ifndef ANIMATION_MANAGER_H
#define ANIMATION_MANAGER_H

#include <FastLED.h>

#include "Animation.h"

#define DATA_PIN 12
#define LED_COUNT 150
#define UPDATE_PERIOD 20 // 1 frame per 20ms, eq 50fps
#define TRANSITION_DURATION 500 // in milliseconds

void lerpColors(CRGB* buf1, CRGB* buf2, CRGB* dest, float t, uint16_t size) {
  for (int i = 0; i < size; i++) { // iterate over leds
    for (int j = 0; j < 3; j++) { // iterate over color channels r, g, b
      uint8_t color1 = buf1[i][j];
      uint8_t color2 = buf2[i][j];
      dest[i][j] = (uint8_t) (color1 * (1-t) + color2 * t);
    }
  }
}

struct QueuedAnimation {
  Animation* anim;
  uint32_t duration;
  uint32_t start;
  bool infinite;

  static QueuedAnimation makeInfinite(Animation* animptr) {
    return { animptr, 0, 0, true };
  }

  static QueuedAnimation makeDuration(Animation* animptr, uint32_t ms) {
    return { animptr, ms, 0, false };
  }
};

struct AnimationManager {
  uint32_t lastUpdateTime = 0;
  uint32_t transitionStart = 0;
  vector<QueuedAnimation> animationQueue {};

  CRGB mainBuffer[LED_COUNT];
  CRGB secondaryBuffer[LED_COUNT];

  AnimationManager(Animation &initialAnimation) {
    animationQueue.push_back(
      QueuedAnimation::makeInfinite(initialAnimation.clone()));
  }

  void setupFastLED() {
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(mainBuffer, LED_COUNT);
  }

  void update() {
    uint32_t time = millis();
    if (time - lastUpdateTime < UPDATE_PERIOD || lastUpdateTime > time) return;
    lastUpdateTime = time;

    if (animationQueue.empty()) return; // This should never happen

    QueuedAnimation current = animationQueue[0];
    current.anim->render(time, mainBuffer, LED_COUNT);

    if (animationQueue.size() > 1) {
      QueuedAnimation next = animationQueue[1];

      // If no transition is happening AND current animation can hand over
      if (transitionStart == 0
            && (current.infinite || time - current.start >= current.duration)) {
        transitionStart = time;
        next.start = time;
      }

      if (transitionStart != 0) { // if transition is happening
        uint32_t transitionElapsed = time - transitionStart;

        // if transition reached its end
        if (transitionElapsed > TRANSITION_DURATION) {
          delete current.anim;
          animationQueue.erase(animationQueue.begin());
          transitionStart = 0;
          animationQueue[0].anim->render(time, mainBuffer, LED_COUNT);
        } else { // during transition
          next.anim->render(time, secondaryBuffer, LED_COUNT);
          
          float progress = 
            ((float)transitionElapsed) / ((float)TRANSITION_DURATION);
          lerpColors(
            mainBuffer, secondaryBuffer, mainBuffer, 
            progress, LED_COUNT
          );
        }
      }
    }

    FastLED.show();
  }

  void queueAnimation(Animation &animation) {
    if (animationQueue.size() >= 3) {
      delete animationQueue.back().anim;
      animationQueue.pop_back();
    }
    
    animationQueue.push_back(
      QueuedAnimation::makeInfinite(animation.clone()));
  }

};

#endif
