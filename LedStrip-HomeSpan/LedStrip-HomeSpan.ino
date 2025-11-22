#include <HomeSpan.h>

#include "DEV_Ledstrip.h"
#include "AnimationManager.h"
#include "StateMachine.h"

RainbowLedstrip* rainbowService;
ColorLedstrip* colorService;

Animations::StaticColor defaultAnimation { CRGB::Black };
AnimationManager animationManager { defaultAnimation };
StateMachine stateMachine { animationManager };

void homeSpanStatusCallback(HS_STATUS status) {
    stateMachine.setHomeSpanStatus(status);
}

void setup() {
    // Serial.begin(115200);

    homeSpan.setStatusPin(2);
    homeSpan.setControlPin(5, PushButton::TRIGGER_ON_LOW);
    homeSpan.setStatusCallback(homeSpanStatusCallback);
    homeSpan.begin(Category::Lighting, "WS2812B Ledstrip");

    new SpanAccessory();
        new Service::AccessoryInformation();
            new Characteristic::Identify();

        colorService = new ColorLedstrip(stateMachine);
        rainbowService = new RainbowLedstrip(stateMachine);

    animationManager.setupFastLED();
}

void loop() {
    homeSpan.poll();
    animationManager.update();
}
