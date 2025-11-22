#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <FastLED.h>
#include <HomeSpan.h>
#include "AnimationManager.h"

class StateMachine {
private:
    bool rainbowEnabled = false;
    bool staticColorEnabled = false;
    CRGB staticColor{0, 0, 0};
    HS_STATUS homeSpanStatus = HS_WIFI_NEEDED;

    AnimationManager &animationManager;

public:
    StateMachine(AnimationManager &manager): animationManager(manager) { }

    void enableRainbow(bool enable) {
        rainbowEnabled = enable;
        updateState();
    }

    void setStaticColor(CRGB color) {
        staticColorEnabled = (color != CRGB{0, 0, 0});
        staticColor = color;
        updateState();
    }

    void setHomeSpanStatus(HS_STATUS status) {
        homeSpanStatus = status;
        if (homeSpanStatus == HS_PAIRED) {
            Animations::ConnectionSuccess anim {};
            animationManager.queueAnimationDuration(anim, 2000);
        }
        updateState();
    }

private:
    void updateState() {
        if (homeSpanStatus == HS_WIFI_CONNECTING) {
            Animations::Connecting anim {};
            animationManager.queueAnimation(anim);
        }
        else if (rainbowEnabled) {
            Animations::Rainbow anim {};
            animationManager.queueAnimation(anim);
        } else if (staticColorEnabled) {
            Animations::StaticColor anim {staticColor};
            animationManager.queueAnimation(anim);
        } else {
            Animations::StaticColor anim { CRGB::Black };
            animationManager.queueAnimation(anim);
        }
    }
};

#endif
