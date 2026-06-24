#include "TurboSystem.hpp"

#define BUTTON_FUNCTION 23
#define BUTTTON_NEXT 33
#define BUTTON_PREV 25

StateManager deviceState;

int lastState_NEXT = HIGH;
int lastState_PREV = HIGH;
int lastState_FUNC = HIGH;

int currentStateNEXT;
int currentStatePREV;
int currentStateFUNC;

void setup() {
    Serial.begin(115200);

    pinMode(BUTTON_FUNCTION, INPUT_PULLUP);
    pinMode(BUTTTON_NEXT, INPUT_PULLUP);
    pinMode(BUTTON_PREV, INPUT_PULLUP);
}

void loop() {
    currentStateNEXT = digitalRead(BUTTTON_NEXT);
    currentStatePREV = digitalRead(BUTTON_PREV);
    currentStateFUNC = digitalRead(BUTTON_FUNCTION);

    if (lastState_NEXT == HIGH && currentStateNEXT == LOW) {
        Serial.println("Przycisk NEXT wcisniety");
    }

    if (lastState_PREV == HIGH && currentStatePREV == LOW) {
        Serial.println("Przycisk PREV wcisniety");
    }

    if (lastState_FUNC == HIGH && currentStateFUNC == LOW) {
        Serial.println("Switching screens");
        deviceState.set_mode(DeviceMode::SLEEP);
    }

    lastState_NEXT = currentStateNEXT;
    lastState_PREV = currentStatePREV;
    lastState_FUNC = currentStateFUNC;

    delay(10);
}
