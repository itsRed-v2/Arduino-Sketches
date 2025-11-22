#ifndef ANIMATION_MANAGER_H
#define ANIMATION_MANAGER_H

#include <FastLED.h>
#include <vector>

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

void adjustForLightPerception(CRGB* buffer, uint16_t led_count) {
    for (int i = 0; i < led_count; i++) {
        for (int channel = 0; channel < 3; channel++) {
            float value = buffer[i][channel];
            float adjusted = (value / 255) * (value / 255) * 255;
            buffer[i][channel] = (uint8_t) adjusted;
        }
    }
}

struct QueuedAnimation {
    Animation* anim;
    uint32_t duration;
    uint32_t start;
    bool infinite;

    QueuedAnimation(const Animation &anim):
            anim(anim.clone()), duration(0), start(0), infinite(true) {}

    QueuedAnimation(const Animation &anim, uint32_t ms):
            anim(anim.clone()), duration(ms), start(0), infinite(false) {}

    QueuedAnimation(const QueuedAnimation &other):
            anim(other.anim->clone()), duration(other.duration),
            start(other.start), infinite(other.infinite) {}

    QueuedAnimation &operator=(const QueuedAnimation &other) {
        if (this != &other) {
            duration = other.duration;
            start = other.start;
            infinite = other.infinite;
            delete anim;
            anim = other.anim->clone();
        }
        return *this;
    }

    ~QueuedAnimation() {
        delete anim;
    }
};

struct AnimationManager {
    uint32_t lastUpdateTime = 0;
    uint32_t transitionStart = 0;
    std::vector<QueuedAnimation> animationQueue {};

    CRGB mainBuffer[LED_COUNT];
    CRGB secondaryBuffer[LED_COUNT];

    AnimationManager(Animation &initialAnimation) {
        animationQueue.push_back(QueuedAnimation(initialAnimation));
        animationQueue.back().start = millis();
    }

    void setupFastLED() {
        FastLED.addLeds<NEOPIXEL, DATA_PIN>(mainBuffer, LED_COUNT);
    }

    void update() {
        uint32_t time = millis();
        if (time - lastUpdateTime < UPDATE_PERIOD || lastUpdateTime > time) return;
        lastUpdateTime = time;

        // TODO: per-animation time origin

        if (animationQueue.empty()) {
            return; // This should never happen
        } else if (animationQueue.size() == 1) { // If there is only one animation left in the queue
            QueuedAnimation &current = animationQueue[0];
            current.anim->render(time - current.start, mainBuffer, LED_COUNT);

            if (!current.infinite && time - current.start >= current.duration) {
                Animations::StaticColor black { CRGB::Black };
                animationQueue.push_back(QueuedAnimation(black));
            }
        } else if (transitionStart == 0) { // More than one animation in queue and no transition is happening
            QueuedAnimation &current = animationQueue[0];
            current.anim->render(time - current.start, mainBuffer, LED_COUNT);

            // If current animation is done, start transition in next frame
            if (current.infinite || time - current.start >= current.duration) {
                QueuedAnimation &next = animationQueue[1];
                transitionStart = time;
                next.start = time;
            }
        } else { // if transition is happening
            uint32_t transitionElapsed = time - transitionStart;

            // if transition reached its end
            if (transitionElapsed > TRANSITION_DURATION) {
                animationQueue.erase(animationQueue.begin());
                transitionStart = 0;
                QueuedAnimation &newCurrent = animationQueue[0];
                newCurrent.anim->render(time - newCurrent.start, mainBuffer, LED_COUNT);
            } else { // during transition
                QueuedAnimation &current = animationQueue[0];
                QueuedAnimation &next = animationQueue[1];
                current.anim->render(time - current.start, mainBuffer, LED_COUNT);
                next.anim->render(time - next.start, secondaryBuffer, LED_COUNT);

                float progress = ((float)transitionElapsed) / ((float)TRANSITION_DURATION);
                lerpColors(mainBuffer, secondaryBuffer, mainBuffer, progress, LED_COUNT);
            }
        }

        adjustForLightPerception(mainBuffer, LED_COUNT);
        FastLED.show();
    }

    void queueAnimation(const Animation &animation) {
        if (animationQueue.size() >= 3 && animationQueue.back().infinite) {
            animationQueue.pop_back();
        }

        animationQueue.push_back(QueuedAnimation(animation));
    }

    void queueAnimationDuration(const Animation &animation, uint32_t duration) {
        animationQueue.push_back(QueuedAnimation(animation, duration));
    }

};

#endif
