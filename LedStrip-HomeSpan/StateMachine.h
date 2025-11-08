#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <FastLED.h>
#include "AnimationManager.h"

class StateMachine {
private:
    bool rainbowEnabled = false;
    bool staticColorEnabled = false;
    CRGB staticColor{0, 0, 0};

    AnimationManager &animationManager;

public:
    StateMachine(AnimationManager &manager): animationManager(manager) { }

    void enableRainbow(bool enable) {
        rainbowEnabled = enable;
        onUpdate();
    }

    void setStaticColor(CRGB color) {
        staticColorEnabled = (color == CRGB{0, 0, 0});
        staticColor = color;
        onUpdate();
    }

private:
    void onUpdate() {
        if (rainbowEnabled) {
            Animations::Rainbow anim {};
            animationManager.queueAnimation(anim);
        } else if (staticColorEnabled) {
            Animations::StaticColor anim {staticColor};
            animationManager.queueAnimation(anim);
        } else {
            Animations::StaticColor anim { CRGB{0, 0, 0} };
            animationManager.queueAnimation(anim);
        }
    }
};

#endif
