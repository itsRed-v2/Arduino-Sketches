#include <HomeSpan.h>

#define LOCKED 1
#define UNLOCKED 0

struct DEV_Door : Service::LockMechanism {
    int relayPin;
    SpanCharacteristic *currentState;
    SpanCharacteristic *targetState;

    DEV_Door(int relayPin) : Service::LockMechanism() {
        currentState = new Characteristic::LockCurrentState(LOCKED);
        targetState = new Characteristic::LockTargetState(LOCKED);
        this->relayPin = relayPin;
        pinMode(relayPin, OUTPUT);
    }

    bool update() {
        int newTarget = targetState->getNewVal<int>();

        if (newTarget == UNLOCKED) {
            digitalWrite(this->relayPin, HIGH);
            delay(500);
            digitalWrite(this->relayPin, LOW);
        }
        currentState->setVal(newTarget, true);
        return true;
    }

    void loop() {
        if (currentState->getVal<int>() == 0 && currentState->timeVal() > 5000) {
            targetState->setVal(1, true);
            currentState->setVal(1, true);
        }
    }
};

void setup() {
    // Comment this line in production. HomeSpan hangs if serial is
    // enabled but the ESP32 is not connected to a computer.
    // Serial.begin(115200);

    homeSpan.setStatusPin(2);
    homeSpan.setControlPin(12, PushButton::TRIGGER_ON_LOW);
    homeSpan.begin(Category::Doors, "Door switch");

    new SpanAccessory();
        new Service::AccessoryInformation();
            new Characteristic::Identify();
        new DEV_Door(4);
}

void loop() {
    homeSpan.poll();
}
